#ifndef __BASEDIAL_ROUTINE_H__
#define __BASEDIAL_ROUTINE_H__

#include "talk/dialplan/include/moddialplan.h"

class CBaseDialRoutine : public CBaseRoutine
{
public:
    CBaseDialRoutine();
    virtual ~CBaseDialRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_DIALING;
    }

    // 名称
    virtual LPCSTR GetName() const
    {
        return "Dial";
    }

    // 创建Routine之后的处理
    virtual void AfterRoutineCreate();

    // 处理ui发送过来的Action事件
    virtual bool OnActionEvent(DataUI2Logic* pActionData);

protected:

    // 拨号界面按Redial重拨
    bool Redial();
};

#endif //__BASEDIAL_ROUTINE_H__
