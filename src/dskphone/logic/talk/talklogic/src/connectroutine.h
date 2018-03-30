#ifndef __CONNECT_ROUTINE_H__
#define __CONNECT_ROUTINE_H__

// 呼出连接过程
class CConnectingRoutine : public CBaseConnectingRoutine
{
public:
    CConnectingRoutine();
    virtual ~CConnectingRoutine();
    // 创建Routine之后的处理
    virtual void AfterRoutineCreate();
    // Routine的定时器响应
    virtual bool OnTimer(UINT uTimerID);
    // Routine的SIP消息响应
    virtual bool OnSIPMsg(msgObject& msg);
    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState();
    virtual bool IsAllowAddToConf();

    virtual bool IsAllowSendDTMF()
    {
        return m_pCallInfo->m_bIsColorRingback;
    }

protected:
    // 处理sip 的hold状态
    void ProcessSIPHold(msgObject& msg);
    // 自动取消呼出
    void SetAutoCancelCallTimer();

    //重新开启通话
    virtual void StartTalkByIpvpParam(ipvp_speech_t* pSpeechInfo, CCallInfo* pCallInfo,
                                      bool bNeedHold = true);

private:
#if IF_BUG_40237
    bool        m_bTransferAfterRingBack;
#endif
};

#endif //__CONNECT_ROUTINE_H__
