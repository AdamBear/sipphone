#include "talklogic_inc.h"

// 来电响铃过程
CAocFinishRoutine::CAocFinishRoutine()
{
    m_iTimerShow = STOP_TIMER_VALUE;
}

CAocFinishRoutine::~CAocFinishRoutine()
{
    CancelTimer(m_iTimerShow);
}

// 创建Routine之后的处理
void CAocFinishRoutine::AfterRoutineCreate()
{
    // 设置定时器
    SetShowTimer();
}

// 销毁Routine之前的处理
void CAocFinishRoutine::BeforeRoutineDestroy(bool bJumpToNextRoutine/* = true*/)
{
    CBaseRoutine::BeforeRoutineDestroy();

    if (NULL != m_pCallInfo)
    {
        _AOCManager.ClearAOCData(m_pCallInfo->GetCallID());
    }
}

// Routine的定时器响应
bool CAocFinishRoutine::OnTimer(UINT uTimerID)
{
    bool bHandled = false;

    if (uTimerID == (UINT)&m_iTimerShow)
    {
        bHandled = true;
        CancelTimer(m_iTimerShow);

        // 定时器时间到, 退出该Routine
        m_pCallSession->ExitSession();
    }

    return bHandled || CBaseRoutine::OnTimer(uTimerID);
}

// 设置显示定时器
void CAocFinishRoutine::SetShowTimer()
{
    CancelTimer(m_iTimerShow);

    m_iTimerShow = configParser_GetConfigInt(kszAOCShowTime);
    m_iTimerShow = (m_iTimerShow > 15 ? 15 : m_iTimerShow);
    m_iTimerShow *= 1000;

    if (m_iTimerShow > 0)
    {
        SetTimerEx(m_iTimerShow);
    }
}
