#include "xkernel.h"
#include "xkernelcommon.h"
#include "xevent.h"
#include "dsklog/log.h"
#include "commonapi/stringhelperapi.h"


BEGIN_XKERNEL_NAMESPACE

#define THREAD_MSG_WAIT_TIMEOUT     1000
static TASK_ID s_idTaskMainThread = 0;

//////////////////////////////////////////////////////////////////////////
// class xMsgObject
xMsgObject::xMsgObject(UINT uMsgID/* = 0*/, WPARAM wPar/* = 0*/, LPARAM lPar/* = 0*/)
    : m_bIsWaitingReply(false)
    , m_nStatus(MT_INVALID)
    , m_pMsgHandler(NULL)
    , m_pMethod(NULL_METHOD)
    , lResult(NO_RESULT)
    , message(uMsgID)
    , wParam(wPar)
    , lParam(lPar)
    , m_idSrcTask(0)
{
}

//////////////////////////////////////////////////////////////////////////
// class xMsgResult
xMsgResult::xMsgResult(LRESULT lResult)
    : m_lResult(lResult)
    , m_bSuccessful(false)
{
}

xMsgResult::xMsgResult(const xMsgResult & src)
    : m_lResult(src.m_lResult)
    , m_bSuccessful(src.m_bSuccessful)
{
    xMsgResult & writebleSrc = const_cast<xMsgResult &>(src);

    writebleSrc.m_lResult = NO_RESULT;
}

xMsgResult::~xMsgResult()
{
}

LRESULT xMsgResult::GetResult()
{
    return m_lResult;
}

bool xMsgResult::IsSuccessful()
{
    return m_bSuccessful;
}

xMsgResult & xMsgResult::operator = (const xMsgResult & src)
{
    m_lResult = src.m_lResult;
    m_bSuccessful = src.m_bSuccessful;

    xMsgResult & writebleSrc = const_cast<xMsgResult &>(src);

    writebleSrc.m_lResult = NO_RESULT;

    return *this;
}

xMsgResult & xMsgResult::operator = (const xMsgObject & msg)
{
    m_lResult = msg.lResult;
    m_bSuccessful = true;

    return *this;
}

//////////////////////////////////////////////////////////////////////////
// class xTaskObject
#if IF_MULTI_THREAD_ENABLE
xTaskObject::xTaskObject(chCriticalSection & csTask)
    : m_csTask(csTask)
    , m_idProcess(0)
#else
xTaskObject::xTaskObject()
    : m_idProcess(0)
#endif
    , m_idTask(0)
    , m_nAwakePort(0)
#if IF_MULTI_THREAD_ENABLE
    , m_nReplayPort(0)
    , m_pReplayEvent(NULL)
    , m_pReplayMsg(NULL)
#endif
{
}

xTaskObject::~xTaskObject()
{
    CloseTaskObject();
}

void xTaskObject::ClearMessageQueue()
{
#if IF_MULTI_THREAD_ENABLE
    m_csTask.Lock();
#endif

    LIST_MSG_OBJECT::iterator iter = m_listMsg.begin();
    LIST_MSG_OBJECT::iterator iterEnd = m_listMsg.end();
    xMsgObject * pMsgObject = NULL;

    for (; iter != iterEnd; ++iter)
    {
        pMsgObject = *iter;

        if (NULL != pMsgObject)
        {
            g_KernelMsgMgr.ReleaseMsgObject(pMsgObject);

            *iter = NULL;
        }
    }

    m_listMsg.clear();

#if IF_MULTI_THREAD_ENABLE
    if (NULL != m_pReplayMsg && MT_REPLYED == m_pReplayMsg->m_nStatus)
    {
        g_KernelMsgMgr.ReleaseMsgObject(m_pReplayMsg);
    }

    m_pReplayMsg = NULL;

    m_csTask.Unlock();
#endif
}

void xTaskObject::CloseTaskObject()
{
    ClearMessageQueue();

#if IF_MULTI_THREAD_ENABLE
    m_csTask.Lock();

    if (NULL != m_pReplayEvent)
    {
        m_pReplayEvent->CloseEvent();

        delete m_pReplayEvent;
        m_pReplayEvent = NULL;
    }
#endif

    m_strTaskName = "";
    m_idProcess = 0;
    m_idTask = 0;
    m_nAwakePort = 0;

#if IF_MULTI_THREAD_ENABLE
    m_nReplayPort = 0;

    m_csTask.Unlock();
#endif
}

