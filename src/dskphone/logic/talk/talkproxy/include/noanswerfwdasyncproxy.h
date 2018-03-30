#pragma once

#include "basetalkproxy.h"
#include "talk/fwd_dnd/include/fwddndtypes.h"
class CCallInfo;
// 封装通话异步处理事件
class CNoAnswerFWDASyncProxy
    : public CBaseTalkASyncProxy
{
public:
    explicit CNoAnswerFWDASyncProxy(int iSessionID = -1);
    void SetFWDTimer(CCallInfo* pCallInfo);
    bool OnCallBack();
    void ForwardIncomingCall(const char* lpszNumber = NULL, FWD_TYPE eFwdType = FWD_TYPE_MANUAL);

    int GetFWDTimerValue()
    {
        return m_iTimerValue;
    }
private:
    int m_iTimerValue;
};

