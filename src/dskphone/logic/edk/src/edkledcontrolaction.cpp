#include "edkledcontrolaction.h"
#include <etlmsghandler/modetlmsghandler.h>
#include "edkactionmanager.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "dsskey/include/moddsskey.h"
#include "dsskey/include/dsskeymsgdefine.h"
#include "autoset/include/autoset_def.h"
#include "blf/include/modblf.h"
#include "../include/edkmsgdefine.h"

#define EDK_LED_CHECK_TIME_INTERVAL 100

CEdkLedControlAction::CEdkLedControlAction(void)
    : CEdkActionBase(EDK_EXCUTELED)
{
    m_mapLedAction.clear();
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, &CEdkLedControlAction::OnTimer);
    etl_RegisterMsgHandle(DSSKEY_CONF_CHANGE, DSSKEY_CONF_CHANGE, &CEdkLedControlAction::OnTimer);
    etl_RegisterMsgHandle(DSSKEY_MODULE_REMOVE, DSSKEY_MODULE_REMOVE, &CEdkLedControlAction::OnTimer);
}

CEdkLedControlAction::~CEdkLedControlAction(void)
{
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, &CEdkLedControlAction::OnTimer);
    etl_UnregisterMsgHandle(DSSKEY_CONF_CHANGE, DSSKEY_CONF_CHANGE, &CEdkLedControlAction::OnTimer);
    etl_UnregisterMsgHandle(DSSKEY_MODULE_REMOVE, DSSKEY_MODULE_REMOVE, &CEdkLedControlAction::OnTimer);
}

bool CEdkLedControlAction::DoAction(int iActionId, edk_action_data_t & tEdkAction, bool & bResult)
{
    LedActionData objLedAction;
    if (ParseLedRule(tEdkAction, objLedAction))
    {
        EDK_INFO("Edk Led DoActon [DsskeyId:%d][%d]", tEdkAction.m_iDsskeyId, objLedAction.m_iDsskeyId);

        // dsskey id小于-1则发消息让外部控制亮灯, 否则EDK去亮灯
        if (IsExternalLightingTypeSupported(objLedAction.m_iDsskeyId))
        {
            // 发送亮灯规则给外部
            LedRuleData * arrLedRuleData = new LedRuleData[objLedAction.m_listLedRule.size()];
            int iIndex = 0;
            for (YLList<LedRuleData>::ListIterator iter = objLedAction.m_listLedRule.begin();
                    iter != objLedAction.m_listLedRule.end();
                    iter ++, iIndex ++)
            {
                arrLedRuleData[iIndex] = (*iter);
            }
            etl_NotifyAppEx(false, EDK_MSG_LIGHTING, LIGHTING_EXP_FLIP, 0,
                            sizeof(LedRuleData)*objLedAction.m_listLedRule.size(), arrLedRuleData);
            delete [] arrLedRuleData;
        }
        else
        {
            bool bRemove = false;

            // 保存数据
            Save2Map(objLedAction);
            LightCtrl(tEdkAction.m_iDsskeyId, bRemove);

            if (bRemove)
            {
                RemoveData(tEdkAction.m_iDsskeyId);
            }
        }
    }
    return true;
}

LRESULT CEdkLedControlAction::OnTimer(msgObject & msg)
{
    return _EdkActionManager.OnMessage(EDK_EXCUTELED, msg) ? 1 : 0;

}

LRESULT CEdkLedControlAction::OnDsskeyAttrChange(msgObject & msg)
{
    return _EdkActionManager.OnMessage(EDK_EXCUTELED, msg) ? 1 : 0;
}

// 获取灯规则数据
bool CEdkLedControlAction::GetLedActionData(UINT iTimerId,
        MAP_ID_TO_LED_ACTION_ITER & iterLedAction)
{
    for (MAP_ID_TO_LED_ACTION_ITER iter = m_mapLedAction.begin();
            iter != m_mapLedAction.end(); ++iter)
    {
        if ((UINT) & (iter->second) == iTimerId)
        {
            iterLedAction = iter;
            return true;
        }
    }
    return false;
}

