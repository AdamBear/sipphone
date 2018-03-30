#include "talklistenertypes.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/src/basesession.h"
#include "talk/talklistener/include/modtalklistener.h"

SignalParams::SignalParams(int iSessID /*= INVALID_SESSION_ID*/, const yl::string& str /*= yl::string()*/)
    : iSessionID(iSessID), iCallID(-1), eSignal(TALK_SIG_UNKOWN), strData(str), uReply(0)
{
    if (iSessID != INVALID_SESSION_ID)
    {
        iCallID = talklogic_GetCallIdBySessionID(iSessID);
    }

    wParam.iValue = lParam.iValue = tParam.iValue = 0;
    //memset(&wParam, 0, sizeof(wParam) * 3);
}

SignalParams::SignalParams(const CBaseSession* p, const yl::string& str /*= yl::string()*/)
    : iSessionID(INVALID_SESSION_ID), iCallID(-1), eSignal(TALK_SIG_UNKOWN), strData(str), uReply(0)
{
    if (p)
    {
        iSessionID = p->GetSessionID();
        /* warring:不可直接通过Session指针获取CallID */
        iCallID = talklogic_GetCallIdBySessionID(iSessionID);
    }

    wParam.iValue = lParam.iValue = tParam.iValue = 0;
}

SignalParams::SignalParams(const CCallInfo* p, const yl::string& str /*= yl::string()*/)
    : iSessionID(-1), iCallID(-1), eSignal(TALK_SIG_UNKOWN), strData(str), uReply(0)
{
    if (p)
    {
        iCallID = p->GetCallID();
        /* warring:不可直接通过callinfo指针获取Session */
        iSessionID = talklogic_GetSessionIdByCallID(iCallID, false);
    }

    wParam.iValue = lParam.iValue = tParam.iValue = 0;
}


CListenInfo::CListenInfo(int iSessionID)
    : m_iSessionID(iSessionID), bLastHold(false), bSeizeFailed(false),
      m_bProcessSeizeLine(false), m_bBLASeizeLineSucced(true),
      iBindSubSession(INVALID_SESSION_ID), iLastFocusSession(INVALID_SESSION_ID),
      m_bSaveCallLog(true), m_bReadySendedS2SDmtf(true),
      m_BlaProxy(iSessionID), m_NoAnwFwdProxy(iSessionID), m_RecordProxy(iSessionID),
      m_AutoAnserProxy(iSessionID), m_AutoRefuseProxy(iSessionID)
{
    m_bInPreCallPark = false;
    m_bInPreGroupCallPark = false;
    m_bShowCallInfo = false;
    m_bTransferFailed = false;
    m_bDPickupDial = false;
    m_eAAChannel = CHANNEL_AUTO;
#if IF_BUG_27033 || IF_BUG_30965
    m_bPagingDial = false;
    m_bIntercomDial = false;
    m_bUnParkDial = false;
#endif
    // 初始需要进入BLAconnect routine,占线成功后就不需要
    m_bEnterBLARoutine = true;
    m_bCallPushFailed = false;
    m_bEstablish = false;
    m_strParkFiled.clear();
    m_tBornTime = 0L;
    m_tEstblishTime = 0L;
}

void CListenInfo::SendInfoSig(int iMask, bool bFlag)
{
    SignalParams sigp(m_iSessionID);
    sigp.wParam.iValue = iMask;
    sigp.lParam.bValue = bFlag;
    modtklsn_emit(TALK_SIG_SESSION_INFO_CHANGED, sigp);
}

void CListenInfo::SetPark(bool bPark)
{
    if (bPark != m_bInPreCallPark)
    {
        m_bInPreCallPark = bPark;
        SendInfoSig(TALK_CALL_INFO_PARK, bPark);
    }
}

void CListenInfo::ReSetPark()
{
    m_bInPreCallPark = false;
}

void CListenInfo::SetGPark(bool bGPark)
{
    if (bGPark != m_bInPreGroupCallPark)
    {
        m_bInPreGroupCallPark = bGPark;
        SendInfoSig(TALK_CALL_INFO_GPARK, bGPark);
    }
}

void CListenInfo::ReSetGPark()
{
    m_bInPreGroupCallPark = false;
}

void CListenInfo::SetCCInfo(bool bShow)
{
    if (bShow != m_bShowCallInfo)
    {
        m_bShowCallInfo = bShow;
        SendInfoSig(TALK_CALL_INFO_CCINFO, bShow);
    }
}

void CListenInfo::SetTransFailed(bool bFailed)
{
    if (bFailed != m_bTransferFailed)
    {
        m_bTransferFailed = bFailed;
        SendInfoSig(TALK_CALL_INFO_TRNSFER, bFailed);
    }
}

