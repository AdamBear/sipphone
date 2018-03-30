#include "scamenuprocessor.h"
#include "scamenuuibase.h"
#include "talk/broadsoftsca/include/modbroadsoft_sca.h"

#include "contacts/directory/include/directoryenumtypes.h"

#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talkuilogicdelegate.h"
#include "adapterbox/include/modadapterbox.h"

#include "broadsoft/executiveassistant/include/modexecutiveassistant.h"

CSCAMenuProcessor::CSCAMenuProcessor()
    : m_pScaMenuData(NULL)
{
    m_pSCAMenuUI = NULL;
}

CSCAMenuProcessor::~CSCAMenuProcessor()
{
    StopTime();

    m_pSCAMenuUI = NULL;
}

//构造完成后 绑定了UI后调用
void CSCAMenuProcessor::Init()
{
    m_pScaMenuData = NULL;

    m_vectorSoftkey.clear();
}

//释放前解除UI绑定前调用
void CSCAMenuProcessor::Uninit()
{
    StopTime();

    m_pScaMenuData = NULL;
}

//设置绑定的UI
void CSCAMenuProcessor::SetBindUI(CSCAMenuUIBase * pSCAMenuUI)
{
    m_pSCAMenuUI = pSCAMenuUI;
}

//获取绑定的UI
CSCAMenuUIBase * CSCAMenuProcessor::GetBindUI()
{
    return m_pSCAMenuUI;
}

//获取 scamenu 数据
CSCAMenuUIData * CSCAMenuProcessor::GetSCAMenuUIData()
{
    return &m_objSCAMenuUIData;

}

//设置数据
void CSCAMenuProcessor::SetData(void * pData)
{
#if TALKUI_DEBUG_TEST
    static ScaMenuData stScaMenuData;

    stScaMenuData.iAccount = 0;
    stScaMenuData.nReturnTime = 1000 * 30;

    stScaMenuData.pScaInfoList = new ListSCAInfo();

    SCA_Info * pInfo = new SCA_Info();

    pInfo->iIndex = 0;
    pInfo->strDisplay = "test1";
    pInfo->strURI = "";
    pInfo->eStatus = CN_BLA_STATUS_IDLE;
    stScaMenuData.pScaInfoList->push_back(pInfo);

    pInfo->iIndex = 1;
    pInfo->strDisplay = "test2";
    pInfo->strURI = "";
    pInfo->eStatus = CN_BLA_STATUS_HELD;
    stScaMenuData.pScaInfoList->push_back(pInfo);

    pInfo->iIndex = 2;
    pInfo->strDisplay = "test3test3test3test3test3test3test3";
    pInfo->strURI = "";
    pInfo->eStatus = CN_BLA_STATUS_HELD;
    stScaMenuData.pScaInfoList->push_back(pInfo);

    pInfo->iIndex = 3;
    pInfo->strDisplay = "test2";
    pInfo->strURI = "";
    pInfo->eStatus = CN_BLA_STATUS_HELD;
    stScaMenuData.pScaInfoList->push_back(pInfo);

    pInfo->iIndex = 4;
    pInfo->strDisplay = "test2";
    pInfo->strURI = "";
    pInfo->eStatus = CN_BLA_STATUS_HELD;
    stScaMenuData.pScaInfoList->push_back(pInfo);

    pInfo->iIndex = 5;
    pInfo->strDisplay = "test2";
    pInfo->strURI = "";
    pInfo->eStatus = CN_BLA_STATUS_HELD;
    stScaMenuData.pScaInfoList->push_back(pInfo);

    pInfo->iIndex = 6;
    pInfo->strDisplay = "test2";
    pInfo->strURI = "";
    pInfo->eStatus = CN_BLA_STATUS_HELD;
    stScaMenuData.pScaInfoList->push_back(pInfo);
    pData = &stScaMenuData;

    pInfo->iIndex = 7;
    pInfo->strDisplay = "1232123212321232123212321232123212321232123212321";
    pInfo->strURI = "";
    pInfo->eStatus = CN_BLA_STATUS_HELD;
    stScaMenuData.pScaInfoList->push_back(pInfo);
    pData = &stScaMenuData;
#endif



    ScaMenuData * pMenuData = (ScaMenuData *)pData;

    // 保存数据指针
    //这个一定先保存 无论指针是否为空  因为为空可能是逻辑层已经把数据清除了  此时想清UI的数据
    m_pScaMenuData = pMenuData;

    TALKUI_INFO("CSCAMenuUI::SetData");

    if (NULL == pData)
    {
        TALKUI_INFO("CSCAMenuUI: Unexpected null pointer !!");
        return;
    }

    // 赋值
    if (pMenuData->pScaInfoList == NULL)
    {
        return;
    }

    // 从数据中获取显示菜单内容
    if (!SetSCAMenuData(*pMenuData->pScaInfoList))
    {
        TALKUI_WARN("Failed to get SCA menu data !");
        return;
    }

    //设置数据后 启动定时器
    StartTime();
}


