#pragma once

#include <cstddef>
#include "talk/talkproxy/include/helpclass.h"

// 更新界面的方式，数值越大优先级越高
enum WND_UPDATE_METHOD
{
    WND_UPDATE_NONE  = 0,
    WND_UPDATE_SYNC  = 0x01 << 0,     /* 同步刷新，最低优先级的刷新方式 */
    WND_UPDATE_ASYNC = 0x01 << 1,     /* 异步刷新，覆盖同步刷新，更新通话信息时，往往伴随状态切换，所以延迟刷新 */
    WND_UPDATE_FORCE = 0x01 << 2,     /* 强制刷新 */
    WND_UPDATE_FORCE_SYNC = WND_UPDATE_SYNC + WND_UPDATE_FORCE,   /* 强制同步刷新，覆盖异步刷新，状态切换时同步显示（routine切换） */
    WND_UPDATE_FORCE_ASYNC = WND_UPDATE_ASYNC + WND_UPDATE_FORCE,  /* 强制异步刷新，覆盖以上，必须延时达到特定时间才允许刷新（开通话操作） */
};

class CScopedWNDUpdate
    : public AUTO_REF_COUNT, protected app::StackObjectBase
{
public:
    CScopedWNDUpdate(WND_UPDATE_METHOD m = WND_UPDATE_SYNC)
        : AUTO_REF_COUNT(m_uDepth), m_eRequestMethod(m)
    {}

    ~CScopedWNDUpdate();

    /* 设置刷新方式 */
    static void SetMethod(WND_UPDATE_METHOD eMethod = WND_UPDATE_SYNC);

    /* 设置退出其它页面 */
    static void ExitOtherModule()
    {
        ++m_uExitModule;
    }

private:
    // 全局
    static size_t                   m_uDepth;
    static size_t                   m_uExitModule;
    static WND_UPDATE_METHOD        m_eCacheMethod;
#if IF_FEATURE_AUTO_TIMER_ELAPSED
    static size_t                   m_uTimer;
#endif

private:
    WND_UPDATE_METHOD               m_eRequestMethod;
};


#define AUTO_WND_UPDATE CScopedWNDUpdate _unused_scoped_wnd_update_guard(WND_UPDATE_SYNC);
