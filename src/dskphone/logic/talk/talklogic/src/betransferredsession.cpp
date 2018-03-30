#include "talklogic_inc.h"

// 被转移之后的通话
CBeTransferredSession::CBeTransferredSession()
    : CLS_LOG_NAME(CBaseSession), m_pTalkSession(NULL), m_pRingbackSession(NULL), m_pCurrentSesssion(NULL)
{
}

CBeTransferredSession::~CBeTransferredSession()
{

}

// 定时器处理
bool CBeTransferredSession::OnTimer(UINT uTimerID)
{
    bool bHandled = false;
    if (m_pTalkSession != NULL)
    {
        bHandled = m_pTalkSession->OnTimer(uTimerID);
    }

    if (!bHandled && m_pRingbackSession != NULL)
    {
        bHandled = m_pRingbackSession->OnTimer(uTimerID);
    }

    return bHandled;
}

// SIP消息响应
bool CBeTransferredSession::OnSIPMsg(msgObject& msg)
{
    // 根据callid来判断要把sip消息分发到哪一个subsession
    if (GetSessionByCallID(msg.lParam) == m_pTalkSession)
    {
        return OnSIPMsgTalkSession(msg);
    }
    else if (GetSessionByCallID(msg.lParam) == m_pRingbackSession)
    {
        return OnSIPMsgRingbackSession(msg);
    }

    return false;
}

// SIP消息响应(旧话路)
bool CBeTransferredSession::OnSIPMsgTalkSession(msgObject& msg)
{
    bool bHandled = false;

    if (m_pTalkSession != NULL)
    {
        // betransferred之后不再处理被转移的消息
        if (msg.message == SIP_CALL_TRANSFERED_BY_REMOTE)
        {
            return false;
        }

        bHandled = m_pTalkSession->OnSIPMsg(msg);
        // warring 在上述接口中可能导致退出当前sesssion

        // 如果是对方hold的消息则要更改当前Session
        if (Comm_IsHoldMsg(msg)
                && bHandled && m_pTalkSession != NULL)
        {
            // 根据原通话中的话路是否被hold来决定当前Session
            ChangeCurrentSession(!m_pTalkSession->GetCallInfo()->IsRemoteHold());
        }
    }

    return bHandled;
}

// SIP消息响应(Ringback话路)
bool CBeTransferredSession::OnSIPMsgRingbackSession(msgObject& msg)
{
    bool bHandled = false;

    if (m_pRingbackSession != NULL)
    {
        if (msg.message == SIP_CALL_ESTABLISH
                // 被转后响彩铃，则需要把另外一路停掉(另外一路可能是held)，否则会导致被转后不响彩铃
                || (msg.message == SIP_CALL_RINGBACK
                    && NULL != msg.GetExtraData()
                    && ((sip_call_ringback_info_t*)msg.GetExtraData())->type == 1))
        {
            // 如果回铃的那一路通话建立，则需要先停掉原来的通话
            if (m_pTalkSession != NULL)
            {
                // 不给sip发hold，只停通话
                CBaseRoutine* pTalkRoutine = m_pTalkSession->GetRoutine();
                if (pTalkRoutine != NULL)
                {
                    pTalkRoutine->StopTalk();
                }
            }

            // http://10.2.1.199/Bug.php?BugID=18621
            ChangeCurrentSession(false);
        }

        bHandled = m_pRingbackSession->OnSIPMsg(msg);
    }

    return bHandled;
}

// SubSession退出时的处理
void CBeTransferredSession::OnSubSessionExit(CBaseSession* pSubSession)
{
    if (pSubSession == NULL || pSubSession->GetHostSession() != this)
    {
        return;
    }

    // SubSession退出，如果当前Session没有Host，则将另外一路SubSession置为非隐藏
    // 并根据当前Session是否焦点来设置SubSession的焦点，同时退出当前Session
    if (pSubSession == m_pTalkSession)
    {
        // 如果TalkSession有sponsor或follow，则其退出后将此关系转给ringbacksession，bugid：15445
        m_pTalkSession->FwdSponsorOrFollow(SPONSOR_TRAN, m_pRingbackSession);
    }

    // 退出当前Session
    ExitSession();
}