//获取softkey数据信息
void CSCAMenuProcessor::GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    if (NULL == m_pSCAMenuUI)
    {
        return;
    }

    m_vectorSoftkey.clear();

    //先插入 4个空的softkey
    m_vectorSoftkey.push_back(ST_EMPTY);
    m_vectorSoftkey.push_back(ST_EMPTY);
    m_vectorSoftkey.push_back(ST_EMPTY);
    m_vectorSoftkey.push_back(ST_EMPTY);


    // 这里可以由logic提供一个接口
    SCA_Info * pInfo = GetInfoByID(m_pSCAMenuUI->GetFocusSCAMenuItemIndex());
    if (NULL == pInfo)
    {
        return;
    }

    m_vectorSoftkey[0] = ST_CANCEL;

    //
    DataLogic2UI objData;
    talklogic_GetDataByAccountBlaIndex(GetBindAccountID(), pInfo->iIndex, objData);
    SESSION_STATE eSessionState = talklogic_GetSessionStateBySessionID(objData.iSessionID);

    if (SESSION_UNKNOWN != eSessionState)
    {
        m_vectorSoftkey[1] = ST_EMPTY;


        if (SESSION_RINGING == eSessionState)
        {
            m_vectorSoftkey[2] = ST_ANSWER;
            m_vectorSoftkey[3] = ST_REJECT;
        }
        else if (objData.iSessionID != talklogic_GetFocusedSessionID())
        {
            m_vectorSoftkey[2] = ST_EMPTY;
            m_vectorSoftkey[3] = ST_SWITCH;

        }
        else
        {
            m_vectorSoftkey[2] = ST_EMPTY;
            m_vectorSoftkey[3] = ST_NEWCALL;
        }
    }
    else
    {
        if (configParser_GetCfgItemIntValue(kszAccountCallPullEnable, GetBindAccountID()) != 1)
        {
            m_vectorSoftkey[1] = ST_EMPTY;
        }
        else
        {
            m_vectorSoftkey[1] = ST_CALLPULL;
        }

        bool bEnabelBarginIn = configParser_GetCfgItemIntValue(kszAccountBargeInEnable,
                               GetBindAccountID()) != 0;

        switch (pInfo->eStatus)
        {
        case CN_BLA_STATUS_ALERTING:
            {
                m_vectorSoftkey[1] = ST_EMPTY;
                m_vectorSoftkey[2] = ST_EMPTY;
                m_vectorSoftkey[3] = ST_NEWCALL;
            }
            break;
        case CN_BLA_STATUS_BRIDGE_ACITVE:
            {
                m_vectorSoftkey[2] = ST_NEWCALL;
                m_vectorSoftkey[3] = ST_BARGEIN;
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
                int iAccount = m_pScaMenuData ? m_pScaMenuData->iAccount : -1;
                if (BWExeAssis_IsSilentMonitorEnable(iAccount))
                {
                    m_vectorSoftkey[2] = ST_SILENTBARGEIN;
                }
#endif
            }
            break;
        case CN_BLA_STATUS_ACTIVE:
            {
                m_vectorSoftkey[2] = ST_NEWCALL;

                if (bEnabelBarginIn)
                {
                    m_vectorSoftkey[3] = ST_BARGEIN;
                }
                else
                {
                    m_vectorSoftkey[3] = ST_EMPTY;
                }
            }
            break;
        case CN_BLA_STATUS_HELD:
            {
                m_vectorSoftkey[2] = ST_NEWCALL;


                if (!bsSCA_GetDsskeySessionByStatus(GetBindAccountID(), CN_BLA_STATUS_HELD))
                {
                    m_vectorSoftkey[3] = ST_SCAMENU_RETRIEVE;
                }
                else
                {
                    m_vectorSoftkey[3] = ST_RESUME;
                }

            }
            break;
        case CN_BLA_STATUS_BRIDGE_HELD:
            {
                m_vectorSoftkey[2] = ST_NEWCALL;

                if (!bsSCA_GetDsskeySessionByStatus(GetBindAccountID(), CN_BLA_STATUS_BRIDGE_HELD))
                {
                    if (bEnabelBarginIn)
                    {
                        m_vectorSoftkey[3] = ST_BARGEIN;
                    }
                    else
                    {
                        m_vectorSoftkey[3] = ST_EMPTY;
                    }
                }
                else
                {
                    m_vectorSoftkey[3] = ST_RESUME;

                }
            }
            break;
        default:
            break;
        }
    }

    //YLVector直接赋值会异常 这边使用 一个个插入
    for (int j = 0; j < m_vectorSoftkey.size(); j++)
    {
        VecSoftkey.push_back(m_vectorSoftkey[j]);
    }

