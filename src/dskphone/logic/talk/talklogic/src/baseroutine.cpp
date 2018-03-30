#include "talklogic_inc.h"

///////////////////////////Routine相关的函数/////////////////////////////////
// 根据话路过程的状态获取话路指针
CBaseRoutine* GetRoutineByState(ROUTINE_STATE eState)
{
    switch (eState)
    {
    case ROUTINE_PREDIAL:
        return new CPredialRoutine;
    case ROUTINE_DIALING:
        return new CDialRoutine;
    case ROUTINE_RINGING:
        return new CRingingRoutine;
    case ROUTINE_CONNECTING:
        return new CConnectingRoutine;
    case ROUTINE_RINGBACK:
        return new CRingbackRoutine;
    case ROUTINE_TALKING:
        return new CTalkRoutine;
    case ROUTINE_FINISHED:
        return new CFinishedRoutine;
    case ROUTINE_PRETRAN:
        return new CPreTranRoutine;
#ifdef IF_FEATURE_BROADSOFT_SCA
    case ROUTINE_BLACONNECTING:
        return new CBLAConnectingRoutine;
#endif
    case ROUTINE_AUTOREDIAL:
        return new CAutoRedialRoutine;
    //case ROUTINE_ATSCONF:  return new CATSConfRoutine;
    case ROUTINE_NETWORKCONF_CONNECT:
        return new CNetworkConfConnectRoutine;
    case ROUTINE_NETWORKCONF:
        return new CNetworkConfRoutine;
    case ROUTINE_RECALL:
        return new CRecallRoutine;
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    case ROUTINE_RETRIEVE_CALLPARK:
        return new CPreRetrieveParkRoutine;
    case ROUTINE_PARK_NOTIFY:
        return new CParkNotifyRoutine;
#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
    case ROUTINE_AOC_FINISH:
        return new CAocFinishRoutine;
#ifdef IF_SUPPORT_LINE_DEVICE
    case ROUTINE_LINE_DEVICE:
        return new CLineDeviceTalkRoutine;
#endif
#ifdef IF_BT_SUPPORT_PHONE
    case ROUTINE_BTCONNECTING:
        return new CBTConnectingRoutine;
    case ROUTINE_BTDIALING:
        return new CBTDialRoutine;
    case ROUTINE_BTPREDIAL:
        return new CBTPredialRoutine;
    case ROUTINE_BTRINGING:
        return new CBTRingingRoutine;
    case ROUTINE_BTTALKING:
        return new CBTTalkingRoutine;
    case ROUTINE_BTFINISHED:
        return new CBTFinishedRoutine;
#endif // IF_BT_SUPPORT_PHONE

    default:
        TALK_ERR("GetRoutineByState error unkown routine type [%d]", eState);
        break;
    }

    return NULL;
}

// CBaseRoutine:Routine的基类
CBaseRoutine::CBaseRoutine() : m_pCallSession(NULL), m_pCallInfo(NULL)
{
}

CBaseRoutine::~CBaseRoutine()
{
    CancelTimer(m_iMadiaSniffer);
}

bool CBaseRoutine::IsInherits(ROUTINE_STATE eState) const
{
    return Comm_IsRoutineInherits(eState, GetRoutineState());
}

// 发消息通知对方结束通话
void CBaseRoutine::EndCall()
{
    int iAccountID = m_pCallInfo->GetAccountID();
    int iCallID = m_pCallInfo->GetCallID();

    if (iAccountID != INVALID_ACCOUNTID
            && iCallID != -1
#ifdef IF_SUPPORT_LINE_DEVICE
            && ROUTINE_LINE_DEVICE != GetRoutineState()
#endif
       )
    {
#ifdef IF_FEATURE_PSTN
        if (m_pCallInfo->GetCallLineType() == CALL_LINE_PSTN)
        {
            // PSTN线路挂机
            pstnadapter_SetHook(acc_GetDeviceIDByAccountID(iAccountID), false);
        }
        else
#endif // IF_FEATURE_PSTN
        {
            // 外接设备不需要发送挂断消息
            // 给sip发消息挂断
            modVoip_HangupCall(iAccountID, iCallID);
        }
    }
}

// 退出前停止通话相关
void CBaseRoutine::StopTalkBeforeDelete()
{
    // 如果还有通话存在，则停止通话
    if (m_pCallInfo->IsTalkStart())
    {
        StopTalk();
    }
}

// 开启通话
void CBaseRoutine::StartTalk(int iCallId)
{
    StartTalk();
}

