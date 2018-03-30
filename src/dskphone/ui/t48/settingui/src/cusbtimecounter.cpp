#include "cusbtimecounter.h"
#include "usbuicommon.h"

CUSBTimeCounter::CUSBTimeCounter()
    : m_nTotalSeconds(0)
    , m_nCurrentSeconds(0)
{
    InitData();
}

CUSBTimeCounter::~CUSBTimeCounter()
{

}

void CUSBTimeCounter::InitData()
{
    //使用TimeCounter的话需要实现OnTimeCountOut的槽
//  QObject::connect(&m_timer, SIGNAL(timeout()), MyListener, SIGNAL(TimerCountOut()));
}

void CUSBTimeCounter::SetCountTime()
{
    StopCountTime();
    m_timer.start(1000);
}

void CUSBTimeCounter::StopCountTime()
{
    if (m_timer.isActive())
    {
        m_timer.stop();
    }
}

long CUSBTimeCounter::CalculateSeekTimePointByPercent(int nPercent)
{
    return (long)((float)nPercent / USB_PROCESSBAR_TOTAL * m_nTotalSeconds);
}

long CUSBTimeCounter::CalculatePausePointByCurrentTime()
{
    return m_nCurrentSeconds;
}