//  VecSoftkey = m_vectorSoftkey;
}

//softKey 按键处理
bool CSCAMenuProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    bool bHandled = true;

    switch (eSoftkey)
    {
    case ST_CANCEL:
        {
            bHandled = true;
        }
        break;
    case ST_ANSWER:
        {
            OnAnswerPress();
        }
        break;
    case ST_REJECT:
        {
            OnRejectPress();
        }
        break;
    case ST_SWITCH:
        {
            OnSwitchPress();
        }
        break;
    case ST_NEWCALL:
        {
            OnNewCallPress();
        }
        break;
    case ST_BARGEIN:
        {
            OnBargeInPress();
        }
        break;
    case ST_RESUME:
        {
            OnResumePress();
        }
        break;
    case ST_CALLPULL:
        {
            OnCallPullPress();
        }
        break;
    case ST_SCAMENU_RETRIEVE:
        {
            OnRetrievePress();
        }
        break;
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    case ST_SILENTBARGEIN:
        {
            OnSilentBarginPress();
        }
        break;
#endif
    default:
        {
            bHandled = false;
        }
        break;

    }

    // 如果处理了，要取消界面
    if (bHandled)
    {
        OnCancel();
    }

    return bHandled;
}

//获取title
yl::string CSCAMenuProcessor::GetTitle()
{
    return _UILOGIC_LANG_TRAN(TRID_SHARED_CALLS);
}

// 获取当前焦点对应的账号分线信息
SCA_Info * CSCAMenuProcessor::GetInfoByID(UINT uIndex)
{
    if (NULL == m_pScaMenuData)
    {
        TALKUI_INFO("Warning! SCA Menu Data Pointer is NULL!");
        return NULL;
    }

    if (-1 == uIndex)
    {
        TALKUI_INFO("Warning! SCA Menu No Focus Data!");
        return NULL;
    }

    ListSCAInfo * pScaInfoList = m_pScaMenuData->pScaInfoList;
    if (pScaInfoList != NULL
            && uIndex < (UINT)pScaInfoList->size())
    {
        ListSCAInfo::iterator it = pScaInfoList->begin();
        ListSCAInfo::iterator itEnd = pScaInfoList->end();
        for (UINT uTemp = 0; it != itEnd; uTemp++, ++it)
        {
            // Private Hold状态无法接起,故不显示在界面
            SCA_Info * pInfo = (SCA_Info *)(*it);
            if (NULL == pInfo
                    || CN_BLA_STATUS_HELD_PRIVATE == pInfo->eStatus)
            {
                uTemp--;
                continue;
            }

            if (uTemp == uIndex)
            {
                return (SCA_Info *)(*it);
            }
        }
    }

    return NULL;
}

