#include "talklistener_inc.h"

CVoiceListener::CVoiceListener()
    : LSN_CLS_LOG_NAME
{
    //routine create
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_ROUTINE_CHANGED, CVoiceListener::OnSessionPreRoutineChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_AFTER_CREATE, CVoiceListener::OnSessionCreate);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CVoiceListener::OnSessionRoutineChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_FOCUS_CHANGE, CVoiceListener::OnSessionFocusChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ACTIVED, CVoiceListener::OnSessionFocusChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_START_TALK, CVoiceListener::OnSessionPreStartTalk);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_AFTER_START_TALK, CVoiceListener::OnSessionAftStartTalk);
    REG_SYNC_SIGNAL(TALK_SIG_ACTION_EVENT, CVoiceListener::OnActionEvent);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HELD_CHANGE, CVoiceListener::OnHeldChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HOLD_CHANGE, CVoiceListener::OnHoldChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_INFO_CHANGED, CVoiceListener::OnDataChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PLAY_RINGING, CVoiceListener::OnPlayRing);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PLAY_BUSY_TONE, CVoiceListener::OnPlayBusyTone);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_DIAL_TONE, CVoiceListener::OnPlayDialTone);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PLAY_CONF_TONE, CVoiceListener::OnPlayConfTone);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ATTENDED_TRANSFER_FAIL, CVoiceListener::OnSessionAttendedTransferFail);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ANSWER, CVoiceListener::OnAnswer);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_RESET_TALK, CVoiceListener::OnResetTalk);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_TEXT_CHANGE, CVoiceListener::OnTextChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CVoiceListener::OnSessionDelete);
    REG_SYNC_SIGNAL(TALK_FLT_SESSION_INCOME, CVoiceListener::OnCallIncome);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CVoiceListener::OnRoutineChanged);

    RegETLMsg(SIP_CALL_RINGBACK);
    RegETLMsg(IPVP_MSG_AUDIO_FIRST_RTP);
    RegETLMsg(IPVP_MSG_AUDIO_DEAD);
#ifdef IF_FEATURE_PSTN
    RegETLMsg(PSTN_MSG_INCOMING_CID);
#endif
}


bool CVoiceListener::OnMessage(msgObject & msg)
{
    bool bRet = true;
    switch (msg.message)
    {
    case SIP_CALL_RINGBACK:
        {
            ProcessRingbackSIPMsg(msg);
        }
        break;
    case IPVP_MSG_AUDIO_FIRST_RTP:
    case IPVP_MSG_AUDIO_DEAD:
        {
            TALK_INFO("IPVP_MSG_AUDIO_FIRST_RTP [%d] [%d]", msg.wParam, msg.lParam);
            CSessionProxy sessionProxy(talklogic_GetSessionIdByCallID(msg.wParam));
            if (sessionProxy.GetRoutineState() == ROUTINE_RINGBACK)
            {
                CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(msg.wParam);
                if (pCallInfo)
                {
                    ProcessIPVPMsg(*pCallInfo, msg);
                }
            }
        }
        break;
#ifdef IF_FEATURE_PSTN
    case  PSTN_MSG_INCOMING_CID:
        {
            CidMsgInfo* pExt = (CidMsgInfo*)msg.GetExtraData();
            if (pExt == NULL)
            {
                return false;
            }
            const char* szNumber = pExt->m_szNumber;
            int iCallId = msg.lParam;
            CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(iCallId);
            if (pCallInfo != NULL)
            {
                // 存在对应Session则修改通话信息
                TALK_INFO("Update call number of pstn: %s", szNumber);
                pCallInfo->m_tRemoteInfo.sSIPName = pCallInfo->m_tRemoteInfo.sFromSIPName = szNumber;
                yl::string strContactName = GetNameFromContact(szNumber, pCallInfo->m_eCallType);
                // 如果无法找到联系人（字符串为空）则自动使用SIPName字段
                pCallInfo->m_tRemoteInfo.sDisplayName = strContactName;
                // 更新铃声
                if (Comm_IsRoutineInherits(ROUTINE_RINGING, pCallInfo->GetRoutine()))
                {
                    // TODO
                    //pRingRoutine->UpdateIncomingCallRingByNumber(szNumber);
                    PlayRing(*pCallInfo);
                }
            }
        }
#endif
    default:
        bRet = false;
        break;
    }
    return bRet;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnCallIncome)
{
    // 退出自动拨号
    CSessionStateProxy ssP(sigp.iSessionID);
    // 退出AutoRedial状态的话路
    talklogic_ExitSession(ROUTINE_AUTOREDIAL);
    // 停止重拨
    _AutoRedialManager.StopAutoRedial();
    // 退出测试模式
    if (AdapterBox_IsTestMode())
    {
        //msgSendMsgToThread(msgGetThreadByName(VP_TEST_THREAD_NAME), TEST_MSG_EXIT, 0, 0);
        //commonAPI_ExitTestMode();
        AdapterBox_ExitTestMode();
        // 重置声音通道
        voice_ResetChannel();
    }
    return false;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnSessionDelete)
{
    //不存在可显示的session 时
    if (talklogic_GetSessionNum() == 1)
    {
        // 不存在任何session才重置通话音量
        voice_ResetTalkVolume();
        voice_SwitchChannel(CHANNEL_IDLE);
        voice_CloseEHS();
#ifdef IF_SUPPORT_BLUETOOTH
        // 通话结束时关闭蓝牙通道，避免因长时间占用蓝牙通道导致耗电严重
        voice_CloseBluetoothChannel();
        Bluetooth_HSStopPlay();
#endif // IF_SUPPORT_BLUETOOTH
        return false;
    }
    return true;
}

#ifdef IF_FEATURE_BROADSOFT_CALLPARK
DECL_SIGNAL_HANDLER(CVoiceListener::OnParkNotifyRoutineDestroy)
{
    CSessionStateProxy ssP(sigp.iSessionID);
    ROUTINE_STATE eOldState = (ROUTINE_STATE)sigp.wParam.iValue;
    ROUTINE_STATE eNewState = (ROUTINE_STATE)sigp.lParam.iValue;
    bool bEnterNextRoutine = (ROUTINE_UNKNOWN != eNewState);
    CCallInfo* pCallInfo = ssP.GetCallInfo();
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }
    switch (eOldState)
    {
    case ROUTINE_PARK_NOTIFY:
        {
            // 取消Notify提示
            priv->m_VoiceInfo.timerParkNotify.Kill();
        }
        break;
    default:
        return false;
        break;
    }
    return true;
}
#endif

