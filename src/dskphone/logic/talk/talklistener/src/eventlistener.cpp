#include "talklistener_inc.h"

// ActionURL中是否将来电作为Idle来判断(客户要求为1)
#define  IF_RINGING_AS_IDLE   1

CEventListener::CEventListener()
    : LSN_CLS_LOG_NAME
    , m_bInIdle(true)
{
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HOLD_CHANGE, CEventListener::OnHold);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HELD_CHANGE, CEventListener::OnHeld);
    REG_SYNC_SIGNAL(TALK_SIG_CALL_SEND_REMOTE_DTMF, CEventListener::OnDTMF);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HANDUP, CEventListener::OnHangUp);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CEventListener::OnRoutineChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_CALLID_CHANGE, CEventListener::OnCallIDChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ANSWER, CEventListener::OnAnwserCall);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_REFUSE_CALL, CEventListener::OnRefuseCall);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_SAVE_LOG, CEventListener::OnSaveLog);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_MAKE_CALL, CEventListener::OnMakeCall);
    REG_SYNC_SIGNAL(TALK_SIG_ACTION_EVENT, CEventListener::OnUIAction);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_TRANSFER, CEventListener::OnTransfer);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_FWD, CEventListener::OnFWD);
    REG_SYNC_SIGNAL(TALK_SIG_TALK_TO_IDLE, CEventListener::OnSessionSizeChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HIDE_CHANGE, CEventListener::OnSessionSizeChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CEventListener::OnSessionExit);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_MUTE_CHANGE, CEventListener::OnMuteChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HOST_CHANGE, CEventListener::OnHostChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_STOP_TALK, CEventListener::OnStopTalk);
    REG_SYNC_SIGNAL(TALK_FLT_SESSION_INCOME, CEventListener::OnCallIncome);
    REG_SYNC_SIGNAL(TALK_SIG_PROCESS_CALLOUT, CEventListener::OnCallout);

    /*消息处理*/
    RegETLMsg(SIP_CALL_EXT_INFO);
    RegETLMsg(SIP_CALL_RELEASED);
    RegETLMsg(SIP_CALL_FAIL);
    RegETLMsg(SIP_CALL_TRANSFERED_RESULT);
    RegETLMsg(SIP_CALL_ESTABLISH);
#ifdef IF_FEATURE_PSTN
    RegETLMsg(PSTN_MSG_DIAL_RESULT);
#endif
    RegETLMsg(SIP_CALL_FORWARDED_BY_REMOTE);
    RegETLMsg(SIP_CALL_UPDATE_INFO);
}

DECL_SIGNAL_HANDLER(CEventListener::OnHold)
{
    CSessionProxy proxy(sigp.iSessionID);
    if (proxy.GetSubSize() > 0)
    {
        return false;
    }

    return SendEvent(sigp.lParam.bValue ? EVENT_HOLD : EVENT_UNHOLD, sigp);
}

DECL_SIGNAL_HANDLER(CEventListener::OnHeld)
{
    return SendEvent(sigp.lParam.bValue ? EVENT_REMOTE_HOLD : EVENT_REMOTE_UNHOLD, sigp);
}

DECL_SIGNAL_HANDLER(CEventListener::OnDTMF)
{
    return SendEvent(EVENT_DTMF, sigp);
}

DECL_SIGNAL_HANDLER(CEventListener::OnHangUp)
{
    /* 只有NoAnw才处理 */
    if (sigp.wParam.iValue != 3)
    {
        return false;
    }
    return SendEvent(EVENT_CALL_REMOTE_NO_ANSWER, sigp);
}

DECL_SIGNAL_HANDLER(CEventListener::OnUIAction)
{
    DataUI2Logic* pDataUI2Logic = (DataUI2Logic*)sigp.wParam.pValue;
    if (pDataUI2Logic == NULL || pDataUI2Logic->eAction != TALK_ACTION_CANCEL)
    {
        return false;
    }

    CSessionProxy proxy(sigp.iSessionID);
    if (proxy.IsInherits(ROUTINE_CONNECTING))
    {
        // 呼出时取消,发送Event
        return SendEvent(EVENT_CANCEL_CALLOUT, sigp);
    }

    return false;
}

