#include "talklogic_inc.h"

// 来电响铃过程
CRingingRoutine::CRingingRoutine()
{
    m_bAutoANSEnable = true;

#if BUG_21442_9744
    m_iDelayToEnterInTalk = STOP_TIMER_VALUE;
#endif

#if IF_BUG_20643
    m_bFirstEstablishMsg = false;
    m_iEstablished = 200;
#endif  //IF_BUG_20643
}

CRingingRoutine::~CRingingRoutine()
{
#if BUG_21442_9744
    if (STOP_TIMER_VALUE != m_iDelayToEnterInTalk)
    {
        timerKillThreadTimer((UINT)&m_iDelayToEnterInTalk);
    }
#endif
#if IF_BUG_20643
    CancelTimer(m_iEstablished);
#endif  //IF_BUG_20643
}

// 创建Routine之后的处理
void CRingingRoutine::AfterRoutineCreate()
{
    //routine切换，相应的标志需要设置
    InitStateFlagData();

    CallInUserInfo* pUserInfo = &m_pCallInfo->GetRingUserInfo();

    TiptelA_ParseUserInfo(pUserInfo);

#ifdef IF_SUPPORT_BLUETOOTH
    Bluetooth_CallIn(pUserInfo->strNumber.c_str());
#endif // IF_SUPPORT_BLUETOOTH
}

// Routine的定时器响应
bool CRingingRoutine::OnTimer(UINT uTimerID)
{
    bool bHandled = true;

#if BUG_21442_9744
    if (uTimerID == (UINT)&m_iDelayToEnterInTalk)
    {
        timerKillThreadTimer((UINT)&m_iDelayToEnterInTalk);
        m_iDelayToEnterInTalk = STOP_TIMER_VALUE;
        // 跳转到通话界面
        m_pCallSession->EnterRoutine(ROUTINE_TALKING);
    }

#endif
#if IF_BUG_20643
    else if (uTimerID == (UINT)&m_iEstablished)
    {
        CancelTimer(m_iEstablished);
        if (m_bFirstEstablishMsg)
        {
            m_bFirstEstablishMsg = false;
            EstablishTalk();
        }
    }
    else
#endif  //IF_BUG_20643

    {
        bHandled = CBaseRingRoutine::OnTimer(uTimerID);
    }

    return bHandled;
}