DECL_SIGNAL_HANDLER(CVoiceListener::OnDialRoutineDestroy)
{
    CSessionStateProxy ssP(sigp.iSessionID);
    ROUTINE_STATE eOldState = (ROUTINE_STATE)sigp.wParam.iValue;
    ROUTINE_STATE eNewState = (ROUTINE_STATE)sigp.lParam.iValue;
    bool bEnterNextRoutine = (ROUTINE_UNKNOWN != eNewState);
    CCallInfo* pCallInfo = ssP.GetCallInfo();
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }
    switch (eOldState)
    {
    case ROUTINE_DIALING:
    case ROUTINE_PREDIAL:
    case ROUTINE_RETRIEVE_CALLPARK:
    case ROUTINE_PRETRAN:
        {
            // http://10.2.1.199/Bug.php?BugID=94321
            CancelBusyTone(ssP, priv->m_VoiceInfo);
        }//该处不break;
    case ROUTINE_BTDIALING:
    case ROUTINE_BTPREDIAL:
        {
            CancelPlayDialTone(priv->m_VoiceInfo);
            // 退出拨号界面时，停止播放拨号音
            StopDialTone();
            CancelMSGTone(priv->m_VoiceInfo);
        }
        break;
    default:
        return false;
        break;
    }
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnFinishRoutineDestroy)
{
    CSessionStateProxy ssP(sigp.iSessionID);
    ROUTINE_STATE eOldState = (ROUTINE_STATE)sigp.wParam.iValue;
    ROUTINE_STATE eNewState = (ROUTINE_STATE)sigp.lParam.iValue;
    bool bEnterNextRoutine = (ROUTINE_UNKNOWN != eNewState);
    CCallInfo* pCallInfo = ssP.GetCallInfo();
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }
    switch (eOldState)
    {
    case ROUTINE_FINISHED:
    case ROUTINE_BTFINISHED:
    case ROUTINE_AUTOREDIAL:
        {
            // http://10.2.1.199/Bug.php?BugID=37754
            // http://10.2.1.199/Bug.php?BugID=38136
            // 停止信号音
            if (ssP.IsFocusSession())
            {
                if (ssP.IsBTTalk())
                {
                    voice_StopTone(TONE_BUSY);
                }
                else
                {
                    voice_StopTone(priv->m_VoiceInfo.m_eToneType);
                }
            }
            // 杀掉响忙音时间的定时器
            priv->m_VoiceInfo.timerBusyTone.Kill();
        }
        break;
    default:
        return false;
        break;
    }
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnConnectRoutineDestroy)
{
    CSessionStateProxy ssP(sigp.iSessionID);
    ROUTINE_STATE eOldState = (ROUTINE_STATE)sigp.wParam.iValue;
    ROUTINE_STATE eNewState = (ROUTINE_STATE)sigp.lParam.iValue;
    bool bEnterNextRoutine = (ROUTINE_UNKNOWN != eNewState);
    CCallInfo* pCallInfo = ssP.GetCallInfo();
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }
    switch (eOldState)
    {
    case ROUTINE_RINGBACK:
        {
            StopRingbackTone(*pCallInfo);
            // 退出回铃将不再应用该变量
            pCallInfo->m_bPlay182Tone = false;
            // 等到退出RingbackRoutine时才将该标志置为false，否则当两路通话切换焦点时，彩铃会变成普通回铃
            pCallInfo->m_bIsColorRingback = false;
            //关闭回铃声定时器
            priv->m_VoiceInfo.timerMadiaSniffer.Kill();
        }
        break;
    default:
        return false;
        break;
    }
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnRingingRoutineDestroy)
{
    CSessionStateProxy ssP(sigp.iSessionID);
    ROUTINE_STATE eOldState = (ROUTINE_STATE)sigp.wParam.iValue;
    ROUTINE_STATE eNewState = (ROUTINE_STATE)sigp.lParam.iValue;
    bool bEnterNextRoutine = (ROUTINE_UNKNOWN != eNewState);
    CCallInfo* pCallInfo = ssP.GetCallInfo();
    if (priv == NULL || pCallInfo == NULL)
    {
        return false;
    }
    VoiceInfo& vInfo = priv->m_VoiceInfo;
    switch (eOldState)
    {
    case ROUTINE_RINGING:
    case ROUTINE_RECALL:
        {
            vInfo.timerRing.Kill();
            CancelIntercomToneTimer(vInfo);
            // 删除临时下载的远程铃声
            if (vInfo.m_strRingPath.find(TEMP_PATH) != yl::string::npos)
            {
                unlink(vInfo.m_strRingPath.c_str());
            }
        }//该处不break
    case ROUTINE_BTRINGING:
        {
            // 停止来电铃声,进入通话会打开EHS,从而关闭响铃,不需要强制关闭
            voice_StopRing(!bEnterNextRoutine);
            //routine 实际已经是销毁了所以直接判断当前是否存在ringroutine
            if (!talklogic_IsRingSessionExist() || !IsNeedPlayCallWaitingTone())
            {
                // 停止播放callwaitingtone
                StopCallWaitingTone(eOldState == ROUTINE_BTRINGING);
            }
            if (vInfo.timerRing.IsOn())
            {
                vInfo.timerRing.Kill();
            }
        }
        break;
    default:
        return false;
        break;
    }
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnTalkingRoutineDestroy)
{
    CSessionStateProxy ssP(sigp.iSessionID);
    ROUTINE_STATE eOldState = (ROUTINE_STATE)sigp.wParam.iValue;
    ROUTINE_STATE eNewState = (ROUTINE_STATE)sigp.lParam.iValue;
    bool bEnterNextRoutine = (ROUTINE_UNKNOWN != eNewState);
    CCallInfo* pCallInfo = ssP.GetCallInfo();
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }
    switch (eOldState)
    {
    case ROUTINE_TALKING:
    case ROUTINE_NETWORKCONF:
    case ROUTINE_LINE_DEVICE:
        {
            // 杀掉播放hold提示音的定时器
            CancelHoldAlertTimer(*pCallInfo);
            // 退出tran失败提示
            ExitTransFailedAlert(*pCallInfo);
#ifdef IF_SUPPORT_LINE_DEVICE
            if (eOldState == ROUTINE_LINE_DEVICE)
            {
                // 关闭多模的声道
                voice_SetLineDeviceChannel(IPVP_FL_DIR_NONE);
            }
#endif
            //关闭回铃声定时器
            priv->m_VoiceInfo.timerMadiaSniffer.Kill();
        }
        break;
    case ROUTINE_BTTALKING:
        {
            // 杀掉播放hold提示音的定时器
            CancelHoldAlertTimer(*pCallInfo);
        }
        break;
    default:
        return false;
        break;
    }
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnTextChange)
{
    CCallInfo* pCallInfo = CSessionProxy(sigp.iSessionID).GetCallInfo();
    if (priv == NULL || pCallInfo == NULL)
    {
        return false;
    }

    //MSGTone播放时间可配，输入号码改变时，停掉MSG Tone播dial tone
    CancelMSGTone(priv->m_VoiceInfo);
    // 判断是否需要播放拨号音
    PlayDialTone(*pCallInfo);
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnSessionCreate)
{
    if (talklogic_GetSessionNum() == 1)
    {
#if defined(IF_SUPPORT_BLUETOOTH) && defined(IF_BT_SUPPORT_A2DP)
        // 须同步调用, 不然拨号会有杂音
        Bluetooth_StopPlayMusic(true);
#endif
        // 增加Session后切换通道（如果是通道键引发的则不用切换，此时通道已不是CHANNEL_IDLE）
        if (voice_GetCurrentChannel() == CHANNEL_IDLE || voice_GetCurrentChannel() == CHANNEL_NULL)
        {
            voice_SwitchChannel();
        }
#if (defined IF_SUPPORT_BLUETOOTH)
        // 开启通话时需要打开蓝牙通道
        if (voice_IsBluetoothMode())
        {
            voice_OpenHeadsetDeviceChannel();
        }
#endif // IF_SUPPORT_BLUETOOTH
        return true;
    }
    return false;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnResetTalk)
{
    if (!sigp.wParam.bValue)
    {
        // 设置一下音量 http://10.2.1.199/Bug.php?BugID=86474
        voice_SetVolume(voice_GetVolume());
    }
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnAnswer)
{
    CSessionProxy proxy(sigp.iSessionID);
    if (!proxy.GetHost())
    {
        // 停止来电铃声，停止铃声后恢复之前的通道 http://10.2.1.199/Bug.php?BugID=18697
        voice_StopRing(false);
    }
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnSessionAttendedTransferFail)
{
    if (priv != NULL)
    {
        // Tran失败，可恢复与之前话路的通话
        priv->SetTransFailed(true);
    }
    CSessionStateProxy ssP(sigp.iSessionID);
    AlertForTransFailed(ssP);
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnPlayConfTone)
{
    voice_PlayTone(TONE_CONF, sigp.wParam.iValue, (CHANNEL_MODE)sigp.lParam.iValue, sigp.tParam.iValue);
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnPlayDialTone)
{
    CSessionProxy sp(sigp.iSessionID);
    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(sigp.iCallID);
    if (pCallInfo && sp.IsInherits(ROUTINE_DIALING))
    {
        PlayDialTone(*pCallInfo);
    }
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnPlayBusyTone)
{
    CSessionProxy sp(sigp.iSessionID);
    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(sigp.iCallID);
    if (pCallInfo && sp.IsInherits(ROUTINE_DIALING))
    {
        SetBusyToneDelay(*pCallInfo);
    }
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnPlayRing)
{
    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(sigp.iCallID);
    if (pCallInfo)
    {
        PlayRing(*pCallInfo);
    }
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnDataChange)
{
    bool bRet;
    switch (sigp.wParam.iValue)
    {
    case TALK_CALL_INFO_RING_MUTE:
        {
            //原 TALK_ACTION_SILENCE
            //原 TALK_ACTION_KEYPRESS:
            //原 TALK_ACTION_SCREEN_CLICK:
            CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(sigp.iCallID);
            if (sigp.lParam.bValue)
            {
                StopRing(*pCallInfo);
            }
            else if (!sigp.lParam.bValue && !pCallInfo->m_bPassiveRingMute)
            {
                //原 TALK_ACTION_VOLUME_CHANGE
                PlayRing(*pCallInfo);
            }
        }
        break;
    default:
        bRet = false;
        break;
    }
    return bRet;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnHeldChange)
{
    CCallInfo* pCallInfo = CSessionProxy(sigp.iSessionID).GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }
    SetHoldAlertTimer(*pCallInfo);
    return true;;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnHoldChange)
{
    CSessionStateProxy sessionSp(sigp.iSessionID);
    CCallInfo* pCallInfo = sessionSp.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    CSessionProxy hostSessionProxy = sessionSp.GetHost();
    if (sigp.lParam.bValue)
    {
        if (!hostSessionProxy
                || hostSessionProxy.GetSubByIndex(0) == sessionSp)
        {
            SetHoldAlertTimer(*pCallInfo);
        }
    }
    else
    {
        SetHoldAlertTimer(*pCallInfo);
    }
    return true;;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnActionEvent)
{
    DataUI2Logic* pActionData = (DataUI2Logic*)sigp.wParam.pValue;
    if (pActionData == NULL)
    {
        return false;
    }
    CSessionStateProxy sessionSp(sigp.iSessionID);
    CCallInfo* pCallInfo = sessionSp.GetCallInfo();
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }
    bool bRet = false;
    switch (pActionData->eAction)
    {
    case TALK_ACTION_ANSWER:
        {
            if (!sessionSp.IsInherits(ROUTINE_RINGING_CLS))
            {
                break;
            }
            // 如果已经应答,则不再处理
            if (pCallInfo->m_bAnswered)
            {
                break;
            }
            bool bSwitchChannel = false;
            if (sessionSp.GetRoutineState() == ROUTINE_BTRINGING)
            {
                // 如果是通道键引发的接起，则不切换通道
                bSwitchChannel = (pActionData->wActionParam == 0);
                if (pActionData->wActionParam == 1 && configParser_GetConfigInt(kszAnswerByHand) == 0)
                {
                    break;
                }
            }
            else
            {
                TALK_ANSWER_TYPE eAnwserType = (TALK_ANSWER_TYPE)(pActionData->wActionParam);
                // 如果是通道键引发的接起，则不切换通道
                bSwitchChannel = (eAnwserType == TAT_NONE || eAnwserType == TAT_AUDIO);
                if (eAnwserType == TAT_CHANNEL)
                {
                    int iKey = pActionData->lActionParam;
#if IF_FEATURE_KTS
                    if (sessionSp.IsKTSIgnore())
                    {
                        // 该部分在talkadaptermanager 中处理
                        break;
                    }
#endif
                    if (configParser_GetConfigInt(kszAnswerByHand) == 0)
                    {
                        break;
                    }
                    // 耳麦键接起来电时，需要将通道切到耳麦，比如耳麦通话中新来电，按耳麦接起
                    if (iKey == PHONE_KEY_HEADSET_CTRL)
                    {
                        if (voice_IsCallCenterMode() && voice_GetHandsetStatus())
                        {
                            // 摘机状态打开双耳麦模式
                            voice_SwitchChannel(CHANNEL_2HEADSET);
                        }
                        else
                        {
                            voice_SwitchChannel(CHANNEL_HEADSET);
                        }
                    }
                    // 免提接起，如果通道不在免提，需要切换通道
                    else if (iKey == PHONE_KEY_HANDFREE && !voice_GetHandfreeStatus())
                    {
                        voice_SwitchChannel(CHANNEL_HANDFREE);
                    }
                }
#if IF_BUG_27883
                // 为防止按OK键接起通话因关闭铃声而关闭耳麦，造成耳麦通话因重启出现声音延时问题，此处先开启耳麦
                if (bSwitchChannel && CHANNEL_AUTO == priv->m_eAAChannel)
                {
                    voice_OpenEHS();
                }
#endif
            }
            // 停止来电铃声，停止铃声后恢复之前的通道 http://10.2.1.199/Bug.php?BugID=18697
            voice_StopRing(false);
            if (bSwitchChannel)
            {
                // http://10.2.1.199/Bug.php?BugID=11265
                // 当前存在通话,新来电的通道使用第一路的通道
                if (talklogic_IsAllSessionRinging())
                {
#if IF_BUG_26732
                    voice_SetDelayOpenChannel(true);
#endif
                    // 非通道键接起来电，则要切换通道，此时不更改音量，等到建立通话才设置
                    voice_SwitchChannel(priv->m_eAAChannel);
                }
            }
        }
        break;
    case TALK_ACTION_JOIN:
        {
#if !IF_FEATURE_NFON_RTP
            // 如果已经应答,则不再处理
            if (pCallInfo->m_bAnswered)
            {
                break;
            }
            // 停止来电铃声，停止铃声后恢复之前的通道 http://10.2.1.199/Bug.php?BugID=18697
            voice_StopRing(false);
#endif
        }
        break;
    case TALK_ACTION_SCREEN_CLICK:
    case TALK_ACTION_EXIT_FAILD_INFO:
        {
            if (sessionSp.GetRoutineState() == ROUTINE_TALKING)
            {
                // 退出Tran失败时的提示
                if (priv->IsTransFailed())
                {
                    ExitTransFailedAlert(*pCallInfo);
                }
            }
        }
        break;
    case TALK_ACTION_PLAY_TONE:
        {
            if (sessionSp.IsInherits(ROUTINE_DIALING_CLS))
            {
                PlayDialTone(*pCallInfo, (TONE_TYPE)pActionData->wActionParam);
            }
            else if (sessionSp.IsInherits(ROUTINE_RINGING_CLS))
            {
                LPCSTR lpPath = (LPCSTR)pActionData->pData;
                if (lpPath == NULL)
                {
                    return false;
                }
                int iStatus = pActionData->wActionParam;
                TALK_INFO("Play ring path[%s][%s] status[%d][%d]",
                          priv->m_VoiceInfo.m_strRingPath.c_str(), lpPath, priv->m_VoiceInfo.m_iRingStatus, iStatus);

                if (iStatus == TALK_RING_LS_FAILED)
                {
                    /* 如果异步下载失败了，则播放本地铃声 */
                    if (priv->m_VoiceInfo.m_iRingStatus == TALK_RING_LS_LOADING)
                    {
                        iStatus = TALK_RING_LS_LOADED;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    priv->m_VoiceInfo.m_strRingPath = lpPath;
                }
                priv->m_VoiceInfo.m_iRingStatus = iStatus;
                // 当前session是焦点，则播放铃声
                if (sessionSp.IsFocusSession())
                {
                    PlayRing(*pCallInfo);
                }
            }
        }
        break;
    case TALK_ACTION_SEND:
    case TALK_ACTION_OK:
        {
            // 如果当前为提示界面,需更改为倒计时界面
            if (AUTOREDIAL_TIP == _AutoRedialManager.GetStatus())
            {
                priv->m_VoiceInfo.timerAutoRedial.Kill();
                // 停止提示音
                voice_StopTone(TONE_BUSY);
            }
        }
        break;
    case TALK_ACTION_GROUP_LISTEN:
        {
            if (!sessionSp.IsHold() && !voice_ProcessChannelKeyAccess(PHONE_KEY_HANDFREE))
            {
                voice_SwitchChannel(CHANNEL_GROUP);
            }
        }
        break;
    default:
        bRet = false;
        break;
    }
    return bRet;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnRoutineDelete)
{
    //session delete情况 或者 切换routine情况处理
    ROUTINE_STATE eDestoryState = (ROUTINE_STATE)sigp.wParam.iValue;
    if (Comm_IsRoutineInherits(ROUTINE_DIALING_CLS, eDestoryState))
    {
        CALL_HANDLER(OnDialRoutineDestroy);
    }
    else if (Comm_IsRoutineInherits(ROUTINE_FINISHED_CLS, eDestoryState))
    {
        CALL_HANDLER(OnFinishRoutineDestroy);
    }
    else if (Comm_IsRoutineInherits(ROUTINE_TALKING_CLS, eDestoryState))
    {
        CALL_HANDLER(OnTalkingRoutineDestroy);
    }
    else if (Comm_IsRoutineInherits(ROUTINE_CONNECTING_CLS, eDestoryState))
    {
        CALL_HANDLER(OnConnectRoutineDestroy);
    }
    else if (Comm_IsRoutineInherits(ROUTINE_RINGING_CLS, eDestoryState))
    {
        CALL_HANDLER(OnRingingRoutineDestroy);
    }
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    else if (Comm_IsRoutineInherits(ROUTINE_PARK_NOTIFY, eDestoryState))
    {
        CALL_HANDLER(OnParkNotifyRoutineDestroy);
    }
#endif
    else
    {
        return false;
    }
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnSessionAftStartTalk)
{
    return false;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnSessionPreStartTalk)
{
    //进入通话前需要停止铃声
    // 停止提示音
    voice_StopTone();
    // 停止响铃,进入通话会打开EHS,从而关闭响铃,不需要强制关闭
    voice_StopRing(false);

    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnSessionPreRoutineChange)
{
    CSessionStateProxy ssP(sigp.iSessionID);
    if (priv == NULL)
    {
        return false;
    }

    VoiceInfo& vInfo = priv->m_VoiceInfo;
    ROUTINE_STATE eCreateState = (ROUTINE_STATE)sigp.lParam.iValue;
    ROUTINE_STATE eDestoryState = (ROUTINE_STATE)sigp.wParam.iValue;
    bool bRet = true;

    CALL_HANDLER(OnRoutineDelete);
    if (eCreateState != ROUTINE_UNKNOWN)
    {
        switch (eCreateState)
        {
        case ROUTINE_RINGING:
            {
                if (talklogic_GetSessionNum() == 1)
                {
#if (defined IF_SUPPORT_BLUETOOTH)
                    // !!!T5XS由于芯片问题,创建SCO通道后会导致其他指令响应慢
                    // 由GUI暂时规避,来电不创建SCO,进入talkroutine后创建
                    // ---- 后续有条件,应该fix
                    Bluetooth_HSStartPlay();
#endif // IF_SUPPORT_BLUETOOTH
                }
            }
        case ROUTINE_RECALL:
        case ROUTINE_BTRINGING:
            {
                // 如果都是响铃界面，则切换通道
                if (talklogic_IsAllSessionRinging())
                {
                    // 切换通道前先停止信号音
                    voice_StopTone();
                    voice_SwitchChannel(CHANNEL_RING);
                }
            }
            break;
        case ROUTINE_TALKING:
        case ROUTINE_BTTALKING:
        case ROUTINE_LINE_DEVICE:
            {
                // 调整EHS的状态
                voice_SwitchEHSStatus();
            }
            break;
        default:
            break;
        }
    }
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnSessionRoutineChange)
{
    CSessionStateProxy ssP(sigp.iSessionID);
    CCallInfo* pCallInfo = ssP.GetCallInfo();
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }
    VoiceInfo& vInfo = priv->m_VoiceInfo;
    ROUTINE_STATE eOldState    = (ROUTINE_STATE)sigp.wParam.iValue;
    ROUTINE_STATE eCreateState = (ROUTINE_STATE)sigp.lParam.iValue;
    switch (eCreateState)
    {
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    case ROUTINE_PARK_NOTIFY:
        {
            if (configParser_GetConfigInt(kszCallParkRingNotifyType) != 0
                    && configParser_GetConfigInt(kszBroadsoftActive) != 0)
            {
                SetNotifyAlertTimer(*pCallInfo);
            }
        }
        break;
#endif
    case ROUTINE_TALKING:
        {
            if (eOldState == ROUTINE_RINGING
                    && !ssP.GetHost()
                    && ssP.IsFocusSession())
            {
                //原 m_iDelayToEnterInTalk 触发时操作
                // 停止提示音
                voice_StopTone();
                // 停止响铃,进入通话会打开EHS,从而关闭响铃,不需要强制关闭
                voice_StopRing(false);
            }
            else if (eOldState == ROUTINE_RECALL
                     && !ssP.GetHost()
                     && ssP.IsFocusSession())
            {
                //http://10.2.1.199/Bug.php?BugID=79755 特殊处理，重复设置一下音量
                voice_SetVolume(voice_GetVolume());
            }
            else if (eOldState == ROUTINE_RINGBACK)
            {
                StopRingbackTone(*pCallInfo);
            }
        }
        break;
    case ROUTINE_BTTALKING:
        {
            // 停止提示音
            voice_StopTone();
            // 停止响铃,进入通话会打开EHS,从而关闭响铃,不需要强制关闭
            voice_StopRing(false);
            //手机端接起时，需要切换通道
            // 当前存在通话,新来电的通道使用第一路的通道
            if (!pCallInfo->m_bAnswered && talklogic_IsAllSessionRinging())
            {
                // 非通道键接起来电，则要切换通道，此时不更改音量，等到建立通话才设置
                voice_SwitchChannel(priv->m_eAAChannel);
            }
        }
        break;
    case ROUTINE_BTCONNECTING:
        {
            // 来电界面按Dsskey去截答新呼出一路需要切换通道
            if (voice_GetCurrentChannel() == CHANNEL_RING)
            {
                voice_StopRing();
                voice_SwitchChannel();
            }
            else
            {
                // http://192.168.1.99/Bug.php?BugID=20668
                voice_SwitchEHSStatus();
            }
        }
        break;
    case ROUTINE_DIALING:
    case ROUTINE_RETRIEVE_CALLPARK:
    case ROUTINE_PRETRAN:
        {
            // 新短息提示
            SetMSGTone(*pCallInfo);
        }
        break;
    case ROUTINE_FINISHED:
    case ROUTINE_BTFINISHED:
        {
            SetToneType(*pCallInfo);
            // 设置响忙音时间定时器
            SetToneDelay(*pCallInfo);
            pCallInfo->m_bPlay182Tone = false;
        }
        break;
    case ROUTINE_NETWORKCONF_CONNECT:
    case ROUTINE_BLACONNECTING:
        {
            // 来电界面按Dsskey去截答新呼出一路需要切换通道
            if (voice_GetCurrentChannel() == CHANNEL_RING)
            {
                voice_StopRing();
                voice_SwitchChannel();
            }
            else
            {
                // http://192.168.1.99/Bug.php?BugID=20668
                voice_SwitchEHSStatus();
            }
        }
        break;
    case ROUTINE_RINGBACK:
        {
            //初始化
            ProcessMediaSniffer(*pCallInfo);
        }
        break;
#ifdef IF_BT_SUPPORT_PHONE
    case ROUTINE_BTRINGING:
        {
            GetIncomingCallRing(*pCallInfo);
        }
        //该处刻意不break
#endif
    case ROUTINE_RINGING:
    case ROUTINE_RECALL:
        {
            if (eCreateState == ROUTINE_RINGING)
            {
                // 播放IntercomTone
                SetIntercomToneTimer(*pCallInfo);
            }
            // 获取来电响铃铃声类型
            GetCallWaitingToneType(*pCallInfo);
            if (IsNeedPlayCallWaitingTone())
            {
                // http://10.2.1.199/Bug.php?BugID=31451
                // 网页Settings->Tones可以配置TONE_CALLWAIT，为防止网页设置Tone音循环与本地循环冲突，故播放CALLWAIT1时不设置本地循环
                if (ssP.GetRoutineState() == ROUTINE_RECALL
                        || vInfo.eCallWaitingTone != TONE_CALLWAIT)

                {
                    SetCallWaitingToneTimer(ssP);
                }
                // 当前只能有一路能设响铃定时器, 只有所有话路都为通话或来电,才能响铃
                PlayCallWaitingTone(*pCallInfo);
            }
        }
        break;
    default:
        break;
    }
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnSessionFocusChange)
{
    CSessionProxy sp(sigp.iSessionID);
    if (sigp.wParam.bValue)
    {
        //获取焦点
        OnSessionFocusGet(sp);
    }
    else
    {
        //失去焦点
        OnSessionFocusLose(sp);
    }
    return true;
}