#if IF_MULTI_THREAD_ENABLE
xEvent_Socket * xTaskObject::GetReplySocketEvent()
{
    xEvent_Socket * pReplayEvent = NULL;

    m_csTask.Lock();

    if (NULL == m_pReplayEvent)
    {
        m_pReplayEvent = new xEvent_Socket;

        if (NULL != m_pReplayEvent)
        {
            m_pReplayEvent->OpenEvent();
        }
    }

    pReplayEvent = m_pReplayEvent;
    m_csTask.Unlock();

    return pReplayEvent;
}
#endif

void xTaskObject::PushMessage(xMsgObject * pMsgObject)
{
    if (NULL == pMsgObject)
    {
        return;
    }

#if IF_MULTI_THREAD_ENABLE
    m_csTask.Lock();
#endif

    if (m_listMsg.size() > 0)
    {
        m_listMsg.insert(m_listMsg.begin(), pMsgObject);
    }
    else
    {
        m_listMsg.push_back(pMsgObject);
    }

#if IF_MULTI_THREAD_ENABLE
    m_csTask.Unlock();
#endif
}

xMsgObject * xTaskObject::ReadMessage()
{
    xMsgObject * pMsgObject = NULL;

#if IF_MULTI_THREAD_ENABLE
    m_csTask.Lock();
#endif

    if (m_listMsg.size() > 0)
    {
        pMsgObject = m_listMsg.back();

        m_listMsg.pop_back();

        if (NULL != pMsgObject)
        {
            pMsgObject->m_nStatus = MT_PROCESSING;
        }
    }

#if IF_MULTI_THREAD_ENABLE
    m_csTask.Unlock();
#endif

    return pMsgObject;
}

#if IF_MULTI_THREAD_ENABLE
void xTaskObject::SetReplyMessage(xMsgObject * pMsgObject)
{
    if (NULL == pMsgObject)
    {
        return;
    }

    m_csTask.Lock();
    m_pReplayMsg = pMsgObject;
    m_csTask.Unlock();
}

xMsgObject * xTaskObject::GetReplyMessage()
{
    xMsgObject * pReplayMsg = NULL;

    m_csTask.Lock();
    pReplayMsg = m_pReplayMsg;
    m_csTask.Unlock();

    return pReplayMsg;
}

bool xTaskObject::WaitMessageReply(xTaskObject * pTarTaskObject, xMsgObject * pMsgObject,
                                   xMsgResult & objResult, int nTimeout)
{
    if (NULL == pTarTaskObject || NULL == pMsgObject || !pMsgObject->m_bIsWaitingReply)
    {
        return false;
    }

    xEvent_Socket * pReplayEvent = GetReplySocketEvent();
    xMsgObject * pReplayMsg = GetReplyMessage();

    if (NULL == pReplayMsg || NULL == pReplayEvent)
    {
        return false;
    }

    pTarTaskObject->PushMessage(pMsgObject);

    // 首先唤醒目标线程
    if (!g_KernelMsgMgr.AwakeTask(pTarTaskObject->m_idProcess))
    {
        // the target task had been non-alive
        return false;
    }

    chTimeout timeOut(nTimeout);

    while (true)
    {
        pReplayEvent->WaitEvent(nTimeout);

        xMsgObject * pFreeMsgObject = NULL;

        m_csTask.Lock();
        int nStatus = m_pReplayMsg->m_nStatus;

        if (MT_REPLYED != nStatus)
        {
            nTimeout = timeOut.GetLeftMillisecond();

            if (nTimeout <= 0)
            {
                // 超时，由消息处理线程释放
                m_pReplayMsg = NULL;
                m_csTask.Unlock();
                return true;
            }

            m_csTask.Unlock();
            continue;
        }

        objResult = *pReplayMsg;
        pFreeMsgObject = pReplayMsg;
        m_pReplayMsg = NULL;
        m_csTask.Unlock();

        g_KernelMsgMgr.ReleaseMsgObject(pFreeMsgObject);

        return true;
    }

    chASSERTx(FALSE, "WaitMessageReply fault.");

    return true;
}

void xTaskObject::ReplyMessage(xMsgObject * pMsgObject)
{
    if (NULL == pMsgObject)
    {
        return;
    }

    xMsgObject * pReplayMsg = GetReplyMessage();

    if (NULL == pReplayMsg || pReplayMsg != pMsgObject)
    {
        g_KernelMsgMgr.ReleaseMsgObject(pMsgObject);
        return;
    }

    m_csTask.Lock();
    pMsgObject->m_nStatus = MT_REPLYED;
    m_csTask.Unlock();
}
#endif

////////////////////////////////////////////////////////////////////////////
// struct threadStartData
struct ST_THREAD_START_DATA
{
    xEvent_Socket       objEvent;
    LPCSTR              lpszThreadName;
    LPTHREAD_MAIN       pRunMethod;
    THREAD_MSG_PROC_EX  pMessageMethod;
    LPVOID              lpThreadData;
    xThreadLocal    *   pThreadObject;

