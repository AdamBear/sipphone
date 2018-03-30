#ifndef __BETRANSFERRED_SESSION_H__
#define __BETRANSFERRED_SESSION_H__

// 被转移之后的通话，包含2个话路，一路旧的话路用于恢复以前的通话，一路新的呼出
class CBeTransferredSession : public CBaseSession
{
public:
    CBeTransferredSession();
    virtual ~CBeTransferredSession();

    // 定时器处理
    virtual bool OnTimer(UINT uTimerID);
    // SIP消息响应
    virtual bool OnSIPMsg(msgObject& msg);
    // SubSession退出时的处理
    virtual void OnSubSessionExit(CBaseSession* pSubSession);
    // Hold、Unhold Session
    virtual void SetHold(bool bHold = true, HOLD_TYPE eHoldType = HT_NORMAL,
                         bool bHoldByHost = false);

    // 获取Session状态
    virtual SESSION_STATE GetSessionState() const
    {
        return SESSION_BETRANSFERRED;
    }

    // 判断session是否正在通话中
    virtual bool IsInTalking();

    // 增加TalkSession
    void AddTalkSession(CSingleSession* pSession);
    // 增加RingbackSession
    void AddRingbackSession(CSingleSession* pSession);
    // 当前是通话还是回铃
    inline bool IsCurrentTalk();
    // 是否允许被加入会议
    virtual bool IsAllowAddToConf()
    {
        return false;
    }
    // 某个话路远端静音
    virtual bool FarMute(int iCallID, bool bFarMute);

    // 重新设置sponsor session
    virtual void ResetSubSessonSponsor(CBaseSession* pSponsorSession);
    virtual bool SetFarHold(int iCallID, bool bFarHold)
    {
        return true;
    }

    void ResetRelation();

    virtual CSingleSession* GetSession();

protected:
    // SIP消息响应(旧话路)
    bool OnSIPMsgTalkSession(msgObject& msg);
    // SIP消息响应(Ringback话路)
    bool OnSIPMsgRingbackSession(msgObject& msg);
    // 当前Session改变
    void ChangeCurrentSession(bool bCurrentTalkSession);

protected:
    // 旧的通话中的话路
    CSingleSession*   m_pTalkSession;
    // 新的被转后呼出的话路
    CSingleSession*   m_pRingbackSession;
    // 当前Session是否旧通话话路
    CSingleSession*   m_pCurrentSesssion;
};

// 当前是通话还是回铃
inline bool CBeTransferredSession::IsCurrentTalk()
{
    return m_pTalkSession != NULL && m_pCurrentSesssion == m_pTalkSession;
}

#endif //__BETRANSFERRED_SESSION_H__
