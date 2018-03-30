#ifndef __FWD_DND_LISTENER_H__
#define __FWD_DND_LISTENER_H__

#include "basetalklistener.h"

/* fwd、dnd 业务 */

class CFWDDNDListener : public CBaseTalkListener
{
public:
    CFWDDNDListener();

protected:

    DECL_SIGNAL_HANDLER(OnIdleToTalk);
    DECL_SIGNAL_HANDLER(OnTalkToIdle);

    DECL_SIGNAL_HANDLER(OnCallIncome);
    DECL_SIGNAL_HANDLER(OnCallRefuse);
    DECL_SIGNAL_HANDLER(OnCallReject);

    DECL_SIGNAL_HANDLER(OnRoutineChanged);
    DECL_SIGNAL_HANDLER(OnUIAction);

    DECL_SIGNAL_HANDLER(OnSaveLog);

    DECL_SIGNAL_HANDLER(OnSessionFWD);
    DECL_SIGNAL_HANDLER(OnPageReceived);

private:
    bool SaveNoTalkingCallLog(int iAccount, CallInUserInfo& userInfo, int iCallID, bool bIsDND = false);
    BOOL ForwardIncomingCall(int iLineId, int iCallID, FWD_TYPE eFwdType);
    void PopFwdMessageBox(int iLineId, FWD_TYPE eFwdType);
};

#endif
