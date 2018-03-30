#ifndef __BTFINISHED_ROUTINE_H__
#define __BTFINISHED_ROUTINE_H__

#ifdef IF_BT_SUPPORT_PHONE
class CBTFinishedRoutine : public CBaseFinishedRoutine
{
public:
    CBTFinishedRoutine();
    virtual ~CBTFinishedRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_BTFINISHED;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "BTFinished";
    }

    // 创建Routine之后的处理
    virtual void AfterRoutineCreate();

    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_BTFINISHED;
    }
};

#endif // IF_BT_SUPPORT_PHONE
#endif //__BTFINISHED_ROUTINE_H__
