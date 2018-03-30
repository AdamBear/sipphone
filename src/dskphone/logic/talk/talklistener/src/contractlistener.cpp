#include "talklistener_inc.h"
#include "talk/talkasync/src/msghandler.h"
#include "presence/include/modpresence.h"

extern bool g_bAfterInit;

// 替换字符FAC
static bool ReplaceString(yl::string& strValue, const char* szName, yl::string strReplaceValue,
                          int nLineID = -1)
{
    yl::string strBsftFAC;
    if (nLineID == -1)
    {
        strBsftFAC = configParser_GetConfigString(szName);
    }
    else
    {
        strBsftFAC = configParser_GetCfgItemStringValue(szName, nLineID);
    }

    if (!strBsftFAC.empty()
            && commonAPI_StringMatch(strValue, strBsftFAC, true, LEAD_MATCH))
    {
        if (strValue.compare(strBsftFAC.c_str()) != 0)
        {
            strReplaceValue += ": ";
        }

        strValue.replace(strBsftFAC.c_str(), strReplaceValue);
        return true;
    }
    return false;
}

// 替换Brosftsoft里的特征码显示
static bool ReplaceBsftFAC(CCallInfo* pCallInfo)
{
    if (pCallInfo == NULL)
    {
        return false;
    }

    if (configParser_GetConfigInt(kszHideFeatureAccessCodes) == 0)
    {
        return false;
    }

    int nLineID = pCallInfo->GetAccountID();
    yl::string& strValue = pCallInfo->m_tRemoteInfo.sSIPName;
    yl::string strFefValue = strValue;

    if (ReplaceString(strValue, kszCallParkCode, kszReplaceCallParkCode)
            || ReplaceString(strValue, kszRetriveParkCode, kszReplaceCallRetriveCode)
            || ReplaceString(strValue, kszGroupCallParkCode, kszReplaceGroupParkCode)
            || ReplaceString(strValue, kszGPickupCode, kszReplaceGPickUpCode)
            || ReplaceString(strValue, kszAccountGPickupCode, kszReplaceGPickUpCode, nLineID)
            || ReplaceString(strValue, kszDPickupCode, kszReplaceDPickUpCode)
            || ReplaceString(strValue, kszAccountDPickupCode, kszReplaceDPickUpCode, nLineID)
            || ReplaceString(strValue, kszAccountVoiceMailNumber, kszReplaceVoiceMailCode, nLineID)
            || ReplaceString(strValue, kszBLFListCode, kszReplaceDPickUpCode, nLineID)
            || ReplaceString(strValue, kszBLFListBargeInCode, kszReplaceBargeInCode, nLineID)
            || ReplaceString(strValue, kszBLFRetrieveCallPark, kszReplaceCallRetriveCode, nLineID)
            || ReplaceString(strValue, kszAccountCallPullFeatureCode, kszReplaceCallPullCode, nLineID)
#if IF_BUG_23548
            || ReplaceString(strValue, kszInitiateCallback, kszReplaceCallBackCode)
#endif
       )
    {
        pCallInfo->m_strFeatureAccessCodes = strFefValue;
        return true;
    }

    return false;
}

static void GetPhoto(yl::string& strPhoto, const yl::string& strNumber,
                     const yl::string& strName)
{
    if (strPhoto.find(TEMP_PATH "SIPPic") != yl::string::npos)
    {
        return;
    }

    if (strPhoto.empty())
    {
        strPhoto = BTDir_GetContactImageByNumber(strNumber, strName);
    }

    TALK_INFO("Photo number[%s], name[%s]", strNumber.c_str(), strName.c_str());
    if (strPhoto.empty())
    {
        strPhoto = LocalDir_GetContactImageByNumber(strNumber, strName);
    }

#if FEATURE_BROADTOUCH_UC
    if (strPhoto.empty())
    {
        // 取UI头像
        Presence_GetImageByNumber(strPhoto, strNumber);
    }
#endif
    if (strPhoto.empty()
            || strPhoto.find_first_of("/") != yl::string::npos)
    {
        // 空或者是全路径直接返回
        return;
    }

    // 隐藏路径的格式则转换为全路径
    commonUnits_Hide2Real(strPhoto, PHFILE_TYPE_CONTACT_IMAGE);
}

