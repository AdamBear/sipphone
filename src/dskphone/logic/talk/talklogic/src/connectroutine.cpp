#include "talklogic_inc.h"

// 呼出连接过程
CConnectingRoutine::CConnectingRoutine()
{
    m_iTimerCancelCall = STOP_TIMER_VALUE;
#if IF_BUG_40237
    m_bTransferAfterRingBack = false;
#endif
}

CConnectingRoutine::~CConnectingRoutine()
{
    CancelTimer(m_iTimerCancelCall);
}
// 创建Routine之后的处理
void CConnectingRoutine::AfterRoutineCreate()
{
    // 呼出无应答时，定时取消呼出
    SetAutoCancelCallTimer();
}

// Routine的定时器响应
bool CConnectingRoutine::OnTimer(UINT uTimerID)
{
    bool bHandled = false;
    if (uTimerID == (UINT)&m_iTimerCancelCall)
    {
        bHandled = true;
        CancelTimer(m_iTimerCancelCall);
        modVoip_HangupCall(m_pCallInfo->GetAccountID(), m_pCallInfo->GetCallID(), 3);

        TalkErrorInfo errorInfo(TALK_ERROR_CALLOUT_TO);
        ProcessAfterCallFinished(errorInfo);
    }

    if (!bHandled)
    {
        bHandled = CBaseRoutine::OnTimer(uTimerID);
    }

    // 不作处理
    return bHandled;
}

