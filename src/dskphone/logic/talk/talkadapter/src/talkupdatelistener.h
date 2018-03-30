#pragma once

#include "talk/talklistener/include/basetalklistener.h"
#include "talk/talkproxy/include/scopedtimer.h"

class CTalkUpdateListener
    : public CBaseTalkListener
{
public:
    CTalkUpdateListener();

    /* ETLmsg消息回调函数 */
    virtual bool OnMessage(msgObject& msg);

    /* 异步刷新回调 */
    int OnUpdate(CScopedTimer& timer);

private:

    /* 通话状态变更， 需要返回idle */
    DECL_SIGNAL_HANDLER(OnStateChange);

    /* 数据变更，同步刷新 */
    DECL_SIGNAL_HANDLER(OnDataChange);

    /* 异步刷新 */
    DECL_SIGNAL_HANDLER(OnCallInfoChange);

    /* 开启通话，强制异步刷新 */
    DECL_SIGNAL_HANDLER(OnStartTalk);

    /* UI刷新通知 */
    DECL_SIGNAL_HANDLER(OnUpdateUI);

    /* 新建话路 */
    DECL_SIGNAL_HANDLER(OnSessionCreate);

private:
    CScopedTimer*       m_pDelayUpdate;
};

