#include "talklistener_inc.h"
#include "listencenter.h"
#include "usb_headset/src/usbheadset.h"
#include "talk/talkhelp/src/securityclassification.h"

CCommonListener::CCommonListener()
    : LSN_CLS_LOG_NAME
{
    REG_ASYNC_SIGNAL(TALK_SIG_TALK_TO_IDLE, CCommonListener::OnTalk2IDLE);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CCommonListener::OnSessionDestory);
    REG_SYNC_SIGNAL(TALK_SIG_DIAL_CALLOUT, CCommonListener::OnDialCallout);

    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CCommonListener::OnRoutineChanged);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_MUTE_CHANGE, CCommonListener::OnMuteChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HOLD_CHANGE, CCommonListener::OnMuteChange);

    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_ROUTINE_CHANGED, CCommonListener::OnRoutineChange);
    //blf
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_FOCUS_CHANGE, CCommonListener::OnFoucsChange);
    REG_ASYNC_SIGNAL(TALK_SIG_TALK_TO_IDLE, CCommonListener::OnFoucsChange);

    REG_SYNC_SIGNAL(TALK_FLT_SESSION_INCOME, CCommonListener::OnCallIncome);
    REG_SYNC_SIGNAL(TALK_SIG_RECV_PAGE, CCommonListener::OnPageReceived);

    REG_SYNC_SIGNAL(TALK_SIG_ACTION_EVENT, CCommonListener::OnActionEvent);
}

bool CCommonListener::IsAllowCallOutByKeyguard(const yl::string & strNumber)
{
    // 键盘锁没开，允许呼出
    if (!keyGuard_IsEnable())
    {
        return true;
    }

    // 键盘锁没锁，允许呼出
    if (!keyGuard_IsLock())
    {
        return true;
    }

    // 没锁住AllKey，说明Send可以操作，允许呼出
    if (KG_ALL_KEYS != keyGuard_GetKeyGuardType())
    {
        return true;
    }

    // 紧急码号，允许呼出
    if (Emergency_IsEmergency(strNumber.c_str()))
    {
        return true;
    }

    keyGuard_PopEmergencyTip();
    return false;
}

// 是否拒绝匿名来电
bool CCommonListener::IsRefuseAnonymousCall(const CallInUserInfo& userInfo, int iAccount)
{
    bool bAnonymousCall = userInfo.strDisplay == "Anonymous" || userInfo.strDisplay == "Private number"
                          || userInfo.strNumber == "Anonymous" || userInfo.strNumber == "Private number";

    // 匿名呼叫拒接：由SIP处理转到GUI处理
    return (bAnonymousCall
            && configParser_GetCfgItemIntValue(kszRejectAnonymousCallSwitch, iAccount) == 1);
}

#ifdef IF_FEATURE_EMERGENCY
bool CCommonListener::OnAnswerEmergencyReCallRespond(CCallInfo& callInfo, bool bAccept)
{
    TALK_INFO("OnAnswerEmergencyReCallRespond  accept[%d]", bAccept);
    int iAccount = callInfo.GetAccountID();
    int iCallID = callInfo.GetCallID();
    int iTID = callInfo.m_tSipReinviteInfo.iTID;
    return  modVoip_ReInviteRespose(iAccount, iCallID, iTID, bAccept);
}
#endif //IF_FEATURE_EMERGENCY


DECL_SIGNAL_HANDLER(CCommonListener::OnActionEvent)
{
    DataUI2Logic* pActionData = (DataUI2Logic*)sigp.wParam.pValue;
    if (pActionData == NULL || priv == NULL)
    {
        return false;
    }

    CSessionStateProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    bool bRet = false;
    switch (pActionData->eAction)
    {
    case TALK_ACTION_KEYPRESS:
    case TALK_ACTION_SCREEN_CLICK:
    case TALK_ACTION_EXIT_FAILD_INFO:
        {
            if (proxy.IsTalkSession() && (priv->IsTransFailed()))
            {
                priv->SetTransFailed(false);
                return true;
            }
        }
        break;
    case TALK_ACTION_CANCEL:
        {
            if (proxy.IsTalkSession())
            {
                if ((pActionData->wActionParam == TAT_CHANNEL
                        && pActionData->wActionParam == TAT_NONE)
                        || pActionData->wActionParam == TAT_DSK_EXTERN)// DSK_MSG_HANG_UP发起的
                {
                    // hold时，通道键不能退出
                    if (proxy.IsHold())
                    {
                        SIG_REPLY(false);
                    }
                }
            }
            if (proxy.IsSingle())
            {
                if (priv->IsTransFailed())
                {
                    priv->SetTransFailed(false);
                }
            }
        }
        break;
    default:
        break;
    }
    return true;
}

