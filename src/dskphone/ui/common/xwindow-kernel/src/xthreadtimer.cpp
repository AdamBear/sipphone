#include "xthreadtimer.h"


BEGIN_XKERNEL_NAMESPACE

//////////////////////////////////////////////////////////////////////////
// class xThreadTimer
xThreadTimer::xThreadTimer()
    : m_pTimerHandler(NULL)
    , m_pTimerMethod(NULL)
    , m_pExtraData(NULL)
    , m_bSingleShot(false)
{
}

xThreadTimer::~xThreadTimer()
{
    KillTimer();
}

BOOL xThreadTimer::SingleShot(int nElapse, xTimerHandler* pHandler, timerMethod pTimerMethod)
{
    m_pExtraData = NULL;

    SetTimer(nElapse, NULL, pHandler, pTimerMethod);

    m_bSingleShot = true;

    return TRUE;
}

BOOL xThreadTimer::SingleShot(int nElapse, LPVOID pExtraData, xTimerHandler* pHandler, timerMethod pTimerMethod)
{
    m_pExtraData = pExtraData;

    SetTimer(nElapse, pExtraData, pHandler, pTimerMethod);

    m_bSingleShot = true;

    return TRUE;
}

BOOL xThreadTimer::SetTimer(int nElapse, xTimerHandler* pHandler, timerMethod pTimerMethod)
{
    SetTimer(nElapse, NULL, pHandler, pTimerMethod);
    m_pExtraData = NULL;

    return TRUE;
}

BOOL xThreadTimer::SetTimer(int nElapse, LPVOID pExtraData, xTimerHandler* pHandler, timerMethod pTimerMethod)
{
    if (nElapse <= 0 || NULL == pHandler || NULL == pTimerMethod)
    {
        return FALSE;
    }

    KillTimer();

    m_pTimerHandler = pHandler;
    m_pExtraData = pExtraData;

    m_pTimerMethod = pTimerMethod;

    m_objTimer.SetTimer(nElapse, pExtraData, MK_MSG_ENTRY(this, xThreadTimer::OnTimeOut));

    return TRUE;
}

void xThreadTimer::KillTimer()
{
    if (m_objTimer.IsTimerRuning())
    {
        m_bSingleShot = false;

        m_objTimer.KillTimer();
    }
}

BOOL xThreadTimer::IsTimerRuning()
{
    return m_objTimer.IsTimerRuning();
}

int xThreadTimer::GetTimerID()
{
    return m_objTimer.GetTimerID();
}

BOOL xThreadTimer::OnTimeOut(msgObject& refObj)
{
    if (m_bSingleShot)
    {
        m_bSingleShot = false;
        m_objTimer.KillTimer();
    }

    if (NULL != m_pTimerHandler && NULL != m_pTimerMethod)
    {
        (m_pTimerHandler->*m_pTimerMethod)(this, m_pExtraData);
    }

    return TRUE;
}

xTimerHandler::xTimerHandler()
{

}

xTimerHandler::~xTimerHandler()
{

}

END_XKERNEL_NAMESPACE