DECL_SIGNAL_HANDLER(CEventListener::OnTransfer)
{
    // TransferCall发送Event
    int iRet = SendEvent(EVENT_TRANSFER_CALL, sigp);

    // 同时要发送Transfer类型Event
    if (sigp.lParam.iValue == -1)
    {
        iRet |= SendEvent(EVENT_BLIND_TRANSFER, sigp);
    }
    else
    {
        iRet |= SendEvent(EVENT_ATTENDED_TRANSFER, sigp);
    }

    return iRet;
}

DECL_SIGNAL_HANDLER(CEventListener::OnMuteChange)
{
    return SendEvent(sigp.lParam.bValue ? EVENT_MUTE : EVENT_UNMUTE, sigp);
}

DECL_SIGNAL_HANDLER(CEventListener::OnRoutineChange)
{
    CALL_HANDLER(OnSessionSizeChange);
    SESSION_STATE eSessionState = talklogic_GetSessionStateBySessionID(sigp.iSessionID);

    switch (eSessionState)
    {
    case SESSION_DIALING:
    case SESSION_PREDIAL:
        {
            return SendEvent(EVENT_ENTER_DIAL, sigp);
        }
        break;
    case SESSION_RINGING:
        {
#if IF_BUG_35381
            CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(sigp.iCallID);
            if (pCallInfo != NULL)
            {
                const yl::string& strNum = pCallInfo->m_tRemoteInfo.sSIPName;
                if (strNum.find("anonymous") != yl::string::npos)
                {
                    break;
                }
            }
#endif//IF_BUG_35381
            return SendEvent(EVENT_INCOMING_CALL, sigp);
        }
        break;
    case SESSION_RINGBACK:
    case SESSION_RINGBACK_TRAN:
        {
            return SendEvent(EVENT_RINGBACK, sigp);
        }
        break;
    case SESSION_TALKING:
    case SESSION_TRANSFERRED:
    case SESSION_AOC_TALKING:
        {
#if IF_BUG_30423
            CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(sigp.iCallID);
            if (pCallInfo == NULL)
            {
                // 初始化当前通话对方号码
                m_strSIPName = pCallInfo->m_tRemoteInfo.sSIPName;
            }
#endif
            return SendEvent(EVENT_ESTABLISHED, sigp);
        }
        break;
    default:
        return false;
    }
    return false;
}

DECL_SIGNAL_HANDLER(CEventListener::OnCallIDChange)
{
#if IF_BUG_34413
    SESSION_STATE eState = talklogic_GetSessionStateBySessionID(sigp.iSessionID);
    if (eState == SESSION_TALKING
            || eState == SESSION_TRANSFERRED
            || eState == SESSION_AOC_TALKING
            || eState == SESSION_RINGING);
    {
        // InComing发送Event
        SendEvent(EVENT_INCOMING_CALL, sigp);
    }
#endif
    //上报通话被替换后的callid
    return SendEvent(EVENT_CALL_BY_REPLACED, sigp);
}

DECL_SIGNAL_HANDLER(CEventListener::OnRefuseCall)
{
    int iRet = SendEvent(EVENT_REJECT_INCOMING_CALL, sigp, (CallInUserInfo*)sigp.tParam.pValue);

    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(sigp.iCallID);
    if (pCallInfo == NULL)
    {
        iRet |= SendEvent(EVENT_CALLOUT, sigp);
    }
    return iRet;
}

DECL_SIGNAL_HANDLER(CEventListener::OnAnwserCall)
{
    return SendEvent(EVENT_ANSWER_NEW_IN_CALL, sigp);
}

DECL_SIGNAL_HANDLER(CEventListener::OnMakeCall)
{
    return SendEvent(EVENT_CALLOUT, sigp);
}