// 根据SCA数据，获取SCA显示内容
// | refSCAList | [in] SCA菜单数据
// 返回是否获取成功
bool CSCAMenuProcessor::SetSCAMenuData(ListSCAInfo & refSCAList)
{
    if (refSCAList.size() <= 0)
    {
        return false;
    }

    //先把保存中的数据全部清除
    m_objSCAMenuUIData.vectorSCAMenuItemData.clear();

    yl::string strNameByStatus = "";
    yl::string strDisplayName = "";

    ListSCAInfo::iterator it = refSCAList.begin();
    ListSCAInfo::iterator itEnd = refSCAList.end();
    for (; it != itEnd; ++it)
    {
        // Private Hold状态无法接起,故不显示在界面
        SCA_Info * pInfo = (SCA_Info *)(*it);
        if (NULL == pInfo
                || CN_BLA_STATUS_HELD_PRIVATE == pInfo->eStatus)
        {

            continue;
        }

        strNameByStatus = GetNameByStatus(pInfo->eStatus);
        strDisplayName = GetDisplayName(pInfo);

        CSCAMenuItemData objSCAMenuItemData;
        objSCAMenuItemData.strStatus = strNameByStatus;
        objSCAMenuItemData.strDisplayName = strDisplayName;

        //放入队列中
        m_objSCAMenuUIData.vectorSCAMenuItemData.push_back(objSCAMenuItemData);
    }


    return true;
}

// 根据SCA状态获取状态显示名
yl::string CSCAMenuProcessor::GetNameByStatus(BLF_STATUS_CODE eStatus)
{
    switch (eStatus)
    {
    case CN_BLA_STATUS_ALERTING:
        return _UILOGIC_LANG_TRAN(TRID_SCA_ALERTING);
    case CN_BLA_STATUS_ACTIVE:
        return _UILOGIC_LANG_TRAN(TRID_SCA_ACTIVE);
    case CN_BLA_STATUS_HELD:
        return _UILOGIC_LANG_TRAN(TRID_SCA_HELD);
    case CN_BLA_STATUS_HELD_PRIVATE:
        return _UILOGIC_LANG_TRAN(TRID_SCA_HELD_PRIVATE);
    case CN_BLA_STATUS_BRIDGE_ACITVE:
        return _UILOGIC_LANG_TRAN(TRID_SCA_BRIDGE_ACTIVE);
    case CN_BLA_STATUS_BRIDGE_HELD:
        {
            //  http://10.3.5.199/Bug.php?BugID=69773
            // 监控话机的通话列表应显示SCA Bridge Active
            if (bsSCA_GetDsskeySessionByStatus(GetBindAccountID(), eStatus) == true)
            {
                return _UILOGIC_LANG_TRAN(TRID_SCA_BRIDGE_HELD);
            }
            else
            {
                return _UILOGIC_LANG_TRAN(TRID_SCA_BRIDGE_ACTIVE);
            }
        }
        break;
    default:
        return _UILOGIC_LANG_TRAN(TRID_SCA_UNKNOWN);
    }

    return _UILOGIC_LANG_TRAN(TRID_SCA_UNKNOWN);
}

// 获取SCA账号分线的显示名
yl::string CSCAMenuProcessor::GetDisplayName(SCA_Info * pInfo)
{
    yl::string strRet = _UILOGIC_LANG_TRAN(TRID_UNKNOWN);
    if (pInfo == NULL)
    {
        return strRet;
    }

    strRet = pInfo->strDisplay;
    if (strRet.empty()
            ||  _UILOGIC_LANG_TRAN(TRID_UNKNOWN) == strRet)
    {
        strRet = pInfo->strURI;
    }

#ifndef _WIN32
#warning SCA_CallIn_Info
#endif
    /*
        if ((strRet.empty() ||  _UILOGIC_LANG_TRAN(TRID_UNKNOWN) == strRet)
            && m_pScaMenuData != NULL)
        {
            SCA_CallIn_Info* pCallIn = bsSCA_GetCallInData(m_pScaMenuData->iAccount, pInfo->iIndex);
            if (pCallIn != NULL)
            {
                strRet = pCallIn->strDisplay;
                if (strRet.empty()
                    ||  _UILOGIC_LANG_TRAN(TRID_UNKNOWN) == strRet)
                {
                    strRet = pCallIn->strURI;
                }
            }
        }
    */

    if (strRet.empty())
    {
        strRet =  _UILOGIC_LANG_TRAN(TRID_UNKNOWN);
    }

    return strRet;
}

