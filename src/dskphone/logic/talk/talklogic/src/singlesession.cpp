#include "talklogic_inc.h"

// CSingleSession:只能有一个话路的Session
CSingleSession::CSingleSession(int iCallID/* = -1*/, bool bHide/* = false*/)
    : CBaseSession(NULL), m_pCallRoutine(NULL), m_pTranSponsorSession(NULL),
      m_pConfSponsorSession(NULL), m_pCallInfo(NULL), m_pSendDTMFProxy(NULL)
{
    m_pCallInfo = new CCallInfo(this, iCallID);
    m_pSendDTMFProxy = new CSendDTMFASyncProxy(m_iSessionID);
    m_bHide = bHide;
}

CSingleSession::~CSingleSession()
{
    // Session删除掉后，需要移除对应的线路
    _VoiceInterface.RemoveSession(m_pCallInfo->GetCallID());

    delete m_pCallInfo;
    m_pCallInfo = NULL;
}

// session del之前的操作(析构函数之前)
void CSingleSession::BeforeDelete()
{
    // 在通知HostSession前将通话停掉
    SignalParams sigp(this);
    sigp.wParam.iValue = m_pCallRoutine->GetRoutineState();
    sigp.lParam.iValue = ROUTINE_UNKNOWN;
    modtklsn_emit(TALK_SIG_SESSION_PRE_ROUTINE_CHANGED, sigp);

    m_pCallRoutine->StopTalkBeforeDelete();

#ifdef IF_SUPPORT_BLUETOOTH
    // 蓝牙新需求，不处于拨号状态，说明有呼出，有呼出就需要调蓝牙的call out,有call out一定要有end call
    if (ROUTINE_DIALING != GetRoutineState())
    {
        Bluetooth_EndCall();
    }
#endif // IF_SUPPORT_BLUETOOTH

    // 退出前先解除sposor关系 http://10.3.5.199/Bug.php?BugID=61430
    SetSponsorSession(SPONSOR_TRAN, NULL);
    SetSponsorSession(SPONSOR_CONF, NULL);

    //调用基类的删除操作，清除对应UI
    CBaseSession::BeforeDelete();

#if IF_BUG_27560
    if (actURI_IsHideTalkInfo())
    {
        // 重置隐藏信息
        actURI_SetHideTalkInfo(false);
    }
#endif

    // 发送通话结束的消息
    etl_NotifyApp(FALSE, TALK_MSG_SESSION_DESTORY, GetSessionID(), m_pCallInfo->GetAccountID());

    // 退出Routine前的处理
    m_pCallRoutine->BeforeRoutineDestroy(false);

    m_pCallRoutine->deleteLater();

    // 先将处理示例析构，析构掉自身维护的数据，对外数据采用延时析构
    delete m_pSendDTMFProxy;
    m_pSendDTMFProxy = NULL;
}

// 话路状态跳转
bool CSingleSession::EnterRoutine(ROUTINE_STATE eNewState)
{
    if (m_bDestory)
    {
        return false;
    }

    SignalParams sigp(this);
    sigp.wParam.iValue = m_pCallRoutine ? m_pCallRoutine->GetRoutineState() : ROUTINE_UNKNOWN;
    sigp.lParam.iValue = eNewState;
    sigp.tParam.bValue = m_pCallRoutine == NULL;

    // 先退出当前的Routine
    if (m_pCallRoutine != NULL)
    {
        TALK_INFO("EnterRoutine [%s] leave", GetName());
        m_pCallRoutine->BeforeRoutineDestroy(true);
        m_pCallRoutine->deleteLater();
        m_pCallRoutine = NULL;
    }

    // 进入相应的Routine
    m_pCallRoutine = GetRoutineByState(eNewState);
    if (m_pCallRoutine == NULL)
    {
        return false;
    }

    // 将当前Session和新的Routine关联
    m_pCallRoutine->SetSession(this, m_pCallInfo);

    modtklsn_emit(TALK_SIG_SESSION_PRE_ROUTINE_CHANGED, sigp);

    TALK_INFO("EnterRoutine [%s] start", GetName());
    // 调用进入新的Routine后的处理函数
    m_pCallRoutine->AfterRoutineCreate();
    TALK_INFO("EnterRoutine [%s] end", GetName());

    modtklsn_emit(TALK_SIG_SESSION_ROUTINE_CHANGED, sigp);
    return true;
}

