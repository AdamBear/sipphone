#include "talklistener_inc.h"

CFWDDNDListener::CFWDDNDListener()
    : LSN_CLS_LOG_NAME
{
    REG_SYNC_SIGNAL(TALK_SIG_IDLE_TO_TALK, CFWDDNDListener::OnIdleToTalk);
    REG_SYNC_SIGNAL(TALK_SIG_TALK_TO_IDLE, CFWDDNDListener::OnTalkToIdle);
    REG_SYNC_SIGNAL(TALK_FLT_SESSION_INCOME, CFWDDNDListener::OnCallIncome);
    REG_SYNC_SIGNAL(TALK_FLT_SESSION_REFUSE, CFWDDNDListener::OnCallRefuse);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_REJECT, CFWDDNDListener::OnCallReject);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CFWDDNDListener::OnRoutineChanged);
    REG_SYNC_SIGNAL(TALK_SIG_ACTION_EVENT, CFWDDNDListener::OnUIAction);
    REG_SYNC_SIGNAL(TALK_SIG_SAVE_NO_TALK_LOG, CFWDDNDListener::OnSaveLog);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_FWD, CFWDDNDListener::OnSessionFWD);
    REG_SYNC_SIGNAL(TALK_SIG_RECV_PAGE, CFWDDNDListener::OnPageReceived);

}

DECL_SIGNAL_HANDLER(CFWDDNDListener::OnIdleToTalk)
{
    // 通话中DND功能：通话后重置DND状态则需要先备份
    if (configParser_GetConfigInt(kszKeepDNDAfterTalk) == 0)
    {
        dnd_Backup();
    }
    return true;
}

DECL_SIGNAL_HANDLER(CFWDDNDListener::OnTalkToIdle)
{
    // 通话中DND功能：通话后重置DND状态
    if (configParser_GetConfigInt(kszKeepDNDAfterTalk) == 0)
    {
        dnd_Restore();
    }
    return true;
}

DECL_SIGNAL_HANDLER(CFWDDNDListener::OnCallIncome)
{
    if (sigp.lParam.pValue == NULL || sigp.tParam.iValue != CALL_LINE_SIP)
    {
        return false;
    }

    CallInUserInfo& userInfo = *((CallInUserInfo*)sigp.lParam.pValue);
    int iAccountID = sigp.wParam.iValue;
    int iCallID = sigp.iCallID;

    const yl::string& strRemoteNumber = userInfo.strEmergFwdTarget;

    // 是否允许Always FWD, for broadsoft
    if (userInfo.bIsFwdEnable
            && !Emergency_IsForwardEmergency(strRemoteNumber.c_str())
            && (!feature_IsForwardSync() || feature_IsForwardLocalProcess(iAccountID))
            && fwd_GetInfo(iAccountID, FWD_TYPE_ALWAYS))
    {
#if IF_FEATURE_LOOPBACK
        // loopback 通话 自动应答开启 不拒接
        if (userInfo.bIsLoopBackCall
                && configParser_GetConfigInt(kszLoopBackAAMode) == 1)
        {
            SIG_REPLY(false);
        }
#endif
        // 如果开启Always FWD,则转移新来电，记录呼叫转移记录
        bool bSaveCalllog = Comm_SaveFwdCallList(iAccountID, &userInfo, FWD_TYPE_ALWAYS);
        if (!(DEVICELIB_IS_CP(devicelib_GetPhoneType())) || bSaveCalllog) // CP根据是否保存Calllog来决定是否弹框
        {
            PopFwdMessageBox(iAccountID, FWD_TYPE_ALWAYS);
        }
        // T49当界面处于通知中心时，需要先保存calllog，再刷新UI才能刷新正确
        ForwardIncomingCall(iAccountID, iCallID, FWD_TYPE_ALWAYS);

        SIG_REPLY(configParser_GetConfigInt(kszFWDRefuseCode));
    }

    // DND开启，而且 未启用同步DND功能或者已启用本地处理DND
    bool bDNDUsable = dnd_IsEnable(iAccountID) && (!feature_IsDNDSync() || feature_IsDNDLocalProcess(iAccountID));
    // DND生效后，还需要判断是否是紧急来电
    if (bDNDUsable && !Emergency_IsIncomingEmergency(strRemoteNumber.c_str()))
    {
        //如果设置了DND,则直接拒绝通话. !!此处包含了遇忙转移
        TALK_WARN("Refuese incoming call (Forbiden DND:%d)", bDNDUsable);
        int iRetCode = 0;
        if (!SaveNoTalkingCallLog(iAccountID, userInfo, iCallID, bDNDUsable))
        {
            iRetCode = configParser_GetConfigInt(kszDNDRefuseCode);
        }

        SIG_REPLY(iRetCode);
    }
    return true;
}

