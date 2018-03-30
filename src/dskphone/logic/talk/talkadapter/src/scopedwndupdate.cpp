#include "talkadapter_inc.h"

size_t CScopedWNDUpdate::m_uDepth = 0;
size_t CScopedWNDUpdate::m_uExitModule = 0;
WND_UPDATE_METHOD CScopedWNDUpdate::m_eCacheMethod = WND_UPDATE_NONE;
#if IF_FEATURE_AUTO_TIMER_ELAPSED
size_t CScopedWNDUpdate::m_uTimer = 0;
#endif

CScopedWNDUpdate::~CScopedWNDUpdate()
{
    /* 自身占用一个引用计数，所以为1，非最外层不刷新 */
    if (m_uDepth > 1)
    {
        return;
    }

    /* 不需要刷新 */
    if (m_eCacheMethod <= WND_UPDATE_NONE)
    {
        return;
    }

    bool bUpdateWnd = false;

    switch (m_eCacheMethod)
    {
    case WND_UPDATE_FORCE_ASYNC:
        {
            /* 强制异步刷新必须匹配才允许刷新 */
            bUpdateWnd = m_eRequestMethod == WND_UPDATE_FORCE_ASYNC;
        }
        break;
    case WND_UPDATE_FORCE_SYNC:
        {
            /* 强制同步刷新处理，只要有任何触发都应该刷新 */
            bUpdateWnd = m_eRequestMethod <= WND_UPDATE_FORCE_SYNC;
        }
        break;
    case WND_UPDATE_ASYNC:
        {
            /* 只要是异步就刷新 */
            bUpdateWnd = (m_eRequestMethod & WND_UPDATE_ASYNC);
        }
        break;
    case WND_UPDATE_SYNC:
        {
            /* 只要是同步就刷新 */
            bUpdateWnd = (m_eRequestMethod & WND_UPDATE_SYNC);
        }
        break;
    default:
        break;
    }

    TALK_DBG("method  cache[%d] request[%d] update[%d]", m_eCacheMethod, m_eRequestMethod, bUpdateWnd);

    /* 想要的可以执行的刷新方式不能满足要求 */
    if (!bUpdateWnd)
    {
        return;
    }

    /* 退出其它页面 */
    if (m_uExitModule > 0)
    {
        AdapterBox_ReturnToIdle();
        m_uExitModule = 0;
    }

    // 获取当前焦点Session的数据
    DataLogic2UI data;
    int iSessionID = _TalkAdapter.GetFocusedSessionID();
    _TalkUIData.GetDataBySessionID(iSessionID, data);

    TALK_DBG("TalkUIUpdateWnd begin session[%d] state[%d] method[%d][%d]", iSessionID, data.eSessionState, m_eCacheMethod, m_eRequestMethod);

    // 同步消息通知ui刷新界面
    AdapterBox_TalkUIUpdateWnd(&data);
    m_eCacheMethod = WND_UPDATE_NONE;

    TALK_DBG("TalkUIUpdateWnd end session[%d] state[%d] method[%d][%d]", iSessionID, data.eSessionState, m_eCacheMethod, m_eRequestMethod);

#if IF_FEATURE_AUTO_TIMER_ELAPSED
    struct timeval val;
    gettimeofday(&val, NULL);
    size_t timer = (val.tv_sec * 1000 + val.tv_usec / 1000);
    TALK_WARN("since last update wnd time elapsed [%d] ms", timer - m_uTimer);
    m_uTimer = timer;
#endif
}

void CScopedWNDUpdate::SetMethod(WND_UPDATE_METHOD eMethod/* = WND_UPDATE_SYNC*/)
{
    /* 高优先级策略才缓存 */
    if (eMethod > m_eCacheMethod)
    {
        m_eCacheMethod = eMethod;
    }
}
