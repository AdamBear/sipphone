#include "talklogic_inc.h"

// 呼出连回铃过程
CRingbackRoutine::CRingbackRoutine()
{
}

CRingbackRoutine::~CRingbackRoutine()
{
    CancelTimer(m_iTimerCancelCall);
}

// 创建Routine之后的处理
void CRingbackRoutine::AfterRoutineCreate()
{
    // 呼出无应答时，定时取消呼出
    SetAutoCancelCallTimer();
}

// 处理ui发送过来的Action事件
bool CRingbackRoutine::OnActionEvent(DataUI2Logic* pActionData)
{
    if (pActionData == NULL)
    {
        return false;
    }

    bool bHandled = false;
    switch (pActionData->eAction)
    {
    case TALK_ACTION_TRAN:
        {
            if (m_pCallSession->GetSponsorSession(SPONSOR_TRAN) != NULL)
            {
                bHandled = true;
#if IF_BUG_21754
                etlSleep(500);
#endif
                m_pCallSession->TranCurrentSession();
            }
        }
        break;
    default:
        bHandled = CConnectingRoutine::OnActionEvent(pActionData);
        break;
    }

    return bHandled;
}

// 获取对应session的状态
SESSION_STATE CRingbackRoutine::GetHostSessionState()
{
    if (m_pCallSession->GetSponsorSession(SPONSOR_TRAN) != NULL)
    {
        return SESSION_RINGBACK_TRAN;
    }
    else
    {
        return SESSION_RINGBACK;
    }
}
