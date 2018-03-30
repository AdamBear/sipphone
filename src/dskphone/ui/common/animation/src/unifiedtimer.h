#ifndef __UNIFIED_TIMER_H__
#define __UNIFIED_TIMER_H__

#include "ETLLib/ETLLib.hpp"
#include "xwindow-kernel/include/xthreadtimer.h"

class CAnimationBase;

class CUnifiedTimer : public xTimerHandler
{
public:
    explicit CUnifiedTimer(CAnimationBase* pAnimation);
    ~CUnifiedTimer();

public:
    void ResetAnimation();

    void Start(int nInterval);
    void Stop();

    void OnAnimationInterval(xThreadTimer* timer, LPVOID pExtraData);

protected:
    CAnimationBase* m_pAnimation;
    xThreadTimer m_timer;
    UINT64 m_u64Time;
};

#endif
