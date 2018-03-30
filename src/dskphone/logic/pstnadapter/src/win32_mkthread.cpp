#include "win32_mkthread.h"

#if WIN32



CMKThread::CMKThread(const char * cszTName)
{
    //InitializeCriticalSection(&m_hSection);
    //m_hThreadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    //m_hTimerMapItemEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CMKThread::~CMKThread()
{
    //DeleteCriticalSection(&m_hSection);
    //CloseHandle(m_hThreadEvent);
    //CloseHandle(m_hTimerMapItemEvent);
}

BOOL CMKThread::AfterThreadLaunched()
{
    return FALSE;
}

void CMKThread::BeforeThreadShutdown()
{
}

BOOL CMKThread::TryThreadMessage(msgObject & msg)
{
    return FALSE;
}

bool CMKThread::Launch()
{
    //DWORD dwThreadId = 0;
    //m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CMKThread::ThreadProc, (void*)this, 0, &dwThreadId);
    //return m_hThread > 0;

    // PC工具无需实时监测上报的数据，只需同步调用即可。
    AfterThreadLaunched();
    return true;
}

bool CMKThread::Destroy()
{
    //return SetEvent(m_hThreadEvent);
    //return TerminateThread(m_hThread, 0);

    // PC工具无需实时监测上报的数据，只需同步调用即可。
    BeforeThreadShutdown();
    return true;
}

UINT CMKThread::timerSetThreadTimer(UINT nTimerID, UINT uTimerOut)
{
    //TimerEntity timer;
    //timer.timerId = nTimerID;
    //timer.interval = timer.timeLeft = uTimerOut;

    //EnterCriticalSection(&m_hSection);
    //m_mapTimer[nTimerID] = timer;
    //SetEvent(m_hTimerMapItemEvent);
    //LeaveCriticalSection(&m_hSection);

    return 0;
}

void CMKThread::timerKillThreadTimer(UINT nTimerID)
{
    //EnterCriticalSection(&m_hSection);
    //m_mapTimer.erase(nTimerID);
    //if (m_mapTimer.empty())
    //{
    //  ResetEvent(m_hThreadEvent);
    //}
    //LeaveCriticalSection(&m_hSection);
}

//DWORD CMKThread::ThreadProc(LPVOID pParam)
//{
//  CMKThread *self = (CMKThread*)pParam;
//
//  self->AfterThreadLaunched();
//
//  msgObject msg;
//  msg.message = TM_TIMER;
//  msg.wParam = msg.lParam = 0;
//
//  DWORD dwLastTime = GetTickCount();
//  DWORD dwNextTick = 0;
//  while (WaitForSingleObject(self->m_hThreadEvent, 1) == WAIT_TIMEOUT)
//  {
//      if (WaitForSingleObject(self->m_hTimerMapItemEvent, 1000) == WAIT_OBJECT_0)
//      {
//          dwNextTick = 0;
//          DWORD dwDiffTime = GetTickCount();
//          EnterCriticalSection(&(self->m_hSection));
//          for (TimerMap::iterator it = self->m_mapTimer.begin(); it != self->m_mapTimer.end(); ++it)
//          {
//              it->second.timeLeft -= dwDiffTime;
//              if (it->second.timeLeft <= 0)
//              {
//                  msg.wParam = it->first;
//                  self->TryThreadMessage(msg);
//                  it->second.timeLeft += it->second.interval;
//              }
//              if (dwNextTick > it->second.timeLeft)
//              {
//                  dwNextTick = it->second.timeLeft;
//              }
//          }
//          LeaveCriticalSection(&(self->m_hSection));
//          dwLastTime += dwDiffTime;
//          if (dwNextTick > 0)
//          {
//              Sleep(dwNextTick);
//          }
//      }
//  }
//
//  self->BeforeThreadShutdown();
//
//  return 0;
//}

#endif // WIN32
