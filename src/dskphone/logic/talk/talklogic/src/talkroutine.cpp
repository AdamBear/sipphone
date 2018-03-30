#include "talklogic_inc.h"

// 会议重协商超时退出
#define RENEGOTIME (10 * 1000)

#define REINVITE_REQUEST_FAILED_TIMER  3*1000;
#define REINVITE_RESPOND_TIMER  20*1000;

// 通话中过程
CTalkRoutine::CTalkRoutine()
{
    m_iWaitTranResultTimeout = STOP_TIMER_VALUE;
    m_iTimerRenego = STOP_TIMER_VALUE;
#if IF_BUG_30423
    m_strOldSIPName = "";
#endif
    m_bReplacedInSub = false;

    m_iRespondTimer = STOP_TIMER_VALUE;
    m_iRequestFailedTimer = STOP_TIMER_VALUE;
    m_bRequestVideo = false;

    m_strTransferNum = "";
    m_eDelayOperate = TDO_NONE;
    m_bWaitForHold = false;
}

CTalkRoutine::~CTalkRoutine()
{
    CancelTimer(m_iWaitTranResultTimeout);
    CancelTimer(m_iTimerRenego);
    CancelTimer(m_iRespondTimer);
    CancelTimer(m_iRequestFailedTimer);
}

// 创建Routine之后的处理
void CTalkRoutine::AfterRoutineCreate()
{
    m_pCallInfo->SetConfManager(false);
    m_pCallInfo->SetTransfer(false);

    // 设置通话状态
    m_pCallInfo->CheckHoldStatus();

    //记录原始通话信息
    m_pCallInfo->SetOriginalRemoteInfo();

#if IF_BUG_30423
    // 初始化当前通话对方号码
    m_strOldSIPName = m_pCallInfo->m_tRemoteInfo.sSIPName;
#endif

    // 进入通话后自动建立会议功能
    if (m_pCallInfo->m_bConferenceDirect)
    {
        m_pCallInfo->m_bConferenceDirect = false;

        //一键建立会议开启则不需要再建一次会议
        if (!m_pCallInfo->m_bConfWithOnePress)
        {
            EstablishConf();
        }
    }

#ifdef IF_SUPPORT_BLUETOOTH
    Bluetooth_AnswerCall();
    Bluetooth_HSStartPlay();
#endif // IF_SUPPORT_BLUETOOTH

    _AOCManager.ProcessTalkStart(m_pCallInfo->GetCallID());

    int iSessionID = (NULL != m_pCallSession->GetHostSession()) ?
                     m_pCallSession->GetHostSession()->GetSessionID() : m_pCallSession->GetSessionID();

    // 发送通话建立的消息
    etl_NotifyApp(FALSE, TALK_MSG_TALK_ENTER, iSessionID, m_pCallInfo->GetAccountID());
}

// 销毁Routine之前的处理
void CTalkRoutine::BeforeRoutineDestroy(bool bJumpToNextRoutine/* = true*/)
{
    CBaseRoutine::BeforeRoutineDestroy(bJumpToNextRoutine);

    // 停止通话
    StopTalk();
    _AOCManager.ProcessTalkFinish(m_pCallInfo->GetCallID());
}

// Routine的定时器响应
bool CTalkRoutine::OnTimer(UINT uTimerID)
{
    bool bHandled = false;

    if (uTimerID == (UINT)&m_iTimerHoldDelay)
    {
        bHandled = true;

        CancelTimer(m_iTimerHoldDelay);
    }
    else if (uTimerID == (UINT)&m_iWaitTranResultTimeout)
    {
        bHandled = true;

        CancelTimer(m_iWaitTranResultTimeout);
        m_pCallSession->ExitSession(true);
    }
    else if (uTimerID == (UINT)&m_iTimerRenego)
    {
        bHandled = true;
        CancelTimer(m_iTimerRenego);
    }
    else if (uTimerID == TIMER_ID(m_iRequestFailedTimer))
    {
        CancelTimer(m_iRequestFailedTimer);
    }

    if (!bHandled)
    {
        bHandled = CBaseRoutine::OnTimer(uTimerID);
    }

    return bHandled;
}

