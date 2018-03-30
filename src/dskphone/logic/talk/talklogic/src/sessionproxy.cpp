#include "sessionproxy.h"
#include "basesession.h"
#include "baseroutine.h"
#include "singlesession.h"
#include "sessionmanager.h"
#include "commonfunctions.h"
#include "modtalklogic.h"
#include "voip/modvoip.h"
#include "talklogicmanager.h"

CSessionProxy::CSessionProxy(int iSessionID) : m_pSession(_SessionManager.GetSession(iSessionID))
{
}

CSessionProxy::CSessionProxy(CBaseSession* p/* = NULL*/) : m_pSession(p)
{
}

CSessionProxy::CSessionProxy(CCallInfo* p)
{
    if (p != NULL)
    {
        m_pSession = _SessionManager.GetSession(p->GetSessionID());
    }
}

CSessionProxy::~CSessionProxy()
{
}

LPCSTR CSessionProxy::Name() const
{
    if (m_pSession == NULL)
    {
        return NULL;
    }

    return m_pSession->GetName();
}

CSessionProxy::operator int() const
{
    return m_pSession ? m_pSession->GetSessionID() : INVALID_HANDLE;
}

CSingleSession* CSessionProxy::ToSingleSession() const
{
    if (m_pSession != NULL)
    {
        return m_pSession->ToSingleSession();
    }

    return NULL;
}

CSessionProxy CSessionProxy::GetTopestHost() const
{
    if (m_pSession)
    {
        return m_pSession->GetTopestHostSession();
    }

    return CSessionProxy();
}

CSessionProxy CSessionProxy::GetHost() const
{
    if (m_pSession)
    {
        return m_pSession->GetHostSession();
    }

    return CSessionProxy();
}

CSessionProxy CSessionProxy::GetThisOrTopestHost() const
{
    if (m_pSession)
    {
        CBaseSession* host = m_pSession->GetTopestHostSession();
        return host ? host : m_pSession;
    }

    return CSessionProxy();
}

CSessionProxy CSessionProxy::GetConfSponsor() const
{
    if (IsSingle())
    {
        return ToSingleSession()->GetSponsorSession(SPONSOR_CONF);
    }

    return CSessionProxy();
}

CSessionProxy CSessionProxy::GetTranSponsor() const
{
    if (IsSingle())
    {
        return ToSingleSession()->GetSponsorSession(SPONSOR_TRAN);
    }

    return CSessionProxy();
}

CSessionProxy CSessionProxy::GetSponsor() const
{
    if (IsSingle())
    {
        return ToSingleSession()->GetSponsorSession(SPONSOR_ALL);
    }

    return CSessionProxy();
}

SESSION_STATE CSessionProxy::GetSessionState() const
{
    if (m_pSession)
    {
        return m_pSession->GetSessionState();
    }

    return SESSION_UNKNOWN;
}

ROUTINE_STATE CSessionProxy::GetRoutineState() const
{
    if (m_pSession == NULL)
    {
        return ROUTINE_UNKNOWN;
    }

    CBaseRoutine* pRoutine = m_pSession->GetRoutine();
    if (pRoutine != NULL)
    {
        return pRoutine->GetRoutineState();
    }

    return ROUTINE_UNKNOWN;
}

ROUTINE_STATE CSessionProxy::GetInheritsRoutine() const
{
    return Comm_GetInheritsRoutine(GetRoutineState());
}

CCallInfo* CSessionProxy::GetCallInfo() const
{
    if (m_pSession)
    {
        return m_pSession->GetCallInfoAsSub();
    }

    return NULL;
}

bool CSessionProxy::IsSingle() const
{
    return m_pSession && m_pSession->IsSingleSession();
}

bool CSessionProxy::IsLocalConf() const
{
    return m_pSession && m_pSession->IsLocalConfSession();
}

bool CSessionProxy::IsBeTransfer() const
{
    return m_pSession && m_pSession->IsBeTransferSession();
}

bool CSessionProxy::IsBla() const
{
    return IsSingle() && acc_IsBLANeedSeize(GetAccountID());
}

bool CSessionProxy::IsDraftBla() const
{
    return IsSingle() && acc_GetBLAType(GetAccountID()) == BLA_DRAFT_BLA;
}

bool CSessionProxy::IsSCA() const
{
    return IsSingle() && acc_GetBLAType(GetAccountID()) == BLA_BROADSOFT_SCA;
}

bool CSessionProxy::IsBtSession() const
{
    return IsSingle() && m_pSession->IsBtSession();
}

int CSessionProxy::GetAccountID() const
{
    CCallInfo* pCallInfo = GetCallInfo();
    if (pCallInfo)
    {
        return pCallInfo->GetAccountID();
    }

    return INVALID_HANDLE;
}

bool CSessionProxy::IsInConf() const
{
    return m_pSession && m_pSession->IsInLocalConf();
}

bool CSessionProxy::IsHold() const
{
    return m_pSession && m_pSession->IsInHold();
}

bool CSessionProxy::IsHeld() const
{
    return m_pSession && m_pSession->IsInHeld();
}

