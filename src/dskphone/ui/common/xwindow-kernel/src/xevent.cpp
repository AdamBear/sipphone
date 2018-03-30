#include "xevent.h"
#include "xmessagehandler.h"
#include "xkernelcommon.h"
#include "dsklog/log.h"


BEGIN_XKERNEL_NAMESPACE
//////////////////////////////////////////////////////////////////////////
// class xEvent_Socket
xEvent_Socket::xEvent_Socket()
    : m_hSocket(INVALID_SOCKET)
    , m_nBindPort(0)
{
}

xEvent_Socket::~xEvent_Socket()
{
    CloseEvent();
}

BOOL xEvent_Socket::IsValid()
{
#if IF_XKERNEL_MULTI_THREAD_MSG
    chCriticalLock csAutoLock(&m_csSocket);
#endif

    return (INVALID_SOCKET != m_hSocket);
}

SOCKET xEvent_Socket::GetEventSocket()
{
#if IF_XKERNEL_MULTI_THREAD_MSG
    chCriticalLock csAutoLock(&m_csSocket);
#endif

    SOCKET hSocket = m_hSocket;

    return hSocket;
}

int xEvent_Socket::GetEventPort()
{
#if IF_XKERNEL_MULTI_THREAD_MSG
    chCriticalLock csAutoLock(&m_csSocket);
#endif

    int nBindPort = m_nBindPort;

    return nBindPort;
}

BOOL xEvent_Socket::OpenEvent()
{
    XWINDOW_INFO("OpenEvent:%p", this);

#if IF_XKERNEL_MULTI_THREAD_MSG
    chCriticalLock csAutoLock(&m_csSocket);
#endif

    if (INVALID_SOCKET == m_hSocket)
    {
        sockaddr_in addrServer;

        addrServer.sin_family = PF_INET;
        addrServer.sin_port = htons(0);
        addrServer.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        socklen_t lenAddr = sizeof(addrServer);

        m_hSocket = socket(PF_INET, SOCK_DGRAM, 0);

        if (INVALID_SOCKET != m_hSocket) // got -1 if invoked in do_exit()
        {
            int res = bind(m_hSocket, (const sockaddr*)&addrServer, lenAddr);
            chWARNINGx(res != SOCKET_ERROR, etlTransLastError());

            res = getsockname(m_hSocket, (sockaddr*)&addrServer, &lenAddr);
            chWARNINGx(res != SOCKET_ERROR, etlTransLastError());

            m_nBindPort = ntohs(addrServer.sin_port);
            chWARNINGx(m_nBindPort != 0, etlTransLastError());
        }
    }

    return INVALID_SOCKET != m_hSocket;
}

void xEvent_Socket::CloseEvent()
{
    XWINDOW_INFO("CloseEvent:%p", this);

#if IF_XKERNEL_MULTI_THREAD_MSG
    chCriticalLock csAutoLock(&m_csSocket);
#endif

    if (INVALID_SOCKET != m_hSocket)
    {
        int result = close(m_hSocket);

#if LINUX_SYSTEM
        chWARNINGx(result != -1, etlTransLastError());
#else
        chWARNINGx(result != -1 || etlGetLastError() == 10093, etlTransLastError());
#endif

        m_hSocket = INVALID_SOCKET;
    }

    m_nBindPort = 0;
}

BOOL xEvent_Socket::WaitEvent(int nTimeout)
{
    SOCKET hSocket = GetEventSocket();

    if (INVALID_SOCKET != hSocket)
    {
        chFD_SET fd(hSocket);

        if (1 == fd.readSelect(nTimeout))
        {
#if IF_XKERNEL_MULTI_THREAD_MSG
            chCriticalLock csAutoLock(&m_csSocket);
#endif

            if (INVALID_SOCKET == m_hSocket)
            {
                return FALSE;
            }

            char buf[64];
            sockaddr_in addrClient;
            int lenAddr = sizeof(addrClient);
            int nRes = recvfrom(m_hSocket, buf, 1, 0, (sockaddr*)&addrClient, &lenAddr);

            if (nRes <= 0)
            {
                XWINDOW_WARN("recvfrom:%p fail, Result:%d", this, nRes);
            }

            return nRes > 0;
        }
    }

    return FALSE;
}

BOOL xEvent_Socket::SetEvent()
{
    return SendEvent(m_nBindPort);
}

void xEvent_Socket::ResetEvent()
{
#if IF_XKERNEL_MULTI_THREAD_MSG
    chCriticalLock csAutoLock(&m_csSocket);
#endif

    chASSERT(INVALID_SOCKET != m_hSocket);

    char buf[64];
    sockaddr_in addrClient;
    int lenAddr = sizeof(addrClient);
    int res = sizeof(buf);
    int nLoop = 0;

    while (res > 0)
    {
        DEAD_LOOP_BREAK(nLoop)

        chFD_SET fd(m_hSocket);

        if (1 == fd.readSelect(0))
        {
            res = recvfrom(m_hSocket, buf, 1, 0, (sockaddr*)&addrClient, &lenAddr);

            if (res <= 0)
            {
                XWINDOW_WARN("recvfrom:%p fail, Result:%d", this, res);
            }
        }
        else
        {
            res = -1;
        }
    }
}

