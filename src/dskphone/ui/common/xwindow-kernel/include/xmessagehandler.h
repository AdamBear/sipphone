#ifndef __MESSAGE_HANDLER_H__
#define __MESSAGE_HANDLER_H__

#include "xkerneldefine.h"
#include <ETLLib/ETLLib.hpp>


BEGIN_XKERNEL_NAMESPACE

class xThreadLocal;
class xMessageHandler;

typedef BOOL (xMessageHandler::*msgMethod)(msgObject& objMessage);
#define MSG_METHOD(pObj, meth) pObj, static_cast<msgMethod>(&meth)
#define NULL_MESSAGE (*(msgObject*)NULL)

//////////////////////////////////////////////////////////////////////////
// class xMessageHandler
class xMessageHandler
{
    friend class xThreadLocal;

public:
    xMessageHandler();
    virtual ~xMessageHandler();

protected:
    xMessageHandler(xThreadLocal* pHostThread);

public:
    TASK_ID hostTaskId() const;
    xThreadLocal* hostThread() const;

public:
    virtual BOOL TryThreadMessage(msgObject& objMessage);

protected:
    xThreadLocal*                   m_pHostThread;
};

extern bool handlerActiveObject(xMessageHandler* pHandler);
extern void handlerDeactiveObject(xMessageHandler* pHandler);
extern bool handlerIsActiveObject(xMessageHandler* pHandler);

END_XKERNEL_NAMESPACE

USING_XKERNEL_NAMESPACE

#endif