// 处理ui发送过来的Action事件
bool CSingleSession::OnActionEvent(DataUI2Logic* pActionData)
{
    if (pActionData == NULL || m_pCallRoutine == NULL || m_pCallInfo == NULL)
    {
        return false;
    }

#ifdef IF_FEATURE_PSTN
    // 当前通话不是sip来电 http://10.2.1.199/Bug.php?BugID=127378
    if (SESSION_RINGBACK != GetSessionState()
            && m_pCallInfo->GetCallLineType() != CALL_LINE_SIP)
    {
        CSingleSession* pIncomingSession = _SessionManager.GetFirstIncomingCallSession();
        CCallInfo* pIncomingCallInfo = pIncomingSession ? pIncomingSession->GetCallInfo() : NULL;
        if (pIncomingCallInfo
                && pIncomingCallInfo->GetCallLineType() == CALL_LINE_PSTN
                && pActionData->eAction == TALK_ACTION_REJECT)
        {
            // PSTN通话不支持拒接来电。注意需要判断来电的类型，而不是当前通话的类型（因为有可能是PSTN通话中拒接SIP来电）。
            return false;
        }
    }

    if (m_pCallInfo->GetCallLineType() == CALL_LINE_PSTN)
    {
        if (pActionData->eAction == TALK_ACTION_FWD_TO
                || pActionData->eAction == TALK_ACTION_TRAN
                || pActionData->eAction == TALK_ACTION_TRAN_TO)
        {
            // 过滤PSTN通话不支持的功能
            return false;
        }
    }
#endif // IF_FEATURE_PSTN

    // 交给routine处理
    if (m_pCallRoutine->OnActionEvent(pActionData))
    {
        return true;
    }

    return CBaseSession::OnActionEvent(pActionData);
}

// 退出当前Session
void CSingleSession::ExitSession(bool bEndCall/* = false*/)
{
    // 退出Session前的操作，不同Routine之间的切换不会调用
    if (m_pCallRoutine != NULL)
    {
        // 退出通话先，发消息告诉对方挂断
        if (bEndCall)
        {
            m_pCallRoutine->EndCall();
        }
    }

    CSingleSession* pTranSponsorSession = NULL;
    CSingleSession* pConfSponsorSession = NULL;

    // transfer退出后,是否自动resume原来的通话 http://10.2.1.199/Bug.php?BugID=21683
    if (configParser_GetConfigInt(kszResumeAfterTran) != 0)
    {
        pTranSponsorSession = GetSponsorSession(SPONSOR_TRAN);
        pConfSponsorSession = GetSponsorSession(SPONSOR_CONF);
    }

    // 如果当前Session是其他Session的SponsorSession，则取消这种关系
    ResetSponsor();

    // 从Session链表中删除
    _SessionManager.DeleteSession(this);

    if (NULL != pTranSponsorSession
            && !pTranSponsorSession->IsHide())
    {
        pTranSponsorSession->HoldSession(false);
    }

    if (NULL != pConfSponsorSession
            && !pConfSponsorSession->IsHide())
    {
        pConfSponsorSession->HoldSession(false);
    }

    return;
}

// 定时器处理
bool CSingleSession::OnTimer(UINT uTimerID)
{
    if (m_pCallRoutine != NULL
            && m_pCallRoutine->OnTimer(uTimerID))
    {
        return true;
    }

    return CBaseSession::OnTimer(uTimerID);
}

// SIP消息响应
bool CSingleSession::OnSIPMsg(msgObject& msg)
{
    return m_pCallRoutine && m_pCallRoutine->OnSIPMsg(msg);
}

// 根据Callid获取CallSession
CBaseSession* CSingleSession::GetSessionByCallID(int iCallID)
{
    if (m_pCallInfo->GetCallID() == iCallID)
    {
        return this;
    }

    return NULL;
}

// 作为SubSession时传给Host的CallInfo(如会议中的BeTran)
CCallInfo* CSingleSession::GetCallInfoAsSub()
{
    return m_pCallInfo;
}

