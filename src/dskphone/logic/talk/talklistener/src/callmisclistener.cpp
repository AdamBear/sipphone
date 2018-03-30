#include "talklistener_inc.h"
#include "listencenter.h"

// 网络异常,退出所有通话的时间
#define EXISTALLSESSION_TIME    5 * 1000

CCallMiscListener::CCallMiscListener()
    : LSN_CLS_LOG_NAME, m_pCableTimer(new CScopedTimer), m_pIdleTimer(new CScopedTimer), m_bCheckIdle(false)
{
    m_pCableTimer->SetCallBack(MAKE_TIMER_CALL_BACK(CCallMiscListener::OnCableTimer));
    m_pIdleTimer->SetCallBack(MAKE_TIMER_CALL_BACK(CCallMiscListener::OnIdleTimer));

    RegETLMsg(SYS_MSG_NOTIFY_NETWORK_STATUS);

    /* 辅助销毁listener内部数据，所以采用异步方式 */
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_AFTER_DELETE, CCallMiscListener::OnSessionDestory);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_CREATE, CCallMiscListener::OnSessioCreate);

    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CCallMiscListener::OnRoutineChanged);
    REG_SYNC_SIGNAL(TALK_FLT_SESSION_INCOME, CCallMiscListener::OnCallIncome);
    REG_SYNC_SIGNAL(TALK_SIG_ACTION_EVENT, CCallMiscListener::OnUIEvent);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_TRY_ENTER_FINISH, CCallMiscListener::OnTryEnterFinish);

    REG_SYNC_SIGNAL(TALK_SIG_PROCESS_CALLOUT, CCallMiscListener::OnTryCallOut);

}

// 是否能够接收来电
bool CCallMiscListener::IsAllowIncomingCall(int iAccountID)
{
#ifdef IF_FEATURE_PSTN
    if (acc_IsPstnAccount(iAccountID))
    {
        return true;
    }
#endif

    if (!talklogic_BeAllowNewCall(NULL, true))
    {
        return false;
    }

    if (AUT_RPS == Autop_GetUpdateType())
    {
        return false;
    }

    int iSize = talklogic_GetSessionNum();
    if (!talkhelp_IsCallWaitingEnable()
            && iSize > 0)
    {
        return (1 == iSize
                && (talklogic_IsSessionExist(ROUTINE_AOC_FINISH)
                    || talklogic_IsSessionExist(ROUTINE_FINISHED)));
    }

    return true;
}

bool CCallMiscListener::OnMessage(msgObject& msg)
{
    //只处理WAN口的状态变化
    if (msg.wParam != SYS_PHY_TYPE_WAN)
    {
        return false;
    }

    if (msg.lParam == SYS_PHY_STATUS_OFF)
    {
        if (configParser_GetConfigInt(kszInvalidNetEndCall) == 1)
        {
            // 网络异常设置5秒退出所有的通话
            m_pCableTimer->SetTimerOnce(EXISTALLSESSION_TIME);
        }
    }
    else if (msg.lParam == SYS_PHY_STATUS_ON)
    {
        // 取消退出所有通话的定时器
        m_pCableTimer->Kill();
    }

    return true;
}

int CCallMiscListener::OnCableTimer(CScopedTimer& timer)
{
    talklogic_ExitAllSession();
    return 1;
}

int CCallMiscListener::OnIdleTimer(CScopedTimer& timer)
{
    bool bIdle = talklogic_GetSessionNum() == 0;

    if (!m_bCheckIdle && !bIdle)
    {
        return modtklsn_emit_ex(TALK_SIG_IDLE_TO_TALK);
    }
    else if (m_bCheckIdle && bIdle)
    {
        return modtklsn_emit_ex(TALK_SIG_TALK_TO_IDLE);
    }

    return 0;
}

