#ifndef __X_EVENT_H__
#define __X_EVENT_H__

#include "xkerneldefine.h"
#include "ETLLib/ETLLib.hpp"
#include "yllist.h"
#include "xmessagehandler.h"


BEGIN_XKERNEL_NAMESPACE

//////////////////////////////////////////////////////////////////////////
// class xEvent_Socket
class xEvent_Socket
{
public:
    xEvent_Socket();
    ~xEvent_Socket();

public:
    BOOL OpenEvent();
    void CloseEvent();

    BOOL WaitEvent(int nTimeout = NEVER_TIMEOUT);
    BOOL SetEvent();
    void ResetEvent();

public:
    BOOL IsValid();
    SOCKET GetEventSocket();
    int GetEventPort();
    BOOL SendEvent(int nEventPort);
    BOOL RecvEvent();

private:
#if IF_XKERNEL_MULTI_THREAD_MSG
    chCriticalSection   m_csSocket;
#endif
    SOCKET              m_hSocket;
    int                 m_nBindPort;
};

//////////////////////////////////////////////////////////////////////////
// class xMsgHandlerListener
class xTaskListener
{
public:
    explicit xTaskListener(SOCKET hSocket, xMessageHandler* pHandler, msgMethod pMethod);
    ~xTaskListener();

public:
    inline SOCKET GetSocket() const
    {
        return m_hSocket;
    }
    inline xMessageHandler* GetHandler() const
    {
        return m_pHandler;
    }
    inline msgMethod GetMethod() const
    {
        return m_pMethod;
    }

    void HandlerTask();

private:
    SOCKET              m_hSocket;
    xMessageHandler*    m_pHandler;
    msgMethod           m_pMethod;
};

//////////////////////////////////////////////////////////////////////////
// class xTaskEvent
class xTaskEvent
{
    typedef YLList<xTaskListener*>  typeEventList;

public:
    xTaskEvent();
    ~xTaskEvent();

public:
    BOOL OpenTaskEvent(TASK_ID idTask, xMessageHandler* pHandler, msgMethod pMethod);
    void CloseTaskEvent();

    void WaitEvent(int nTimeout);
    BOOL SetEvent();
    void ResetEvent();

    BOOL AddListener(SOCKET hSocket, xMessageHandler* pHandler, msgMethod pMethod);
    BOOL RemoveListener(SOCKET hSocket);

    bool operator == (const xTaskEvent&) const
    {
        return false;
    }

    xEvent_Socket& GetSocketEvent();

private:
    xEvent_Socket       m_sockEvent;
    typeEventList       m_listEvent;
};

END_XKERNEL_NAMESPACE

#endif
