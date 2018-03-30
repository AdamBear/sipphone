#pragma once

#include "ylvector.h"
#include "ylhashmap.h"
#include "talk/talkproxy/include/helpclass.h"
#include "talk/talkproxy/include/callbackbase.h"
#include "talksignaliddefine.h"
#include "talklistenertypes.h"

class CSequenceHelp;
class CBaseTalkListener;
struct CListenInfo;

class CListenCenter
    : public app::SingletonObject<CListenCenter>
{
public:
    typedef ListenCallBackFunctor                    CallBackFunctor;
    typedef YLVector<const CBaseTalkListener*>       VEC_LISTENERS;

    /* 保存回调信息 */
    class HandlerInfo
    {
    public:
        const CBaseTalkListener*        obj;
        int                             priority;
        CallBackFunctor                 cb;

        HandlerInfo(const CBaseTalkListener* p = NULL, const CallBackFunctor& b = CallBackFunctor(), int prio = SIG_PRIORITY_NORMAL)
            : obj(p), cb(b), priority(prio)
        {}
    };

    typedef YLVector<HandlerInfo>                    VEC_HANDLE;
    typedef YLVector<SignalParams>                   VEC_SIG_PARAM;
    typedef YLHashMap<TALK_SIGNAL_TYPE, VEC_HANDLE>  MAP_SIGNAL;
    typedef YLHashMap<long long, CListenInfo*>       MAP_DATA;

public:

    CListenCenter();

    /* 信号入口 */
    int Emit(int flag, TALK_SIGNAL_TYPE sig, const SignalParams& sigp);

    // 注册信号接收器
    void Register(bool bsync, TALK_SIGNAL_TYPE sig, const CallBackFunctor& cb, const CBaseTalkListener* listener, int priority);

    // 获取数据
    CListenInfo* GetData(int iSessionID, bool bAutoCreate = false);

    // 删除数据
    void EraseData(int iSessionID);

    // 获取数据大小
    size_t GetPrivDataSize()
    {
        return m_mapData.size();
    }

    // 排序
    void Sort();

protected:
    // 异步接收函数
    bool AsyncEmit();

    // ETL事件处理
    bool OnETLMessage(msgObject& msg);

    // 信号发送
    int SigEmit(int flag, TALK_SIGNAL_TYPE sig, const SignalParams& sigp);
    int EmitImpl(int flag, const SignalParams& sigp, const VEC_HANDLE& vec);

    // 排序实现
    void SortImpl(MAP_SIGNAL& map, CSequenceHelp& help);

private:
    bool                m_bSendAsync;           // 是否发送异步信息，减少信号数量
    size_t              m_uSigDepth;            // 深度

    VEC_LISTENERS       m_vecInitialization;    // 初始化注册Listener指针集合
    VEC_SIG_PARAM       m_vecASyncSigParam;     // 异步消息体缓存

    MAP_SIGNAL          m_mapSignals;           // 同步接收器
    MAP_SIGNAL          m_mapASyncSignals;      // 异步接收器
    MAP_DATA            m_mapData;              // 数据
};

#define ListenCenter    GET_SINGLETON_INSTANCE(CListenCenter)