// Routine的SIP消息响应
bool CTalkRoutine::OnSIPMsg(msgObject& msg)
{
    bool bHandled = false;

    switch (msg.message)
    {
    case SIP_CALL_ESTABLISH:
        {
            bHandled = ProcessEstablish(msg);
        }
        break;
    case SIP_CALL_REPLACED_BY_REMOTE:
        {
            bHandled = true;

            // 获取新的通话信息
            sip_call_be_replace_t* pReplaceInfo = (sip_call_be_replace_t*)(msg.GetExtraData());
            if (NULL == pReplaceInfo)
            {
                break;
            }

            if (m_pCallInfo->GetCallID() != pReplaceInfo->new_cid)
            {
                // 先停掉当前通话
                StopTalk();
                _VoiceInterface.RemoveSession(m_pCallInfo->GetCallID());
                // 更新CallID之后再重新启动通话
                m_pCallInfo->SetCallID(pReplaceInfo->new_cid);

                if (!m_pCallInfo->IsLocalHold())
                {
                    // 会议中参与方被替换,则通话建立后重新开启会议 http://bugfree.yealink.com/Bug.php?BugID=89592
                    if (NULL != m_pCallSession->GetHostSession()
                            && !m_pCallInfo->IsTalkStart())
                    {
                        m_bReplacedInSub = true;
                    }
                    else
                    {
                        m_pCallSession->OnSubSessionChanged(m_pCallSession);
                    }

#if !IF_BUG_22272
                    StartTalk();
#endif
                }
            }
        }
        break;
    case SIP_CALL_TRANSFERED_BY_REMOTE:
        {
            bHandled = true;
            // 跳转到被转移Session
            BeTransferred(msg);
        }
        break;
    case SIP_CALL_FAIL:
        {
            bHandled = true;
            _TalklogicManager.ProcessCallFailed(this, msg);
        }
        break;
    case SIP_CALL_RELEASED:
        {
            bHandled = true;

            //csta
            if (m_pCallSession->InLocalConf())
            {
                m_pCallInfo->m_bIsSingleExitCof = true;
            }

            if (m_pCallInfo->IsTransferred())
            {
                m_pCallSession->ExitSession(true);
            }
#ifdef IF_FEATURE_EMERGENCY
            else if (m_pCallSession->IsHide()
                     && (m_pCallSession->GetHostSession() == NULL))
            {
                //场景：dut 呼叫紧急号码,接听后挂断,紧急号码端接着挂断
                //非转移情况下,隐藏 并且非会议时
                //当前隐藏话路的情况，
                m_pCallSession->ExitSession();
            }
#endif
            else
            {
                bool bAddedToConf = false;
                if (m_pCallInfo->m_bAddToNetworkConf)
                {
                    // 如果该话路已经被加入网络会议中，则在退出不给对方发送CallEnd消息
                    // 否则对方已被加入网络会议之后，收到callend就退出了
                    bAddedToConf = true;
                }

                // 对方挂断的情况下，只有当_objConfig.GetBusyToneDelay()不等于0才播放忙音
                // 对方挂断通话，将通话时间传入Finished界面
                int iDuration = AdapterBox_TalkUIGetTalkTimeBySessionId(m_pCallSession->GetSessionID());
                yl::string strDuration = commonAPI_FormatString("%.2d:%.2d:%.2d",
                                         iDuration / 3600, (iDuration % 3600 / 60), (iDuration % 3600 % 60));

                TalkErrorInfo errorInfo(TARK_ERROR_CALL_RELEASED, strDuration);
                ProcessAfterCallFinished(errorInfo, false, false, !bAddedToConf);
            }
        }
        break;
    case SIP_CMD_CALL_HOLD:
        {
            if (!m_pCallInfo->IsLocalHold())
            {
                bHandled = true;
                HoldRoutine(true);
            }
        }
        break;
    case SIP_CMD_CALL_RESUME:
        {
            // 进行Hold或UnHold
            if (m_pCallInfo->IsLocalHold())
            {
                bHandled = true;
                HoldRoutine(false);
            }
        }
        break;
    case SIP_CALL_TRANSFERED_RESULT:
        {
            if (!m_pCallInfo->IsTransferred())
            {
                bHandled = false;
            }
            else
            {
                sip_reason_info_t* pTransferResult = (sip_reason_info_t*)(msg.GetExtraData());
                if (NULL == pTransferResult)
                {
                    break;
                }

                int iAccountID = m_pCallInfo->GetAccountID();
                bHandled = true;
                // 0标识Transfer成功
                if (pTransferResult->code == 0)
                {
                    // kszHangUpAfterSuccessTrans 这个配置sip那边也有在用，两边重复了，因此这边去掉,
                    m_pCallSession->ExitSession();
                }
                else
                {
                    // 注意：只有中性咨询转的情况下，因为本地自己挂断了通话(没有通知sip)，所以收到结果后，要挂断
                    // http://10.2.1.199/Bug.php?BugID=83702
                    // 盲转失败，把前面的那一路挂断
                    if (m_pCallInfo->m_iTransferId != -1
                            && m_pCallInfo->m_iTransferId != 0)
                    {
                        modVoip_HangupCall(iAccountID, m_pCallInfo->m_iTransferId);
                    }

                    m_pCallInfo->m_iTransferId = -1;
                    // 盲转等待Sesion隐藏，盲转失败进入recall
                    if (m_pCallSession->IsHide())
                    {
                        if (!_TalklogicManager.IsAllowNewCall(iAccountID))
                        {
                            m_pCallSession->ExitSession(true);
                            break;
                        }

                        EnterRecall();
                    }
                    else
                    {
                        modtklsn_emit_sync(TALK_SIG_SESSION_ATTENDED_TRANSFER_FAIL, SignalParams(m_pCallInfo));
                    }
                }
            }
        }
        break;
    case SIP_CALL_REINVITE_RECIEVE:
        {
            sip_reinvite_t* pReInviteInfo = (sip_reinvite_t*)(msg.GetExtraData());
            if (NULL == pReInviteInfo)
            {
                break;
            }

            if (MapSipReInviteInfo(pReInviteInfo, m_pCallInfo->m_tSipReinviteInfo))
            {
#ifdef IF_FEATURE_EMERGENCY
                if (SIP_CALL_REINVITE_FLAG_BW_EMERGENCY_RING_BACK == pReInviteInfo->type)//紧急回呼请求
                {
                    // 紧急通话挂断  Sesion隐藏，紧急号码 呼入时，进入recall 流程
                    if (m_pCallSession->IsHide())
                    {
                        EnterRecall();
                    }
                }
#endif
            }
        }
        break;
    default:
        break;
    }

    return bHandled;
}

