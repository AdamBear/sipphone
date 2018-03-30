#ifndef __BT_PREDIAL_ROUTINE_H__
#define __BT_PREDIAL_ROUTINE_H__

#ifdef IF_BT_SUPPORT_PHONE
// 预拨号过程（Idle界面按数字键进入）
class CBTPredialRoutine : public CBTDialRoutine
{
public:

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_BTPREDIAL;
    }

    // 名称
    virtual LPCSTR GetName() const
    {
        return "BTPreDial";
    }

    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_PREDIAL;
    }
};

#endif // IF_BT_SUPPORT_PHONE
#endif // __BT_PREDIAL_ROUTINE_H__
