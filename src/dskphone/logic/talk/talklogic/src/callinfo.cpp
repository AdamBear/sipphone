#include "talklogic_inc.h"

CCallInfo::CCallInfo(CSingleSession* pSession, int nCallId/* = -1*/)
    : m_pSession(pSession), m_tVPMParam(m_tIPVPParam), m_pCCInfo(NULL),
      m_pDialData(NULL), m_pCallOutParam(NULL), m_pRingUserInfo(NULL)
{
    m_bIsInConf = false;
    m_bIsSingleExitCof = false;
    m_strConfId = "";
    m_iTransferId = -1;

    if (nCallId == -1)
    {
        m_iCallId = modVoip_NewCallId();
    }
    else
    {
        m_iCallId = nCallId;
    }

    m_eCallType = CT_CALLALL;
    m_eCallLineType = CALL_LINE_SIP;
    m_iSipAccount = INVALID_ACCOUNTID;
    m_iUnregSipAccount = INVALID_ACCOUNTID;
    m_iCallDuration = 0;
    m_eStateOfRoutine = ROUTINE_UNKNOWN;
    memset(&m_tIPVPParam, 0, sizeof(ipvp_speech_t));
    memset(&m_sipChangnels, 0, sizeof(sip_channels_v20_t));
    m_bLocalHold = false;
    m_bRemoteHold = false;
    m_eTalkState = TS_TALK;
    m_bStartTalk = false;
    m_bIsColorRingback = false;
    m_eRecordType = INF_RECORD_NONE;
    m_eURLRecordError = URE_NONE;
    m_iBLAIndex = 0;
    m_bBLASeizeLine = false;
    m_bNeedToSeizeLine = true;
    m_bSilentAnswer = false;
    m_bIsIntercom = false;
    m_bAddToNetworkConf = false;
    m_bConferenceDirect = false;
#if IF_FEATURE_PAGING
    m_pPageParam = NULL;
    m_eStopPageType = SP_REJECT;
#endif
    m_eTalkCallOutType = TCT_NORMAL;
    m_bFinishedError = false;
//   m_bHoldButNotStopTalk = false;
    m_strNumberBeforeEncrypt = "";
    m_bPlay182Tone = false;
    m_bFarMute = false;
    m_strRemotePhoto = "";
    m_strFeatureAccessCodes = "";
    m_strForwardTarget = "";
    m_strRemoteImageURL = "";
    m_bIsAutoAnswerCall = false;
    m_bMissCallInTalk = false;

#if IF_BUG_27033 || IF_BUG_30965
    m_eDialDsskeyType = DDT_NONE;
#endif

#if IF_BUG_22457_Record
    m_eSIPRecordStatus = SIP_RECORD_STATUS_UNKNOWN;
#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    m_eRecordStatus = CALL_RECORD_UNKNOWN;
    m_eRecordResult = CALL_RECORD_NONE;
#endif
#if IF_FEATURE_DIAL_ON_HOLD
    m_bDialOnHold = false;
#endif
#if IF_BUG_30961
    m_bTranByBLF = false;
#endif
#if IF_BUG_27560
    m_bHideInfo = false;
#endif
    m_bNeedBindDsskey = !m_pSession->IsHide();

    // hwz:非ice协商，sip保证第一个通话建立就是hold可用
    // ice协商，默认不能hold，需要sip同步
    m_bHoldAvailable = false;

    m_bFarHold = false;
    m_bConfWithOnePress = false;
    m_eCallRelation = CR_NONE;

    memset(&m_tSipReinviteInfo, 0, sizeof(SipReinviteInfo));

#if IF_FEATURE_LOOPBACK
    m_bIsLoopBackCall = false;
#endif

    m_bTransferred = false;
    m_bInForward = false;
    m_bRingMute = false;

    m_bAutoAnswerNow = true;
    m_bPassiveRingMute = false;
    m_bAnswered = false;

    m_iDigitKey = PHONE_KEY_0;
    m_bInConfManager = false;
    m_bJoinConf = false;

    memset(m_iConfCallID, 0, sizeof(int) * CONFCALLNUM);
}

