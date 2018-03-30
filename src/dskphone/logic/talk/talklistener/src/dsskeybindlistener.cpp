#include "talklistener_inc.h"
#include "talk/talkdsskey/include/dsskeybindmanager.h"

CDsskeyBindListener::CDsskeyBindListener()
    : LSN_CLS_LOG_NAME
{
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HIDE_CHANGE, CDsskeyBindListener::OnHideChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HOST_CHANGE, CDsskeyBindListener::OnHostChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_FOCUS_CHANGE, CDsskeyBindListener::OnFocusChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HOLD_CHANGE,  CDsskeyBindListener::OnRefresh);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ACCOUNT_CHANGE, CDsskeyBindListener::OnAccountChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CDsskeyBindListener::OnRoutineChange);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CDsskeyBindListener::OnRefresh);

    /* 通话退出的消息要同步处理 */
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CDsskeyBindListener::OnSessionExit);

    REG_SYNC_SIGNAL(TALK_SIG_DIAL_CALLOUT, CDsskeyBindListener::OnDialCallout);
    REG_SYNC_SIGNAL(TALK_SIG_PROCESS_CALLOUT, CDsskeyBindListener::OnCallOut);
    REG_SYNC_SIGNAL(TALK_FLT_SESSION_INCOME, CDsskeyBindListener::OnCallIncome);

    REG_SYNC_SIGNAL(TALK_FLT_SESSION_CREATE, CDsskeyBindListener::TryCreateSession);

}

DECL_SIGNAL_HANDLER(CDsskeyBindListener::OnSessionExit)
{
    AUTO_SCOPED_TIMER;

    CSessionProxy proxy(sigp.iSessionID);
    if (!proxy)
    {
        return false;
    }

    int iAccountID = proxy.GetAccountID();
    if (INVALID_HANDLE != iAccountID)
    {
        if (talklogic_GetCallNumberByAccountID(iAccountID) == 1
                && !acc_IsSupportSCA(iAccountID))
        {
            acc_SetSupportSCA(iAccountID, true);
            bsSCA_UpdateLightInfo(iAccountID);
        }
    }

    bool bRet = false;
    int iOldBindLinekey = _DsskeyBindManager.GetDsskeyBySession(proxy);
    bRet |= _DsskeyBindManager.DeleteBindBySession(proxy);
    TALK_DBG("OnSessionExit session[%d] key[%d] ret[%d]", sigp.iSessionID, iOldBindLinekey, bRet);
    if (iOldBindLinekey == INVALID_DSSKEY_ID || proxy.GetHost())
    {
        return bRet;
    }

    int iLineID = dsskey_GetDsskeyLine(iOldBindLinekey);
    // 刷新被释放话路的BLA状态(当前话机可能既是来电方又是呼出方)
    if (acc_GetBLAType(iLineID) != BLA_GENBAND)
    {
        bsSCA_UpdateLightInfo(iLineID);
    }
    // session退出后，需要将空闲出来的linedsskey绑到其他未绑定的dsskey上
    // 现在话路统计在linekey后,就算有空闲linekey也不重新做绑定
    bRet |= _DsskeyBindManager.RebindLinekey(iOldBindLinekey, proxy);
    return bRet;
}

DECL_SIGNAL_HANDLER(CDsskeyBindListener::OnHideChange)
{
    CSessionProxy proxy(sigp.iSessionID);
    if (!proxy || proxy.GetHost())
    {
        return false;
    }

    bool bHide = sigp.lParam.bValue;
    TALK_DBG("OnSessionHide session[%d] hide[%d] host[%p]", sigp.iSessionID, bHide, (int)proxy.GetHost());

    if (bHide)
    {
        // http://10.2.1.199/Bug.php?BugID=58552
        // 记录旧的dsskey id,盲转后重新绑定给新的话路
        int iOldDsskeyID = _DsskeyBindManager.GetDsskeyBySession(proxy);
        bool bRet = _DsskeyBindManager.DeleteBindBySession(proxy);
        bRet |= _DsskeyBindManager.RebindLinekey(iOldDsskeyID, proxy);
        return bRet;
    }

    // 重新绑定Dsskey
    return _DsskeyBindManager.AddBinderByAccount(proxy, proxy.GetAccountID());
}