DECL_SIGNAL_HANDLER(CCommonListener::OnRoutineFlagDataInit)
{
    CSessionStateProxy ssP(sigp.iSessionID);
    ROUTINE_STATE eOldState = (ROUTINE_STATE)sigp.wParam.iValue;
    CCallInfo* pCallInfo = ssP.GetCallInfo();
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }
    if (eOldState != ROUTINE_UNKNOWN)
    {
        //routine 中途发生切换时
        pCallInfo->ResetDialedNumber();
    }

    switch (ssP.GetInheritsRoutine())
    {
    case ROUTINE_DIALING://拨号
        {
            priv->m_bEnterBLARoutine = true;
            priv->m_bDPickupDial = false;
#if IF_BUG_27033 || IF_BUG_30965
            priv->m_bPagingDial = false;
            priv->m_bIntercomDial = false;
            priv->m_bUnParkDial = false;
#endif
            priv->m_bBLASeizeLineSucced = false;
            priv->m_bProcessSeizeLine = false;
        }
        break;
    case ROUTINE_CONNECTING://呼出连接
        {
            priv->m_bProcessSeizeLine = false;
        }
        break;
    case ROUTINE_FINISHED://通话完成
        {
            priv->m_VoiceInfo.m_eToneType = TONE_BUSY;
        }
        break;
    case ROUTINE_RINGING://响铃
        {
            priv->m_eAAChannel = CHANNEL_AUTO;
            priv->m_VoiceInfo.m_strRingPath = "";
            priv->m_VoiceInfo.eCallWaitingTone = TONE_CALLWAIT;
            priv->m_VoiceInfo.m_iRingStatus = TALK_RING_LS_UNKOWN;
            priv->m_bShowCallInfo = false;
        }
        break;
    case ROUTINE_TALKING://通话中
        {
            priv->m_VoiceInfo.eHoldAlertType = VoiceInfo::HAT_NO_ALERT;
            priv->m_VoiceInfo.m_iTranFailedToneTimes = 0;
            priv->ReSetPark();
            priv->ReSetGPark();
            priv->m_VoiceInfo.m_bCallParkFailedAlert = false;
            priv->m_strConfName = "";
        }
        break;
    default:
        return false;
        break;
    }
    return true;
}

DECL_SIGNAL_HANDLER(CCommonListener::OnRoutineChange)
{
    CALL_HANDLER(OnRoutineFlagDataInit);
    ROUTINE_STATE eOldState = (ROUTINE_STATE)sigp.wParam.iValue;
    ROUTINE_STATE eNewState = (ROUTINE_STATE)sigp.lParam.iValue;
    if (eOldState == ROUTINE_RECALL && eNewState == ROUTINE_TALKING)
    {
        //recall 接听情况下
#ifdef IF_FEATURE_EMERGENCY
        CSessionStateProxy ssP(sigp.iSessionID);
        CCallInfo* pCallInfo = ssP.GetCallInfo();
        if (pCallInfo == NULL)
        {
            return false;
        }
        if (ssP.IsEmergencyCalled())
        {
            return OnAnswerEmergencyReCallRespond(*pCallInfo, true);
        }
#endif
    }
    return false;
}

DECL_SIGNAL_HANDLER(CCommonListener::OnTalk2IDLE)
{
    if (talklogic_GetSessionNum() != 0)
    {
        return false;
    }

    // 退出通话查看是否有需要显示的xmlbrowser界面
    xmlbrowser_HideXmlBrowser(false);

    autoset_CheckAutoSetAfterTalk();

    int iSize = ListenCenter.GetPrivDataSize();
    if (iSize != 0)
    {
        TALK_ERR("memory leak for listener private data count[%d]", iSize);
    }

    return true;
}