void CBaseRoutine::StartTalk(bool bNeedHold /*= true*/)
{
    if (m_pCallInfo == NULL)
    {
        return;
    }

#if IF_FEATURE_LOOPBACK
    if (m_pCallInfo->m_bIsLoopBackCall)
    {
        return;
    }
#endif

    int iCallID = m_pCallInfo->GetCallID();
    SignalParams sigp(m_pCallInfo);
    sigp.wParam.bValue = bNeedHold;
    //通话开启
    modtklsn_emit(TALK_SIG_SESSION_PRE_START_TALK, sigp);

    // 开音频之前通知
    // 开启音视频通话，通话整改时，保留此处即可
    etl_NotifyApp(true, TALK_MSG_TALK_START, m_pCallSession->GetSessionID(),
                  m_pCallInfo->GetAccountID());

    if (m_pCallInfo->IsTalkStart())
    {
        _VoiceInterface.ResetTalk(iCallID, m_pCallInfo->m_tVPMParam);
    }
    else
    {
        // 添加通话
        _VoiceInterface.AddSession(iCallID);
    }

#if 0
    PrintVoice(m_pCallInfo->m_tVPMParam);
#endif

    // 将标志置为true
    m_pCallInfo->SetTalkStart(true);

    // 话路远端被禁言且当前话路在会议中
    if (m_pCallInfo->IsFarMute())
    {
        if (m_pCallSession->InLocalConf())
        {
            _VoiceInterface.SetFarMute(iCallID, m_pCallInfo->m_tVPMParam, true);
        }
        else
        {
            // 退出会议后禁言就取消
            m_pCallInfo->SetFarMute(false);
        }
    }

    modtklsn_emit(TALK_SIG_SESSION_AFTER_START_TALK, SignalParams(m_pCallInfo));
}

// 进入呼叫完成状态的处理
void CBaseRoutine::ProcessAfterCallFinished(const TalkErrorInfo& errorInfo,
        bool bCallFail/* = true*/, bool bPlayBusyTone/* = true*/, bool bEndCall/* = true*/)
{
    bool bExitSession = false;
    // bPlayBusyTone为false时只有当BusyToneDelay不等于0才播放忙音
    // callfailed时也根据kszBusytoneDelay来判断是否响忙音，不强制响 http://192.168.1.99/Bug.php?BugID=16475
    if (configParser_GetConfigInt(kszForceToPlayBusytoneAfterCallFailed) == 0
            || !bPlayBusyTone)
    {
        if (configParser_GetConfigInt(kszBusytoneDelay) == 0)
        {
            bExitSession = true;
        }
    }

    // 如果是Silence自动应答，则不响忙音 || 如果被加入到网络会议则不响忙音
    if (m_pCallInfo->m_bSilentAnswer
            || m_pCallInfo->m_bAddToNetworkConf)
    {
        bExitSession = true;
    }

    // 如果该Session还有HostSession，则不响忙音(比如会议中的一路或BeTran之后的一路)
    if (m_pCallSession->GetHostSession() != NULL)
    {
        bExitSession = true;
    }

    //呼叫失败
    if (bCallFail)
    {
        //更新CBeTransferredSession中的sponsor关系
        CBaseSession* pSession = m_pCallSession->GetHostSession();
        if (pSession)
        {
            CBeTransferredSession* pBetranSession = pSession->ToBeTransferredSession();
            if (pBetranSession != NULL)
            {
                pBetranSession->ResetRelation();
            }
        }

        m_pCallInfo->SetRelationInfo(CR_BUSY);
    }

    // 如果当前Session是其他Session的SponsorSession，则取消这种关系
    // 需提前处理,防止进入finish界面后,不会马上取消Sponsor关系
    m_pCallSession->ResetSponsor();

    // 如果开启预通话，进入CallFinish则要关闭通话
    // 如果已开启了通话，则先停止
    if (m_pCallInfo->IsTalkStart())
    {
        StopTalk();
    }

    // 话路多于一路，不允许AutoRedial
    // 如果允许自动重拨,则强制进入finish界面
    if (bCallFail && _AutoRedialManager.BeAllowAutoRedial()
            && _SessionManager.GetSessionListSize() <= 1)
    {
        CBaseSession* pHostSession = m_pCallSession->GetHostSession();
        // BeTransfer自动发起的呼出不应该触发AutoRedial http://bugfree.yealink.com/Bug.php?BugID=132455
        if ((pHostSession == NULL || !pHostSession->IsBeTransferSession())
                && !_AutoRedialManager.BeCountDownFinished())
        {
            // 进入AutoRedial
            m_pCallSession->EnterRoutine(ROUTINE_AUTOREDIAL);
        }
        else
        {
            // 倒计时完成,直接退出session
            m_pCallSession->ExitSession(bEndCall);
        }
    }
    else if (bExitSession)
    {
        m_pCallSession->ExitSession(bEndCall);
    }
    else
    {
        m_pCallInfo->m_errorInfo = errorInfo;
        m_pCallSession->EnterRoutine(ROUTINE_FINISHED);
    }
}

