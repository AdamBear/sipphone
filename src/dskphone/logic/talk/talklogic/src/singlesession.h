#ifndef __SINGLE_SESSION_H__
#define __SINGLE_SESSION_H__

class CSendDTMFASyncProxy;
// CSingleSession:只能有一个话路的Session
class CTalkRoutine;

class CSingleSession : public CBaseSession
{
public:
    CSingleSession(int iCallID = -1, bool bHide = false);
    virtual ~CSingleSession();

public:
    // 话路状态跳转
    virtual bool EnterRoutine(ROUTINE_STATE eNewState);
    // 处理ui发送过来的Action事件
    virtual bool OnActionEvent(DataUI2Logic* pActionData);
    // 退出当前Session
    virtual void ExitSession(bool bEndCall = false);
    // 定时器处理
    virtual bool OnTimer(UINT uTimerID) DECLARE_ATTR;
    // SIP消息响应
    virtual bool OnSIPMsg(msgObject& msg);
    // 根据Callid获取CallSession
    virtual CBaseSession* GetSessionByCallID(int iCallID);
    // SubSession退出时的处理
    virtual void OnSubSessionExit(CBaseSession* pSubSession) {}
    // 作为SubSession时传给Host的CallInfo(如会议中的BeTran)
    virtual CCallInfo* GetCallInfoAsSub();
    // session del之前的操作(析构函数之前)
    virtual void BeforeDelete();
    // Hold、Unhold Session
    virtual void SetHold(bool bHold = true, HOLD_TYPE eHoldType = HT_NORMAL,
                         bool bHoldByHost = false);
    // 判断是否通话session(conf/tran/talk等)
    virtual bool IsTalkSession();
    // 判断session是否正在通话中
    virtual bool IsInTalking();
    // 是否允许被加入会议
    virtual bool IsAllowAddToConf();
    // 某个话路远端静音
    virtual bool FarMute(int iCallID, bool bFarMute);

    // 获取Tran话路的发起session
    inline CSingleSession* GetSponsorSession(SPONSOR_TYPE eType) const;
    // 设置Tran话路的发起session
    inline void SetSponsorSession(SPONSOR_TYPE eType, CSingleSession* pSession);

    // 获取通话信息
    CCallInfo* GetCallInfo() const
    {
        return m_pCallInfo;
    }

    // transfer当前存在的话路
    void TranCurrentSession();
    // 取消Tran/conf Sponsor的关系(该路是发起者)
    void CancelSponsor(SPONSOR_TYPE eType = SPONSOR_ALL);
    // 将sponsor和follow关系转给另外一个
    void FwdSponsorOrFollow(SPONSOR_TYPE eType, CSingleSession* pSessionNew);
    // 如果当前话路是SponsorSession获取对应的FollowSession
    CSingleSession* GetFollowSession(SPONSOR_TYPE eType);
    // 是否由transfer发起，或该话路是其他话路的transponsor
    bool InTransfer();
    // 是否本地会议中
    bool InLocalConf();
    // 是否在会议新起一路过程中（还未建立会议）
    bool InPreConf();

    // 是否LocalConf中的第一个sub
    bool IsFirstSubInConf();
#if IF_FEATURE_PAGING
    // 是否是Multicast Page通话
    bool BeMulticastPage();
    // 是否是Multicast Page呼出通话
    bool BePageOut();
    // 获取Paging的优先级
    int GetMulticastPriority();
#endif
    // 获取Session状态
    virtual SESSION_STATE GetSessionState() const;
    // 将来电或者通话transfer或者forward给lpTarget
    bool Redirectcall(LPCSTR lpTarget);

    void ResetSponsor();

    virtual bool SetFarHold(int iCallID, bool bFarHold);

    //是否为网络会议
    bool IsNetConf();

    virtual CSingleSession* GetSession()
    {
        return this;
    }

    // 获取对应的Routine
    virtual CBaseRoutine* GetRoutine() const
    {
        return m_pCallRoutine;
    }

    CTalkRoutine* GetTalkRoutine() const;

    virtual bool SendDTMF(const yl::string& strDTMF, int interval = 0);

    virtual LPCSTR GetName();

protected:
    // 设置对应的FollowSession
    void SetFollowSession(SPONSOR_TYPE eType, CSingleSession* pSession);

protected:
    // session唯一对应的routine
    CBaseRoutine*           m_pCallRoutine;
    // 该Session是由另外一个Session Tran发起
    CSingleSession*         m_pTranSponsorSession;
    // 该Session是由另外一个Session conf发起
    CSingleSession*         m_pConfSponsorSession;
    // 通话信息维护
    CCallInfo*              m_pCallInfo;
    // DTMF send
    CSendDTMFASyncProxy*    m_pSendDTMFProxy;
};

// 获取该话路的发起session
inline CSingleSession* CSingleSession::GetSponsorSession(SPONSOR_TYPE eType) const
{
    switch (eType)
    {
    case SPONSOR_TRAN:
        return m_pTranSponsorSession;
    case SPONSOR_CONF:
        return m_pConfSponsorSession;
    case SPONSOR_ALL:
        return m_pConfSponsorSession ? m_pConfSponsorSession : m_pTranSponsorSession;
    default:
        break;
    }

    return NULL;
}

// 设置该话路的发起session
inline void CSingleSession::SetSponsorSession(SPONSOR_TYPE eType, CSingleSession* pSession)
{
    if (pSession == this)
    {
        return;
    }

    switch (eType)
    {
    case SPONSOR_TRAN:
        m_pTranSponsorSession = pSession;
        break;
    case SPONSOR_CONF:
        m_pConfSponsorSession = pSession;
        break;
    default:
        break;
    }
}

#endif //__SINGLE_SESSION_H__
