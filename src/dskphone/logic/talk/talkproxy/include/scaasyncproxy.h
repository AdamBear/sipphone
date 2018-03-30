#pragma once

#include "talk/talkproxy/include/basetalkproxy.h"

// 封装通话异步处理事件
class CSCAASyncProxy
    : public CBaseTalkASyncProxy
{
public:

    CSCAASyncProxy(int iSessionID)
        : CBaseTalkASyncProxy(iSessionID)
    {}

    bool OnBLAError();

private:
    virtual bool OnCallBack();
};
