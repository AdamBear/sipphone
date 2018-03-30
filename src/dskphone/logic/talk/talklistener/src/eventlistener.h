#ifndef __ACTION_URL_LISTENER_H__
#define __ACTION_URL_LISTENER_H__

#include "basetalklistener.h"

/* event业务：url、csta等 */

class CEventListener : public CBaseTalkListener
{
public:
    CEventListener();

    DECL_SIGNAL_HANDLER(OnHold);

    DECL_SIGNAL_HANDLER(OnHeld);

    DECL_SIGNAL_HANDLER(OnDTMF);

    DECL_SIGNAL_HANDLER(OnRoutineChange);

    DECL_SIGNAL_HANDLER(OnCallIDChange);

    DECL_SIGNAL_HANDLER(OnMuteChange);

    DECL_SIGNAL_HANDLER(OnTransfer);

    DECL_SIGNAL_HANDLER(OnSessionExit);

    DECL_SIGNAL_HANDLER(OnUIAction);

    DECL_SIGNAL_HANDLER(OnHostChange);

    DECL_SIGNAL_HANDLER(OnSaveLog);

    DECL_SIGNAL_HANDLER(OnAnwserCall);

    DECL_SIGNAL_HANDLER(OnRefuseCall);

    DECL_SIGNAL_HANDLER(OnMakeCall);

    DECL_SIGNAL_HANDLER(OnHangUp);

    DECL_SIGNAL_HANDLER(OnFWD);

    DECL_SIGNAL_HANDLER(OnSessionSizeChange);

    DECL_SIGNAL_HANDLER(OnStopTalk);

    DECL_SIGNAL_HANDLER(OnCallIncome);

    DECL_SIGNAL_HANDLER(OnCallout);

protected:
    virtual bool OnMessage(msgObject& msg);

private:
    static int SendEvent(int iEvent, const SignalParams& sigp, CallInUserInfo* pUserInfo = NULL);

    bool            m_bInIdle;  // 是否处于Idle状态
#if IF_BUG_30423
    yl::string      m_strSIPName;  //当前通话对方号码
#endif
};

#endif