DECL_SIGNAL_HANDLER(CDsskeyBindListener::OnFocusChange)
{
    CSessionProxy proxy(sigp.iSessionID);

    /* 没有焦点，则没有通话，删除绑定关系 */
    if (!proxy || proxy.GetHost())
    {
        return false;
    }

    // SCA通话焦点切换，重新绑定session
    if (!proxy.IsBla())
    {
        return false;
    }

    if (_DsskeyBindManager.GetDsskeyBySession(proxy) != INVALID_DSSKEY_ID)
    {
        return false;
    }

    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    int iDsskeyID = dsskey_GetDsskeyByBLAIndex(pCallInfo->m_iBLAIndex, pCallInfo->GetAccountID());
    CSessionProxy bind = _DsskeyBindManager.GetSessionByDsskey(iDsskeyID);
    if (!bind)
    {
        return false;
    }

    TALK_DBG("OnFocusChange session[%d] key[%d] bind[%d]", sigp.iSessionID, iDsskeyID, (int)bind);
    return _DsskeyBindManager.ModifySession(bind, proxy, true);
}

DECL_SIGNAL_HANDLER(CDsskeyBindListener::OnHostChange)
{
    AUTO_SCOPED_TIMER;

    CSessionProxy sub(sigp.iSessionID);
    if (!sub)
    {
        return false;
    }

    int iOldHost = sigp.wParam.iValue;
    int iNewHost = sigp.lParam.iValue;

    TALK_DBG("OnHostChange target[%d] host[%d]->[%d]", sigp.iSessionID, iOldHost, iNewHost);

    /* null 2 host */
    if (iOldHost == INVALID_SESSION_ID
            && iNewHost != INVALID_SESSION_ID)
    {
        /* sub转移给host */
        CSessionProxy host(iNewHost);

        /* SCA BT should not transfer bind relationship */
        if (sub.IsBla() || sub.IsBtSession())
        {
            /* SCA话路不绑定到LocalSession，保持subsession的绑定关系
            * 蓝牙话路也不绑定到LocalSession */
            // http://10.2.1.199/Bug.php?BugID=85182
            // sca本地会议需要改变统计话路，计算话路个数，只算一路 既只算localsession TODO
            _DsskeyBindManager.ChangeRelationSession(sub, host);
        }
        else
        {
            if (_DsskeyBindManager.GetDsskeyBySession(host) != INVALID_DSSKEY_ID)
            {
                /* host已经绑定了，直接删除sub绑定 */
                CALL_HANDLER(OnSessionExit);
            }
            else
            {
                _DsskeyBindManager.ModifySession(sub, host, true);
                /* 记录绑定关系 */
                CListenInfo* privTemp = GetSessionData(host);
                if (privTemp != NULL)
                {
                    privTemp->iBindSubSession = sub;
                }
            }
        }
    }
    /* host 2 null */
    else if (iOldHost != INVALID_SESSION_ID
             && iNewHost == INVALID_SESSION_ID)
    {
        /* host转移给sub */
        CSessionProxy host(iOldHost);
        if (_DsskeyBindManager.GetDsskeyBySession(host) == INVALID_DSSKEY_ID)
        {
            /* host没有绑定关系，sub需要重新分配一个 */
            _DsskeyBindManager.AddBinderByAccount(sub, sub.GetAccountID());
        }
        else
        {
            /* 还原绑定关系，优先还原给转移给Host的那一路 */
            CListenInfo* privTemp = GetSessionData(host);
            if (privTemp != NULL)
            {
                CSessionProxy oldBind(privTemp->iBindSubSession);
                _DsskeyBindManager.ModifySession(host, oldBind ? oldBind : sub, true);
            }
        }
    }
    /* host 2 host */
    else
    {
        /* host改变，修改保存记录 */
        CListenInfo* privOld = GetSessionData(iOldHost);
        if (privOld != NULL && privOld->iBindSubSession == sigp.iSessionID)
        {
            /* 修改记录： new插入到原 host和sub中间 */
            CListenInfo* privNew = GetSessionData(iNewHost);
            if (privNew != NULL)
            {
                privOld->iBindSubSession = iNewHost;
                privNew->iBindSubSession = sigp.iSessionID;
            }
        }
    }

    return true;
}

