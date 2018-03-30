#include "talklogic_inc.h"

extern bool g_bAfterInit;

// 定时器处理
LRESULT talk_OnMSG(msgObject& msg)
{
    return _TalklogicManager.OnSIPMsg(msg);
}

IMPLEMENT_GETINSTANCE(CTalklogicManager)

CTalklogicManager::CTalklogicManager()
{
    m_bCallInIntercom = false;
}

CTalklogicManager::~CTalklogicManager()
{
}

// 初始化
void CTalklogicManager::Init()
{
    SingleMsgReg(TM_TIMER, talk_OnMSG);
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, talk_OnMSG);
}

// 释放
void CTalklogicManager::Uninit()
{
}

// 处理SIP消息
bool CTalklogicManager::OnSIPMsg(msgObject& msg)
{
    // 消息是否被处理
    bool bHandled = false;

    switch (msg.message)
    {
    case TM_TIMER:
        {
            FOR_SESSION_LIST_EXPORT(it)
            {
                CBaseSession* pSessionTmp = (*it);
                if (pSessionTmp != NULL
                        && pSessionTmp->OnTimer(msg.wParam))
                {
                    return true;
                }
            }

            return false;
        }
        break;
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (msg.wParam == ST_AUTOREDIAL)
            {
                _AutoRedialManager.OnConfigChange();
            }
        }
        break;
    case SIP_CALL_INCOMING:
        {
            bHandled = true;

            int iReturnCode = ProcessSipIncomingCall(msg);

            if (iReturnCode != 1)
            {
                // 设置返回值，SIP会用到该值来拒绝通话,回复1的情况已经在函数ProcessIncomingCall中处理
                modVoip_ResponseIncoming(msg.wParam, msg.lParam, iReturnCode);
            }

            return bHandled;
        }
        break;
#ifdef IF_FEATURE_PSTN
    case PSTN_MSG_INCOMING_CALL:
        {
            CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(msg.lParam);
            if (pCallInfo == NULL)
            {
                // 不存在对应Session则进入振铃流程（因为有可能PSTN_MSG_INCOMING_CID消息先到）
                bHandled = ProcessPstnIncomingCall(msg);
            }
        }
        break;
    //395251
    case PSTN_MSG_INCOMING_CID:   //显示Call Information
        {
            CidMsgInfo* pExt = (CidMsgInfo*)msg.GetExtraData();
            if (pExt == NULL)
            {
                break;
            }

            const char* szNumber = pExt->m_szNumber;
            bool bCidAfterRing = pExt->m_bCidAfterRing;
            TALK_INFO("CID Msg Number [%s] AfterRing [%d]", szNumber, bCidAfterRing);
            CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(msg.lParam);

            if (pCallInfo == NULL)
            {
                bHandled = true;
                // 之前已经处理过一次了，则在来cid消息不处理为来电。
                if (bCidAfterRing)
                {
                    break;
                }
                // 不存在对应Session则进入振铃流程（因为有可能PSTN_MSG_INCOMING_CID消息先到）
                ProcessPstnIncomingCall(msg, szNumber);
            }
            break;
        }
    case PSTN_MSG_INCOMING_CANCEL:
        {
            // 构造SIP消息
            msgObject objSipMsg;
            objSipMsg.message = SIP_CALL_RELEASED;
            objSipMsg.wParam = acc_GetAccountIDByDeviceID(msg.wParam);
            objSipMsg.lParam = msg.lParam;

            return OnSIPMsg(objSipMsg);
        }
        break;
#endif
    default:
        {
            CBaseSession* pSession = _SessionManager.GetSessionByCallID(msg.lParam);
            if (pSession != NULL)
            {
                bHandled = pSession->OnSIPMsg(msg);
            }
            else
            {
                TALK_WARN("[TALK] No session own CallID[%d]", msg.lParam);
            }
        }
        break;
    }

    return bHandled;
}

