#ifndef _AUTOREDIAL_ROUTINE_H__
#define _AUTOREDIAL_ROUTINE_H__

// 通话完成过程(通话失败、通话结束)
class CAutoRedialRoutine : public CFinishedRoutine
{
public:

    CAutoRedialRoutine();
    virtual ~CAutoRedialRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_AUTOREDIAL;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "AutoRedial";
    }

    // 创建Routine之后的处理
    virtual void AfterRoutineCreate();
    // Routine的定时器响应
    virtual bool OnTimer(UINT uTimerID);
    // 处理ui发送过来的Action事件
    virtual bool OnActionEvent(DataUI2Logic* pActionData);

    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_AUTO_REDIAL;
    }
};

#endif //_AUTOREDIAL_ROUTINE_H__
