#pragma once

#include <cstddef>
#include "talk/talkproxy/include/callbackbase.h"

class CScopedTimer
    : public CCallBackBase
{
public:

    typedef TimerCallBackFunctor    CallBackFunctor;
    typedef size_t                  size_type;

protected:
    enum
    {
        timer_error = (size_type)(-1),
        timer_invalid = 0,
        timer_repeated = 0,
        timer_once = 1,
    };

public:
    CScopedTimer()
        : m_uInterval(timer_invalid), m_uCount(timer_repeated)
    {}

    virtual ~CScopedTimer();

    // 杀定时器
    virtual void Kill();

    // 设置回调
    void SetCallBack(const CallBackFunctor& cb)
    {
        m_pCallBack = cb;
    }

    // 以毫秒为单位
    bool SetTimer(size_type ms, size_type count = timer_repeated);

    // 设置只回调一次
    bool SetTimerOnce(size_type ms)
    {
        return SetTimer(ms, timer_once);
    }

    // 是否正在运行
    bool IsOn() const
    {
        return m_uInterval != timer_invalid;
    }

    // 是否定时器错误
    bool IsError() const
    {
        return m_uCount != timer_error;
    }

    // 获取剩余次数
    size_type GetTick() const
    {
        return m_uCount;
    }

public:
    /* for inherits class */

    virtual size_t Get(int index = 0) const
    {
        return TIMER_ID(m_uInterval);
    }

protected:
    // 用于响应事件
    virtual bool OnTimeOut(msgObject& msg);

protected:

    size_type                   m_uInterval;
    size_type                   m_uCount;
    // 回调入口指针
    CallBackFunctor             m_pCallBack;
};