DECL_SIGNAL_HANDLER(CEventListener::OnFWD)
{
    return SendEvent(EVENT_FORWARD_INCOMING_CALL, sigp, (CallInUserInfo*)sigp.tParam.pValue);
}

DECL_SIGNAL_HANDLER(CEventListener::OnSessionExit)
{
    CALL_HANDLER(OnSessionSizeChange);
    CALL_HANDLER(OnStopTalk);

    bool bEndCall = sigp.wParam.bValue;//通过信号传过来

    SESSION_STATE eState = talklogic_GetSessionStateBySessionID(sigp.iSessionID);
    bool bEnd = (bEndCall
                 || eState == SESSION_FINISHED
                 || eState == SESSION_TALKING
                 || eState == SESSION_TRANSFERRED
                 || eState == SESSION_AOC_TALKING);

    return SendEvent(bEnd ? EVENT_TERMINATED : EVENT_END_DIAL, sigp);
}

DECL_SIGNAL_HANDLER(CEventListener::OnHostChange)
{
    CSessionProxy sessionProxy(sigp.iSessionID);
    if (sessionProxy)
    {
        return false;
    }

    CSessionProxy hostSessionProxy = sessionProxy.GetHost();
    if (!hostSessionProxy)
    {
        /* 截获创建BeTransfer事件 */
        if (hostSessionProxy.GetSessionState() == SESSION_BETRANSFERRED
                && sessionProxy.GetSessionState() == SESSION_CONNECTING)
        {
            return SendEvent(EVENT_CALL_TRANSFERED_BY_REMOTE, sigp);
        }
        /* 截获创建LocalConf事件 */
        else if (hostSessionProxy.GetSessionState() == SESSION_LOCALCONF)
        {
            CCallInfo* pCurrentCallInfo = talklogic_GetCallInfoByCallId(sigp.iCallID);
            if (pCurrentCallInfo == NULL)
            {
                return false;
            }

            CSessionProxy anotherSessionProxy = hostSessionProxy.GetAnOtherSubSession(sessionProxy);
            if (!anotherSessionProxy)
            {
                return false;
            }

            CCallInfo* pAnotherCallInfo = anotherSessionProxy.GetCallInfo();
            if (pAnotherCallInfo == NULL)
            {
                return false;
            }

            TALK_DBG("SendEvent action[%d] caller[%d][%d]",
                     EVENT_CONFERENCE, pCurrentCallInfo->GetCallID(), pAnotherCallInfo->GetCallID());

            TalkEventData event;
            event.m_wParam = pCurrentCallInfo->GetAccountID();
            event.m_eEventType = EVENT_CONFERENCE;
            event.m_vecCallID.push_back(pCurrentCallInfo->GetCallID());
            event.m_vecCallID.push_back(pAnotherCallInfo->GetCallID());
            SEND_RESULT eRet = event_SendEvent(&event);
            return eRet == SR_SUCCESS;
        }
    }

    return false;
}

DECL_SIGNAL_HANDLER(CEventListener::OnSaveLog)
{
    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(sigp.iCallID);
    if (pCallInfo == NULL)
    {
        return false;
    }

    if (CT_MISSCALL == pCallInfo->m_eCallType)
    {
        return SendEvent(EVENT_MISSED_CALL, sigp);
    }

    return false;
}

DECL_SIGNAL_HANDLER(CEventListener::OnSessionSizeChange)
{
    bool bCurrentIdle = true;
    // 判断当前是否Idle状态
    if (talklogic_GetSessionNum() > 0)
    {
#if IF_RINGING_AS_IDLE
        // 来电界面被认为是Idle状态
        if (!talklogic_IsAllSessionRinging())
#endif
        {
            bCurrentIdle = false;
        }
    }

    // 如果状态未改变则不处理
    if (bCurrentIdle == m_bInIdle)
    {
        return false;
    }

    m_bInIdle = bCurrentIdle;
    return SendEvent(bCurrentIdle ? EVENT_BUSY_TO_IDLE : EVENT_IDLE_TO_BUSY, sigp);
}

