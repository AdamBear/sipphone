#ifndef __X_THREAD_TIMER_H__
#define __X_THREAD_TIMER_H__

#include <dskthread/mkthread.h>
#include "xkerneldefine.h"


BEGIN_XKERNEL_NAMESPACE

//////////////////////////////////////////////////////////////////////////
// class xThreadTimer
class xThreadTimer;
class xTimerHandler;
typedef void (xTimerHandler::*timerMethod)(xThreadTimer* timer, LPVOID pExtraData);
#define TIMER_METHOD(pObj, meth) pObj, static_cast<timerMethod>(&meth)

class xThreadTimer : public CMKProcessor
{
public:
    xThreadTimer();
    ~xThreadTimer();

public:
    BOOL SetTimer(int nElapse, LPVOID pExtraData, xTimerHandler* pHandler, timerMethod pTimerMethod);
    BOOL SetTimer(int nElapse, xTimerHandler* pHandler, timerMethod pTimerMethod);
    BOOL SingleShot(int nElapse, LPVOID pExtraData, xTimerHandler* pHandler, timerMethod pTimerMethod);
    BOOL SingleShot(int nElapse, xTimerHandler* pHandler, timerMethod pTimerMethod);

    void KillTimer();

    BOOL IsTimerRuning();
    int GetTimerID();

    BOOL OnTimeOut(msgObject& refObj);

protected:
    mkThreadTimer       m_objTimer;
    xTimerHandler*      m_pTimerHandler;
    timerMethod         m_pTimerMethod;
    LPVOID              m_pExtraData;
    bool                m_bSingleShot;
};

class xTimerHandler
{
public:
    xTimerHandler();
    ~xTimerHandler();
};

END_XKERNEL_NAMESPACE

USING_XKERNEL_NAMESPACE

#endif