BOOL xEvent_Socket::SendEvent(int nEventPort)
{
#if IF_XKERNEL_MULTI_THREAD_MSG
    chCriticalLock csAutoLock(&m_csSocket);
#endif

    if (INVALID_SOCKET != m_hSocket && nEventPort > 0)
    {
        sockaddr_in addrTarget;

        addrTarget.sin_family = PF_INET;
        addrTarget.sin_port = htons(nEventPort);
        addrTarget.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        int res = sendto(m_hSocket, "x", 1, 0, (sockaddr*)&addrTarget, sizeof(addrTarget));

        if (1 != res)
        {
            XWINDOW_WARN("sendto:%p fail, Result:%d", this, res);

            return FALSE;
        }

        return TRUE;
    }

    return FALSE;
}

BOOL xEvent_Socket::RecvEvent()
{
#if IF_XKERNEL_MULTI_THREAD_MSG
    chCriticalLock csAutoLock(&m_csSocket);
#endif

    if (INVALID_SOCKET == m_hSocket)
    {
        return FALSE;
    }

    char buf[64];
    sockaddr_in addrClient;
    int lenAddr = sizeof(addrClient);

    int nRes = recvfrom(m_hSocket, buf, 1, 0, (sockaddr*)&addrClient, &lenAddr);

    if (1 != nRes)
    {
        XWINDOW_WARN("recvfrom:%p fail, Result:%d", this, nRes);

        return FALSE;
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// class xTaskListener
xTaskListener::xTaskListener(SOCKET hSocket, xMessageHandler* pHandler, msgMethod pMethod)
{
    m_hSocket = hSocket;
    m_pHandler = pHandler;
    m_pMethod = pMethod;
}

xTaskListener::~xTaskListener()
{

}

void xTaskListener::HandlerTask()
{
    if (NULL == m_pHandler)
    {
        return;
    }

    if (NULL_METHOD == m_pMethod)
    {
        m_pHandler->TryThreadMessage(NULL_MESSAGE);
    }
    else
    {
        (m_pHandler->*m_pMethod)(NULL_MESSAGE);
    }
}

//////////////////////////////////////////////////////////////////////////
// class xTaskEvent
xTaskEvent::xTaskEvent()
{
}

xTaskEvent::~xTaskEvent()
{
}

xEvent_Socket& xTaskEvent::GetSocketEvent()
{
    return m_sockEvent;
}

BOOL xTaskEvent::OpenTaskEvent(TASK_ID idTask, xMessageHandler* pHandler, msgMethod pMethod)
{
    chASSERT(etlGetCurrentTaskId() == idTask);

    if (!m_sockEvent.OpenEvent())
    {
        return FALSE;
    }

    AddListener(m_sockEvent.GetEventSocket(), pHandler, pMethod);

    return TRUE;
}

void xTaskEvent::CloseTaskEvent()
{
    m_sockEvent.CloseEvent();
}

void xTaskEvent::ResetEvent()
{
    m_sockEvent.ResetEvent();
}

void xTaskEvent::WaitEvent(int nTimeout)
{
    if (0 == m_listEvent.size())
    {
        return;
    }

    chFD_SET fdSet;

    typeEventList::iterator iter = m_listEvent.begin();

    for (; iter != m_listEvent.end(); ++iter)
    {
        xTaskListener* pListener = *iter;

        if (NULL != pListener)
        {
            fdSet.add(pListener->GetSocket());
        }
    }

    if (fdSet.readSelect(nTimeout) <= 0)
    {
        return;
    }

    typeEventList listHandle;

    for (iter = m_listEvent.begin(); iter != m_listEvent.end(); ++iter)
    {
        xTaskListener* pListener = *iter;

        if (NULL == pListener)
        {
            continue;
        }

        SOCKET sListener = pListener->GetSocket();

        if (fdSet.isSet(sListener))
        {
            if (m_sockEvent.GetEventSocket() == sListener)
            {
                m_sockEvent.RecvEvent();
            }

            listHandle.push_front(pListener);
        }
    }

    while (listHandle.size() > 0)
    {
        xTaskListener* pListener = listHandle.back();

        listHandle.pop_back();

        if (NULL != pListener)
        {
            pListener->HandlerTask();
        }
    }
}

BOOL xTaskEvent::SetEvent()
{
    return m_sockEvent.SetEvent();
}

BOOL xTaskEvent::AddListener(SOCKET hSocket, xMessageHandler* pHandler, msgMethod pMethod)
{
    if (INVALID_SOCKET == hSocket || NULL == pHandler || NULL == pMethod)
    {
        return FALSE;
    }

    typeEventList::iterator iter = m_listEvent.begin();

    for (; iter != m_listEvent.end(); ++iter)
    {
        xTaskListener* pListener = *iter;

        if (NULL != pListener && pListener->GetSocket() == hSocket)
        {
            return FALSE;
        }
    }

    xTaskListener* pListener = new xTaskListener(hSocket, pHandler, pMethod);

    if (NULL == pListener)
    {
        return FALSE;
    }

    m_listEvent.push_back(pListener);

    return TRUE;
}

BOOL xTaskEvent::RemoveListener(SOCKET hSocket)
{
    typeEventList::iterator iter = m_listEvent.begin();

    for (; iter != m_listEvent.end(); ++iter)
    {
        xTaskListener* pListener = *iter;

        if (NULL != pListener && pListener->GetSocket() == hSocket)
        {
            m_listEvent.erase(iter);
            return TRUE;
        }
    }

    return FALSE;
}

END_XKERNEL_NAMESPACE
