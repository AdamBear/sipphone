#pragma once

#include "basetalkproxy.h"
#include "talk/talklogic/include/talklogic_common.h"
class CCallInfo;
struct CallInUserInfo;

// 封装通话异步处理事件
class CRefuseASyncProxy
    : public CBaseTalkASyncProxy
{
public:
    explicit CRefuseASyncProxy(int iSessionID);
    bool RefuseBySIPInfo(CCallInfo* pCallInfo, CallInUserInfo* pUserInfo);
protected:
    bool OnCallBack();
private:
    //设置拒绝来电定时器
    void SetRefuseCallTimer();
};
