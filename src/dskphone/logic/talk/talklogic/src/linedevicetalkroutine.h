#ifndef _LINE_DEVICE_TALKROUTINE_H_
#define _LINE_DEVICE_TALKROUTINE_H_

#ifdef IF_SUPPORT_LINE_DEVICE
#include "talkroutine.h"

class CLineDeviceTalkRoutine : public CTalkRoutine
{
public:
    CLineDeviceTalkRoutine();
    virtual ~CLineDeviceTalkRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_LINE_DEVICE;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "LineDevice";
    }

    // 创建Routine之后的处理
    virtual void AfterRoutineCreate();

    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_LINEDEVICE;
    }

    // 处理ui发送过来的Action事件
    virtual bool OnActionEvent(DataUI2Logic* pActionData);

    // 开启通话
    virtual void StartTalk(bool bNeedHold = true);
    // 停止通话
    virtual void StopTalk();

protected:
    // hold/unhold当前话路
    virtual bool SetCallHold(bool bHold);

    // 重新开启声音
    void ResetVolume();

    // 重新选取账号
    void ReSelectAccount();
};

#endif // IF_SUPPORT_LINE_DEVICE
#endif // _LINE_DEVICE_TALKROUTINE_H_
