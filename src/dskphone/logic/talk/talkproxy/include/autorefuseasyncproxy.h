#pragma once

#include "basetalkproxy.h"
#include "talk/talklogic/include/talklogic_common.h"
// 封装通话异步处理事件
class CAutoRefuseASyncProxy
    : public CBaseTalkASyncProxy
{
public:
    explicit CAutoRefuseASyncProxy(int iSessionID = -1);
    // 设置自动拒绝来电定时器
    void SetAutoRefuseCallTimer(int iSessionID);
private:
    bool OnCallBack();
    void SetSessionID(int iSessionID);
};