CContractListener::CContractListener()
    : LSN_CLS_LOG_NAME
{
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CContractListener::OnRoutineChanged);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_CONTRACT_CHANGED, CContractListener::OnCallInfoChange);

    REG_SYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CContractListener::OnRoutineChange);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CContractListener::OnSessionDestory);
    REG_SYNC_SIGNAL(TALK_FLT_SESSION_INCOME, CContractListener::OnCallIncome);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_SAVE_LOG, CContractListener::SaveCallLog);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_CALLID_CHANGE, CContractListener::OnCallIDChanged);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_TRANSFER, CContractListener::OnTransfer);
    REG_SYNC_SIGNAL(TALK_SIG_SUB_SESSION_ACTIVED, CContractListener::OnSubActived);

#ifdef IF_FEATURE_PSTN
    RegETLMsg(PSTN_MSG_INCOMING_CID);
#endif
    RegETLMsg(SIP_CALL_RELEASED);
}

bool CContractListener::UpdateCallinfo(const CSessionProxy & proxy, bool bSearchDisp)
{
    if (!proxy)
    {
        return false;
    }

    // 获取联系人大头像
    // 通过搜索、联系人、历史记录带名字的呼出,不再搜索名字且取头像需要同时匹配号码与名字
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    yl::string& strNumber = pCallInfo->m_tRemoteInfo.sSIPName;
    yl::string& strName = pCallInfo->m_tRemoteInfo.sDisplayName;
    int nLineID = pCallInfo->GetAccountID();

    // 获取联系人大头像
    // 通过搜索、联系人、历史记录带名字的呼出,不再搜索名字且取头像需要同时匹配号码与名字
    if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
    {
        GetPhoto(pCallInfo->m_strRemotePhoto, strNumber, bSearchDisp ? "" : strName);
        TALK_INFO("Photo [%s]", pCallInfo->m_strRemotePhoto.c_str());
    }

    // 替换Brosftsoft里的特征码显示
    if (ReplaceBsftFAC(pCallInfo))
    {
        return true;
    }

    // 不搜索名字，直接返回
    if (!bSearchDisp)
    {
        return false;
    }

    // 从联系人中查找名称
    yl::string strContactName = GetNameFromContact(strNumber, pCallInfo->m_eCallType);
    if (!strContactName.empty())
    {
        strName = strContactName;
        return true;
    }
    // 来电去电都要到ldap中查询
    else if ((CT_OUTGOING == pCallInfo->m_eCallType
              && configParser_GetConfigInt(kszLDAPCallOutLookup) == 1)
             || (CT_INCOMING == pCallInfo->m_eCallType
                 && configParser_GetConfigInt(kszLDAPCallInLookup) == 1))
    {
        return talkasync_CreateLdapSearch(pCallInfo);
    }

    return false;
}

bool CContractListener::OnMessage(msgObject& msg)
{
    if (SIP_CALL_RELEASED == msg.message)
    {
        sip_reason_info_t* pReasonInfo = (sip_reason_info_t*)msg.GetExtraData();
        if (pReasonInfo == NULL)
        {
            return false;
        }

        CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(msg.lParam);
        if (pCallInfo == NULL)
        {
            return false;
        }

        CListenInfo* priv = GetCallerData(pCallInfo);
        if (priv != NULL && CALL_LINE_PSTN != pCallInfo->GetCallLineType())
        {
            // SIP_CALL_RELEASED消息中的code为1时不记录呼叫信息
            if (pReasonInfo->code == 1) // Reason: SIP ;cause=200 ;text="Call completed elsewhere"
            {
                priv->m_bSaveCallLog = false;
            }
        }
    }

    return true;
}

