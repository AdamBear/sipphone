#ifndef __LOCAL_CONF_SESSION_H__
#define __LOCAL_CONF_SESSION_H__

// 会议建立条件限制
#define CHECK_TYPE_G722 (1 << 0)
#define CHECK_TYPE_OPUS (1 << 1)
#define CHECK_TYPE_SRTP (1 << 2)

// CLocalConfSession:本地会议，有多个话路
class CLocalConfSession : public CBaseSession
{
public:
    CLocalConfSession();
    virtual ~CLocalConfSession();

    // Session创建之后的处理
    virtual void AfterSessionCreate();

    // 获取Session状态
    virtual SESSION_STATE GetSessionState() const
    {
        return SESSION_LOCALCONF;
    }

    // 处理ui发送过来的Action事件
    virtual bool OnActionEvent(DataUI2Logic* pActionData);

    // 定时器处理
    virtual bool OnTimer(UINT uTimerID);
    // SIP消息响应
    virtual bool OnSIPMsg(msgObject& msg);
    // SubSession退出时的处理
    virtual void OnSubSessionExit(CBaseSession* pSubSession);
    // SubSession状态改变的处理（如：会议中的某一路收到CallReplace消息）
    virtual void OnSubSessionChanged(CBaseSession* pSubSession);
    // Hold、Unhold Session
    virtual void SetHold(bool bHold = true, HOLD_TYPE eHoldType = HT_NORMAL,
                         bool bHoldByHost = false);
    // 判断session是否Hold状态
    virtual bool IsInHold();
    // 判断session是否至少有一个话路处于Held状态
    virtual bool IsInHeld();
    // 返回当前session是否处于Hold/held状态
    virtual bool IsInHoldOrHeld();
    // 判断session是否正在通话中
    virtual bool IsInTalking();

    // 是否允许被加入会议
    virtual bool IsAllowAddToConf()
    {
        return true;
    }

    // DTMF按键的处理(通话中)
    virtual bool SendDTMF(const yl::string& strDTMF, int interval = 0);

    // 退出Session，需要删除所有SubSession
    virtual void ExitSessionAndSub(bool bEndCall = true);

    CBaseSession* GetSub2CreateConf();
    // 重新设置sponsor session
    virtual void ResetSubSessonSponsor(CBaseSession* pSponsorSession);

    virtual bool SetFarHold(int iCallID, bool bFarHold);

protected:
    // 开启或关闭会议，pExitSession不为空表示退出的子话路引起的会议状态改变
    void ChangeConfState(bool bStart, CBaseSession* pExitSession = NULL);
    // 获取CallInfo
    CCallInfo* GetCallInfo(int iIndex);

    bool JoinIncomingCallToConf();

    bool SplitConfbyCallID(int iCallID);
    // 拆开会议
    void SplitConf();
    // 重新开启会议
    void ReStartConf(CBaseSession* pSubSession);
    // 判断session是否全部都处于Held状态
    bool IsAllInHeld();
    // 是否能够挂机后转接另外两路
    bool IsAllowTranOthersAfterExit();
    // Conf退出后将参与方连接
    bool TranAfterConfExit();

    // 踢出会议中某个话路
    bool RemoveConfParty(int iCallID);
    // 会议中某个话路远端静音
    virtual bool FarMute(int iCallID, bool bFarMute);
    //重新mute原来的farmute项
    void ReFarMute();

    void CheckHold();

    virtual CSingleSession* GetSession();

    virtual void SetConfManager(bool bConf);

    virtual bool IsConfManager()
    {
        return m_bInConfManager;
    }

private:
    // Hold防抖
    int               m_iTimerHoldDelay;
    // 是否进入会议管理
    bool              m_bInConfManager;

    bool              m_bLocalHold;

};

#endif //__LOCAL_CONF_SESSION_H__