CCallInfo::~CCallInfo()
{
    delete m_pPageParam;
    m_pPageParam = NULL;

    delete m_pCCInfo;
    m_pCCInfo = NULL;

    delete m_pDialData;
    m_pDialData = NULL;

    delete m_pCallOutParam;
    m_pCallOutParam = NULL;

    delete m_pRingUserInfo;
    m_pRingUserInfo = NULL;
}

// 刚呼出时对方的信息，在发生通话转移或CallReplace之前
void CCallInfo::SetOriginalRemoteInfo(bool bForced/* = false*/)
{
    // m_tOriginalReomteInfo应该只设置一次，只有当为空时才设置
    if (m_tOriginalRemoteInfo.sSIPName.empty()
            || bForced)
    {
        m_tOriginalRemoteInfo.sSIPName = m_tRemoteInfo.sSIPName;
        m_tOriginalRemoteInfo.sDisplayName = m_tRemoteInfo.sDisplayName;
        m_tOriginalRemoteInfo.sServerName = m_tRemoteInfo.sServerName;
    }
}

void CCallInfo::SetOriginalRemoteInfo(const SipUserInfo& userInfo)
{
    m_tOriginalRemoteInfo.sSIPName = userInfo.sSIPName;
    m_tOriginalRemoteInfo.sDisplayName = userInfo.sDisplayName;
    m_tOriginalRemoteInfo.sServerName = userInfo.sServerName;
}

// 通话是否TLS加密
bool CCallInfo::IsTLSEncrypt()
{
    int nTransport = acc_GetAccountTransPort(m_iSipAccount);
    if (nTransport < 0)
    {
        return acc_GetServerTransport(m_iSipAccount) == 2;
    }
    else
    {
        return nTransport == 2;
    }
}

// 通话是否SRTP加密

// 通话是否SRTP加密
bool CCallInfo::IsSRTPEncrypt()
{
    return IsAudioSrtp();
}

bool CCallInfo::IsAudioSrtp()
{
#if IF_BUG_33301
    return (m_tVPMParam.audio.enc_key[0] & 0xFF) && (m_sipChangnels.call_flag & SIP_CALL_FLAG_ENCRYPT);
#else
    return (m_tVPMParam.audio.enc_key[0] & 0xFF);
#endif
}

// 获取账号对应的会议类型
CONF_TYPE CCallInfo::GetConfType()
{
    if (m_iSipAccount != INVALID_ACCOUNTID)
    {
        return (CONF_TYPE)configParser_GetCfgItemIntValue(kszConfType, m_iSipAccount);
    }

    return CT_LOCAL;
}

// 获取账号对应的BLA类型
BLA_TYPE CCallInfo::GetBLAType()
{
    if (acc_IsAccountIDValid(m_iSipAccount))
    {
        return (BLA_TYPE)acc_GetBLAType(m_iSipAccount);
    }

    return BLA_NONE;
}

// 保存通话时长
void CCallInfo::SetCallDuration(int iSec)
{
    if (0 == iSec)
    {
        return;
    }

    m_iCallDuration = iSec;
}

// 给对方发送Hold消息
void CCallInfo::SendHoldMsg(bool bHold/* = true*/)
{
#ifdef IF_FEATURE_PSTN
    if (m_eCallLineType == CALL_LINE_PSTN)
    {
        pstnadapter_SetHold(acc_GetDeviceIDByAccountID(m_iSipAccount), bHold);
    }
    else
#endif // IF_FEATURE_PSTN
    {
        modVoip_HoldCall(m_iSipAccount, m_iCallId, bHold, false);
    }
}

// 查找联系人信息
void CCallInfo::SetContactInfo(bool bSearchDisp/* = true*/)
{
    SignalParams sigp(this);
    sigp.wParam.bValue = bSearchDisp;
    modtklsn_emit(TALK_SIG_SESSION_CONTRACT_CHANGED, this);
}

