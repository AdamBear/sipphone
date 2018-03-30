#include "cdsskeylightlogic.h"
#include "dsskey/include/moddsskey.h"
#include "exp/include/modexp.h"
#include "lamp/ledlamp/include/modledlamp.h"

#include "dsskey/src/dsskeycommonfunc.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "dsskeyuilogic.h"

#define DSSK_SLOWER_FLASH_TIMER     1000
#define DSSK_SLOW_FLASH_TIMER       500
#define DSSK_FAST_FLASH_TIMER       300
#define DSSK_FASTER_FLASH_TIMER     200


// 回调函数，遍历消息函数表
static LRESULT OnTimerMessage(msgObject & objMessage)
{
    BOOL bHandled = FALSE;
    switch (objMessage.message)
    {
    case TM_TIMER:
        {
            bHandled = g_pDssKeyLightLogic->OnTimer(objMessage.wParam);
        }
        break;
    default:
        break;
    }
    return bHandled;
}

CDssKeyLightLogic * CDssKeyLightLogic::s_pInst = NULL;

CDssKeyLightLogic * CDssKeyLightLogic::GetInstance()
{
    if (NULL == s_pInst)
    {
        s_pInst = new CDssKeyLightLogic();
    }

    return s_pInst;
}

void CDssKeyLightLogic::DestoryInstance()
{
    if (NULL != s_pInst)
    {
        delete s_pInst;
        s_pInst = NULL;
    }
}

CDssKeyLightLogic::CDssKeyLightLogic(void)
{
    m_lightStatus = 0;
    m_bForbidden = FALSE;
    m_iMaxNumber = 0;
    m_iMaxNumPerPage = 0;
    m_arryFlashTimer[0].iDelay = DSSK_FASTER_FLASH_TIMER;
    m_arryFlashTimer[0].iRunNum = 0;

    m_arryFlashTimer[1].iDelay = DSSK_FAST_FLASH_TIMER;
    m_arryFlashTimer[1].iRunNum = 0;

    m_arryFlashTimer[2].iDelay = DSSK_SLOW_FLASH_TIMER;
    m_arryFlashTimer[2].iRunNum = 0;

    m_arryFlashTimer[3].iDelay = DSSK_SLOWER_FLASH_TIMER;
    m_arryFlashTimer[3].iRunNum = 0;

    SingleMsgReg(TM_TIMER, OnTimerMessage);

}

CDssKeyLightLogic::~CDssKeyLightLogic(void)
{
    YLList<LIGHT_INFO *>::iterator itr = m_listLightInfo.begin();
    YLList<LIGHT_INFO *>::iterator itrEnd = m_listLightInfo.end();
    for (; itr != itrEnd; itr++)
    {
        if (*itr != NULL)
        {
            delete *itr;
            *itr = NULL;
        }
    }

    m_listLightInfo.clear();
}

//开灯
void CDssKeyLightLogic::UpdateLight(int iDsskeyID, DSS_KEY_COLOR_TYPE eColorType,
                                    DSS_KEY_FLASH_TYPE eFlashType, int iFlashuration /*= -1*/)
{
    DSSKEYUI_INFO("CDssKeyLightLogic::UpdateLight %d", iDsskeyID);
    if (iDsskeyID < 0)
    {
        return;
    }

    if (LED_COLOR_OFF == eColorType)
    {
        TurnOffLight(iDsskeyID);
        return;
    }

#ifdef _MAP_TEST_
    LightStatusMapIterator iter = LightStatusFind(iDsskeyID);
#else
    LightStatusMapIterator iter = m_lightStatusMap.find(iDsskeyID);
#endif
    if (iter != m_lightStatusMap.end())
    {
#ifdef _MAP_TEST_
        LightData & Tmp = *iter;
        if (Tmp.m_lightMeta.m_eColorType != eColorType)
        {
            // 先关灯
            TurnOffLightByIndex(iDsskeyID);
        }

        (*iter).m_lightMeta.m_eColorType = eColorType;
        (*iter).m_lightMeta.m_eFlashType = eFlashType;
#else
        if (iter->second.m_eColorType != eColorType)
        {
            // 先关灯
            TurnOffLightByIndex(iDsskeyID);
        }

        iter->second.m_eColorType = eColorType;
        iter->second.m_eFlashType = eFlashType;
#endif
    }
    else
    {
        SLightMeta lm;
        lm.m_iDssKeyID = iDsskeyID;
        lm.m_eColorType = eColorType;
        lm.m_eFlashType = eFlashType;
        lm.m_iPriority = 0;
#ifdef _MAP_TEST_
        LightData LightTmp;
        LightTmp.m_nLightKey = iDsskeyID;
        LightTmp.m_lightMeta = lm;
        m_lightStatusMap.push_back(LightTmp);
#else
        m_lightStatusMap[iDsskeyID] = lm;
#endif

    }

    if (iFlashuration > 0)
    {
        /*timerSetThreadTimer((UINT)&m_listTimerID[iDsskeyID], iFlashuration);*/

    }
    updateLightStatus(iDsskeyID, eColorType, eFlashType);
}