DECL_SIGNAL_HANDLER(CCallMiscListener::OnRoutineChanged)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (NULL == pCallInfo)
    {
        return false;
    }

    ROUTINE_STATE eRoutine = (ROUTINE_STATE)sigp.lParam.iValue;
    if (eRoutine == ROUTINE_RINGING)
    {
        int iAccountID = pCallInfo->GetAccountID();
        int iCallID = pCallInfo->GetCallID();

#if IF_BUG_27560
        if (actURI_IsHideTalkInfo())
        {
            // 配置，隐藏来电信息
            pCallInfo->m_bHideInfo = true;
            actURI_SetHideTalkInfo(false);
        }
#endif

        //事先答复SIP，使得来电方可及时进入回铃
        //CallWait开启时要回复182
        if (configParser_GetConfigInt(kszCallWaitReply182) != 0
                && (!talklogic_BeAllowNewCall()
                    || talkhelp_IsCallWaitingEnable()
                    && talklogic_GetSessionNum() > 1)
           )
        {
            modVoip_ResponseIncoming(iAccountID, iCallID, 182);
        }
        else
        {
            modVoip_ResponseIncoming(iAccountID, iCallID, 180);
        }

        // sip协议自己获取远端设备
        if (acc_GetAccountProtocol(iAccountID) != PROTOCOL_H323
                && pCallInfo->m_strUserAgent.empty())
        {
            pCallInfo->m_strUserAgent = modVoip_GetUserAgent(iAccountID, pCallInfo->GetCallID());
        }

        CallInUserInfo& userInfo = pCallInfo->GetRingUserInfo();
        // 设置话路信息
        BasicUserInfo basicUserInfo;
        MapCallInUserInfo_BasicInfo(&userInfo, basicUserInfo);

        pCallInfo->SetLocalInfo(basicUserInfo);
        pCallInfo->SetRemoteInfo(basicUserInfo);

        return true;
    }

    return false;
}

DECL_SIGNAL_HANDLER(CCallMiscListener::OnSessionDestory)
{
    ListenCenter.EraseData(sigp.iSessionID);
    m_pIdleTimer->SetTimerOnce(10);
    m_bCheckIdle = true;

    return true;
}

DECL_SIGNAL_HANDLER(CCallMiscListener::OnSessioCreate)
{
    ListenCenter.GetData(sigp.iSessionID, true);
    m_pIdleTimer->SetTimerOnce(10);
    m_bCheckIdle = false;

    return true;
}

DECL_SIGNAL_HANDLER(CCallMiscListener::OnCallIncome)
{
    if (sigp.lParam.pValue == NULL)
    {
        return false;
    }

    CallInUserInfo& userInfo = *((CallInUserInfo*)sigp.lParam.pValue);
    int iAccountID = sigp.wParam.iValue;

    ///////////// 是否开启Callwaiting ///////////////////
    // 限制通话路数
    if (!IsAllowIncomingCall(iAccountID))
    {
        int iRetCode = 0;

        // 如果未开启Callwaiting且当前已有话路k，先判断是否开启忙转
        if (!modtkflt_emit(TALK_SIG_SAVE_NO_TALK_LOG, sigp))
        {
            TALK_WARN("Callwaiting is Off just refuse incoming call");
            iRetCode = configParser_GetConfigInt(kszNormalRefuseCode);
        }

        SIG_REPLY(iRetCode);
    }

    // 是否拒绝Intercom来电
    if (userInfo.iAutoAnswer == 0
            && configParser_GetConfigInt(kszIntercomAllowance) == 0)
    {
        userInfo.iAutoAnswer = 1;
    }
    return false;
}

DECL_SIGNAL_HANDLER(CCallMiscListener::OnUIEvent)
{
    DataUI2Logic* pActionData = (DataUI2Logic*)sigp.wParam.pValue;
    if (pActionData == NULL || TALK_ACTION_TRAN != pActionData->eAction)
    {
        return false;
    }

    CSessionProxy proxy(sigp.iSessionID);
    if (!proxy.IsInherits(ROUTINE_TALKING))
    {
        return false;
    }

    if (configParser_GetConfigInt(kszEnableDTMFReplaceTran) == 0)
    {
        return false;
    }

    yl::string strTransferDTMF = configParser_GetConfigString(kszTransferDTMF);
    if (strTransferDTMF.empty())
    {
        return false;
    }

    proxy.SendDTMF(strTransferDTMF);
    SIG_REPLY(true);
}