// 当前Session改变
void CBeTransferredSession::ChangeCurrentSession(bool bCurrentTalkSession)
{
    if (IsCurrentTalk() == bCurrentTalkSession)
    {
        return;
    }

    m_pCurrentSesssion = bCurrentTalkSession ? m_pTalkSession : m_pRingbackSession;

    SignalParams sigp(this);
    sigp.wParam.iValue = m_pCurrentSesssion ? m_pCurrentSesssion->GetSessionID() : INVALID_SESSION_ID;
    sigp.lParam.bValue = bCurrentTalkSession;
    modtklsn_emit(TALK_SIG_SUB_SESSION_ACTIVED, sigp);
}

// Hold、Unhold Session
void CBeTransferredSession::SetHold(bool bHold/* = true*/, HOLD_TYPE eHoldType/* = HT_NORMAL*/,
                                    bool bHoldByHost/* = false*/)
{
    CCallInfo* pCallInfoTalk = NULL;
    CCallInfo* pCallInfoRingback = NULL;

    if (m_pTalkSession != NULL)
    {
        pCallInfoTalk = m_pTalkSession->GetCallInfo();
        m_pTalkSession->HoldSession(bHold, eHoldType, true);
    }

    // ringback不处理hold事件，但是要记录hold状态
    if (m_pRingbackSession != NULL)
    {
        pCallInfoRingback = m_pRingbackSession->GetCallInfo();
        if (pCallInfoRingback != NULL && pCallInfoTalk != NULL)
        {
            pCallInfoRingback->SetHold(pCallInfoTalk->IsLocalHold());
        }
    }
}

// 判断session是否正在通话中
bool CBeTransferredSession::IsInTalking()
{
    CCallInfo* pCallInfo = GetCallInfoAsSub();
    return pCallInfo && pCallInfo->IsTalkStart() && IsCurrentTalk();
}

// 某个话路远端静音
bool CBeTransferredSession::FarMute(int iCallID, bool bFarMute)
{
    return IsCurrentTalk() && m_pTalkSession->FarMute(iCallID, bFarMute);
}

// 增加TalkSession
void CBeTransferredSession::AddTalkSession(CSingleSession* pSession)
{
    if (NULL != m_pTalkSession
            && m_pTalkSession != pSession
            && m_pTalkSession != m_pRingbackSession)
    {
        m_pTalkSession->SetHostSession(NULL);
    }

    m_pTalkSession = pSession;
    pSession->SetHostSession(this);
    m_pCurrentSesssion = m_pTalkSession;
}

// 增加RingbackSession
void CBeTransferredSession::AddRingbackSession(CSingleSession* pSession)
{
    if (NULL != m_pRingbackSession
            && m_pRingbackSession != pSession
            && m_pRingbackSession != m_pTalkSession)
    {
        m_pRingbackSession->SetHostSession(NULL);
    }

    m_pRingbackSession = pSession;
    pSession->SetHostSession(this);
}

void CBeTransferredSession::ResetSubSessonSponsor(CBaseSession* pSponsorSession)
{
    if (pSponsorSession == NULL)
    {
        return;
    }

    // 退出TalkSession要重新建立SPONSOR
    // http://192.168.1.99/Bug.php?BugID=34368
    if (m_pTalkSession != NULL && pSponsorSession == m_pTalkSession)
    {
        CSingleSession* pSponsorConf = m_pTalkSession->GetSponsorSession(SPONSOR_CONF);
        if (pSponsorConf != NULL)
        {
            TALK_INFO("CBeTransferredSession::ResetSubSessonSponsor");
            m_pRingbackSession->SetSponsorSession(SPONSOR_CONF, pSponsorConf);
        }
    }

}

void CBeTransferredSession::ResetRelation()
{
    if (m_pTalkSession == NULL)
    {
        return;
    }

    CCallInfo* pCallInfo = m_pTalkSession->GetCallInfo();
    if (pCallInfo != NULL)
    {
        pCallInfo->SetRelationInfo(CR_NONE);
    }
}

CSingleSession* CBeTransferredSession::GetSession()
{
    return m_pCurrentSesssion;
}