DECL_SIGNAL_HANDLER(CContractListener::OnRoutineChanged)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (NULL == pCallInfo)
    {
        return false;
    }

    /* 来电铃声获取 */
    switch (sigp.lParam.iValue)
    {
    case ROUTINE_RINGING:
        {
            return talksync_OnIncomingCall(pCallInfo->GetCallID(), pCallInfo->GetAccountID(), pCallInfo->GetRingUserInfo());
        }
        break;
    case ROUTINE_RECALL:
        {
            yl::string strPath;
            const yl::string& strNumber = pCallInfo->m_tRemoteInfo.sSIPName;
            bool bRet = talksync_GetIncomingCallRing(pCallInfo->GetAccountID(), strNumber, strPath);

            TALK_DBG("OnRoutineChange num[%s] ret[%d] path[%s]", strNumber.c_str(), bRet, strPath.c_str());

            int iStatus = bRet ? TALK_RING_LS_LOADED : TALK_RING_LS_FAILED;
            return talklogic_UIEvent(sigp.iSessionID, TALK_ACTION_PLAY_TONE, iStatus, 0, (void*)strPath.c_str());
        }
        break;
    case ROUTINE_CONNECTING:
        {
            // http://10.2.1.199/Bug.php?BugID=36464
            // 直接呼出该号码时，CCallInfo::SetContactInfo中还不能通过callid获取到session故无法ldap查找
            if (pCallInfo->m_tRemoteInfo.sDisplayName.empty()
                    || pCallInfo->m_tRemoteInfo.sDisplayName == pCallInfo->m_tRemoteInfo.sSIPName)
            {
                // 增加去电搜索LDAP开关
                if (configParser_GetConfigInt(kszLDAPCallOutLookup) == 1)
                {
                    // LDAP搜索联系人
                    talkasync_CreateLdapSearch(pCallInfo);
                }
            }
        }
    case ROUTINE_TALKING:
        {
            // 记录原始通话信息
            pCallInfo->SetOriginalRemoteInfo();
            return true;
        }
        break;
    case ROUTINE_BLACONNECTING:
        {
            // 替换callpull特征码
            return ReplaceBsftFAC(pCallInfo);
        }
    default:
        break;
    }

    return false;
}

DECL_SIGNAL_HANDLER(CContractListener::OnRoutineChange)
{
    CSessionProxy proxy(sigp.iSessionID);
    if (!proxy || priv == NULL)
    {
        return false;
    }

    ROUTINE_STATE eOldRoutine = (ROUTINE_STATE)sigp.wParam.iValue;
    ROUTINE_STATE eNewRoutine = (ROUTINE_STATE)sigp.lParam.iValue;
    if (ROUTINE_NETWORKCONF_CONNECT == eNewRoutine)
    {
        // 网络会议不保存呼叫记录
        priv->m_bSaveCallLog = false;
    }
    else if (ROUTINE_CONNECTING == eNewRoutine && proxy.IsHide())
    {
        priv->m_bSaveCallLog = false;
    }

    if (eOldRoutine == ROUTINE_UNKNOWN
            || eNewRoutine == ROUTINE_RINGING
            || eNewRoutine == ROUTINE_CONNECTING)
    {
        priv->m_tBornTime = GetCurrentTick();
    }

    if (eNewRoutine == ROUTINE_TALKING
            || eNewRoutine == ROUTINE_BTTALKING)
    {
        priv->m_tEstblishTime = GetCurrentTick();
    }

    return false;
}

DECL_SIGNAL_HANDLER(CContractListener::OnCallInfoChange)
{
    return UpdateCallinfo(sigp.iSessionID, sigp.wParam.bValue);
}

DECL_SIGNAL_HANDLER(CContractListener::OnSessionDestory)
{
    if (priv == NULL)
    {
        return false;
    }

    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(sigp.iCallID);
    if (NULL == pCallInfo)
    {
        return false;
    }

    /* 拨号退出不记录 */
    if (Comm_IsRoutineInherits(ROUTINE_DIALING_CLS, pCallInfo->GetRoutine()))
    {
        priv->m_bSaveCallLog = false;
    }

    if (pCallInfo->m_strRemotePhoto.find(TEMP_PATH "SIPPic") != yl::string::npos)
    {
        unlink(pCallInfo->m_strRemotePhoto.c_str());
    }

    talkasync_ReleaseLdapSearch(pCallInfo);
    return CALL_HANDLER(SaveCallLog);
}