#if IF_FEATURE_EXTEND_LOG
ListenerName GetSignalName(int type)
{
    switch (type)
    {
        CASE_VALE_RET(TALK_SIG_SESSION_PRE_ROUTINE_CHANGED);
        CASE_VALE_RET(TALK_SIG_SESSION_ROUTINE_CHANGED);
        CASE_VALE_RET(TALK_SIG_SESSION_FARHOLD_CHANGE);
        CASE_VALE_RET(TALK_SIG_SESSION_FARMUTE_CHANGE);
        CASE_VALE_RET(TALK_SIG_SESSION_HOLD_CHANGE);
        CASE_VALE_RET(TALK_SIG_SESSION_HELD_CHANGE);
        CASE_VALE_RET(TALK_SIG_SESSION_IPVPPARAM_CHANGE);
        CASE_VALE_RET(TALK_SIG_SESSION_SIPPARAM_CHANGE);
        CASE_VALE_RET(TALK_SIG_SESSION_ACCOUNT_CHANGE);
        CASE_VALE_RET(TALK_SIG_SESSION_CALLID_CHANGE);
        CASE_VALE_RET(TALK_SIG_SESSION_REMOTE_INFO_CHANGE);
        CASE_VALE_RET(TALK_SIG_SESSION_CALLID_IMAGE_CHANGE);
        CASE_VALE_RET(TALK_SIG_SESSION_FOCUS_CHANGE);
        CASE_VALE_RET(TALK_SIG_GET_DATA_TO_UI);
        CASE_VALE_RET(TALK_SIG_SESSION_TEXT_CHANGE);
        CASE_VALE_RET(TALK_SIG_ACTION_EVENT);
        CASE_VALE_RET(TALK_SIG_SESSION_INFO_CHANGED);
        CASE_VALE_RET(TALK_SIG_SESSION_CONTRACT_CHANGED);
        CASE_VALE_RET(TALK_SIG_SESSION_HOST_CHANGE);
        CASE_VALE_RET(TALK_SIG_SESSION_HIDE_CHANGE);
        CASE_VALE_RET(TALK_SIG_SUB_SESSION_ACTIVED);
        CASE_VALE_RET(TALK_SIG_RECV_PAGE);
        CASE_VALE_RET(TALK_SIG_SESSION_PRE_CREATE);
        CASE_VALE_RET(TALK_SIG_SESSION_AFTER_CREATE);
        CASE_VALE_RET(TALK_SIG_SESSION_PRE_DELETE);
        CASE_VALE_RET(TALK_SIG_SESSION_AFTER_DELETE);
        CASE_VALE_RET(TALK_SIG_SESSION_TRANSFER);
        CASE_VALE_RET(TALK_SIG_SESSION_DTMF_FINISH);
        CASE_VALE_RET(TALK_SIG_SESSION_SAVE_LOG);
        CASE_VALE_RET(TALK_SIG_DIAL_CALLOUT);
        CASE_VALE_RET(TALK_SIG_BEFORE_CALLOUT);
        CASE_VALE_RET(TALK_SIG_PROCESS_CALLOUT);
        CASE_VALE_RET(TALK_SIG_SAVE_NO_TALK_LOG);
        CASE_VALE_RET(TALK_SIG_SESSION_RETRIEVEPARK);
        CASE_VALE_RET(TALK_SIG_SESSION_ACTIVED);
        CASE_VALE_RET(TALK_SIG_IDLE_TO_TALK);
        CASE_VALE_RET(TALK_SIG_TALK_TO_IDLE);
        CASE_VALE_RET(TALK_SIG_SESSION_REJECT);
        CASE_VALE_RET(TALK_SIG_SESSION_PRE_START_TALK);
        CASE_VALE_RET(TALK_SIG_SESSION_AFTER_START_TALK);
        CASE_VALE_RET(TALK_SIG_SESSION_STOP_TALK);
        CASE_VALE_RET(TALK_SIG_SESSION_MUTE_CHANGE);
        CASE_VALE_RET(TALK_SIG_SESSION_ANSWER);
        CASE_VALE_RET(TALK_SIG_SESSION_HANDUP);
        CASE_VALE_RET(TALK_SIG_SESSION_ADD_TO_NET_CONF);
        CASE_VALE_RET(TALK_SIG_SESSION_FWD);
        CASE_VALE_RET(TALK_SIG_SESSION_SIP_MESSAGE);
        CASE_VALE_RET(TALK_SIG_CALL_SEND_REMOTE_DTMF);
        CASE_VALE_RET(TALK_SIG_SESSION_REFUSE_CALL);
        CASE_VALE_RET(TALK_SIG_SESSION_MAKE_CALL);
        CASE_VALE_RET(TALK_SIG_SESSION_PLAY_BUSY_TONE);
        CASE_VALE_RET(TALK_SIG_SESSION_DIAL_TONE);
        CASE_VALE_RET(TALK_SIG_SESSION_TRY_ENTER_FINISH);
        CASE_VALE_RET(TALK_SIG_SESSION_RESET_TALK);
        CASE_VALE_RET(TALK_SIG_SESSION_PLAY_RINGING);
        CASE_VALE_RET(TALK_SIG_SESSION_PLAY_CONF_TONE);
        CASE_VALE_RET(TALK_SIG_SESSION_ATTENDED_TRANSFER_FAIL);
        CASE_VALE_RET(TALK_SIG_SESSION_TALK_STATE_CHANGE);
        CASE_VALE_RET(TALK_FLT_SESSION_INCOME);
        CASE_VALE_RET(TALK_FLT_SESSION_REFUSE);
        CASE_VALE_RET(TALK_FLT_SESSION_CREATE);
        CASE_VALE_RET(TALK_FLT_SESSION_DELETE);
        CASE_VALE_RET(TALK_FLT_ENTER_DIAL);
    default:
        return NULL;
    }
}
#endif