// 是否HD通话
bool CCallInfo::IsHDTalking()
{
    //G722，AMR-WB，opus（16K以上）， G722.1C的是HD
    yl::string strCodecName = m_tVPMParam.audio.codec_name;

    yl::to_lower(strCodecName);

    if (strCodecName == "g722"
            || strCodecName == "g722.1c"
            || strCodecName == "amr-wb")
    {
        return true;
    }

    if (strCodecName == "opus"
            && m_tVPMParam.audio.local_maxcapturerate >= 16000
            && m_tVPMParam.audio.remote_maxplaybackrate >= 16000)
    {
        return true;
    }

    return false;
}

yl::string CCallInfo::GetRemoteSipURL() const
{
    yl::string strUrl = "sip:";

    strUrl.append(m_tRemoteInfo.sSIPName).append("@").append(m_tRemoteInfo.sServerName);

    return strUrl;
}

AUDIO_CODEC_TYPE CCallInfo::GetAudioCodecType()
{
    AUDIO_CODEC_TYPE eAudioCodec = ACT_NONE;
    yl::string strCodecName = m_tVPMParam.audio.codec_name;
    yl::to_lower(strCodecName);

    if (strCodecName == "opus")
    {
        eAudioCodec = ACT_OPUS;
    }
    else if (strCodecName == "amr-wb"
             || strCodecName == "amr-nb"
             || strCodecName == "amr")
    {
        eAudioCodec = ACT_AMR;
    }
    else if (IsHDTalking())
    {
        eAudioCodec = ACT_HD;
    }
    return eAudioCodec;
}

bool CCallInfo::IsNeedSaveMissCall()
{
    int nMissCallType = configParser_GetConfigInt(kszMissCallType);
    // http://192.168.1.99/Bug.php?BugID=27452
    // 默认配置项为0时，Missed Calls记录所有未接来电;
    // 配置项为1时，过滤DND和通话中的来电记录，仅保存话机空闲时的未接来电;
    // 配置项为2时，过滤话机空闲时的未接来电，仅保存开启DND和通话中的未接来电;

    if (1 == nMissCallType && m_bMissCallInTalk
            || 2 == nMissCallType && !m_bMissCallInTalk)
    {
        return false;
    }

    return true;
}
#if IF_FEATURE_PAGING
bool CCallInfo::BeMulticastPage()
{
    return (CT_MULTICAST_OUT == m_eCallType
            || CT_MULTICAST_IN == m_eCallType);
}
#endif

// 是否音频通话
bool CCallInfo::IsAudioTalk()
{
    if ((m_tVPMParam.mode & IPVP_FL_VAM_AUDIO) != 0
            || (m_tVPMParam.mode == BT_IPVP_TALK_MODE))
    {
        return true;
    }

    return false;
}

#ifdef IF_BT_SUPPORT_PHONE
//是否蓝牙话路
bool CCallInfo::IsBtTalk()
{
    if (ROUTINE_BTDIALING == m_eStateOfRoutine
            || ROUTINE_BTPREDIAL == m_eStateOfRoutine
            || ROUTINE_BTRINGING == m_eStateOfRoutine
            || ROUTINE_BTCONNECTING == m_eStateOfRoutine
            || ROUTINE_BTTALKING == m_eStateOfRoutine
            || ROUTINE_BTFINISHED == m_eStateOfRoutine)
    {
        return true;
    }

    return false;
}
#endif // IF_BT_SUPPORT_PHONE

void CCallInfo::SetRelationInfo(CALL_RELATION eRelation /*= CR_NONE*/,
                                const yl::string& strRelation /*= ""*/)
{
    m_eCallRelation = eRelation;
    m_strRelationNum = strRelation;
}

void CCallInfo::SetCallerImageURL(const yl::string& strURL)
{
    if (m_strRemoteImageURL == strURL)
    {
        return;
    }

    SignalParams sigp(this, m_strRemoteImageURL);
    m_strRemoteImageURL = strURL;
    modtklsn_emit(TALK_SIG_SESSION_CALLID_IMAGE_CHANGE, sigp);
}