DECL_SIGNAL_HANDLER(CContractListener::OnCallIncome)
{
    if (sigp.lParam.pValue == NULL)
    {
        return false;
    }

    CallInUserInfo& userInfo = *((CallInUserInfo*)sigp.lParam.pValue);
    int iAccountID = sigp.wParam.iValue;

#ifdef IF_FEATURE_PSTN
    if (sigp.tParam.iValue == CALL_LINE_PSTN)
    {
        yl::string strContactName = GetNameFromContact(userInfo.strNumber, CT_INCOMING);
        if (!strContactName.empty())
        {
            userInfo.strDisplay = strContactName;
        }

        return false;
    }
#endif

    yl::string strDivertNum;
    LocalDir_GetDivertNumberByNumber(userInfo.strNumber, strDivertNum);
    if (!strDivertNum.empty())
    {
        TALK_WARN("[TALK]Refuse incoming call if number has divert number");
        modVoip_RequestForward(iAccountID, sigp.iCallID, strDivertNum.c_str(), FWD_TYPE_ALWAYS);
        SIG_REPLY(configParser_GetConfigInt(kszFWDRefuseCode));
    }

    // 来电号码是否在黑名单中，在则拒绝
    if (LocalDir_IsBlackContactNumber(userInfo.strNumber))
    {
        TALK_WARN("[TALK]Refuse incoming call if number in blacklist");
        SIG_REPLY(configParser_GetConfigInt(kszNormalRefuseCode));
    }

    yl::string strContactName = GetNameFromContact(userInfo.strNumber.c_str(), CT_INCOMING);
    if (!strContactName.empty())
    {
        userInfo.strDisplay = strContactName;
    }

    // 上电不写配置
    if (g_bAfterInit && !userInfo.strNumber.empty()) // num为不为空时才保存
    {
        yl::string strTempNumber = userInfo.strNumber;
        bool bAnonymousCall = strTempNumber.to_lower() == "anonymous"
                              || strTempNumber.to_lower() == "private number";
        if (!bAnonymousCall)
        {
            // 更新回拨信息
            // 保存回拨信息
            configParser_SetConfigString(kszCallReturnUser, acc_GetUsername(iAccountID).c_str());
            configParser_SetConfigString(kszCallReturnNumber, userInfo.strNumber.c_str());
            configParser_SetConfigInt(kszCallReturnAccount, iAccountID);
            // GUIID=17494回拨带server IP的账号出问题
            // 如果呼叫记录的本地号码同本地服务器相同,或者本地帐号服务器与对方帐号服务器不相同，
            // 满足前两个条件之一，并且本地帐号服务器为正确的IP，则应用对方的服务器作为服务器呼出
            if (netIsValidIP(acc_GetServerName(iAccountID).c_str())
                    && (acc_GetUsername(iAccountID) == acc_GetServerName(iAccountID))
                    || acc_GetServerName(iAccountID) != userInfo.strServer)
            {
                configParser_SetConfigString(kszCallReturnServer, userInfo.strServer.c_str());
            }
            else
            {
                configParser_SetConfigString(kszCallReturnServer, acc_GetServerName(iAccountID).c_str());
            }
        }
    }

    return false;
}