    ST_THREAD_START_DATA()
        : lpszThreadName(NULL)
        , pRunMethod(NULL)
        , pMessageMethod(NULL)
        , lpThreadData(NULL)
        , pThreadObject(NULL)
    {
    }
};

xThreadLocal * msgStartThread(ST_THREAD_START_DATA & data, int iPriority);

#if IF_XTHREAD_LOCAL_EXIT_METHOD
//////////////////////////////////////////////////////////////////////////
// class xExitMethodWrap
xExitMethodWrap::xExitMethodWrap(xExitMethodList * pOwner, msgExitMethod method)
    : m_pOwnerList(pOwner)
    , m_pMethod(method)
{
    if (NULL != pOwner)
    {
        pOwner->AddExitMethod(this);
    }
}

xExitMethodWrap::~xExitMethodWrap()
{
    if (NULL != m_pOwnerList)
    {
        m_pOwnerList->RemoveExitMethod(this);
        onExit();
    }
}

void xExitMethodWrap::onExit()
{
    if (NULL != m_pMethod)
    {
        m_pMethod();
    }

    m_pMethod = NULL_METHOD;
    m_pOwnerList = NULL;
}

//////////////////////////////////////////////////////////////////////////
// class xExitMethodList
xExitMethodList::xExitMethodList()
{

}

xExitMethodList::~xExitMethodList()
{

}

bool xExitMethodList::AddExitMethod(xExitMethodWrap * pWrap)
{
    if (NULL == pWrap || IsHadExitMethod(pWrap))
    {
        return false;
    }

    if (m_listExitMethod.size() > 0)
    {
        m_listExitMethod.insert(m_listExitMethod.begin(), pWrap);
    }
    else
    {
        m_listExitMethod.push_back(pWrap);
    }

    return true;
}

void xExitMethodList::RemoveExitMethod(xExitMethodWrap * pWrap)
{
    LIST_EXIT_METHOD::iterator iter = m_listExitMethod.begin();
    LIST_EXIT_METHOD::iterator iterEnd = m_listExitMethod.end();

    for (; iter != iterEnd; ++iter)
    {
        if (pWrap == (*iter))
        {
            m_listExitMethod.erase(iter);
        }
    }
}

void xExitMethodList::ExecuteOnExit()
{
    xExitMethodWrap * pWrap = NULL;

    while (m_listExitMethod.size() > 0)
    {
        pWrap = m_listExitMethod.back();
        m_listExitMethod.pop_back();

        if (NULL != pWrap)
        {
            pWrap->onExit();
        }
    }
}

bool xExitMethodList::IsHadExitMethod(xExitMethodWrap * pWrap)
{
    if (NULL == pWrap)
    {
        return false;
    }
}
#endif

//////////////////////////////////////////////////////////////////////////
// class chThreadBase
xThreadBase::xThreadBase()
#if IF_MULTI_THREAD_ENABLE
    : m_TaskObject(m_csThread)
#endif
{
}

xThreadBase::~xThreadBase()
{

}

// ??? To be improved on etlIsValidTaskID
BOOL xThreadBase::CheckThreadAlive()
{
    return etlIsValidTaskID(m_TaskObject.m_idTask);
}

BOOL xThreadBase::IsCalledByThisThread()
{
    return etlGetCurrentTaskId() == m_TaskObject.m_idTask;
}

PROCESS_ID xThreadBase::GetProcessID()
{
    return m_TaskObject.m_idProcess;
}

TASK_ID xThreadBase::GetTaskID()
{
    return m_TaskObject.m_idTask;
}

#if IF_MULTI_THREAD_ENABLE
xMsgResult xThreadBase::sendThreadMsgEx(UINT uMsgID, WPARAM wParam, LPARAM lParam, int nTimeout,
                                        xMessageHandler * pHandler/* = NULL*/, msgMethod pMethod/* = NULL*/)
{
    xMsgResult objResult(NO_RESULT);

    xMsgObject * pMsgObject = AllocThreadMessage(TRUE);

    if (NULL == pMsgObject)
    {
        return objResult;
    }

    xTaskObject * pThisTaskObject = g_KernelMsgMgr.GetTaskObject(pMsgObject->m_idSrcTask);

    if (NULL == pThisTaskObject)
    {
        g_KernelMsgMgr.ReleaseMsgObject(pMsgObject);
        return objResult;
    }

    pMsgObject->m_nStatus = MT_UNPROCESSED;
    pMsgObject->m_pMsgHandler = pHandler;
    pMsgObject->m_pMethod = pMethod;
    pMsgObject->message = uMsgID;
    pMsgObject->wParam = wParam;
    pMsgObject->lParam = lParam;

    pThisTaskObject->SetReplyMessage(pMsgObject);

    if (!pThisTaskObject->WaitMessageReply(&m_TaskObject, pMsgObject, objResult, nTimeout))
    {
        g_KernelMsgMgr.ReleaseMsgObject(pMsgObject);
        pThisTaskObject->SetReplyMessage(NULL);
    }

    return objResult;
}
#endif