DECL_SIGNAL_HANDLER(CCommonListener::OnSessionDestory)
{
    CSessionProxy session(sigp.iSessionID);
    if (!session.IsSingle())
    {
        return false;
    }

    CCallInfo* pCallInfo = session.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    if (session.IsInherits(ROUTINE_CONNECTING))
    {
        // http://10.2.1.199/Bug.php?BugID=18661
        // 连接中,退出通话要取消订阅
        CallCompletion_UnsubscribeByCallId(pCallInfo->GetCallID());
    }

#ifdef IF_SUPPORT_USB_AUDIO

    _USBHeadset.SyncState();
#endif

    // 数据清除
    _SecurityClassification.DeleteTalkSecurityLevel(sigp.iSessionID);

    return true;
}

DECL_SIGNAL_HANDLER(CCommonListener::OnRoutineChanged)
{
    CSessionProxy proxy(sigp.iSessionID);
    if (proxy.IsHide())
    {
        return false;
    }

    ROUTINE_STATE eOldRoutine = (ROUTINE_STATE)(sigp.wParam.iValue);
    ROUTINE_STATE eNewRoutine = (ROUTINE_STATE)(sigp.lParam.iValue);

#ifdef IF_FEATURE_GETBALANCE
    // 隐藏的通话不再下载余额
    if (eOldRoutine == ROUTINE_TALKING
            || eNewRoutine == ROUTINE_TALKING)
    {
        acc_SetNeedGetBalanceAccount(proxy.GetAccountID());
    }
#endif

    /* 进拨号、呼出失败之前先停掉blf tone */
    if (proxy.IsInherits(ROUTINE_DIALING)
            || proxy.IsInherits(ROUTINE_TALKING)
            || proxy.IsInherits(ROUTINE_FINISHED))
    {
        // 如果有响铃进入预拨号界面需要取消铃声
        if (blf_IsBlfIdleRing())
        {
            blf_StopBlfIdleRing();
        }
    }

#ifdef IF_SUPPORT_USB_AUDIO
    if (ROUTINE_RINGING == eNewRoutine
            || ROUTINE_RINGING == eOldRoutine
            || Comm_IsRoutineInherits(ROUTINE_DIALING_CLS, eNewRoutine)
            || Comm_IsRoutineInherits(ROUTINE_DIALING_CLS, eOldRoutine))
    {
        _USBHeadset.SyncState();
    }
#endif

    if ((ROUTINE_UNKNOWN == eOldRoutine && eNewRoutine != ROUTINE_AUTOREDIAL)
            || (ROUTINE_AUTOREDIAL == eOldRoutine && eNewRoutine == ROUTINE_UNKNOWN))
    {
        talklogic_ExitSession(ROUTINE_AUTOREDIAL);
        _AutoRedialManager.StopAutoRedial();
    }

    return true;
}

DECL_SIGNAL_HANDLER(CCommonListener::OnFoucsChange)
{
    //设置铃声
    blf_ProcessTalkFoucsChange();
    return true;
}

DECL_SIGNAL_HANDLER(CCommonListener::OnDialCallout)
{
    const yl::string& strNumber = sigp.strData;
    if (!IsAllowCallOutByKeyguard(strNumber))
    {
        keyGuard_PopEmergencyTip();
        SIG_REPLY(true);
    }

    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL || priv == NULL)
    {
        SIG_REPLY(true);
    }

    yl::string strDialedNum;
#if IF_BUG_27033 || IF_BUG_30965
    if (priv->m_bDPickupDial)
    {
        pCallInfo->m_eTalkCallOutType = TCT_PICKUP;
        pCallInfo->m_eDialDsskeyType = DDT_PICKUP;
    }
    else if (priv->m_bPagingDial)
    {
        pCallInfo->m_eTalkCallOutType = TCT_PAGING;
        pCallInfo->m_eDialDsskeyType = DDT_PAGING;
    }
    else if (priv->m_bIntercomDial)
    {
        pCallInfo->m_eTalkCallOutType = TCT_INTERCOM;
        pCallInfo->m_eDialDsskeyType = DDT_INTERCOM;
    }
    else if (priv->m_bUnParkDial)
    {
        pCallInfo->m_eTalkCallOutType = TCT_UNPARK;
        pCallInfo->m_eDialDsskeyType = DDT_UNPARK;
    }

    strDialedNum = strNumber;
