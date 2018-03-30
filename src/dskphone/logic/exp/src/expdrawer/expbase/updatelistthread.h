#ifndef _EXP_EXP_DRAWER_UPDATE_LIST_THREAD_H__
#define _EXP_EXP_DRAWER_UPDATE_LIST_THREAD_H__


#include <dskthread/mkthread.h>
#include "etlmsghandler/modetlmsghandler.h"
#include "exp/include/explogicdefine.h"
#include "expdefine.h"
#include "expmanagerbase.h"


class CUpdateListThread
{
private:
    CUpdateListThread();

    ~CUpdateListThread();

public:
    static bool OnMessage(msgObject& objMessage);

    static CUpdateListThread & GetInstance();

    void SetEXPManager(EXPManagerBase * pEXPManager)
    {
        m_pEXPManager = pEXPManager;
    }

    EXPManagerBase * m_pEXPManager;
};

#define  _ExpUpdateListThread CUpdateListThread::GetInstance()

#endif // !_EXP_EXP_DRAWER_UPDATE_LIST_THREAD_H__
