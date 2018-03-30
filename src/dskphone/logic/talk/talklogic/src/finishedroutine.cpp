#include "talklogic_inc.h"

CFinishedRoutine::CFinishedRoutine()
{
    m_eToneType = TONE_BUSY;
}

CFinishedRoutine::~CFinishedRoutine()
{
}

// 创建Routine之后的处理
void CFinishedRoutine::AfterRoutineCreate()
{
    const TalkErrorInfo& errorInfo = m_pCallInfo->m_errorInfo;

    if (errorInfo.iReason != TARK_ERROR_CALL_RELEASED)
    {
        m_pCallInfo->m_bFinishedError = true;
    }
    else
    {
        int iHour = 0;
        int iMin = 0;
        int iSec = 0;

        sscanf(errorInfo.strError.c_str(), "%2d:%2d:%2d", &iHour, &iMin, &iSec);
        m_pCallInfo->SetCallDuration(iHour * 3600 + iMin * 60 + iSec);
    }
    // 进到callfinished界面就把通话停掉 bugid:17308
    StopTalk();
}

// Routine的定时器响应
bool CFinishedRoutine::OnTimer(UINT uTimerID)
{
    bool bHandled = false;
    return bHandled || CBaseRoutine::OnTimer(uTimerID);
}

// Routine的SIP消息响应
bool CFinishedRoutine::OnSIPMsg(msgObject& msg)
{
    // 暂无sip消息需要处理
    return false;
}
