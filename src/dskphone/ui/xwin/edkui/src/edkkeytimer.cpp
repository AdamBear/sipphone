#include "edkkeytimer.h"
#include "edkui/include/modedkui.h"
#include "edk/include/modedk.h"

#define EDK_VOLUMNKEY_LONG_PRESS_TIME       2000

IMPLEMENT_GETINSTANCE(CEdkKeyTimer)

CEdkKeyTimer::CEdkKeyTimer()
{
}

CEdkKeyTimer::~CEdkKeyTimer()
{
    StopKeyTimer();
}

//开启定时器
void CEdkKeyTimer::StartKeyTimer()
{
    m_timerKey.SingleShot(EDK_VOLUMNKEY_LONG_PRESS_TIME, TIMER_METHOD(this,
                          CEdkKeyTimer::OnLongKeyTimer));
}

//停止定时器
void CEdkKeyTimer::StopKeyTimer()
{
    if (m_timerKey.IsTimerRuning())
    {
        m_timerKey.KillTimer();
    }
}

void CEdkKeyTimer::OnLongKeyTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    StopKeyTimer();
    EdkUI_SetHintMode(!EDK_IsTipModeEnable());
}