DECL_SIGNAL_HANDLER(CDsskeyBindListener::OnAccountChange)
{
    CSessionProxy proxy(sigp.iSessionID);
    if (!proxy || proxy.GetHost())
    {
        return false;
    }

    // 记录旧的lineky，以便将绑定的Dsskey让给其他Session
    int iOldBindLinekey = _DsskeyBindManager.GetDsskeyBySession(proxy);
    TALK_DBG("OnAccountChange session[%d] bind[%d]", sigp.iSessionID, iOldBindLinekey);
    // 先取消绑定
    _DsskeyBindManager.DeleteBindBySession(proxy);
    // 绑定到新的Dsskey
    _DsskeyBindManager.AddBinderByAccount(proxy, proxy.GetAccountID());
    // 将之前绑定的Dsskey让给新的Session
    _DsskeyBindManager.RebindLinekey(iOldBindLinekey, proxy);
    return true;
}

DECL_SIGNAL_HANDLER(CDsskeyBindListener::OnRefresh)
{
    CSessionProxy proxy(sigp.iSessionID);
    if (!proxy.IsBla())
    {
        _DsskeyBindManager.ProcessAfterBind(proxy.GetThisOrTopestHost());
        return true;
    }
    return false;
}

DECL_SIGNAL_HANDLER(CDsskeyBindListener::OnRoutineChange)
{
    CSessionProxy session(sigp.iSessionID);
    if (session.GetHost())
    {
        return false;
    }

    CCallInfo* pCallInfo = session.GetCallInfo();
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }

    CallOut_Param& calloutParam = pCallInfo->GetCallOutParam();
    int iDsskey = calloutParam.iSponsorDsskeyID;
    bool bLinekey = false;
    if (iDsskey != INVALID_DSSKEY_ID)
    {
        bLinekey = dsskey_GetDsskeyType(iDsskey) == DT_LINE;
    }

    bool bFirstRoutine = sigp.tParam.bValue;
    if (calloutParam.bBindDsskey
            && iDsskey != INVALID_DSSKEY_ID
            && bFirstRoutine
            && session.IsSingle()
            && !session.IsBla())
    {
        /* dsskey呼出直接绑定到对应的key */
        bool bModify = false;
        if (Comm_IsRoutineInherits(ROUTINE_DIALING_CLS, calloutParam.eRoutineType))
        {
            CSessionProxy bind = _DsskeyBindManager.GetSessionByDsskey(iDsskey);
            if (bind)
            {
                // 读取配置,账号限制通话数量
                bModify = _DsskeyBindManager.ModifySession(bind, session, false, calloutParam.bDelOldLineSession);
            }
        }

        if (!bModify)
        {
            // Dsskey和Session绑定
            _DsskeyBindManager.AddBinder(session, iDsskey, bLinekey);
        }
    }

    if (bFirstRoutine
            && pCallInfo->m_bNeedBindDsskey
            && _DsskeyBindManager.GetDsskeyBySession(session, true) == INVALID_DSSKEY_ID)
    {
        // 第一次创建Session时将其绑定到对应的key
        _DsskeyBindManager.AddBinderByAccount(session, pCallInfo->GetAccountID());
    }

    if (session.GetRoutineState() != ROUTINE_DIALING)
    {
        return true;
    }

    iDsskey = _DsskeyBindManager.GetDsskeyBySession(session, false);
    if (iDsskey == INVALID_DSSKEY_ID)
    {
        return true;
    }

    DssKey_Type eType = dsskey_GetDsskeyType(iDsskey);

    priv->m_bDPickupDial = (eType == DT_PICKUP
                            || pCallInfo->m_eTalkCallOutType == TCT_PICKUP);
#if IF_BUG_27033
#if IF_FEATURE_PAGING
    priv->m_bPagingDial = (eType == DT_PAGING
                           || pCallInfo->m_eTalkCallOutType == TCT_PAGING);
#endif
    priv->m_bIntercomDial = (eType == DT_INTERCOM)
                            || pCallInfo->m_eTalkCallOutType == TCT_INTERCOM);
#endif

#if IF_BUG_30965
    priv->m_bUnParkDial = (eType == DT_UNPARK
                           || pCallInfo->m_eTalkCallOutType == TCT_UNPARK);
#endif //#if IF_BUG_30965
    return true;
}

DECL_SIGNAL_HANDLER(CDsskeyBindListener::OnDialCallout)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    // Intercom绑定的Linekey
    int iDsskeyID = _DsskeyBindManager.GetDsskeyBySession(proxy, false);
    if (dsskey_GetDsskeyType(iDsskeyID) == DT_INTERCOM)
    {
        // KTS intercom value为空的情况下，呼出时不需要绑定
        if (dsskey_GetDsskeyValue(iDsskeyID).empty())
        {
            _DsskeyBindManager.DeleteBindByDsskey(iDsskeyID, false);
        }

        yl::string strDialedNum = pCallInfo->GetDialText();
        dsskey_GetCallOutNumber(iDsskeyID, strDialedNum, &(pCallInfo->GetCallOutParam()));
        pCallInfo->SetDialedNumber(strDialedNum);
    }
    return true;
}