BOOL xThreadBase::postThreadMsgEx(UINT uMsgID, WPARAM wParam, LPARAM lParam,
                                  xMessageHandler * pHandler/* = NULL*/, msgMethod pMethod/* = NULL*/)
{
    xMsgObject * pMsg = AllocThreadMessage(FALSE);

    if (NULL == pMsg)
    {
        return FALSE;
    }

    pMsg->m_nStatus = MT_UNPROCESSED;
    pMsg->m_pMsgHandler = pHandler;
    pMsg->m_pMethod = pMethod;
    pMsg->message = uMsgID;
    pMsg->wParam = wParam;
    pMsg->lParam = lParam;

    m_TaskObject.PushMessage(pMsg);

    return g_KernelMsgMgr.AwakeTask(m_TaskObject.m_idTask);
}

xMsgObject * xThreadBase::AllocThreadMessage(bool bIsSendMessage)
{
    //alloc one message and flag it to send or post
    xMsgObject * pMsg = NULL;

    if (CheckThreadAlive())
    {
        pMsg = g_KernelMsgMgr.AllocMsgObject();
    }

    if (NULL != pMsg)
    {
        pMsg->m_bIsWaitingReply = bIsSendMessage;
        pMsg->m_nStatus = MT_UNPOST;
        pMsg->lResult = NO_RESULT;
        pMsg->m_idSrcTask = etlGetCurrentTaskId();
    }

    return pMsg;
}

void xThreadBase::AfterProcessMessage(xMsgObject * pMsgObject)
{
    if (NULL == pMsgObject)
    {
        return;
    }

    if (pMsgObject->m_bIsWaitingReply)
    {
        // message is sent mode
#if IF_MULTI_THREAD_ENABLE
        xTaskObject * pSrcTaskObject = g_KernelMsgMgr.GetTaskObject(pMsgObject->m_idSrcTask);

        if (NULL != pSrcTaskObject)
        {
            pSrcTaskObject->ReplyMessage(pMsgObject);

            return;
        }
#else
        pMsgObject->m_nStatus = MT_REPLYED;

        g_KernelMsgMgr.ReleaseMsgObject(pMsgObject);

        return;
#endif
    }

    // message is post mode, or message is sent mode but is timeout;
    pMsgObject->m_nStatus = MT_PROCESSED;
    g_KernelMsgMgr.ReleaseMsgObject(pMsgObject);
}

//////////////////////////////////////////////////////////////////////////
// class xThreadLocal
#if !IF_MULTI_THREAD_ENABLE
xThreadLocal * s_pThreadLocal = NULL;
#endif
xThreadLocal::xThreadLocal(const yl::string & strThreadName, THREAD_MSG_PROC_EX pCallbackEx)
    : xMessageHandler(this)
    , m_strThreadName(strThreadName)
    , m_pMsgCallbackEx(pCallbackEx)
    , m_iPriority(-1)
    , m_bRuning(FALSE)
    , m_nExitCode(-1)
    , m_idThisThread(0)
{
#if !IF_MULTI_THREAD_ENABLE
    s_pThreadLocal = this;
#endif
}

xThreadLocal::~xThreadLocal()
{
#if !IF_MULTI_THREAD_ENABLE
    s_pThreadLocal = NULL;
#endif
    if (0 != m_TaskObject.m_idTask)
    {
        if (IsCalledByThisThread())
        {
            DetachThread();
        }
        else
        {
            Stop();
        }
    }
}

BOOL xThreadLocal::registerCurrentThread()
{
#if IF_MULTI_THREAD_ENABLE
    xKernelMsgMgr & objMsgHelper = g_KernelMsgMgr;

    objMsgHelper.ClearGhostThread();
#endif

    TASK_ID idTask = etlGetCurrentTaskId();

    if (0 == idTask || !m_TaskEvent.OpenTaskEvent(idTask, MSG_METHOD(this, xThreadLocal::TryThreadMessage)))
    {
        return FALSE;
    }

    m_idThisThread = etlGetCurrentThreadId();
    m_TaskObject.m_idProcess = etlGetCurrentProcessId();
    m_TaskObject.m_idTask = idTask;
    m_TaskObject.m_nAwakePort = m_TaskEvent.GetSocketEvent().GetEventPort();

#if IF_MULTI_THREAD_ENABLE
    objMsgHelper.addLocalThread(this);
#endif

    chASSERTx(m_pHostThread == this, "m_pHostThread MUST be this in registerCurrentThread.(%d)", m_pHostThread->m_idThisTask);

    if (m_strThreadName.empty())
    {
        m_strThreadName = commonAPI_FormatString("@%d", m_TaskObject.m_idTask);
    }

    m_bRuning = TRUE;

    postThreadMsgEx(XEVENT_TYPE_INIT, 0, 0);

    return TRUE;
}

