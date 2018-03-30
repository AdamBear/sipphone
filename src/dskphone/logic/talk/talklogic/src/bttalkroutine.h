#ifndef __BTTALK_ROUTINE_H__
#define __BTTALK_ROUTINE_H__

#ifdef IF_BT_SUPPORT_PHONE
class CBTTalkingRoutine : public CBaseTalkRoutine
{
public:
    CBTTalkingRoutine();
    virtual ~CBTTalkingRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_BTTALKING;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "BTTalk";
    }

    // 创建Routine之后的处理
    virtual void AfterRoutineCreate();
    // 销毁Routine之前的处理
    virtual void BeforeRoutineDestroy(bool bJumpToNextRoutine = true);
    // Routine的定时器响应
    virtual bool OnTimer(UINT uTimerID) DECLARE_ATTR;
    // Routine的Mobile消息响应
    virtual bool OnSIPMsg(msgObject& msg);
    // 处理ui发送过来的Action事件
    virtual bool OnActionEvent(DataUI2Logic* pActionData);
    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_BTTALKING;
    }

    //结束通话
    virtual void EndCall();

protected:
    // hold/unhold当前话路
    bool HoldRoutine(bool bHold, HOLD_TYPE eHoldType = HT_NORMAL, bool bHoldByHost = false);
    //通话建立
    bool Establish(int iHoldState);
    //处理声音通道切换
    bool OnChannelLinkChange(bool bLinked);
    //处理HOLD消息
    void ProcessBTHold(int iHoldState);

    virtual bool IsAllowAddToConf()
    {
        return true;
    }

private:
    // 声音通道切换防抖
    int         m_iTimerChannelLinkDelay;
};

#endif // IF_BT_SUPPORT_PHONE
#endif //__BTTALK_ROUTINE_H__
