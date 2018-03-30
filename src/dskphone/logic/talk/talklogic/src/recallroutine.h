#ifndef __RECALL_ROUTINE_H__
#define __RECALL_ROUTINE_H__

#include "ringroutine.h"

// Blind Transfer失败后的回拨
class CRecallRoutine : public CRingingRoutine
{
public:
    CRecallRoutine();
    virtual ~CRecallRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_RECALL;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "Recall";
    }

    // 创建Routine之后的处理 该接口需要保留，避免流程调用到ringroutine的接口
    virtual void AfterRoutineCreate()
    {
        CRingingRoutine::InitStateFlagData();
    }
    // Routine的SIP消息响应
    virtual bool OnSIPMsg(msgObject& msg);
    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_RECALL;
    }
    // 拒绝新来电
    virtual void RefuseIncomingCall(int iRefuseType = RT_NORMAL);
    // 接起新来电
    virtual bool AnswerIncomingCall(bool bSwitchChannel = true, bool bAudioOnly = true);
    // 发消息通知对方结束通话
    virtual void EndCall();
};

#endif //__RECALL_ROUTINE_H__