void xThreadLocal::unregisterThread()
{
    m_bRuning = FALSE;

    m_TaskEvent.CloseTaskEvent();

#if IF_MULTI_THREAD_ENABLE
    xKernelMsgMgr & objMsgHelper = g_KernelMsgMgr;

    objMsgHelper.removeLocalThread(this);
#else
    s_pThreadLocal = NULL;
#endif

    m_pHostThread = NULL;
    m_idThisThread = 0;
}

BOOL xThreadLocal::AttachCurrentThread()
{
    return registerCurrentThread() && AfterThreadLaunched();
}

void xThreadLocal::DetachThread()
{
    if (0 != m_TaskObject.m_idTask)
    {
        bool bMainThreadStoped = false;

        if (s_idTaskMainThread == m_TaskObject.m_idTask)
        {
            bMainThreadStoped = true;
        }

        BeforeThreadShutdown();
        unregisterThread();

#if IF_XTHREAD_LOCAL_EXIT_METHOD
        xExitMethodList::ExecuteOnExit();

        if (bMainThreadStoped)
        {
            g_KernelMsgMgr.ExecuteOnExit();
        }
#endif
    }
}

BOOL xThreadLocal::Start()
{
    ST_THREAD_START_DATA data;

    data.lpszThreadName = m_strThreadName.c_str();
    data.pRunMethod = NULL;
    data.lpThreadData = NULL;
    data.pThreadObject = this;

    return msgStartThread(data, -1) == this;
}

void xThreadLocal::Stop()
{
    if (0 != m_TaskObject.m_idTask)
    {
        chASSERT(!IsCalledByThisThread());
        // m_idThisThread may be set to 0 between PostThreadMsg and chJoinThread
        // so, back it up
        THREAD_ID idThread = m_idThisThread;

        postThreadMsgEx(XEVENT_TYPE_QUIT, 0, 0);

        chJoinThread(idThread);
    }
}

BOOL xThreadLocal::AfterThreadLaunched()
{
    return TRUE;
}

void xThreadLocal::BeforeThreadShutdown()
{
    chASSERT(IsCalledByThisThread());
}

BOOL xThreadLocal::TryThreadMessage(msgObject &)
{
    xMsgObject * pMsgObject = m_TaskObject.ReadMessage();

    if (NULL == pMsgObject)
    {
        return FALSE;
    }

    if (NULL != m_pMsgCallbackEx)
    {
        msgObject objETLMsg(pMsgObject->message, pMsgObject->wParam, pMsgObject->lParam);

        objETLMsg.lResult = pMsgObject->lResult;

        LRESULT result = (*m_pMsgCallbackEx)(&objETLMsg);

        if (MT_PROCESSING == pMsgObject->m_nStatus)
        {
            pMsgObject->lResult = result;
        }
    }
    else
    {
        DefaultProcessMessage(*pMsgObject);
    }

    AfterProcessMessage(pMsgObject);

    return TRUE;
}

int xThreadLocal::Run()
{
#ifdef _WIN32
    while (m_bRuning && m_TaskEvent.GetSocketEvent().IsValid())
    {
        int nMsgLoop = 0;
        MSG msg;

        // Windows主消息循环:
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE | PM_NOYIELD))
        {
            ++nMsgLoop;

            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (WM_QUIT == msg.message)
            {
                m_bRuning = FALSE;

                if (-1 == m_nExitCode)
                {
                    m_nExitCode = 0;
                }

                break;
            }

            if (nMsgLoop > 10)
            {
                break;;
            }
        }

        m_TaskEvent.WaitEvent(10);
    }
#else
    while (m_bRuning && m_TaskEvent.GetSocketEvent().IsValid())
    {
        m_TaskEvent.WaitEvent(NEVER_TIMEOUT);
    }
#endif

    return m_nExitCode;
}

BOOL xThreadLocal::AwakeTask()
{
    if (0 != m_TaskObject.m_nAwakePort)
    {
        return m_TaskEvent.GetSocketEvent().SendEvent(m_TaskObject.m_nAwakePort);
    }

    return FALSE;
}

