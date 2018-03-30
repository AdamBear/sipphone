#ifndef __CALL_MISC_LISTENER_H__
#define __CALL_MISC_LISTENER_H__

#include "talk/talkproxy/include/delaycalloutasyncproxy.h"
#include "basetalklistener.h"

/* 处理通话的一些琐碎业务杂项 */

class CCallMiscListener : public CBaseTalkListener
{
public:
    CCallMiscListener();

    // 是否能够接收来电
    static bool IsAllowIncomingCall(int iAccountID);

private:

    /* ETLmsg消息回调函数 */
    virtual bool OnMessage(msgObject& msg);

    int OnCableTimer(CScopedTimer& timer);

    int OnIdleTimer(CScopedTimer& timer);

    // 处理呼出号码和账号
    bool FormatAccountAndNumber(CCallInfo* pCallInfo, yl::string& strFullName,
                                yl::string& strExtNumber, LPCSTR lpszName = "");

    // 格式化成E164
    void FormatE164Number(yl::string& strNumber);

    // 选择ip账号
    int SelectAccount(bool bEmergencyCall);

    typedef CallOutInfo::LIST_INFO    LIST_INFO;

private:

    DECL_SIGNAL_HANDLER(OnRoutineChanged);

    DECL_SIGNAL_HANDLER(OnCallIncome);

    DECL_SIGNAL_HANDLER(OnSessionDestory);

    DECL_SIGNAL_HANDLER(OnUIEvent);

    DECL_SIGNAL_HANDLER(OnTryEnterFinish);

    DECL_SIGNAL_HANDLER(OnTryCallOut);

    DECL_SIGNAL_HANDLER(OnSessioCreate);

private:
    CScopedTimer*       m_pCableTimer;       // 检测网络不可用定时器
    CScopedTimer*       m_pIdleTimer;        // 检测idle定时器
    bool                m_bCheckIdle;        // 检测Idle还是Talk状态
    LIST_INFO           m_listDelayInvite;   // 延时发送invite定时器
};

#endif
