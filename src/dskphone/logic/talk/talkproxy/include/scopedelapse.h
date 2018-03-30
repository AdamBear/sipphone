#pragma once

#include <cstddef>
#include "talk/talkproxy/include/helpclass.h"

/* 扩展日志 */
#define IF_FEATURE_EXTEND_LOG               1
/* 时间收集 */
#define IF_FEATURE_AUTO_TIMER_ELAPSED       1

#if IF_FEATURE_AUTO_TIMER_ELAPSED

namespace app
{

class CScopedTimerElapse;

class CScopedTimerElapseGuard
    : public app::StackObjectBase
{
    friend class CScopedTimerElapse;

public:
    typedef size_t size_type;

    CScopedTimerElapseGuard(size_type i)
    {
        m_timer = s_timeout;
        s_timeout = i;
    }

    ~CScopedTimerElapseGuard()
    {
        s_timeout = m_timer;
    }

private:

    static size_type  s_timeout;

    size_type         m_timer;
};

class CScopedTimerElapse
    : public app::StackObjectBase
{
public:
    typedef size_t size_type;
    typedef const char* name_type;

    CScopedTimerElapse(name_type scope, size_type id)
        : m_lpszName(scope), m_uID(id), m_uStart(getTick())
    {}

    // return ms
    size_type getTick();

    ~CScopedTimerElapse();

private:
    size_t          m_uStart;
    size_t          m_uID;
    name_type       m_lpszName;
};

} // namespace

#   define  UNNAMED_AUTO_TIMER_THRESHOLD(timer, uuid) app::CScopedTimerElapseGuard _unused_scoped_timer_elapse_guard##uuid(timer)
#   define  UNNAMED_AUTO_TIMER(scope, id, uuid)       app::CScopedTimerElapse _unused_scoped_timer_elapse##uuid(scope, id)
#else
#   define  UNNAMED_AUTO_TIMER_THRESHOLD(timer)
#   define  UNNAMED_AUTO_TIMER(scope, id, uuid)
#endif

#define  AUTO_TIMER_THRESHOLD(timer)            UNNAMED_AUTO_TIMER_THRESHOLD(timer, __LINE__)
#define  AUTO_SCOPED_TIMER_NAME_ID(scope, id)   UNNAMED_AUTO_TIMER(scope, id, __LINE__)
#define  AUTO_SCOPED_TIMER_ID(id)               AUTO_SCOPED_TIMER_NAME_ID(__FUNCTION__, id)
#define  AUTO_SCOPED_TIMER                      AUTO_SCOPED_TIMER_ID(__LINE__)


#if IF_FEATURE_EXTEND_LOG
extern void dbg_long_log(const char* fmt, ...);

#   define  TALK_DBG(fmt,...)  TALK_INFO("DBG: " fmt, ##__VA_ARGS__)
#   define  APP_DBG(fmt,...)   APP_INFO("DBG: " fmt, ##__VA_ARGS__)
#   define  LONG_LOG(fmt, ...) dbg_long_log(fmt, ##__VA_ARGS__)
#else
#   define  APP_DBG(fmt,...)
#   define  TALK_DBG(fmt,...)  APP_DBG(fmt, ##__VA_ARGS__)
#   define  LONG_LOG(fmt,...)  APP_DBG(fmt, ##__VA_ARGS__)
#endif