LRESULT xThreadLocal::SendThreadMsg(UINT uMsgID, WPARAM wParam, LPARAM lParam,
                                    xMessageHandler * pHandler/* = NULL*/, msgMethod pMethod/* = NULL_METHOD*/)
{
#if IF_MULTI_THREAD_ENABLE
    if (IsCalledByThisThread())
    {
        xThreadLocal * pLocalThread = g_KernelMsgMgr.GetLocalThreadByTaskID(m_TaskObject.m_idTask);
#else
    xThreadLocal * pLocalThread = s_pThreadLocal;
#endif

        if (NULL != pLocalThread)
        {
            return pLocalThread->InvokeThreadMsg(uMsgID, wParam, lParam, pHandler, pMethod).GetResult();
        }

        return FALSE;
#if IF_MULTI_THREAD_ENABLE
    }
    else
    {
        // 发送线程间消息
        return sendThreadMsgEx(uMsgID, wParam, lParam, NEVER_TIMEOUT, pHandler, pMethod).GetResult();
    }
#endif
}

bool xThreadLocal::ConnectMsgReadHandler(SOCKET fd, xMessageHandler * pHandler, msgMethod pMessageMeth)
{
    return m_TaskEvent.AddListener(fd, pHandler, pMessageMeth);
}

bool xThreadLocal::DisconnectMsgReadHandler(SOCKET fd)
{
    return m_TaskEvent.RemoveListener(fd);
}

xMsgResult xThreadLocal::InvokeThreadMsg(UINT uMsgID, WPARAM wParam, LPARAM lParam, xMessageHandler * pHandler, msgMethod pMethod)
{
    xMsgObject * pMsg = AllocThreadMessage(TRUE);

    if (NULL == pMsg)
    {
        return NO_RESULT;
    }

    pMsg->m_pMsgHandler = pHandler;
    pMsg->m_pMethod = pMethod;
    pMsg->message = uMsgID;
    pMsg->wParam = wParam;
    pMsg->lParam = lParam;
    pMsg->m_nStatus = MT_PROCESSING;

    DefaultProcessMessage(*pMsg);

    pMsg->m_nStatus = MT_PROCESSED;

    xMsgResult objResult(pMsg->lResult);

    g_KernelMsgMgr.ReleaseMsgObject(pMsg);

    return objResult;
}

void xThreadLocal::CallMessageMeth(xMessageHandler * pHandler, msgMethod pMessageMeth, xMsgObject & objMessage)
{
    if (NULL == pHandler)
    {
        return;
    }

    msgObject objETLMsg(objMessage.message, objMessage.wParam, objMessage.lParam);

    objETLMsg.lResult = objMessage.lResult;

    if (NULL_METHOD == pMessageMeth)
    {
        pHandler->TryThreadMessage(objETLMsg);
    }
    else
    {
        (pHandler->*pMessageMeth)(objETLMsg);
    }
}

void xThreadLocal::DefaultProcessMessage(xMsgObject & objMessage)
{
    chASSERT(IsCalledByThisThread());

    if (objMessage.message >= xKernel::XEVENT_TYPE_CUSTOM)
    {
        CallMessageMeth(objMessage.m_pMsgHandler, objMessage.m_pMethod, objMessage);

        return;
    }

    switch (objMessage.message)
    {
    case xKernel::XEVENT_TYPE_QUIT:
        {
            m_nExitCode = (int)objMessage.wParam;
#ifdef _WIN32
            PostQuitMessage(0);
#else
            m_bRuning = FALSE;
#endif
        }
        break;
    default:
        break;
    }
}

//////////////////////////////////////////////////////////////////////////
// extern functions
IMPLEMENT_THREAD_METHOD(xThreadLocal::eventMessageThread)
{
    ST_THREAD_START_DATA * pData = static_cast<ST_THREAD_START_DATA *>(lpData);
    xThreadLocal * pThreadObject = pData->pThreadObject;

    if (NULL != pThreadObject)
    {
        pThreadObject->registerCurrentThread();
        pData->objEvent.SetEvent();
        pThreadObject->AfterThreadLaunched();
        pThreadObject->Run();
        pThreadObject->DetachThread();
    }
    else
    {
        xThreadLocal objThread(pData->lpszThreadName, pData->pMessageMethod);

        // objThread.AttachCurrentThread();
        objThread.registerCurrentThread();
        pData->pThreadObject = &objThread;

        // pData passed away after SetEvent(), so save it first
        LPTHREAD_MAIN pThreadMain = pData->pRunMethod;
        LPVOID pThreadData = pData->lpThreadData;

        pData->objEvent.SetEvent();
        objThread.AfterThreadLaunched();

        if (NULL_METHOD != pThreadMain)
        {
            (*pThreadMain)(pThreadData);
        }
        else
        {
            objThread.Run();
        }

        objThread.DetachThread();
    }

    return 0;
}

