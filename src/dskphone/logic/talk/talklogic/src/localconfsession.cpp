#include "talklogic_inc.h"

#define FOR_CONF_PARTY_LIST(it) \
LIST_CALL_SESSION& reflist = _SessionManager.GetSessionList();\
for (LIST_CALL_SESSION::iterator it = reflist.begin(); it != reflist.end(); ++it)

#define FOR_CONF_PARTY_LIST_EX(pSession) \
FOR_CONF_PARTY_LIST(it) \
{\
    CBaseSession* pSession = *it;\
    if (pSession->GetHostSession() != this)\
        continue;

#define FOR_CONF_PARTY_LIST_EX_END }

// CLocalConfSession:本地会议，有多个话路
CLocalConfSession::CLocalConfSession() : CLS_LOG_NAME(CBaseSession)
{
    m_iTimerHoldDelay = STOP_TIMER_VALUE;
    m_bInConfManager = false;
    m_bLocalHold = false;
}

CLocalConfSession::~CLocalConfSession()
{
    // 杀掉hold防抖定时器
    CancelTimer(m_iTimerHoldDelay);

    timerKillThreadTimer((UINT)&m_bLocalHold);
}

// Session创建之后的处理
void CLocalConfSession::AfterSessionCreate()
{
    CBaseSession::AfterSessionCreate();
    // unhold会议
    HoldSession(false);
}

// 处理ui发送过来的Action事件
bool CLocalConfSession::OnActionEvent(DataUI2Logic* pActionData)
{
    if (pActionData == NULL)
    {
        return false;
    }

    bool bHandled = true;

    switch (pActionData->eAction)
    {
    case TALK_ACTION_SPLIT:
        {
            if (2 == GetSubSessionSize())
            {
                SplitConf();
            }
            else
            {
                SplitConfbyCallID(pActionData->wActionParam);
            }
        }
        break;
    case TALK_ACTION_REMOVE_CONF_PARTY:
        {
            // 踢出会议中的某个话
            bHandled = RemoveConfParty(pActionData->wActionParam);
        }
        break;
    case TALK_ACTION_CONF_FAR_MUTE:
        {
            // 会议中某个参与方静音
            bHandled = FarMute(pActionData->wActionParam, (pActionData->lActionParam == 1));
        }
        break;
    case TALK_ACTION_CONF:
        {
            // 其他机型调用第一个话路处理Conf（一键会议）
            CBaseSession* pSession = GetSub2CreateConf();
            if (pSession)
            {
                return pSession->OnActionEvent(pActionData);
            }
        }
        break;
    case TALK_ACTION_CONF_FAR_HOLD:
        {
            bHandled = SetFarHold(pActionData->wActionParam, (pActionData->lActionParam == 1));
        }
        break;
    case TALK_ACTION_FAR_RESUME:
        {
            // 会议中某个参与方FarHold
            bHandled = SetFarHold(pActionData->wActionParam, false);
        }
        break;
    case TALK_ACTION_JOIN:
        {
            bHandled = JoinIncomingCallToConf();
        }
        break;
    case TALK_ACTION_SPLIT_ALL:
        {
            SplitConf();
        }
        break;
    default:
        bHandled = CBaseSession::OnActionEvent(pActionData);
        break;
    }

    return bHandled;
}

bool CLocalConfSession::JoinIncomingCallToConf()
{
    CSingleSession* pSession = _SessionManager.GetFirstIncomingCallSession();
    if (pSession == NULL)
    {
        return false;
    }

    ProcessIncomingCall(true);

    return _SessionManager.EstablishLocalConf(pSession);
}

// 定时器处理
bool CLocalConfSession::OnTimer(UINT uTimerID)
{
    if (uTimerID == (UINT)&m_iTimerHoldDelay)
    {
        CancelTimer(m_iTimerHoldDelay);
        return true;
    }
    else if (uTimerID == (UINT)&m_bLocalHold)
    {
        timerKillThreadTimer(uTimerID);
        CheckHold();
        return true;
    }

    // 遍历Session链表
    FOR_CONF_PARTY_LIST_EX(pSession)
    if (pSession->OnTimer(uTimerID))
    {
        return true;
    }
    FOR_CONF_PARTY_LIST_EX_END

    return false;
}

// SIP消息响应
bool CLocalConfSession::OnSIPMsg(msgObject& msg)
{
    CBaseSession* pSession = GetSessionByCallID(msg.lParam);
    if (pSession == NULL)
    {
        return false;
    }

    return pSession->OnSIPMsg(msg);
}

// SubSession退出时的处理
void CLocalConfSession::OnSubSessionExit(CBaseSession* pSubSession)
{
    if (pSubSession == NULL || pSubSession->GetHostSession() != this)
    {
        return;
    }

    //csta-syl
    CCallInfo* pDeleteCallInfo = pSubSession->GetCallInfoAsSub();

    if (pDeleteCallInfo != NULL)
    {
        pDeleteCallInfo->m_bIsSingleExitCof = true;
    }

    // 如果会议中有2路以上话路，则其中一路退出不影响会议
    if (GetSubSessionSize() > 2)
    {
        //三方会议的机型与会者进行transfer会在挂断时删除session过程中频繁调用，导致另外两方通话卡顿。
        //http://bugfree.yealink.com/Bug.php?BugID=116643
#ifdef IF_SUPPORT_FIVE_PARTY
        // 重新开启会议
        ReStartConf(pSubSession);
#endif
        // http://10.2.1.199/Bug.php?BugID=49348
        pSubSession->SetHostSession(NULL);
        return;
    }

    // 如果只有2路，其中一路退出，将退出会议，转变为单路通话
    // 找到不退出的那一路Session
    CBaseSession* pAnotherSession = GetAnotherSubSession(pSubSession);
    if (pAnotherSession != NULL)
    {
        CCallInfo* pReverseCallInfo = pAnotherSession->GetCallInfoAsSub();
        if (pReverseCallInfo != NULL)
        {
            // http://10.2.1.199/Bug.php?BugID=39259
            // http://10.3.5.199/Bug.php?BugID=62368
            if (pReverseCallInfo->IsFarMute())
            {
                pAnotherSession->FarMute(pReverseCallInfo->GetCallID(), false);
            }
            //CSTA
            pReverseCallInfo->m_bIsInConf = false;

            if (pReverseCallInfo->IsFarHold())
            {
                pReverseCallInfo->SetFarHold(false);
            }
        }
    }

    // 退出当前Session
    ExitSession();
}

void CLocalConfSession::ResetSubSessonSponsor(CBaseSession* pSponsorSession)
{
    if (NULL == pSponsorSession)
    {
        return;
    }

    FOR_CONF_PARTY_LIST(it)
    {
        CSingleSession* pSingleSession = (*it)->ToSingleSession();

        if (NULL == pSingleSession
                || pSingleSession->GetHostSession() != this
                || pSingleSession == pSponsorSession
                || pSponsorSession != pSingleSession->GetSponsorSession(SPONSOR_CONF))
        {
            continue;
        }

        CSingleSession* pFirstSub = NULL;

        //寻找一路子话路
        FOR_CONF_PARTY_LIST(itTmp)
        {
            CSingleSession* pSessionTmp = (*itTmp)->ToSingleSession();

            if (NULL == pSessionTmp
                    || pSessionTmp->GetHostSession() != this
                    || pSessionTmp == pSponsorSession
                    || pSessionTmp == pSingleSession)
            {
                continue;
            }

            pFirstSub = pSessionTmp;
        }

        if (NULL != pFirstSub)
        {
            pSingleSession->SetSponsorSession(SPONSOR_CONF, pFirstSub);
        }
    }
}

// SubSession状态改变的处理（如：会议中的某一路收到CallReplace消息）
void CLocalConfSession::OnSubSessionChanged(CBaseSession* pSubSession)
{
    if (pSubSession == NULL)
    {
        return;
    }

    // 子话路状态改变，如CallId改变，则重新开启会议
    ReStartConf(NULL);
}

void CLocalConfSession::ReStartConf(CBaseSession* pSubSession)
{
    // http://10.2.1.199/Bug.php?BugID=49348
    if (pSubSession != NULL && pSubSession->IsBeTransferSession())
    {
        return;
    }

    if (IsInHold())
    {
        // 如果会议是hold状态，则不重新开启会议
        return;
    }

    // 这两句，V50中无实际意义
    ChangeConfState(false, pSubSession);
    ChangeConfState(true, pSubSession);

    //盲转完成或者是calling方接起后还是要重新mute
    //http://bugfree.yealink.com/Bug.php?BugID=116204
    ReFarMute();
}

// 退出Session，需要删除所有SubSession
void CLocalConfSession::ExitSessionAndSub(bool bEndCall/* = true*/)
{
    // 退出会议时判断是否要tran另外两路，如果会议中的有一路不是SingleSession，则不允许挂机转接
    if (IsAllowTranOthersAfterExit())
    {
        // 根据转接是否成功来决定是否结束通话
        bEndCall = !TranAfterConfExit();
    }

    // 退出会议时将会议状态置为false,需在退出其他话路前处理
    ChangeConfState(false);

    ExitSubSession(bEndCall);

    // 退出当前Session
    ExitSession();
}

// 获取sub session去发起conf, bt line不可当作发起方
CBaseSession* CLocalConfSession::GetSub2CreateConf()
{
    FOR_CONF_PARTY_LIST_EX(pSession)
    if (!pSession->IsSingleSession()
            || pSession->GetCallInfoAsSub()->GetCallLineType() == CALL_LINE_BT)
    {
        continue;
    }

    return pSession;
    FOR_CONF_PARTY_LIST_EX_END
    return NULL;
}

// 开启或关闭会议
void CLocalConfSession::ChangeConfState(bool bStart, CBaseSession* pExitSession/* = NULL*/)
{
    YLVector<int> vec;

    // 由子话路退出导致会议状态改变，需要排除该子话路
    FOR_CONF_PARTY_LIST_EX(pSession)
    // 获取对应Session作为subSession时的CallInfo
    CCallInfo* pCallInfo = pSession->GetCallInfoAsSub();
    // 判断当前会议中两路的callid是否合法
    if (pSession == pExitSession || pCallInfo == NULL || pCallInfo->GetCallID() == -1)
    {
        continue;
    }

    if (bStart
            && pCallInfo->IsFarHold())
    {
        //Farhold的不用开启
        continue;
    }
    vec.push_back(pCallInfo->GetCallID());
    FOR_CONF_PARTY_LIST_EX_END;

    int iSize = vec.size();
    if (iSize == 0)
    {
        return;
    }

    YLVector<int>::iterator it = vec.begin();
    int* pCallIdList = new int[vec.size()];
    for (int i = 0; it != vec.end(); ++it)
    {
        pCallIdList[i++] = *it;
    }

    // 开启或停止会议
    if (bStart)
    {
        voice_StartConf(vec.size(), pCallIdList, false);
    }
    else
    {
        voice_StopConf(vec.size(), pCallIdList, false);
    }

    delete[] pCallIdList;
    pCallIdList = NULL;
    return;
}

// 获取CallInfo
CCallInfo* CLocalConfSession::GetCallInfo(int iIndex)
{
    CBaseSession* pSession = GetSubSessionByIndex(iIndex);
    return pSession ? pSession->GetCallInfoAsSub() : NULL;
}

bool CLocalConfSession::SplitConfbyCallID(int iCallID)
{
    CBaseSession* pSession = GetSessionByCallID(iCallID);
    if (pSession == NULL)
    {
        return false;
    }

    pSession->SetHostSession(NULL);

    CCallInfo* pCallInfo = pSession->GetCallInfoAsSub();
    if (pCallInfo == NULL)
    {
        return false;
    }

    if (pCallInfo->IsFarMute())
    {
        pSession->FarMute(pCallInfo->GetCallID(), false);
    }

    if (pCallInfo->IsFarHold())
    {
        pCallInfo->SetFarHold(false);
    }

    CSingleSession* pSingleSession = pSession->ToSingleSession();
    if (NULL != pSingleSession)
    {
        pSingleSession->HoldSession(true);
        pSingleSession->SetSponsorSession(SPONSOR_CONF, NULL);
    }

    return true;
}

// 拆开会议
void CLocalConfSession::SplitConf()
{
    // 关闭会议，且将两个子Session都hold
    ChangeConfState(false);

    // 拆开后hold
    HoldSession(true);

    // 遍历Session链表
    FOR_CONF_PARTY_LIST_EX(pSession)
    CCallInfo* pCallInfo = pSession->GetCallInfoAsSub();
    if (pCallInfo != NULL)
    {
        // http://10.2.1.199/Bug.php?BugID=39259
        // http://10.3.5.199/Bug.php?BugID=64972
        if (pCallInfo->IsFarMute())
        {
            pSession->FarMute(pCallInfo->GetCallID(), false);
        }

        if (pCallInfo->IsFarHold())
        {
            pCallInfo->SetFarHold(false);
        }
    }

    //解除sponor 关系
    CSingleSession* pSingleSession = pSession->ToSingleSession();
    if (NULL != pSingleSession
            && pSingleSession->GetSponsorSession(SPONSOR_CONF) != NULL)
    {
        pSingleSession->SetSponsorSession(SPONSOR_CONF, NULL);
    }
    FOR_CONF_PARTY_LIST_EX_END

    // 退出会议Session
    ExitSession();
}

// 获取hold状态
bool CLocalConfSession::IsInHold()
{
    return m_bLocalHold;
}

// 判断session是否至少有一个话路处于Held状态
bool CLocalConfSession::IsInHeld()
{
    // 会议参与方是否都处于held状态
    FOR_CONF_PARTY_LIST_EX(pSession)
    // 只要存在对方未hold的话路则认为held为false
    CCallInfo* pCallInfo = pSession->GetCallInfoAsSub();
    if (pCallInfo != NULL && pCallInfo->IsRemoteHold())
    {
        return true;
    }
    FOR_CONF_PARTY_LIST_EX_END

    return false;
}

// 判断session是否全部都处于Held状态
bool CLocalConfSession::IsAllInHeld()
{
    // 会议参与方是否都处于held状态
    FOR_CONF_PARTY_LIST_EX(pSession)
    // 只要存在对方未hold的话路则认为held为false
    CCallInfo* pCallInfo = pSession->GetCallInfoAsSub();
    if (pCallInfo != NULL && !pCallInfo->IsRemoteHold())
    {
        return false;
    }
    FOR_CONF_PARTY_LIST_EX_END

    return true;
}

bool CLocalConfSession::IsInHoldOrHeld()
{
    // 会议参与方任何一方hold，都认为处于held状态
    FOR_CONF_PARTY_LIST_EX(pSession)
    CCallInfo* pCallInfo = pSession->GetCallInfoAsSub();
    if (pCallInfo != NULL && pCallInfo->IsRemoteHold())
    {
        return TRUE;
    }
    FOR_CONF_PARTY_LIST_EX_END

    return IsInHold();
}

// Hold、Unhold Session
void CLocalConfSession::SetHold(bool bHold/* = true*/, HOLD_TYPE eHoldType/* = HT_NORMAL*/,
                                bool bHoldByHost/* = false*/)
{
    if (!bHold)
    {
        // 如果有设置了Hold操作定时器，则下一次的hold不再处理
        if (m_iTimerHoldDelay != STOP_TIMER_VALUE)
        {
            return;
        }

        // 对unhold防抖
        m_iTimerHoldDelay = HOLD_DELAY_TIME;
        SetTimerEx(m_iTimerHoldDelay);
    }

    // hold、unhold之后退出conf管理
    SetConfManager(false);

    SignalParams sigp(this);
    sigp.wParam.bValue = m_bLocalHold;
    sigp.lParam.bValue = bHold;

    m_bLocalHold = bHold;

    // 如果是hold,则要先split,unhold时,要先建议会议
    if (bHold)
    {
        ChangeConfState(false);
    }

    // 遍历Session链表
    FOR_CONF_PARTY_LIST_EX(pSession)
    CCallInfo* pCallInfo = pSession->GetCallInfoAsSub();
    if (pCallInfo != NULL)
    {
        if (pCallInfo->IsFarHold() && !bHold)
        {
            // Far Hold则不需要再解hold
            continue;
        }
    }
    pSession->HoldSession(bHold, eHoldType, true);
    FOR_CONF_PARTY_LIST_EX_END

    if (!bHold)
    {
        ChangeConfState(true);

        // 解hold后，要将farmute的通话重新farmute
        ReFarMute();
    }

    modtklsn_emit(TALK_SIG_SESSION_HOLD_CHANGE, sigp);
    // hold状态检测
    timerSetThreadTimer((UINT)&m_bLocalHold, 5 * 1000);
    return;
}

// 判断session是否正在通话中
bool CLocalConfSession::IsInTalking()
{
    // 找到第一路子话路
    CBaseSession* pFirstSubSession = GetFirstSub();
    if (pFirstSubSession != NULL)
    {
        // 根据第一路子话路的通话状态来判断
        CCallInfo* pCallInfo = pFirstSubSession->GetCallInfoAsSub();
        if (pCallInfo != NULL && pCallInfo->IsTalkStart())
        {
            return true;
        }
    }

    return false;
}

// DTMF按键的处理(通话中)
bool CLocalConfSession::SendDTMF(const yl::string& strDTMF, int interval/* = 0*/)
{
    bool bRet = false;
    FOR_CONF_PARTY_LIST_EX(pSession)
    bRet |= pSession->SendDTMF(strDTMF, interval);
    FOR_CONF_PARTY_LIST_EX_END
    return bRet;
}

// 是否能够挂机后转接另外两路
bool CLocalConfSession::IsAllowTranOthersAfterExit()
{
    if (configParser_GetConfigInt(kszTranAfterConfExitSwitch) == 0)
    {
        return false;
    }

    // 如果会议中的有一路不是SingleSession，则不允许挂机转接
    CBaseSession* pFirstSession = GetFirstSub();
    CBaseSession* pSecondSession = GetAnotherSubSession(pFirstSession);

    if (pFirstSession && pSecondSession && pFirstSession->IsSingleSession() && pSecondSession->IsSingleSession())
    {
#ifdef IF_SUPPORT_LINE_DEVICE
        return (SESSION_LINEDEVICE != pFirstSession->GetSessionState()
                && SESSION_LINEDEVICE != pSecondSession->GetSessionState());
#else
        return true;
#endif
    }

    return false;
}

// Conf退出后将参与方连接
bool CLocalConfSession::TranAfterConfExit()
{
    if (GetSubSessionSize() != 2)
    {
        return false;
    }

    CCallInfo* pCallInfoFrist = GetCallInfo(0);
    CCallInfo* pCallInfoSecd = GetCallInfo(1);
    if (pCallInfoFrist == NULL || pCallInfoSecd == NULL)
    {
        return false;
    }

    // 当会议中的两路由同一个账号发起时才能tran
    if (pCallInfoFrist->GetAccountID() != pCallInfoSecd->GetAccountID())
    {
        return false;
    }

    int iHoldDelay = configParser_GetConfigInt(kszHoldTranDelay);
    if (iHoldDelay != 0)
    {
        int index = 0;
        while (index < LOCAL_CONF_MAX_PARTY)
        {
            CBaseSession* pSubSession = GetSubSessionByIndex(index);
            if (pSubSession != NULL)
            {
                pSubSession->HoldSession(true);
                etlSleep(iHoldDelay);
            }

            ++index;
        }
    }

    if (pCallInfoFrist->IsLocalHold() && pCallInfoSecd->IsLocalHold())
    {
        // 如果此时是conf中hold状态，则延迟发送transfer另外两方的消息(SIP要求)
        etlSleep(100);
    }

    // 将第一路转移到第二路
    modVoip_RequestTransfer(pCallInfoFrist->GetAccountID(), pCallInfoFrist->GetCallID(),
                            pCallInfoSecd->GetCallID());

    // 为防止连续发消息造成会议转接方挂断,此处延时1秒
    // http://192.168.1.99/Bug.php?BugID=13315
    etlSleep(1000);
    // 将第一路转移到第二路，挂掉第一路
    modVoip_HangupCall(pCallInfoFrist->GetAccountID(), pCallInfoFrist->GetCallID());
    return true;
}

// 踢出会议中某个话路
bool CLocalConfSession::RemoveConfParty(int iCallID)
{
    CBaseSession* pSession = GetSessionByCallID(iCallID);
    if (pSession == NULL)
    {
        return false;
    }

    // 退出该路
    pSession->ExitSession(true);
    return true;
}

// 会议中某个话路远端静音
bool CLocalConfSession::FarMute(int iCallID, bool bFarMute)
{
    CBaseSession* pSession = GetSessionByCallID(iCallID);
    return pSession && pSession->FarMute(iCallID, bFarMute);
}

void CLocalConfSession::CheckHold()
{
    FOR_CONF_PARTY_LIST_EX(pSession)
    if (m_bLocalHold == pSession->IsInHold())
    {
        continue;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfoAsSub();
    if (pCallInfo == NULL
            || (pCallInfo->IsFarHold() && pCallInfo->IsLocalHold()))
    {
        //farhold时不需要再重新同步
        continue;
    }

    pSession->HoldSession(m_bLocalHold, HT_NORMAL, true);
    FOR_CONF_PARTY_LIST_EX_END

    return;
}

bool CLocalConfSession::SetFarHold(int iCallID, bool bFarHold)
{
    if (!bFarHold)
    {
        // 如果有设置了Hold操作定时器，则下一次的hold不再处理
        if (m_iTimerHoldDelay != STOP_TIMER_VALUE)
        {
            return true;
        }

        // 对unhold防抖
        m_iTimerHoldDelay = HOLD_DELAY_TIME;
        SetTimerEx(m_iTimerHoldDelay);
    }

    CBaseSession* pSession = GetSessionByCallID(iCallID);
    if (pSession == NULL)
    {
        return false;
    }

    if (!pSession->SetFarHold(iCallID, bFarHold))
    {
        return false;
    }

    if (!bFarHold)
    {
        //Farhold要重新开启会议
        //http://bugfree.yealink.com/Bug.php?BugID=116162
        ChangeConfState(true);

        //调用ChangeConfState(true)接口后必须ReFarmute
        ReFarMute();
    }

    return true;
}

//重新mute原来的farmute项
void CLocalConfSession::ReFarMute()
{
    FOR_CONF_PARTY_LIST_EX(pSession)
    CCallInfo* pCallInfo = pSession->GetCallInfoAsSub();
    if (pCallInfo && pCallInfo->IsFarMute())
    {
        _VoiceInterface.SetFarMute(pCallInfo->GetCallID(), pCallInfo->m_tVPMParam, true);
    }
    FOR_CONF_PARTY_LIST_EX_END
}

CSingleSession* CLocalConfSession::GetSession()
{
    int iSize = GetSubSessionSize();
    for (int i = 0; i < iSize; ++i)
    {
        CBaseSession* pSession = GetSubSessionByIndex(i);
        if (pSession != NULL)
        {
            return pSession->GetSession();
        }
    }

    return NULL;
}

void CLocalConfSession::SetConfManager(bool bConf)
{
    if (m_bInConfManager == bConf)
    {
        return;
    }

    m_bInConfManager = bConf;

    SignalParams sigp(this);
    sigp.wParam.iValue = TALK_CALL_CONF_MANAGER;
    sigp.lParam.bValue = bConf;
    modtklsn_emit(TALK_SIG_SESSION_INFO_CHANGED, sigp);
}
