#include "talklogic_inc.h"

CBaseTalkRoutine::CBaseTalkRoutine()
{
    m_iTimerHoldDelay = STOP_TIMER_VALUE;
} 

CBaseTalkRoutine::~CBaseTalkRoutine()
{
    // 杀掉hold防抖定时器
    CancelTimer(m_iTimerHoldDelay);
}

