#include "timepiece.h"
#include "commonapi/stringhelperapi.h"

TimePiece::TimePiece()
{
    m_uStartTime = 0;
    m_uTickTime = 0;
    m_uPauseTime = 0;
}

TimePiece::~TimePiece()
{
    StopTimer();
}

void TimePiece::StartTimer()
{

    m_RefreshTimer.SetTimer(1000, NULL, MK_MSG_ENTRY(this, TimePiece::OnTimeOut));

    if (m_uPauseTime == 0)
    {
        m_uStartTime = etlGetTickCount() - 100;
        m_uTickTime = 0;
    }
    else
    {
        UINT64 uNowTime = etlGetTickCount();
        if (uNowTime < m_uStartTime || uNowTime < m_uPauseTime)
        {
            chWARNING2(FALSE, commonAPI_FormatString("Start Time[%d], Now Time[%d], Pause Time[%d]",
                       m_uStartTime, uNowTime, m_uPauseTime).c_str());
        }
        chASSERT(uNowTime >= m_uStartTime && uNowTime >= m_uPauseTime);

        //adjust start time
        m_uStartTime += (uNowTime - m_uPauseTime);
        m_uPauseTime = 0;
    }
}

void TimePiece::StopTimer()
{
    if (m_RefreshTimer.IsTimerRuning())
    {
        m_RefreshTimer.KillTimer();
    }

    m_uStartTime = 0;
    m_uPauseTime = 0;
    m_uTickTime = 0;
}

void TimePiece::PauseTimer()
{
    if (m_uPauseTime == 0)
    {
        m_uPauseTime = etlGetTickCount();

        if (m_RefreshTimer.IsTimerRuning())
        {
            m_RefreshTimer.KillTimer();
        }
    }
}

BOOL TimePiece::IsTimerStart()
{
    return (BOOL)(m_RefreshTimer.IsTimerRuning());
}

BOOL TimePiece::IsTimerPause()
{
    return (BOOL)(m_uPauseTime != 0);
}

void TimePiece::GetTime(UINT & uHour, UINT & uMinute, UINT & uSecond)
{
    uHour = m_uTickTime / 3600;
    uMinute = (m_uTickTime / 60) % 60;
    uSecond = m_uTickTime % 60;
}

void TimePiece::SetTime(UINT uHour, UINT uMinute, UINT uSecond)
{
    if (uMinute > 59)
    {
        uMinute = 59;
    }

    if (uSecond > 59)
    {
        uSecond = 59;
    }

    UINT64 uTempTime = (uHour * 3600 + uMinute * 60 + uSecond) * 1000;

    m_uStartTime = etlGetTickCount() - uTempTime;
}

BOOL TimePiece::OnTimeOut(msgObject & objMessage)
{
    UINT64 uNowTime = etlGetTickCount();
    if (uNowTime < m_uStartTime)
    {
        chWARNING2(FALSE, commonAPI_FormatString("Start Time[%d], Now Time[%d]", m_uStartTime,
                   uNowTime).c_str());
        return FALSE;
    }

    UINT64 uNewTime = (uNowTime - m_uStartTime) / 1000;
    if (m_uTickTime != uNewTime)
    {
        m_uTickTime = uNewTime;
    }

    return TRUE;
}
