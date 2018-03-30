#ifndef _CEXP_APP_BLACK_
#define _CEXP_APP_BLACK_

#include <dskthread/mkthread.h>
#include "etlmsghandler/modetlmsghandler.h"

#include "exp/include/explogicdefine.h"
#include "singletonclassdefine.h"
#include "service_config.h"

//消息处理函数
bool OnEXPMessage(msgObject& objMessage);

class CEXPAppBlack
{
public:

    CEXPAppBlack();

    ~CEXPAppBlack();

    //CEXPAppBlack(const CEXPAppBlack&);

public:
    static CEXPAppBlack & GetInstance();
};
#define _EXPLcdDrawThread GET_SINGLETON_INSTANCE(CEXPAppBlack)
#endif // !define __BTTHREAD_H__
