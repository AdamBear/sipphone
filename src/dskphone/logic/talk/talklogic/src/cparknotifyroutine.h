#ifndef __PARK_NOTIFY_ROUTINE_H__
#define __PARK_NOTIFY_ROUTINE_H__

#include "broadsoft/callpark/include/callparkcommon.h"

#ifdef IF_FEATURE_BROADSOFT_CALLPARK
// 通话中过程
class CParkNotifyRoutine : public CBaseRoutine
{
public:
    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_PARK_NOTIFY;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "ParkNotify";
    }

    CParkNotifyRoutine();
    virtual ~CParkNotifyRoutine();

    // 发消息通知对方结束通话
    virtual void EndCall();

    // 获取对应的Session状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_PARK_NOTIFY;
    }

protected:
    // 取回Park的电话
    void RetrievePark();
};
#endif // #if IF_FEATURE_BROADSOFT_CALLPARK
#endif //__TALK_ROUTINE_H__
