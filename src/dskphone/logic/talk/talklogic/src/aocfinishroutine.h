#ifndef __AOC_FINISH_ROUTINE_H__
#define __AOC_FINISH_ROUTINE_H__

#include "baseroutine.h"

// 来电响铃过程
class CAocFinishRoutine : public CBaseRoutine
{
public:
    CAocFinishRoutine();
    virtual ~CAocFinishRoutine();

public:
    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_AOC_FINISH;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "AocFinish";
    }

    // 创建Routine之后的处理
    virtual void AfterRoutineCreate();
    // 销毁Routine之前的处理
    virtual void BeforeRoutineDestroy(bool bJumpToNextRoutine = true);

    // 发消息通知对方结束通话
    virtual void EndCall() {}

    // Routine的定时器响应
    virtual bool OnTimer(UINT uTimerID);

    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_AOC_FINISH;
    }

private:
    // 设置显示定时器
    void SetShowTimer();

private:
    int     m_iTimerShow; // 显示定时器

};

#endif //__AOC_FINISH_ROUTINE_H__