#ifdef IF_FEATURE_PSTN
bool CTalklogicManager::ProcessPstnIncomingCall(const msgObject& msg, const yl::string& strFromNumber /* = yl::string() */)
{
    // 判断账号状态
    int iDeviceID = msg.wParam;
    int iAccountID = acc_GetAccountIDByDeviceID(iDeviceID);
    int iCallID = msg.lParam;

    CallInUserInfo userInfo;
    userInfo.iAutoAnswer = 0;
    userInfo.iBlaIndex = 0;
    userInfo.uAutoAnswerType = 0;
    userInfo.bForceAutoAnswer = false;
    userInfo.uAutoAnswerDevice = 0;
    userInfo.bIsEmergency = false;
    userInfo.bIsFwdEnable = false;
    // 来电没有显示号码时显示“PSTN”
    userInfo.strNumber = userInfo.strDisplay = strFromNumber.empty() ? "PSTN" : strFromNumber;

    SignalParams sigp;
    sigp.iCallID = iCallID;
    sigp.wParam.iValue = iAccountID;
    sigp.lParam.pValue = &userInfo;
    sigp.tParam.iValue = CALL_LINE_PSTN;

    if (modtkflt_emit(TALK_FLT_SESSION_INCOME, sigp))
    {
        return sigp.uReply;
    }

    CSingleSession* pSession = _SessionManager.AllocSession();
    if (pSession == NULL)
    {
        return false;
    }

    // 满足以上条件则建立通话
    CCallInfo* pCallInfo = pSession->GetCallInfo();
    pCallInfo->GetRingUserInfo() = userInfo;
    pCallInfo->m_eCallType = CT_INCOMING;
    pCallInfo->SetAccountID(iAccountID);

    TALK_INFO("New PSTN Incoming Call in line [%d], Ringing", iAccountID);
    pSession->EnterRoutine(ROUTINE_RINGING);
    return true;
}
#endif // IF_FEATURE_PSTN

// 处理新来电, 返回值发给sip，让sip根据返回值不同来拒绝来电
int CTalklogicManager::ProcessSipIncomingCall(msgObject& msg)
{
    sip_call_identify_info_t sipCallInInfo;
    Comm_GetSipData(&sipCallInInfo, msg.GetExtraData(), SIP_MAP_METHOD_IDENTIFY);

    CallInUserInfo userInfo;
    MapCallIdentifyInfo_CallInInfo(&sipCallInInfo, userInfo);
    return ProcessIncomingCall(msg.wParam, msg.lParam, userInfo);
}

int CTalklogicManager::ProcessIncomingCall(int iAccountID, int iCallID, CallInUserInfo& userInfo)
{
    SignalParams sigp;
    sigp.iCallID = iCallID;
    sigp.wParam.iValue = iAccountID;
    sigp.lParam.pValue = &userInfo;
    sigp.tParam.iValue = CALL_LINE_SIP;

    if (modtkflt_emit(TALK_FLT_SESSION_INCOME, sigp))
    {
        return sigp.uReply;
    }

    // v80新接口，等于0，就表示是intercom来电，值代表要自动接起的延迟时间,0为立即接起
    // 不允许intercom来电则将intercom来电当作普通来电处理
    bool bIsIntercomCall = userInfo.iAutoAnswer == 0;

    CSingleSession* pSession = _SessionManager.AllocSession(iCallID);
    if (pSession == NULL)
    {
        return configParser_GetConfigInt(kszNormalRefuseCode);
    }

    // 满足以上条件则建立通话
    CCallInfo* pCallInfo = pSession->GetCallInfo();
    pCallInfo->m_eCallType = CT_INCOMING;
    pCallInfo->GetRingUserInfo() = userInfo;
    pCallInfo->SetAccountID(iAccountID);
    pCallInfo->m_bIsIntercom = bIsIntercomCall;
#if IF_FEATURE_LOOPBACK
    pCallInfo->m_bIsLoopBackCall = userInfo.bIsLoopBackCall;
#endif

    TALK_INFO("New Incoming Call in line [%d], Ringing", iAccountID);
    pSession->EnterRoutine(ROUTINE_RINGING);

    // http://192.168.1.99/Bug.php?BugID=18570
    m_bCallInIntercom = bIsIntercomCall;
    return 1;
}