// 停止通话
void CBaseRoutine::StopTalk()
{
    if (m_pCallInfo->IsTalkStart())
    {
        TALK_INFO("CBaseRoutine::StopTalk");

#if IF_FEATURE_PAGING
        _MulticastPage.PagingTalkSwitch(m_pCallInfo->GetCallID(), false);
#endif
        m_pCallInfo->SetTalkStart(false);
        _VoiceInterface.StopSession(m_pCallInfo->GetCallID());
    }

    etl_NotifyApp(true, TALK_MSG_TALK_STOP, m_pCallSession->GetSessionID(), m_pCallInfo->GetAccountID());

    //停止通话
    modtklsn_emit(TALK_SIG_SESSION_STOP_TALK, SignalParams(m_pCallInfo));
}

// 根据callid停止通话
void CBaseRoutine::StopTalk(int iCallId)
{
#if IF_FEATURE_PAGING
    // 停Paging
    _MulticastPage.PagingTalkSwitch(iCallId, false);
#endif
    _VoiceInterface.StopSession(iCallId);

    // 关闭音视频通话，通话整改时，保留此处即可
    etl_NotifyApp(true, TALK_MSG_TALK_STOP, m_pCallSession->GetSessionID(), m_pCallInfo->GetAccountID());

    //停止通话
    modtklsn_emit(TALK_SIG_SESSION_STOP_TALK, SignalParams(talklogic_GetCallInfoByCallId(iCallId)));
}

// 通话被转移
void CBaseRoutine::BeTransferred(msgObject& msg)
{
    sip_call_be_transfer_t* pTransferInfo = (sip_call_be_transfer_t*)(msg.GetExtraData());
    if (NULL == pTransferInfo)
    {
        return;
    }

    // step1:新建一路呼出状态的Session
    CSingleSession* pConnectSession = _SessionManager.AllocSession(pTransferInfo->new_cid, true, m_pCallSession);
    if (pConnectSession == NULL)
    {
        return;
    }

    // 被转移之后记录新的通话信息
    CCallInfo* pNewCallInfo = pConnectSession->GetCallInfo();
    pNewCallInfo->SetAccountID(m_pCallInfo->GetAccountID());
    // 通话状态
    pNewCallInfo->SetTalkStart(m_pCallInfo->m_eTalkState);
    // hold状态
    pNewCallInfo->SetHold(m_pCallInfo->IsLocalHold());
    // FarHold状态
    pNewCallInfo->SetFarHold(m_pCallInfo->IsFarHold());
    // FarMute状态
    pNewCallInfo->SetFarMute(m_pCallInfo->IsFarMute());
    // 被转之后的话路当成呼出类型处理
    pNewCallInfo->m_eCallType = CT_OUTGOING;

    // step2:将其跳转到connect状态
    pConnectSession->EnterRoutine(ROUTINE_CONNECTING);

    CBeTransferredSession* pBeTranSession = NULL;
    CBaseSession* pHostSession = m_pCallSession->GetHostSession();
    if (pHostSession != NULL)
    {
        pBeTranSession = pHostSession->ToBeTransferredSession();
    }

    // step3:新建被转移的Session并将当前两路Session赋给被转移Session
    if (pBeTranSession == NULL)
    {
        pBeTranSession = new CBeTransferredSession;
        // 加入Sessionlist失败
        if (!_SessionManager.AddSession(pBeTranSession, pConnectSession, false))
        {
            return;
        }

        // step4:将当前Session的host转交给BeTranSession
        if (m_pCallSession->GetHostSession() != NULL)
        {
            pBeTranSession->SetHostSession(m_pCallSession->GetHostSession());
        }
    }

    pBeTranSession->AddTalkSession(m_pCallSession);
    pBeTranSession->AddRingbackSession(pConnectSession);

    pBeTranSession->AfterSessionCreate();

    // session添加到sessionlist中之后再取设置对方信息
    BasicUserInfo userInfo;
    MapSipUserInfo_BasicInfo(&pTransferInfo->user_info, userInfo);
    pNewCallInfo->SetRemoteInfo(userInfo);

    m_pCallInfo->m_strRelationNum = pNewCallInfo->m_tRemoteInfo.sSIPName;
}

// 根据SIP消息的参数开启通话
void CBaseRoutine::StartTalkByIpvpParam(ipvp_speech_t* pSpeechInfo, CCallInfo* pCallInfo,
                                        bool bNeedHold /*= true*/)
{
    if (NULL == pSpeechInfo
            || pCallInfo == NULL)
    {
        return;
    }

    pCallInfo->SetIPVPParam(*pSpeechInfo);
    StartTalk(bNeedHold);
}

void CBaseRoutine::SetSession(CSingleSession* pSession, CCallInfo* pCallInfo)
{
    m_pCallSession = pSession;
    m_pCallInfo = pCallInfo;
    m_pCallInfo->SetRoutineState(GetRoutineState());
}