DECL_SIGNAL_HANDLER(CDsskeyBindListener::OnCallOut)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    CallOut_Param& calloutParam = pCallInfo->GetCallOutParam();
    int iDsskeyID = calloutParam.iSponsorDsskeyID;
    int iAccountID = pCallInfo->GetAccountID();

    if (iDsskeyID != INVALID_DSSKEY_ID)
    {
        DssKey_Type eDskType = dsskey_GetDsskeyType(iDsskeyID);

#ifdef IF_FEATURE_BROADSOFT_MEET_ME
        // 获取预输入的DTMF
        if (eDskType == DT_MEET_ME_CONFERENCE)
        {
            pCallInfo->m_strAutoDTMF = dsskey_GetDsskeyExtension(iDsskeyID);
        }
#endif //#ifdef IF_FEATURE_BROADSOFT_MEET_ME

#if IF_BUG_27033 || IF_BUG_30965
        switch (eDskType)
        {
        case DT_PICKUP:
            {
                pCallInfo->m_eDialDsskeyType = DDT_PICKUP;
            }
            break;
#if IF_FEATURE_PAGING
        case DT_PAGING:
            {
                pCallInfo->m_eDialDsskeyType = DDT_PAGING;
            }
            break;
#endif
        case DT_INTERCOM:
            {
                pCallInfo->m_eDialDsskeyType = DDT_INTERCOM;
            }
            break;
        case DT_UNPARK:
            {
                pCallInfo->m_eDialDsskeyType = DDT_UNPARK;
            }
            break;
        default:
            pCallInfo->m_eDialDsskeyType = DDT_NONE;
            break;
        }
#endif

        bool bBindSession = true;
        if (eDskType == DT_BLF
#ifdef IF_FEATURE_GENBAND
                || eDskType == DT_BLFGROUP
#endif
                || eDskType == DT_BLFLIST)
        {
            int iStatus = dsskey_GetDsskeyStatus(iDsskeyID);
            switch (iStatus)
            {
            case DS_BLF_STATUS_RINGING://如果是BLF类型，截答其他话路后不去绑定到Session上:http://10.2.1.199/Bug.php?BugID=25157
            case DS_BLF_STATUS_TALKING://http://192.168.1.99/Bug.php?BugID=22416
            case DS_BLF_STATUS_HOLD:
            case DS_BLF_STATUS_PARK:
                {
                    bBindSession = false;
                }
                break;
            default:
                break;
            }

            bBindSession &=
                calloutParam.iPickUpBLFIndex == -1
                && calloutParam.eCallOutType != TCT_PICKUP;
        }

        if (bBindSession)
        {
            _DsskeyBindManager.AddBinder(proxy, iDsskeyID, false);
        }

        dialplan_GetCalloutParam(proxy, iAccountID, COT_DSSKEY, calloutParam);
    }

    // SCA 不需要替换号码
    if (calloutParam.iBLAIndex == INVALID_HANDLE)
    {
        return false;
    }

    // 在SCA占线之前匹配否则可能跳过该流程，呼出前必须带匹配标志同时没有设置特殊呼出标志
    dialplan_GetChangeAccount(iAccountID, sigp.strData.c_str(), &calloutParam);
    pCallInfo->SetAccountID(iAccountID);

    return true;
}

DECL_SIGNAL_HANDLER(CDsskeyBindListener::OnCallIncome)
{
    if (sigp.tParam.iValue != CALL_LINE_SIP)
    {
        return false;
    }

    int iAccountID = sigp.wParam.iValue;

    // 所有注册该账号的line key通话数量都达到上限则拒绝
    if (!acc_IsDirectIPAccount(iAccountID)
            && !_DsskeyBindManager.IsAllowNewCallByAccount(iAccountID))
    {
        SIG_REPLY(configParser_GetConfigInt(kszNormalRefuseCode));
    }

    return false;
}

DECL_SIGNAL_HANDLER(CDsskeyBindListener::TryCreateSession)
{
    if (sigp.iSessionID != INVALID_SESSION_ID)
    {
        return false;
    }

    if (_DsskeyBindManager.IsAllowNewCallByAccount(sigp.tParam.iValue))
    {
        return true;
    }

    SIG_REPLY(false);
}
