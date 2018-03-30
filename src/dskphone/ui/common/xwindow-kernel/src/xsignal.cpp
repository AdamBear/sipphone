#include "xsignal.h"
#include "xkernelcommon.h"
#include "dsklog/log.h"


BEGIN_XKERNEL_NAMESPACE

//////////////////////////////////////////////////////////////////////////
// xSignal
xSignal::xSignal()
{
    signalActiveObject(this);
}

xSignal::~xSignal()
{
    signalDeactiveObject(this);

    m_listSlot.clear();
    m_listProcess.clear();
}

bool xSignal::connect(xSlotHandler* pHandler, slotMethod pMethod)
{
    chASSERT(NULL != pHandler);
#if IF_XKERNEL_THREAD_CHECK
    chWARNING(pHandler->hostThread()->IsCalledByThisThread());
#endif

    for (LIST_HANDLE_SLOT::iterator it = m_listSlot.begin(); it != m_listSlot.end(); ++it)
    {
        const ST_HANDLE_SLOT& stSlot = *it;

        if (stSlot.m_pHandle == pHandler)
        {
            if (stSlot.m_pSlotMethod == pMethod)
            {
                return false;
            }
            else
            {
                chWARNINGx(FALSE, "Another slot had conected");
            }
        }
    }

    ST_HANDLE_SLOT stSlot;

    stSlot.m_pHandle = pHandler;
    stSlot.m_pSlotMethod = pMethod;

    m_listSlot.push_back(stSlot);

    return true;
}

int xSignal::disconnect(xSlotHandler* pHandler, slotMethod pMethod)
{
    LIST_HANDLE_SLOT::iterator it = m_listSlot.begin();
    int nRemovedCount = 0;

    while (it != m_listSlot.end())
    {
        const ST_HANDLE_SLOT& stSlot = *it;

        if (stSlot.m_pHandle == pHandler && stSlot.m_pSlotMethod == pMethod)
        {
            it = m_listSlot.erase(it);
            ++nRemovedCount;

            continue;
        }

        ++it;
    }

    for (it = m_listProcess.begin(); it != m_listProcess.end();)
    {
        const ST_HANDLE_SLOT& stSlot = *it;

        if (stSlot.m_pHandle == pHandler && stSlot.m_pSlotMethod == pMethod)
        {
            it = m_listProcess.erase(it);

            continue;
        }

        ++it;
    }
    return nRemovedCount;
}

int xSignal::disconnect(xSlotHandler* pHandler)
{
    LIST_HANDLE_SLOT::iterator it = m_listSlot.begin();
    int nRemovedCount = 0;

    for (; it != m_listSlot.end();)
    {
        const ST_HANDLE_SLOT& stSlot = *it;

        if (stSlot.m_pHandle == pHandler)
        {
            it = m_listSlot.erase(it);
            ++nRemovedCount;

            continue;
        }

        ++it;
    }

    for (it = m_listProcess.begin(); it != m_listProcess.end();)
    {
        const ST_HANDLE_SLOT& stSlot = *it;

        if (stSlot.m_pHandle == pHandler)
        {
            it = m_listProcess.erase(it);

            continue;
        }

        ++it;
    }

    return nRemovedCount;
}

void xSignal::emitSignal(WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    slotMethod pEmptySlotMethod = NULL;

    // 允许在槽函数中disconnect或释放xSignal对象
    m_listProcess.clear();
    m_listProcess = m_listSlot;

    xSignal* pThis = this;
    int nLoop = 0;

    while (m_listProcess.size() > 0)
    {
        DEAD_LOOP_BREAK(nLoop);

        ST_HANDLE_SLOT stSlot = m_listProcess.front();

        m_listProcess.erase(m_listProcess.begin());

        xSlotHandler* pHandler = stSlot.m_pHandle;

        if (NULL == pHandler || pEmptySlotMethod == stSlot.m_pSlotMethod)
        {
            continue;
        }

#if IF_XKERNEL_THREAD_CHECK
        chWARNINGx(pHandler->hostThread()->IsCalledByThisThread(), "emitSignal task[%d] not in host thread(%d).",
                   pHandler->hostTaskId(), etlGetCurrentTaskId());
#endif

        (pHandler->*(stSlot.m_pSlotMethod))(pThis, wParam, lParam, nDataBytes, pData);
    }
}

//////////////////////////////////////////////////////////////////////////
// xSignalSlotManager
class xSignalSlotManager
{
    typedef YLList<xSignal*> LIST_SIGNAL;

public:
    xSignalSlotManager()
    {
    }

    ~xSignalSlotManager()
    {
        m_listSignal.clear();
    }

    static xSignalSlotManager& getInstance()
    {
#if IF_MULTI_XKERNEL_ENABLE
        MULTI_KERNEL_INSTANCE(xSignalSlotManager);
#else
        SINGLE_KERNEL_INSTANCE(xSignalSlotManager);
#endif
    }

public:
    void ActivateSignal(xSignal* pSignal)
    {
        m_listSignal.push_back(pSignal);
    }

    void DeactivateSignal(xSignal* pSignal)
    {
        m_listSignal.remove(pSignal);
    }

    int ClearSoltHandler(xSlotHandler* pHandler)
    {
        int nRemovedCount = 0;

        for (LIST_SIGNAL::iterator it = m_listSignal.begin(); it != m_listSignal.end(); ++it)
        {
            xSignal* pSignal = *it;

            if (NULL == pSignal)
            {
                continue;
            }

            nRemovedCount += pSignal->disconnect(pHandler);
        }

        return nRemovedCount;
    }

private:
    LIST_SIGNAL m_listSignal;
};

#define g_mgrSignalSlot xSignalSlotManager::getInstance()

void signalActiveObject(xSignal* pSignal)
{
    g_mgrSignalSlot.ActivateSignal(pSignal);
}

void signalDeactiveObject(xSignal* pSignal)
{
    g_mgrSignalSlot.DeactivateSignal(pSignal);
}

//////////////////////////////////////////////////////////////////////////
// xSlotHandler
xSlotHandler::xSlotHandler()
{
}

xSlotHandler::~xSlotHandler()
{
    g_mgrSignalSlot.ClearSoltHandler(this);
}

END_XKERNEL_NAMESPACE