//关灯
void CDssKeyLightLogic::TurnOffLight(int iDsskeyID)
{
#ifdef _MAP_TEST_
    LightStatusMapIterator iter = LightStatusFind(iDsskeyID);
#else
    LightStatusMapIterator iter = m_lightStatusMap.find(iDsskeyID);
#endif

    if (iter != m_lightStatusMap.end())
    {
        //删除灯状态记录
        m_lightStatusMap.erase(iter);
    }

    TurnOffLightByIndex(iDsskeyID);
}

void CDssKeyLightLogic::TurnOffLightByIndex(int iDsskeyID)
{
    UpdateFlashTimer(iDsskeyID, 0, FALSE);

    DSSKEY_MODULE_TYPE eType = dsskey_GetModuleType(iDsskeyID);
    if (IsEXPModule(eType))
    {
		exp_SetExpLEDStatus(iDsskeyID, LT_OFF);
    }
    else if (DMT_LINEKEY_MODULE == eType)
    {
        if (iDsskeyID > -1
                && (m_lightStatus & 1 << iDsskeyID) != 0)
        {
            m_lightStatus &= ~(1 << iDsskeyID);
            ledlamp_PostDsskeyEvent(iDsskeyID);
        }
    }
}

void CDssKeyLightLogic::UpdateFlashTimer(int iDsskeyID, int iDelay,
        BOOL bFlash)//delay就是flashtime
{
    LIGHT_INFO * pLightInfo = NULL;
    YLList<LIGHT_INFO *>::iterator itr = m_listLightInfo.begin();
    YLList<LIGHT_INFO *>::iterator itrEnd = m_listLightInfo.end();
    for (; itr != itrEnd; itr++)
    {
        if ((*itr)->iLightID == iDsskeyID)//lcm当前dsskey原来就有闪灯的信息
        {
            // 找到旧的灯信息
            pLightInfo = *itr;
            break;
        }
    }

    int iOldTimerIndex = -1;
    int iNewTimerIndex = -1;

    for (int i = 0; i < LIGHT_TIMER_NUM; ++i)//lcm闪动用的定时器
    {
        // 获取旧定时器类型
        if (iDelay ==
                m_arryFlashTimer[i].iDelay)//M_arrayFlashTimer[i]这个定时器和我们需要使用的定时器类型是一样的
        {
            iNewTimerIndex = i;//因此我们可以直接使用，将新定时器的index直接设置为这个
        }
        // 获取新定时器类型
        if (pLightInfo != NULL
                && m_arryFlashTimer[i].iDelay ==
                pLightInfo->iDelay)//这边的是这个dsskey原来就有设置过 m_arryFlashTimer[i]这个定时器的，
        {
            //首先tpLightInfo！=NULL说明这个dsskey上次就由设置，后一个说明，用的就是这个
            iOldTimerIndex = i;
        }
    }

    if (bFlash)// true eColorType == LED_COLOR_RED_GREEN交替颜色闪
    {
        if (iOldTimerIndex != -1)
        {
            // 定时器类型发生变化,旧定时器数量减一
            m_arryFlashTimer[iOldTimerIndex].iRunNum--;
            if (m_arryFlashTimer[iOldTimerIndex].iRunNum == 0)
            {
                timerKillThreadTimer((UINT)&m_arryFlashTimer[iOldTimerIndex]);
            }
        }

        if (iNewTimerIndex < 0)
        {
            return;
        }

        m_arryFlashTimer[iNewTimerIndex].iRunNum++;
        if (m_arryFlashTimer[iNewTimerIndex].iRunNum == 1)
        {
            timerSetThreadTimer((UINT)&m_arryFlashTimer[iNewTimerIndex],
                                m_arryFlashTimer[iNewTimerIndex].iDelay);
        }

        if (NULL == pLightInfo)
        {
            pLightInfo = new LIGHT_INFO();
            m_listLightInfo.push_back(pLightInfo);
            pLightInfo->eLightType = LED_COLOR_TYPE_RED;
            pLightInfo->iLightID = iDsskeyID;
            pLightInfo->bFlash = bFlash;
        }

        pLightInfo->iDelay = m_arryFlashTimer[iNewTimerIndex].iDelay;
    }
    else
    {
        if (NULL == pLightInfo
                || itr == itrEnd
                || iOldTimerIndex == -1)
        {
            return;
        }

        // 关闭已经存在的定时器
        m_arryFlashTimer[iOldTimerIndex].iRunNum--;
        if (m_arryFlashTimer[iOldTimerIndex].iRunNum == 0)
        {
            timerKillThreadTimer((UINT)&m_arryFlashTimer[iOldTimerIndex]);
        }

        delete pLightInfo;
        pLightInfo = NULL;
        m_listLightInfo.erase(itr);
    }

    DSSKEYUI_INFO("listLightInfo.size=%d", m_listLightInfo.size());
}

