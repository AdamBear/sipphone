#pragma once

#include <cstddef>
#include "talk/talkproxy/include/scopedtimer.h"
#include "ylvector.h"

class CScopedTimerEx
    : public CScopedTimer
{
public:

    enum
    {
        INVALID_PARAM = (size_t)(-1),
    };

public:

    CScopedTimerEx()
        : CScopedTimer()
    {
    }

    virtual ~CScopedTimerEx()
    {
        Kill();
    }

    /* get param by index */
    virtual size_t Get(int index = 0) const
    {
        size_t value = INVALID_PARAM;
        Get(index, value);
        return value;
    }

    virtual void Kill();

    size_t Size() const
    {
        return m_vecParam.size();
    }

public:

    /* set callback for timer killed */
    void SetCleanUpCallBack(const CallBackFunctor& cb)
    {
        m_pCleanUpCallBack = cb;
    }

    /* auto push param list */
    CScopedTimerEx& operator % (size_t value)
    {
        return Push(value);
    }

    CScopedTimerEx& Push(size_t value);

    /* pop the first param */
    size_t Pop() const;

    void PopAll() const
    {
        m_vecParam.clear();
    }

private:
    bool Get(int index, size_t& value) const;

private:
    typedef YLVector<size_t>    VEC_PARAM;

    /* extern param data */
    mutable VEC_PARAM           m_vecParam;
    /* callback for timer killed */
    CallBackFunctor             m_pCleanUpCallBack;
};
