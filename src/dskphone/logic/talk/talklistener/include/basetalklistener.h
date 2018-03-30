#pragma once

#include "talksignaliddefine.h"
#include "talk/talkproxy/include/callbackbase.h"

class CSessionProxy;
class CCallInfo;
class CSessionStateProxy;
struct SignalParams;
struct CListenInfo;


// 定义信号处理函数
#define DECL_SIGNAL_HANDLER(entry) int entry(const SignalParams& sigp, CListenInfo* priv)

class CBaseTalkListener
    : public CCallBackBase
{
    typedef ListenCallBackFunctor   CallBackFunctor;

public:
    explicit CBaseTalkListener(ListenerName nm);

    ListenerName GetName() const
    {
        return name;
    }

protected:

    /* ETLmsg消息回调函数 */
    virtual bool OnMessage(msgObject& msg)
    {
        return false;
    }

    /* 防止创建堆对象*/
    virtual ~CBaseTalkListener()
    {}

    /* ETLmsg消息注册 */
    void RegETLMsg(size_t from, size_t to = 0);

    /* 注册信号接收入口 */
    void Connect(bool bsync, TALK_SIGNAL_TYPE sig, const CallBackFunctor& cb, int priority);

    /* ETLmsg消息回调函数 */
    bool OnETLMessage(msgObject& msg);

protected:

    /* 获取私有数据 */
    static CListenInfo* GetCallerData(int iCallID);

    static CListenInfo* GetCallerData(CCallInfo* p);

    static CListenInfo* GetSessionData(int iSessionID);

protected:
    ListenerName const name;  // 名称标志
};

#define LSN_CLS_LOG_NAME       CLS_LOG_NAME(CBaseTalkListener)

#define CALL_HANDLER(entry)	   this->entry(sigp, priv)

#define SIG_REPLY(reply) \
        { sigp.uReply = reply;  \
        TALK_DBG("break at listener[%s] sig[%d][%s] session[%d][%d] reply[%d] pos[%s:%d] syntax[%s]", name, sigp.eSignal, GetSignalName(sigp.eSignal), sigp.iSessionID, sigp.iCallID, sigp.uReply, __FUNCTION__, __LINE__, #reply); \
        return SIG_RET_BREAK | sigp.uReply; }


// 连接信号处理函数
#define REG_SIGNAL(sync, sig, cb, priority)   \
        { TALK_DBG("reg listener[%s] sync[%s] sig[%d][%s] callback[%s]", name, #sync, sig, #sig, #cb); \
        CBaseTalkListener::Connect(sync, sig, MAKE_LISTEN_CALL_BACK(cb), priority); }

#define REG_SYNC_SIGNAL(sig, cb)    REG_SIGNAL(true , sig, cb, SIG_PRIORITY_NORMAL)
#define REG_ASYNC_SIGNAL(sig, cb)   REG_SIGNAL(false, sig, cb, SIG_PRIORITY_NORMAL)

