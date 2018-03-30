#ifndef __WIN32_MKTHREAD_H__
#define __WIN32_MKTHREAD_H__

#if 0 // WIN32

#include "win32_adapter.h"

// 将对象地址作为定时器ID
#define TIMER_ID(timer)      (UINT(&(timer)))

#define MKIT_MSG_IDLE   0
#define TM_TIMER        1

struct msgObject
{
    long message;
    long wParam;
    long lParam;
};

class CMKThread
{
public:
    CMKThread(const char * cszTName);
    virtual ~CMKThread();

    virtual BOOL AfterThreadLaunched();

    virtual void BeforeThreadShutdown();

    virtual BOOL TryThreadMessage(msgObject & msg);

    // Launch a new thread
    bool Launch();

    // Destroy
    bool Destroy();


    // ETL定时器相关函数，暂且放在这里
    UINT timerSetThreadTimer(UINT nTimerID, UINT uTimerOut);
    void timerKillThreadTimer(UINT nTimerID);

protected:
    //struct TimerEntity
    //{
    //  UINT timerId;
    //  UINT interval;
    //  UINT timeLeft;
    //};
    //typedef std::map<UINT, TimerEntity> TimerMap;
    //TimerMap m_mapTimer;
    //CRITICAL_SECTION m_hSection;
    //HANDLE m_hTimerMapItemEvent;

    //HANDLE m_hThread;
    //HANDLE m_hThreadEvent;

    //static DWORD ThreadProc(LPVOID pParam);
};

#endif // WIN32

#endif // !__WIN32_MKTHREAD_H__
