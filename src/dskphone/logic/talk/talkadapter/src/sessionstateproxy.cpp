#include "talkadapter_inc.h"
#include "talk/talklogic/src/talklogic_inc.h"
#include "talk/talkdsskey/include/dsskeybindmanager.h"

extern CBaseSession* GetFocusedSession();
extern CSingleSession* GetFocusedSingleSession();

CSessionStateProxy::CSessionStateProxy(CSingleSession* p /*= NULL*/) : CSessionProxy(p)
{
    if (p == NULL)
    {
        m_pSession = GetFocusedSession();
    }
}

DIAL_UI_TYPE CSessionStateProxy::GetDialUIType() const
{
    CSingleSession* pSession = ToSingleSession();
    if (pSession == NULL)
    {
        return DIAL_UI_UNKNOWN;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();
    CListenInfo* priv = modtklsn_get_privdata(pSession->GetSessionID());
    if (priv == NULL)
    {
        return DIAL_UI_UNKNOWN;
    }
    switch (GetInheritsRoutine())
    {
    case ROUTINE_RINGING:
        {
            if (pCallInfo->IsPreForword())
            {
                return DIAL_UI_RINGFORWARD;
            }
        }
        break;
    case ROUTINE_TALKING:
        {
            if (priv->IsPrePark())
            {
                return DIAL_UI_CALLPARK;
            }
            else if (priv->IsPreGPark())
            {
                return DIAL_UI_GROUPCALLPARK;
            }
        }
        break;
    case ROUTINE_DIALING:
        break;
    default:
        return DIAL_UI_UNKNOWN;
    }

    switch (pSession->GetRoutineState())
    {
    case ROUTINE_DIALING:
        {
            if (pSession->InPreConf())
            {
                return DIAL_UI_CONFERENCE;
            }
            else if (priv->m_bDPickupDial)
            {
                return DIAL_UI_DPICKUP;
            }
#if IF_BUG_27033
#if IF_FEATURE_PAGING
            else if (priv->m_bPagingDial)
            {
                return DIAL_UI_PAGING;
            }
#endif
            else if (priv->m_bIntercomDial)
            {
                return DIAL_UI_INTERCOM;
            }
#endif
#if IF_BUG_30965
            else if (priv->m_bUnParkDial)
            {
                return DIAL_UI_UNPARK;
            }
#endif

            return DIAL_UI_DIALING;
        }
        break;
    case ROUTINE_PREDIAL:
        {
            if (priv->m_bDPickupDial)
            {
                return DIAL_UI_DPICKUP;
            }
            else
            {
                return DIAL_UI_PREDIAL;
            }
        }
        break;
    case ROUTINE_PRETRAN:
        {
#if IF_FEATURE_DIAL_ON_HOLD
            if (pCallInfo->m_bDialOnHold)
            {
                return DIAL_UI_ON_HOLD;
            }
            else
#endif //IF_FEATURE_DIAL_ON_HOLD
            {
                return DIAL_UI_TRANSFER;
            }
        }
        break;
    case ROUTINE_RETRIEVE_CALLPARK:
        {
            return DIAL_UI_RETRIEVE_CALLPARK;
        }
        break;
#ifdef IF_BT_SUPPORT_PHONE
    case ROUTINE_BTDIALING:
        {
            return DIAL_UI_BLUETOOTH_DIALING;
        }
        break;
#endif
    default:
        break;
    }

    return DIAL_UI_UNKNOWN;
}

bool CSessionStateProxy::IsFocusSession() const
{
    return m_pSession && m_pSession == GetFocusedSession();
}

bool CSessionStateProxy::IsTopestHostFocused() const
{
    CSessionStateProxy host = GetThisOrTopestHost();
    return host.IsFocusSession();
}

bool CSessionStateProxy::IsAllowSwap() const
{
    CSingleSession* pSession = ToSingleSession();
    if (pSession == NULL)
    {
        return false;
    }

    if (_SessionManager.GetSessionListSize() == 2)
    {
        if (configParser_GetConfigInt(kszIncomingCallIgnor) != 0
                && talklogic_IsRingSessionExist())
        {
            // 当前只有两路，且当前路为通话，另一路不能是来电
            return false;
        }

        bool bIsAllowConf = !_SessionManager.IsLocalConfExist()
                            && IsAllowCreateConf()
                            && IsAllowAddToConf()
                            && IsAllowCreateLocalConf();

        if (bIsAllowConf)
        {
            // 原有体验开启(通话中 && 满足会议)
            if (configParser_GetConfigInt(kszCustomSoftkeySwitch) != 0
                    || configParser_GetConfigInt(kszHoldOrSwapMode) == 0)
            {
                return pSession->GetCallInfo()->m_eTalkState == TS_TALK;
            }

            return configParser_GetConfigInt(kszHoldOrSwapMode) == 2;
        }

        CBaseSession* pAnotherSession = pSession->GetAnotherSession();
        if (pAnotherSession
                && pAnotherSession->IsSingleSession()
                && pAnotherSession->GetCallInfoAsSub()->IsTransferred())
        {
            return false;
        }
    }

    return false;

}

bool CSessionStateProxy::IsAllowCreateConf() const
{
    if (!IsSingle())
    {
        return false;
    }

    CCallInfo* pCallInfo = GetCallInfo();

    if (configParser_GetConfigInt(kszLocalConferenceEnabled) == 0
            && pCallInfo->GetConfType() == CT_LOCAL)
    {
        return false;
    }

    if (!_DsskeyBindManager.IsAllowConf(m_pSession))
    {
        return false;
    }

    // 当前话路是通话中才允许
    bool bIsBSFTConf = pCallInfo->GetConfType() == CT_BROADSOFT;
    if ((bIsBSFTConf && pCallInfo->m_eTalkState != TS_TALK)
            || (!bIsBSFTConf &&
                (pCallInfo->m_eTalkState != TS_TALK
                 && pCallInfo->m_eTalkState != TS_HELD))
#if IF_FEATURE_PAGING
            || pCallInfo->BeMulticastPage()
#endif
       )
    {
        return false;
    }

    // 如果只有一个话路，且通话双方都未hold允许Conf
    if (_SessionManager.GetSessionListSize() == 1
            && (pCallInfo->m_eTalkState == TS_TALK
                || pCallInfo->m_eTalkState == TS_HELD))
    {
        return true;
    }

    // 当前话路是网络会议类型
    return !bIsBSFTConf || !configParser_GetCfgItemStringValue(kszConfURI, pCallInfo->GetAccountID()).empty();
}

bool CSessionStateProxy::IsAllowConf() const
{
    if (!IsSingle())
    {
        return false;
    }

    if (!IsAllowCreateConf()
            || !IsAllowAddToConf())
    {
        return false;
    }

    CCallInfo* pCallInfo = GetCallInfo();
    if (pCallInfo && pCallInfo->GetConfType() != CT_BROADSOFT)
    {
        return IsAllowCreateLocalConf();
    }

    return true;
}

bool CSessionStateProxy::IsAllowAddToConf() const
{
    return m_pSession && m_pSession->IsAllowAddToConf();
}

bool CSessionStateProxy::IsAllowConfWithAnother() const
{
    CSingleSession* pSession = ToSingleSession();
    if (pSession == NULL)
    {
        return false;
    }

    CCallInfo* pCallInfo = GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    bool bConfbyNewDial = (configParser_GetConfigInt(kszConfWithPreviousCall) == 0);

    if (pCallInfo->GetConfType() == CT_BROADSOFT)
    {
        //是否存在主持方网络会议
        bool bExistHostNetConf = _SessionManager.GetHostNetConfSession(pCallInfo->GetAccountID()) != NULL;

        return (!bExistHostNetConf
                && (pSession->GetSponsorSession(SPONSOR_CONF) == NULL)
                && !pSession->InTransfer()
                && IsExistSessionToConf()
                && (bConfbyNewDial
                    || GetTotalNumOfRoutineAllowAddNetConf(pCallInfo->GetAccountID()) > CONFCALLNUM));
    }

    //当前不存在网络会议，且存在可以加入conf的话路，且当前话路没有sponsor conf，并且bConfbyNewDial为true或者话路超出会议最大支持数
    return ((pSession->GetSponsorSession(SPONSOR_CONF) == NULL)
            && !pSession->InTransfer()
            && IsExistSessionToConf()
            && (bConfbyNewDial || talklogic_IsAllowAddLocalConfNumOverMax()));
}

bool CSessionStateProxy::IsAllowCreateLocalConf() const
{
    //yef:只要当前会议与会者数量没有超过最大限制就ok
    CLocalConfSession* pSession = _SessionManager.GetConfSession();
    if (pSession == NULL)
    {
        return true;
    }

    //会议中参与方达到最大话路时，不能再建会议
    return (pSession->GetSubSessionSize() < LOCAL_CONF_MAX_PARTY);
}

bool CSessionStateProxy::IsAllowAddToNetConf(int iAccountID) const
{
    if (!IsSingle())
    {
        return false;
    }

    // 网络会议的两路是不同账号或者网络会议的conf uri为空，则不允许会议
    return (GetAccountID() == iAccountID
            && !configParser_GetCfgItemStringValue(kszConfURI, iAccountID).empty());
}

bool CSessionStateProxy::IsAllowTran() const
{
    CSingleSession* pSession = ToSingleSession();
    if (pSession == NULL)
    {
        return false;
    }

    if (pSession->GetRoutineState() == ROUTINE_NETWORKCONF)
    {
        return false;
    }

#if IF_FEATURE_FORBIDDEN_FWD_TRAN
    if (configParser_GetConfigInt(kszAllowTransfer) != 1)
    {
        return false;
    }
#endif //IF_FEATURE_FORBIDDEN_FWD_TRAN

    CCallInfo* pCallInfo = pSession->GetCallInfo();

#ifdef IF_BT_SUPPORT_PHONE
    if (pCallInfo->IsBtTalk())
    {
        return false;
    }
#endif

#ifdef IF_FEATURE_PSTN
    if (pCallInfo->GetCallLineType() == CALL_LINE_PSTN)
    {
        // PSTN通话禁止转移
        return false;
    }
#endif // IF_FEATURE_PSTN

    if (pCallInfo->IsTransferred()
#if IF_FEATURE_PAGING
            || pCallInfo->BeMulticastPage()
#endif
       )
    {
        return false;
    }

    // 如果sponsor有hostsession则不允许tran
    CSingleSession* pTmpSession = pSession->GetSponsorSession(SPONSOR_TRAN);
    if (pTmpSession != NULL && pTmpSession->GetHostSession() != NULL)
    {
        return false;
    }

    // 如果follow有hostsession则不允许tran
    pTmpSession = pSession->GetFollowSession(SPONSOR_TRAN);
    if (pTmpSession != NULL && pTmpSession->GetHostSession() != NULL)
    {
        return false;
    }

    return true;
}

bool CSessionStateProxy::BeAllowTransferTo() const
{
    return GetSessionState() == SESSION_TALKING;
}

bool CSessionStateProxy::IsAllowTranTo(int iSessioID) const
{
    if (m_pSession == NULL || m_pSession->GetSessionID() == iSessioID)
    {
        return false;
    }

    CSessionStateProxy other(iSessioID);
    // 如果有hostsession则不允许tran
    if (m_pSession->GetHostSession() != NULL
            || other.GetHost())
    {
        return false;
    }

    // 有一路不允许transfer to
    if (!BeAllowTransferTo()
            || !other.BeAllowTransferTo())
    {
        return false;
    }

    // 非单路不允许
    if (!other.IsSingle() || !IsSingle())
    {
        return false;
    }

    // 两个Session的账号相同才允许互相transfer
    return (GetAccountID() == other.GetAccountID());
}

bool CSessionStateProxy::IsTransferDirectly() const
{
    CSingleSession* pSession = ToSingleSession();
    if (pSession == NULL)
    {
        return false;
    }

    int iSessionSize = _SessionManager.GetSessionListSize();
    int iCallType = pSession->GetCallInfo()->m_eTalkCallOutType;
    if (iSessionSize != 2 || iCallType != TCT_TIPTEL_AUTO_CALLOUT)
    {
        TALK_INFO("Not allow trans directly size[%d] type[%d]", iSessionSize, iCallType);
        return false;
    }

    return true;
}

bool CSessionStateProxy::IsAllowFwd() const
{
    if (!IsSingle())
    {
        return false;
    }

    //flexible seating禁用来电时的Forward
    return (fwd_IsActive()
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
            && !CallCenter_IsFlexibleSeatingOn(GetCallInfo()->GetAccountID())
#endif
           );
}

bool CSessionStateProxy::IsAllowNewCall() const
{
    CCallInfo* pCallInfo = GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    bool bNewDial = IsAllowNewDail();

    if (IsInherits(ROUTINE_NETWORKCONF))
    {
        return bNewDial;
    }
    else if (IsInherits(ROUTINE_TALKING))
    {
        return bNewDial || pCallInfo->IsLocalHold();
    }

    return false;
}

bool CSessionStateProxy::IsAllowNewCall(int iAccountID) const
{
    return iAccountID != INVALID_ACCOUNTID && _DsskeyBindManager.IsAllowNewCallByAccount(iAccountID);
}

bool CSessionStateProxy::IsAllowNewDail() const
{
    // T19没有dsskey,无需限制
    if (PT_T19 == devicelib_GetPhoneType())
    {
        return true;
    }

    return IsAllowNewCall(GetAccountID());
}

bool CSessionStateProxy::IsAllowCreateNewCall() const
{
    CBaseSession* pSession = GetFocusedSession();
    if (pSession != NULL && pSession->IsSingleSession())
    {
        // 如果焦点话路是拨号界面则允许再呼出新的话路
        if (IsInherits(ROUTINE_DIALING))
        {
            return true;
        }

        if (pSession->GetCallInfoAsSub()->IsPreForword())
        {
            return true;
        }
    }

    // 有呼出话路存在时不允许再新建Session
    return !_SessionManager.IsSessionExist(ROUTINE_CONNECTING);
}

bool CSessionStateProxy::IsAllowHold() const
{
    CCallInfo* pCallInfo = GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    return pCallInfo->m_bHoldAvailable || pCallInfo->BeMulticastPage();
}

bool CSessionStateProxy::IsAllowJoinLocalConf() const
{
    if (IsInherits(ROUTINE_RINGING))
    {
        //网络会议不需要
        if (talklogic_GetSessionNum() < 2
                || talklogic_IsSessionExist(ROUTINE_NETWORKCONF, *this)
                || GetCallInfo()->GetConfType() == CT_BROADSOFT
                || talklogic_IsSessionExist(ROUTINE_TALKING, *this))
        {
            return false;
        }

        return IsAllowCreateLocalConf();
    }
    else if (IsInherits(ROUTINE_TALKING))
    {
        CSingleSession* pSession = _SessionManager.GetFirstIncomingCallSession();
        if (pSession == NULL || pSession == m_pSession)
        {
            return false;
        }

        CSessionStateProxy proxy(pSession);
        return proxy.IsAllowJoinLocalConf();
    }

    return false;
}

bool CSessionStateProxy::SwapToAnother() const
{
    if (IsAllowSwap() && IsInherits(ROUTINE_TALKING))
    {
        CBaseSession* pAnother = m_pSession->GetAnotherSession();
        if (pAnother == NULL)
        {
            return false;
        }

        pAnother->HoldSession(false);
        _TalkAdapter.SetFocusSession(pAnother->GetSessionID());
        return true;
    }

    return false;
}

bool CSessionStateProxy::ShowSessionAddToConf() const
{
    // 如果网络会议失败了，则将加入网络会议的话路显示
    _SessionManager.ShowSessionAddToConf();
    return true;
}

void CSessionStateProxy::AddConfToTarget(YLList<CCallInfo*>& list) const
{
    list.clear();

    YLVector<int> vec;
    if (!GetSessionToConf(vec))
    {
        return;
    }

    for (YLVector<int>::iterator it = vec.begin(); it != vec.end(); ++it)
    {
        CCallInfo* pCallInfo = talklogic_GetCallInfoBySessionID(*it);
        if (pCallInfo == NULL)
        {
            continue;
        }

        list.push_back(pCallInfo);
    }
}

void CSessionStateProxy::AddTransferToTarget(YLList<CCallInfo*>& list) const
{
    list.clear();

    CSingleSession* pSession = ToSingleSession();
    if (pSession == NULL)
    {
        return;
    }

    CSessionStateProxy proxy(pSession->GetSponsorSession(SPONSOR_TRAN));
    if (!proxy)
    {
        return;
    }

    FOR_SESSION_LIST_EXPORT(it)
    {
        if (!proxy.IsAllowTranTo((*it)->GetSessionID()))
        {
            continue;
        }

        CCallInfo* pCallInfo = (*it)->GetCallInfoAsSub();
        if (pCallInfo == NULL)
        {
            continue;
        }

        // 更新logic中的链表
        list.push_back(pCallInfo);
    }
}

bool CSessionStateProxy::GetSessionToConf(YLVector<int>& vec) const
{
    CSingleSession* pSession = ToSingleSession();
    if (pSession == NULL)
    {
        return false;
    }

    //判断是否网络会议
    CCallInfo* pCallInfo = pSession->GetCallInfo();
    bool bNetConf = pCallInfo->GetConfType() == CT_BROADSOFT;

    FOR_SESSION_LIST_EXPORT(it)
    {
        CSingleSession* pSessionTmp = (*it)->ToSingleSession();

        //发起方不显示
        //会议参与方不显示
        if (NULL == pSessionTmp
                || pSessionTmp == pSession->GetSponsorSession(SPONSOR_CONF)
                || NULL != pSessionTmp->GetHostSession())
        {
            continue;
        }

        CSessionStateProxy proxy(pSessionTmp);
        if (!proxy.IsAllowAddToConf()
                || pSessionTmp->IsHide())
        {
            continue;
        }

        CCallInfo* pCallInfoTmp = pSessionTmp->GetCallInfo();
        //半咨询转不用加入会议。
        if (pCallInfoTmp->IsTransferred())
        {
            continue;
        }

        //网络会议需要再判断是否允许建立网络会议
        if (bNetConf)
        {
            if (proxy.IsInherits(ROUTINE_TALKING)
                    && !proxy.IsAllowAddToNetConf(pCallInfo->GetAccountID()))
            {
                continue;
            }
        }
        else
        {
            //当前是本地会议的conf with则不显示网络会议
            if (proxy.IsInherits(ROUTINE_NETWORKCONF))
            {
                continue;
            }
        }

        vec.push_back(pSessionTmp->GetSessionID());
    }

    return vec.size() > 0;
}

bool CSessionStateProxy::IsExistSessionToConf() const
{
    YLVector<int> vec;
    return GetSessionToConf(vec);
}

int CSessionStateProxy::GetNumberOfAllowTranTo() const
{
    int iNum = 0;
    FOR_SESSION_LIST_EXPORT(it)
    {
        if (IsAllowTranTo((*it)->GetSessionID()))
        {
            ++iNum;
        }
    }

    return iNum;
}

bool CSessionStateProxy::IsCallDeclineEnable() const
{
    if (configParser_GetConfigInt(kszBroadsoftActive) == 0)
    {
        return false;
    }

    yl::string bCallDeclineEnable = configParser_GetCfgItemStringValue(kszAccountCallDeclineEnable, GetCallInfo()->GetAccountID());
    if (bCallDeclineEnable.empty())
    {
        bCallDeclineEnable = configParser_GetConfigString(kszCallDeclineEnable);
    }

    return atoi(bCallDeclineEnable.c_str()) == 1;
}

bool CSessionStateProxy::IsEmergencyCalled() const
{
    CCallInfo* pCallInfo = GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    const yl::string& strRemoteNumber =
        configParser_GetConfigInt(kszUseFromNumberMatchEmergency) == 1 ?
        pCallInfo->m_tRemoteInfo.sFromSIPName : pCallInfo->m_tRemoteInfo.sSIPName;

    bool bIsEmergency = Emergency_IsIncomingEmergency(strRemoteNumber.c_str());
    //若是 紧急通话，并且配置有开启
    return (bIsEmergency && Emergency_IsEnabelEmergencyCalling());
}

int CSessionStateProxy::GetTotalNumOfRoutineAllowAddNetConf(int nAccountID) const
{
    return _SessionManager.GetTotalNumOfRoutineAllowAddNetConf(nAccountID);
}

//可建立本地会议数是否超过会议最大支持路数
bool CSessionStateProxy::IsAllowAddLocalConfNumOverMax() const
{
    return _SessionManager.IsAllowAddLocalConfNumOverMax();
}

//获取当前允许加入本地会议的话路数
int CSessionStateProxy::GetAllowAddLocalConfRoutineNum() const
{
    return _SessionManager.GetAllowAddLocalConfRoutineNum();
}

void CSessionStateProxy::ShowInfo(int iType)
{
    Error_Info eErrorInfo;
    eErrorInfo.eErrorType = (ERROR_INFO_TYPE)iType;

    if (eErrorInfo.eErrorType == ETI_ALREADY_PARKED_TO_THIS_SLOT)
    {
        eErrorInfo.iErrorShowTime = 3000;
    }

    // 通知UI更新
    AdapterBox_TalkUIUpdateErrorInfo((void*)&eErrorInfo);
}

bool CSessionStateProxy::IsPreForword() const
{
    CCallInfo* pCallInfo = GetCallInfo();
    return pCallInfo && pCallInfo->IsPreForword();
}

bool CSessionStateProxy::AttendedTransfer(const CallOut_Param& callParam, bool bTranByBLF /*= false*/) const
{
    if (callParam.strNumber.empty())
    {
        return false;
    }

    CSingleSession* pSponsorSession = _SessionManager.GetRelateSessionByCallID(GetCallID());
    if (NULL == pSponsorSession)
    {
        return false;
    }

    if (!pSponsorSession->IsInherits(ROUTINE_TALKING))
    {
        return false;
    }

    // 如果当前Session是dial Session，直接从当前话路转，无需新建话路
    CSingleSession* pFollowSession = NULL;

    CSingleSession* pFocusedSession = GetFocusedSingleSession();
    if (NULL != pFocusedSession
            && pFocusedSession->IsInherits(ROUTINE_DIALING))
    {
        pFollowSession = pFocusedSession;
    }
    else
    {
        pFollowSession = _SessionManager.AllocSession();
        if (pFollowSession == NULL)
        {
            return false;
        }
    }

    CCallInfo* pFollowCallInfo = pFollowSession->GetCallInfo();
    CCallInfo* pSponsorCallInfo = pSponsorSession->GetCallInfo();

    if (pFollowSession != pFocusedSession)
    {
        pFollowCallInfo->GetCallOutParam() = callParam;
    }

    pFollowSession->SetSponsorSession(SPONSOR_TRAN, pSponsorSession);

    pFollowCallInfo->SetAccountID(pSponsorCallInfo->GetAccountID());
#if IF_BUG_30961
    pFollowCallInfo->m_bTranByBLF = bTranByBLF;
    pSponsorCallInfo->m_bTranByBLF = bTranByBLF;
#endif

#if IF_BUG_31081
    if (configParser_GetConfigInt(kszTransferUseNewLinekey) == 0)
    {
        pFollowCallInfo->m_bNeedBindDsskey = false;
    }
#endif

    // 呼出号码
    return _TalklogicManager.ProcessCallOut(pFollowSession, callParam.strNumber.c_str(), "");
}

void CSessionStateProxy::ExitPreFWD()
{
    CCallInfo* pCallInfo = GetCallInfo();
    if (pCallInfo == NULL)
    {
        return;
    }

    pCallInfo->SetFWD(false);
}

bool CSessionStateProxy::SetFocus() const
{
    return talklogic_SetFocusSessionByID(*this);
}

void CSessionStateProxy::SetRelationInfo(CALL_RELATION eRelation /*= CR_NONE*/, const yl::string& strRelation /*= ""*/)
{
    CCallInfo* pCallInfo = GetCallInfo();
    if (pCallInfo != NULL)
    {
        pCallInfo->SetRelationInfo(eRelation, strRelation);
    }
}

bool CSessionStateProxy::IsKTSIgnore()
{
    return configParser_GetConfigInt(kszIncomingCallIgnor) != 0
           && _SessionManager.IsAllSessionRinging(true);
}

const yl::string& CSessionStateProxy::GetDialText() const
{
    CCallInfo* pCallInfo = GetCallInfo();
    if (pCallInfo != NULL)
    {
        return pCallInfo->GetDialText();
    }

    /* 待处理 */
    static yl::string str;
    return str;
}
