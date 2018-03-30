#include "xmessagehandler.h"
#include "yllist.h"
#include "xkernel.h"
#include "xkernelcommon.h"


BEGIN_XKERNEL_NAMESPACE

extern bool handlerActiveObject(xMessageHandler* pHandler);
extern void handlerDeactiveObject(xMessageHandler* pHandler);
extern bool handlerIsActiveObject(xMessageHandler* pHandler);
typedef YLList<xMessageHandler*> LIST_MSG_HANDLER;

//////////////////////////////////////////////////////////////////////////
// class xMessageHandler
xMessageHandler::xMessageHandler()
{
    handlerActiveObject(this);
    m_pHostThread = g_KernelMsgMgr.GetCurrentThreadObject();
    chWARNINGx(m_pHostThread != NULL, "xMessageHandler should be construct after xThreadLocal launched.");
}

xMessageHandler::xMessageHandler(xThreadLocal* pHostThread)
{
    handlerActiveObject(this);
    m_pHostThread = pHostThread;
}

xMessageHandler::~xMessageHandler()
{
    m_pHostThread = NULL;

    handlerDeactiveObject(this);
}

xThreadLocal* xMessageHandler::hostThread() const
{
    return m_pHostThread;
}

TASK_ID xMessageHandler::hostTaskId() const
{
    if (NULL != m_pHostThread)
    {
        return m_pHostThread->GetTaskID();
    }

    return 0;
}

BOOL xMessageHandler::TryThreadMessage(msgObject& /*objMessage*/)
{
    return FALSE;
}
//////////////////////////////////////////////////////////////////////////
class xHandlerHelper
{
public:
    static xHandlerHelper& GetInstance();

public:
    bool ActivateHandler(xMessageHandler* pHandler);
    void DeactivateHandler(xMessageHandler* pHandler);
    bool IsActiveHandler(xMessageHandler* pHandler);

private:
    LIST_MSG_HANDLER        m_listHandler;
};

xHandlerHelper& xHandlerHelper::GetInstance()
{
#if IF_MULTI_XKERNEL_ENABLE
    MULTI_KERNEL_INSTANCE(xHandlerHelper)
#else
    SINGLE_KERNEL_INSTANCE(xHandlerHelper)
#endif
}

bool xHandlerHelper::ActivateHandler(xMessageHandler* pHandler)
{
    if (NULL == pHandler)
    {
        return false;
    }

    if (IsActiveHandler(pHandler))
    {
        return true;
    }

    m_listHandler.push_back(pHandler);

    return true;
}

void xHandlerHelper::DeactivateHandler(xMessageHandler* pHandler)
{
    LIST_MSG_HANDLER::iterator iter = m_listHandler.begin();
    LIST_MSG_HANDLER::iterator iterEnd = m_listHandler.end();

    for (; iter != iterEnd; ++iter)
    {
        if (pHandler == (*iter))
        {
            m_listHandler.erase(iter);

            break;
        }
    }
}

bool xHandlerHelper::IsActiveHandler(xMessageHandler* pHandler)
{
    if (NULL == pHandler)
    {
        return false;
    }

    LIST_MSG_HANDLER::iterator iter = m_listHandler.begin();
    LIST_MSG_HANDLER::iterator iterEnd = m_listHandler.end();

    for (; iter != iterEnd; ++iter)
    {
        if (pHandler == (*iter))
        {
            return true;
        }
    }

    return false;
}

#define g_HandlerHelper xHandlerHelper::GetInstance()

bool handlerActiveObject(xMessageHandler* pHandler)
{
    return g_HandlerHelper.ActivateHandler(pHandler);
}

void handlerDeactiveObject(xMessageHandler* pHandler)
{
    g_HandlerHelper.DeactivateHandler(pHandler);
}

bool handlerIsActiveObject(xMessageHandler* pHandler)
{
    return g_HandlerHelper.IsActiveHandler(pHandler);
}

END_XKERNEL_NAMESPACE
