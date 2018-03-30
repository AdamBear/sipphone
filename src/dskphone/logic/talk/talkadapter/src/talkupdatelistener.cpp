#include "talkadapter_inc.h"
#include "broadsoft/callcenter/include/modcallcenter.h"

CTalkUpdateListener::CTalkUpdateListener()
    : LSN_CLS_LOG_NAME, m_pDelayUpdate(new CScopedTimer)
{
    m_pDelayUpdate->SetCallBack(MAKE_TIMER_CALL_BACK(CTalkUpdateListener::OnUpdate));

    RegETLMsg(SYS_MSG_NOTIFY_PHY_STATUS);
    RegETLMsg(ACD_MESSAGE_QUEUE_STATUS);
#ifdef IF_FEATURE_PSTN
    // 收到flash结果刷新界面即可，CTalkingUI会重新获取状态
    RegETLMsg(PSTN_MSG_FLASH_RESULT);
#endif // IF_FEATURE_PSTN

    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CTalkUpdateListener::OnStateChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_FOCUS_CHANGE, CTalkUpdateListener::OnDataChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HIDE_CHANGE, CTalkUpdateListener::OnDataChange);

    /* 开关通话VPM优先级高 */
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_START_TALK, CTalkUpdateListener::OnStartTalk);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HELD_CHANGE, CTalkUpdateListener::OnStartTalk);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HOLD_CHANGE, CTalkUpdateListener::OnStartTalk);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ANSWER, CTalkUpdateListener::OnStartTalk);

    /* 通话信息改变意味着马上可能就要需要开关通话 */
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_INFO_CHANGED, CTalkUpdateListener::OnCallInfoChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_REMOTE_INFO_CHANGE, CTalkUpdateListener::OnCallInfoChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_CALLID_IMAGE_CHANGE, CTalkUpdateListener::OnCallInfoChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_AFTER_CREATE, CTalkUpdateListener::OnSessionCreate);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_AFTER_DELETE, CTalkUpdateListener::OnDataChange);

    REG_SYNC_SIGNAL(TALK_SIG_SESSION_MUTE_CHANGE, CTalkUpdateListener::OnDataChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_FARHOLD_CHANGE, CTalkUpdateListener::OnDataChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_FARMUTE_CHANGE, CTalkUpdateListener::OnDataChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ACCOUNT_CHANGE, CTalkUpdateListener::OnDataChange);

    /* 接收ui刷新通知 */
    REG_SYNC_SIGNAL(TALK_SIG_GET_DATA_TO_UI, CTalkUpdateListener::OnUpdateUI);
}

bool CTalkUpdateListener::OnMessage(msgObject& msg)
{
    /* 网络变更需要刷新提示信息 */
    CScopedWNDUpdate::SetMethod(WND_UPDATE_SYNC);
    return true;
}

int CTalkUpdateListener::OnUpdate(CScopedTimer& timer)
{
    /* 异步刷新 */
    CScopedWNDUpdate method(WND_UPDATE_FORCE_ASYNC);
    return true;
}

DECL_SIGNAL_HANDLER(CTalkUpdateListener::OnStateChange)
{
    ROUTINE_STATE eNewRoutine = (ROUTINE_STATE)sigp.lParam.iValue;

    /* 需要强制执行退出其它页面 */
    if (eNewRoutine == ROUTINE_PARK_NOTIFY
            || Comm_IsRoutineInherits(eNewRoutine, ROUTINE_DIALING_CLS)
            || Comm_IsRoutineInherits(eNewRoutine, ROUTINE_TALKING_CLS))
    {
        CScopedWNDUpdate::ExitOtherModule();
    }

    /* 退出操作不刷新 */
    if (eNewRoutine != ROUTINE_UNKNOWN)
    {
        /* routine切换需要强制刷新 */
        CScopedWNDUpdate::SetMethod(WND_UPDATE_FORCE_SYNC);
    }

    return true;
}

/* 数据变更按适配之前版本异步刷新 */
DECL_SIGNAL_HANDLER(CTalkUpdateListener::OnDataChange)
{
    CScopedWNDUpdate::SetMethod(WND_UPDATE_SYNC);
    return true;
}

DECL_SIGNAL_HANDLER(CTalkUpdateListener::OnCallInfoChange)
{
    CScopedWNDUpdate::SetMethod(WND_UPDATE_ASYNC);
    /* 普通的更新信息，延迟刷新，但不能无限延时，保留前一个设置的时间 */
    if (!m_pDelayUpdate->IsOn() || m_pDelayUpdate->IsError())
    {
        m_pDelayUpdate->SetTimerOnce(500);
    }

    return true;
}

DECL_SIGNAL_HANDLER(CTalkUpdateListener::OnStartTalk)
{
    CScopedWNDUpdate::SetMethod(WND_UPDATE_FORCE_ASYNC);
    /* 开启通话，之后必须等待150ms，待vpm优先开启通话 */
    m_pDelayUpdate->SetTimerOnce(200);

    return true;
}

DECL_SIGNAL_HANDLER(CTalkUpdateListener::OnUpdateUI)
{
    m_pDelayUpdate->Kill();
    return true;
}

DECL_SIGNAL_HANDLER(CTalkUpdateListener::OnSessionCreate)
{
    CSessionProxy proxy(sigp.iSessionID);
    if (proxy.IsLocalConf())
    {
        CScopedWNDUpdate::ExitOtherModule();
    }

    if (!proxy.IsHide())
    {
        CScopedWNDUpdate::SetMethod(WND_UPDATE_SYNC);
    }

    return true;
}
