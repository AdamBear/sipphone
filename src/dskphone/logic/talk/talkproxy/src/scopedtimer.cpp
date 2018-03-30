#include "scopedtimer.h"
#include "dsklog/log.h"
#include "etlmsghandler/modetlmsghandler.h"

CScopedTimer::~CScopedTimer()
{
    /* warning: 析构函数中不得调用虚函数 */
    CScopedTimer::Kill();
}

bool CScopedTimer::OnTimeOut(msgObject& msg)
{
    if (msg.wParam != TIMER_ID(m_uInterval))
    {
        return false;
    }

    if (m_uCount == 1)
    {
        Kill();
    }
    else if (m_uCount > 1)
    {
        --m_uCount;
    }

    /* warning: 回调函数里面可能会有 delete this 操作 */
    return m_pCallBack && m_pCallBack(*this);
}

void CScopedTimer::Kill()
{
    if (IsOn())
    {
        m_uInterval = 0;
        m_uCount = 0;

        timerKillThreadTimer(TIMER_ID(m_uInterval));
        SingleMsgUnReg(TM_TIMER, CMSGSubscriber(this, &CScopedTimer::OnTimeOut));
    }
}

bool CScopedTimer::SetTimer(size_type ms, size_type count/* = timer_repeated*/)
{
    if (ms == 0)
    {
        return false;
    }

    if (IsOn())
    {
        timerKillThreadTimer(TIMER_ID(m_uInterval));
    }
    else
    {
        SingleMsgReg(TM_TIMER, CMSGSubscriber(this, &CScopedTimer::OnTimeOut));
    }

    m_uCount = count;
    m_uInterval = ms;

    // TODO: 需考虑定时器溢出的情况
    if (timerSetThreadTimer(TIMER_ID(m_uInterval), ms) == 0)
    {
        m_uCount = timer_error;
        APP_ERR("CScopedTimer::SetTimer overflow timer[%d] count[%d] id[%p][%#x]", ms, count, this, TIMER_ID(m_uInterval));
        return false;
    }

    return true;
}