bool CEventListener::OnMessage(msgObject& msg)
{
    CSessionProxy sessionProxy(talklogic_GetSessionIdByCallID(msg.lParam, false));
    CCallInfo* pCallInfo = sessionProxy.GetCallInfo();
    if (pCallInfo == NULL
            || !sessionProxy.IsSingle())
    {
        return false;
    }
    SignalParams sigp(pCallInfo);
    sigp.wParam.iValue = pCallInfo->GetAccountID();

    SESSION_STATE eState = sessionProxy.GetSessionState();
    switch (msg.message)
    {
#if IF_BUG_35381
    case SIP_CALL_EXT_INFO:
        {
            if (eState == SESSION_RINGING)
            {
                // InComing发送ActionURL
                return SendEvent(EVENT_INCOMING_CALL, sigp);
            }
        }
        break;
#endif
    case SIP_CALL_RELEASED:
        {
            if (eState == SESSION_CONNECTING
                    || eState == SESSION_CONNECTING_TRAN
                    || eState == SESSION_RINGBACK
                    || eState == SESSION_RINGBACK_TRAN
                    || eState == SESSION_NETWORKCONF_CONNECT
                    || eState == SESSION_RINGING)
            {
                return SendEvent(EVENT_CALL_REMOTE_CANCELED, sigp);
            }
            else if (eState == SESSION_TALKING)
            {
                if (pCallInfo->IsTransferred())
                {
                    // 发送Tran成功的Event
                    return SendEvent(EVENT_TRANSFER_FINISHED, sigp);
                }
            }
        }
        break;
    case SIP_CALL_FAIL:
        {
            sip_reason_info_t* pReasonInfo = (sip_reason_info_t*)msg.GetExtraData();
            if (NULL == pReasonInfo)
            {
                break;
            }

            if (eState == SESSION_CONNECTING
                    || eState == SESSION_CONNECTING_TRAN
                    || eState == SESSION_RINGBACK
                    || eState == SESSION_RINGBACK_TRAN
                    || eState == SESSION_NETWORKCONF_CONNECT)
            {
                // 呼出失败发送url
                if (486 == pReasonInfo->code)
                {
                    // 服务器返回486为对方忙,这时候认为是被那对方拒绝的情况
                    SendEvent(EVENT_REMOTE_BUSY, sigp);
                }

                return SendEvent(EVENT_CALL_FAIL, sigp);
            }
        }
        break;
    case SIP_CALL_TRANSFERED_RESULT:
        {
            sip_reason_info_t* pResult = (sip_reason_info_t*)(msg.GetExtraData());
            if (NULL == pResult || !pCallInfo->IsTransferred())
            {
                break;
            }

            // 0标识Transfer成功
            if (pResult->code == 0)
            {
                // 发送Tran成功的Event
                return SendEvent(EVENT_TRANSFER_FINISHED, sigp);
            }
            else
            {
                // 发送Tran失败的Event
                int iRet = SendEvent(EVENT_TRANSFER_FAILED, sigp);
                iRet |= SendEvent(sessionProxy.IsHide() ? EVENT_BLIND_TRANSFER_FAILED : EVENT_ATTENDED_TRANSFER_FAILED, sigp);
                return iRet;
            }
        }
        break;
    case SIP_CALL_ESTABLISH:
        {
            CListenInfo* priv = GetCallerData(pCallInfo);
            if (priv != NULL)
            {
                priv->m_bEstablish = true;
            }

#if IF_BUG_30423
            if (!Comm_IsHoldMsg(msg)
                    && msg.GetExtraData() != NULL)
            {
                if (m_strSIPName != pCallInfo->m_tRemoteInfo.sSIPName)
                {
                    // 收到第二个invite的时候去重新发个established事件
                    // http://192.168.1.99/Bug.php?BugID=30423
                    m_strSIPName = pCallInfo->m_tRemoteInfo.sSIPName;
                    return SendEvent(EVENT_ESTABLISHED, sigp);
                }
            }
#endif
        }
        break;
#ifdef IF_FEATURE_PSTN
    case PSTN_MSG_DIAL_RESULT:
        {
            TalkErrorDefine* pDialResult = (TalkErrorDefine*)msg.GetExtraData();
            if (NULL == pDialResult || *pDialResult == TALK_ERROR_NONE)
            {
                break;
            }

            // 外拨失败
            return SendEvent(EVENT_CALL_FAIL, sigp);
        }
        break;
#endif
    case SIP_CALL_FORWARDED_BY_REMOTE:
        {
            return SendEvent(EVENT_REMOTE_FORWARD, sigp);
        }
        break;
    case SIP_CALL_UPDATE_INFO:
        {
            if (eState != SESSION_NETWORKCONF_CONNECT)
            {
                return SendEvent(EVENT_CALL_INFO_UPDATE, sigp);
            }
        }
        break;
    default:
        break;
    }
    return false;
}

