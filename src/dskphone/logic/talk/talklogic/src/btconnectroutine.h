#ifndef __BTCONNECT_ROUTINE_H__
#define __BTCONNECT_ROUTINE_H__

#ifdef IF_BT_SUPPORT_PHONE
class CBTConnectingRoutine : public CBaseConnectingRoutine
{
public:

    CBTConnectingRoutine();
    virtual ~CBTConnectingRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_BTCONNECTING;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "BTConnecting";
    }

    // 创建Routine之后的处理
    virtual void AfterRoutineCreate();
    // 销毁Routine之前的处理
    virtual void BeforeRoutineDestroy(bool bJumpToNextRoutine = true);
    // Routine的定时器响应
    virtual bool OnTimer(UINT uTimerID);
    // Routine的Mobile消息响应
    virtual bool OnSIPMsg(msgObject& msg);

    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_BTCONNECTING;
    }

    // 发消息通知对方结束通话
    virtual void EndCall();

protected:

    // 自动取消呼出
    void SetAutoCancelCallTimer();

    //建立通话
    bool Establish(int iHoldState);

    //处理HOLD消息
    void ProcessBTHold(int iHoldState);
};

#endif // IF_BT_SUPPORT_PHONE
#endif //__BTCONNECT_ROUTINE_H__
