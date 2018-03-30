#ifndef __FINISHED_ROUTINE_H__
#define __FINISHED_ROUTINE_H__

// 通话完成过程(通话失败、通话结束)
class CFinishedRoutine : public CBaseFinishedRoutine
{
public:
    CFinishedRoutine();
    virtual ~CFinishedRoutine();

    // 创建Routine之后的处理
    virtual void AfterRoutineCreate();

    // Routine的定时器响应
    virtual bool OnTimer(UINT uTimerID);
    // Routine的SIP消息响应
    virtual bool OnSIPMsg(msgObject& msg);
    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_FINISHED;
    }
protected:
    TONE_TYPE m_eToneType;
};

#endif //__FINISHED_ROUTINE_H__