// Routine的SIP消息响应
bool CConnectingRoutine::OnSIPMsg(msgObject& msg)
{
    bool bHandled = true;

    switch (msg.message)
    {
    case SIP_CALL_RINGBACK:
        {
#if IF_BUG_40237
            if (m_bTransferAfterRingBack)
            {
                if (m_pCallSession->InTransfer())
                {
                    m_pCallSession->TranCurrentSession();
                    return true;
                }
                else
                {
                    m_pCallSession->SetHide(false);
                }
            }
#endif
            sip_call_ringback_info_t* pRingbackInfo = (sip_call_ringback_info_t*)msg.GetExtraData();
            if (NULL == pRingbackInfo)
            {
                bHandled = false;
                break;
            }
            TALK_INFO("CConnectingRoutine::OnSIPMsg(SIP_CALL_RINGBACK): pRingbackInfo->type = %d",
                      pRingbackInfo->type);

            // 根据回铃的类型来处理(0：普通回铃 1：彩铃 2:182铃声,3:华为ACD签入成功)
            if (pRingbackInfo->type == 0)
            {
                // 普通回铃
                m_pCallInfo->m_bIsColorRingback = false;
            }
            else if (pRingbackInfo->type == 2)
            {
                m_pCallInfo->m_bPlay182Tone = true;
            }
            else
            {
                // 彩铃
                m_pCallInfo->m_bIsColorRingback = true;

                // 保存播放彩铃的通话参数
                Comm_RingbackParam_sip2ipvp(pRingbackInfo, &m_pCallInfo->GetIPVPParam(), &m_pCallInfo->m_stVideoExt);
            }
            // 跳转到回铃界面
            m_pCallSession->EnterRoutine(ROUTINE_RINGBACK);
        }
        break;
    case SIP_CALL_FAIL:
        {
            if (NULL == msg.GetExtraData())
            {
                bHandled = false;
                break;
            }

#if IF_BUG_40237
            if (m_bTransferAfterRingBack
                    && m_pCallSession->InTransfer())
            {
                m_pCallSession->SetHide(false);
            }
#endif
            // 如果呼叫失败则一定会播放忙音,不管BusyToneDelay是否为0
            _TalklogicManager.ProcessCallFailed(this, msg);
        }
        break;
    case SIP_CALL_RELEASED:
        {
            // 对方挂断的情况下，只有当BusyToneDelay不等于0才播放忙音
            _TalklogicManager.ProcessCallFailed(this, msg, false, false);
        }
        break;
    case SIP_CALL_ESTABLISH:
        {
            sip_channels_v20_t sipChannelInfo;
            Comm_GetSipChannelsData(sipChannelInfo, msg);

            // sip协议自己获取远端设备
            if (m_pCallInfo->m_strUserAgent.empty())
            {
                m_pCallInfo->m_strUserAgent = modVoip_GetUserAgent(m_pCallInfo->GetAccountID(),
                                              m_pCallInfo->GetCallID());
            }

            // 如果是本地hold状态，则只处理hold
            if (Comm_IsLocalHold(sipChannelInfo))
            {
                ProcessSIPHold(msg);
                break;
            }

#if IF_BUG_23673
            // 将cpu让出来（让给从ui到vpm的消息传递以及vpm开启通话）
            etlSleep(100);
#endif
            bool bReStartTalk = true;

            // 如果新的通话VPM参数和彩铃VPM参数相同则不再重新开启通话
            ipvp_speech_t voiceParam;
            memset(&voiceParam, 0, sizeof(ipvp_speech_t));

            Comm_MediaParam_sip2ipvp(&sipChannelInfo, m_pCallInfo->m_strUserAgent, &voiceParam,
                                     &m_pCallInfo->m_stVideoExt);
            Comm_AudioCodec_sip2ipvp(m_pCallInfo->m_stMediaInfo.m_stAudio, &voiceParam);

            if (m_pCallInfo->m_bIsColorRingback)
            {
                bReStartTalk = m_pCallInfo->SetIPVPParam(voiceParam);
            }

            //是否加入了本地会议
            bool bIsInLocalConf = m_pCallSession->IsInLocalConf();
            //本地会议是否hold
            bool bConfHold = false;
            //是否播放conf的tone音
            bool bPlayConfTone = false;

            if (bReStartTalk)
            {
                // 当存在HostSession时，可能处于Hold状态(如BeTransferredSession)
                if (!m_pCallInfo->IsLocalHold())
                {
                    if (bIsInLocalConf)
                    {
                        CLocalConfSession* pConfSession = _SessionManager.GetConfSession();
                        if (pConfSession != NULL)
                        {
                            if (pConfSession->IsInHold())
                            {
                                bConfHold = true;
                            }
                            else
                            {
                                bPlayConfTone = true;
                            }
                        }
                    }

                    if (bConfHold)
                    {
                        StartTalkByIpvpParam(&voiceParam, m_pCallInfo, false);
                        _VoiceInterface.HoldSession(m_pCallInfo->GetCallID());
                        m_pCallInfo->SetTalkStart(false);

                        m_pCallInfo->SendHoldMsg();
                    }
                    else
                    {
                        // V80需要先开通话，再开启会议
                        // 开启通话，而不是到TalkRoutine中开启，否则界面刷新会慢
                        StartTalkByIpvpParam(&voiceParam, m_pCallInfo);

                        // 会议中参与方被tran，则重新开启会议
                        m_pCallSession->OnSubSessionChanged(m_pCallSession);

                        if (bPlayConfTone)
                        {
                            //播放conf提示音
                            SignalParams sigp(m_pCallInfo);
                            sigp.wParam.iValue = 0;
                            sigp.lParam.iValue = CHANNEL_AUTO;
                            sigp.tParam.iValue = 3;
                            modtklsn_emit(TALK_SIG_SESSION_PLAY_CONF_TONE, sigp);
                        }
                    }
                }
                else
                {
                    // 先记录参数，在发送hold；否则是否是视频通话的判断会出错
                    // 如果connect状态就hold，则进入到通话界面后对方要变成held状态
                    m_pCallInfo->SendHoldMsg();
                }
            }
            else
            {
                // http://192.168.1.99/Bug.php?BugID=20304
                // 防止在CRingbackRoutine::BeforeRoutineDestroy中停止彩铃，从而停止了通话
                m_pCallInfo->m_bIsColorRingback = false;
            }

            // http://bugfree.yealink.com/Bug.php?BugID=66597
            if ((sipChannelInfo.call_flag & SIP_CALL_FLAG_SERVER_CONF) != 0)
            {
                // 跳转到网络会议界面
                m_pCallSession->EnterRoutine(ROUTINE_NETWORKCONF);
            }
            else
            {
                // 跳转到通话界面
                m_pCallSession->EnterRoutine(ROUTINE_TALKING);
            }
        }
        break;
    case SIP_CALL_REPLACED_BY_REMOTE:
        {
            // http://10.2.1.199/Bug.php?BugID=9698
            // 将此处理由Ringback移到此处
            sip_call_be_replace_t* pReplaceInfo = (sip_call_be_replace_t*)msg.GetExtraData();
            if (NULL == pReplaceInfo)
            {
                break;
            }

            if (m_pCallInfo->IsTalkStart())
            {
                // 在CallID改变之前先停止通话
                StopTalk();

                // 更新CallID之后再重新启动通话
                m_pCallInfo->SetCallID(pReplaceInfo->new_cid);
                _VoiceInterface.RemoveSession(m_pCallInfo->GetCallID());

                // 按新的callid开启通话
                StartTalk();
            }
            else
            {
                m_pCallInfo->SetCallID(pReplaceInfo->new_cid);
                _VoiceInterface.RemoveSession(m_pCallInfo->GetCallID());
            }
        }
        break;
#ifdef IF_FEATURE_PSTN
    case PSTN_MSG_DIAL_RESULT:
        {
            TALK_INFO("PSTN_MSG_DIAL_RESULT when connecting.");
            TalkErrorDefine* pDialResult = (TalkErrorDefine*)msg.GetExtraData();
            if (NULL == pDialResult)
            {
                bHandled = false;
                break;
            }

            int iAccountID = m_pCallInfo->GetAccountID();

            TALK_INFO("PSTN_MSG_DIAL_RESULT: result=%d", *pDialResult);
            if (*pDialResult == TALK_ERROR_NONE)
            {
                // 外拨成功
                ipvp_speech_t voiceParam;
                memset(&voiceParam, 0, sizeof(ipvp_speech_t));
                int iDeviceId = acc_GetDeviceIDByAccountID(iAccountID);
                int iLineId = pstnadapter_GetLineId(iDeviceId);
                Comm_VoiceParam_pstn2ipvp(iLineId, &voiceParam);
                StartTalkByIpvpParam(&voiceParam, m_pCallInfo);

                // 跳转到通话界面
                m_pCallSession->EnterRoutine(ROUTINE_TALKING);
            }
            else
            {
                // 外拨失败则自动挂机（进入ROUTINE_FINISHED不会调用挂机）
                int iDeviceId = acc_GetDeviceIDByAccountID(iAccountID);
                pstnadapter_SetHook(iDeviceId, false);

                // 如果呼叫失败则一定会播放忙音,不管BusyToneDelay是否为0
                ProcessAfterCallFinished(TALK_ERROR_CALLOUT_FN);
            }
            break;
        }
#endif // IF_FEATURE_PSTN
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

void CConnectingRoutine::StartTalkByIpvpParam(ipvp_speech_t* pSpeechInfo, CCallInfo* pCallInfo,
        bool bNeedHold /*= true*/)
{
    CBaseRoutine::StartTalkByIpvpParam(pSpeechInfo, pCallInfo, bNeedHold);

    // 当前音频视频参数不完全放在，需要强制发此消息
    etl_NotifyApp(true, TALK_MSG_TALK_RESET, m_pCallSession->GetSessionID(),
                  m_pCallInfo->GetAccountID());
}

// 获取对应session的状态
SESSION_STATE CConnectingRoutine::GetHostSessionState()
{
    if (m_pCallSession->GetSponsorSession(SPONSOR_TRAN) != NULL)
    {
        return SESSION_CONNECTING_TRAN;
    }
    else
    {
        return SESSION_CONNECTING;
    }
}

// 处理sip 的hold状态
void CConnectingRoutine::ProcessSIPHold(msgObject& msg)
{
    int iState = 0;
    if (!Comm_GetSipCodeState(msg, iState))
    {
        return;
    }

    if (SIP_CALL_HOLD_DEFAULT == iState)
    {
        return;
    }

    m_pCallInfo->SetHold(0 != (SIP_CALL_HOLD_BY_LOCAL & iState));

    sip_channels_v20_t sipChannelInfo;
    Comm_GetSipChannelsData(sipChannelInfo, msg);

    Comm_MediaParam_sip2ipvp(&sipChannelInfo, m_pCallInfo->m_strUserAgent, &m_pCallInfo->GetIPVPParam(),
                             &m_pCallInfo->m_stVideoExt);
    Comm_AudioCodec_sip2ipvp(m_pCallInfo->m_stMediaInfo.m_stAudio, &m_pCallInfo->GetIPVPParam());
    // 跳转到通话界面
    m_pCallSession->EnterRoutine(ROUTINE_TALKING);
}

void CConnectingRoutine::SetAutoCancelCallTimer()
{
    m_iTimerCancelCall = configParser_GetConfigInt(kszRingBackTimeout);
    if (m_iTimerCancelCall <= 0 || m_iTimerCancelCall > 3600)
    {
        m_iTimerCancelCall = 180;
    }

    m_iTimerCancelCall *= 1000;

    SetTimerEx(m_iTimerCancelCall);
}

bool CConnectingRoutine::IsAllowAddToConf()
{
    //配置kszConfWithOnePress开启后，去电也能够直接加入会议
    return m_pCallInfo->GetConfType() == CT_LOCAL
           && configParser_GetConfigInt(kszConfWithOnePress) == 1;
}