xThreadLocal * msgStartThread(ST_THREAD_START_DATA & data, int iPriority)
{
    data.objEvent.OpenEvent();

    THREAD_ID idThread = chCreateThread(xThreadLocal::eventMessageThread, &data, iPriority);

    chASSERT(idThread != 0);

    do
    {
        data.objEvent.WaitEvent();

        chWARNING(NULL != data.pThreadObject && 0 != data.pThreadObject->GetTaskID());
    }
    while (NULL == data.pThreadObject || 0 == data.pThreadObject->GetTaskID());

    return data.pThreadObject;
}

xThreadLocal * msgStartThread(LPCSTR lpszThreadName, LPTHREAD_MAIN pMethod, LPVOID lpThreadData, int iPriority)
{
    ST_THREAD_START_DATA data;

    data.lpszThreadName = lpszThreadName;
    data.pRunMethod = pMethod;
    data.lpThreadData = lpThreadData;
    data.pMessageMethod = NULL;
    data.pThreadObject = NULL;

    return msgStartThread(data, iPriority);
}

xThreadLocal * msgStartThread(LPCSTR lpszThreadName, THREAD_MSG_PROC_EX pMessageMethod, int iPriority)
{
    ST_THREAD_START_DATA data;

    data.lpszThreadName = lpszThreadName;
    data.pRunMethod = NULL;
    data.lpThreadData = NULL;
    data.pMessageMethod = pMessageMethod;
    data.pThreadObject = NULL;

    printf("msgStartThread [%s] \n", lpszThreadName);
    return msgStartThread(data, iPriority);
}

#if IF_MULTI_THREAD_ENABLE
//////////////////////////////////////////////////////////////////////////
// class xProcessLocal
xProcessLocal::xProcessLocal()
{
    s_idTaskMainThread = etlGetCurrentTaskId();
}

xProcessLocal::~xProcessLocal()
{
}

xThreadLocal * xProcessLocal::GetLocalThreadByTaskID(TASK_ID idTask)
{
    chASSERT(idTask != 0);

    xThreadLocal * pTargetThread = NULL;

    m_csThread.Lock();

    LIST_THREAD_LOCAL::iterator iter = m_listThreadLocal.begin();
    LIST_THREAD_LOCAL::iterator iterEnd = m_listThreadLocal.end();

    for (; iter != iterEnd; ++iter)
    {
        xThreadLocal * pThread = *iter;

        if (NULL != pThread && pThread->GetTaskID() == idTask)
        {
            pTargetThread = pThread;
            break;
        }
    }

    m_csThread.Unlock();

    return pTargetThread;
}

xThreadLocal * xProcessLocal::GetCurrentThreadObject()
{
    return GetLocalThreadByTaskID(etlGetCurrentTaskId());
}

xTaskObject * xProcessLocal::GetTaskObject(TASK_ID idTask)
{
    xThreadLocal * pLocalThread = GetLocalThreadByTaskID(idTask);

    if (NULL != pLocalThread)
    {
        return &(pLocalThread->m_TaskObject);
    }

    return NULL;
}

bool xProcessLocal::IsValidLocalThread(xThreadLocal * pThread)
{
    return NULL != pThread && (NULL != GetLocalThreadByTaskID(pThread->GetTaskID()));
}

void xProcessLocal::ClearGhostThread()
{
    m_csThread.Lock();

    LIST_THREAD_LOCAL::iterator iter = m_listThreadLocal.begin();

    while (iter != m_listThreadLocal.end())
    {
        xThreadLocal * pThreadLocal = *iter;

        if (NULL == pThreadLocal)
        {
            iter = m_listThreadLocal.erase(iter);
            continue;
        }

        if (
#if WINDOWS_SYSTEM
            !etlIsValidProcessID(pThreadLocal->GetProcessID()) ||
#endif
            !etlIsValidTaskID(pThreadLocal->GetTaskID()))
        {
            iter = m_listThreadLocal.erase(iter);

            delete pThreadLocal;
            pThreadLocal = NULL;
            continue;
        }

        ++iter;
    }

    m_csThread.Unlock();
}

BOOL xProcessLocal::AwakeTask(TASK_ID idTask)
{
    xThreadLocal * pThreadLocal = GetLocalThreadByTaskID(idTask);

    if (NULL != pThreadLocal)
    {
        return pThreadLocal->AwakeTask();
    }

    return FALSE;
}