// 保存通话记录
DECL_SIGNAL_HANDLER(CContractListener::SaveCallLog)
{
    if (priv == NULL)
    {
        return false;
    }

    CSessionProxy proxy(sigp.iSessionID);
    if (!proxy.IsSingle())
    {
        return false;
    }

    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

#ifdef IF_BT_SUPPORT_PHONE
    if (proxy.IsBTTalk()
            && configParser_GetConfigInt(kszSaveMobileCallHistory) == 0)
    {
        priv->m_bSaveCallLog = false;
    }
#endif

    if (priv->m_tEstblishTime != 0)
    {
        pCallInfo->SetCallDuration(GetCurrentTick() - priv->m_tEstblishTime);
    }

    TALK_INFO("SaveCallLog session[%d] save[%d]", sigp.iSessionID, priv->m_bSaveCallLog);
    if (pCallInfo == NULL || !priv->m_bSaveCallLog)
    {
        return false;
    }

    int iAccountID = pCallInfo->GetAccountID();
    // 保存呼叫记录
    // 蓝牙手机账号也要保存
    if (!acc_IsAccountIDValid(iAccountID)
#ifdef IF_BT_SUPPORT_PHONE
            && iAccountID != Bluetooth_GetBtAccountID()
#endif
       )
    {
        return false;
    }

#if IF_BUG_19894
    BOOL bSend2CallRecord = configParser_GetConfigInt(kszSend2CallRecord);
#endif

    if (CT_MISSCALL == pCallInfo->m_eCallType)
    {
        if (!acc_IsAccountSaveMissCallLog(iAccountID)
                || !pCallInfo->IsNeedSaveMissCall())
        {
            TALK_WARN("SaveCallLog: Do not log misscall!");
            return false;
        }
    }

    LocalLogItemRecord sCallRecord;
    sCallRecord.SetLogType(pCallInfo->m_eCallType); // 通话类型
    sCallRecord.SetRelationInfo(pCallInfo->m_eCallRelation, pCallInfo->m_strRelationNum);

    char szBuf[128];
    sprintf(szBuf, "%d", (int)priv->m_tBornTime);
    sCallRecord.m_strBornTick = szBuf;// 通话建立时间

    memset(szBuf, 0, 128);
    sprintf(szBuf, "%d", pCallInfo->m_iCallDuration);
    sCallRecord.m_strDuration = szBuf;

    // 保存呼叫记录时，类型是否发生变化
    // Pickup的去电，保存记录时要为来电，但去电失败的不保存
    if (pCallInfo->m_eTalkCallOutType == TCT_PICKUP
#if IF_BUG_19894
            && !bSend2CallRecord
#endif
       )
    {
        if (pCallInfo->m_bFinishedError)
        {
            TALK_WARN("Pickup Call Failed! Skip to save call log");
            return false;
        }
    }

    // 本地账号对应的注册名  本地账号对应的主机名
    if (acc_IsAccountIDValid(pCallInfo->m_iUnregSipAccount))
    {
        sCallRecord.m_strLocalSipName = acc_GetUsername(pCallInfo->m_iUnregSipAccount);
        sCallRecord.m_strLocalSipServer = acc_GetServerName(pCallInfo->m_iUnregSipAccount);
        sCallRecord.m_nLine = pCallInfo->m_iUnregSipAccount;
    }
    else
    {
        sCallRecord.m_strLocalSipName = acc_GetUsername(iAccountID);
        sCallRecord.m_strLocalSipServer = acc_GetServerName(iAccountID);
        sCallRecord.m_nLine = iAccountID;
    }

    // 对方显示名 对方注册名 对方主机名
    // 如果原呼出不为空，且不为Pickup类型，则保存m_tOriginalReomteInfo
    if (!pCallInfo->m_tOriginalRemoteInfo.sSIPName.empty()
            && (pCallInfo->m_eTalkCallOutType != TCT_PICKUP
#if IF_BUG_19894
                || bSend2CallRecord
#endif
               ))
    {
        // http://10.2.1.199/Bug.php?BugID=39672
        // 当前后号码相同时取新的显示名。因为匹配联系人或BroadSoft回铃时均会替换掉显示名
        if (configParser_GetConfigInt(kszSaveOriginalNum2History) == 0)
        {
            sCallRecord.m_strDisplayName = pCallInfo->m_tRemoteInfo.sDisplayName;
            sCallRecord.SetNumber(pCallInfo->m_tRemoteInfo.sSIPName);
            sCallRecord.m_strRemoteSipServer = pCallInfo->m_tRemoteInfo.sServerName;
        }
        else
        {
            yl::string strSIPName = pCallInfo->m_tOriginalRemoteInfo.sSIPName;
            int iPos = strSIPName.find('@');
            if (iPos != yl::string::npos)
            {
                strSIPName = strSIPName.substr(0, iPos);
            }
            // todo 此处截掉后缀，有较多地方处理，可通话统一格式化处理

            if (strSIPName == pCallInfo->m_tRemoteInfo.sSIPName
                    && !pCallInfo->m_tRemoteInfo.sDisplayName.empty())
            {
                sCallRecord.m_strDisplayName = pCallInfo->m_tRemoteInfo.sDisplayName;
            }
            else
            {
                sCallRecord.m_strDisplayName = pCallInfo->m_tOriginalRemoteInfo.sDisplayName;
            }

            sCallRecord.SetNumber(pCallInfo->m_tOriginalRemoteInfo.sSIPName);
            sCallRecord.m_strRemoteSipServer = pCallInfo->m_tOriginalRemoteInfo.sServerName;

            //需要兼容配置打开的情况，Relation此时存储的应该是替换后的号码
            //因为开关开起来之后存储的是替换之前的号码
            if (!sCallRecord.m_strRelationNum.empty()
                    && sCallRecord.m_strRelationNum == pCallInfo->m_tOriginalRemoteInfo.sSIPName
                    && sCallRecord.m_strRelationNum != pCallInfo->m_tRemoteInfo.sSIPName)
            {
                sCallRecord.m_strRelationNum = pCallInfo->m_tRemoteInfo.sSIPName;
            }
        }
    }
    else
    {
        sCallRecord.m_strDisplayName = pCallInfo->m_tRemoteInfo.sDisplayName;
        sCallRecord.SetNumber(pCallInfo->m_tRemoteInfo.sSIPName);
        sCallRecord.m_strRemoteSipServer = pCallInfo->m_tRemoteInfo.sServerName;
    }

    // http://10.3.5.199/Bug.php?BugID=62107
    // 1、特征码不为空且非pick up类型；2、pick类型：特征码不未空且sip号码未改变过，才使用第一次的信息
    if (!pCallInfo->m_strFeatureAccessCodes.empty()
            && (pCallInfo->m_eTalkCallOutType != TCT_PICKUP
                || pCallInfo->m_tOriginalRemoteInfo.sSIPName == sCallRecord.GetNumber()))
    {
        // 保存历史记录时，显示名保存为替换后的名字
        sCallRecord.m_strDisplayName = sCallRecord.GetNumber();
        // 把特征码替换回来
        sCallRecord.SetNumber(pCallInfo->m_strFeatureAccessCodes);
    }

    if (pCallInfo->m_eCallType == CT_OUTGOING)
    {
        // 保存重拨信息
        configParser_SetConfigString(kszRedialName, sCallRecord.GetDisplayName().c_str());
        configParser_SetConfigString(kszRedialServer, sCallRecord.m_strRemoteSipServer.c_str());
        configParser_SetConfigString(kszRedialNumber, talklogic_HidePSW(sCallRecord.GetNumber()).c_str());
        configParser_SetConfigString(kszRedialUser, sCallRecord.m_strLocalSipName.c_str());
#if IF_BUG_27560
        configParser_SetConfigInt(kszIsRedialHideSipName, pCallInfo->m_bHideInfo ? 1 : 0);
#endif
    }
    else if (!sCallRecord.GetNumber().empty()) //num为空时则不保存CallReturn配置内容
    {
        yl::string strNum = sCallRecord.GetNumber();
        bool bAnonymousCall = (strNum.to_lower() == "anonymous" || strNum.to_lower() == "private number");
        if (!bAnonymousCall)
        {
            // 保存回拨信息
            configParser_SetConfigString(kszCallReturnUser, sCallRecord.m_strLocalSipName.c_str());
            configParser_SetConfigString(kszCallReturnNumber, sCallRecord.GetNumber().c_str());
            configParser_SetConfigInt(kszCallReturnAccount, iAccountID);
#if IF_BUG_27560
            configParser_SetConfigInt(kszIsReturnHideSipName, pCallInfo->m_bHideInfo ? 1 : 0);
#endif

            // GUIID=17494回拨带server IP的账号出问题
            // 如果呼叫记录的本地号码同本地服务器相同,或者本地帐号服务器与对方帐号服务器不相同，
            // 满足前两个条件之一，并且本地帐号服务器为正确的IP，则应用对方的服务器作为服务器呼出
            if (configParser_GetConfigInt(kszReCallUseLocalSipServer) == 0
                    || netIsValidIP(sCallRecord.m_strLocalSipName.c_str())
                    && (sCallRecord.m_strLocalSipName.compare(sCallRecord.m_strLocalSipServer) == 0
                        || sCallRecord.m_strLocalSipServer.compare(sCallRecord.m_strRemoteSipServer) != 0))
            {
                configParser_SetConfigString(kszCallReturnServer, sCallRecord.m_strRemoteSipServer.c_str());
            }
            else
            {
                configParser_SetConfigString(kszCallReturnServer, sCallRecord.m_strLocalSipServer.c_str());
            }
        }
    }

#ifdef IF_FEATURE_PSTN
    if (CALL_LINE_PSTN == pCallInfo->GetCallLineType())
    {
        char szPstnName[8] = { 0 };
        sprintf(szPstnName, "PSTN%d", (iAccountID - PSTN_LINE_BEGIN_ID + 1));
        sCallRecord.m_strLocalSipName = szPstnName;
        sCallRecord.m_nLine = iAccountID;
    }
#endif // IF_FEATURE_PSTN

    // 保存是否隐藏信息的通话到通话记录
#if IF_BUG_27560
    sCallRecord.m_bHideSipName = pCallInfo->m_bHideInfo;
#endif

    bool bRet = false;
    // 判断保存呼叫记录的开关是否打开
    // 保存历史记录接口里面有判断
    if (configParser_GetConfigInt(kszSaveCallLogSwitch) == 1)
    {
        TALK_INFO("CSingleSession::SaveCallHistory Type[%d] Duration[%d]", pCallInfo->m_eCallType,
                  pCallInfo->m_iCallDuration);
        // 保存呼叫记录
        bRet = Comm_AddToCallList(sCallRecord);

#if IF_BUG_19894
        if (pCallInfo->m_eCallType == CT_OUTGOING
                && !pCallInfo->m_tRemoteInfo.sSIPName.empty()
                && pCallInfo->m_tOriginalRemoteInfo.sSIPName.compare(pCallInfo->m_tRemoteInfo.sSIPName) != 0
                && bSend2CallRecord)
        {
            sCallRecord.m_strDisplayName = pCallInfo->m_tRemoteInfo.sDisplayName;
            sCallRecord.SetNumber(pCallInfo->m_tRemoteInfo.sSIPName);
            sCallRecord.SetLogType(CT_INCOMING);
            bRet |= Comm_AddToCallList(sCallRecord);
        }
#endif
    }
    // 保存上次呼出的记录，重播时候使用
    else if (pCallInfo->m_eCallType == CT_OUTGOING)
    {
        TALK_INFO("Save Callout log");
        bRet = Comm_AddToCallList(sCallRecord);
    }

    if (!bRet)
    {
        return false;
    }

    if (CT_FORWARDCALL == pCallInfo->m_eCallType)
    {
        const yl::string& strTarget = sigp.strData;
        // 保存通话记录
        yl::string strName = GetNameFromContact(strTarget, pCallInfo->m_eCallType);
        if (strName.empty())
        {
            strName = strTarget;
        }

        // 只有当历史记录被成功保存, 才通知弹窗, 避免View查看不到记录
        idleScreen_SetStatus(PS_STATE_CALLFORWARD, strName);
    }

    // 不再保存呼叫记录
    priv->m_bSaveCallLog = false;
    return true;
}