DECL_SIGNAL_HANDLER(CFWDDNDListener::OnCallRefuse)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }

    yl::string strTarget;
    const yl::string& strRemoteNumber = pCallInfo->GetRingUserInfo().strEmergFwdTarget;

    if (Emergency_IsForwardEmergency(strRemoteNumber.c_str())
            || !fwd_GetInfo(pCallInfo->GetAccountID(), FWD_TYPE_BUSY, &strTarget)
#if IF_BUG_23511_SYNC_IGNORE_LOCAL_DNDFWD
            || feature_IsForwardSync() && !feature_IsForwardLocalProcess(pCallInfo->GetAccountID())
#endif //IF_BUG_23511_SYNC_IGNORE_LOCAL_DNDFWD
       )
    {
        return false;
    }

    priv->m_NoAnwFwdProxy.ForwardIncomingCall(strTarget.c_str(), FWD_TYPE_BUSY);
    SIG_REPLY(true);
}

DECL_SIGNAL_HANDLER(CFWDDNDListener::OnCallReject)
{
    CSessionProxy sp(sigp.iSessionID);
    CCallInfo* pCallInfo = sp.GetCallInfo();
    if (!sp || pCallInfo == NULL || priv == NULL)
    {
        return false;
    }
    // 拒接忙转
    if (feature_IsForwardSync() && !feature_IsForwardLocalProcess(pCallInfo->GetAccountID()))
    {
        // 同步忽略本地DND
        return true;
    }

    const yl::string& strRemoteNumber = pCallInfo->GetRingUserInfo().strEmergFwdTarget;

    if (Emergency_IsForwardEmergency(strRemoteNumber.c_str()))
    {
        // 紧急号码穿透不进行忙转
        return true;
    }
    yl::string strTarget;
    if (!fwd_GetInfo(pCallInfo->GetAccountID(), FWD_TYPE_BUSY, &strTarget))
    {
        // 未启用忙转
        return true;
    }

    priv->m_NoAnwFwdProxy.ForwardIncomingCall(strTarget.c_str());
    SIG_REPLY(true);
}

DECL_SIGNAL_HANDLER(CFWDDNDListener::OnRoutineChanged)
{
    CSessionProxy sp(sigp.iSessionID);
    CCallInfo* pCallInfo = sp.GetCallInfo();

    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }
    else if (sp.GetRoutineState() != ROUTINE_RINGING)
    {
        priv->m_NoAnwFwdProxy.Kill();
        return false;
    }

    CallInUserInfo& userInfo = pCallInfo->GetRingUserInfo();
    pCallInfo->m_strDiversionNumber = userInfo.strFwdNumber;

    // RING状态处理No Answer Forward
    int iAccountID = pCallInfo->GetAccountID();
    if (feature_IsForwardSync() && !feature_IsForwardLocalProcess(iAccountID))
    {
        return true;
    }

    priv->m_NoAnwFwdProxy.SetFWDTimer(pCallInfo);
    return true;
}

DECL_SIGNAL_HANDLER(CFWDDNDListener::OnUIAction)
{
    CSessionProxy sp(sigp.iSessionID);
    CCallInfo* pCallInfo = sp.GetCallInfo();
    DataUI2Logic* pDataUI2Logic = (DataUI2Logic*)sigp.wParam.pValue;

    if (pCallInfo == NULL
            || priv == NULL
            || pDataUI2Logic == NULL
            || !sp.IsRinging())
    {
        return false;
    }

    bool bRet = true;
    switch (pDataUI2Logic->eAction)
    {
    case TALK_ACTION_SEND:
        {
            priv->m_NoAnwFwdProxy.ForwardIncomingCall(pCallInfo->GetDialText().c_str());
        }
        break;
    case TALK_ACTION_FWD_TO:
        {
#if IF_FEATURE_FORBIDDEN_FWD_TRAN
            if (configParser_GetConfigInt(kszForwardForbidden) == 1)
            {
                bRet = false;
                break;
            }
#endif //IF_FEATURE_FORBIDDEN_FWD_TRAN
#if IF_FEATURE_FORBIDDEN_TRAN_IN_RING
            if (configParser_GetConfigInt(kszForwardForbiddenInRinging) == 1)
            {
                bRet = false;
                break;
            }
#endif
            pCallInfo->SetFWD(true);
        }
        break;
    case TALK_ACTION_CANCEL:
        {
            if (sp.IsSingle())
            {
                if (pCallInfo->IsPreForword())
                {
                    pCallInfo->SetFWD(false);
                }
            }
        }
        break;
    default:
        bRet = false;
        break;
    }
    return bRet;
}

DECL_SIGNAL_HANDLER(CFWDDNDListener::OnSaveLog)
{
    if (sigp.lParam.pValue == NULL)
    {
        return false;
    }

    CallInUserInfo& userInfo = *((CallInUserInfo*)sigp.lParam.pValue);
    int iAccountID = sigp.wParam.iValue;

    if (SaveNoTalkingCallLog(iAccountID, userInfo, sigp.iCallID))
    {
        SIG_REPLY(true);
    }

    return false;
}