DECL_SIGNAL_HANDLER(CVoiceListener::OnRoutineChanged)
{
    if ((ROUTINE_STATE)sigp.wParam.iValue != ROUTINE_PREDIAL)
    {
        return false;
    }

    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();

    if (pCallInfo == NULL)
    {
        return false;
    }

    int iDigitKey = pCallInfo->m_iDigitKey;
    if (iDigitKey == PHONE_KEY_NONE)
    {
        return false;
    }

#ifdef IF_SUPPORT_BLUETOOTH
    //蓝牙耳机中不播放第一个dtmf按键音
    if (!voice_IsBluetoothMode())
#endif // IF_SUPPORT_BLUETOOTH
    {
        // 播放按键音
        voice_PlayKeyTone(iDigitKey);
    }

    return true;
}

bool CVoiceListener::OnSessionFocusGet(const CSessionProxy& sp)
{
    CCallInfo* pCallInfo = sp.GetCallInfo();
    CListenInfo* priv = GetSessionData(sp);
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }
    switch (sp.GetRoutineState())
    {
    case ROUTINE_RINGING:
    case ROUTINE_RECALL:
        {
#if IF_FEATURE_LOOPBACK
            if (!pCallInfo->m_bIsLoopBackCall)
            {
                HoldLoopBackSession();
            }
#endif
            // 来电界面位于焦点时要停止播放callwaitingtone
            StopCallWaitingTone(false);
            if (pCallInfo->IsRingMute()
#if IF_FEATURE_LOOPBACK
                    || pCallInfo->m_bIsLoopBackCall
#endif
               )
            {
                StopRing(*pCallInfo);
            }
            else
            {
                // 处于焦点时播放铃声
                PlayRing(*pCallInfo);
            }
        }
        break;
#ifdef IF_BT_SUPPORT_PHONE
    case ROUTINE_BTRINGING:
        {
            // 来电界面位于焦点时要停止播放callwaitingtone
            StopCallWaitingTone(true);
            if (pCallInfo->IsRingMute())
            {
                StopRing(*pCallInfo);
            }
            else
            {
                // 处于焦点时播放铃声
                PlayRing(*pCallInfo);
            }
        }
        break;
    case ROUTINE_BTTALKING:
#endif
    case ROUTINE_TALKING:
    case ROUTINE_LINE_DEVICE:
        {
            if (pCallInfo->IsLocalHold())
            {
                SetHoldAlertTimer(*pCallInfo);
            }
            if (sp.GetRoutineState() != ROUTINE_BTTALKING)
            {
                //http://bugfree.yealink.com/Bug.php?BugID=123566
                //解决：开启一键会议时如果加入conf预拨号界面再退回通话界面，音量还是拨号音的大小。
                if (configParser_GetConfigInt(kszConfWithOnePress) == 1)
                {
                    voice_SetVolume(voice_GetVolume());
                }
            }
        }
        break;
    case ROUTINE_RINGBACK:
        {
            PlayRingbackTone(*pCallInfo);
        }
        break;
#ifdef IF_BT_SUPPORT_PHONE
    case ROUTINE_BTFINISHED:
        {
            // 如果存在通话中的话路，则不播放忙音
            if (!talklogic_IsTalkingSessionExist())
            {
                voice_PlayTone(TONE_BUSY);
            }
        }
        break;
    case ROUTINE_BTDIALING:
    case ROUTINE_BTPREDIAL:
#endif
    case ROUTINE_DIALING:
    case ROUTINE_PREDIAL:
    case ROUTINE_RETRIEVE_CALLPARK:
    case ROUTINE_PRETRAN:
        {
            // 延迟播放拨号音，先更新界面
            PlayDialToneDelay(priv->m_VoiceInfo, pCallInfo->GetSessionID());
        }
        break;
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    case ROUTINE_PARK_NOTIFY:
        {
            if (configParser_GetConfigInt(kszCallParkRingNotifyType) != 0
                    && configParser_GetConfigInt(kszBroadsoftActive) != 0)
            {
                // 响铃提醒用户
                voice_PlayTone(TONE_HOLD_ALERT, DEFAULT_TONE_INTERVAL, CHANNEL_HANDFREE);
            }
        }
        break;
#endif
    case ROUTINE_FINISHED:
        {
            // 如果存在通话中的话路，则不播放忙音
            if (!talklogic_IsTalkingSessionExist())
            {
                voice_PlayTone(priv->m_VoiceInfo.m_eToneType);
            };
        }
        break;
    case ROUTINE_AUTOREDIAL:
        {
            // 重获焦点,在提示界面重新设置定时器,否则继续倒计时
            if (AUTOREDIAL_TIP == _AutoRedialManager.GetStatus())
            {
                SetAutoRedialTimeOut(priv->m_VoiceInfo, (int)sp, AUTOREDIAL_TIP_TIME);
                //播放提示音
                voice_PlayTone(TONE_BUSY);
            }
        }
        break;

    default:
        break;
    }
    return true;
}