// 获取关联的账号序号
int CSCAMenuProcessor::GetBindAccountID()
{
    return (m_pScaMenuData == NULL) ? -1 : m_pScaMenuData->iAccount;
}

//处理硬按键按下事件
bool CSCAMenuProcessor::OnKeyPress(int iKeyCode)
{
    //重新启动定时器
    StartTime();

    bool bResult = false;

    switch (iKeyCode)
    {
    case PHONE_KEY_OK:
    case PHONE_KEY_CANCEL:
        {
            // 设置返回值
            BOOL bOk = (PHONE_KEY_OK == iKeyCode);

            // 确认是否是账号绑定来电
            BOOL bRing = FALSE;
            SCA_Info * pInfo = NULL;
            if (NULL != m_pSCAMenuUI)
            {
                pInfo = GetInfoByID(m_pSCAMenuUI->GetFocusSCAMenuItemIndex());
            }

            if (pInfo != NULL
                    && CN_BLA_STATUS_ALERTING == pInfo->eStatus)
            {
                DataLogic2UI objData;
                talklogic_GetDataByAccountBlaIndex(GetBindAccountID(), pInfo->iIndex, objData);
                if (talklogic_GetSessionStateBySessionID(objData.iSessionID) == SESSION_RINGING)
                {
                    bRing = TRUE;
                }
            }

            // 按键处理
            if (bRing)
            {
                if (bOk)
                {
                    OnAnswerPress();
                }
                else
                {
                    OnRejectPress();
                }
                //  OnAction(bOk ? ACTION_DLG_SCAMENU_ANSWER : ACTION_DLG_SCAMENU_REJECT);
            }
            else
            {
                if (bOk)
                {
                    OnSoftkeyPress(m_vectorSoftkey[3]);
                }
                else
                {
                    OnCancel();
                }
                //  OnAction(bOk ? m_SoftkeyData[3].m_strSoftkeyAction.c_str() : ACTION_DLG_SCAMENU_CANCEL);
            }

            return true;
        }
        break;
    default:
        {

        }
        break;
    }

    return bResult;
}

//处理 触摸屏按下事件
void CSCAMenuProcessor::OnMousePress()
{
    //重新启动定时器
    StartTime();
}


//处理new call事件
void CSCAMenuProcessor::OnNewCallPress()
{
    // 转逻辑处理
    talklogic_EnterDialUI("", GetBindAccountID());
}

//处理cancle事件
void CSCAMenuProcessor::OnCancel()
{
    //要先停止定时器
    StopTime();

    AdapterBox_ExitSCAMenu();
    //talkui_ExitSCAMenu();
}

//处理Answer事件
void CSCAMenuProcessor::OnAnswerPress()
{
    if (NULL == m_pSCAMenuUI)
    {
        return;
    }

    SCA_Info * pInfo = GetInfoByID(m_pSCAMenuUI->GetFocusSCAMenuItemIndex());
    if (NULL == pInfo)
    {
        TALKUI_WARN("CSCAMenuUI:: Can't get SCA Info !");
        return;
    }

    // 转逻辑处理
    DataLogic2UI objData;
    talklogic_GetDataByAccountBlaIndex(GetBindAccountID(), pInfo->iIndex, objData);

    talklogic_UIEvent(objData.iSessionID, TALK_ACTION_ANSWER);
}

