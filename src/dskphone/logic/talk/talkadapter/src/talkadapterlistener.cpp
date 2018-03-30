#include "talkadapter_inc.h"
#include "talklogic/src/autoredialmanager.h"
#include "talklogic/src/multicastpage.h"
#include "application/include/modapplication.h"


CTalkAdapterListener::CTalkAdapterListener()
    : LSN_CLS_LOG_NAME
{
    REG_SYNC_SIGNAL(TALK_FLT_SESSION_CREATE, CTalkAdapterListener::TryCreateSession);

    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CTalkAdapterListener::OnRoutineChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HIDE_CHANGE, CTalkAdapterListener::OnSessionHide);

    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ANSWER, CTalkAdapterListener::BeforeAnswerCall);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ANSWER, CTalkAdapterListener::OnAnswerCall);

    REG_SYNC_SIGNAL(TALK_SIG_SUB_SESSION_ACTIVED, CTalkAdapterListener::OnSubSessionActive);

    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ADD_TO_NET_CONF, CTalkAdapterListener::OnAddSessionNetConf);
    REG_SYNC_SIGNAL(TALK_SIG_RECV_PAGE, CTalkAdapterListener::OnPageReceived);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_START_TALK, CTalkAdapterListener::OnStartTalk);

    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_CREATE, CTalkAdapterListener::OnSessionCreate);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CTalkAdapterListener::OnSessionPreDelete);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_AFTER_DELETE, CTalkAdapterListener::OnSessionAftDelete);

    REG_SYNC_SIGNAL(TALK_SIG_BEFORE_CALLOUT, CTalkAdapterListener::BeforeCallout);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HOLD_CHANGE, CTalkAdapterListener::OnHoldChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HOST_CHANGE, CTalkAdapterListener::OnHostChange);

    REG_SYNC_SIGNAL(TALK_SIG_SESSION_AFTER_CREATE, CTalkAdapterListener::ResetFocusSession);
}