bool CVoiceListener::OnSessionFocusLose(const CSessionProxy& sp)
{
    CCallInfo* pCallInfo = sp.GetCallInfo();
    CListenInfo* priv = GetSessionData(sp);
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }
    switch (sp.GetRoutineState())
    {
    case ROUTINE_RINGING:
    case ROUTINE_RECALL:
        {
#if IF_FEATURE_LOOPBACK
            if (!pCallInfo->m_bIsLoopBackCall)
            {
                HoldLoopBackSession();
            }
#endif
            // 在FWD拨号界面时被切为非焦点Session，则需要退出FWD拨号
            if (pCallInfo->IsPreForword())
            {
                CSessionStateProxy(sp).ExitPreFWD();
            }
            StopRing(*pCallInfo);
        }
        break;
#ifdef IF_BT_SUPPORT_PHONE
    case ROUTINE_BTRINGING:
        {
            StopRing(*pCallInfo);
        }
        break;
    case ROUTINE_BTTALKING:
#endif
    case ROUTINE_TALKING:
    case ROUTINE_LINE_DEVICE:
        {
            CancelHoldAlertTimer(*pCallInfo);
        }
        break;
    case ROUTINE_RINGBACK:
        {
#ifdef IF_FEATURE_C5_SYS_VOICE_RECORD
            if (m_pCallInfo->IsAutoVoiceRecord())
            {
                PlayRingbackTone(*pCallInfo);
            }
            else
#endif
            {
                StopRingbackTone(*pCallInfo);
            }
        }
        break;
#ifdef IF_BT_SUPPORT_PHONE
    case ROUTINE_BTFINISHED:
        {
            voice_StopTone(TONE_BUSY);
        }
        break;
    case ROUTINE_BTDIALING:
    case ROUTINE_BTPREDIAL:
#endif
    case ROUTINE_DIALING:
    case ROUTINE_PREDIAL:
    case ROUTINE_RETRIEVE_CALLPARK:
    case ROUTINE_PRETRAN:
        {
            StopDialTone();
        }
        break;
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    case ROUTINE_PARK_NOTIFY:
        {
            voice_StopTone(TONE_HOLD_ALERT);
        }
        break;
#endif
    case ROUTINE_FINISHED:
        {
            voice_StopTone(priv->m_VoiceInfo.m_eToneType);
        }
        break;
    case ROUTINE_AUTOREDIAL:
        {
            // 失去焦点,如果在提示界面,停止定时器(UI处理),否则暂停倒计时
            if (AUTOREDIAL_TIP == _AutoRedialManager.GetStatus())
            {
                priv->m_VoiceInfo.timerAutoRedial.Kill();
                voice_StopTone(TONE_BUSY);
            }
        }
        break;
    default:
        break;
    }
    return true;
}

// 停止Callwaiting Tone
void CVoiceListener::StopCallWaitingTone(bool bIsBtTalk /*= false*/)
{
    // http://10.2.1.199/Bug.php?BugID=42773
    voice_StopTone(TONE_CALLWAIT);
    if (!bIsBtTalk)
    {
        // 来电界面位于焦点时要停止播放callwaitingtone
        // 此处不能直接调用voice_StopTone，有一些信号音不能停
        voice_StopTone(TONE_CALLWAIT1);
        voice_StopTone(TONE_CALLWAIT2);
        voice_StopTone(TONE_CALLWAIT3);
        voice_StopTone(TONE_CALLWAIT4);
    }
}

// 停止铃声
void CVoiceListener::StopRing(CCallInfo& callInfo)
{
    // m_bRingMute为true时不管是否有多路通话都需要stop
    if (!callInfo.IsRingMute()
            && talklogic_GetSessionNum() > 1
            && talklogic_IsAllSessionRinging())
    {
        // 如果有多个来电session则不停止响铃
        return;
    }
    voice_StopRing();
}

// 播放铃声
void CVoiceListener::PlayRing(CCallInfo& callInfo)
{
    if (!IsNeedToPlayRing(callInfo))
    {
        TALK_WARN("CBaseRingingRoutine::PlayRing: No need to play ring !");
        return;
    }
    if (!callInfo.IsRingMute())
    {
        CListenInfo* priv = GetCallerData(&callInfo);
        if (priv)
        {
            //TALK_INFO("CRingingRoutine::PlayRing: %s", m_strRingPath.c_str());
            // 播放铃声时，需要VPM通道资源
            _VoiceInterface.PlayRing(priv->m_VoiceInfo.m_strRingPath.c_str());
        }
    }
}

