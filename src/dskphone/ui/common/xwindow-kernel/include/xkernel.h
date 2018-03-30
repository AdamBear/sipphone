#ifndef __X_KERNEL_H__
#define __X_KERNEL_H__

#include "xkerneldefine.h"
#include <ETLLib/ETLLib.hpp>
#include "ylstring.h"
#include "yllist.h"
#include "xevent.h"
#include "xmessagehandler.h"


BEGIN_XKERNEL_NAMESPACE

class xMessageHandler;
class xThreadLocal;
class xThreadBase;

typedef YLList<TASK_ID>     typeTaskIDList;


//////////////////////////////////////////////////////////////////////////
// class xMsgObject
class xMsgObject
{
    friend class xMsgResult;
    friend class xTaskObject;
    friend class xThreadBase;
    friend class xThreadLocal;
    friend class xKernelMsgMgr;

public:
    explicit xMsgObject(UINT uMsgID = 0, WPARAM wPar = 0, LPARAM lPar = 0);

private:
    xMsgObject(const xMsgObject& objMessage);
    bool operator == (const xMsgObject& objMessage) const;
    xMsgObject& operator =(const xMsgObject& objMessage);

protected:
    bool                m_bIsWaitingReply;
    int                 m_nStatus;
    xMessageHandler*    m_pMsgHandler;
    msgMethod           m_pMethod;

public:
    LRESULT             lResult;
    UINT                message;
    WPARAM              wParam;
    LPARAM              lParam;

public:
    TASK_ID             m_idSrcTask;
};
typedef YLList<xMsgObject*> LIST_MSG_OBJECT;

//////////////////////////////////////////////////////////////////////////
// class xMsgResult
class xMsgResult
{
public:
    LRESULT GetResult();
    bool IsSuccessful();

public:
    xMsgResult& operator = (const xMsgObject& msg);
    xMsgResult& operator = (const xMsgResult& src);

public:
    xMsgResult(const xMsgResult& src);
    xMsgResult(LRESULT lResult);
    ~xMsgResult();

private:
    LRESULT             m_lResult;
    bool                m_bSuccessful;
};

//////////////////////////////////////////////////////////////////////////
// class taskObject
class xTaskObject
{
    friend class xThreadBase;
    friend class xThreadLocal;

public:
#if IF_MULTI_THREAD_ENABLE
    xTaskObject(chCriticalSection& csTask);
#else
    xTaskObject();
#endif
    ~xTaskObject();

public:
    void PushMessage(xMsgObject* pMsgObject);

    xMsgObject* ReadMessage();

#if IF_MULTI_THREAD_ENABLE
    void SetReplyMessage(xMsgObject* pMsgObject);
    xMsgObject* GetReplyMessage();
    bool WaitMessageReply(xTaskObject* pTarTaskObject, xMsgObject* pMsgObject,
                          xMsgResult& objResult, int nTimeout);
    void ReplyMessage(xMsgObject* pMsgObject);
#endif

protected:
#if IF_MULTI_THREAD_ENABLE
    xEvent_Socket* GetReplySocketEvent();
#endif

    void ClearMessageQueue();
    void CloseTaskObject();

protected:
    yl::string          m_strTaskName;
    PROCESS_ID          m_idProcess;
    TASK_ID             m_idTask;

private:
    // 接收消息的端口
    int                 m_nAwakePort;
#if IF_MULTI_THREAD_ENABLE
    // 回复消息的端口(防止有Send消息时，丢掉消息处理)
    int                 m_nReplayPort;
    xEvent_Socket*      m_pReplayEvent;
    xMsgObject*         m_pReplayMsg;

    chCriticalSection&  m_csTask;
#endif
    LIST_MSG_OBJECT     m_listMsg;
};
typedef YLHashMap<TASK_ID, xTaskObject*> MAP_TASK_OBJECT;

#if IF_XTHREAD_LOCAL_EXIT_METHOD
//////////////////////////////////////////////////////////////////////////
// class xExitMethodWrap
class xExitMethodList;
typedef void (*msgExitMethod)();
class xExitMethodWrap
{
    friend class xExitMethodList;

public:
    void onExit();

public:
    xExitMethodWrap(xExitMethodList* pOwner, msgExitMethod method);
    ~xExitMethodWrap();

private:
    msgExitMethod       m_pMethod;
    xExitMethodList*    m_pOwnerList;
};
typedef YLList<xExitMethodWrap*> LIST_EXIT_METHOD;

//////////////////////////////////////////////////////////////////////////
// class xExitMethodList
class xExitMethodList
{
public:
    xExitMethodList();
    ~xExitMethodList();

public:
    bool AddExitMethod(xExitMethodWrap* pWrap);
    void RemoveExitMethod(xExitMethodWrap* pWrap);
    void ExecuteOnExit();

    bool IsHadExitMethod(xExitMethodWrap* pWrap);

protected:
    LIST_EXIT_METHOD    m_listExitMethod;
};
#endif

//////////////////////////////////////////////////////////////////////////
// class xThreadBase
class xThreadBase
{
    friend class xProcessLocal;

public:
    xThreadBase();
    virtual ~xThreadBase();

public:
    BOOL CheckThreadAlive();
    BOOL IsCalledByThisThread();