void CCallInfo::SetCallID(int iCallID)
{
    if (m_iCallId == iCallID)
    {
        return;
    }
    /* 重置为非法值，不通知外部 */
    else if (iCallID == -1)
    {
        m_iCallId = iCallID;
        return;
    }

    TALK_INFO("SetCallID [%d]->[%d]", m_iCallId, iCallID);

    SignalParams sigp(this);
    sigp.wParam.iValue = m_iCallId;
    sigp.lParam.iValue = iCallID;
    m_iCallId = iCallID;
    modtklsn_emit(TALK_SIG_SESSION_CALLID_CHANGE, sigp);
}

void CCallInfo::SetAccountID(int iAccountID)
{
    if (m_iSipAccount == iAccountID)
    {
        return;
    }

#ifdef IF_FEATURE_PSTN
    if (acc_IsPstnAccount(iAccountID))
    {
        SetCallLineType(CALL_LINE_PSTN);
    }
#endif // IF_FEATURE_PSTN

    // 初始化为-1
    if (m_iSipAccount != INVALID_ACCOUNTID)
    {
        SignalParams sigp(this);
        sigp.wParam.iValue = m_iSipAccount;
        sigp.lParam.iValue = iAccountID;
        m_iSipAccount = iAccountID;
        modtklsn_emit(TALK_SIG_SESSION_ACCOUNT_CHANGE, sigp);
    }
    else
    {
        m_iSipAccount = iAccountID;
    }
}

void CCallInfo::SetHold(bool bHold)
{
    if (m_bLocalHold == bHold)
    {
        return;
    }

    SignalParams sigp(this);
    sigp.wParam.bValue = m_bLocalHold;
    sigp.lParam.bValue = bHold;
    m_bLocalHold = bHold;
    CheckHoldStatus();
    modtklsn_emit(TALK_SIG_SESSION_HOLD_CHANGE, sigp);
}

void CCallInfo::SetHeld(bool bHeld)
{
    if (m_bRemoteHold == bHeld)
    {
        return;
    }

    SignalParams sigp(this);
    sigp.wParam.bValue = m_bRemoteHold;
    sigp.lParam.bValue = bHeld;
    m_bRemoteHold = bHeld;
    CheckHoldStatus();
    modtklsn_emit(TALK_SIG_SESSION_HELD_CHANGE, sigp);
}

void CCallInfo::SetFarMute(bool bMute)
{
    if (m_bFarMute == bMute)
    {
        return;
    }

    SignalParams sigp(this);
    sigp.wParam.bValue = m_bFarMute;
    sigp.lParam.bValue = bMute;
    m_bFarMute = bMute;
    modtklsn_emit(TALK_SIG_SESSION_FARMUTE_CHANGE, sigp);
}

void CCallInfo::SetFarHold(bool bHold)
{
    if (m_bFarHold == bHold)
    {
        return;
    }

    SignalParams sigp(this);
    sigp.wParam.bValue = m_bFarHold;
    sigp.lParam.bValue = bHold;
    m_bFarHold = bHold;
    modtklsn_emit(TALK_SIG_SESSION_FARHOLD_CHANGE, sigp);
}

bool CCallInfo::SetIPVPParam(const ipvp_speech_t& pm)
{
    if (memcmp(&pm, &m_tVPMParam, sizeof(ipvp_speech_t)) == 0)
    {
        return false;
    }

    SignalParams sigp(this, yl::string((char*)&pm, sizeof(ipvp_speech_t)));
    sigp.wParam.pValue = (void*)&m_tIPVPParam;
    sigp.lParam.pValue = (void*)&pm;
    memcpy(&m_tIPVPParam, &pm, sizeof(ipvp_speech_t));
    modtklsn_emit(TALK_SIG_SESSION_IPVPPARAM_CHANGE, sigp);
    return true;
}

