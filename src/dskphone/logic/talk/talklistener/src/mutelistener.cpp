#include "talklistener_inc.h"

bool CMuteListener::m_bGlobalMute = false;

CMuteListener::CMuteListener()
    : LSN_CLS_LOG_NAME
{
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CMuteListener::OnRoutineChanged);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CMuteListener::OnSessionExit);
    REG_SYNC_SIGNAL(TALK_SIG_ACTION_EVENT, CMuteListener::OnUiAction);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_FOCUS_CHANGE, CMuteListener::OnSessionFocusChange);
    RegETLMsg(CONFIG_MSG_BCAST_CHANGED);
}

DECL_SIGNAL_HANDLER(CMuteListener::OnRoutineChanged)
{
    CSessionProxy sp(sigp.iSessionID);
    if (sp.IsInherits(ROUTINE_TALKING))
    {
        if (IsNeedMuteAudio(sp.GetCallInfo()))
        {
            SetMuteStatus(true);
            voice_SetMute(sigp.iCallID, GetMuteStatus());
            return true;
        }
    }
    voice_SetMute(sigp.iCallID, GetMuteStatus());
    SetLight();
    return false;
}

DECL_SIGNAL_HANDLER(CMuteListener::OnSessionExit)
{
    //不存在通话会话情况
    if (!talklogic_IsTalkSessionExist() && !talklogic_IsLocalConfSessionExist())
    {
        if (configParser_GetConfigInt(kszMuteMode) == 2)
        {
            SetMuteStatus(true);
        }
        else if (configParser_GetConfigInt(kszMuteMode) == 0
                 && GetMuteStatus())
        {
            SetMuteStatus(false);
        }
    }
    // session退出,强制设置mute为false
    voice_SetMute(sigp.iCallID, false);
    return true;
}

DECL_SIGNAL_HANDLER(CMuteListener::OnUiAction)
{
    DataUI2Logic* pData = (DataUI2Logic*)sigp.wParam.pValue;
    if (pData == NULL)
    {
        return false;
    }
    TALK_ACTION_TYPE eAction = pData->eAction;
    if (TALK_ACTION_MUTE != eAction)
    {
        return false;
    }

    CSessionProxy sp(sigp.iSessionID);
    if (!talklogic_GetSessionNum(false))
    {
        //idle界面下可切换mute情况
        if (configParser_GetConfigInt(kszMuteMode) == 1)
        {
            SetMuteStatus(!GetMuteStatus());
        }
    }
    else
    {
        SessionMuteStateChange(sp);
    }
    return true;
}

DECL_SIGNAL_HANDLER(CMuteListener::OnSessionFocusChange)
{
    CSessionProxy sp(sigp.iSessionID);
    if (sp.GetRoutineState() == ROUTINE_TALKING
            || sp.GetRoutineState() == ROUTINE_LINE_DEVICE)
    {
#if IF_FEATURE_LOOPBACK
        CCallInfo *pCallInfo = sp.GetCallInfo();
        if (pCallInfo && pCallInfo->m_bIsLoopBackCall)
        {
            //停止通话，根据当前是否Mute显示Mute状态
            sp.StopTalk();
            bool bMute = talklogic_GetMuteStatus();
            TALK_INFO("---------GetMuteStatus[%d]", bMute);
            if (bMute)
            {
                voice_SetMute(pCallInfo->GetCallID(), bMute);
            }
        }
#endif
    }
    return true;
}

bool CMuteListener::SessionMuteStateChange(const CSessionProxy& sp)
{
    bool ret = false;
    if (sp.IsSingle())
    {
        ret = SingleSessionMuteChange(sp);
    }
    else if (sp.IsLocalConf())
    {
        if ((sp.IsHold() || sp.IsAllHeld())
                || configParser_GetConfigInt(kszAllowMute) == 0)
        {
            return false;
        }
        SetMuteStatus(!GetMuteStatus());
        // VPM发消息设置mute
        voice_SetMute(1, GetMuteStatus());
        return true;
    }
    else if (sp.IsBeTransfer())
    {
        CSessionProxy talkSession = sp.GetSubByIndex(0);
        ret = SingleSessionMuteChange(talkSession);
    }

    return ret;
}