ListenerName GetETLMSGName(int iIndex)
{
    switch (iIndex)
    {
        CASE_VALE_RET(SIP_CALL_ESTABLISH);
        CASE_VALE_RET(SIP_CALL_RINGBACK);
        CASE_VALE_RET(SIP_CALL_FAIL);
        CASE_VALE_RET(SIP_CALL_FORWARDED_BY_REMOTE);
        CASE_VALE_RET(SIP_CALL_TRANSFERED_BY_REMOTE);
        CASE_VALE_RET(SIP_CALL_REPLACED_BY_REMOTE);
        CASE_VALE_RET(SIP_CALL_INCOMING);
        CASE_VALE_RET(SIP_CALL_RELEASED);
        CASE_VALE_RET(SIP_CALL_UPDATE_INFO);
        CASE_VALE_RET(SIP_CALL_RECV_FRAME_SYNC);
        CASE_VALE_RET(SIP_SUB_UPDATE_BROADSOFT_SCA_SEIZE_RESULT);
        CASE_VALE_RET(SIP_SUB_UPDATE_BROADSOFT_SCA_SEIZE_RESULT2);
        CASE_VALE_RET(SIP_CALL_TRANSFERED_RESULT);
        CASE_VALE_RET(SIP_CALL_CONFERENCE_NOTIFY);
        CASE_VALE_RET(SIP_REG_UPDATE_SIP_SERVER);
        CASE_VALE_RET(SIP_CALL_INFORMATION_UPDATE);
        CASE_VALE_RET(SIP_CALL_CODEC_LIST);
        CASE_VALE_RET(SIP_SUB_DRAFT_BLA_SEIZE_LINE_RESULT);
        CASE_VALE_RET(SIP_SUB_DRAFT_BLA_UNSEIZE_LINE_RESULT);
        CASE_VALE_RET(SIP_CALL_REINVITE_RECIEVE);
        CASE_VALE_RET(SIP_CALL_REINVITE_FAIL);
        CASE_VALE_RET(SIP_CALL_EXT_INFO);
        CASE_VALE_RET(SIP_MSG_EXT_VIDEO_STATE);
        CASE_VALE_RET(SIP_CALL_BW_CALL_RECORD_STATUS);

        CASE_VALE_RET(DSK_MSG_REDIRECTCALL);
        CASE_VALE_RET(DSK_MSG_HANG_UP);
        CASE_VALE_RET(DSK_MSG_MAKE_CALL);

        CASE_VALE_RET(IPVP_MSG_AUDIO_SRTP_REINVITE);
        CASE_VALE_RET(IPVP_MSG_AUDIO_FIRST_RTP);
        CASE_VALE_RET(IPVP_MSG_AUDIO_DEAD);

        CASE_VALE_RET(SIP_CMD_CALL_ACCEPT);
        CASE_VALE_RET(SIP_CMD_CALL_HOLD);
        CASE_VALE_RET(SIP_CMD_CALL_RESUME);
        CASE_VALE_RET(SIP_CMD_CALL_RESPONSE);
        CASE_VALE_RET(SIP_CMD_CALL_REJECT);
        CASE_VALE_RET(SIP_CMD_CALL_HANGUP);
        CASE_VALE_RET(SIP_CMD_CALL_DTMF);
        CASE_VALE_RET(SIP_CMD_CALL_FORWARD);
        CASE_VALE_RET(SIP_CMD_CALL_TRANSFER);

#ifdef IF_FEATURE_PSTN
        CASE_VALE_RET(PSTN_MSG_INCOMING_CALL);
        CASE_VALE_RET(PSTN_MSG_INCOMING_CID);
        CASE_VALE_RET(PSTN_MSG_INCOMING_CANCEL);
        CASE_VALE_RET(PSTN_MSG_DIAL_RESULT);
        CASE_VALE_RET(ACCOUNT_PSTN_CREATED);
        CASE_VALE_RET(ACCOUNT_PSTN_REMOVED);
        CASE_VALE_RET(PSTN_MSG_FLASH_RESULT);
#endif // IF_FEATURE_PSTN
#ifdef IF_BTTALK_UPDATECALLINFO
        CASE_VALE_RET(BLUETOOTH_CONTACT_RELOAD);
        CASE_VALE_RET(BLUETOOTH_CONTACT_DOWNLOAD_RESULT);
#endif
    default:
        break;
    }

    return NULL;
}