bool CSessionProxy::IsHide() const
{
    return m_pSession && m_pSession->IsHide();
}

bool CSessionProxy::IsInHoldOrHeld() const
{
    return m_pSession && m_pSession->IsInHoldOrHeld();
}

bool CSessionProxy::IsAllHeld() const
{
    return m_pSession && m_pSession->IsAllInHeld();
}

bool CSessionProxy::IsInTalking() const
{
    return m_pSession && m_pSession->IsInTalking();
}

bool CSessionProxy::IsRinging() const
{
    return IsInherits(ROUTINE_RINGING);
}

bool CSessionProxy::IsTalking() const
{
    // 多路通话一定是talking状态
    return IsInherits(ROUTINE_TALKING);
}

bool CSessionProxy::IsTalkSession() const
{
    return m_pSession && m_pSession->IsTalkSession();
}

bool CSessionProxy::IsConecting() const
{
    return IsInherits(ROUTINE_CONNECTING);
}

bool CSessionProxy::IsDialing() const
{
    return IsInherits(ROUTINE_DIALING);
}

bool CSessionProxy::IsFinished() const
{
    return IsInherits(ROUTINE_FINISHED);
}

bool CSessionProxy::IsBTTalk() const
{
#ifdef IF_BT_SUPPORT_PHONE
    CCallInfo* pCallInfo = GetCallInfo();
    return pCallInfo && pCallInfo->IsBtTalk();
#else
    return false;
#endif
}

int CSessionProxy::GetCallID() const
{
    CCallInfo* pCallInfo = GetCallInfo();
    if (pCallInfo)
    {
        pCallInfo->GetCallID();
    }

    return -1;
}

bool CSessionProxy::SendDTMF(const yl::string& strDTMF) const
{
    return m_pSession && m_pSession->SendDTMF(strDTMF);
}

bool CSessionProxy::EstablishLocalConf() const
{
    return _SessionManager.EstablishLocalConf(m_pSession);
}

bool CSessionProxy::ExitSession() const
{
    if (m_pSession == NULL)
    {
        return false;
    }

    m_pSession->ExitSessionAndSub();
    return true;
}

bool CSessionProxy::TransferForCallpark(yl::string& strNumber) const
{
    if (!IsInherits(ROUTINE_TALKING)
            || GetTalkState() != TS_TALK)
    {
        return false;
    }

    RemoveInvalidChar(strNumber);

    if (strNumber.empty())
    {
        return false;
    }

    CCallInfo* pCallInfo = GetCallInfo();
    if (NULL != pCallInfo)
    {
        if (configParser_GetConfigInt(kszHoldBeforeTransferCallpark) == 1)
        {
            HoldSession(true);
        }

        pCallInfo->SetTransfer(true);
        modVoip_RequestTransfer(GetAccountID(), GetCallID(), -1, strNumber.c_str());
        TALK_INFO("TransferForCallpark, strTarget = [%s]", strNumber.c_str());
    }

    return true;
}

void CSessionProxy::SetHide(bool bHide) const
{
    if (m_pSession)
    {
        m_pSession->SetHide(bHide);
    }
}

bool CSessionProxy::TranCurrentSession()
{
    CSingleSession* pSession = ToSingleSession();
    if (pSession)
    {
        pSession->TranCurrentSession();
        return true;
    }

    return false;
}

bool CSessionProxy::IsConfManager() const
{
    return m_pSession && m_pSession->IsConfManager();
}

void CSessionProxy::SetConfManager(bool bConf)
{
    if (m_pSession)
    {
        m_pSession->SetConfManager(bConf);
    }
}

TALK_STATE CSessionProxy::GetTalkState() const
{
    CCallInfo* pCallInfo = GetCallInfo();
    return pCallInfo ? pCallInfo->m_eTalkState : TS_UNKNOW;
}

TALK_CALLOUT_TYPE CSessionProxy::GetCallOutType() const
{
    CCallInfo* pCallInfo = GetCallInfo();
    return pCallInfo ? pCallInfo->m_eTalkCallOutType : TCT_UNKNOWN;
}

CALL_TYPE CSessionProxy::GetCallType() const
{
    CCallInfo* pCallInfo = GetCallInfo();
    return pCallInfo ? pCallInfo->m_eCallType : CT_UNKNOW;
}

CSessionProxy CSessionProxy::GetSubByIndex(int index) const
{
    if (m_pSession != NULL)
    {
        return m_pSession->GetSubSessionByIndex(index);
    }

    return CSessionProxy();
}

bool CSessionProxy::IsInherits(ROUTINE_STATE eRoutine) const
{
    return Comm_IsRoutineInherits(eRoutine, GetRoutineState());
}

bool CSessionProxy::IsInherits(SESSION_STATE eSession) const
{
    return Comm_IsSessionInherits(eSession, GetSessionState());
}

bool CSessionProxy::BeMulticastPage() const
{
    CSingleSession* pSession = ToSingleSession();
    return pSession && pSession->BeMulticastPage();
}

bool CSessionProxy::IsInPreTrans() const
{
    CSingleSession* pSession = ToSingleSession();
    return pSession && pSession->InTransfer();
}

