#ifndef __COMMON_LISTENER_H__
#define __COMMON_LISTENER_H__

#include "basetalklistener.h"

/* 外部零散业务集合 */

class CCommonListener : public CBaseTalkListener
{
private:
    // 键盘锁的处理
    static bool IsAllowCallOutByKeyguard(const yl::string& strNumber);
    // 是否拒接匿名呼叫
    static bool IsRefuseAnonymousCall(const CallInUserInfo& userInfo, int iAccount);
#ifdef IF_FEATURE_EMERGENCY
    bool OnAnswerEmergencyReCallRespond(CCallInfo& callInfo, bool bAccept);
#endif
public:
    CCommonListener();

    DECL_SIGNAL_HANDLER(OnActionEvent);

    DECL_SIGNAL_HANDLER(OnRoutineFlagDataInit);

    DECL_SIGNAL_HANDLER(OnRoutineChange);

    DECL_SIGNAL_HANDLER(OnTalk2IDLE);

    DECL_SIGNAL_HANDLER(OnSessionDestory);

    DECL_SIGNAL_HANDLER(OnRoutineChanged);

    DECL_SIGNAL_HANDLER(OnFoucsChange);

    DECL_SIGNAL_HANDLER(OnDialCallout);

    DECL_SIGNAL_HANDLER(OnCallIncome);

    DECL_SIGNAL_HANDLER(OnPageReceived);

    DECL_SIGNAL_HANDLER(OnMuteChange);

};

#endif