//更新灯的状态
void CDssKeyLightLogic::updateLightStatus(int iDsskeyID, DSS_KEY_COLOR_TYPE eColorType,
        DSS_KEY_FLASH_TYPE eFlashType)
{
    DSSKEYUI_INFO("CDssKeyLightLogic::updateLightStatus:iDsskeyID:%d eColorType:%d eFlashType:%d\n",
                  iDsskeyID, eColorType, eFlashType);
    if (iDsskeyID < 0)
    {
        return;
    }
    int iFlashTime = 0;

    //闪灯类型
    switch (eFlashType)
    {
    case REFRESH_TYPE_SLOW_FLASH://慢闪
        iFlashTime = DSSK_SLOWER_FLASH_TIMER;//1000ms
        break;

    case REFRESH_TYPE_FLASH://闪动500ms
        iFlashTime = DSSK_SLOW_FLASH_TIMER;
        break;

    case REFRESH_TYPE_FAST_FLASH://快闪 300ms
        iFlashTime = DSSK_FAST_FLASH_TIMER;
        break;

    case REFRESH_TYPE_FASTER_FLASH://快快闪 200ms
        iFlashTime = DSSK_FASTER_FLASH_TIMER;
        break;
    default:
        iFlashTime = 0;
        break;
    }

    if (eColorType == LED_COLOR_RED_GREEN)//交替闪？ //更新闪烁使用的定时器
    {
        UpdateFlashTimer(iDsskeyID, iFlashTime, TRUE);
        return;
    }
    else
    {
        UpdateFlashTimer(iDsskeyID, iFlashTime, FALSE);
    }

    LightCtrl(iDsskeyID, eColorType, iFlashTime);
}

void CDssKeyLightLogic::LightCtrl(int iDsskeyID, DSS_KEY_COLOR_TYPE eColorType, int iDelay,
                                  bool bFlash/* = false*/)
{
    bool bGreen = false;
    bool bRed = false;
    LIGHT_Type lType = LT_OFF;
    //灯的颜色
    switch (eColorType)
    {
    case LED_COLOR_TYPE_GREEN:
        bGreen = true;
        lType = LT_GREEN_ON;
        break;
    case LED_COLOR_TYPE_RED:
        bRed = true;
        lType = LT_RED_ON;
        break;
    case LED_COLOR_TYPE_ORANGE:
        bGreen = true;
        bRed = true;
        lType = LT_RED_GREEN_ON;
        break;
    default:
        bGreen = true;
        break;
    }

    DSSKEY_MODULE_TYPE eType = dsskey_GetModuleType(iDsskeyID);
    if (IsEXPModule(eType))
    {
		exp_SetExpLEDStatus(iDsskeyID, lType);
    }
    else if (DMT_LINEKEY_MODULE == eType)
    {
        m_lightStatus |= (1 << iDsskeyID);
        ledlamp_PostDsskeyEvent(iDsskeyID, iDelay, bFlash ? 0 : iDelay, bGreen, bRed);
    }
}