bool CEdkLedControlAction::OnMessage(msgObject & msg)
{
    if (msg.message == TM_TIMER)
    {
        // 获取灯的规则
//      MAP_ID_TO_LED_ACTION_ITER iterLedAction;
//      if (!GetLedActionData(msg.wParam, iterLedAction))
//      {
//          return false;
//      }
//
//      return LightCtrl(iterLedAction->first);
        ProcessTimerBack(msg.wParam);
    }
    else if (msg.message == DSSKEY_CONF_CHANGE)
    {
        dsskey_change_info_t * pChangeData = NULL;
        int iChangeNum = msg.GetExtraSize() / sizeof(dsskey_change_info_t);
        // 必须是dsskey_change_info_t的整数倍，ACCOUNT_STATUS_CHANGED 会携带一个 bool 类型的 ext，避免混淆
        if (msg.GetExtraData() != NULL && iChangeNum > 0
                && (msg.GetExtraSize() % sizeof(dsskey_change_info_t) == 0))
        {
            pChangeData = (dsskey_change_info_t *)msg.GetExtraData();
            if (NULL != pChangeData)
            {
                for (int i = 0; i < iChangeNum; i++)
                {
                    EDK_INFO("EDK LED dsskey change remove [%d]", pChangeData[i].iDsskeyID);
                    RemoveData(pChangeData[i].iDsskeyID, true);
                }
            }
        }
    }
    else if (msg.message == DSSKEY_MODULE_REMOVE)
    {
        if (msg.GetExtraData() != NULL)
        {
            int * pDsskeyId = (int *)msg.GetExtraData();
            if (NULL != pDsskeyId)
            {
                for (int i = 0; i < msg.GetExtraSize() / sizeof(int); ++i)
                {
                    EDK_INFO("Exp remote EDK LED dsskey remove [%d]", pDsskeyId[i]);
                    // 该模块移除，需要把这些dsskey的灯数据删除，并解绑掉blf的dsskeyid
                    RemoveData(pDsskeyId[i]);
                    //blf_UnBindDsskeyId(pDsskeyId[i]);
                }
            }
        }
    }

    return true;
}

// 保存到map
bool CEdkLedControlAction::Save2Map(LedActionData & objLedAction)
{
    // 判断该dsskey是否已经有在闪烁，如有，则先删除之前的，多种规则，保存数据
    MAP_ID_TO_LED_ACTION_ITER iter = m_mapLedAction.find(objLedAction.m_iDsskeyId);
    if (iter != m_mapLedAction.end())
    {
        LedActionData & objTmp = iter->second;
//      if (objTmp.m_bTimerRunning)
//      {
//          timerKillThreadTimer((UINT)&(iter->second));
//      }

        iter->second = objLedAction;
    }
    else
    {
        m_mapLedAction.insert(objLedAction.m_iDsskeyId, objLedAction);
    }
    return true;
}

