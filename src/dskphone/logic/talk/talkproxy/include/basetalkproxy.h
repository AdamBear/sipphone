#pragma once

#include "ylstring.h"
#include "talk/talkproxy/include/scopedtimer.h"

// 封装通话异步处理事件
class CBaseTalkASyncProxy
    : public CScopedTimer
{
public:
    explicit CBaseTalkASyncProxy(int iSessionID);

    virtual ~CBaseTalkASyncProxy()
    {}

    virtual bool OnCallBack() = 0;

    int GetSessionID()
    {
        return m_iSessionID;
    }

private:
    int CallBack(CScopedTimer& timer)
    {
        return OnCallBack();
    }

protected:
    int   m_iSessionID;
};