void CTalkAdapterListener::Push(int sub, int host)
{
    if (sub == INVALID_SESSION_ID
            || host == INVALID_SESSION_ID)
    {
        return;
    }

    int iSize = m_vec.size();
    /* 先清空上一次操作 */
    if (iSize >= 3)
    {
        m_vec.clear();
    }

    if (iSize == 0)
    {
        m_vec.push_back(host);
    }

    m_vec.push_back(sub);
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::TryCreateSession)
{
    if (sigp.wParam.iValue != SESSION_UNKNOWN || sigp.iSessionID == INVALID_SESSION_ID)
    {
        return false;
    }

    if (talklogic_IsMaxSessionNum())
    {
        SIG_REPLY(true);
    }

    return false;
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::OnRoutineChange)
{
    CSessionStateProxy proxy(sigp.iSessionID);
    ROUTINE_STATE eOldRoutine = (ROUTINE_STATE)sigp.wParam.iValue;
    ROUTINE_STATE eNewRoutine = (ROUTINE_STATE)sigp.lParam.iValue;

    bool bSetAsFocus = false;

    /* 被隐藏不处理 */
    if (proxy.IsHide())
    {
        // do nothing
    }
    /* 只有一路一定是焦点 */
    else if (talklogic_GetSessionNum() == 1)
    {
        bSetAsFocus = true;
    }
    /* 拨号强制设置为焦点 */
    else if (Comm_IsRoutineInherits(ROUTINE_DIALING_CLS, eNewRoutine))
    {
        // 退出占用通话资源的话路
        talklogic_ExitSession(ROUTINE_FINISHED_CLS);
        //_SessionManager.ExitSession(ROUTINE_AOC_FINISH);

        bool bHoldOthers = true;

        CSessionProxy sponsor = proxy.GetConfSponsor();
        if (sponsor)
        {
            CCallInfo* pCallInfo = proxy.GetCallInfo();
            if (pCallInfo == NULL)
            {
                return false;
            }

#if IF_BUG_27560
            if (actURI_IsHideTalkInfo())
            {
                pCallInfo->m_bHideInfo = true;
            }
#endif

#if IF_BUG_31081
            if (configParser_GetConfigInt(kszConfUseNewLinekey) == 0)
            {
                pCallInfo->m_bNeedBindDsskey = false;
            }
#endif
            if (configParser_GetConfigInt(kszConfWithOnePress) == 1
                    && pCallInfo->GetConfType() == CT_LOCAL)
            {
                pCallInfo->m_bConferenceDirect = true;
                bHoldOthers = false;
            }
        }


        if (bHoldOthers)
        {
            // hold其它话路
            talklogic_HoldAllSession(sigp.iSessionID);
        }

        bSetAsFocus = true;
    }
    /* 来电时全部为Hold，或者讲点为park notify */
    else if (Comm_IsRoutineInherits(ROUTINE_RINGING_CLS, eNewRoutine, ROUTINE_RECALL))
    {
        // 退出占用通话资源的话路
        talklogic_ExitSession(ROUTINE_FINISHED_CLS);
        //_SessionManager.ExitSession(ROUTINE_AOC_FINISH);

        // 只有一路 直接设置为焦点
        if (proxy.IsAllOtherSessionHold()
                && configParser_GetConfigInt(kszIncomingCallIgnor) == 0)
        {
            // bsft:认证要求hold中来电，要为焦点 http://10.2.1.199/Bug.php?BugID=82454 http://10.2.1.199/Bug.php?BugID=69842
            bSetAsFocus = true;
        }
        else
        {
            //如果当前焦点的通话为ParkNotify，则设置新来电为焦点
            CSessionStateProxy focus;
            bSetAsFocus = (focus.GetSessionState() == SESSION_PARK_NOTIFY);

            // 来电是否打断用户呼出操作
            if (focus.IsInherits(ROUTINE_DIALING_CLS))
            {
                // Priority == 0, 来电优先级最低, 不打断当前操作
                if (configParser_GetConfigInt(kszIncomingCallPriorityWhenDial) != 0)
                {
                    AdapterBox_ReturnToIdle();
                }
                // Priority == 2, 来电优先级最高, 来电设置为焦点, 暂时不做
            }
        }
    }
    /* 呼出话路不作为子话路时应该设为焦点 */
    else if (Comm_IsRoutineInherits(ROUTINE_CONNECTING_CLS, eNewRoutine))
    {
        bSetAsFocus = true;

        // 非拨号界面再做其他处理
        // 创建新的话路前将其他话路Hold
        // 一键建立会议不需要hold其他路
        CCallInfo* pCallInfo = proxy.GetCallInfo();
        if (pCallInfo != NULL && !pCallInfo->m_bConfWithOnePress)
        {
            talklogic_HoldAllSession();
        }
    }
    /* 切到通话状态，最复杂 */
    else if (Comm_IsRoutineInherits(ROUTINE_TALKING_CLS, eNewRoutine))
    {
        /* 直接进通话的如LineDevice，Paging等 */
        if (eOldRoutine == ROUTINE_UNKNOWN)
        {
#if IF_BUG_30963
            if (proxy.GetCallType() == CT_MULTICAST_IN)
            {
                bool bHoldPage = false;
                if (configParser_GetConfigInt(kszHoldPageInMultiSession) != 0)
                {
                    bool bMultiSession = talklogic_GetSessionNum(); //是否为多路通话
                    if (bMultiSession)
                    {
                        CSessionStateProxy focus;
                        CCallInfo* pCallInfo = GetCallInfo();
                        if (focus.IsSingle())
                        {
                            bool bFocusPage = focus.BeMulticastPage(); //当前话路是否为page
                            bool bRemoteHold = pCallInfo->m_bRemoteHold;
                            bool bStarTalk = pCallInfo->IsTalkStart();
                            bHoldPage = bMultiSession && !bFocusPage && bStarTalk && !bRemoteHold; //是否hold来电page
                        }
                    }
                }

                bSetAsFocus = !bHoldPage;
            }
#else
            bSetAsFocus = true;
#endif

            if (bSetAsFocus)
            {
                talklogic_HoldAllSession(sigp.iSessionID);
            }
        }
        /* 由来电切到通话需要设置为焦点 */
        else if (Comm_IsRoutineInherits(ROUTINE_RINGING_CLS, eOldRoutine))
        {
#ifdef IF_FEATURE_ENTRANCE_GUARD
            if (application_IsAutoAnswer(sigp.iSessionID))
            {
                // 门禁来电自动接起时不需要设置为焦点
                bSetAsFocus = false;
            }
            else
#endif //IF_FEATURE_ENTRANCE_GUARD
            {
                bSetAsFocus = !proxy.GetHost();
            }
        }
        /* 呼出进通话需要看当前焦点状态，以决定是否抢占 */
        else if (Comm_IsRoutineInherits(ROUTINE_CONNECTING_CLS, eOldRoutine))
        {
            CSessionStateProxy focus;
            CSessionStateProxy host = proxy.GetHost();
            /* 奇葩逻辑：在会议中，不处于Hold时，将Host设置为焦点 */
            if (host.IsLocalConf())
            {
                if (!host.IsHold())
                {
                    _TalkAdapter.SetFocusSession(host);
                }
            }
            /* 焦点为来电还未接起，当前呼出已经进通话，当前设为焦点 */
            else if (focus.IsInherits(ROUTINE_RINGING_CLS)
#if IF_FEATURE_PAGING
                     || focus.BeMulticastPage()
#endif
                    )
            {
                bSetAsFocus = true;
            }
        }
    }
    /* 第一次进routine(非来电)，需要设置为焦点 */
    else if (eOldRoutine == ROUTINE_UNKNOWN)
    {
        bSetAsFocus = true;
    }

    if (bSetAsFocus)
    {
        // 停止重拨
        _AutoRedialManager.StopAutoRedial();
        _TalkAdapter.SetFocusSession(sigp.iSessionID);
    }
    /* 单路通话由通话状态切到完成状态，则焦点的网络会议需要激活 */
    else if (!proxy.GetHost()
             && Comm_IsRoutineInherits(ROUTINE_TALKING_CLS, eOldRoutine)
             && Comm_IsRoutineInherits(ROUTINE_FINISHED_CLS, eNewRoutine))
    {
        CSessionStateProxy focus;
        if (focus.IsInherits(ROUTINE_NETWORKCONF))
        {
            focus.HoldSession(false);
        }
    }

    if (!sigp.tParam.bValue)
    {
        /* 切换Routine之后触发焦点事件以切换声道 */
        CSessionStateProxy proxy(sigp.iSessionID);
        SignalParams sigp1(sigp);
        sigp1.wParam.bValue = proxy.IsFocusSession();
        modtklsn_emit(TALK_SIG_SESSION_ACTIVED, sigp1);
    }

    return true;
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::OnSessionHide)
{
    /* 从隐藏状态到非隐藏，且只有当前一路的情况下需要设置为焦点 */
    if (sigp.wParam.bValue && !sigp.lParam.bValue)
    {
        if (talklogic_GetSessionNum() == 1)
        {
            return _TalkAdapter.SetFocusSession(sigp.iSessionID);
        }
    }
    /*else if (sigp.lParam.bValue)
    {
        _TalkAdapter.Remove(sigp.iSessionID);
    }*/

    return false;
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::OnSessionPreDelete)
{
    CSessionStateProxy proxy(sigp.iSessionID);

    if (!proxy.IsSingle())
    {
        return CALL_HANDLER(ResetFocusSession);
    }
    /* 当前是焦点才需要处理*/
    else if (proxy.IsFocusSession())
    {
        _TalkAdapter.ResetFocusedByLastSession(sigp.iSessionID);
    }
    /* 退出非焦点单路通话，焦点的网络会议需要激活 */
    else if (proxy.IsInherits(ROUTINE_TALKING))
    {
        CSessionStateProxy focus;
        if (focus.IsInherits(ROUTINE_NETWORKCONF))
        {
            focus.HoldSession(false);
        }
    }

    return true;
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::OnSessionAftDelete)
{
    /* 清空UI适配层数据 */
    _TalkUIData.OnSessionDestory(sigp.iSessionID);
    _TalkAdapter.OnSessionDestory(sigp.iSessionID);
    return true;
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::OnAnswerCall)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (proxy.GetHost() || pCallInfo == NULL || pCallInfo->m_bJoinConf)
    {
        return false;
    }

    talklogic_HoldAllSession(sigp.iSessionID);
    return true;
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::BeforeAnswerCall)
{
    CSessionStateProxy proxy;
    // 退出位于拨号和呼出的界面
    if (proxy.IsInherits(ROUTINE_DIALING_CLS))
    {
        return proxy.ExitSession();
    }

    return false;
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::OnSubSessionActive)
{
    CSessionStateProxy proxy(sigp.iSessionID);
    if (!proxy.IsBeTransfer())
    {
        return false;
    }

    if (proxy.IsFocusSession())
    {
        bool bTalk = sigp.lParam.bValue;
        // note: 0 一定是talk, 1一定是ringback
        CSessionStateProxy sub1 = proxy.GetSubByIndex(0);
        CSessionStateProxy sub2 = proxy.GetSubByIndex(1);

        if (!sub1 || !sub2)
        {
            return false;
        }

        SignalParams sigp1(sub1);
        SignalParams sigp2(sub2);
        sigp1.wParam.bValue = bTalk;
        sigp2.wParam.bValue = !bTalk;

        modtklsn_emit(TALK_SIG_SESSION_ACTIVED, sigp1);
        modtklsn_emit(TALK_SIG_SESSION_ACTIVED, sigp2);
    }
    else if (!proxy.GetHost() && proxy.IsInTalking())
    {
        _TalkAdapter.SetFocusSession(sigp.iSessionID);
    }

    return true;
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::OnAddSessionNetConf)
{
    CSessionStateProxy proxy(sigp.iSessionID);
    if (proxy.IsInherits(ROUTINE_NETWORKCONF))
    {
        _TalkAdapter.SetFocusSession(sigp.iSessionID);
    }

    return true;
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::OnPageReceived)
{
    //判断Callwaiting和呼入Multicast的权限
    if (!talklogic_SessionExist())
    {
        SIG_REPLY(RCR_OK);
    }

    int iPriority = sigp.wParam.iValue;
    int iReceivePriority = sigp.lParam.iValue;
    bool bPriorityActive = sigp.tParam.bValue;
    int iTalkingPriority = -1;

    CSessionStateProxy proxy;

    bool bFocusPage = proxy.BeMulticastPage();
    if (bFocusPage)
    {
        // 记录优先级
        iTalkingPriority = proxy.GetCallID();
    }

    // 限制通话路数
    // Call wait关闭时,通话中来Paging要拒绝
    // 当前通话是非Paging通话时来Paging,如果Paging权限比设定权限Page Barge低（即值大）,则要拒绝Paging来电
    // 当前通话是Paging通话且关闭权限比较(即Page Priority Active)时来Paging,Paging来电一律拒绝
    // 当前通话是Paging通话且开启权限比较时来Paging,如果Paging来电权限比当前Paging通话权限低（即值大）,则要拒绝来电
    bool bCallWait = talkhelp_IsCallWaitingEnable();
    if (!bCallWait
            || !talklogic_BeAllowNewCall()
            || !bFocusPage && iPriority > iReceivePriority
            || bFocusPage && !bPriorityActive
            || bFocusPage && bPriorityActive && iPriority > iTalkingPriority)
    {
        TALK_WARN("Refuese incoming call.(Callwaiting[%d]/CallInPriority[%d]/TalkingPriority[%d])",
                  bCallWait, iPriority, iTalkingPriority);
        SIG_REPLY(RCR_WAIT);
    }

    // 退出CallFinished状态的话路
    talklogic_ExitSession(ROUTINE_FINISHED);
    SIG_REPLY(RCR_OK);
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::OnSessionCreate)
{
    _TalkAdapter.OnSessionCreate(sigp.iSessionID);
    return true;
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::BeforeCallout)
{
    if (sigp.wParam.pValue == NULL)
    {
        return false;
    }

    CSessionStateProxy proxy;
    /* 尝试使用焦点话路呼出 */
    if (proxy)
    {
        CCallInfo* pCallInfo = proxy.GetCallInfo();
        if (pCallInfo == NULL)
        {
            return false;
        }

        CallOut_Param& param = (*(CallOut_Param*)sigp.wParam.pValue);
        int iAccount = param.iAccountID;
        const yl::string& strNumber = param.strNumber;

        if (!acc_IsAccountIDValid(iAccount) || strNumber.empty())
        {
            return false;
        }

        /* 返回拦截 */
        if (proxy.IsInherits(ROUTINE_CONNECTING))
        {
            SIG_REPLY(true);
        }
        else if (proxy.IsInherits(ROUTINE_PRETRAN))
        {
            // 在PreTran界面再新呼出一路，则先判断新呼出的账号如果和当前PreTran账号相同
            // 则在PreTran界面呼出新的号码，否则不再继续处理呼出
            if (iAccount == pCallInfo->GetAccountID())
            {
                CallOut_Param callParam;
                callParam.eCallOutTrigger = COT_DSSKEY;
                if (proxy.CallOut(strNumber, callParam))
                {
                    SIG_REPLY(true);
                }
            }
        }
        else if (proxy.IsInherits(ROUTINE_RINGING))
        {
            // 如果当前处于来电fwd界面，且新呼出的账号和当前fwd账号相同，则fwd新呼出的号码
            if (pCallInfo->IsPreForword()
                    && iAccount == pCallInfo->GetAccountID())
            {
                if (proxy.Forward(strNumber))
                {
                    SIG_REPLY(true);
                }
            }
        }
    }

    return false;
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::OnHoldChange)
{
    CSessionStateProxy proxy(sigp.iSessionID);

    /* unhold 当前需要hold其它话路 */
    if (sigp.lParam.bValue)
    {
        return false;
    }

    proxy = proxy.GetThisOrTopestHost();

    if (!proxy)
    {
        return false;
    }

    talklogic_HoldAllSession(sigp.iSessionID, proxy.IsBTTalk() ? HT_BTPHONE : HT_NORMAL);
    return true;
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::OnHostChange)
{
    int iOldSessionID = sigp.wParam.iValue;
    int iNewSessionID = sigp.lParam.iValue;

    if (iOldSessionID != INVALID_SESSION_ID
            && iNewSessionID == INVALID_SESSION_ID)
    {
        Push(sigp.iSessionID, iOldSessionID);
    }
    else if (iOldSessionID == INVALID_SESSION_ID
             && iNewSessionID != INVALID_SESSION_ID)
    {
        Push(sigp.iSessionID, iNewSessionID);
    }

    return true;
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::ResetFocusSession)
{
    if (m_vec.size() != 3)
    {
        return false;
    }

    int iHost = m_vec[0];
    int iFirst = m_vec[1];
    int iSecond = m_vec[2];
    int iFocus = _TalkAdapter.GetFocusedSessionID();
    int iNewFocus = INVALID_SESSION_ID;

    m_vec.clear();

    if (iHost == iFocus)
    {
        CSessionProxy first(iFirst);
        CSessionProxy second(iSecond);
        CSessionProxy host(iHost);

        bool bFirstValid = first && !first.IsDestructing();
        bool bSecondValid = second && !second.IsDestructing();

        if (first.IsRinging())
        {
            iNewFocus = iFirst;
        }
        else if (second.IsRinging())
        {
            iNewFocus = iSecond;
        }
        /* 新需求要求正常通话会议拆分后，焦点切换到最后一路 */
        else if (bSecondValid && host.IsLocalConf())
        {
            iNewFocus = iSecond;
        }
        else if (bFirstValid)
        {
            iNewFocus = iFirst;
        }
        else if (bSecondValid)
        {
            iNewFocus = iSecond;
        }
        else
        {
            iNewFocus = _TalkAdapter.GetFirstUnhideSession();
        }
    }
    else if (iFirst == iFocus
             || iSecond == iFocus)
    {
        iNewFocus = iHost;
    }

    if (iNewFocus != INVALID_SESSION_ID)
    {
        return _TalkAdapter.SetFocusSession(iNewFocus);
    }

    return false;
}

DECL_SIGNAL_HANDLER(CTalkAdapterListener::OnStartTalk)
{
    if (!sigp.wParam.bValue)
    {
        return false;
    }

    return CALL_HANDLER(OnAnswerCall);
}