// 点灯或亮图标
bool CEdkLedControlAction::LightCtrl(int iDsskeyId, bool & bRemove)
{
    MAP_ID_TO_LED_ACTION_ITER iter = m_mapLedAction.find(iDsskeyId);
    if (iter != m_mapLedAction.end())
    {
        LedActionData & objLedAction = iter->second;

        int iIndex = 0;
        for (LIST_LED_RULE_TIER it = objLedAction.m_listLedRule.begin();
                it != objLedAction.m_listLedRule.end(); ++it, iIndex++)
        {
            LedRuleData & objLedRule = *it;

            // 判断是否需要循环,标志位重置，从第一个开始闪
            if (objLedRule.m_eColorType == COLOR_LOOP)
            {
                objLedAction.ReSet();
                return LightCtrl(iDsskeyId, bRemove);
            }


            // 1、判断是否亮过
            if (objLedRule.m_bFlashed)
            {
                continue;
            }

            // 2、点灯，TODO // 彩屏的需要提供接口
            UpdateDsskey(objLedAction.m_iDsskeyId, objLedRule, objLedAction.m_eFlushType);

            // 记住当前颜色的持续时间
            objLedAction.m_iTimer = objLedRule.m_iDurTime;

            //EDK_INFO("Edk Led Ctrl [DsskeyId:%d] [Color:%d] [Time:%d] [%s]", iDsskeyId, objLedRule.m_eColorType, objLedRule.m_iDurTime, objLedAction.m_strLabel.c_str());

            // 3、标志为已亮过
            objLedRule.m_bFlashed = true;

            // 后续都只要刷新LED灯即可
            objLedAction.m_eFlushType = DSS_KEY_REFRESH_LED;

            if (objLedAction.m_listLedRule.size() == 1)
            {
                break;
            }

            // 不是常亮，需要设置颜色的亮的时间
            if (!objLedAction.m_bTimerRunning)
            {
                objLedAction.m_bTimerRunning = true;
            }

            // 4、设置灯闪烁持续时间定时器
            //EDK_INFO("Edl Led Set Time [id=%p] [time=%d]", &(iter->second), objLedRule.m_iDurTime);
//          if (objLedRule.m_iDurTime != -1)
//          {
//              timerSetThreadTimer((UINT)&(iter->second), objLedRule.m_iDurTime);
//          }

            // 设置定时器
            if (m_iUpdateTimer != EDK_LED_CHECK_TIME_INTERVAL)
            {
                m_iUpdateTimer = EDK_LED_CHECK_TIME_INTERVAL;
                timerSetThreadTimer((UINT)&m_iUpdateTimer, m_iUpdateTimer);
            }

            return true;
        }

        // 不需要无限循环，则删除灯规则
        //RemoveData(objLedAction.m_iDsskeyId);
        // 删除由外面去删除，避免外面在遍历map时调用该函数，导致迭代器出问题
        bRemove = true;
    }

    // 如果都没有数据，取消定时器
    if (m_mapLedAction.size() == 0
            && m_iUpdateTimer == EDK_LED_CHECK_TIME_INTERVAL)
    {
        m_iUpdateTimer = -1;
        timerKillThreadTimer((UINT)&m_iUpdateTimer);
    }
    return true;
}

// 更新灯或dsskey的图标，label
void CEdkLedControlAction::UpdateDsskey(int iDsskeyId, LedRuleData & objLedRule,
                                        DsskeyExpRefresh eFlushType)
{
    switch (objLedRule.m_eColorType)
    {
    case COLOR_NONE:
    case COLOR_RED:
    case COLOR_GREEN:
    case COLOR_ORANGE:
    case COLOR_GREEN_PIC_IN:
    case COLOR_GREEN_PIC_OUT:
    case COLOR_RED_PIC_IN:
    case COLOR_RED_PIC_OUT:
        {
            // 调用彩屏刷新接口
            dsskey_refreshDSSKeyUI(iDsskeyId, eFlushType, false);
        }
        break;
    default:
        break;
    }
}

// 删除数据
bool CEdkLedControlAction::RemoveData(int iDsskeyId, bool bLightOff/* = false*/)
{
    MAP_ID_TO_LED_ACTION_ITER iter = m_mapLedAction.find(iDsskeyId);
    if (iter != m_mapLedAction.end())
    {
        EDK_INFO("Edk Led Remove DsskeyId:%d", iDsskeyId);
        // 比如网页把blflist设置成别的类型，需要把blflist的自定义灯灭掉
        if (bLightOff)
        {
            // 灭灯
            ledlamp_PostDsskeyEvent(iDsskeyId, -1, -1, false, false);
        }

        m_mapLedAction.erase(iter);
        return true;
    }
    return false;
}