#else
    // 如果是DPickup界面，则要进行DPickupCode添加的判断
    if (priv->m_bDPickupDial)
    {
        // 设置Pickup呼出标志
        pCallInfo->m_eTalkCallOutType = TCT_PICKUP;
        // 加Code前缀
        int iAccount = pCallInfo->GetAccountID();
        if (acc_IsAccountIDValid(iAccount))
        {
            GetDPickupCallOutNumber(-1, strDialedNum, strNumber, "", iAccount);
        }
    }
    else
    {
        strDialedNum = strNumber;
    }
#endif

    // callParam.eCallOutType = m_pCallInfo->m_eTalkCallOutType;
#if IF_FEATURE_DIAL_ON_HOLD
    // 呼出则不绑定之前的Session
    if (pCallInfo->m_bDialOnHold)
    {
        // 取消当前Session和sponsor的关系
        proxy.SetTranSponsor();
    }
#endif //IF_FEATURE_DIAL_ON_HOLD

    pCallInfo->SetDialedNumber(strDialedNum);
    return true;
}

DECL_SIGNAL_HANDLER(CCommonListener::OnCallIncome)
{
    if (sigp.lParam.pValue == NULL)
    {
        return false;
    }

    CallInUserInfo& userInfo = *((CallInUserInfo*)sigp.lParam.pValue);
    int iAccountID = sigp.wParam.iValue;

    userInfo.strEmergFwdTarget = configParser_GetConfigInt(kszUseFromNumberMatchEmergency) == 1 ? userInfo.strFromNumber : userInfo.strNumber;

    /* 紧急呼叫中断后续操作 */
    if (Emergency_IsAcceptNewCall(userInfo.strEmergFwdTarget.c_str()))
    {
        return false;
    }
    else if (!acc_IsAccountIDValid(iAccountID))
    {
        TALK_WARN("Call income with invalid lineid(%d), Ignor.", iAccountID);
        SIG_REPLY(404);
    }
    else if (sigp.tParam.iValue == CALL_LINE_SIP)
    {
        int iRetCode = 0;
        // 匿名呼叫拒接：由SIP处理转到GUI处理
        if (IsRefuseAnonymousCall(userInfo, iAccountID))
        {
            iRetCode = configParser_GetConfigInt(kszRejectAnonymousCallCode);
        }
        // 网络异常时来电,拒绝
        else if (!netIsNetworkReady())
        {
            TALK_WARN("Refuse incoming call(network unavailable).");
            //_SIPSender.RefuseCall(iAccountID, iCallID, configParser_GetConfigInt(kszNormalRefuseCode));
            //SIP根据SIP_CALL_INCOMING的返回值来拒绝来电，不用发拒绝的消息给SIP
            iRetCode = configParser_GetConfigInt(kszNormalRefuseCode);
        }
#if IF_FEATURE_LOOPBACK
        // loopback 来电，当前有通话拒接
        else if (userInfo.bIsLoopBackCall
                 && talklogic_GetSessionNum() > 0)
        {
            iRetCode = configParser_GetConfigInt(kszNormalRefuseCode);
        }
#endif
        else
        {
            return false;
        }

        SIG_REPLY(iRetCode);
    }

    return false;
}

DECL_SIGNAL_HANDLER(CCommonListener::OnPageReceived)
{
    // 显示提示框时的处理
    // 如果存在CC的提示框，则将其退出
    idleScreen_DropStatus(PS_STATE_CALL_COMPLETION);

    // 网络异常时来电,拒绝
    if (!netIsNetworkReady())
    {
        TALK_WARN("Refuse incoming call (network unavailable)");
        SIG_REPLY(RCR_REFUSE);
    }

    return false;
}

DECL_SIGNAL_HANDLER(CCommonListener::OnMuteChange)
{
#ifdef IF_SUPPORT_USB_AUDIO
    CSessionProxy proxy(sigp.iSessionID);
    if (!proxy.IsSingle())
    {
        return false;
    }

    _USBHeadset.SyncState();
#endif

    return true;
}