DECL_SIGNAL_HANDLER(CContractListener::OnCallIDChanged)
{
    CSessionProxy proxy(sigp.iSessionID);
    if (!proxy.IsInherits(ROUTINE_TALKING))
    {
        return false;
    }

    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }

    if (configParser_GetConfigInt(kszSaveSrcCallLog))
    {
        CALL_HANDLER(SaveCallLog);
        pCallInfo->m_tOriginalRemoteInfo.sSIPName.clear();
        priv->m_tBornTime = GetCurrentTick();
    }

    pCallInfo->m_strUserAgent = modVoip_GetUserAgent(pCallInfo->GetAccountID(), pCallInfo->GetCallID());
    return true;
}

DECL_SIGNAL_HANDLER(CContractListener::OnTransfer)
{
    CCallInfo* pSponorInfo = talklogic_GetCallInfoByCallId(sigp.iCallID);
    if (pSponorInfo == NULL)
    {
        return false;
    }

    if (sigp.lParam.iValue == -1)
    {
        // 忙转
        yl::string strNewNum = sigp.strData;

        dialplan_GetReplaceNumberEx(strNewNum, pSponorInfo->GetAccountID(), DPCM_TRANSFER);

        if (LocalLog_DetailedEnable())
        {
            pSponorInfo->SetRelationInfo(CR_TRANS_TO, strNewNum);
        }

        sigp.strData = FormatSIPNumber(strNewNum, "");
        return true;
    }

    CCallInfo* pFollowInfo = talklogic_GetCallInfoByCallId(sigp.lParam.iValue);
    if (pFollowInfo == NULL)
    {
        return false;
    }

    int iFollowAccount = pFollowInfo->GetAccountID();
    CSessionProxy proxy(pSponorInfo);

    if (proxy.IsInConf())
    {
        pSponorInfo->SetRelationInfo(CR_TRANS_TO, pFollowInfo->m_tRemoteInfo.sSIPName);
        pFollowInfo->SetRelationInfo(CR_NONE, pSponorInfo->m_tRemoteInfo.sSIPName);

        yl::string strSIPName = pFollowInfo->m_tRemoteInfo.sSIPName;
        if (!pFollowInfo->m_strNumberBeforeEncrypt.empty())
        {
            strSIPName = pFollowInfo->m_strNumberBeforeEncrypt;
        }

        yl::string strServer = "";
        if (!acc_IsDirectIPAccount(iFollowAccount)
                && acc_GetServerName(iFollowAccount) != pFollowInfo->m_tRemoteInfo.sServerName)
        {
            // 如果服务器名与账号服务器不一致，则带上服务器地址
            strServer = pFollowInfo->m_tRemoteInfo.sServerName;
        }

        // 格式化转移的号码
        sigp.strData = FormatSIPNumber(strSIPName, strServer);
    }
    else
    {
        //支持Refer时的转接流程
        yl::string strSIPName = pFollowInfo->m_tRemoteInfo.sSIPName;
        if (!pFollowInfo->m_strNumberBeforeEncrypt.empty())
        {
            strSIPName = pFollowInfo->m_strNumberBeforeEncrypt;
        }

        yl::string strServer = "";
        if (!acc_IsDirectIPAccount(iFollowAccount)
                && pFollowInfo->m_tRemoteInfo.sServerName != acc_GetServerName(iFollowAccount))
        {
            strServer = pFollowInfo->m_tRemoteInfo.sServerName;
        }

        // 如果服务器名与账号服务器不一致，则带上服务器地址
        sigp.strData = FormatSIPNumber(strSIPName, strServer);

        if (LocalLog_DetailedEnable())
        {
            priv = GetCallerData(pFollowInfo);
            if (priv != NULL)
            {
                priv->m_bSaveCallLog = false;
            }

            pSponorInfo->SetRelationInfo(CR_TRANS_TO, pFollowInfo->m_tRemoteInfo.sSIPName);
        }
    }

    return true;
}