bool CMuteListener::IsNeedMuteAudio(CCallInfo* pCallInfo)
{
    if (pCallInfo == NULL)
    {
        return false;
    }
    /*
    * 1、自动接听静音
    * 2、账号自动接听静音
    * 3、Intercom 自动接听静音
    * 4、全局静音
    */
    //全局静音
    if (configParser_GetConfigInt(kszMuteMode) == 2
            || (configParser_GetConfigInt(kszMuteMode) == 1
                && GetMuteStatus()))
    {
        return true;
    }

    // Intercom 自动接听静音
    if (pCallInfo->m_bIsIntercom
            && pCallInfo->m_bIsAutoAnswerCall
            && configParser_GetConfigInt(kszIntercomMuteSwitch) != 0)
    {
        return true;
    }

    // 账号自动接听静音/全局自动接听静音
    // 只要有一个开启了，就以account开关为准，及账号自动接听优先级更高
    if (!pCallInfo->m_bIsIntercom
            && pCallInfo->m_bIsAutoAnswerCall
            && (configParser_GetConfigInt(kszAutoAnswerAudioMute) == 1
                || acc_GetCfgItemIntValue(kszAccountAutoAnswerMuteSwitch, pCallInfo->GetAccountID()) == 1))
    {
        return true;
    }

#if IF_FEATURE_LOOPBACK
    if (pCallInfo->m_bIsLoopBackCall)
    {
        return true;
    }
#endif

    return false;
}

bool CMuteListener::OnMessage(msgObject& msg)
{
    bool bHandled = true;
    if (msg.message != CONFIG_MSG_BCAST_CHANGED)
    {
        return false;
    }

    switch (msg.wParam)
    {
    case ST_MUTE_MODE:
        {
            if (msg.lParam == 1 && configParser_GetConfigInt(kszAllowMute) == 0
                    || msg.lParam == 0 && configParser_GetConfigInt(kszMuteMode) == 0)
            {
                SetMuteStatus(false);
            }
            else if (msg.lParam == 0 && configParser_GetConfigInt(kszMuteMode) == 2)
            {
                SetMuteStatus(true);
            }
        }
        break;
    case ST_MUTE_STRATEGY_MODE:
        {
#warning zqs
            //kszMuteMode  kszMuteStrategy 两者映射为一个配置
            if (configParser_GetConfigInt(kszMuteMode) == 2)
            {
                SetMuteStatus(true);
            }
            else if (configParser_GetConfigInt(kszMuteMode) == 0)
            {
                SetMuteStatus(false);
            }
        }
        break;
    default:
        bHandled = false;
        break;
    }
    return bHandled;
}
bool CMuteListener::SingleSessionMuteChange(const CSessionProxy& sp)
{
    CCallInfo* pCallInfo = sp.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }
    // 1.在Hold或者被Hold的状态下, VPM不存在通话话路, Mute没有意义
    // 2.Keep mute开启,不判断hold/held状态(因为该情况下设置vpm状态也是可以生效的)
    if ((configParser_GetConfigInt(kszMuteMode) == 0 && sp.IsInHoldOrHeld())
            || configParser_GetConfigInt(kszAllowMute) == 0)
    {
        // http://10.2.1.199/Bug.php?BugID=7738, 增加mute开关
        return false;
    }
    SetMuteStatus(!GetMuteStatus());
    // VPM发消息设置mute
    voice_SetMute(pCallInfo->GetCallID(), GetMuteStatus());
    return true;
}

bool CMuteListener::GetMuteStatus()
{
    return m_bGlobalMute;
}

bool CMuteListener::SetMuteStatus(bool bMute)
{
    // Mute状态未改变，不处理
    if (GetMuteStatus() == bMute)
    {
        return false;
    }
    m_bGlobalMute = bMute;

    SignalParams sigp;
    sigp.wParam.bValue = !bMute;
    sigp.lParam.bValue = bMute;
    // 通知事件
    modtklsn_emit(TALK_SIG_SESSION_MUTE_CHANGE, sigp);

    //应该要放到IDLESCREEN 去做，但是先留在这里
    if (bMute)
    {
        idleScreen_SetStatus(PS_STATE_TALK_MUTE);
    }
    else
    {
        idleScreen_DropStatus(PS_STATE_TALK_MUTE);
    }

    // 灯控制
    SetLight();
    return true;
}

void CMuteListener::SetLight()
{
    //更新电源灯
    powerled_FlushPowerLed();
    //更新Mute灯: 上次是mute，mute灯也要亮
    ledlamp_PostEvent(LAMP_EVENTTYPE_MUTE, GetMuteStatus());
}