bool CVoiceListener::IsNeedToPlayRing(CCallInfo& callInfo)
{
    bool bPlayingRing = false;
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL)
    {
        return false;
    }

    if (!CSessionProxy(&callInfo).IsBTTalk())
    {
        // 如果需要马上自动应答/不需要马上自动应答挂断剩下一路之后需要自动应答, 则不播放铃声
        if (callInfo.m_bAutoAnswerNow
                && priv->m_AutoAnserProxy.IsOn()
                || !callInfo.m_bAutoAnswerNow
                && talklogic_GetSessionNum() == 1)
        {
            return false;
        }
        if (callInfo.m_bIsLoopBackCall)
        {
            return false;
        }
        // 如果在播放IntercomTone，先不响铃
        if (priv->m_VoiceInfo.timerIntercomTone.IsOn())
        {
            return false;
        }
        // http://10.2.1.199/Bug.php?BugID=37732
        // 如果no answer fwd时间设置为0则不播放铃声(等于1是因为定时器时间不能为0)
        int iTimerValue = priv->m_NoAnwFwdProxy.GetFWDTimerValue();
        if (iTimerValue == 0 || iTimerValue == DEFAULT_TIMER_VALUE)
        {
            return false;
        }
        // 正在下载服务器铃声时不播放
        if (priv->m_VoiceInfo.m_iRingStatus != TALK_RING_LS_LOADED)
        {
            return false;
        }
    }
    // 如果已经给对方发送接听的消息，则不再播放铃声，等待通话建立的消息
    if (callInfo.m_bAnswered)
    {
        return false;
    }
    // T3x:所有通话都是来电类型才播放
    bPlayingRing = !IsForbinPlayRingByOtherSession();
    return bPlayingRing;
}

#if IF_FEATURE_LOOPBACK
void CVoiceListener::HoldLoopBackSession()
{
    if (talklogic_GetSessionNum() == 0)
    {
        return;
    }
    YLVector<int> Idlist;
    talklogic_GetAllSessionID(Idlist);
    for (int i = 0; i < Idlist.size(); ++i)
    {
        CSessionProxy sp(Idlist[i]);
        if (!sp || sp.GetHost())
        {
            continue;
        }
        else if (sp.IsSingle())
        {
            CCallInfo* pCallInfo = sp.GetCallInfo();
            if (pCallInfo != NULL && pCallInfo->m_bIsLoopBackCall)
            {
                sp.HoldSession(true);
            }
        }
    }
}
#endif
// 设置Hold/Held提示定时器
void CVoiceListener::SetHoldAlertTimer(CCallInfo& callInfo)
{
    // 获取Hold/Held提示音开始时间（第一次提示音的延时）
    int iTimeout = STOP_TIMER_VALUE;
    VoiceInfo::HOLD_ALERT_TYPE alertType = VoiceInfo::HAT_NO_ALERT;
    if (callInfo.IsLocalHold() // Hold状态
            && configParser_GetConfigInt(kszPlayHoldToneSwitch)) // 启用Hold提示音
    {
        // 设置Hold提示音开始时间
        iTimeout = configParser_GetConfigInt(kszPlayHoldToneDelay);
        if (iTimeout < 3 || iTimeout > 3600)
        {
            // 超出范围则取默认值
            iTimeout = 30;
        }
        alertType = VoiceInfo::HAT_HOLD_ALERT;
    }
    else if (callInfo.IsRemoteHold() // Held状态
             && configParser_GetConfigInt(kszPlayHeldToneSwitch) // 启用Held提示音
             && configParser_GetCfgItemStringValue(kszAccountMusicServerUri,
                     callInfo.GetAccountID()).empty()) // 当前账号Music Server URI为空
    {
        // 设置Held提示音开始时间
        iTimeout = configParser_GetConfigInt(kszPlayHeldToneDelay);
        if (iTimeout < 3 || iTimeout > 3600)
        {
            // 超出范围则取默认值
            iTimeout = 30;
        }
        alertType = VoiceInfo::HAT_HELD_ALERT;
    }
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL)
    {
        return;
    }
    VoiceInfo& vInfo = priv->m_VoiceInfo;
    if (alertType == vInfo.eHoldAlertType)
    {
        // Hold/Held提示音类型没有变化，则不重新计时
        return;
    }

    // 提示音类型发生变化, 停止前一个提示音
    if (VoiceInfo::HAT_HOLD_ALERT == vInfo.eHoldAlertType)
    {
        voice_StopTone(TONE_HOLD_ALERT);
    }

    // 提示音类型发生变化，则重新计时
    vInfo.eHoldAlertType = alertType;
    if (vInfo.timerHoldAlert.IsOn())
    {
        // 如果已有定时，则先取消定时
        vInfo.timerHoldAlert.Kill();
        TALK_INFO("Hold alert timer canceled.");
    }
    if (iTimeout != STOP_TIMER_VALUE)
    {
        vInfo.timerHoldAlert.SetCleanUpCallBack(MAKE_TIMER_CALL_BACK(CVoiceListener::StopHoldAlert));
        vInfo.timerHoldAlert.SetCallBack(MAKE_TIMER_CALL_BACK(CVoiceListener::OnTimerOut));
        vInfo.timerHoldAlert  % callInfo.GetSessionID() % (iTimeout * 1000);
        vInfo.timerHoldAlert.SetTimer(iTimeout * 1000);
        TALK_INFO("Hold alert timer set[%d].", iTimeout * 1000);
    }
}
int CVoiceListener::StopHoldAlert(CScopedTimer& timer)
{
    CListenInfo* priv = GetSessionData(timer.Get());
    if (priv)
    {
        priv->m_VoiceInfo.eHoldAlertType = VoiceInfo::HAT_NO_ALERT;
    }
    return 0;
}
// Hold/Held提示定时器触发
void CVoiceListener::OnHoldAlertTimer(CCallInfo& callInfo)
{
    // 获取Hold/Held提示音间隔时间
    int iTimeout = STOP_TIMER_VALUE;
    if (callInfo.IsLocalHold() // Hold状态
            && configParser_GetConfigInt(kszPlayHoldToneSwitch)) // 启用Hold提示音
    {
        // 设置Hold提示音开始时间
        iTimeout = configParser_GetConfigInt(kszPlayHoldToneInterval);
        if (iTimeout < 3 || iTimeout > 3600)
        {
            // 超出范围则取默认值
            iTimeout = 30;
        }
    }
    else if (callInfo.IsRemoteHold() // Held状态
             && configParser_GetConfigInt(kszPlayHeldToneSwitch) // 启用Held提示音
             && configParser_GetCfgItemStringValue(kszAccountMusicServerUri,
                     callInfo.GetAccountID()).empty()) // 当前账号Music Server URI为空
    {
        // 设置Held提示音开始时间
        iTimeout = configParser_GetConfigInt(kszPlayHeldToneInterval);
        if (iTimeout < 3 || iTimeout > 3600)
        {
            // 超出范围则取默认值
            iTimeout = 60;
        }
    }
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL)
    {
        return;
    }
    VoiceInfo& vInfo = priv->m_VoiceInfo;
    if (iTimeout * 1000 != vInfo.timerHoldAlert.Get(1) && iTimeout != STOP_TIMER_VALUE)
    {
        // 定时器时间不同，则重新设置定时器；否则保留原有定时器
        vInfo.timerHoldAlert.Kill();
        vInfo.timerHoldAlert  % callInfo.GetSessionID() % (iTimeout * 1000);
        vInfo.timerHoldAlert.SetTimer(iTimeout * 1000);
        TALK_INFO("Hold alert timer updated[%d].", iTimeout * 1000);
    }
}
// 取消Hold/Held提示定时器
void CVoiceListener::CancelHoldAlertTimer(CCallInfo& callInfo)
{
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv)
    {
        priv->m_VoiceInfo.timerHoldAlert.Kill();
        TALK_INFO("Hold alert timer canceled by api.");
        priv->m_VoiceInfo.eHoldAlertType = VoiceInfo::HAT_NO_ALERT;
    }
}

// 是否播放hold提示音
bool CVoiceListener::IsPlayHoldAlert(CSessionProxy& sp)
{
    // 当前session是焦点Sesson时，要播放hold提示音
    if (!sp.IsHide())
    {
        // 被隐藏（且没有host）时不播放hold tone,比如Recall
        return true;
    }
    // 当前session的父Session是焦点Sesson时，要播放hold提示音
    CSessionProxy hostSp = sp.GetHost();
    if (hostSp && !hostSp.IsHide())
    {
        return true;
    }
    return false;
}


// 播放回铃音
void CVoiceListener::PlayRingbackTone(CCallInfo& callInfo)
{
    if (callInfo.m_bIsColorRingback)
    {
        ProcessRingBackMedia(callInfo);
        CSessionProxy(&callInfo).StartTalk();
    }
    else
    {
        int iToneType = callInfo.m_bPlay182Tone ? TONE_CALLER_WAIT : TONE_RINGBACK;
        /*在预通话状态下，收到180(183,StartTalk已经处理了)，根据用户选择策略，由vpm自行处理：
        1：ignore，tone与RTP混合
        2：force，丢弃RTP包，强制播放tone
        3：skip，跳过tone，播放RTP*/
        // 播放回铃音
        voice_PlayTone(iToneType);
    }
}

void CVoiceListener::ProcessRingBackMedia(CCallInfo& callInfo)
{
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL)
    {
        return;
    }
    TALK_INFO("CBaseRoutine::ProcessRingBackMedia [%d]", priv->m_VoiceInfo.m_ePlayRingType);
    //http://bugfree.yealink.com/Bug.php?BugID=120749
    //加入会议则不再响回铃音
    if (priv->m_VoiceInfo.m_ePlayRingType == PRT_PLAY_LOCAL_RING
            && !callInfo.m_bIsInConf)
    {
        voice_PlayTone(TONE_RINGBACK);
    }
    else
    {
        voice_StopTone();
    }
}

// 停止播放回铃音
void CVoiceListener::StopRingbackTone(CCallInfo& callInfo)
{
    bool bIsInLocalConf = CSessionProxy(&callInfo).IsInConf();
    CListenInfo* priv = GetCallerData(&callInfo);
    //已经加入会议的不用结束tone音，否则会影响进入会议的提示音
    if (configParser_GetConfigInt(kszPlayLocalOnProvResponseSdp) == 0
            || (!callInfo.m_bIsColorRingback
                && !bIsInLocalConf)
            || (priv && priv->m_VoiceInfo.m_ePlayRingType == PRT_PLAY_LOCAL_RING))
    {
        voice_StopTone();
    }
}