bool CTalklogicManager::IsAllowNewCall(int iAccountID)
{
    SignalParams sigp;
    sigp.tParam.iValue = iAccountID;
    return !modtkflt_emit(TALK_FLT_SESSION_CREATE, sigp);
}

// 进入拨号界面
CSingleSession* CTalklogicManager::EnterDialUI(const yl::string& strNumber, int iAccountId/* = AUTO_SELECT_LINE_ID*/,
        TALK_CALLOUT_TYPE eCallOutType /*= TCT_NORMAL*/, ROUTINE_STATE eRoutie/* = ROUTINE_DIALING*/)
{
    int iUseAccounID = GetUsableAccount(iAccountId);
#ifdef IF_BT_SUPPORT_PHONE
    if (AUTO_SELECT_LINE_ID == iAccountId
            && ACCOUNTID_OF_IPDIRECT_CALL == GetUsableAccount(iAccountId)
            && Bluetooth_IsMobileConnected())
    {
        return _BTMobileManager.EnterDialUI(strNumber);
    }
#endif

    // 判断账号是否可以呼出
    if (!IsAllowNewCall(iUseAccounID))
    {
        return NULL;
    }

    CSingleSession* pSession = _SessionManager.AllocSession();
    if (pSession == NULL)
    {
        return NULL;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();
    // 获取可用账号
    pCallInfo->SetAccountID(iUseAccounID);
    pCallInfo->m_tRemoteInfo.sDisplayName = strNumber;
    pCallInfo->m_tRemoteInfo.sSIPName = strNumber;
    pCallInfo->m_eTalkCallOutType = eCallOutType;

#if IF_BUG_27560
    if (actURI_IsHideTalkInfo())
    {
        pCallInfo->m_bHideInfo = true;
    }
#endif //IF_BUG_27560

    pCallInfo->SetDialedNumber(strNumber);
    TryEnterRoutine(pSession, eRoutie, strNumber.c_str());
    return pSession;
}

// 账号是否允许呼出
bool CTalklogicManager::IsAccountEnableToCallout(int iAccount)
{
#ifdef IF_FEATURE_EMERGENCY
    if (Emergency_IsEmergencyModeEnable())
    {
        return true;
    }
#endif //IF_FEATURE_EMERGENCY

    // 账号状态为不可用且账号对应的Server为空
    if (!acc_IsAccountAvailable(iAccount)
            && (acc_GetServerName(iAccount).find_first_not_of(" \t") == yl::string::npos
                || configParser_GetConfigInt(kszEnableDirectIPCall) == 0))
    {
        // 账号对应的Server不为空则可用于无账号呼出功能
        return false;
    }

    return true;
}

// 呼出处理
TALK_CALLOUT_RESULT CTalklogicManager::ProcessCallOut(CSingleSession* pSession, LPCSTR lpszNumber, const yl::string& strName)
{
    if (pSession == NULL)
    {
        return TALK_CALLOUT_FAIL;
    }

    if (TryEnterFinishedRoutine(pSession))
    {
        return TALK_CALLOUT_FAIL;
    }

    SignalParams sigp(pSession, lpszNumber);
    sigp.wParam.pValue = strName.c_str();
    modtkflt_emit(TALK_SIG_PROCESS_CALLOUT, sigp);
    ROUTINE_STATE eRoutine = (ROUTINE_STATE)sigp.uReply;

    if (eRoutine <= ROUTINE_UNKNOWN)
    {
        return TALK_CALLOUT_FAIL;
    }

    return TryEnterRoutine(pSession, eRoutine, lpszNumber) ? TALK_CALLOUT_SUCCESS : TALK_CALLOUT_FAIL;
}

// 获取可用账号
int CTalklogicManager::GetUsableAccount(int iAccountId, const yl::string& strServer /*= ""*/)
{
    if (iAccountId != AUTO_SELECT_LINE_ID
            && acc_IsAccountAvailable(iAccountId))
    {
        return iAccountId;
    }
#ifdef IF_FEATURE_PSTN
    if (strServer.empty())
    {
        iAccountId = acc_GetUsableAccount(false, ACCOUNT_TYPE_ALL);
    }
    else
#endif // IF_FEATURE_PSTN
    {
        iAccountId = acc_GetUsableAccount(strServer);
    }
    return iAccountId;
}

// 进入拨号呼出界面
TALK_CALLOUT_RESULT CTalklogicManager::EnterCallOut(const yl::string& strNumber,
        const yl::string& strName,
        int iAccountId/* = AUTO_SELECT_LINE_ID*/, bool bConferenceDirect/* = false*/,
        CallOut_Param* pCallParam /*= NULL*/, int nCallId/* = -1*/, bool bNeedPrefix/* = false*/,
        bool bForceToCallOut /*= false*/, bool bConfOnePress/* = false*/)
{
    // 分解收到数据得到呼出号码和服务器
    yl::string strSIPName = "";
    yl::string strServer = "";

    strSIPName = strNumber;
    if (iAccountId == AUTO_SELECT_LINE_ID)
    {
        // 账号为-1时才分解服务器
        commonUnits_ParserDialURI(strNumber, strSIPName, strServer);
    }

    // 获取可用账号
    int iAccount = GetUsableAccount(iAccountId, strServer);

    TALK_CALLOUT_TYPE eCallType = TCT_NORMAL;
    if (pCallParam != NULL)
    {
        eCallType = pCallParam->eCallOutType;
    }

    // 由dsskey为conf触发的，不去判断mca规则
    if (!bConferenceDirect && !IsAllowNewCall(iAccount)
#if IF_BUG_MCA_PARK
            && eCallType != TCT_CALLPARK
#endif
       )
    {
        return TALK_CALLOUT_FAIL;
    }

    CSingleSession* pSession = _SessionManager.AllocSession(nCallId);
    if (pSession == NULL)
    {
        return TALK_CALLOUT_FAIL;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();

    if (pCallParam != NULL)
    {
        pCallInfo->GetCallOutParam() = *pCallParam;
    }

    pCallInfo->SetAccountID(iAccount);
    pCallInfo->m_bConferenceDirect = bConferenceDirect;
    pCallInfo->m_bConfWithOnePress = bConfOnePress;

#ifdef IF_FEATURE_BROADSOFT_MEET_ME
    if (pCallParam != NULL)
    {
        pCallInfo->m_strAutoDTMF = pCallParam->m_strAutoDTMF;
    }
#endif

#if IF_BUG_31081
    if (bConferenceDirect && configParser_GetConfigInt(kszConfUseNewLinekey) == 0)
    {
        pCallInfo->m_bNeedBindDsskey = false;
    }
#endif

    return ProcessCallOut(pSession, strNumber.c_str(), strName.c_str());
}

bool CTalklogicManager::TryEnterRoutine(CSingleSession* pSession, ROUTINE_STATE eNewState,
                                        LPCSTR lpNum /*= NULL*/)
{
    if (pSession == NULL)
    {
        return false;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();

    bool bRet = false;
    int iFailedReason = TALK_ERROR_NONE;

    if (
#ifdef IF_FEATURE_PSTN
        pCallInfo->GetCallLineType() != CALL_LINE_PSTN
        &&
#endif // IF_FEATURE_PSTN
        !netIsNetworkReady()
    )
    {
        iFailedReason = TALK_ERROR_CALLOUT_NU;
    }
    else if (!Comm_IsRoutineInherits(ROUTINE_DIALING_CLS, eNewState)
             && !IsAccountEnableToCallout(pCallInfo->GetAccountID()))
    {
        iFailedReason = TALK_ERROR_CALLOUT_IA;
    }
    else if (!talklogic_BeAllowNewCall(lpNum))
    {
        iFailedReason = TALK_ERROR_CALLOUT_MSE;
    }
    else
    {
        bRet = true;
    }

    if (bRet && iFailedReason == TALK_ERROR_NONE)
    {
        pSession->EnterRoutine(eNewState);
    }
    else
    {
        pCallInfo->m_errorInfo.iReason = iFailedReason;
        pSession->EnterRoutine(ROUTINE_FINISHED);
    }

    return bRet;
}

bool CTalklogicManager::TryEnterPreRoutine(CSingleSession* pSession, ROUTINE_STATE eNewState,
        const yl::string& strNum)
{
    if (pSession == NULL)
    {
        return false;
    }

    bool bRet = false;
    int iFailedReason = TALK_ERROR_NONE;
    CCallInfo* pCallInfo = pSession->GetCallInfo();

    bool bNotMatchAutopCodePrefix = Autop_MatchAutoPCodePrefix(strNum) < 0;
    bool bNotMatchTestModePrefix = !commonUnits_IsTestModePrefix(strNum);

    // 如果是测试模式前缀则不论是否有账号都要进入预拨号界面
    if (bNotMatchAutopCodePrefix && bNotMatchTestModePrefix
#ifdef IF_FEATURE_PSTN
            && pCallInfo->GetCallLineType() != CALL_LINE_PSTN
#endif // IF_FEATURE_PSTN
            && !netIsNetworkReady())
    {
        iFailedReason = TALK_ERROR_CALLOUT_NU;
    }
    else if (!Comm_IsRoutineInherits(ROUTINE_DIALING_CLS, eNewState)
             && !IsAccountEnableToCallout(pCallInfo->GetAccountID())
             && bNotMatchAutopCodePrefix && bNotMatchTestModePrefix)
    {
        iFailedReason = TALK_ERROR_CALLOUT_IA;
    }
    else if (!talklogic_BeAllowNewCall(strNum.c_str())
             && bNotMatchAutopCodePrefix && bNotMatchTestModePrefix)
    {
        iFailedReason = TALK_ERROR_CALLOUT_MSE;
    }
    else
    {
        bRet = true;
    }

    if (bRet && iFailedReason == TALK_ERROR_NONE)
    {
        pSession->EnterRoutine(eNewState);
    }
    else
    {
        pCallInfo->m_errorInfo.iReason = iFailedReason;
        pSession->EnterRoutine(ROUTINE_FINISHED);
    }

    return bRet;
}

bool CTalklogicManager::TryEnterFinishedRoutine(CSingleSession* pSession, TalkErrorDefine eError/* = TALK_ERROR_NONE*/)
{
    if (pSession == NULL)
    {
        return false;
    }

    SignalParams sigp(pSession);
    sigp.wParam.iValue = eError;
    if (modtklsn_emit(TALK_SIG_SESSION_TRY_ENTER_FINISH, sigp))
    {
        pSession->GetCallInfo()->m_errorInfo.iReason = sigp.uReply;
        pSession->EnterRoutine(ROUTINE_FINISHED);
        return true;
    }

    return false;
}

bool CTalklogicManager::ProcessCallFailed(CBaseRoutine* pRoutine, msgObject& objMsg,
        bool bCallFail /*= true*/, bool bPlayBusyTone /*= true*/, bool bEndCall /*= true*/)
{
    if (pRoutine == NULL)
    {
        return false;
    }

    sip_reason_info_t* pReasonInfo = static_cast<sip_reason_info_t*>(objMsg.GetExtraData());
    if (pReasonInfo == NULL)
    {
        return false;
    }

    TalkErrorInfo errorInfo(pReasonInfo->code, pReasonInfo->reason);

    pRoutine->ProcessAfterCallFinished(errorInfo, bCallFail, bPlayBusyTone, bEndCall);
    return true;
}

