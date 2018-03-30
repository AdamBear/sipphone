#include "actionuri_url_inc.h"

/////////////////////////Action URL/////////////////////////////////////
#if !IF_FEATURE_EVENT_MODULE
LRESULT actUR_TimeOutMegProcess(msgObject & refObj)
{
    g_pActionUrl->OnTimeoutMessage(refObj.wParam);

    return TRUE;
}

// 处理DND状态改变消息
LRESULT actUR_DndMegProcess(msgObject & refObj)
{
    if (refObj.message != DND_MESSAGE_STATUS_CHANGED)
    {
        return FALSE;
    }
    const int iAccount = refObj.wParam;
    ActionUrlData objValue;
    if (acc_IsAccountIDValid(iAccount))
    {
        objValue.m_strSIPURLUser = acc_GetUsername(iAccount);
        objValue.m_strSIPURLHost = acc_GetServerName(iAccount);
        objValue.m_strSIPURL = acc_GetFullName(iAccount);
    }
    bool bDndEnable = *((bool *)refObj.GetExtraData());
    actURL_SendActionURL(bDndEnable ? ACTION_URL_OPEN_DND : ACTION_URL_CLOSE_DND, objValue);
    return TRUE;
}

// 处理FWD状态改变消息
LRESULT actUR_FwdMegProcess(msgObject & refObj)
{
    if (refObj.message != FWD_MESSAGE_STATUS_CHANGED)
    {
        return FALSE;
    }
    FWDDND_INFO("FWD: Send ActionURL for Type [%d] -> [%d])", m_iLastType, GetForwardType());
    const int iAccount = refObj.wParam;
    ActionUrlData objValue;
    if (acc_IsAccountIDValid(iAccount))
    {
        objValue.m_strSIPURLUser = acc_GetUsername(iAccount).c_str();
        objValue.m_strSIPURLHost = acc_GetServerName(iAccount).c_str();
        objValue.m_strSIPURL = acc_GetFullName(iAccount).c_str();
    }
    objValue.m_strFwdLine = commonAPI_FormatString("%d", iAccount);
    FwdMessageStatusData * pFwdInfo = (FwdMessageStatusData *)refObj.GetExtraData();
    objValue.m_strFwdNum = pFwdInfo->szTarget;
    switch (pFwdInfo->eType)
    {
    case FWD_TYPE_ALWAYS:
        {
            actURL_SendActionURL(
                pFwdInfo->bEnabled ? ACTION_URL_OPEN_ALWAYS_FORWARD : ACTION_URL_CLOSE_ALWAYS_FORWARD,
                objValue);
            break;
        }
    case FWD_TYPE_BUSY:
        {
            actURL_SendActionURL(
                pFwdInfo->bEnabled ? ACTION_URl_OPEN_BUSY_FORWARD : ACTION_URL_CLOSE_BUSY_FORWARD,
                objValue);
            break;
        }
    case FWD_TYPE_NO_ANSWER:
        {
            int iRingDuration = configParser_GetConfigInt(kszNoAnswerForwardRingDuration);
            if (iRingDuration <= 0)
            {
                iRingDuration = 6;
            }
            objValue.m_strFwdNoAnswerTime = commonAPI_FormatString("%d", pFwdInfo->iRingDelay * iRingDuration);
            actURL_SendActionURL(
                pFwdInfo->bEnabled ? ACTION_URL_OPEN_NO_ANSWER_FORWARD : ACTION_URL_CLOSE_NO_ANSWER_FORWARD,
                objValue);
            break;
        }
    default:
        break;
    }
    return TRUE;
}
#endif

// 初始化ActionURL功能
void actURL_ActionURLInit()
{
    g_pActionUrl;
#if !IF_FEATURE_EVENT_MODULE
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, actUR_TimeOutMegProcess);
    SingleMsgReg(DND_MESSAGE_STATUS_CHANGED, actUR_DndMegProcess);
    SingleMsgReg(FWD_MESSAGE_STATUS_CHANGED, actUR_FwdMegProcess);
#endif
}

#if !IF_FEATURE_EVENT_MODULE
// 发送某事件的Action URL
void actURL_SendActionURL(ActionUrlType eEventType)
{
    ActionUrlData objValue;
    g_pActionUrl->Send(eEventType, objValue);
}

// 发送某事件的Action URL, 有额外变量
void actURL_SendActionURL(ActionUrlType eEventType, const ActionUrlData & objValue)
{
    g_pActionUrl->Send(eEventType, objValue);
}
#endif

#ifdef IF_SUPPORT_UME
void actURL_SendPhoneInfo()
{
    g_pActionUrl->SendPhoneInfo();
}
#endif

/////////////////////////Action URL/////////////////////////////////////


/////////////////////////Action URI/////////////////////////////////////

// 发送某事件的Action URL, 有额外变量
void actURI_Init()
{
    g_pActionUri;
}

#if IF_BUG_23996
// 判断当前按键是否为ActionUri操控的key
BOOL actURI_IsActionUriKey()
{
    g_pActionUri->IsActionUriKey();
}

// 设置当前按键是否为ActionUri操控的key
void actURI_SetActionUriKey(BOOL bIsActionUriKey)
{
    g_pActionUri->SetActionUriKey(bIsActionUriKey);
}

#endif

#if IF_BUG_27560
bool actURI_IsHideTalkInfo()
{
    return g_pActionUri->GetIsHideInfo();
}

void actURI_SetHideTalkInfo(bool bHideTalkInfo)
{
    g_pActionUri->SetIsHideInfo(bHideTalkInfo);
}
#endif

// 判断是否需要询问用户是否能控制话机
BOOL accessCtr_Process(msgObject & objMsg)
{
    return _AccessManager.ProcessMsg(objMsg);
}

// 判断是否需要询问用户是否能控制话机
BOOL accessCtr_NeedRequest()
{
    return _AccessManager.IsDelayNotify();
}

// 清除延迟到IDLE弹出提示的标志
void accessCtr_PopSuccess()
{
    _AccessManager.ClearDelayNotifyFlag();
    _AccessManager.SetWaitingAuthorization(
        TRUE);//修改在menu等非idle状态下弹出询问框,点确认后再次截图会再次弹出的问题
}

// 设置对话机的控制权
void accessCtr_SetPermissions(BOOL bAllowCtr)
{
    _AccessManager.SetPermissions(bAllowCtr);
}

