#ifndef __BTRING_ROUTINE_H__
#define __BTRING_ROUTINE_H__

#ifdef IF_BT_SUPPORT_PHONE
class CBTRingingRoutine : public CBaseRingRoutine
{
public:
    CBTRingingRoutine();
    virtual ~CBTRingingRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_BTRINGING;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "BTRing";
    }

    // 创建Routine之后的处理
    virtual void AfterRoutineCreate()
    {
        InitStateFlagData();
    }

    // Routine的Mobile消息响应
    virtual bool OnSIPMsg(msgObject& msg);
    // 处理ui发送过来的Action事件
    virtual bool OnActionEvent(DataUI2Logic* pActionData);
    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_BTRINGING;
    }
    // 发消息通知对方结束通话
    virtual void EndCall();
    // 拒绝新来电
    // | bSaveMisscall | [in] 是否保存呼叫记录。
    virtual void RefuseIncomingCall(int iRefuseType = RT_NORMAL);
    // 接起新来电，是否需要调整通道
    virtual bool AnswerIncomingCall(bool bSwitchChannel = true, bool bAudioOnly = true);

protected:
    //通话建立
    bool Established(int iHoldState);
    //处理HOLD消息
    bool ProcessBTHold(int iHoldState);
};

#endif // IF_BT_SUPPORT_PHONE
#endif //__BTRING_ROUTINE_H__