DECL_SIGNAL_HANDLER(CEventListener::OnStopTalk)
{
#ifdef IF_FEATURE_DIAGNOSE
    if (priv == NULL || !priv->m_bEstablish)
    {
        return false;
    }

    priv->m_bEstablish = false;
    return SendEvent(EVENT_MEDIA_VOICE_STOP, sigp);
#else
    return false;
#endif
}

DECL_SIGNAL_HANDLER(CEventListener::OnCallIncome)
{
#if IF_FEATURE_TALKINFO_REPORT
    return SendEvent(EVENT_INCOMING_CALLS_RECEIVED, sigp);
#else
    return false;
#endif
}

DECL_SIGNAL_HANDLER(CEventListener::OnCallout)
{
#if IF_FEATURE_TALKINFO_REPORT
    return SendEvent(EVENT_OUTGOING_CALLS_ATTEMPTED, sigp);
#else
    return false;
#endif
}

int CEventListener::SendEvent(int iAction, const SignalParams& sigp, CallInUserInfo* pUserInfo/* = NULL*/)
{
    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(sigp.iCallID);

    TALK_DBG("SendEvent action[%d] acc[%d] caller[%d] session[%d] type[%d] num[%s]",
             iAction, sigp.wParam.iValue, sigp.iCallID, sigp.iSessionID, sigp.lParam, sigp.strData.c_str());

    /* 没有通话信息则调用其它接口 */
    if (pCallInfo == NULL)
    {
        RemoteInfoData event;
        event.m_wParam = sigp.wParam.iValue;

        event.m_strCallID = commonAPI_FormatString("%d", sigp.iCallID);
        event.m_strCallDir = "in";

        if (pUserInfo != NULL)
        {
            event.m_strDisplayName = pUserInfo->strDisplay;
            event.m_strSIPName = pUserInfo->strNumber;
            event.m_strServerName = pUserInfo->strServer;
        }

        event.m_eEventType = (EventType)iAction;
        return event_SendEvent(&event) == SR_SUCCESS;
    }

    TalkEventData event;
    event.m_eEventType = (EventType)iAction;
    event.m_wParam = pCallInfo->GetAccountID();
    event.m_lParam = talklogic_GetSessionNum(false);
    event.m_tParam = sigp.lParam.iValue;

    CListenInfo* priv = GetCallerData(pCallInfo);
    // bugid:7669 技术支持要求增加两项
    if (priv && priv->m_tEstblishTime != 0)
    {
        pCallInfo->SetCallDuration(GetCurrentTick() - priv->m_tEstblishTime);
    }

    // 诊断模块参数
    event.m_tTime = time(NULL);
    event.m_nCallType = pCallInfo->m_eCallType;
    event.m_nDuration = pCallInfo->m_iCallDuration;

    event.m_vecCallID.push_back(sigp.iCallID);
    return event_SendEvent(&event) == SR_SUCCESS;
}
