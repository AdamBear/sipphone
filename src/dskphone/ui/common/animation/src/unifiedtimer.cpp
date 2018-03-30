#include "unifiedtimer.h"
#include "animationbase.h"
#if ANIMATION_DEBUG
#include "dsklog/log.h"
#endif


CUnifiedTimer::CUnifiedTimer(CAnimationBase* pAnimation)
    : m_pAnimation(pAnimation)
    , m_u64Time(0)
{
#if ANIMATION_DEBUG
    XWINDOW_INFO("New CUnifiedTimer:%p", this);
#endif
}

CUnifiedTimer::~CUnifiedTimer()
{
    m_timer.KillTimer();

#if ANIMATION_DEBUG
    XWINDOW_INFO("Delete CUnifiedTimer:%p", this);
#endif
}

void CUnifiedTimer::ResetAnimation()
{
    m_pAnimation = NULL;
}

void CUnifiedTimer::Start(int nInterval)
{
    if (nInterval <= 0)
    {
        return;
    }

    m_u64Time = etlGetTickCount();

    m_timer.SetTimer(nInterval, TIMER_METHOD(this, CUnifiedTimer::OnAnimationInterval));

#if ANIMATION_DEBUG
    XWINDOW_INFO("Start Timer:%p Interval:%d", this, nInterval);
#endif
}

void CUnifiedTimer::Stop()
{
    if (m_timer.IsTimerRuning())
    {
        m_timer.KillTimer();

#if ANIMATION_DEBUG
        XWINDOW_INFO("Stop Timer:%p", this);
#endif
    }
}

void CUnifiedTimer::OnAnimationInterval(xThreadTimer* timer, LPVOID pExtraData)
{
    UINT64 u64Time = etlGetTickCount();
    int nIntervale = 0;

    if (u64Time >= m_u64Time)
    {
        nIntervale = (int)(u64Time - m_u64Time);
    }
    else
    {
        nIntervale = (int)u64Time;
    }

    m_u64Time = u64Time;

    if (NULL != m_pAnimation)
    {
        m_pAnimation->OnInterval(nIntervale);
    }
}
