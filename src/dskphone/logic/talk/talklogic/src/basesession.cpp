#include "talklogic_inc.h"

// 只有两路Session时获取另外一路
CBaseSession* CBaseSession::GetAnotherSession() const
{
    if (_SessionManager.GetSessionListSize(false) != 2)
    {
        return NULL;
    }

    FOR_SESSION_LIST_EXPORT(it)
    {
        CBaseSession* pSession = *it;
        if (pSession != this)
        {
            return pSession;
        }
    }

    return NULL;
}

int CBaseSession::GetSubSessionSize() const
{
    int size = 0;
    FOR_SESSION_LIST_EXPORT(it)
    {
        if ((*it)->GetHostSession() == this)
        {
            ++size;
        }
    }

    return size;
}

CBaseSession* CBaseSession::GetAnotherSubSession(CBaseSession* pSubSession) const
{
    if (pSubSession != NULL && GetSubSessionSize() == 2)
    {
        for (int i = 0; i < 2; ++i)
        {
            CBaseSession* pSession = GetSubSessionByIndex(i);
            if (pSession != pSubSession)
            {
                return pSession;
            }
        }
    }

    return NULL;
}

CBaseSession* CBaseSession::GetSubSessionByIndex(int index) const
{
    int size = 0;
    FOR_SESSION_LIST_EXPORT(it)
    {
        CBaseSession* pSession = *it;
        if (pSession->GetHostSession() == this)
        {
            if (size++ == index)
            {
                return pSession;
            }
        }
    }

    TALK_ERR("GetSubSessionByIndex [%d][%d]", m_iSessionID, index);
    return NULL;
}

bool CBaseSession::IsInHold()
{
    CSingleSession* pSession = GetSession();
    return pSession && pSession->GetCallInfo()->IsLocalHold();
}

bool CBaseSession::IsInHeld()
{
    CSingleSession* pSession = GetSession();
    return pSession && pSession->GetCallInfo()->IsRemoteHold();
}

void CBaseSession::FwdHostSession(CBaseSession* pSession/* = NULL*/)
{
    FOR_SESSION_LIST_EXPORT(it)
    {
        if ((*it)->GetHostSession() == this)
        {
            (*it)->SetHostSession(pSession);
        }
    }
}

CBaseSession* CBaseSession::GetSessionByCallID(int iCallID)
{
    FOR_SESSION_LIST_EXPORT(it)
    {
        CBaseSession* pSession = *it;
        if (pSession->GetHostSession() == this
                && pSession->GetSessionByCallID(iCallID) != NULL)
        {
            return pSession;
        }
    }

    return NULL;
}

// Session的基类
CBaseSession::CBaseSession(LPCSTR name) : m_pHostSession(NULL), m_bHide(false), m_szName(name), m_bDestory(false)
{
    m_iSessionID = _SessionManager.GenSessionID();
}

CBaseSession::~CBaseSession()
{
}

// SubSession状态改变的处理（如：会议中的某一路收到CallReplace消息）
void CBaseSession::OnSubSessionChanged(CBaseSession* pSubSession)
{
    // 向上传给自己的Host
    if (m_pHostSession != NULL)
    {
        m_pHostSession->OnSubSessionChanged(pSubSession);
    }
}

// 获取该session的最顶层父session
CBaseSession* CBaseSession::GetTopestHostSession() const
{
    CBaseSession* pTempSession = GetHostSession();
    CBaseSession* pTopestSession = pTempSession;
    while (NULL != pTempSession)
    {
        pTopestSession = pTempSession;
        pTempSession = pTempSession->GetHostSession();
    }

    return pTopestSession;
}

CBaseSession* CBaseSession::GetThisOrTopestHostSession() const
{
    CBaseSession* pSession = GetTopestHostSession();
    return pSession ? pSession : (CBaseSession*)this;
}

// session del之前的操作(析构函数之前)
void CBaseSession::BeforeDelete()
{
    // http://10.2.1.199/Bug.php?BugID=49348
    // 会议中被转一路中的退出后,需要再上报给会议,重新开启会议
    while (m_pHostSession != NULL)
    {
        m_pHostSession->OnSubSessionExit(this);
    }

    modtklsn_emit(TALK_SIG_SESSION_PRE_DELETE, this);
}