DECL_SIGNAL_HANDLER(CCallMiscListener::OnTryEnterFinish)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();

    if (pCallInfo == NULL)
    {
        return false;
    }

    int iFailReason = sigp.wParam.iValue;
    if (iFailReason == TALK_ERROR_NONE)
    {
        iFailReason = pCallInfo->m_errorInfo.iReason;
    }

    if (iFailReason == TALK_ERROR_NONE
#ifdef IF_FEATURE_PSTN
            && pCallInfo->GetCallLineType() != CALL_LINE_PSTN
#endif // IF_FEATURE_PSTN
            && !netIsNetworkReady())
    {
        iFailReason = TALK_ERROR_CALLOUT_NU;
    }

    //新增配置kszCongestionToneCodeList，区分需要播放congestion tone音的code序列
    yl::string str = configParser_GetConfigString(kszCongestionToneCodeList);
    YLList<yl::string> list;
    if (commonAPI_splitStringBySeparator(str, ',', list)
            && list.size() > 0)
    {
        YLList<yl::string>::iterator iter = list.begin();
        for (; iter != list.end(); iter++)
        {
            yl::string& strCode = *iter;
            if (commonAPI_IsDigital(strCode))
            {
                int iCode = atoi(strCode.c_str());

                if (iFailReason == iCode)
                {
                    iFailReason = TALK_ERROR_CUSTOM_CONGESTION_TONE;
                    break;
                }
            }
        }
    }

    if (iFailReason != TALK_ERROR_NONE)
    {
        SIG_REPLY(iFailReason);
    }

    return false;
}

