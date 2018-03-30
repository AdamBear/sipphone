#include "scopedelapse.h"
#include "ETLLib.hpp"
#include "dsklog/log.h"
#include <winimp/winimp.h>

#if IF_FEATURE_AUTO_TIMER_ELAPSED

app::CScopedTimerElapseGuard::size_type app::CScopedTimerElapseGuard::s_timeout = 35;

app::CScopedTimerElapse::size_type app::CScopedTimerElapse::getTick()
{
    struct timeval val;
    gettimeofday(&val, NULL);
    return val.tv_sec * 1000 + val.tv_usec / 1000;
}

app::CScopedTimerElapse::~CScopedTimerElapse()
{
    size_type uTimer = getTick() - m_uStart;
    // 避免频繁日志打印，只有超过阈值的才打印
    if (uTimer >= CScopedTimerElapseGuard::s_timeout)
    {
        TALK_WARN("scope[%s] id[%d] take too much timer with[%3ld]ms", m_lpszName, m_uID, uTimer);
    }
}
#endif

#if IF_FEATURE_EXTEND_LOG
#define DBG_LOG_LINE_MAX_LEN    128 // DBG_LOG_LOGMAX

// split words by ctr char
static void dbg_split_by_cntrl(const char* buffer, int len)
{
    if (buffer == NULL || len == 0)
    {
        return;
    }

    const char* pos = buffer;
    const char* end = buffer + len;

    // split by ctr
    while (1)
    {
        if (pos == end)
        {
            break;
        }

        if (end - pos <= DBG_LOG_LINE_MAX_LEN)
        {
            APP_INFO("%s", pos);
            break;
        }
        else
        {
            const char* pre = pos + DBG_LOG_LINE_MAX_LEN;
            // split words by space or punct
            while (pre != pos)
            {
                if (isblank(*pre) || ispunct(*pre))
                {
                    break;
                }

                --pre;
            }

            int size = pre - pos;
            if (size == 0)
            {
                size = DBG_LOG_LINE_MAX_LEN;
            }

            APP_INFO("%.*s", size, pos);
            pos += size;
        }
    }
}

// split words by line
static void dbg_split_by_line(const char* buffer, int len)
{
    if (buffer == NULL || len == 0)
    {
        return;
    }

    const char* pos = buffer;
    const char* end = buffer + len;

#define LINE_BRK(ch) (ch == '\r' || ch == '\n')
    // split by cntrl (such as \r or \n)
    while (1)
    {
        if (pos == end)
        {
            break;
        }

        const char* pre = pos;
        while (pos < end)
        {
            char ch = *(pos++);
            if (LINE_BRK(ch))
            {
                ch = *pos;
                if (pos == end || !LINE_BRK(ch))
                {
                    break;
                }
            }
        }

        DEF_LOG(APP, DBG_LOG_LEVEL_INFO, "%.*s", pos - pre, pre);
    }
}

static void dbg_long_log_impl(const char* fmt, va_list vl)
{
    int len = vsnprintf(NULL, 0, fmt, vl);
    if (len <= 0)
    {
        return;
    }

    char* buffer = (char*)malloc(sizeof(char) * (len + 1));
    if (buffer == NULL)
    {
        return;
    }

    memset(buffer, 0, len + 1);
    vsnprintf(buffer, len + 1, fmt, vl);

    if (len > DBG_LOG_LINE_MAX_LEN && strpbrk(buffer, "\r\n") != NULL)
    {
        dbg_split_by_line(buffer, len);
    }
    else
    {
        dbg_split_by_cntrl(buffer, len);
    }

    free(buffer);
}

void dbg_long_log(const char* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    dbg_long_log_impl(fmt, vl);
    va_end(vl);
}
#endif