//处理Reject事件
void CSCAMenuProcessor::OnRejectPress()
{
    if (NULL == m_pSCAMenuUI)
    {
        return;
    }

    SCA_Info * pInfo = GetInfoByID(m_pSCAMenuUI->GetFocusSCAMenuItemIndex());
    if (NULL == pInfo)
    {
        TALKUI_WARN("CSCAMenuUI:: Can't get SCA Info !");
        return;
    }

    // 转逻辑处理
    DataLogic2UI objData;
    talklogic_GetDataByAccountBlaIndex(GetBindAccountID(), pInfo->iIndex, objData);

    talklogic_UIEvent(objData.iSessionID, TALK_ACTION_REJECT);
}

//处理switch事件
void CSCAMenuProcessor::OnSwitchPress()
{
    if (NULL == m_pSCAMenuUI)
    {
        return;
    }

    SCA_Info * pInfo = GetInfoByID(m_pSCAMenuUI->GetFocusSCAMenuItemIndex());
    if (NULL == pInfo)
    {
        TALKUI_WARN("CSCAMenuUI:: Can't get SCA Info !");
        return;
    }

    // 转逻辑处理
    DataLogic2UI objData;
    talklogic_GetDataByAccountBlaIndex(GetBindAccountID(), pInfo->iIndex, objData);
    // 设置为焦点
    talklogic_SetFocusSessionByID(objData.iSessionID);
}

//处理bargein事件
void CSCAMenuProcessor::OnBargeInPress()
{
    OnResumePress();
}

//处理resume事件
void CSCAMenuProcessor::OnResumePress()
{
    if (NULL == m_pSCAMenuUI)
    {
        return;
    }

    SCA_Info * pInfo = GetInfoByID(m_pSCAMenuUI->GetFocusSCAMenuItemIndex());
    if (NULL == pInfo)
    {
        TALKUI_WARN("CSCAMenuUI:: Can't get SCA Info !");
        return;
    }

    talklogic_CallOutBySCALine(GetBindAccountID(), pInfo->iIndex);
}

void CSCAMenuProcessor::OnCallPullPress()
{
    yl::string strCallPullCode = "";
    if (m_pScaMenuData != NULL)
    {
        strCallPullCode = configParser_GetCfgItemStringValue(kszAccountCallPullFeatureCode,
                          m_pScaMenuData->iAccount);
    }

    if (strCallPullCode.empty())
    {
        talklogic_EnterDialUI(strCallPullCode, GetBindAccountID());
    }
    else
    {
        talklogic_CallOutBySCALine(GetBindAccountID(), 0, strCallPullCode);
    }
}

void CSCAMenuProcessor::OnRetrievePress()
{
    OnResumePress();
}

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
void CSCAMenuProcessor::OnSilentBarginPress()
{
    if (NULL == m_pSCAMenuUI)
    {
        return;
    }

    SCA_Info * pInfo = GetInfoByID(m_pSCAMenuUI->GetFocusSCAMenuItemIndex());
    if (NULL == pInfo)
    {
        TALKUI_WARN("CSCAMenuUI:: Can't get SCA Info !");
        return;
    }

    talklogic_CallOutBySCALine(GetBindAccountID(), pInfo->iIndex, "", true);
}
#endif

//定时函数
BOOL CSCAMenuProcessor::OnSCAMenuTimer(msgObject & msg)
{
    //无论如何 先停止定时器
    StopTime();

    OnCancel();

    return TRUE;
}

//启动定时器
void CSCAMenuProcessor::StartTime()
{
    //无论如何 先调用一次 停止定时函数
    StopTime();

    if (NULL != m_pScaMenuData)
    {
        m_objSCAMenuTimer.SetTimer(m_pScaMenuData->nReturnTime, NULL, MK_MSG_ENTRY(this,
                                   CSCAMenuProcessor::OnSCAMenuTimer));
    }
}

//停止定时器
void CSCAMenuProcessor::StopTime()
{
    //如果定时器在跑 先关闭定时器
    if (m_objSCAMenuTimer.IsTimerRuning())
    {
        m_objSCAMenuTimer.KillTimer();
    }
}