    PROCESS_ID GetProcessID();
    TASK_ID GetTaskID();

protected:
    xMsgObject* AllocThreadMessage(bool bIsSendMessage);

protected:
    void AfterProcessMessage(xMsgObject* pMsgObject);

public:
    BOOL postThreadMsgEx(UINT uMsgID, WPARAM wParam, LPARAM lParam,
                         xMessageHandler* pHandler = NULL, msgMethod pMethod = NULL);
#if IF_MULTI_THREAD_ENABLE
    xMsgResult sendThreadMsgEx(UINT uMsgID, WPARAM wParam, LPARAM lParam, int nTimeout,
                               xMessageHandler* pHandler = NULL, msgMethod pMethod = NULL);
#endif

protected:
    chCriticalSection   m_csThread;
    xTaskObject         m_TaskObject;
};

//////////////////////////////////////////////////////////////////////////
// class xThreadLocal
class xThreadLocal : public xMessageHandler, public xThreadBase
#if IF_XTHREAD_LOCAL_EXIT_METHOD
    , public xExitMethodList
#endif
{
#if IF_MULTI_THREAD_ENABLE
    friend class xProcessLocal;
#else
    friend class xKernelMsgMgr;
#endif

public:
    BOOL AttachCurrentThread();
    void DetachThread();
    BOOL Start();
    void Stop();
    int Run();

public:
    virtual BOOL AfterThreadLaunched();
    virtual void BeforeThreadShutdown();
    virtual void DefaultProcessMessage(xMsgObject& objMessage);
    virtual BOOL TryThreadMessage(msgObject&);

public:
    LRESULT SendThreadMsg(UINT uMsgID, WPARAM wParam, LPARAM lParam,
                          xMessageHandler* pHandler = NULL, msgMethod pMethod = NULL_METHOD);

    bool ConnectMsgReadHandler(SOCKET fd, xMessageHandler* pHandler, msgMethod pMessageMeth);
    bool DisconnectMsgReadHandler(SOCKET fd);

    xMsgResult InvokeThreadMsg(UINT uMsgID, WPARAM wParam, LPARAM lParam, xMessageHandler* pHandler, msgMethod pMethod);

protected:
    void CallMessageMeth(xMessageHandler* pHandler, msgMethod pMessageMeth, xMsgObject& objMessage);

    BOOL AwakeTask();

private:
    BOOL registerCurrentThread();
    void unregisterThread();

public:
    static IMPLEMENT_THREAD_METHOD(eventMessageThread);

public:
    xThreadLocal(const yl::string& strThreadName, THREAD_MSG_PROC_EX pCallbackEx);
    virtual ~xThreadLocal();

public:
    yl::string          m_strThreadName;
    THREAD_MSG_PROC_EX  m_pMsgCallbackEx;
    xTaskEvent          m_TaskEvent;
    BOOL                m_bRuning;

protected:
    int                 m_nExitCode;
    int                 m_iPriority;
    THREAD_ID           m_idThisThread;         // used for chJoinThread only
};
typedef YLList<xThreadLocal*> LIST_THREAD_LOCAL;

#if IF_MULTI_THREAD_ENABLE
//////////////////////////////////////////////////////////////////////////
// class xProcessLocal
class xProcessLocal
#if IF_XTHREAD_LOCAL_EXIT_METHOD
    : public xExitMethodList
#endif
{
    friend class xThreadLocal;
public: // section for xThreadLocal manager
    xThreadLocal* GetLocalThreadByTaskID(TASK_ID idTask);
    xThreadLocal* GetCurrentThreadObject();
    xTaskObject* GetTaskObject(TASK_ID idTask);

    bool IsValidLocalThread(xThreadLocal* pThread);

protected:
    void ClearGhostThread();

public: // section for event manager
    BOOL AwakeTask(TASK_ID idTask);

private:
    BOOL addLocalThread(xThreadLocal* pLocalThread);
    void removeLocalThread(xThreadLocal* pLocalThread);

public:
    xProcessLocal();
    ~xProcessLocal();

protected:
    chCriticalSection   m_csThread;
    LIST_THREAD_LOCAL   m_listThreadLocal;
};
#endif

//////////////////////////////////////////////////////////////////////////
// class xKernelMsgMgr
class xKernelMsgMgr
#if IF_MULTI_THREAD_ENABLE
    : public xProcessLocal
#endif
{
private:
    xKernelMsgMgr();
    ~xKernelMsgMgr();

public:
    xMsgObject* AllocMsgObject();
    void ReleaseMsgObject(xMsgObject* pMsgObject);
    void ClearCacheMsgObject();

#if !IF_MULTI_THREAD_ENABLE
    BOOL AwakeTask(TASK_ID idTask);
    xThreadLocal* GetCurrentThreadObject();
    bool IsValidLocalThread(xThreadLocal* pThread);
#endif

protected:
#if IF_MULTI_THREAD_ENABLE
    chCriticalSection   m_csMsgMgr;
#endif
    LIST_MSG_OBJECT     m_listCacheMsg;

public:
    DECLARE_XGLOBLE_OBJECT(xKernelMsgMgr);
};

#define g_KernelMsgMgr  REFERENCE_XGLOBLE_OBJECT(xKernelMsgMgr)

EXTERN_DECLARE(xThreadLocal* msgStartThread(LPCSTR lpszThreadName, LPTHREAD_MAIN pMethod, LPVOID lpThreadData, int iPriority));
EXTERN_DECLARE(xThreadLocal* msgStartThread(LPCSTR lpszThreadName, THREAD_MSG_PROC_EX pMessageMethod, int iPriority));

END_XKERNEL_NAMESPACE

#endif
