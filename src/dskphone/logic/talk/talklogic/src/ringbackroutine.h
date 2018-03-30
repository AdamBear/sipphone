#ifndef __RINGBACK_ROUTINE_H__
#define __RINGBACK_ROUTINE_H__

// 呼出回铃过程
class CRingbackRoutine : public CConnectingRoutine
{
public:
    CRingbackRoutine();
    virtual ~CRingbackRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_RINGBACK;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "Ringback";
    }

    // 创建Routine之后的处理
    virtual void AfterRoutineCreate();
    // 处理ui发送过来的Action事件
    virtual bool OnActionEvent(DataUI2Logic* pActionData);
    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState();
};

#endif //__RINGBACK_ROUTINE_H__