// 停止拨号音
void CVoiceListener::StopDialTone()
{
    voice_StopTone(TONE_DIAL);
    voice_StopTone(TONE_DIAL2);
    voice_StopTone(TONE_DIAL3);
    voice_StopTone(TONE_BUSY);
}

// 延迟播放拨号音
void CVoiceListener::PlayDialToneDelay(VoiceInfo& vInfo, int iSessionId)
{
    vInfo.timerDelayPlayTone.SetCallBack(MAKE_TIMER_CALL_BACK(CVoiceListener::OnTimerOut));
    vInfo.timerDelayPlayTone % iSessionId % 10;
    vInfo.timerDelayPlayTone.SetTimer(10);
}

int CVoiceListener::OnTimerOut(CScopedTimer& timer)
{
    CListenInfo* priv = GetSessionData(timer.Get());
    CSessionStateProxy ssP(timer.Get());
    CCallInfo* pCallInfo = ssP.GetCallInfo();
    if (priv == NULL || pCallInfo == NULL)
    {
        return false;
    }
    VoiceInfo& vInfo = priv->m_VoiceInfo;
    if (&vInfo.timerParkNotify == &timer)
    {
        // 当前session是焦点Sesson时，要播放hold提示音
        if (ssP.IsFocusSession())
        {
            voice_PlayTone(TONE_HOLD_ALERT, DEFAULT_TONE_INTERVAL, CHANNEL_HANDFREE);
        }
    }
    else if (&vInfo.timerTranFailedAudioAlert == &timer)
    {
        bool bCancelAlert = false;
        if (voice_GetCurrentChannel() != CHANNEL_RING)
        {
            voice_PlayTone(TONE_TRAN_FAILED, 300);
        }
        if (--vInfo.m_iTranFailedToneTimes <= 0)
        {
            bCancelAlert = true;
        }
        // call park失败不需要定时提示
        if (vInfo.m_bCallParkFailedAlert)
        {
            bCancelAlert = true;
        }
        if (bCancelAlert)
        {
            // 退出提示
            ExitTransFailedAlert(*pCallInfo);
        }
    }
    else if (&vInfo.timerToneDelay == &timer)
    {
        CancelBusyTone(ssP, vInfo);
    }
    else if (&vInfo.timerIntercomTone == &timer)
    {
        CancelIntercomToneTimer(vInfo);
        // 停止播放Tone之后要响铃
        PlayRing(*pCallInfo);
    }
    else if (&vInfo.timerDelayPlayTone == &timer)
    {
        CancelPlayDialTone(vInfo);
        PlayDialTone(*pCallInfo);
    }
    else if (&vInfo.timerPlayMsgTone == &timer)
    {
        CancelMSGTone(vInfo);
        PlayDialTone(*pCallInfo);
    }
    else if (&vInfo.timerBusyTone == &timer)
    {
        if (ssP.IsInherits(ROUTINE_FINISHED_CLS))
        {
            vInfo.timerBusyTone.Kill();
            // 响忙音时间到，退出该Routine
            ssP.ExitSession();
        }
    }
    else if (&vInfo.timerMadiaSniffer == &timer)
    {
        vInfo.timerMadiaSniffer.Kill();
        if (pCallInfo
                && priv->m_VoiceInfo.m_ePlayRingType == PRT_PLAY_NONE
                //已经变成非焦点不需要再响回铃
                && ssP.IsFocusSession())
        {
            priv->m_VoiceInfo.m_ePlayRingType = PRT_PLAY_LOCAL_RING;
            ProcessRingBackMedia(*pCallInfo);
        }
    }
    else if (&vInfo.timerAutoRedial == &timer)
    {
        // 提示超时
        vInfo.timerAutoRedial.Kill();
        _AutoRedialManager.StopAutoRedial();
        ssP.ExitSession();
    }
    else if (&vInfo.timerHoldAlert == &timer)
    {
        if (!ssP.IsBTTalk())
        {
            if (priv->IsTransFailed())
            {
                vInfo.timerHoldAlert.Kill();
                return true;
            }
        }
        if (IsPlayHoldAlert(ssP))
        {
            voice_PlayTone(TONE_HOLD_ALERT, DEFAULT_TONE_INTERVAL, CHANNEL_HANDFREE);
        }
        OnHoldAlertTimer(*pCallInfo);
    }
    else if (&vInfo.timerRing == &timer)
    {
        TALK_INFO("Play call waiting tone timer");
        PlayCallWaitingTone(*pCallInfo);
    }
    return true;
}

void CVoiceListener::SetAutoRedialTimeOut(VoiceInfo& info, int iSessionID, int iTime)
{
    info.timerAutoRedial.SetCallBack(MAKE_TIMER_CALL_BACK(CVoiceListener::OnTimerOut));
    info.timerAutoRedial % (iSessionID);
    info.timerAutoRedial.SetTimer(AUTOREDIAL_TIP_TIME);
}

// 获取来电响铃类型
void CVoiceListener::GetCallWaitingToneType(CCallInfo& callInfo)
{
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL)
    {
        return;
    }
    if (CSessionProxy(&callInfo).IsBTTalk())
    {
        priv->m_VoiceInfo.eCallWaitingTone = TONE_CALLWAIT;
        return;
    }
    talkhelp_GetCallWaitingToneType(priv->m_VoiceInfo.eCallWaitingTone, &callInfo.GetRingUserInfo());
}

// 判断是否需要响来电提示音
bool CVoiceListener::IsNeedPlayCallWaitingTone()
{
    if (configParser_GetConfigInt(kszCallWaitingToneSwitch) == 0)
    {
        return false;
    }
    // 焦点Session是通话中才提示
    CSessionStateProxy focusSessionSp;
    if (focusSessionSp.IsTalkSession()
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
            && (!focusSessionSp.IsHold() || PT_T49 == devicelib_GetPhoneType())
#endif //IF_FEATURE_BROADSOFT_CALL_CENTER
       )
    {
        return true;
    }
    return false;
}

// 播放通话中新来电提示
void CVoiceListener::PlayCallWaitingTone(CCallInfo& callInfo)
{
    if (IsNeedPlayCallWaitingTone())
    {
        CListenInfo* priv = GetCallerData(&callInfo);
        if (priv)
        {
            TALK_INFO("CBaseRingingRoutine::PlayCallWaitingTone[%d]", priv->m_VoiceInfo.eCallWaitingTone);
            // 先停止播放信号音，再重新播放，如：通话中有两路新来电必须要提示两次
            voice_StopTone();
            // 播放新来电提示音
            voice_PlayTone(priv->m_VoiceInfo.eCallWaitingTone);
        }
    }
}

// 设置定时响铃定时器
void CVoiceListener::SetCallWaitingToneTimer(CSessionStateProxy& ssP)
{
    CListenInfo* priv = GetSessionData(ssP);
    if (priv == NULL)
    {
        return;
    }
    VoiceInfo& vInfo = priv->m_VoiceInfo;
    if (!vInfo.timerRing.IsOn())
    {
        vInfo.timerRing.SetCallBack(MAKE_TIMER_CALL_BACK(CVoiceListener::OnTimerOut));
        vInfo.timerRing % (int)ssP % ((int)RING_INTERVAL_TIME);
        vInfo.timerRing.SetTimer(RING_INTERVAL_TIME);
    }
}

#ifdef IF_BT_SUPPORT_PHONE
// 获取来电铃声
void CVoiceListener::GetIncomingCallRing(CCallInfo& callInfo)
{
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL)
    {
        return;
    }
    // step1:获取联系人中对应的铃声信息
    if (talkhelp_GetContractRing(priv->m_VoiceInfo.m_strRingPath, callInfo.m_tRemoteInfo.sSIPName.c_str()))
    {
        return;
    }

    // step2:如果账号指定铃声不存在，则判断配置中的默认铃声
    if (talkhelp_GetDefaultRing(priv->m_VoiceInfo.m_strRingPath))
    {
        return;
    }
    priv->m_VoiceInfo.m_strRingPath.clear();
}
#endif

void CVoiceListener::ProcessMediaSniffer(CCallInfo& callInfo)
{
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL)
    {
        return;
    }
    VoiceInfo& vInfo = priv->m_VoiceInfo;
    int iMadiaSniffer = configParser_GetConfigInt(kszMediaSnifferTimeout);
    TALK_INFO("CRingbackRoutine::ProcessMediaSniffer [%d]", iMadiaSniffer);
    if (-1 == iMadiaSniffer)
    {
        vInfo.m_ePlayRingType = PRT_PLAY_NONE;
    }
    else if (0 == iMadiaSniffer)
    {
        vInfo.m_ePlayRingType = PRT_PLAY_LOCAL_RING;
    }
    else
    {
        vInfo.m_ePlayRingType = PRT_PLAY_NONE;
        if (iMadiaSniffer <= 0 || iMadiaSniffer > 100)
        {
            iMadiaSniffer = 1;
        }
        iMadiaSniffer *= 1000;
        vInfo.timerMadiaSniffer.SetCallBack(MAKE_TIMER_CALL_BACK(CVoiceListener::OnTimerOut));
        vInfo.timerMadiaSniffer % callInfo.GetSessionID() % iMadiaSniffer;
        vInfo.timerMadiaSniffer.SetTimer(iMadiaSniffer);
    }
}