DECL_SIGNAL_HANDLER(CContractListener::OnSubActived)
{
    if (!sigp.lParam.bValue)
    {
        return false;
    }

    CSessionProxy proxy(sigp.iSessionID);
    CSessionProxy talk = proxy.GetSubByIndex(0);
    CSessionProxy ring = proxy.GetSubByIndex(1);

    if (!talk || !ring)
    {
        return false;
    }

    CCallInfo* pTalkCallInfo = talk.GetCallInfo();
    CCallInfo* pRingCallInfo = ring.GetCallInfo();

    if (!pTalkCallInfo || !pRingCallInfo)
    {
        return false;
    }

    CListenInfo* privTalk = GetCallerData(pTalkCallInfo);
    CListenInfo* privRing = GetCallerData(pRingCallInfo);

    if (!privTalk || !privRing)
    {
        return false;
    }

    // 被转移时根据配置项判断保存哪个呼叫记录，为0代表保存转移后的那路通话
    if (0 == configParser_GetConfigInt(kszSaveOriginalNum2History))
    {
        privTalk->m_bSaveCallLog = false;
        privRing->m_bSaveCallLog = true;

        // 开关关闭，保存的是源号码，源号码需要兼容Relation
        pRingCallInfo->SetRelationInfo(CR_NONE, pTalkCallInfo->m_tRemoteInfo.sSIPName);
    }
    else if (LocalLog_DetailedEnable())
    {
        privTalk->m_bSaveCallLog = true;
        privRing->m_bSaveCallLog = false;

        // 开关开启，保存的是源号码，源号码需要兼容Relation
        pRingCallInfo->SetOriginalRemoteInfo(pTalkCallInfo->m_tOriginalRemoteInfo);
        pRingCallInfo->SetRelationInfo(CR_NONE, pTalkCallInfo->m_tOriginalRemoteInfo.sSIPName);
    }

    return true;
}
