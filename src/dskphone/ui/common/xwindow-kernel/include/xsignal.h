#ifndef __X_SIGNAL_H__
#define __X_SIGNAL_H__

#include <ETLLib/ETLLib.hpp>
#include <yllist.h>
#include "xkerneldefine.h"


BEGIN_XKERNEL_NAMESPACE

//////////////////////////////////////////////////////////////////////////
// signal & slot
class xSignal;
class xSlotHandler;
typedef void (xSlotHandler::*slotMethod)(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

struct ST_HANDLE_SLOT
{
    xSlotHandler*       m_pHandle;
    slotMethod          m_pSlotMethod;

    ST_HANDLE_SLOT()
        : m_pHandle(NULL)
        , m_pSlotMethod(NULL)
    {
    }

    ~ST_HANDLE_SLOT()
    {

    }
};

typedef YLList<ST_HANDLE_SLOT> LIST_HANDLE_SLOT;

class xSignal
{
public:
    xSignal();
    ~xSignal();

public:
    bool connect(xSlotHandler* pHandler, slotMethod pMethod);
    int disconnect(xSlotHandler* pHandler, slotMethod pMethod);
    int disconnect(xSlotHandler* pHandler);
    void emitSignal(WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

protected:
    LIST_HANDLE_SLOT    m_listSlot;
    LIST_HANDLE_SLOT    m_listProcess;
};

class xSlotHandler
{
public:
    xSlotHandler();
    ~xSlotHandler();
};

#define SLOT_METHOD(receiver, slotName)                         receiver, static_cast<slotMethod>(&slotName)
#define SLOT_CONNECT(sender, sigName, receiver, slotName)       (sender)->sigName.connect(SLOT_METHOD(receiver, slotName))
#define SLOT_DISCONNECT(sender, sigName, receiver, slotName)    (sender)->sigName.disconnect(SLOT_METHOD(receiver, slotName))

extern void signalActiveObject(xSignal* pSignal);
extern void signalDeactiveObject(xSignal* pSignal);

END_XKERNEL_NAMESPACE

USING_XKERNEL_NAMESPACE

#endif // __X_SIGNAL_H__