// 处理回铃消息
void CVoiceListener::ProcessRingbackSIPMsg(msgObject& msg)
{
    if (NULL == msg.GetExtraData())
    {
        return;
    }
    //对当前焦点话路进行操作
    CSessionStateProxy focusSessionSp;
    CCallInfo* pCallInfo = focusSessionSp.GetCallInfo();
    ROUTINE_STATE eState = focusSessionSp.GetRoutineState();
    CListenInfo* priv = GetCallerData(pCallInfo);
    if (pCallInfo == NULL || priv == NULL
            || (eState != ROUTINE_RINGBACK && !focusSessionSp.IsInherits(ROUTINE_TALKING)))
    {
        return ;
    }
    sip_call_ringback_info_t* pRingbackInfo = (sip_call_ringback_info_t*)msg.GetExtraData();
    TALK_INFO("ProcessRingbackSIPMsg: pRingbackInfo->type = %d", pRingbackInfo->type);
    if (pRingbackInfo->type == 0 || pRingbackInfo->type == 2)
    {
        pCallInfo->m_bPlay182Tone = (pRingbackInfo->type == 2);
        int iToneType = pCallInfo->m_bPlay182Tone ? TONE_CALLER_WAIT : TONE_RINGBACK;
        // 如果已经在彩铃中,则停止彩铃,播放回铃音
        if (pCallInfo->m_bIsColorRingback)
        {
            priv->m_VoiceInfo.m_bNeedProcessRtpMsg = true;
            // 如果满足183之后处理180的条件则停止彩铃
            if (configParser_GetConfigInt(kszDeal180After183Switch) != 0)
            {
                focusSessionSp.StopTalk();
                pCallInfo->m_bIsColorRingback = false;
                priv->m_VoiceInfo.m_bNeedProcessRtpMsg = false;
            }
            else
            {
                // 如果不处理183之后的180，则直接返回
                return;
            }
        }
        // 播放回铃音
        voice_PlayTone(iToneType);
    }
    else
    {
#ifdef IF_FEATURE_CHANGE183TO180
        if (pCallInfo->m_bIsColorRingback)
        {
            // 为了兼容BugID#19362的问题,将第二个回复的183转换成180处理
            if (configParser_GetConfigInt(kszBeChangeSec183to180) != 0)
            {
                focusSessionSp.StopTalk();
                pCallInfo->m_bIsColorRingback = false;
                priv->m_VoiceInfo.m_bNeedProcessRtpMsg = false;
                // 播放回铃音
                voice_PlayTone(TONE_RINGBACK);
                // 进入回铃阶段
                return;
            }
        }
#endif
        priv->m_VoiceInfo.m_bNeedProcessRtpMsg = true;
        // 如果新的通话VPM参数和彩铃VPM参数相同则不再重新开启通话
        ipvp_speech_t voiceParam;
        Comm_RingbackParam_sip2ipvp(pRingbackInfo, &voiceParam, &pCallInfo->m_stVideoExt);
        if (pCallInfo->m_bIsColorRingback)
        {
            //http://10.2.1.199/Bug.php?BugID=115290
            //vpm 要求183之后183停止通话在打开通话
            focusSessionSp.StopTalk();
        }
        else
        {
            // http://192.168.1.99/Bug.php?BugID=22824
            // 180之后收到183，停止回铃音
            voice_StopTone();
        }
        ProcessMediaSniffer(*pCallInfo);
        ProcessRingBackMedia(*pCallInfo);
        // 彩铃
        pCallInfo->m_bIsColorRingback = true;
        // 保存播放彩铃的通话参数
        StartTalkByIpvpParam(&voiceParam, pCallInfo);
    }
}
// 根据SIP消息的参数开启通话
void CVoiceListener::StartTalkByIpvpParam(ipvp_speech_t* pSpeechInfo, CCallInfo* pCallInfo, bool bNeedHold /*= true*/)
{
    if (NULL == pSpeechInfo
            || pCallInfo == NULL)
    {
        return;
    }
    pCallInfo->SetIPVPParam(*pSpeechInfo);
    CSessionProxy(pCallInfo).StartTalk();
}

bool CVoiceListener::ProcessIPVPMsg(CCallInfo& callInfo, msgObject& msg)
{
    TALK_INFO("CVoiceListener::ProcessIPVPMsg msg[%d] wp[%d] lp[%d]",
              msg.message, msg.wParam, msg.lParam);
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL || !priv->m_VoiceInfo.m_bNeedProcessRtpMsg)
    {
        return false;
    }
    switch (msg.message)
    {
    case IPVP_MSG_AUDIO_FIRST_RTP:
        {
            ProcessRingbackRing(callInfo, PRT_PLAY_COLOR_RING);
            return true;
        }
        break;
    case IPVP_MSG_AUDIO_DEAD:
        {
            // 彩铃没有收到RTP
            if (msg.lParam == 1
                    && priv->m_VoiceInfo.m_ePlayRingType != PRT_PLAY_LOCAL_RING)
            {
                TALK_INFO("play Tone [%d]", priv->m_VoiceInfo.m_bNeedProcessRtpMsg);
                ProcessRingbackRing(callInfo, PRT_PLAY_LOCAL_RING);
                return true;
            }
        }
        break;
    default:
        break;
    }
    return false;
}

void CVoiceListener::ProcessRingbackRing(CCallInfo& callInfo, PlayRingType ePlayType)
{
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv)
    {
        priv->m_VoiceInfo.m_ePlayRingType = ePlayType;
    }

    ProcessRingBackMedia(callInfo);
}

void CVoiceListener::SetToneType(CCallInfo& callInfo)
{
    const TalkErrorInfo& errorInfo = callInfo.m_errorInfo;
    TALK_WARN("code=[%#x] reason=[%s][%s]", errorInfo.iReason, errorInfo.strError.c_str());
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL)
    {
        return;
    }
    switch (errorInfo.iReason)
    {
    case TALK_ERROR_CUSTOM_CONGESTION_TONE:
    case TALK_ERROR_CALLOUT_NU:
        {
            priv->m_VoiceInfo.m_eToneType = TONE_CONGESTION;
        }
        break;
    default:
        priv->m_VoiceInfo.m_eToneType = TONE_BUSY;
        break;
    }
}

// 设置忙音延迟
void CVoiceListener::SetToneDelay(CCallInfo& callInfo)
{
    // 设置响忙音时间定时器
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL)
    {
        return;
    }
    // 如果busytone时间为0则默认响3s的忙音
    int iBusyToneTime = configParser_GetConfigInt(kszBusytoneDelay) * 1000;
    if (iBusyToneTime == 0)
    {
        iBusyToneTime = 3 * 1000;
    }
    VoiceInfo& vInfo = priv->m_VoiceInfo;
    vInfo.timerBusyTone.SetCallBack(MAKE_TIMER_CALL_BACK(CVoiceListener::OnTimerOut));
    vInfo.timerBusyTone % callInfo.GetSessionID() % iBusyToneTime;
    vInfo.timerBusyTone.SetTimer(iBusyToneTime);
}
// 设置新短息提示
void CVoiceListener::SetMSGTone(CCallInfo& callInfo, TONE_TYPE eTone/* = TONE_NONE*/, int iTimerPlayMsgTone/* = DEFAULT_TONE_INTERVAL*/)
{
    // 输入号码为空才提示
    if (eTone == TONE_NONE && !callInfo.GetDialText().empty())
    {
        return;
    }
    //fwd,DND,以及VM有一个满足,则播放stutterdial tone
    bool bPlayStutterTone = (eTone == TONE_NONE
                             && (dnd_IsEnable(callInfo.GetAccountID()) && configParser_GetConfigInt(kszStutterDialToneDNDSwitch))
                             || (fwd_GetInfo(callInfo.GetAccountID(), FWD_TYPE_ALL) && configParser_GetConfigInt(kszStutterDialToneFWDSwitch))
                             || (acc_GetUnReadVoiceMailNum(callInfo.GetAccountID()) > 0 && configParser_GetConfigInt(kszStutterDialToneVMSwitch)));
    if (bPlayStutterTone)
    {
        iTimerPlayMsgTone = 0;
        //先从网页选中国家的stutterdial配置中获取值，为空则赋缺省值425/1000,0/250
        yl::string strToneCountry = configParser_GetConfigString(kszToneCountry);
        yl::string strStutterDialTone = "";
        if (strToneCountry == "Custom")
        {
            strStutterDialTone = configParser_GetConfigString(kszStutterDialTone);
        }
        else
        {
            char pBuffer[160] = "";
            etlGetProfileString(CONFIG_PATH "tone/CptToneCountry.cfg", strToneCountry.c_str(), "stutterdial", pBuffer, "");
            strStutterDialTone = commonAPI_FormatString("%s", pBuffer);
        }
        if (strStutterDialTone.empty())//缺省
        {
            strStutterDialTone = "425/1000,0/250";
        }
        //解析VPM测的配置参数stutterdial,获取播放时长
        //开头不带"!",则无需解析时间,一直播放
        if (!strStutterDialTone.empty()
                && strStutterDialTone.substr(0, 1) == "!")
        {
            YLList<yl::string> listStutterDialTone;
            if (commonAPI_splitStringBySeparator(strStutterDialTone, ',', listStutterDialTone)
                    && listStutterDialTone.size() > 0)
            {
                YLList<yl::string>::iterator it = listStutterDialTone.begin();
                for (; it != listStutterDialTone.end(); it++)
                {
                    yl::string strDialTone = *it;
                    int post1 = strDialTone.find("/");

                    if (post1 != yl::string::npos)
                    {
                        strDialTone = strDialTone.substr(post1 + 1);
                        iTimerPlayMsgTone += atoi(strDialTone.c_str());
                    }
                }
            }
        }
    }

    // 如果未指定Tone类型则自己去判断是否播放
    if (eTone == TONE_NONE)
    {
        // 如果当前账号存在新的未读语音信息，开启fwd或者DND，则播放提示音
        if (bPlayStutterTone)
        {
            eTone = TONE_STUTTER_DIAL;
        }
        // 如果当前账号存在新的语音信息或者未读短信，则播放提示音
        // 未读VoiceMail数目
        else if (AdapterBox_VoiceMailGetUnReadNum(callInfo.GetAccountID(), true) > 0)
        {
            eTone = TONE_VOICEMAIL;
        }
        // 获取对应账号的未读短信数目 TODO
        /*else if (Setting_GetTotalUnReadMessage() > 0)
        {
        eTone = TONE_TEXT_MSG;
        }*/
    }

    // 输入号码为空才提示
    if (TONE_NONE == eTone
            || !callInfo.GetDialText().empty())
    {
        return;
    }
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL)
    {
        return;
    }
    if (iTimerPlayMsgTone > 0)
    {
        VoiceInfo& vInfo = priv->m_VoiceInfo;
        vInfo.timerPlayMsgTone.SetCallBack(MAKE_TIMER_CALL_BACK(CVoiceListener::OnTimerOut));
        vInfo.timerPlayMsgTone % callInfo.GetSessionID() % iTimerPlayMsgTone;
        vInfo.timerPlayMsgTone.SetTimer(iTimerPlayMsgTone);
    }
    // 通知VPM播放信号音
    voice_PlayTone(eTone);
}

// 取消新短息提示
void CVoiceListener::CancelMSGTone(VoiceInfo& vInfo)
{
    if (vInfo.timerPlayMsgTone.IsOn())
    {
        vInfo.timerPlayMsgTone.Kill();
        // 需要停止播放tone
        voice_StopTone();
    }
}