bool CCallInfo::SetSipChannels(const sip_channels_v20_t& channels)
{
    if (memcmp(&channels, &m_sipChangnels, sizeof(sip_channels_v20_t)) == 0)
    {
        return false;
    }

    SignalParams sigp(this, yl::string((char*)&channels, sizeof(sip_channels_v20_t)));
    sigp.wParam.pValue = (void*)&m_sipChangnels;
    sigp.lParam.pValue = (void*)&channels;
    memcpy(&m_sipChangnels, &channels, sizeof(sip_channels_v20_t));
    TALK_INFO("sip call flag[%02x] hold[%02x]", channels.call_flag, channels.hold_state);

    // held 处理
    SetHeld(0 != (SIP_CALL_HOLD_BY_REMOTE & channels.hold_state));

    m_bHoldAvailable = ((SIP_CALL_FLAG_CAN_HOLD & m_sipChangnels.call_flag) != 0);
    modtklsn_emit(TALK_SIG_SESSION_SIPPARAM_CHANGE, sigp);
    return true;
}

void CCallInfo::SetCallLineType(CALL_LINE_TYPE eType)
{
    if (m_eCallLineType == eType)
    {
        return;
    }

#ifdef IF_BT_SUPPORT_PHONE
    m_eCallLineType = eType;
    // 更新BT账号
    if (eType == CALL_LINE_BT && m_iSipAccount == INVALID_ACCOUNTID)
    {
        SetAccountID(Bluetooth_GetBtAccountID());
    }
#endif
}

ROUTINE_STATE CCallInfo::GetRoutine() const
{
    return m_pSession->GetRoutineState();
}

SESSION_STATE CCallInfo::GetSession() const
{
    return m_pSession->GetSessionState();
}

int CCallInfo::GetSessionID() const
{
    return m_pSession->GetSessionID();
}

bool CCallInfo::IsInherits(ROUTINE_STATE eRoutine) const
{
    return m_pSession->IsInherits(eRoutine);
}

void CCallInfo::SetTalkStart(bool bTalk)
{
    if (m_bStartTalk == bTalk)
    {
        return;
    }

    SignalParams sigp(this);
    sigp.wParam.bValue = m_bStartTalk;
    sigp.lParam.bValue = bTalk;
    m_bStartTalk = bTalk;
    modtklsn_emit(TALK_SIG_SESSION_TALK_STATE_CHANGE, sigp);
}

// 设置对方的信息(参数为不带扩展数据的sip info)
void CCallInfo::SetRemoteInfo(const BasicUserInfo& userInfo)
{
    yl::string strDisplay = userInfo.strDisplay;
    if (!strDisplay.empty()
            && configParser_GetConfigInt(kszSkipDisplayName) != 0)
    {
        //不去使用sip发送过来的display名称。http://192.168.1.99/Bug.php?BugID=21856
        strDisplay.clear();
    }

    // http://192.168.1.99/Bug.php?BugID=20808
    // 该问题属于中性Bug，现添加到中性
    if (m_tRemoteInfo.sSIPName == userInfo.strNumber
            && m_tRemoteInfo.sServerName == userInfo.strServer
            && m_tRemoteInfo.sDisplayName == strDisplay
            && m_tRemoteInfo.sFromSIPName == userInfo.strFromNumber)
    {
        return;
    }

    m_tRemoteInfo.sSIPName = userInfo.strNumber;
    // 去掉引号及制表符
    trim_both(m_tRemoteInfo.sSIPName, "\" \t");

    m_tRemoteInfo.sServerName = userInfo.strServer;
    trim_both(m_tRemoteInfo.sServerName, "\" \t");

    m_tRemoteInfo.sDisplayName = strDisplay;
    trim_both(m_tRemoteInfo.sDisplayName, "\" \t");

    m_tRemoteInfo.sFromSIPName = userInfo.strFromNumber;
    trim_both(m_tRemoteInfo.sFromSIPName, "\" \t");

    if (!m_strNumberBeforeEncrypt.empty())
    {
        m_strNumberBeforeEncrypt.clear();
    }

    SignalParams sigp(this);
    sigp.wParam.pValue = (void*)&m_tRemoteInfo;
    modtklsn_emit(TALK_SIG_SESSION_REMOTE_INFO_CHANGE, sigp);
}