DECL_SIGNAL_HANDLER(CCallMiscListener::OnTryCallOut)
{
    AUTO_SCOPED_TIMER;

    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    LPCSTR lpszNumber = sigp.strData.c_str();
    LPCSTR lpszName = (LPCSTR)sigp.wParam.pValue;

    if (lpszNumber == NULL || lpszName == NULL)
    {
        SIG_REPLY(ROUTINE_UNKNOWN);
    }

    // 跳到失败界面
    if (!talklogic_BeAllowNewCall(lpszNumber))
    {
        pCallInfo->m_errorInfo.iReason = TALK_ERROR_CALLOUT_MSE;
        SIG_REPLY(ROUTINE_FINISHED);
    }

    // 呼出
#if IF_BUG_27560
    if (actURI_IsHideTalkInfo())
    {
        pCallInfo->m_bHideInfo = true;
        actURI_SetHideTalkInfo(false);
    }
#endif

    int iAccountID = pCallInfo->GetAccountID();
    CallOut_Param& param = pCallInfo->GetCallOutParam();

    bool bIsEmergencyNum = Emergency_IsEmergency(lpszNumber);
    bool bIsEmergencyMode = (bIsEmergencyNum && Emergency_IsEmergencyMode(lpszNumber));

    if (!bIsEmergencyMode && !acc_IsAccountIDValid(iAccountID))
    {
        // 密码拨号功能
        if (configParser_GetConfigInt(kszEnablePSWDial) != 0)
        {
            yl::string strHidedSipNum = lpszNumber;
            RemoveInvalidChar(strHidedSipNum);
            // 将输入隐藏起来
            pCallInfo->m_strNumberBeforeEncrypt = strHidedSipNum;
            pCallInfo->m_tRemoteInfo.sSIPName = talklogic_HidePSW(strHidedSipNum);
            strHidedSipNum = lpszName;
            RemoveInvalidChar(strHidedSipNum);
            pCallInfo->m_tRemoteInfo.sDisplayName = talklogic_HidePSW(strHidedSipNum);
        }

        pCallInfo->m_errorInfo.iReason = TALK_ERROR_CALLOUT_IA;
        SIG_REPLY(ROUTINE_FINISHED);
    }

    bool bAuthCall = false;
    yl::string strAuthCallNum;
    if (!bIsEmergencyMode
            && acc_GetAccountProtocol(iAccountID) == PROTOCOL_SIP
            && configParser_GetCfgItemIntValue(kszAccountAuthCallSwitch, iAccountID) != 0)
    {
        // http://192.168.1.99/Bug.php?BugID=23124
        strAuthCallNum = configParser_GetCfgItemStringValue(kszAccountAuthCallNumber, iAccountID);
        if (!strAuthCallNum.empty())
        {
            lpszNumber = strAuthCallNum.c_str();
            lpszName = lpszNumber;
            bAuthCall = true;
        }
    }

    bool bNativeNum = param.eCallOutType == TCT_CALL_CENTER;
    bNativeNum |= bAuthCall;

    yl::string strFullName = lpszNumber;
    yl::string strExtNumber;
    // 格式化呼出号码,并设置到pCallInfo中
    if (!FormatAccountAndNumber(pCallInfo, strFullName, strExtNumber, lpszName))
    {
        pCallInfo->m_errorInfo.iReason = TALK_ERROR_CALLOUT_IA;
        SIG_REPLY(ROUTINE_FINISHED);
    }

    if (strFullName.empty()
            && strExtNumber.empty())
    {
        // http://10.2.1.199/Bug.php?BugID=96953
        pCallInfo->m_errorInfo.iReason = TALK_ERROR_CALLOUT_IN;
        SIG_REPLY(ROUTINE_FINISHED);
    }

    // 密码拨号功能
    if (configParser_GetConfigInt(kszEnablePSWDial) != 0
#ifdef IF_FEATURE_PSTN
            && pCallInfo->GetCallLineType() != CALL_LINE_PSTN
#endif // IF_FEATURE_PSTN
       )
    {
        yl::string strHidedSipNum = pCallInfo->m_tRemoteInfo.sSIPName;
        if (!bNativeNum)
        {
            RemoveInvalidChar(strHidedSipNum);
        }

        // 将输入隐藏起来
        pCallInfo->m_strNumberBeforeEncrypt = strHidedSipNum;
        pCallInfo->m_tRemoteInfo.sSIPName = talklogic_HidePSW(strHidedSipNum);
        strHidedSipNum = pCallInfo->m_tRemoteInfo.sDisplayName;

        if (!bNativeNum)
        {
            RemoveInvalidChar(strHidedSipNum);
        }

        pCallInfo->m_tRemoteInfo.sDisplayName = talklogic_HidePSW(strHidedSipNum);
    }

    // 判断是否限制呼出的号码，紧急呼叫号码不限制
    if (!bAuthCall && !bIsEmergencyNum
            && dialplan_IsBlockCallout(lpszNumber, iAccountID, &param))
    {
        pCallInfo->m_errorInfo.iReason = TALK_ERROR_CALLOUT_FN;
        SIG_REPLY(ROUTINE_FINISHED);
    }

    CallOutInfo* pCallOutInfo = new CallOutInfo(pCallInfo, m_listDelayInvite);

#ifdef IF_FEATURE_EMERGENCY
    if (bIsEmergencyMode)
    {
        pCallOutInfo->m_iMask |= (SIP_CALL_MASK_EMERGENCY | SIP_CALL_MASK_E911_CALL);
        pCallOutInfo->m_iEmergencyIndex = Emergency_GetPosIndex(lpszNumber);
        Emergency_StartWindows();
    }
#endif

    if (TCT_TIPTEL_AUTO_CALLOUT == pCallInfo->m_eTalkCallOutType)
    {
        // tiptel服务器控制话机呼出
        strFullName = strFullName + ";Event:call_invoke;" + TiptelA_GetInvokeID();
    }

    // 设置呼出参数
    pCallOutInfo->m_strNumber = strFullName;
    // 呼入会议室
    pCallOutInfo->m_strExtNumber = strExtNumber;

    // CC不去订阅紧急呼叫号码
    if (!(bIsEmergencyNum && keyGuard_GetKeyGuardType() == KG_ALL_KEYS))
    {
        // 订阅Call Completion.
        CallCompletion_Subscibe(
            iAccountID,
            pCallInfo->GetCallID(),
            pCallInfo->m_tRemoteInfo.sDisplayName.c_str(),
            pCallInfo->m_tRemoteInfo.sSIPName.c_str(),
            pCallInfo->m_tRemoteInfo.sServerName.c_str());
    }

#ifdef IF_SUPPORT_BLUETOOTH
// 通知蓝牙设备呼出
    Bluetooth_Callout(strFullName.c_str());
#endif // IF_SUPPORT_BLUETOOTH

    SIG_REPLY(ROUTINE_CONNECTING);
}