DECL_SIGNAL_HANDLER(CFWDDNDListener::OnSessionFWD)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    yl::string strTarget = sigp.strData;
    // 不允许forward到lpszNumber则不做处理
    if (!fwd_IsAllowForwardTo(strTarget.c_str()))
    {
        return false;
    }

    // 根据替换规则替换转移号码
    dialplan_GetReplaceNumberEx(strTarget, pCallInfo->GetAccountID(), DPCM_FWD);

    // 通话类型设置为CT_FORWARDCALL
    pCallInfo->m_eCallType = CT_FORWARDCALL;

    pCallInfo->m_eCallRelation = CR_FWD_TO;
    pCallInfo->m_strRelationNum = strTarget;

    SignalParams sigp1(pCallInfo, strTarget);
    modtklsn_emit_sync(TALK_SIG_SESSION_SAVE_LOG, sigp1);

    pCallInfo->m_strForwardTarget = strTarget;
    return true;
}

DECL_SIGNAL_HANDLER(CFWDDNDListener::OnPageReceived)
{
    int iPriority = sigp.wParam.iValue;

    // 目前PHone模式下的DND才能拒绝Paging
    bool bDNDUsable = (dnd_GetMode() == FWDDND_MODE_GLOBAL && dnd_IsEnable(FWDDND_ACCOUNT_GLOBAL));
    if (bDNDUsable && iPriority > configParser_GetConfigInt(kszMulticastIPIgnorePriority))
    {
        // 如果设置了DND,则直接拒绝通话
        TALK_WARN("Refuese incoming call(DND) iPriority [%d] [%d]",
                  iPriority, configParser_GetConfigInt(kszMulticastIPIgnorePriority));
        SIG_REPLY(RCR_WAIT);
    }

    return false;
}

bool CFWDDNDListener::SaveNoTalkingCallLog(int iAccount, CallInUserInfo& userInfo, int iCallID, bool bIsDND /*= false*/)
{
    if (!acc_IsAccountIDValid(iAccount))
    {
        return false;
    }

    const yl::string& strRemoteNumber = userInfo.strEmergFwdTarget;

    if (Emergency_IsForwardEmergency(strRemoteNumber.c_str())
            || !ForwardIncomingCall(iAccount, iCallID, FWD_TYPE_BUSY)) // 遇忙转移
    {
        // MissCallType为1时，不记录DND和通话中CallWaiting未开启的MissCall;
        if (1 != configParser_GetConfigInt(kszMissCallType))
        {
            //将Missed Call保存到CallList
            Comm_SaveToCallList(iAccount, &userInfo, iCallID, CT_MISSCALL);
        }
        else
        {
            TALK_INFO("SaveNoTalkingCallLog: MissCallType = [1], Do not log misscall!");
        }

        // 拒接来电
        modVoip_RefuseCall(iAccount, iCallID, (bIsDND ? RT_DND : RT_NORMAL));
        return false;
    }
    else
    {
        int iFwdType = CT_FORWARDCALL;
        //将Forward Call保存到CallList
        if (Comm_SaveFwdCallList(iAccount, &userInfo, FWD_TYPE_BUSY))
        {
            PopFwdMessageBox(iAccount, FWD_TYPE_BUSY);
            ForwardIncomingCall(iAccount, iCallID, FWD_TYPE_BUSY);
        }

        return true;
    }
}

BOOL CFWDDNDListener::ForwardIncomingCall(int iLineId, int iCallID, FWD_TYPE eFwdType)
{
    if (feature_IsForwardSync() && !feature_IsForwardLocalProcess(iLineId))
    {
        // 同步则不需要本地处理
        return FALSE;
    }
    yl::string strNumber;
    if (!fwd_GetInfo(iLineId, eFwdType, &strNumber))
    {
        return FALSE;
    }
    // 根据替换规则替换转移号码
    dialplan_GetReplaceNumberEx(strNumber, iLineId, DPCM_FWD);

    // 发送fwd消息
    modVoip_RequestForward(iLineId, iCallID, strNumber.c_str(), eFwdType);

    if (!(DEVICELIB_IS_CP(devicelib_GetPhoneType())))
    {
        yl::string strName;
        Dir_GetCotactNameByNumber(strNumber, strName);
        if (strName.empty())
        {
            strName = strNumber;
        }
        TALK_INFO("ForwardIncomingCall %s", strName.c_str());
        // 设备提示
        idleScreen_SetStatus(PS_STATE_CALLFORWARD, strName);
    }

    return true;
}

// 弹出Fwd的提示框
void CFWDDNDListener::PopFwdMessageBox(int iLineId, FWD_TYPE eFwdType)
{
    yl::string strTarget;
    fwd_GetInfo(iLineId, eFwdType, &strTarget);

    // 根据替换规则替换转移号码
    dialplan_GetReplaceNumberEx(strTarget, iLineId, DPCM_FWD);

    yl::string strName;
    Dir_GetCotactNameByNumber(strTarget, strName);

    if (strName.empty())
    {
        strName = strTarget;
    }

    // 设备提示
    idleScreen_SetStatus(PS_STATE_CALLFORWARD, strName);
}