// 设置本方的消息(参数为不带扩展数据的sip_info)
void CCallInfo::SetLocalInfo(const BasicUserInfo& userInfo)
{
    // 判断是否和之前的号码相同
    if (m_tLocalInfo.sSIPName == userInfo.strToNumber
            && m_tLocalInfo.sServerName == userInfo.strToServer
            && m_tLocalInfo.sDisplayName == userInfo.strToDisplay)
    {
        return;
    }

    m_tLocalInfo.sSIPName = userInfo.strToNumber;
    trim_both(m_tLocalInfo.sSIPName, "\" \t");

    m_tLocalInfo.sServerName = userInfo.strToServer;
    trim_both(m_tLocalInfo.sServerName, "\" \t");

    m_tLocalInfo.sDisplayName = userInfo.strToDisplay;
    trim_both(m_tLocalInfo.sDisplayName, "\" \t");

    if (m_tLocalInfo.sDisplayName.empty())
    {
        m_tLocalInfo.sDisplayName = acc_GetAccountShowText(m_iSipAccount);
    }
}

void CCallInfo::CheckHoldStatus()
{
    if (m_bLocalHold && m_bRemoteHold)
    {
        m_eTalkState = TS_HOLD_HELD;
    }
    else if (m_bLocalHold)
    {
        m_eTalkState = TS_HOLD;
    }
    else if (m_bRemoteHold)
    {
        m_eTalkState = TS_HELD;
    }
    else
    {
        m_eTalkState = TS_TALK;
    }
}

void CCallInfo::SetDialedNumber(const yl::string& strText)
{
    /* 未初始化 */
    if (m_strDialedNumber == strText)
    {
        return;
    }

    m_strDialedNumber = strText;

    if (m_eStateOfRoutine != ROUTINE_UNKNOWN)
    {
        modtklsn_emit(TALK_SIG_SESSION_TEXT_CHANGE, SignalParams(this, strText));
    }
}

void CCallInfo::ResetDialedNumber()
{
    m_strDialedNumber.clear();
}

void CCallInfo::SetRoutineState(ROUTINE_STATE eRoutine)
{
    m_eStateOfRoutine = eRoutine;
}

void CCallInfo::SetTransfer(bool bTrans)
{
    if (bTrans != m_bTransferred)
    {
        m_bTransferred = bTrans;
        SendInfoSig(TALK_CALL_INFO_TRNSFER, bTrans);
    }
}

void CCallInfo::SetFWD(bool bFWD)
{
    if (bFWD != m_bInForward)
    {
        m_bInForward = bFWD;
        m_strDialedNumber.clear();
        SendInfoSig(TALK_CALL_INFO_FWD, bFWD);
    }
}

void CCallInfo::SetRingMute(bool bMute)
{
    if (bMute != m_bRingMute)
    {
        m_bRingMute = bMute;
        SendInfoSig(TALK_CALL_INFO_RING_MUTE, bMute);
    }
}

void CCallInfo::SetConfManager(bool bOn)
{
    if (bOn != m_bInConfManager)
    {
        m_bInConfManager = bOn;
        SendInfoSig(TALK_CALL_CONF_MANAGER, bOn);
    }
}

void CCallInfo::SendInfoSig(int iMask, bool bFlag)
{
    SignalParams sigp(this);
    sigp.wParam.iValue = iMask;
    sigp.lParam.bValue = bFlag;
    modtklsn_emit(TALK_SIG_SESSION_INFO_CHANGED, sigp);
}

CallCenterInfo& CCallInfo::GetCCInfo()
{
    if (m_pCCInfo == NULL)
    {
        m_pCCInfo = new CallCenterInfo;
    }

    return *m_pCCInfo;
}

DataDial& CCallInfo::GetDialData()
{
    if (m_pDialData == NULL)
    {
        m_pDialData = new DataDial;
    }

    return *m_pDialData;
}

CallOut_Param& CCallInfo::GetCallOutParam()
{
    if (m_pCallOutParam == NULL)
    {
        m_pCallOutParam = new CallOut_Param;
    }

    return *m_pCallOutParam;
}

CallInUserInfo& CCallInfo::GetRingUserInfo()
{
    if (m_pRingUserInfo == NULL)
    {
        m_pRingUserInfo = new CallInUserInfo;
    }

    return *m_pRingUserInfo;
}