// 播放或停止拨号音
void CVoiceListener::PlayDialTone(CCallInfo& callInfo, TONE_TYPE eToneType/* = TONE_NONE*/)
{
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL)
    {
        return;
    }
    VoiceInfo& vInfo = priv->m_VoiceInfo;
    // 正在播放Msg提示音时不播放拨号音
    if (vInfo.timerPlayMsgTone.IsOn())
    {
        return;
    }
    //BTDialRoutine
    if (CSessionStateProxy(&callInfo).IsBTTalk())
    {
        if (eToneType != TONE_NONE || IsNeedToPlayDialTone(callInfo))
        {
            TONE_TYPE eAccTone = talkhelp_GetDialTone(callInfo.GetAccountID());
            voice_PlayTone(eToneType != TONE_NONE ? eToneType : eAccTone);
        }
        else
        {
            StopDialTone();
        }
        return;
    }
    // 停止异步播放拨号音
    if (eToneType != TONE_NONE)
    {
        CancelPlayDialTone(vInfo);
    }
    if (vInfo.timerToneDelay.IsOn())
    {
        return;
    }

    if (eToneType != TONE_NONE || IsNeedToPlayDialTone(callInfo))
    {
        // xmlbrowser下发的命令会响铃声,因为没有界面,所以没有地方去停铃声,导致进入拨号界面的时候铃声还在
        voice_StopRing();

        if (acc_IsAccountAvailable(callInfo.GetAccountID())
                && acc_IsDirectIPAccount(callInfo.GetAccountID())
                || configParser_GetConfigInt(kszUnregisterdPartitionTone) == 0)
        {
            // v81新需求 ReDialtone 匹配时需要使用Second DialTone http://10.2.1.199/Bug.php?BugID=94862
            if (!callInfo.GetDialText().empty()
                    && callInfo.GetDialText() == configParser_GetConfigString(kszReDialtoneNumber))
            {
                voice_PlayTone(TONE_DIAL2);
            }
            else
            {
                voice_PlayTone(eToneType != TONE_NONE ? eToneType : talkhelp_GetDialTone(callInfo.GetAccountID()));
            }
        }
        else
        {
            // 7854
            voice_PlayTone(TONE_DIAL2);
        }
    }
    else
    {
        StopDialTone();
    }
}

void CVoiceListener::CancelPlayDialTone(VoiceInfo& vInfo)
{
    vInfo.timerDelayPlayTone.Kill();
}

// 判断是否需要播放拨号音
bool CVoiceListener::IsNeedToPlayDialTone(CCallInfo& callInfo)
{
    if (!CSessionProxy(&callInfo).IsBTTalk())
    {
        if (IsNeedPlayBusyTone(callInfo))
        {
            return false;
        }
        // 已输入号码为空或等于ReDialTone的号码时需要播放拨号音
        if (callInfo.GetDialText().empty()
                || callInfo.GetDialText() == configParser_GetConfigString(kszReDialtoneNumber))
        {
            return true;
        }
    }
    // 已输入号码为空时需要播放拨号音
    return callInfo.GetDialText().empty();
}

bool CVoiceListener::IsNeedPlayBusyTone(CCallInfo& callInfo)
{
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL)
    {
        return false;
    }
    return (!priv->m_bBLASeizeLineSucced
            && Emergency_IsEmergencyModeEnable()
            && acc_IsBLAEnable(callInfo.GetAccountID()));
}

// 设置IntercomTone定时器
void CVoiceListener::SetIntercomToneTimer(CCallInfo& callInfo)
{
    if (callInfo.m_bIsIntercom
            && configParser_GetConfigInt(kszIntercomToneSwitch) != 0
            && configParser_GetConfigInt(kszIntercomBargeSwitch) == 0)
    {
        CListenInfo* priv = GetCallerData(&callInfo);
        if (priv == NULL)
        {
            return;
        }
        // 如果允许Tone，且没开启Barge，则播放提示音 (开启了barge也会提示)
        priv->m_VoiceInfo.timerIntercomTone.SetCallBack(MAKE_TIMER_CALL_BACK(CVoiceListener::OnTimerOut));
        priv->m_VoiceInfo.timerIntercomTone % callInfo.GetSessionID() % DEFAULT_TONE_INTERVAL;
        priv->m_VoiceInfo.timerIntercomTone.SetTimer(DEFAULT_TONE_INTERVAL);
        voice_PlayTone(TONE_AUTOANSWER);
    }
}
// 取消IntercomTone定时器
void CVoiceListener::CancelIntercomToneTimer(VoiceInfo& vInfo)
{
    voice_StopTone(TONE_AUTOANSWER);
    vInfo.timerIntercomTone.Kill();
}

bool CVoiceListener::IsForbinPlayRingByOtherSession()
{
    YLVector<int> Idlist;
    talklogic_GetAllSessionID(Idlist);
    for (int i = 0; i < Idlist.size(); ++i)
    {
        CSessionProxy sp(Idlist[i]);
        if (!sp || sp.IsHide())
        {
            continue;
        }
        // Hold的话路 不算
        if (sp.IsTalkSession() && sp.IsHold())
        {
            continue;
        }
        // Ringing/Park/AOC话路 不算
        if (SESSION_RINGING == sp.GetSessionState()
                || SESSION_RECALL == sp.GetSessionState()
                || SESSION_PARK_NOTIFY == sp.GetSessionState()
                || SESSION_AOC_FINISH == sp.GetSessionState()
                || SESSION_BTRINGING == sp.GetSessionState())
        {
            continue;
        }
        return true;
    }
    return false;
}

void CVoiceListener::SetBusyToneDelay(CCallInfo& callInfo)
{
    // 设置响忙音时间定时器
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL)
    {
        return;
    }
    if (CSessionStateProxy(&callInfo).IsFocusSession())
    {
        PlayDialTone(callInfo, TONE_BUSY);

        // 如果busytone时间为0则默认响3s的忙音
        int iTimerToneDelay = configParser_GetConfigInt(kszBusytoneDelay) * 1000;
        if (iTimerToneDelay == 0)
        {
            iTimerToneDelay = 3 * 1000;
        }
        priv->m_VoiceInfo.timerToneDelay.SetCallBack(MAKE_TIMER_CALL_BACK(CVoiceListener::OnTimerOut));
        priv->m_VoiceInfo.timerToneDelay % callInfo.GetSessionID() % iTimerToneDelay;
        priv->m_VoiceInfo.timerToneDelay.SetTimer(iTimerToneDelay);
    }
    else
    {
        priv->m_VoiceInfo.timerToneDelay.Kill();
    }

    priv->m_bBLASeizeLineSucced = false;
}

void CVoiceListener::CancelBusyTone(CSessionStateProxy& sp, VoiceInfo& vInfo)
{
    vInfo.timerToneDelay.Kill();
    if (sp.IsFocusSession())
    {
        voice_StopTone(TONE_BUSY);
    }
}

// 转移失败提示
void CVoiceListener::AlertForTransFailed(CSessionStateProxy& ssP, bool bCallParkFailed/* = false*/)
{
    CCallInfo* pCallInfo = ssP.GetCallInfo();
    CListenInfo* priv = GetCallerData(pCallInfo);
    if (pCallInfo == NULL || priv == NULL)
    {
        return;
    }
    VoiceInfo& vInfo = priv->m_VoiceInfo;

    vInfo.m_bCallParkFailedAlert = bCallParkFailed;
    // 如果有新来电或当前通话不是焦点话路,则不提示transferfailed
    if (talklogic_IsRingSessionExist()  || !ssP.IsFocusSession())
    {
        // 返回到正常通话
        pCallInfo->SetTransfer(false);
        return;
    }
    int iTimerTranFailedAudioAlert = STOP_TIMER_VALUE;
    // 通道是免提且没有话路是在通话中的状态,则响铃声提示,否则用嘟嘟提示
    if (voice_GetCurrentChannel() == CHANNEL_HANDFREE
            && !talklogic_IsTalkingSessionExist())
    {
        // 则播放铃声提示（最长1min）
        yl::string strRingPath = kszSysConfigPath + configParser_GetConfigString(kszRingForTranFailed);
        commonUnits_Hide2Real(strRingPath, PHFILE_TYPE_RING_FILE);

        _VoiceInterface.PlayRing(strRingPath.c_str());
        iTimerTranFailedAudioAlert = TRAN_FAILED_RING_DURATION;
        vInfo.m_iTranFailedToneTimes = 1;
    }
    else
    {
        // 耳麦和手柄则每隔5s播放嘟嘟声
        voice_PlayTone(TONE_TRAN_FAILED, 300);
        iTimerTranFailedAudioAlert = 5;
        vInfo.m_iTranFailedToneTimes = 12; // 5s一次，12次为60s
    }

    // call park失败，只需显示3s
    if (vInfo.m_bCallParkFailedAlert)
    {
        iTimerTranFailedAudioAlert = 3;
        vInfo.m_iTranFailedToneTimes = 1;
    }
    iTimerTranFailedAudioAlert *=  1000;
    vInfo.timerTranFailedAudioAlert.SetCallBack(MAKE_TIMER_CALL_BACK(CVoiceListener::OnTimerOut));
    vInfo.timerTranFailedAudioAlert % (int)ssP % iTimerTranFailedAudioAlert;
    vInfo.timerTranFailedAudioAlert.SetTimer(iTimerTranFailedAudioAlert);
}

// 退出转移失败提示
void CVoiceListener::ExitTransFailedAlert(CCallInfo& callInfo)
{
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL || !priv->IsTransFailed())
    {
        return;
    }
    VoiceInfo& vInfo = priv->m_VoiceInfo;
    vInfo.timerTranFailedAudioAlert.Kill();
    vInfo.m_iTranFailedToneTimes = 0;
    if (!talklogic_IsFoucsInRingSession() && voice_IsPlayingRing())
    {
        // 停止播放铃声
        voice_StopRing();
    }
    // 返回到正常通话
    callInfo.SetTransfer(false);
}

#ifdef IF_FEATURE_BROADSOFT_CALLPARK
// ParkNotify界面提示间隔(ms)
#define ALERT_PARK_NOTIFY   15 * 1000
// 设置Notify提示定时器
void CVoiceListener::SetNotifyAlertTimer(CCallInfo& callInfo)
{
    CListenInfo* priv = GetCallerData(&callInfo);
    if (priv == NULL)
    {
        return;
    }
    VoiceInfo& vInfo = priv->m_VoiceInfo;
    vInfo.timerParkNotify.SetCallBack(MAKE_TIMER_CALL_BACK(CVoiceListener::OnTimerOut));
    vInfo.timerParkNotify % callInfo.GetSessionID() % ((int)ALERT_PARK_NOTIFY);
    vInfo.timerParkNotify.SetTimer(ALERT_PARK_NOTIFY);
}
#endif