// Routine的SIP消息响应
bool CRingingRoutine::OnSIPMsg(msgObject& msg)
{
    bool bHandled = true;

    switch (msg.message)
    {
    case SIP_CALL_RELEASED:
        {
            // 响铃界面，对方挂断，通话类型设置为misscall
            m_pCallInfo->m_eCallType = CT_MISSCALL;

            if (_SessionManager.GetSessionListSize() > 1)
            {
                m_pCallInfo->m_bMissCallInTalk = true;
            }
            // 来电响铃时对方挂断，则退出该Session
            m_pCallSession->ExitSession();
        }
        break;
    case SIP_CALL_ESTABLISH:
        {
            if (Comm_IsHoldMsg(msg))
            {
                // http://bugfree.yealink.com/Bug.php?BugID=83168
                Established(msg);
                break;
            }

            // remote hold和establish消息整合，所以以前可能会先发remote hold消息过来说明解hold,再发establish消息
            // 现在只发establish，所以不是hold消息的话，就一定是通话建立，且是unhold，所以要置成false
            m_pCallInfo->SetHeld(FALSE);

#if IF_BUG_20643
#if BUG_21442_9744
            if (m_bAnswered)
            {
                // http://10.2.1.199/Bug.php?BugID=21442
                CancelTimer(m_iDelayToEnterInTalk);
            }
#endif
            if (SIP_CALL_ESTABLISH == msg.message)
            {
                TALKUI_INFO("m_bFirstEstablishMsg %d", m_bFirstEstablishMsg);
                m_bFirstEstablishMsg = !m_bFirstEstablishMsg;
                if (m_bFirstEstablishMsg)
                {
#ifdef IF_FEATURE_PSTN
                    if (m_pCallInfo->GetCallLineType() == CALL_LINE_PSTN)
                    {
                        int iDeviceId = acc_GetDeviceIDByAccountID(m_pCallInfo->m_iSipAccount);
                        int iLineId = pstnadapter_GetLineId(iDeviceId);
                        Comm_VoiceParam_pstn2ipvp(iLineId, &voiceParam);
                    }
                    else
#endif // IF_FEATURE_PSTN
                    {
                        // 保存通话参数
                        sip_channels_v20_t sipChannelInfo;
                        Comm_GetSipChannelsData(sipChannelInfo, msg);

                        Comm_MediaParam_sip2ipvp(&sipChannelInfo, m_pCallInfo->m_strUserAgent, &m_pCallInfo->GetIPVPParam(),
                                                 &m_pCallInfo->m_stVideoExt);
                        Comm_AudioCodec_sip2ipvp(m_pCallInfo->m_stMediaInfo.m_stAudio, &m_pCallInfo->GetIPVPParam());
                    }

                    SetTimerEx(m_iEstablished);
                    return true;
                }
            }
#endif  //IF_BUG_20643
            // BUG17486修改SIP_CALL_ESTABLISH等价成SIP_CALL_ESTABLISH来处理
            // 因为在客户环境会在Callin发送SIP_CALL_ESTABLISH建立通话播放彩铃,
            // 所以这里需要建立通话,直接进入Talk
            Established(msg);
        }
        break;
    case SIP_CALL_REPLACED_BY_REMOTE:
        {
            sip_call_be_replace_t* pReplaceInfo = (sip_call_be_replace_t*)(msg.GetExtraData());
            if (NULL == pReplaceInfo)
            {
                break;
            }

            // 更新CallID
            _VoiceInterface.RemoveSession(m_pCallInfo->GetCallID());
            m_pCallInfo->SetCallID(pReplaceInfo->new_cid);
        }
        break;
    case SIP_CALL_TRANSFERED_BY_REMOTE:
        {
            // http://10.2.1.199/Bug.php?BugID=8763
            // 增加该消息的处理
            // 跳转到被转移Session
            BeTransferred(msg);
        }
        break;
    case SIP_CALL_FAIL:
        {
            _TalklogicManager.ProcessCallFailed(this, msg, true, false, false);
        }
        break;
    case SIP_CALL_RINGBACK:
        {
            sip_call_ringback_info_t* pRingbackInfo = (sip_call_ringback_info_t*)msg.GetExtraData();
            if (NULL == pRingbackInfo)
            {
                bHandled = false;
                break;
            }

            if (pRingbackInfo->type == 1)
            {
                // 收到183直接进入通话界面，播放彩铃
                Established(msg);
                break;
            }
            else
            {
                // 进入回铃界面
                m_pCallInfo->m_bIsColorRingback = false;
                m_pCallSession->EnterRoutine(ROUTINE_RINGBACK);
                break;
            }
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

// 进入通话
void CRingingRoutine::Established(msgObject& msg)
{
    // 开启通话，而不是到TalkRoutine中开启，否则界面刷新会慢
    ipvp_speech_t voiceParam;
    memset(&voiceParam, 0, sizeof(ipvp_speech_t));

#ifdef IF_FEATURE_PSTN
    if (m_pCallInfo->GetCallLineType() == CALL_LINE_PSTN)
    {
        int iDeviceId = acc_GetDeviceIDByAccountID(m_pCallInfo->GetAccountID());
        int iLineId = pstnadapter_GetLineId(iDeviceId);
        Comm_VoiceParam_pstn2ipvp(iLineId, &voiceParam);
    }
    else
#endif // IF_FEATURE_PSTN
    {
        sip_channels_v20_t sipChannelInfo;
        Comm_GetSipChannelsData(sipChannelInfo, msg);
        Comm_MediaParam_sip2ipvp(&sipChannelInfo, m_pCallInfo->m_strUserAgent, &voiceParam,
                                 &m_pCallInfo->m_stVideoExt);
        Comm_AudioCodec_sip2ipvp(m_pCallInfo->m_stMediaInfo.m_stAudio, &voiceParam);
    }

    CBaseRoutine::StartTalkByIpvpParam(&voiceParam, m_pCallInfo);

    // 跳转到通话界面
    m_pCallSession->EnterRoutine(ROUTINE_TALKING);
}

#if IF_BUG_20643
// 进入通话
void CRingingRoutine::EstablishTalk()
{
    // 开启通话，而不是到TalkRoutine中开启，否则界面刷新会慢
    StartTalk();

    // 跳转到通话界面
    m_pCallSession->EnterRoutine(ROUTINE_TALKING);
}
#endif  //IF_BUG_20643

// 处理ui发送过来的Action事件
bool CRingingRoutine::OnActionEvent(DataUI2Logic* pActionData)
{
    if (pActionData == NULL)
    {
        return false;
    }

    bool bHandled = true;
    switch (pActionData->eAction)
    {
    case TALK_ACTION_CANCEL:
    case TALK_ACTION_REJECT:
        {
            int iCode = RT_NORMAL;
            if (pActionData->wActionParam > iCode)
            {
                iCode = pActionData->wActionParam;
            }
            // 主动拒绝才记录
            RefuseIncomingCall(iCode);
        }
        break;
    case TALK_ACTION_ANSWER:
        {
            // 接听来电不刷新界面,Recall要刷新
            AnswerIncomingCall();

#if BUG_21442_9744
            // http://10.2.1.199/Bug.php?BugID=21442
            if (STOP_TIMER_VALUE == m_iDelayToEnterInTalk)
            {
                m_iDelayToEnterInTalk = 500;
                timerSetThreadTimer((UINT)&m_iDelayToEnterInTalk, m_iDelayToEnterInTalk);
            }
#endif
        }
        break;
    case TALK_ACTION_JOIN:
        {
            AnswerIncomingCall(false, false);
            _SessionManager.EstablishLocalConf(m_pCallSession);
        }
        break;
    default:
        bHandled = CBaseRingRoutine::OnActionEvent(pActionData);
        break;
    }

    return bHandled;
}

// 拒绝新来电
void CRingingRoutine::RefuseIncomingCall(int iRefuseType/* = RT_NORMAL*/)
{
#if IF_BUG_38956
    if (0 == configParser_GetConfigInt(kszRejectIncomingCallSwitch))
    {
        return;
    }
#endif

    if (!acc_IsAccountIDValid(m_pCallInfo->GetAccountID())
#ifdef IF_FEATURE_PSTN
            || acc_IsPstnAccount(m_pCallInfo->GetAccountID())
#endif // IF_FEATURE_PSTN
       )
    {
        return;
    }

    // 如果已经接起，则直接挂机
    // http://10.2.1.199/Bug.php?BugID=25659
    if (m_pCallInfo->m_bAnswered)
    {
        m_pCallSession->ExitSession(true);
        return;
    }

    //若消息处理则进行
    SignalParams sigp(m_pCallSession);
    if (modtkflt_emit(TALK_FLT_SESSION_REFUSE, sigp) && sigp.uReply)
    {
        return;
    }

    int iRefuseCode;
    if (iRefuseType == RT_NORMAL)
    {
        iRefuseCode = configParser_GetConfigInt(kszNormalRefuseCode);
    }
    else if (iRefuseType == RT_DND)
    {
        iRefuseCode = configParser_GetConfigInt(kszForbiddenRefuseCode);
    }
    else if (iRefuseType == RT_BUSY)
    {
        iRefuseCode = configParser_GetConfigInt(kszNormalBusyRefuseCode);
    }

    modVoip_RefuseCall(m_pCallInfo->GetAccountID(), m_pCallInfo->GetCallID(), iRefuseCode);

    // 如果已经应答了再拒绝，则需要EndCall
    // 退出当前routine， 当拒接时，不用再发Hangup了
    m_pCallSession->ExitSession(false);
}

// 接起新来电，是否需要调整通道
bool CRingingRoutine::AnswerIncomingCall(bool bSwitchChannel/* = true*/, bool bAudioOnly/* = true*/)
{
    // 如果已经应答,则不再处理
    if (m_pCallInfo->m_bAnswered)
    {
        return false;
    }

    if (m_pCallInfo->IsRingMute() && m_pCallInfo->m_bPassiveRingMute)
    {
        m_pCallInfo->SetRingMute(false);
    }

    int iAccountID = m_pCallInfo->GetAccountID();
    if (acc_IsAccountIDValid(iAccountID))
    {
        m_pCallInfo->m_bAnswered = true;

#ifdef IF_FEATURE_PSTN
        if (m_pCallInfo->GetCallLineType() == CALL_LINE_PSTN)
        {
            // 摘机
            pstnadapter_SetHook(acc_GetDeviceIDByAccountID(iAccountID), true);
            // PSTN应答后直接建立通话
            m_pCallInfo->SetHeld(false);

            msgObject msgTemp;
            Established(msgTemp);
            return true;
        }
        else
#endif // IF_FEATURE_PSTN
        {
            // 发送接听通话的消息后等待通话建立的消息
            modVoip_AnswerCall(iAccountID, m_pCallInfo->GetCallID(), 0, false);
        }
    }

    // 不需要刷新界面
    return false;
}

// forward新来电
bool CRingingRoutine::ForwardIncomingCall(LPCSTR lpszNumber/* = NULL*/,
        FWD_TYPE eFwdType/* = FWT_NONE*/)
{
    // 如果lpszNumber不为空则FWD到lpszNumber，否则FWD到m_strFWDNumber
    yl::string strTarget = m_pCallInfo->GetDialText();
    if (lpszNumber != NULL)
    {
        strTarget = lpszNumber;
    }

    if (strTarget.empty())
    {
        return false;
    }

    modVoip_RequestForward(m_pCallInfo->GetAccountID(), m_pCallInfo->GetCallID(),
                           strTarget.c_str(), eFwdType);

    m_pCallSession->ExitSession();
    return true;
}

// 发消息通知对方结束通话
void CRingingRoutine::EndCall()
{
    if (m_pCallInfo->m_bAnswered)
    {
        CBaseRoutine::EndCall();
    }
    else
    {
        modVoip_RefuseCall(m_pCallInfo->GetAccountID(),
                           m_pCallInfo->GetCallID(), configParser_GetConfigInt(kszNormalRefuseCode));
    }
}

bool CRingingRoutine::Redirectcall(LPCSTR lpTarget)
{
    return ForwardIncomingCall(lpTarget);
}

bool CRingingRoutine::IsAllowAddToConf()
{
    //conf界面按join时routine还处于ringing
    return true;
}
