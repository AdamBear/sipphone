#ifndef __CONTRACT_HELPER_LISTENER_H__
#define __CONTRACT_HELPER_LISTENER_H__

#include "basetalklistener.h"

/* 联系人相关业务 */

class CContractListener : public CBaseTalkListener
{
public:
    CContractListener();

private:

    static bool UpdateCallinfo(const CSessionProxy& proxy, bool bSearchDisp);

    /* ETLmsg消息回调函数 */
    virtual bool OnMessage(msgObject& msg);

private:

    DECL_SIGNAL_HANDLER(OnRoutineChanged);

    DECL_SIGNAL_HANDLER(OnRoutineChange);

    DECL_SIGNAL_HANDLER(OnCallInfoChange);

    DECL_SIGNAL_HANDLER(OnSessionDestory);

    DECL_SIGNAL_HANDLER(OnCallIncome);

    DECL_SIGNAL_HANDLER(SaveCallLog);

    DECL_SIGNAL_HANDLER(OnCallIDChanged);

    DECL_SIGNAL_HANDLER(OnTransfer);

    DECL_SIGNAL_HANDLER(OnSubActived);

};

#endif