// 1.创建消息队列的时候会创建当前线程ID相关的Event
// 2.消息队列结束的时候会销毁当前线程ID相关的Event
// 3.SendMeesage的时候会创建发送者线程ID相关的Event
// 4.SendMeesage的时候会打开接收者线程ID相关的Event
// 5.PostMessage时时候会打开接收者线程ID相关的Event
// 6.ReplyMessag的时候会打开发送者线程ID相关的Event
// 7.消息处理完以后会打开发送线程ID相关的Event

BOOL xProcessLocal::addLocalThread(xThreadLocal * pLocalThread)
{
    if (NULL == pLocalThread || NULL != GetLocalThreadByTaskID(pLocalThread->GetTaskID()))
    {
        return FALSE;
    }

    m_csThread.Lock();
    m_listThreadLocal.push_back(pLocalThread);
    m_csThread.Unlock();

    return TRUE;
}

void xProcessLocal::removeLocalThread(xThreadLocal * pLocalThread)
{
    if (NULL == pLocalThread)
    {
        return;
    }

    m_csThread.Lock();

    LIST_THREAD_LOCAL::iterator iter = m_listThreadLocal.begin();
    LIST_THREAD_LOCAL::iterator iterEnd = m_listThreadLocal.end();

    for (; iter != iterEnd; ++iter)
    {
        xThreadLocal * pTmpThread = *iter;

        if (pTmpThread == pLocalThread)
        {
            m_listThreadLocal.erase(iter);
            break;
        }
    }

    m_csThread.Unlock();
}
#endif

//////////////////////////////////////////////////////////////////////////
// class xKernelMsgMgr
IMPLEMENT_XGLOBLE_OBJECT(xKernelMsgMgr)

xKernelMsgMgr::xKernelMsgMgr()
{
    ASSERT_XGLOBAL_OBJECT();
}

xKernelMsgMgr::~xKernelMsgMgr()
{
    ClearCacheMsgObject();
}

xMsgObject * xKernelMsgMgr::AllocMsgObject()
{
    xMsgObject * pMsgObject = NULL;

#if IF_MULTI_THREAD_ENABLE
    m_csMsgMgr.Lock();
#endif

    if (m_listCacheMsg.size() > 0)
    {
        pMsgObject = m_listCacheMsg.back();

        m_listCacheMsg.pop_back();
    }

#if IF_MULTI_THREAD_ENABLE
    m_csMsgMgr.Unlock();
#endif

    if (NULL == pMsgObject)
    {
        pMsgObject = new xMsgObject;
    }

    return pMsgObject;
}

void xKernelMsgMgr::ReleaseMsgObject(xMsgObject * pMsgObject)
{
    if (NULL == pMsgObject)
    {
        return;
    }

#if IF_MULTI_THREAD_ENABLE
    m_csMsgMgr.Lock();
#endif

    int nMsgCount = m_listCacheMsg.size();

    if (nMsgCount < MAX_MSG_OBJECT_CACHE_COUNT)
    {
        if (nMsgCount > 0)
        {
            m_listCacheMsg.insert(m_listCacheMsg.begin(), pMsgObject);
        }
        else
        {
            m_listCacheMsg.push_back(pMsgObject);
        }

        pMsgObject = NULL;
    }

#if IF_MULTI_THREAD_ENABLE
    m_csMsgMgr.Unlock();
#endif

    if (NULL != pMsgObject)
    {
        delete pMsgObject;
        pMsgObject = NULL;
    }
}

void xKernelMsgMgr::ClearCacheMsgObject()
{
#if IF_MULTI_THREAD_ENABLE
    m_csMsgMgr.Lock();
#endif

    LIST_MSG_OBJECT::iterator iter = m_listCacheMsg.begin();
    LIST_MSG_OBJECT::iterator iterEnd = m_listCacheMsg.end();
    xMsgObject * pMsgObject = NULL;

    for (; iter != iterEnd; ++iter)
    {
        pMsgObject = *iter;

        if (NULL != pMsgObject)
        {
            g_KernelMsgMgr.ReleaseMsgObject(pMsgObject);
            *iter = NULL;
        }
    }

    m_listCacheMsg.clear();
#if IF_MULTI_THREAD_ENABLE
    m_csMsgMgr.Unlock();
#endif
}

#if !IF_MULTI_THREAD_ENABLE
BOOL xKernelMsgMgr::AwakeTask(TASK_ID idTask)
{
    if (NULL != s_pThreadLocal && s_pThreadLocal->GetTaskID() == idTask)
    {
        return s_pThreadLocal->AwakeTask();
    }

    return FALSE;
}

xThreadLocal * xKernelMsgMgr::GetCurrentThreadObject()
{
    return s_pThreadLocal;
}

bool xKernelMsgMgr::IsValidLocalThread(xThreadLocal * pThread)
{
    return (NULL != pThread && pThread == s_pThreadLocal);
}
#endif

END_XKERNEL_NAMESPACE
