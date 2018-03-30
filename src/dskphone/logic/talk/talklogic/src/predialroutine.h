#ifndef __PREDIAL_ROUTINE_H__
#define __PREDIAL_ROUTINE_H__

// 预拨号过程（Idle界面按数字键进入）
class CPredialRoutine : public CDialRoutine
{
public:
 
    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_PREDIAL;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "PreDial";
    }

    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_PREDIAL;
    }
};

#endif //__PREDIAL_ROUTINE_H__
