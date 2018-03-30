#ifdef IF_FEATURE_RTCPXR
#include "talklistener_inc.h"
#include "talk/vqrtcp/include/modvqrtcp.h"
#include "talk/vqrtcp/src/vqreportmanager.h"

CRTCPListener::CRTCPListener()
    : LSN_CLS_LOG_NAME
{
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_FOCUS_CHANGE, CRTCPListener::OnFocusChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HELD_CHANGE, CRTCPListener::OnHeldChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_AFTER_START_TALK, CRTCPListener::OnStartTalk);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_STOP_TALK,  CRTCPListener::OnStopTalk);
}

DECL_SIGNAL_HANDLER(CRTCPListener::OnFocusChange)
{
    _VQReportManager.OnFocusSessionChange(sigp.iSessionID);
    return true;
}

DECL_SIGNAL_HANDLER(CRTCPListener::OnStartTalk)
{
    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(sigp.iCallID);
    if (pCallInfo == NULL)
    {
        return false;
    }

    _VQReportManager.ProcessStartTalk(pCallInfo->GetCallID(), pCallInfo->GetAccountID(),
                                      (CT_OUTGOING == pCallInfo->m_eCallType), pCallInfo->m_tRemoteInfo.sSIPName);
    return true;
}

DECL_SIGNAL_HANDLER(CRTCPListener::OnStopTalk)
{
    _VQReportManager.ProcessEndCall(sigp.iCallID);
    return true;
}

DECL_SIGNAL_HANDLER(CRTCPListener::OnHeldChange)
{
    _VQReportManager.ProcessHeldTalk(sigp.iSessionID);
    return true;
}
#endif
