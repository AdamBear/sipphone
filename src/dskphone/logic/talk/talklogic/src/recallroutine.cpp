#include "talklogic_inc.h"

// 来电响铃过程
CRecallRoutine::CRecallRoutine()
{
}

CRecallRoutine::~CRecallRoutine()
{

}

// Routine的SIP消息响应
bool CRecallRoutine::OnSIPMsg(msgObject& msg)
{
    bool bHandled = true;

    switch (msg.message)
    {
    case SIP_CALL_RELEASED:
        {
            m_pCallSession->ExitSession();
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

// 拒绝新来电
void CRecallRoutine::RefuseIncomingCall(int iRefuseType/* = RT_NORMAL*/)
{
    m_pCallSession->ExitSession(true);
}

// 接起新来电
bool CRecallRoutine::AnswerIncomingCall(bool bSwitchChannel/* = true*/, bool bAudioOnly/* = true*/)
{
    m_pCallSession->EnterRoutine(ROUTINE_TALKING);
    m_pCallSession->HoldSession(false);
    // 需要刷新界面
    return true;
}

// 发消息通知对方结束通话
void CRecallRoutine::EndCall()
{
    CBaseRoutine::EndCall();
}