// 处理ui发送过来的Action事件
bool CTalkRoutine::OnActionEvent(DataUI2Logic* pActionData)
{
    if (pActionData == NULL)
    {
        return false;
    }

    bool bHandled = true;
    switch (pActionData->eAction)
    {
    case TALK_ACTION_TRAN:
        {
            EstablishTran();
        }
        break;
    case TALK_ACTION_TRAN_BIND_SPONSOR:
        {
            if (configParser_GetConfigInt(kszTransferSetHoldSessionAsHost) != 1)
            {
                break;
            }

            CSingleSession* pTranSession = _SessionManager.GetSingleSession(pActionData->wActionParam);

            if (pTranSession != NULL)
            {
                CCallInfo* pCallInfo = pTranSession->GetCallInfo();

                if (pCallInfo != NULL)
                {
                    if (m_pCallSession->GetSponsorSession(SPONSOR_TRAN) == pTranSession
                            || pTranSession->GetSponsorSession(SPONSOR_TRAN) == m_pCallSession)
                    {
                        bHandled = false;
                        break;
                    }

                    TALK_INFO("this id=[%d] hold=[%d] trans id =[%d] hold=[%d]",
                              m_pCallSession->GetSessionID(), m_pCallInfo->IsLocalHold(),
                              pActionData->wActionParam, pCallInfo->IsLocalHold());

                    if (m_pCallInfo->IsLocalHold() && !pCallInfo->IsLocalHold())
                    {
                        pTranSession->SetSponsorSession(SPONSOR_TRAN, m_pCallSession);
                    }
                    else if (!m_pCallInfo->IsLocalHold() && pCallInfo->IsLocalHold())
                    {
                        m_pCallSession->SetSponsorSession(SPONSOR_TRAN, pTranSession);
                    }
                }
            }
        }
        break;
    case TALK_ACTION_TRAN_TO:
        {
            //if (IsAllowTran())
            // 去掉该条件 http://10.2.1.199/Bug.php?BugID=36939
            bHandled = TransferToSession(_SessionManager.GetSession(pActionData->wActionParam));
        }
        break;
    case TALK_ACTION_CONF:
        {
            if (DSK_PHONE_TYPE_BLACK == commonUnits_GetDskPhoneType())
            {
                CSingleSession* pSession = _SessionManager.GetRelateSessionByCallID(pActionData->wActionParam);
                if (NULL != pSession)
                {
                    pSession->SetSponsorSession(SPONSOR_CONF, m_pCallSession);
                    if (pSession->GetCallInfo()->GetConfType() == CT_BROADSOFT
                            && m_pCallSession->GetCallInfo()->GetConfType() == CT_BROADSOFT)
                    {
                        _SessionManager.EstablishNetworkConf(pSession);
                    }
                    else
                    {
                        _SessionManager.EstablishLocalConf(pSession);
                    }
                    break;
                }
            }

            EstablishConf();
        }
        break;
    case TALK_ACTION_JOIN:
        {
            CSingleSession* pSession = _SessionManager.GetFirstIncomingCallSession();
            if (pSession == NULL)
            {
                return false;
            }

            CCallInfo* pCallInfo = pSession->GetCallInfo();
            pCallInfo->m_bJoinConf = true;
            m_pCallSession->ProcessIncomingCall(true);
            _SessionManager.EstablishLocalConf(pSession);
            pCallInfo->m_bJoinConf = false;
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

// 获取对应session的状态
SESSION_STATE CTalkRoutine::GetHostSessionState()
{
    if (m_pCallInfo->IsTransferred())
    {
        return SESSION_TRANSFERRED;
    }
    else if (NULL != m_pCallInfo
             && _AOCManager.IsAocShow(m_pCallInfo->GetCallID()))
    {
        return SESSION_AOC_TALKING;
    }
    else
    {
        return SESSION_TALKING;
    }
}

bool CTalkRoutine::ProcessEstablish(msgObject& msg)
{
    // 如果是hold则不再处理
    if (Comm_IsHoldMsg(msg))
    {
        ProcessSIPHold(msg);
        return true;
    }

    bool bHandled = false;
    if (m_pCallInfo->IsLocalHold())
    {
        if (NULL != m_pCallSession->GetHostSession()
                && m_pCallInfo->m_bHoldAvailable)
        {
            TALK_WARN("Hold stats sync");
            // http://10.2.1.199/Bug.php?BugID=94934 如果通话是子话路，那么此时以本地hold状态为准
            m_pCallInfo->SendHoldMsg(true);
            return bHandled;
        }

        // 普通通话以sip同步回来的状态为准
        m_pCallInfo->SetHold(false);
        // http://10.3.5.199/Bug.php?BugID=62892 需要开启通话，通话参数变化时，才会去reset
        _VoiceInterface.ResumeSession(m_pCallInfo->GetCallID());
    }

    if (m_pCallInfo->IsRemoteHold())
    {
        m_pCallInfo->SetHeld(false);
    }
    if (msg.GetExtraData() != NULL)
    {
        ipvp_speech_t voiceParam;
        sip_channels_v20_t sipChannelInfo;
        Comm_GetSipChannelsData(sipChannelInfo, msg);

        Comm_MediaParam_sip2ipvp(&sipChannelInfo, m_pCallInfo->m_strUserAgent, &voiceParam,
                                 &m_pCallInfo->m_stVideoExt);
        Comm_AudioCodec_sip2ipvp(m_pCallInfo->m_stMediaInfo.m_stAudio, &voiceParam);
        // 开启通话
        bHandled = ReStartTalk(&voiceParam);
        // todo test
        //会议中参与方被替换, 则通话建立后重新开启会议 http://bugfree.yealink.com/Bug.php?BugID=89592
        if (m_bReplacedInSub && m_pCallInfo->IsTalkStart())
        {
            m_bReplacedInSub = false;
            m_pCallSession->OnSubSessionChanged(m_pCallSession);
        }

        // http://10.2.1.199/Bug.php?BugID=88426 新Hold方式需要收到消息后刷新界面
        bHandled = true;
    }

    ProcessDelayOperate();

    return bHandled;
}

bool CTalkRoutine::HoldRoutine(bool bHold, HOLD_TYPE eHoldType/* = HT_NORMAL*/,
                               bool bHoldByHost/* = false*/)
{
    // !m_pCallInfo->m_bHoldButNotStopTalk for voice_hold// 调用该接口hold，不停止通话
    if (m_pCallInfo->IsLocalHold() && bHold
            || !m_pCallInfo->IsLocalHold() && !bHold)
    {
        TALK_WARN("[TALK] Current Routine is already hold or unhold!");
        return false;
    }

    // 只对unhold防抖，hold操作有可能是被动的（比如新起一路，hold其他路）
    // 会议中由confsession自己做防抖
    if (m_pCallInfo->IsLocalHold()
            && !m_pCallSession->InLocalConf())
    {
        // 如果有设置了Hold操作定时器,则下一次的hold不再处理
        if (m_iTimerHoldDelay != STOP_TIMER_VALUE)
        {
            return false;
        }

        m_iTimerHoldDelay = HOLD_DELAY_TIME;
        SetTimerEx(m_iTimerHoldDelay);
    }

#if IF_FEATURE_PAGING
    // Paging的呼出
    if (m_pCallInfo->BeMulticastPage())
    {
        return HoldPagingTalk(bHold);
    }
#endif

    BLA_TYPE eBla = m_pCallInfo->GetBLAType();
    if (eBla == BLA_DRAFT_BLA
            && eHoldType != HT_PUBLIC)
    {
        eHoldType = HT_PUBLIC;
    }

    if (eBla == BLA_BROADSOFT_SCA
            && eHoldType != HT_PUBLIC
            && eHoldType != HT_PRIVATE)
    {
        eHoldType = HT_PUBLIC;
    }

#if DEBUG_TALK_LOGIC
    TALK_INFO("HoldRoutine: hold[%d] type[%d] host[%d]", bHold, eHoldType, bHoldByHost);
#endif //DEBUG_TALK_LOGIC

    switch (eHoldType)
    {
    case HT_PRIVATE:
    case HT_PUBLIC:
        {
            if (eBla == BLA_BROADSOFT_SCA)
            {
                TALK_INFO("HoldRoutine: SCA hold[%d] BLAIndex[%d]", bHold, m_pCallInfo->m_iBLAIndex);
                modVoip_BLAHold(m_pCallInfo->GetAccountID(), eHoldType == HT_PUBLIC,
                                m_pCallInfo->GetCallID(), bHold, m_pCallInfo->m_iBLAIndex);
            }
            else if (eBla == BLA_DRAFT_BLA)
            {
                TALK_INFO("HoldRoutine: DraftBla hold[%d]", bHold);
                modVoip_DraftBLAHold(m_pCallInfo->GetAccountID(), eHoldType == HT_PUBLIC, m_pCallInfo->GetCallID(),
                                     bHold);
            }
            else
            {
                // 向SIP服务器发送Hold消息
                m_pCallInfo->SendHoldMsg(bHold);
            }

            break;
        }
    case HT_STAUTS_ONLY:
    case HT_NORMAL:
    case HT_EMENERGY_CALL:
        {
#ifdef IF_FEATURE_NOT_HOLD_INTERCOM_CALL
            // http://192.168.1.99/Bug.php?BugID=18570
            if (configParser_GetConfigInt(kszNotHoldByIntercomCall) != 0)
            {
                // held状态时，如果是Intercom来电，则不发送hold信息
                if (_TalklogicManager.GetCallInIntercom()
                        && m_pCallInfo->IsRemoteHold())
                {
                    _TalklogicManager.SetCallInIntercom(false);
                    break;
                }
            }
#endif // IF_FEATURE_NOT_HOLD_INTERCOM_CALL

            if (HT_EMENERGY_CALL != eHoldType)
            {
                // 向SIP服务器发送Hold消息
                m_pCallInfo->SendHoldMsg(bHold);
            }
            break;
        }
    default:
        break;
    }

    // 设置本地hold的状态
    m_pCallInfo->SetHold(bHold);

    if (m_pCallInfo->IsLocalHold())
    {
        // hold
        SetCallHold(true, (eHoldType == HT_STAUTS_ONLY));

        m_pCallInfo->SetTalkStart(false);
    }
    else
    {
        SetCallHold(false, (eHoldType == HT_STAUTS_ONLY));

        // 此处held时，没有设置m_bStartTalk为true，大版本需要考虑设置为true
        // http://10.3.5.199/Bug.php?BugID=62516 因为StartTalk未设置true导致
        if (!m_pCallInfo->IsRemoteHold())
        {
            m_pCallInfo->SetTalkStart(true);
        }
    }

    m_bWaitForHold = true;

    return true;
}

// hold/unhold当前话路
bool CTalkRoutine::SetCallHold(bool bHold, bool bStatusOnly)
{
    if (bHold)
    {
        _VoiceInterface.HoldSession(m_pCallInfo->GetCallID(), bStatusOnly);
    }
    else
    {
        _VoiceInterface.ResumeSession(m_pCallInfo->GetCallID(), bStatusOnly);
    }
    return true;
}

// 盲转
void CTalkRoutine::BlindTransfer(const yl::string& strNumber)
{
    if (strNumber.empty())
    {
        return;
    }

    // tran之后隐藏的Session一定要处于hold状态
    if (!m_pCallInfo->IsLocalHold())
    {
        // 要强制停止通话，不能只hold
        // http://10.2.1.199/Bug.php?BugID=36486
        // V80 不需要强制Stop
        HoldRoutine(true);
    }

    // 隐藏Session并解除Dsskey绑定
    m_pCallSession->SetHide(true);

    // 等待sip发送hold成功的消息过来后再去执行盲转
    if (m_bWaitForHold)
    {
        TALK_INFO("Wait For Hold strNumber[%s]", strNumber.c_str());
        m_strTransferNum = strNumber;
        m_eDelayOperate = TDO_BLIND_TRANSFER;
        return;
    }
    else
    {
        m_strTransferNum.clear();
    }

    //http://ts.yealink.com/Bug.php?BugID=42069
    int iHoldDelay = configParser_GetConfigInt(kszHoldTranDelay);
    if (iHoldDelay > 0)
    {
        etlSleep(iHoldDelay);
    }

    // test
    TALK_INFO("BlindTransfer Number[%s]", strNumber.c_str());
    // 实现盲转
    modVoip_RequestTransfer(m_pCallInfo->GetAccountID(), m_pCallInfo->GetCallID(), -1, strNumber.c_str());
    //csta 不为-1即可
    m_pCallInfo->m_iTransferId = 0;

    // 转移以后状态标志置为true
    m_pCallInfo->SetTransfer(true);
    // 设置2分钟超时(2分钟未收到Tran Result则退出)
    m_iWaitTranResultTimeout = 2 * 60 * 1000;
    SetTimerEx(m_iWaitTranResultTimeout);
}

// 半咨询转，如果不允许半咨询转，则用盲转代替
void CTalkRoutine::SemiAttendedTransfer(CSingleSession* pSessionToTran)
{
    if (pSessionToTran == NULL)
    {
        return;
    }

    // 获取要转移的话路的CallInfo
    CCallInfo* pCallInfo = pSessionToTran->GetCallInfo();

    // 根据配置来判断是否允许半咨询转
    if (configParser_GetConfigInt(kszSemiAttendedTranSwitch) == 0)
    {
        // ExitSession和BlindTransfer顺序不允许调换 http://10.2.1.199/Bug.php?BugID=89625
        // 如果不允许半咨询转，则用盲转代替，必须在退出前获取目标号码
        yl::string strTarget = pCallInfo->m_tRemoteInfo.sSIPName;
        // follow session退出且给对方发endcal
        pSessionToTran->ExitSession(true);
        // 用盲转代替
        BlindTransfer(strTarget);
    }
    else
    {
        //转接
        Transfer(pSessionToTran);
    }
}

// 咨询转
void CTalkRoutine::AttendedTransfer(CSingleSession* pSessionToTran)
{
    if (pSessionToTran == NULL)
    {
        return;
    }

    // 获取要转移的话路的CallInfo
    CCallInfo* pCallInfo = pSessionToTran->GetCallInfo();

    int iHoldDelay = configParser_GetConfigInt(kszHoldTranDelay);
    if (iHoldDelay != 0)
    {
        // 先将两路hold再进行咨询转:http://192.168.1.99/Bug.php?BugID=11544
        pSessionToTran->HoldSession(true);
        etlSleep(iHoldDelay);
        m_pCallSession->HoldSession(true);
        etlSleep(iHoldDelay);
    }

    // Matrix 需求Transfer之后如果失败了，Flow Session不退出
    if (configParser_GetConfigInt(kszTransferWaitResult) == 0)
    {
        // 转接
        m_pCallSession->SetHide(TRUE);
        // 设置2分钟超时(2分钟未收到Tran Result则退出)
        m_iWaitTranResultTimeout = 2 * 60 * 1000;
        SetTimerEx(m_iWaitTranResultTimeout);
    }

    Transfer(pSessionToTran);
}


bool CTalkRoutine::Transfer(CSingleSession* pSessionToTran)
{
    if (pSessionToTran == NULL)
    {
        return false;
    }

    CCallInfo* pCallInfo = pSessionToTran->GetCallInfo();
    int iTransCallID = pCallInfo->GetCallID();
    int iAccountID = m_pCallInfo->GetAccountID();

    if (configParser_GetConfigInt(kszTransferByByeReason) != 0)
    {
        //Panasonic PBX的Transfer的方式，以HandUp进行
        //http://192.168.1.99/Bug.php?BugID=23151
        return modVoip_HangupCall(iAccountID, iTransCallID, 2);
    }
    else
    {
        modVoip_RequestTransfer(iAccountID, m_pCallInfo->GetCallID(), iTransCallID);
    }

    //CSTA-syl
    m_pCallInfo->m_iTransferId = iTransCallID;

    pCallInfo ->m_iTransferId = iTransCallID;
    // 转移以后状态标志置为true
    m_pCallInfo->SetTransfer(true);

    if (configParser_GetConfigInt(kszTransferWaitResult) == 0)
    {
        // follow session退出但不给对方发endcal
        pSessionToTran->ExitSession();
    }

    return true;
}


// 当进入tran拨号界面，或者tran当前两路
void CTalkRoutine::EstablishTran()
{
    if (m_pCallSession->InTransfer())
    {
        // 如果已发起tran或被其他话路发起tran
        m_pCallSession->TranCurrentSession();
    }
    else
    {
        // 直接TRANSFER
        if (TransferSessionDirectly())
        {
            return;
        }

        CSingleSession* pSession = m_pCallSession->CreateNewDial(ROUTINE_PRETRAN);
        if (pSession != NULL)
        {
            pSession->SetSponsorSession(SPONSOR_TRAN, m_pCallSession);
        }
    }

    return;
}

BOOL CTalkRoutine::TransferToSession(CBaseSession* pSessionToTran)
{
    if (pSessionToTran == NULL)
    {
        return FALSE;
    }

    CSingleSession* pTransSession = pSessionToTran->ToSingleSession();
    if (pTransSession == NULL)
    {
        return FALSE;
    }

    if (m_pCallSession->GetSponsorSession(SPONSOR_TRAN) == pTransSession)
    {
        CTalkRoutine* pTalkRoutine = pTransSession->GetTalkRoutine();
        if (pTalkRoutine == NULL)
        {
            return FALSE;
        }

        pTalkRoutine->AttendedTransfer(m_pCallSession);
    }
    else
    {
        AttendedTransfer(pTransSession);
    }

    return TRUE;
}

// 建立新的拨号话路
void CTalkRoutine::EstablishNewDial(bool bConfTo/* = false*/, LPCSTR lpszDialNumber/* = NULL*/)
{
#if IF_FEATURE_PAGING
    if (m_pCallSession->BeMulticastPage())
    {
        return;
    }
#endif

    CSingleSession* pSession = _SessionManager.AllocSession();
    if (pSession == NULL)
    {
        return;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();
    pCallInfo->SetAccountID(m_pCallInfo->GetAccountID());

    if (bConfTo)
    {
        // Conf发起的拨号界面
        pSession->SetSponsorSession(SPONSOR_CONF, m_pCallSession);
    }

    _TalklogicManager.TryEnterRoutine(pSession, ROUTINE_DIALING, lpszDialNumber);
}

void CTalkRoutine::AddNetworkConf(CSingleSession* pSessionAddToConf)
{
    if (pSessionAddToConf == NULL)
    {
        return;
    }

    if (!pSessionAddToConf->IsInherits(ROUTINE_TALKING))
    {
        return;
    }

    ROUTINE_STATE eThisState = GetRoutineState();
    ROUTINE_STATE eState = pSessionAddToConf->GetRoutineState();

    if (IsTalkRoutine(eThisState)
            && IsTalkRoutine(eState))
    {
        // 两路都是通话中
        EstablishNetworkConf(pSessionAddToConf);
    }
    else if (IsTalkRoutine(eThisState)
             && eState == ROUTINE_NETWORKCONF)
    {
#ifdef IF_SUPPORT_LINE_DEVICE
        if (SESSION_LINEDEVICE == m_pCallSession->GetSessionState())
        {
            _SessionManager.EstablishLocalConf(m_pCallSession);
        }
        else
#endif
        {
            // 当前是通话，另外一路是网络会议中
            pSessionAddToConf->GetRoutine()->AddToConf(m_pCallSession);
        }
    }
    else if (eThisState == ROUTINE_NETWORKCONF
             && IsTalkRoutine(eState))
    {
#ifdef IF_SUPPORT_LINE_DEVICE
        if (SESSION_LINEDEVICE == pSessionAddToConf->GetSessionState())
        {
            _SessionManager.EstablishLocalConf(m_pCallSession);
        }
        else
#endif
        {
            // 当前是网络会议中，另外一路是通话
            AddToConf(pSessionAddToConf);
        }
    }
}

// 当有两路通话时，建立会议，如果只有1路通话，则进入会议拨号界面
void CTalkRoutine::EstablishConf()
{
    //是否通过拨号界面选择
    bool bConfbyNewDial = (configParser_GetConfigInt(kszConfWithPreviousCall) == 0);
    CONF_TYPE eConfType = m_pCallInfo->GetConfType();

    //网络会议入口
    if (eConfType == CT_BROADSOFT)
    {
        EstablishNetworkConf(NULL);
        return;
    }

    int iSessionSize = _SessionManager.GetAllowAddLocalConfRoutineNum();
    //是否存在本地会议
    bool bLocalConfExist = _SessionManager.IsLocalConfExist();
    //是否有会议发起session
    bool bHaveSponConf = (NULL != m_pCallSession->GetSponsorSession(SPONSOR_CONF));
    //是否允许加入会议总话路数大于本地会议最大话路数
    bool bRoutineNumbOverMaxConf = _SessionManager.IsAllowAddLocalConfNumOverMax();
    //是否存在transfer绑定关系
    bool bInTransfer = m_pCallSession->InTransfer();
    /*
    //进入拨号界面的条件：
    1.只有一路
    2.无会议时bConfbyNewDial为false,话路总数大于会议最大支持路数并且无conf发起session并且不存在transfer绑定关系
    3.无会议时bConfbyNewDial为true，无conf发起session并且不存在transfer绑定关系
    4.存在本地会议时bConfbyNewDial为false,话路总数大于会议最大支持路数并且从会议发起的
    5.存在本地会议时bConfbyNewDial为true，从会议发起的。
    */
    if (iSessionSize == 1
            || (!bConfbyNewDial && !bLocalConfExist && bRoutineNumbOverMaxConf && !bHaveSponConf
                && !bInTransfer)
            || (bConfbyNewDial && !bLocalConfExist && !bHaveSponConf && !bInTransfer)
            || (!bConfbyNewDial && bLocalConfExist && bRoutineNumbOverMaxConf && m_pCallSession->InLocalConf())
            || (bConfbyNewDial && bLocalConfExist && m_pCallSession->InLocalConf()))
    {
        EstablishNewDial(true);
    }
    else
    {
        _SessionManager.EstablishLocalConf(m_pCallSession);
    }
}

// 建立网络会议
void CTalkRoutine::EstablishNetworkConf(CSingleSession* pSessionAddToConf)
{
    int iAccountID = m_pCallInfo->GetAccountID();
    //是否一键合并会议
    bool bConfWithPreviousCall = (configParser_GetConfigInt(kszConfWithPreviousCall) == 1);
    //是否存在主持方会议
    bool bExistHostConf = _SessionManager.GetHostNetConfSession(iAccountID) != NULL;
    //允许加入网络会议的话路总数
    int iTotalNumOfAllowAddConf = _SessionManager.GetTotalNumOfRoutineAllowAddNetConf(iAccountID);
    //当前话路是否为主持方网络会议
    bool bIsHostNetConf = m_pCallSession->GetSessionState() == SESSION_NETWORKCONF;
    //判断是否要进入conf播号界面
    if (pSessionAddToConf == NULL)
    {
        /*进入拨号界面的条件：
        1.当前账号只有一路通话或者存在主持方会议并且无可加入会议的话路。
        2.当前话路是主持方会议,并且允许加入会议话路总数大于最大限制或未配置一键合并会议。
        3.不存在主持方会议，允许加入会议的话路总数大于最大限制，当前话路无Transfer绑定关系并且无SP_CONF绑定关系。
        4.未开启一键合并会议，不存在主持方会议，允许加入会议总数小于最大限制，当前话路无Transfer并且无SP_CONF绑定关系。
        */
        if ((_SessionManager.GetSingleLineSessionListSize(iAccountID) == 1
                || (bIsHostNetConf && iTotalNumOfAllowAddConf == 0))
                || (bIsHostNetConf && (iTotalNumOfAllowAddConf > CONFCALLNUM || !bConfWithPreviousCall))
                || (!bExistHostConf && iTotalNumOfAllowAddConf > CONFCALLNUM
                    && !m_pCallSession->InTransfer() && m_pCallSession->GetSponsorSession(SPONSOR_CONF) == NULL)
                || (!bConfWithPreviousCall && !bExistHostConf && iTotalNumOfAllowAddConf <= CONFCALLNUM
                    && !m_pCallSession->InTransfer() && m_pCallSession->GetSponsorSession(SPONSOR_CONF) == NULL)
           )
        {
            EstablishNewDial(true);
        }
        /*将所有允许加入会议的话路加入会议的条件：
        1.开启一键合并会议配置，并且存在主持方会议，当前允许加入会议话路总数未超过最大限制.
        2.开启一键合并会议配置，不存在主持方，当前允许加入会议话路总数未超过最大限制，当前话路不是主持方会议并且未绑定Transfer关系
        */
        else if ((bConfWithPreviousCall && bExistHostConf && iTotalNumOfAllowAddConf <= CONFCALLNUM)
                 || (bConfWithPreviousCall && !bExistHostConf && iTotalNumOfAllowAddConf <= CONFCALLNUM
                     && !m_pCallSession->InTransfer() && m_pCallSession->GetSponsorSession(SPONSOR_CONF) == NULL)
                )
        {
            //将所有允许话路加入会议
            MergeAllowJoinNetConfRoutine();
        }
        else
        {
            _SessionManager.EstablishNetworkConf(m_pCallSession);
        }

    }
    else
    {
        //加入会议
        CCallInfo* pAddToConfInfo = pSessionAddToConf->GetCallInfo();
        if (pAddToConfInfo == NULL)
        {
            return;
        }

        // 停掉当前通话，此处不能给sip发送hold消息，否则建立网络会议后会有问题
        StopTalk();

        CSingleSession* pConfSession = _SessionManager.AllocSession();
        if (pConfSession == NULL)
        {
            return;
        }

        // 设置网络会议的通话信息
        CCallInfo* pCallInfo = pConfSession->GetCallInfo();
        pCallInfo->SetAccountID(iAccountID);
        pCallInfo->m_tRemoteInfo.sDisplayName = _LANG(TRID_CONFERENCE);
        pCallInfo->m_tRemoteInfo.sSIPName = _LANG(TRID_CONFERENCE);
        pCallInfo->m_tRemoteInfo.sFullContactInfo = _LANG(TRID_CONFERENCE);//http://10.2.1.199/Bug.php?BugID=59058

        // 要加入会议中的双方的CallID
        //兼容BugID=38784，hold的话路先Refer
        int iConfCallID[CONFCALLNUM] = {0};
        int iAddCallID = pAddToConfInfo->GetCallID();
        int iThisCallID = m_pCallInfo->GetCallID();
        bool bLocalHold = pAddToConfInfo->IsLocalHold();

        iConfCallID[0] = bLocalHold ? iAddCallID : iThisCallID;
        iConfCallID[1] = bLocalHold ? iThisCallID : iAddCallID;

        // 两路加入网络会议的话路标记
        pAddToConfInfo->m_bAddToNetworkConf = true;
        m_pCallInfo->m_bAddToNetworkConf = true;

        // 将加入会议的两路隐藏
        pSessionAddToConf->SetHide();
        m_pCallSession->SetHide();

        memcpy(pCallInfo->m_iConfCallID, iConfCallID, sizeof(int) * CONFCALLNUM);
        pConfSession->EnterRoutine(ROUTINE_NETWORKCONF_CONNECT);
    }
}

// 重新开启通话by sip msg
bool CTalkRoutine::ReStartTalk(ipvp_speech_t* pVoiceParam)
{
    if (pVoiceParam == NULL)
    {
        return false;
    }

#if IF_FEATURE_LOOPBACK
    // Loopback 通话始终是关的
    if (m_pCallInfo->m_bIsLoopBackCall)
    {
        return false;
    }
#endif

#if 0
    PrintVoice(*pVoiceParam);
    PrintVoice(&m_pCallInfo->m_tVPMParam);
#endif

#if 0
    Comm_PrintMedia(*pVoiceParam);
    Comm_PrintMedia(&m_pCallInfo->m_tVPMParam);
#endif
    bool bHandled = false;
    // 如果当前未开启通话或新的VPM参数和之前的VPM参数不同，则重新开启通话
    if (!m_pCallInfo->IsTalkStart()
            || m_pCallInfo->SetIPVPParam(*pVoiceParam))
    {
        StartTalk();
        bHandled = true;
    }
    else
    {
        TALK_INFO("VoiceParam no Changed!");
    }

    SignalParams sigp(m_pCallInfo);
    sigp.wParam.bValue = bHandled;
    modtklsn_emit(TALK_SIG_SESSION_RESET_TALK, sigp);
    // 当前音频视频参数不完全放在，需要强制发此消息
    etl_NotifyApp(true, TALK_MSG_TALK_RESET, m_pCallSession->GetSessionID(),
                  m_pCallInfo->GetAccountID());

    return bHandled;
}

// 是否支持直接转接
bool CTalkRoutine::IsTransferDirectly()
{
    if (_SessionManager.GetSessionListSize() != 2
            || m_pCallInfo->m_eTalkCallOutType != TCT_TIPTEL_AUTO_CALLOUT)
    {
        TALK_WARN("Not allow trans directly[%d]", m_pCallInfo->m_eTalkCallOutType);
        return false;
    }

    return true;
}

bool CTalkRoutine::TransferSessionDirectly()
{
    // 是否是两路，且支持直接TRANSFER(目前只有TCT_TIPTEL_AUTO_CALLOUT支持)
    if (!IsTransferDirectly())
    {
        TALK_WARN("Not allow trans directly CallOutType[%d]", m_pCallInfo->m_eTalkCallOutType);
        return false;
    }

    return TransferToSession(m_pCallSession->GetAnotherSession());
}

#if IF_FEATURE_PAGING
// Hold Paging
bool CTalkRoutine::HoldPagingTalk(bool bHold)
{
    if (!m_pCallSession->BeMulticastPage())
    {
        TALK_WARN("Not Paging Talking !");
        return false;
    }

    // 设置本地hold的状态
    m_pCallInfo->SetHold(bHold);

    if (m_pCallInfo->IsLocalHold())
    {
        // Stop talk
        m_pCallInfo->m_eStopPageType = SP_HOLD;

        //
        StopTalk();

        // 恢复，如果下次直接在Hold中挂断，则会使用此 SP_REJECT 类型
        m_pCallInfo->m_eStopPageType = SP_REJECT;
    }
    else
    {
        // 先判断是否调用paging
        _MulticastPage.PagingTalkSwitch(m_pCallInfo->GetCallID(), true);

        // unhold之后用之前的vpm参数开启通话，在收到SIP_CALL_ESTABLISH
        // 后判断通话参数是否有发生变化，如果未改变则不再重新开启通话
        _VoiceInterface.ResumeSession(m_pCallInfo->GetCallID());
        // 开启通话后必须将info中是否通话的参数置为true，否则在下次开启时不会停止会议，会开启失败
        m_pCallInfo->SetTalkStart(true);
    }

    return true;
}
#endif
// 盲转失败进入recall
void CTalkRoutine::EnterRecall()
{
    CancelTimer(m_iWaitTranResultTimeout);
    m_pCallSession->SetHide(false);
    _TalklogicManager.TryEnterRoutine(m_pCallSession, ROUTINE_RECALL);

}

bool CTalkRoutine::ProcessSIPHold(msgObject& msg)
{
    int iState = 0;
    if (!Comm_GetSipCodeState(msg, iState))
    {
        return false;
    }

    // 如果音频信息有改变，则处理
    ipvp_speech_t voiceParam;
    memset(&voiceParam, 0, sizeof(ipvp_speech_t));

    sip_channels_v20_t sipChannelInfo;
    Comm_GetSipChannelsData(sipChannelInfo, msg);
    Comm_MediaParam_sip2ipvp(&sipChannelInfo, m_pCallInfo->m_strUserAgent, &voiceParam,
                             &m_pCallInfo->m_stVideoExt);
    Comm_AudioCodec_sip2ipvp(m_pCallInfo->m_stMediaInfo.m_stAudio, &voiceParam);

    //如果音频信息不为空,通知出去 // yzh sk 确认优化
    ipvp_speech_t voiceTempParam;
    memset(&voiceTempParam, 0, sizeof(ipvp_speech_t));
    bool bVoiceParamEmpty = (0 == memcmp(&voiceTempParam, &voiceParam, sizeof(ipvp_speech_t)));
    bool bChanged = false;

    // 本地没有hold的情况下才改变通话状态
    if (!m_pCallInfo->IsLocalHold())
    {
        if (m_pCallInfo->IsRemoteHold())
        {
            bChanged = true;
            if (!bVoiceParamEmpty)
            {
                ReStartTalk(&voiceParam);
            }
            else
            {
#if IF_BUG_20944
                memcpy(&voiceTempParam, &m_pCallInfo->m_tVPMParam, sizeof(ipvp_speech_t));
                voiceTempParam.audio.flag = IPVP_ADDR_FL_SENDRECV;
                ReStartTalk(&voiceTempParam);
#else
                // http://10.2.1.199/Bug.php?BugID=21340
                //voice_Hold(m_pCallInfo->GetCallID());
#endif
            }
        }
    }

    BOOL bLocalHold = (0 != (SIP_CALL_HOLD_BY_LOCAL & iState));
    if (bLocalHold != m_pCallInfo->IsLocalHold())
    {
        bChanged = true;
        // 如果sip同步过来的状态是hold，而gui状态不是，则要重新hold一次，
        // 防止hold,resume过快，导致后面的resume不生效
        HoldRoutine(bLocalHold);
    }
    else
    {
        if (ProcessDelayOperate())
        {
            bChanged = true;
        }
    }

    return bChanged;
}

// 判断是否是talkroutine,包含外接设备话路
bool CTalkRoutine::IsTalkRoutine(ROUTINE_STATE eState)
{
    return (ROUTINE_TALKING == eState
#ifdef IF_SUPPORT_LINE_DEVICE
            || ROUTINE_LINE_DEVICE == eState
#endif
           );
}

// 合并所有允许建立网络会议话路
bool CTalkRoutine::MergeAllowJoinNetConfRoutine()
{
    YLVector<int> vecSessionId;

    _SessionManager.GetAllSessionID(vecSessionId);

    // 要加入会议中的双方的CallID
    int iConfCallID[CONFCALLNUM] = {0};

    int nCount = 0;

    CCallInfo* pConfInfo = NULL;
    int iAccount = m_pCallInfo->GetAccountID();

    for (int i = 0; i < vecSessionId.size(); i++)
    {
        CSingleSession* pSession = _SessionManager.GetSingleSession(vecSessionId[i]);
        if (pSession == NULL)
        {
            continue;;
        }

        CCallInfo* pCallInfo = pSession->GetCallInfo();
        if (pCallInfo->GetAccountID() != iAccount)
        {
            continue;
        }

        if (pCallInfo->m_eStateOfRoutine == ROUTINE_NETWORKCONF)
        {
            pConfInfo = pCallInfo;
            continue;
        }

        if (pSession->IsInherits(ROUTINE_TALKING)
                || !pSession->IsAllowAddToConf())
        {
            continue;
        }

        iConfCallID[nCount] = pCallInfo->GetCallID();

        TALK_INFO("CTalkRoutine::EstablishNetworkConf iConfCallID[%d]: %d", nCount, iConfCallID[nCount]);

        pCallInfo->m_bAddToNetworkConf = true;

        // 将加入会议的两路隐藏
        pSession->SetHide();

        if (nCount + 1 > CONFCALLNUM)
        {
            break;
        }

        nCount++;
    }

    if ((nCount == 1 && pConfInfo == NULL) || (nCount == 0 && pConfInfo != NULL))
    {
        CSingleSession* pSession = _SessionManager.GetRelateSessionByCallID(iConfCallID[0]);
        if (pSession != NULL)
        {
            pSession->SetHide(false);

            pSession->GetCallInfo()->m_bAddToNetworkConf = false;
        }

        EstablishNewDial(true);
        return false;
    }

    if (pConfInfo != NULL)
    {
        for (int i = 0; i < nCount; i++)
        {
            CBaseSession* pSession = _SessionManager.GetSessionByCallID(iConfCallID[i]);

            if (pSession != NULL)
            {
                pSession->SetHide(false);

                modVoip_AddToNetworkConf(iAccount, pConfInfo->GetCallID(), iConfCallID[i]);
                TALK_INFO("Add to Conference ConfId[%d],addId[%d]", pConfInfo->GetCallID(), iConfCallID[i]);
            }
        }
    }
    else
    {
        // 停掉当前通话，此处不能给sip发送hold消息，否则建立网络会议后会有问题
        StopTalk();

        CSingleSession* pConfSession = _SessionManager.AllocSession();
        if (pConfSession == NULL)
        {
            return false;
        }

        // 设置网络会议的通话信息
        CCallInfo* pCallInfo = pConfSession->GetCallInfo();
        pCallInfo->SetAccountID(iAccount);
        pCallInfo->m_tRemoteInfo.sDisplayName = _LANG(TRID_CONFERENCE);
        pCallInfo->m_tRemoteInfo.sSIPName = _LANG(TRID_CONFERENCE);
        pCallInfo->m_tRemoteInfo.sFullContactInfo = _LANG(TRID_CONFERENCE);

        memcpy(pCallInfo->m_iConfCallID, iConfCallID, sizeof(int) * CONFCALLNUM);
        pConfSession->EnterRoutine(ROUTINE_NETWORKCONF_CONNECT);
    }

    return true;
}

bool CTalkRoutine::Redirectcall(LPCSTR lpTarget)
{
    if (textEMPTY(lpTarget))
    {
        return false;
    }

    // hold当前话路
    HoldRoutine(true);
    BlindTransfer(lpTarget);
    return true;
}

bool CTalkRoutine::ProcessDelayOperate()
{
    m_bWaitForHold = false;
    bool bHandle = true;
    switch (m_eDelayOperate)
    {
    case TDO_BLIND_TRANSFER:
        {
            if (m_pCallInfo->IsLocalHold())
            {
                if (!m_strTransferNum.empty())
                {
                    BlindTransfer(m_strTransferNum);
                }
            }
        }
        break;
    default:
        bHandle = false;
        break;
    }

    if (bHandle)
    {
        m_eDelayOperate = TDO_NONE;
    }
    return bHandle;
}

bool CTalkRoutine::IsAllowAddToConf()
{
    if ((m_pCallInfo->IsRemoteHold() && m_pCallInfo->GetConfType() == CT_BROADSOFT)
#if IF_FEATURE_PAGING
            || m_pCallInfo->BeMulticastPage()
#endif
            || m_pCallInfo->IsTransferred()
       )
    {
        // 如果在半咨询tran过程中，则不允许conf
        return false;
    }

    return true;
}