#ifdef _MAP_TEST_
CDssKeyLightLogic::LightStatusMapIterator CDssKeyLightLogic::LightStatusFind(int index)
{
    CDssKeyLightLogic::LightStatusMapIterator iter = m_lightStatusMap.begin();
    while (iter != m_lightStatusMap.end())
    {
        if ((*iter).m_nLightKey == index)
        {
            break;
        }
        iter++;
    }
    return iter;
}
#endif

BOOL CDssKeyLightLogic::OnTimer(UINT uTimerID)
{
    listInt_it it = m_listTimerID.begin();
    for (; it != m_listTimerID.end(); ++it)
    {
        if (uTimerID == (UINT) & (*it))
        {
            timerKillThreadTimer(uTimerID);
            OnStopLightTimer(*it);
            m_listTimerID.erase(it);
            return TRUE;
        }
    }


    int iDelay = -1;
    for (int i = 0; i < LIGHT_TIMER_NUM; ++i)
    {
        if (uTimerID == (UINT) & (m_arryFlashTimer[i]))
        {
            iDelay = m_arryFlashTimer[i].iDelay;
            break;
        }
    }

    if (iDelay != -1
            && m_listLightInfo.size() > 0)
    {
        YLList<LIGHT_INFO *>::iterator itr = m_listLightInfo.begin();
        YLList<LIGHT_INFO *>::iterator itrEnd = m_listLightInfo.end();

        for (; itr != itrEnd; itr++)
        {
            LIGHT_INFO * pLightInfo = *itr;
            if (pLightInfo != NULL
                    && iDelay == pLightInfo->iDelay)
            {
                if (LED_COLOR_TYPE_RED == pLightInfo->eLightType)
                {
                    pLightInfo->eLightType = LED_COLOR_TYPE_GREEN;
                }
                else
                {
                    pLightInfo->eLightType = LED_COLOR_TYPE_RED;
                }

                LightCtrl(pLightInfo->iLightID,
                          pLightInfo->eLightType,
                          pLightInfo->iDelay,
                          pLightInfo->bFlash);
            }
        }
    }

    return FALSE;
}

// 设置每页显示的Dsskey个数
void CDssKeyLightLogic::SetMaxNumPerPage(int iMaxNumPerPage)
{
    if (iMaxNumPerPage < 0)
    {
        return;
    }
    m_iMaxNumPerPage = iMaxNumPerPage;
//  m_listTimerID.clear();
//  BYTE TimerID;
//  for (int i = 0; i < m_iMaxNumPerPage; ++i)
//  {
//      m_listTimerID.push_back(TimerID);
//  }
}

void CDssKeyLightLogic::SetMaxNumber(int iMaxNumber)
{
    if (iMaxNumber < 0)
    {
        return;
    }
    m_iMaxNumber = iMaxNumber;
}

void CDssKeyLightLogic::SetStopLightTimerByDsskeyID(int iDsskeyID, int iStopTime/* = 5000*/)
{
    listInt_it it = m_listTimerID.begin();
    for (; it != m_listTimerID.end(); ++it)
    {
        if (iDsskeyID == *it)
        {
            // 已存在，直接重置定时器
            timerKillThreadTimer((UINT) & (*it));
            timerSetThreadTimer((UINT) & (*it), iStopTime);
            return;
        }
    }
    // 不存在，则添加到容器中
    m_listTimerID.push_back(iDsskeyID);
    int TIDsize = m_listTimerID.size();
    if (TIDsize > 0)
    {
        //  取对应定时值的地址作为定时器ID
        timerSetThreadTimer((UINT) & (m_listTimerID.back()), iStopTime);
    }
}

// 定时关闭灯处理
void CDssKeyLightLogic::OnStopLightTimer(int iDsskeyID)
{
    // 关灯处理
    TurnOffLightByIndex(iDsskeyID);
    // Linekey刷新显示数据
    _DssKeyUILogic.UpdateDsskeyLight(iDsskeyID, LED_COLOR_OFF, REFRESH_TYPE_NO_FLASH);
}

void CDssKeyLightLogic::Forbid(BOOL bForbidden)
{
    // 禁用所有Dsskey灯
    if (bForbidden)
    {
        for (int i = 0; i < m_iMaxNumPerPage; ++i)
        {
            TurnOffLight(i);
        }
    }

    m_bForbidden = bForbidden;
}