// $LEDr1000o1000g1000o$
bool CEdkLedControlAction::ParseLedRule(edk_action_data_t & edkAction, LedActionData & objLedInfo)
{
    // 判断第一个字符要是非数字
    if (edkAction.m_strAction.empty()
            || isdigit(edkAction.m_strAction.at(0)))
    {
        return false;
    }

    objLedInfo.m_iDsskeyId = edkAction.m_iDsskeyId;

    YLVector<yl::string> vecValue;
    yl::string strLedRule = edkAction.m_strAction;

    int iPos = strLedRule.find("&");
    if (iPos != yl::string::npos)
    {
        if (strLedRule.find("&L") == yl::string::npos)
        {
            return false;
        }
        // 取出label值
        objLedInfo.m_strLabel = strLedRule.substr(iPos + 2);

        strLedRule = strLedRule.substr(0, iPos);
        EDK_INFO("Edk ParseLedRule [%s] Label[%s]", strLedRule.c_str(), objLedInfo.m_strLabel.c_str());
    }

    // 第一次全部刷新,因为label从default->custom，custom->default
    objLedInfo.m_eFlushType = DSS_KEY_REFRESH_ALL;

    char * pPos = (char *)strLedRule.c_str();
    char * pPrePos = pPos;

    yl::string strValue = "";

    // other parse method
    char * pPosChar = pPos;
    char * pPosDigit = NULL;
    bool bCompleted = false;
    LedRuleData objRule;
    while (*pPos != '\0')
    {
        // 第一次
        if (pPos == pPrePos)
        {
            pPos++;
            continue;
        }

        bCompleted = false;

        // 当前字符为数字
        if (isdigit(*pPos))
        {
            // 判断前面的字符是非数字
            if (!isdigit(*pPrePos))
            {
                pPosDigit = pPos;

                // 取出灯的颜色
                strValue = strLedRule.substr(pPosChar - strLedRule.c_str(), pPosDigit - pPosChar);
                // 判断该颜色是否正确
                if (!GetLedColorType(strValue, objRule.m_eColorType))
                {
                    return false;
                }
            }
        }
        else
        {
            // 判断前面的字符是数字
            if (isdigit(*pPrePos))
            {
                bCompleted = true;
                pPosChar = pPos;
                // 取出灯亮时间
                strValue = strLedRule.substr(pPosDigit - strLedRule.c_str(), pPosChar - pPosDigit);
                objRule.m_iDurTime = atoi(strValue.c_str());
                // 容错处理
                if (objRule.m_iDurTime > 60000)
                {
                    objRule.m_iDurTime = 60000;
                }
                else if (objRule.m_iDurTime < 300)
                {
                    objRule.m_iDurTime = 300;
                }
            }
        }

        // 位置往后移
        pPrePos = pPos;
        pPos++;

        // 已经完整的规则了，加入到灯规则链表
        if (bCompleted)
        {
            objLedInfo.m_listLedRule.push_back(objRule);
        }
    }

    // 常亮一种颜色，或灭灯 LEDr ,LEDo
    if (objLedInfo.m_listLedRule.size() == 0)
    {
        EDK_INFO("EDK LED One Type [%s]", strLedRule.c_str());

        if (pPosDigit == NULL)
        {
            // 判断该颜色是否正确
            if (!GetLedColorType(strLedRule, objRule.m_eColorType))
            {
                return false;
            }
            objRule.m_iDurTime = -1;
            objLedInfo.m_listLedRule.push_back(objRule);
            return true;
        }
    }

    if (pPosDigit == NULL)
    {
        return false;
    }

    // 最后一个灯规则
    if (isdigit(*pPrePos))
    {
        strValue = strLedRule.substr(pPosDigit - strLedRule.c_str(), pPrePos - pPosDigit + 1);
        objRule.m_iDurTime = atoi(strValue.c_str());
        // 容错处理
        if (objRule.m_iDurTime > 60000)
        {
            objRule.m_iDurTime = 60000;
        }
        else if (objRule.m_iDurTime < 300)
        {
            objRule.m_iDurTime = 300;
        }
        objLedInfo.m_listLedRule.push_back(objRule);

        // 插入一个循环的命令
        objRule.m_eColorType = COLOR_LOOP;
        objLedInfo.m_listLedRule.push_back(objRule);
    }
    else
    {
        strValue = strLedRule.substr(pPosChar - strLedRule.c_str(), pPrePos - pPosChar + 1);
        // 判断是否为最后一个字符
        if (strValue != "o")
        {
            return false;
        }
        objRule.m_eColorType = COLOR_NONE;
        objRule.m_iDurTime = -1;
        objLedInfo.m_listLedRule.push_back(objRule);
    }

    // test for caijz
//  for (LIST_LED_RULE_TIER iter = objLedInfo.m_listLedRule.begin();
//      iter != objLedInfo.m_listLedRule.end(); ++iter)
//  {
//      LedRuleData& objTmp = *iter;
//      EDK_INFO("Edk Led Rule [Color:%d][Timer:%d]", objTmp.m_eColorType, objTmp.m_iDurTime);
//  }
    return true;
}