void CSingleSession::ResetSponsor()
{
    //先更新会议中的sponsor关系
    CBaseSession* pSession = GetHostSession();
    if (NULL != pSession)
    {
        pSession->ResetSubSessonSponsor(this);
    }

    CancelSponsor();
}

// 取消Tran/conf Sponsor的关系(该路是发起者)
void CSingleSession::CancelSponsor(SPONSOR_TYPE eType/* = SPONSOR_ALL*/)
{
    if (eType == SPONSOR_ALL)
    {
        // 取消所有的Sponsor关系
        CancelSponsor(SPONSOR_TRAN);
        CancelSponsor(SPONSOR_CONF);
    }
    else
    {
        CSingleSession* pFollowSession = GetFollowSession(eType);
        // 该Session未发起tran则返回false
        while (pFollowSession != NULL)
        {
            // 重置子Session的tran发起者
            pFollowSession->SetSponsorSession(eType, NULL);
            // 循环取消所有的follow session
            pFollowSession = GetFollowSession(eType);
        }
    }
}

// 将某个session的sponsor和follow关系转给另外一个
void CSingleSession::FwdSponsorOrFollow(SPONSOR_TYPE eType, CSingleSession* pSessionNew)
{
    if (pSessionNew == NULL)
    {
        return;
    }

    // bugid：15445
    pSessionNew->SetSponsorSession(eType, GetSponsorSession(eType));
    pSessionNew->SetFollowSession(eType, this);
}

// 如果当前话路是SponsorSession获取对应的FollowSession
CSingleSession* CSingleSession::GetFollowSession(SPONSOR_TYPE eType)
{
    // 遍历sessionlist找到该session发起的子Session
    FOR_SESSION_LIST_EXPORT(it)
    {
        CSingleSession* pTempSession = (*it)->ToSingleSession();
        if (pTempSession != NULL
                && pTempSession->GetSponsorSession(eType) == this)
        {
            return pTempSession;
        }
    }

    return NULL;
}

// 设置对应的FollowSession
void CSingleSession::SetFollowSession(SPONSOR_TYPE eType, CSingleSession* pSession)
{
    if (pSession == NULL || pSession == this)
    {
        return;
    }

    pSession->SetSponsorSession(eType, this);
}

SESSION_STATE CSingleSession::GetSessionState() const
{
    if (m_pCallRoutine != NULL)
    {
        return m_pCallRoutine->GetHostSessionState();
    }

    return SESSION_UNKNOWN;
}

// transfer当前存在的话路
void CSingleSession::TranCurrentSession()
{
    // 如果没有Sponsor或Follow,则不处理
    if (!InTransfer())
    {
        return;
    }

    CSingleSession* pSponsor = NULL;
    CSingleSession* pFollow = NULL;

    // 找出sponsor和follow session
    if (m_pTranSponsorSession != NULL)
    {
        pFollow = this;
        pSponsor = m_pTranSponsorSession;
    }
    else
    {
        pSponsor = this;
        pFollow = GetFollowSession(SPONSOR_TRAN);
    }

    if (pSponsor == NULL || pFollow == NULL)
    {
        return;
    }

    talklogic_Transfer(pSponsor->GetCallInfo()->GetCallID(), pFollow->GetCallInfo()->GetCallID());
}

// 是否由transfer发起，或该话路是其他话路的transponsor
bool CSingleSession::InTransfer()
{
    // 由其他话路transfer发起，或该话路是其他话路的transponsor，则返回true
    return (GetSponsorSession(SPONSOR_TRAN) != NULL
            || GetFollowSession(SPONSOR_TRAN) != NULL);
}

// 是否本地会议中
bool CSingleSession::InLocalConf()
{
    // 递延到最上层的主session
    CBaseSession* pSession = GetTopestHostSession();
    return pSession && pSession->IsLocalConfSession();
}

// 是否在会议新起一路过程中（还未建立会议）
bool CSingleSession::InPreConf()
{
    // 由其他话路Conf发起，或该话路是其他话路的Confsponsor，则返回true
    return (GetSponsorSession(SPONSOR_CONF) != NULL
            || GetFollowSession(SPONSOR_CONF) != NULL);
}