// 处理呼出号码和账号
bool CCallMiscListener::FormatAccountAndNumber(CCallInfo* pCallInfo,
        yl::string& strFullName, yl::string& strExtNumber, LPCSTR lpszName /*= ""*/)
{
    if (pCallInfo == NULL || lpszName == NULL)
    {
        return false;
    }

    int iAccountID = pCallInfo->GetAccountID();
    CallOut_Param& param = pCallInfo->GetCallOutParam();

    bool bNativeNum = param.eCallOutType == TCT_CALL_CENTER;

    // strFullName：传递给协议层的号码
    TALK_INFO("FormatAccountAndNumber(CallInfo=%p, strFullName=%s, lpszName=%s", pCallInfo, strFullName.c_str(), lpszName);

    // sip ip直播账号
    int iSipP2PAccount = acc_GetDirectIPAccountID();
    SipAccountType eAccountType = acc_GetSipAccountType(iAccountID);

    // strUser :用于存储历史记录
    yl::string strUser = strFullName;
    yl::string strServer = "";
    int iUnregAccount = INVALID_ACCOUNTID;

    if (acc_IsAccountIDValid(iAccountID))
    {
        strServer = acc_GetServerName(iAccountID);

        // 是否为未注册上账号的呼出, 是则保留指针, (无账号呼出)
        iUnregAccount = (!acc_IsAccountAvailable(iAccountID) ? iAccountID : INVALID_ACCOUNTID);
    }

#ifdef IF_FEATURE_EMERGENCY
    bool bEmergencyMode = Emergency_IsEmergencyMode(strUser.c_str());
#else
    bool bEmergencyMode = false;
#endif //IF_FEATURE_EMERGENCY

#ifdef IF_FEATURE_EMERGENCY
    if (bEmergencyMode && acc_IsDirectIPAccount(iAccountID))
    {
        iAccountID = acc_GetEmergencyAccount();
        pCallInfo->SetAccountID(iAccountID);
    }
#endif //IF_FEATURE_EMERGENCY

    // 号码和名字是否一致
    bool bNameSameAsNum = (strFullName == lpszName);
    bool bNumChanged = false;

    bool bNeedRemoveServerPort = true;
    yl::string strDefaultServerPort = ":5060";

    //输入的名称中，不带默认端口号才需要替换
    if (strFullName.rfind(strDefaultServerPort) != yl::string::npos)
    {
        bNeedRemoveServerPort = false;
    }

    TALK_INFO("account[%d] type[%d] number[%s] ext[%s] server[%s]", iAccountID,
              eAccountType, strFullName.c_str(), strExtNumber.c_str(), strServer.c_str());

    if (strFullName.find("@") == yl::string::npos)
    {
        yl::string strIP = strUser;
        string_replace(strIP, "*", ".");
        bool bIsValidIP = Comm_IsValidIP(strIP.c_str());

        // 非IP直播则替换号码后呼出
        if (!bIsValidIP)
        {
            bNumChanged |= dialplan_GetReplaceNumber(strUser, iAccountID, &param);
        }

        if (bIsValidIP || acc_IsAccountIDValid(iUnregAccount))
        {
            if (!bIsValidIP &&
                    !bEmergencyMode && configParser_GetConfigInt(kszForbiddenUnregisterdAccount) == 1)
            {
                return false;
            }

            iAccountID = SelectAccount(bEmergencyMode);
            pCallInfo->SetAccountID(iAccountID);

            // 紧急呼叫可以不依赖账号状态
            if (!bEmergencyMode && !acc_IsAccountAvailable(iAccountID))
            {
                return false;
            }

            if (bIsValidIP)
            {
                // 输入的号码带端口号肯定是非法ip，肯定不是此处处理
                // V80给sip的服务器可以不带5060
                strUser = strIP;
                strServer = strIP;
            }
        }
        else
        {
            // 不允许用IP直拨账号呼出非IP号码，将strServer置为空可以防止呼出本机
            if (acc_IsDirectIPAccount(iAccountID))
            {
                // http://10.2.1.199/Bug.php?BugID=91410
                if (!bEmergencyMode && configParser_GetConfigInt(kszForbiddenUnregisterdAccount) == 1)
                {
                    return false;
                }

                strServer = "";
            }
        }

        // 将号码中的非法字符去掉
        yl::string strTmp = strUser;
        if (!bNativeNum)
        {
            bNumChanged |= RemoveInvalidChar(strTmp);
        }

        // http://10.2.1.199/Bug.php?BugID=57048
        if (bIsValidIP)
        {
            strFullName = strTmp;
        }
        else
        {
#if IF_BUG_26015
            if (!bEmergencyMode && acc_IsDirectIPAccount(iAccountID))
            {
                // 限制使用IP直播账号呼出非IP地址
                return false;
            }
#endif

            // 普通号码呼出是不带服务器名的，如果是联系人or历史记录呼出，需外部传入服务器
            strFullName = strTmp;

            // http://192.168.1.99/Bug.php?BugID=29259
            // 最终使用IP直播账号呼出非IP地址（普通号码），需要传给SIP完整URI(sip:NUMBER@SERVER)
            if (acc_IsDirectIPAccount(iAccountID))
            {
                // 若为未注册账号呼出且该账号具备服务器名，则优先取其服务器名
                // 否则取第一个具备服务器名的账号的服务器名
                if (acc_IsAccountIDValid(iUnregAccount) && !acc_GetServerName(iUnregAccount).empty())
                {
                    strServer = acc_GetServerName(iUnregAccount);
                }
                // 免注册的云账号不需要此处补全Server
                else if (!modVoip_NeedDirectIPCloudAccount(eAccountType))
                {
                    int iFirstSIPServerAccount = acc_GetNoEmptyServerAccountID();
                    if (acc_IsAccountIDValid(iFirstSIPServerAccount))
                    {
                        strServer = acc_GetServerName(iFirstSIPServerAccount);
                    }
                }

#ifdef IF_FEATURE_EMERGENCY
                if (bEmergencyMode && strServer.empty())
                {
                    netGetWanIP(strServer);
                }
#endif //IF_FEATURE_EMERGENCY

                if (!strServer.empty())
                {
                    strFullName = strTmp + "@" + strServer;
                }
            }
            else if ((acc_IsNoNeedRegisterAccount(iAccountID)
                      || modVoip_NeedDirectIPCloudAccount(eAccountType))
                     && !strServer.empty())
            {
                // 免注册云账号加上域名呼出
                strFullName = strTmp + "@" + strServer;
            }
        }
    }
    else
    {
        // 处理带@呼出:AA@BB.com
        if (acc_IsAccountIDValid(iUnregAccount))
        {
            if (!bEmergencyMode && configParser_GetConfigInt(kszForbiddenUnregisterdAccount) == 1)
            {
                return false;
            }

            iAccountID = SelectAccount(bEmergencyMode);
            pCallInfo->SetAccountID(iAccountID);
        }

        if (!bEmergencyMode && !acc_IsAccountIDValid(iAccountID))
        {
            return false;
        }

        yl::string strNum = "";
        yl::string strDialServer = "";
        commonUnits_ParserDialURI(strFullName, strNum, strDialServer);
        TALK_INFO("Dial URI: strNum=%s, strServer=%s", strNum.c_str(), strServer.c_str());

        bool bIsValidIP = Comm_IsValidIP(strNum.c_str());
        if (bIsValidIP)
        {
            iAccountID = SelectAccount(bEmergencyMode);
            pCallInfo->SetAccountID(iAccountID);

            if (!bEmergencyMode && !acc_IsAccountIDValid(iAccountID))
            {
                return false;
            }

            // 默认端口为5060,V80不需要GUI添加5060
            strUser = strNum;
            strServer = strUser;

            strFullName = strUser + "@" + strServer;
        }
        else
        {
#if IF_BUG_26015
            if (!bEmergencyMode && acc_IsDirectIPAccount(iAccountID))
            {
                // 限制使用IP直播账号呼出非IP地址
                return false;
            }
#endif

            if (strNum[0] != '\0' && strDialServer[0] != '\0')
            {
                strFullName = strDialServer;
                string_replace(strFullName, "*", ".");

                ////http://192.168.1.99/Bug.php?BugID=31412
                ////服务器未带上端口则手动添加
                //if (strFullName.find(":") == yl::string::npos
                //  && !modVoip_NeedDirectIPCloudAccount(eAccountType))
                //{
                //  // 云账号呼出，使用账号自己的端口，GUI不需要设置
                //  strFullName = commonAPI_FormatString("%s:%d", strFullName.c_str(), acc_GetServerPort(iAccountID));
                //}

                // 将号码中的非法字符去掉
                yl::string strTmp = strNum;
                //http://192.168.1.99/Bug.php?BugID=25537
                bNumChanged |= dialplan_GetReplaceNumber(strTmp, iAccountID, &param);

                // 处理E164格式，如果是2-15位数字，则要带上加号（前2位代表国家代号，因此一定是大于2位；15位是协议规定）
                if (configParser_GetConfigInt(kszE164AddGlobalPrefix) != 0)
                {
                    FormatE164Number(strTmp);
                }

                // 如果服务器名A与账户注册的服务器不一致则带上服务器A
                if (strServer != strDialServer)
                {
                    // http://192.168.1.99/Bug.php?BugID=19142
                    strUser = strTmp + "@" + strFullName;
                }
                else
                {
                    strUser = strTmp;
                }

                strFullName = strTmp + "@" + strFullName;
            }
            else if (!bNativeNum)
            {
                // 将号码中的非法字符去掉
                bNumChanged |= RemoveInvalidChar(strFullName);
            }
        }
    }

    if (acc_IsAccountIDValid(iUnregAccount))
    {
        pCallInfo->m_iUnregSipAccount = iUnregAccount;
    }

    // 更新通话信息
    pCallInfo->m_eCallType = CT_OUTGOING;
    yl::string strTmp = strUser;

    if (!bNativeNum)
    {
        bNumChanged |= RemoveInvalidChar(strTmp);
    }

    //需要去除端口号的显示 http://bugfree.yealink.com/Bug.php?BugID=101718
    if (bNeedRemoveServerPort)
    {
        string_replace(strTmp, strDefaultServerPort, "");
    }

    pCallInfo->m_tRemoteInfo.sServerName = strServer;

    if (!acc_IsDirectIPAccount(iAccountID)
            && pCallInfo->m_tRemoteInfo.sServerName != acc_GetServerName(iAccountID))
    {
        TALK_DBG("apend server [%d][%s]", iAccountID, pCallInfo->m_tRemoteInfo.sServerName.c_str());

        // 非IP直拨账号且服务器与账号注册服务器不一致，则显示全名（带服务器）
        if (strFullName.substr(0, 4) == "sip:")
        {
            pCallInfo->m_tRemoteInfo.sSIPName = strFullName.substr(4);
            size_t iPortPos = pCallInfo->m_tRemoteInfo.sSIPName.rfind(':');
            if (iPortPos != yl::string::npos)
            {
                pCallInfo->m_tRemoteInfo.sSIPName = pCallInfo->m_tRemoteInfo.sSIPName.substr(0, iPortPos);
            }
        }
        else
        {
            pCallInfo->m_tRemoteInfo.sSIPName = strFullName;
        }
    }
    else if (!strExtNumber.empty())
    {
        pCallInfo->m_tRemoteInfo.sSIPName = strTmp + "##" + strExtNumber;
    }
    else
    {
        // 否则只显示SIP名称
        pCallInfo->m_tRemoteInfo.sSIPName = strTmp;
    }

    if (bNumChanged)
    {
        // 替换规则发生后，号码被替换了，默认名字也需要跟着改变
        if (bNameSameAsNum)
        {
            pCallInfo->m_tRemoteInfo.sDisplayName = strTmp;
        }
        else
        {
            pCallInfo->m_tRemoteInfo.sDisplayName.clear();
        }
    }
    else
    {
        pCallInfo->m_tRemoteInfo.sDisplayName = lpszName;
    }

    // 设置联系人信息,T4X需要查找头像
    pCallInfo->SetContactInfo(pCallInfo->m_tRemoteInfo.sDisplayName.empty());

    TALK_INFO("DisplayName=%s, FromSIPName=%s, FullContactInfo=%s, ServerName=%s, SIPName=%s",
              pCallInfo->m_tRemoteInfo.sDisplayName.c_str(), pCallInfo->m_tRemoteInfo.sFromSIPName.c_str(),
              pCallInfo->m_tRemoteInfo.sFullContactInfo.c_str(), pCallInfo->m_tRemoteInfo.sServerName.c_str(),
              pCallInfo->m_tRemoteInfo.sSIPName.c_str());

    return true;
}

void CCallMiscListener::FormatE164Number(yl::string& strNumber)
{
    // 处理E164格式，如果是2-15位数字，则要带上加号（前2位代表国家代号，因此一定是大于2位；15位是协议规定）
    // 替换号码
    char tempBuf[1024];
    int iRet = regExp_Replace("(\\d{2,15})$", strNumber.c_str(), "+$1", tempBuf, 1024);
    if (0 == iRet)
    {
        strNumber = tempBuf;
    }

    TALK_INFO("format e164[%s]", strNumber.c_str());
}

int CCallMiscListener::SelectAccount(bool bEmergencyCall)
{
    int iAccount = -1;
#ifdef IF_FEATURE_EMERGENCY
    if (bEmergencyCall)
    {
        iAccount = acc_GetEmergencyAccount();
    }
    else
#endif //IF_FEATURE_EMERGENCY
    {
        // 如果不是无账号呼出则将账号切为IP直拨账号
        // 将Sip账号切换为IP直拨账号
        iAccount = acc_GetDirectIPAccountID();
    }

    return iAccount;
}
