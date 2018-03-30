#pragma once
#include "basetalkproxy.h"
#include "talk/talklogic/include/talklogic_common.h"

class CCallInfo;
struct CallInUserInfo;
class CRefuseASyncProxy;

// 封装通话异步处理事件
class CAutoAnswerASyncProxy
    : public CBaseTalkASyncProxy
{
public:
    explicit CAutoAnswerASyncProxy(int iSessionID);
    ~CAutoAnswerASyncProxy();
    bool OnCallBack();
    bool ProcessAutoAnswer(CCallInfo* pCallInfo);
    bool ProcessAccountAutoAnswer(CCallInfo* pCallInfo);
    bool ProcessIntercom(CCallInfo* pCallInfo);
private:
    bool AutoAnswerBySIPInfo(CCallInfo* pCallInfo, CallInUserInfo* pUserInfo);
    // 设置自动应答定时器
    void SetAutoAnswerTimer(CCallInfo* pCallInfo, int iTimeMS = DEFAULT_TONE_INTERVAL, bool bPlayTone = true);
#ifdef IF_FEATURE_SPECIAL_AATONE
    void SetSpecialAutoAnswerTimer(CCallInfo* pCallInfo);
#endif
    bool IsAllTalkSessionHoldOrHeld();

private:
    CRefuseASyncProxy* m_pRefuseAsyncProxy;
};