// Hold、Unhold Session
void CSingleSession::SetHold(bool bHold/* = true*/, HOLD_TYPE eHoldType/* = HT_NORMAL*/,
                             bool bHoldByHost/* = false*/)
{
    if (m_pCallRoutine)
    {
        m_pCallRoutine->HoldRoutine(bHold, eHoldType, bHoldByHost);
    }
}

// 判断是否通话session(conf/tran/talk等)
bool CSingleSession::IsTalkSession()
{
    return dynamic_cast<CBaseTalkRoutine*>(m_pCallRoutine) != NULL;
}

// 判断session是否正在通话中
bool CSingleSession::IsInTalking()
{
    return (IsTalkSession() && m_pCallInfo->IsTalkStart());
}

// 是否LocalConf中的第一个sub
bool CSingleSession::IsFirstSubInConf()
{
    return m_pHostSession
           && m_pHostSession->IsLocalConfSession()
           && m_pHostSession->GetSubSessionByIndex(0) == this;
}

#if IF_FEATURE_PAGING
// 获取Paging的优先级
int CSingleSession::GetMulticastPriority()
{
    return m_pCallInfo->GetCallID();
}
#endif

#if IF_FEATURE_PAGING
// 是否是Multicast Page通话
bool CSingleSession::BeMulticastPage()
{
    return m_pCallInfo->BeMulticastPage();
}

// 是否是Multicast Page呼出通话
bool CSingleSession::BePageOut()
{
    return (CT_MULTICAST_OUT == m_pCallInfo->m_eCallType);
}
#endif

// 将来电或者通话transfer或者forward给lpTarget
bool CSingleSession::Redirectcall(LPCSTR lpTarget)
{
    return m_pCallRoutine && m_pCallRoutine->Redirectcall(lpTarget);
}

bool CSingleSession::IsAllowAddToConf()
{
    return m_pCallRoutine && m_pCallRoutine->IsAllowAddToConf();
}

// 某个话路远端静音
bool CSingleSession::FarMute(int iCallID, bool bFarMute)
{
    if (!IsTalkSession())
    {
        return false;
    }

    if (m_pCallInfo->IsFarMute() == bFarMute)
    {
        return false;
    }

    m_pCallInfo->SetFarMute(bFarMute);

    // 通话中才设置，否则只记录标识
    if (m_pCallInfo->IsTalkStart())
    {
        // http://10.2.1.199/Bug.php?BugID=86877 T49外机型，在hold与resume之间不能设置farmute
        _VoiceInterface.SetFarMute(m_pCallInfo->GetCallID(), m_pCallInfo->m_tVPMParam, bFarMute);
    }

    return true;
}

bool CSingleSession::SetFarHold(int iCallID, bool bFarHold)
{
    if (!IsTalkSession())
    {
        return false;
    }

    HoldSession(bFarHold);
    m_pCallInfo->SetFarHold(bFarHold);

    return true;
}

//是否为网络会议
bool CSingleSession::IsNetConf()
{
    if (m_pCallRoutine == NULL)
    {
        return false;
    }

    ROUTINE_STATE eRoutine = GetRoutineState();
    return eRoutine == ROUTINE_NETWORKCONF || eRoutine == ROUTINE_NETWORKCONF_CONNECT;
}

CTalkRoutine* CSingleSession::GetTalkRoutine() const
{
    if (m_pCallRoutine == NULL
            || !m_pCallRoutine->IsInherits(ROUTINE_TALKING))
    {
        return NULL;
    }

    return (CTalkRoutine*)m_pCallRoutine;
}

bool CSingleSession::SendDTMF(const yl::string& strDTMF, int interval/* = 0*/)
{
    if (m_pCallRoutine == NULL || m_pSendDTMFProxy == NULL || !m_pCallRoutine->IsAllowSendDTMF())
    {
        return false;
    }

    return m_pSendDTMFProxy->Send(strDTMF, interval);
}

LPCSTR CSingleSession::GetName()
{
    if (m_pCallRoutine == NULL)
    {
        return m_szName;
    }

    return m_pCallRoutine->GetName();
}

