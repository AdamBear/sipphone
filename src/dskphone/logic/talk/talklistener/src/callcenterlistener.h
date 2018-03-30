#ifndef __CALL_CENTER_LISTENER_H__
#define __CALL_CENTER_LISTENER_H__

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
#include "basetalklistener.h"

/* cc 业务 */

class CCallCenterListener : public CBaseTalkListener
{
public:
    CCallCenterListener();

private:
    DECL_SIGNAL_HANDLER(OnUIAction);

#if IF_FEATURE_START2START_ACD
    DECL_SIGNAL_HANDLER(OnSessionDestory);
    DECL_SIGNAL_HANDLER(OnRoutineChanged);
#endif //#if IF_FEATURE_START2START_ACD
    DECL_SIGNAL_HANDLER(OnGetDataToUI);

protected:
    int OnTimer(CScopedTimer& timer);

    virtual bool OnMessage(msgObject& msg);

    //设置当前队列信息
    bool SetCallCenterInfo(CCallInfo* pCallInfo, const yl::string& strInfo);
};

#endif //IF_FEATURE_BROADSOFT_CALL_CENTER
#endif //__CALL_CENTER_LISTENER_H__