// 退出Session，需要删除所有SubSession
void CBaseSession::ExitSessionAndSub(bool bEndCall/* = true*/)
{
    ExitSubSession(bEndCall);

    // 退出当前Session
    ExitSession(bEndCall);
}

void CBaseSession::ExitSubSession(bool bEndCall /*= true*/)
{
    SetDestory();

    FOR_SESSION_LIST_EXPORT(it)
    {
        CBaseSession* pSession = *it;

        if (pSession->GetHostSession() == this)
        {
            pSession->SetHostSession(NULL);

#ifdef IF_SUPPORT_LINE_DEVICE
            if (SESSION_LINEDEVICE == pSession->GetSessionState() && IsLocalConfSession())
            {
                continue;
            }
#endif
            pSession->ExitSessionAndSub(bEndCall);
            return ExitSubSession(bEndCall);
        }
    }
}

void CBaseSession::ExitSession(bool bEndCall /*= false*/)
{
    FwdHostSession(GetHostSession());

    _SessionManager.DeleteSession(this);
}

bool CBaseSession::IsInHoldOrHeld()
{
    return (IsInHold() || IsInHeld());
}

bool CBaseSession::OnTimer(UINT uTimerID)
{
    return false;
}

bool CBaseSession::SendDTMF(const yl::string& strDTMF, int interval/* = 0*/)
{
    CSingleSession* pSession = GetSession();
    return pSession && pSession->SendDTMF(strDTMF, interval);
}

CCallInfo* CBaseSession::GetCallInfoAsSub()
{
    CSingleSession* pSession = GetSession();
    if (pSession == NULL)
    {
        return NULL;
    }

    return pSession->GetCallInfo();
}

bool CBaseSession::IsInherits(ROUTINE_STATE eState)
{
    CBaseRoutine* pRoutine = GetRoutine();
    if (pRoutine == NULL)
    {
        return ROUTINE_UNKNOWN;
    }

    return pRoutine->IsInherits(eState);
}

ROUTINE_STATE CBaseSession::GetRoutineState()
{
    CBaseRoutine* pRoutine = GetRoutine();
    if (pRoutine == NULL)
    {
        return ROUTINE_UNKNOWN;
    }

    return pRoutine->GetRoutineState();
}

CSingleSession* CBaseSession::ToSingleSession() const
{
    return dynamic_cast<CSingleSession*>(const_cast<CBaseSession*>(this));
}

CBeTransferredSession* CBaseSession::ToBeTransferredSession() const
{
    return dynamic_cast<CBeTransferredSession*>(const_cast<CBaseSession*>(this));
}

CLocalConfSession* CBaseSession::ToLocalConfSession() const
{
    return dynamic_cast<CLocalConfSession*>(const_cast<CBaseSession*>(this));
}

bool CBaseSession::GetSubSessionList(YLVector<CBaseSession*>& vec) const
{
    FOR_SESSION_LIST_EXPORT(it)
    {
        if ((*it)->GetHostSession() == this)
        {
            vec.push_back(*it);
        }
    }

    return vec.size() > 0;
}

void CBaseSession::HoldSession(bool bHold /*= true*/, HOLD_TYPE eHoldType /*= HT_NORMAL*/, bool bHoldByHost /*= false*/)
{
    SetHold(bHold, eHoldType, bHoldByHost);
}

void CBaseSession::SetHostSession(CBaseSession* pSession)
{
    if (m_pHostSession == pSession)
    {
        return;
    }

    /* 设置为非空Host，当前正在退出，或者Host为正在退出，则跳过设置 */
    if (pSession != NULL && (m_bDestory || pSession->m_bDestory))
    {
        return;
    }

    SignalParams sigp(this);
    sigp.wParam.iValue = m_pHostSession ? m_pHostSession->GetSessionID() : INVALID_SESSION_ID;
    sigp.lParam.iValue = pSession ? pSession->GetSessionID() : INVALID_SESSION_ID;

    m_pHostSession = pSession;
    modtklsn_emit(TALK_SIG_SESSION_HOST_CHANGE, sigp);

    // 如果Host为空则取消隐藏，否则隐藏
    SetHide(pSession != NULL);
}

