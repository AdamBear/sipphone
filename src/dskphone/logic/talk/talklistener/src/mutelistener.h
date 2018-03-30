#ifndef __MUTE_LISTENER_H__
#define __MUTE_LISTENER_H__

#include "basetalklistener.h"

/* mute 业务 */

class CMuteListener : public CBaseTalkListener
{
public:
    CMuteListener();

    DECL_SIGNAL_HANDLER(OnSessionExit);

    DECL_SIGNAL_HANDLER(OnRoutineChanged);

    DECL_SIGNAL_HANDLER(OnUiAction);

    DECL_SIGNAL_HANDLER(OnSessionFocusChange);
protected:
    /* ETLmsg消息回调函数 */
    bool OnMessage(msgObject& msg);


private:
    bool SessionMuteStateChange(const CSessionProxy& sp);
    bool IsNeedMuteAudio(CCallInfo* pCallInfo);
    bool SingleSessionMuteChange(const CSessionProxy& sessionProxy);

    bool GetMuteStatus();
    bool SetMuteStatus(bool bMute);

    void SetLight();

public:
    // 全局Mute状态
    static bool m_bGlobalMute;
};

#endif