bool CSessionProxy::IsInPreConf() const
{
    CSingleSession* pSession = ToSingleSession();
    return pSession && pSession->InPreConf();
}


bool CSessionProxy::IsAllOtherSessionHold() const
{
    return m_pSession && _SessionManager.IsAllOtherSessionHold(m_pSession->GetSessionID());
}

bool CSessionProxy::IsDestructing() const
{
    return m_pSession && m_pSession->m_bDestory;
}

void CSessionProxy::HoldSession(bool bHold, HOLD_TYPE eHoldType/* = HT_NORMAL*/) const
{
    if (m_pSession)
    {
        m_pSession->HoldSession(bHold, eHoldType);
    }
}

CSessionProxy CSessionProxy::GetAnOther() const
{
    if (m_pSession)
    {
        return m_pSession->GetAnotherSession();
    }

    return CSessionProxy();
}

CSessionProxy CSessionProxy::GetAnOtherSubSession(const CSessionProxy& other) const
{
    CBaseSession* pSession = _SessionManager.GetSession(other);
    if (m_pSession == NULL || pSession == NULL)
    {
        return CSessionProxy();
    }

    return m_pSession->GetAnotherSubSession(pSession);
}

CSessionProxy CSessionProxy::GetIncomeSession() const
{
    return _SessionManager.GetFirstIncomingCallSession();
}

int CSessionProxy::GetSubSize() const
{
    if (m_pSession)
    {
        return m_pSession->GetSubSessionSize();
    }

    return 0;
}

bool CSessionProxy::GetSubSession(YLVector<CSessionProxy>& vec) const
{
    if (m_pSession == NULL)
    {
        return false;
    }

    YLVector<CBaseSession*> vecSession;
    if (!m_pSession->GetSubSessionList(vecSession))
    {
        return false;
    }

    for (YLVector<CBaseSession*>::iterator it = vecSession.begin();
            it != vecSession.end(); ++it)
    {
        vec.push_back(*it);
    }

    return vec.size() > 0;
}

void CSessionProxy::SetConfSponsor(const CSessionProxy& proxy/* = CSessionProxy()*/) const
{
    if (IsSingle())
    {
        ToSingleSession()->SetSponsorSession(SPONSOR_CONF, proxy.ToSingleSession());
    }
}

void CSessionProxy::SetTranSponsor(const CSessionProxy& proxy/* = CSessionProxy()*/) const
{
    if (IsSingle())
    {
        ToSingleSession()->SetSponsorSession(SPONSOR_TRAN, proxy.ToSingleSession());
    }
}

bool CSessionProxy::CallOut(const yl::string& strNumber, const CallOut_Param& callParam) const
{
    if (!IsInherits(ROUTINE_DIALING))
    {
        return false;
    }

    CBaseRoutine* pRoutine = m_pSession->GetRoutine();
    if (pRoutine == NULL)
    {
        return false;
    }

    return pRoutine->CallOut(strNumber, "", callParam.eCallOutTrigger);
}

bool CSessionProxy::CallOut() const
{
    CSingleSession* pSession = ToSingleSession();
    if (pSession == NULL)
    {
        return false;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();
    return _TalklogicManager.ProcessCallOut(pSession, pCallInfo->m_tRemoteInfo.sSIPName.c_str(),
                                            pCallInfo->m_tRemoteInfo.sDisplayName.c_str()) != TALK_CALLOUT_SUCCESS;
}

bool CSessionProxy::Forward(const yl::string& strNumber) const
{
    return talklogic_ForwardIncomingCall(GetCallID(), strNumber);
}

bool CSessionProxy::BlindTransfer(const yl::string& strNumber) const
{
    return talklogic_BlindTransfer(GetCallID(), strNumber.c_str());
}

bool CSessionProxy::Transfer(const CSessionProxy& to) const
{
    if (m_pSession)
    {
        CBaseRoutine* pRoutine = m_pSession->GetRoutine();
        if (pRoutine != NULL)
        {
            return pRoutine->Transfer(_SessionManager.GetSingleSession(to));
        }
    }

    return false;
}

bool CSessionProxy::Answer() const
{
    return talklogic_AnswerCall(GetCallID());
}

bool CSessionProxy::Refuse() const
{
    return talklogic_RefuseCall(GetCallID());
}

bool CSessionProxy::StartTalk() const
{
    if (m_pSession == NULL)
    {
        return false;
    }

    CBaseRoutine* pRoutine = m_pSession->GetRoutine();
    if (pRoutine == NULL)
    {
        return false;
    }

    pRoutine->StartTalk();
    return true;
}

bool CSessionProxy::StopTalk() const
{
    if (m_pSession == NULL)
    {
        return false;
    }

    CBaseRoutine* pRoutine = m_pSession->GetRoutine();
    if (pRoutine == NULL)
    {
        return false;
    }

    pRoutine->StopTalk();
    return true;
}

bool operator==(int i, const CSessionProxy& other)
{
    return other.m_pSession == _SessionManager.GetSession(i);
}