// 设置隐藏状态
void CBaseSession::SetHide(bool bHide/* = true*/)
{
    if (m_bHide == bHide || m_bDestory)
    {
        return;
    }

    SignalParams sigp(this);
    sigp.wParam.bValue = m_bHide;
    sigp.lParam.bValue = bHide;

    m_bHide = bHide;

    _SessionManager.OnHideChanged(this);

    modtklsn_emit(TALK_SIG_SESSION_HIDE_CHANGE, sigp);
    // 需要通知通话隐藏，外部可以更新状态
    etl_NotifyApp(false, TALK_MSG_TALK_HIDE, m_iSessionID, bHide ? 1 : 0);
}

void CBaseSession::AfterSessionCreate()
{
    modtklsn_emit(TALK_SIG_SESSION_AFTER_CREATE, this);
}

CSingleSession* CBaseSession::CreateNewDial(ROUTINE_STATE eRoutine)
{
    // 保留账号信息
    CCallInfo* pCallInfo = GetCallInfoAsSub();
    if (pCallInfo == NULL)
    {
        return NULL;
    }

    if (modtkflt_emit(TALK_FLT_ENTER_DIAL, this))
    {
        return NULL;
    }

#ifdef IF_BT_SUPPORT_PHONE
    if (ACCOUNTID_OF_IPDIRECT_CALL == acc_GetUsableAccount(false, ACCOUNT_TYPE_ALL)
            && Bluetooth_IsMobileConnected())
    {
        return _BTMobileManager.EnterDialUI();
    }
#endif

    // 新建Session
    CSingleSession* pSession = _SessionManager.AllocSession();
    if (pSession == NULL)
    {
        return NULL;
    }

    CCallInfo* pInfo = pSession->GetCallInfo();
    pInfo->SetAccountID(pCallInfo->GetAccountID());
    pInfo->SetCallLineType(pCallInfo->GetCallLineType());

    _TalklogicManager.TryEnterRoutine(pSession, eRoutine);
    return pSession;
}

bool CBaseSession::OnActionEvent(DataUI2Logic* pActionData)
{
    if (pActionData == NULL)
    {
        return false;
    }

    switch (pActionData->eAction)
    {
    case TALK_ACTION_HOLD:
        {
            HoldSession(true);
        }
        break;
    case TALK_ACTION_RESUME:
        {
            HoldSession(false);
        }
        break;
    case TALK_ACTION_HOLD_SWITCH:
    case TALK_ACTION_HOLD_PUBLIC:
    case TALK_ACTION_HOLD_PRIVATE:
        {
            HoldSession(!IsInHold());
        }
        break;
    case TALK_ACTION_CANCEL:
        {
            ExitSessionAndSub(true);
        }
        break;
    case TALK_ACTION_DTMF:
        {
            yl::string str = GetDigitKeyString(pActionData->wActionParam);
            SendDTMF(str);
        }
        break;
    default:
        return false;
    }

    return true;
}

bool CBaseSession::ProcessIncomingCall(bool bAnswer, int iCode/* = 0*/)
{
    CSingleSession* pSession = _SessionManager.GetFirstIncomingCallSession();
    if (pSession == NULL)
    {
        return false;
    }

    CBaseRoutine* pRoutine = pSession->GetRoutine();
    if (pRoutine == NULL)
    {
        return false;
    }

    return pRoutine->ProcessIncomingCall(bAnswer, iCode);
}

void CBaseSession::SetConfManager(bool bConf)
{
    if (IsInherits(ROUTINE_TALKING))
    {
        GetCallInfoAsSub()->SetConfManager(bConf);
    }
}

bool CBaseSession::IsConfManager()
{
    return IsInherits(ROUTINE_TALKING) && GetCallInfoAsSub()->IsConfManager();
}

bool CBaseSession::IsBtSession()
{
#ifdef IF_BT_SUPPORT_PHONE
    CCallInfo* pCallInfo = GetCallInfoAsSub();
    return pCallInfo && pCallInfo->IsBtTalk();
#else
    return false;
#endif
}

void CBaseSession::SetDestory(bool bDestory /*= true*/)
{
    if (m_bDestory == bDestory)
    {
        return;
    }

    m_bDestory = bDestory;
}