bool CEdkLedControlAction::GetLedColorType(const yl::string & strColor, COLOR_TYPE & eType)
{
    struct LedColor
    {
        yl::string strValue;
        COLOR_TYPE eColorType;
    };

    static LedColor map_Color[] =
    {
        { "r", COLOR_RED },
        { "g", COLOR_GREEN },
        { "o", COLOR_NONE },
        { "ro", COLOR_RED_PIC_OUT },
        { "ri", COLOR_RED_PIC_IN },
        { "go", COLOR_GREEN_PIC_OUT },
        { "gi", COLOR_GREEN_PIC_IN },
        { "", COLOR_NONE},
    };

    int i = 0;
    while (map_Color[i].strValue != "")
    {
        if (strColor == map_Color[i].strValue)
        {
            eType = map_Color[i].eColorType;
            return true;
        }
        ++i;
    }
    return false;
}

bool CEdkLedControlAction::ParseAction(edk_action_data_t & tEdkAction)
{
    LedActionData objData;
    return ParseLedRule(tEdkAction, objData);
}

// 获取dsskey的显示数据
bool CEdkLedControlAction::GetDsskeyDisDataById(int iDsskeyId,
        EdkCtrlDsskeyDispData & objDisplayData)
{
    DssKey_Type eDskType = dsskey_GetDsskeyType(iDsskeyId);
    if (eDskType != DT_BLF
            && eDskType != DT_BLFLIST)
    {
        return false;
    }

    LedRuleData preLedData;
    MAP_ID_TO_LED_ACTION_ITER iter = m_mapLedAction.find(iDsskeyId);
    if (iter != m_mapLedAction.end())
    {
        LedActionData & objLedAction = iter->second;
        for (LIST_LED_RULE_TIER itr = objLedAction.m_listLedRule.begin();
                itr != objLedAction.m_listLedRule.end(); ++itr)
        {
            LedRuleData & objLedData = *itr;
            // 获取第一个未刷新的状态数据
            if (!objLedData.m_bFlashed)
            {
                if (objLedData.m_eColorType != COLOR_LOOP)
                {
                    objDisplayData.m_eColorType = objLedData.m_eColorType;
                    objDisplayData.m_strLable = objLedAction.m_strLabel;
                }
                else
                {
                    // 如果是循环的，则把上一个颜色状态给出去
                    objDisplayData.m_eColorType = preLedData.m_eColorType;
                    objDisplayData.m_strLable = objLedAction.m_strLabel;
                }
                return true;
            }

            preLedData = objLedData;
        }
    }
    return false;
}

bool CEdkLedControlAction::ProcessTimerBack(UINT uTimerId)
{
    if (uTimerId == (UINT)&m_iUpdateTimer)
    {
        m_iUpdateTimer = -1;
        timerKillThreadTimer((UINT)&m_iUpdateTimer);

        bool bRemove;
        MAP_ID_TO_LED_ACTION_ITER itr = m_mapLedAction.begin();
        while (itr != m_mapLedAction.end())
        {
            LedActionData & objLedAction = itr->second;

            objLedAction.m_iTimer = objLedAction.m_iTimer - EDK_LED_CHECK_TIME_INTERVAL;
            if (objLedAction.m_iTimer <= 0)
            {
                bRemove = false;

                // 检查dsskey类型, 如果不是blf/blflist则表示类型变更过, 不需要再亮灯
                if (dsskey_GetDsskeyType(itr->first) != DT_BLF
                        && dsskey_GetDsskeyType(itr->first) != DT_BLFLIST)
                {
                    bRemove = true;
                }
                else
                {
                    LightCtrl(itr->first, bRemove);
                }

                if (bRemove)
                {
                    EDK_INFO("EDK Led Remove [%d]", itr->first);
                    itr = m_mapLedAction.erase(itr);
                    continue;
                }
            }

            ++itr;
        }

        if (m_mapLedAction.size() != 0)
        {
            m_iUpdateTimer = EDK_LED_CHECK_TIME_INTERVAL;
            timerSetThreadTimer((UINT)&m_iUpdateTimer, m_iUpdateTimer);
        }
        return true;
    }
    return false;
}

// 外部亮灯类型是否支持
bool CEdkLedControlAction::IsExternalLightingTypeSupported(int iType)
{
    bool bExist = false;

    switch (iType)
    {
    case LIGHTING_EXP_FLIP:
        bExist = true;
        break;
    default:
        break;;
    }

    return bExist;
}
