#include "talklogic_inc.h"
#include "edk/src/edkmanager.h"
#include "talk/talkadapter/include/scopedwndupdate.h"

// 初始化Talk模块
void talklogic_Core_Init()
{
    _TalklogicManager.Init();
    _AOCManager.Init();
    _SessionManager;
    _AutoRedialManager.Init();

#if IF_FEATURE_PAGING
    _MulticastPage.InitInfo();
#endif

#ifdef IF_BT_SUPPORT_PHONE
    _BTMobileManager;
#endif //IF_BT_SUPPORT_PHONE
    TiptelA_Init();

#ifdef IF_FEATURE_DSSKEY
    talk_dsskey_init();
#endif  //IF_FEATURE_DSSKEY
    dialplan_Init();
    vqrtcp_Init();
    talkhelp_Init();
    talkasync_Init();
}

// 退出前的处理，释放线程数据
void talklogic_Uninit()
{
    _TalklogicManager.Uninit();
}

// 获取所有SessionID
bool talklogic_GetAllSessionID(YLVector<int>& vecSessionID, bool bExceptHide/* = true*/)
{
    return _SessionManager.GetAllSessionID(vecSessionID, bExceptHide);
}

// 进入拨号界面
int talklogic_EnterDialUI(const yl::string& strNumber, int iAccountId/* = AUTO_SELECT_LINE_ID*/,
                          bool bSwitchChannel/* = true*/, TALK_CALLOUT_TYPE eCallOutType /*= TCT_NORMAL*/)
{
    AUTO_WND_UPDATE;

    // 分解收到数据得到呼出号码和服务器
    yl::string strSIPName;
    yl::string strServerName;
    commonUnits_ParserDialURI(strNumber, strSIPName, strServerName);

    CSingleSession* pSession = _TalklogicManager.EnterDialUI(strSIPName, iAccountId, eCallOutType);
    return pSession ? pSession->GetSessionID() : INVALID_SESSION_ID;
}

// 进入预拨号界面
int talklogic_EnterPredialUI(int iDigitKey, int iAccountId/* = AUTO_SELECT_LINE_ID*/)
{
    AUTO_WND_UPDATE;

    // 获取数字键按下时对应的字符串
    yl::string strNumber = GetDigitKeyString(iDigitKey);
    CSingleSession* pSession = _TalklogicManager.EnterDialUI(strNumber, iAccountId, TCT_NORMAL, ROUTINE_PREDIAL);
    if (pSession == NULL)
    {
        return INVALID_SESSION_ID;
    }

    pSession->GetCallInfo()->m_iDigitKey = iDigitKey;
    return pSession->GetSessionID();
}

// 函数描述:    根据指定账号进入拨号界面
int talklogic_EnterDialUIByAccount(int iAccountID)
{
    // 获取账号
    if (!acc_IsAccountAvailable(iAccountID))
    {
        TALK_WARN("EnterDialUI: NULL Account !!");
        return INVALID_SESSION_ID;
    }

    // 先进入拨号界面之后再切换通道
    return talklogic_EnterDialUI("", iAccountID);
}

// 函数描述:    根据SCA线程呼出
bool talklogic_CallOutBySCALine(int iAccountID, int iBlaIndex,
                                const yl::string& strCallPullCode/* = ""*/, bool bSilentBarginIn /*= false*/)
{
    int iSessionID = 0;
    if (talklogic_CallOutBySCALine(iAccountID, iBlaIndex, iSessionID, strCallPullCode,
                                   bSilentBarginIn) != TALK_CALLOUT_SUCCESS)
    {
        return false;
    }

    return true;
}

// 函数描述:    根据SCA线程呼出
TALK_CALLOUT_RESULT talklogic_CallOutBySCALine(int iAccountID, int iBlaIndex, int& iSessionID,
        const yl::string& strCallPullCode/* = ""*/, bool bSilentBarginIn /*= false*/)
{
    if (!acc_IsAccountIDValid(iAccountID))
    {
        TALK_WARN("Invalid SCA Account! AccountID[%d]", iAccountID);
        return TALK_CALLOUT_FAIL;
    }

    CallOut_Param param;
    param.iAccountID = iAccountID;
    param.iBLAIndex = iBlaIndex;
    param.strNumber = strCallPullCode;
    param.bSilentBarginIn = bSilentBarginIn;

    CCallInfo* pCallInfo = talklogic_ProcessCallOut(param);
    if (pCallInfo == NULL)
    {
        return TALK_CALLOUT_FAIL;
    }

    iSessionID = pCallInfo->GetSessionID();
    return TALK_CALLOUT_SUCCESS;
}

// 回拨
void talklogic_CallReturn()
{
    yl::string strNumber = configParser_GetConfigString(kszCallReturnNumber);
    if (strNumber.empty())
    {
        return;
    }

    yl::string strUser = configParser_GetConfigString(kszCallReturnUser);
    yl::string strServer = configParser_GetConfigString(kszCallReturnServer);
    TALK_INFO("CallReturnUser=%s, CallReturnServer=%s", strUser.c_str(), strServer.c_str());

    // 查找帐号逻辑：
    // a.对于SIP来电（有SIP账号和服务器信息）：优先使用相同账号和服务器的SIP账号回拨，其次使用IP直拨账号，最后才用PSTN账号。
    // b.对于PSTN来电（没有SIP账号和服务器信息）：优先使用PSTN账号回拨，其次使用SIP账号，最后使用IP直拨账号。
    int iUseLineID = INVALID_ACCOUNTID;
#ifdef IF_FEATURE_PSTN
    // 服务器为空说明是PSTN来电
    if (strUser.empty() && strServer.empty() && acc_ExistAvailableAccount(ACCOUNT_TYPE_PSTN))
    {
        int iAccountID = configParser_GetConfigInt(kszCallReturnAccount);
        TALK_INFO("CallReturnAccount=%d", iAccountID);
        if (acc_IsPstnAccount(iAccountID) && acc_IsAccountAvailable(iAccountID))
        {
            // 如果存储的账号为PSTN账号且可用，则优先使用来电账号
            iUseLineID = iAccountID;
        }
        else
        {
            // 否则使用第一个可用账号
            ListAccounts lstPstnAccounts = acc_GetAvailableAccountList(ACCOUNT_TYPE_PSTN);
            if (lstPstnAccounts.size() > 0)
            {
                iUseLineID = lstPstnAccounts.front();
            }
        }
    }
    else
#endif // IF_FEATURE_PSTN
    {
        for (int i = 0; i < acc_AccountNum(); i++)
        {
            if (acc_IsAccountAvailable(i)
                    && (strUser.empty() || acc_GetUsername(i) == strUser)
                    && (strServer.empty() || acc_GetServerName(i) == strServer))
            {
                iUseLineID = i;
                break;
            }
        }
    }

    // 取ip直播账号
    if (iUseLineID == INVALID_ACCOUNTID)
    {
        int iDirectIpAccountId = acc_GetDirectIPAccountID();
        if (acc_IsAccountAvailable(iDirectIpAccountId))
        {
            iUseLineID = iDirectIpAccountId;
        }
    }

#ifdef IF_FEATURE_PSTN
    if (iUseLineID == INVALID_ACCOUNTID && acc_ExistAvailableAccount(ACCOUNT_TYPE_PSTN))
    {
        // 对于SIP来电最后才使用PSTN账号回拨
        ListAccounts lstPstnAccounts = acc_GetAvailableAccountList(ACCOUNT_TYPE_PSTN);
        if (lstPstnAccounts.size() > 0)
        {
            iUseLineID = lstPstnAccounts.front();
        }
    }
#endif // IF_FEATURE_PSTN

    // 回拨ip时不加server http://10.2.1.199/Bug.php?BugID=24125
    if (!Comm_IsValidIP(strNumber.c_str())
            && !strServer.empty()
            && acc_GetServerName(iUseLineID) != strServer
#ifdef IF_FEATURE_PSTN
            && !acc_IsPstnAccount(iUseLineID) /* 使用PSTN账号回拨不加server */
#endif // IF_FEATURE_PSTN
       )
    {
        strNumber = strNumber + "@" + strServer;
    }

    TALK_INFO("Call return number [%s][%d]", strNumber.c_str(), iUseLineID);
#if IF_BUG_27560
    // 最后一次呼入隐藏号码，则回拨也隐藏
    if (configParser_GetConfigInt(kszIsReturnHideSipName) == 1)
    {
        actURI_SetHideTalkInfo(true);
    }
#endif

    talklogic_CallOut(strNumber, "", iUseLineID);
}

// 呼出号码
bool talklogic_CallOut(const yl::string& strNumber, const yl::string& strName,
                       int iAccountId/* = AUTO_SELECT_LINE_ID*/,
                       CallOut_Param* pCallParam /*= NULL*/, bool bNeedPrefix/* = false*/)
{
    AUTO_WND_UPDATE;

#ifdef IF_BT_SUPPORT_PHONE
    //蓝牙手机话路走另外的流程
    if (iAccountId == Bluetooth_GetBtAccountID())
    {
        return talklogic_CallMobileOut(strNumber, strName);
    }
#endif

    TALK_INFO("talklogic_CallOut Num[%s] Name[%s] AccountID[%d]", strNumber.c_str(), strName.c_str(),
              iAccountId);
    bool bSucc = (_TalklogicManager.EnterCallOut(strNumber, strName, iAccountId, false, pCallParam, -1,
                  bNeedPrefix) == TALK_CALLOUT_SUCCESS);
    return bSucc;
}

bool talklogic_CallOut(const yl::string& strNumber, const yl::string& strName, int iAccountId,
                       int nCallId)
{
    AUTO_WND_UPDATE;

#ifdef IF_BT_SUPPORT_PHONE
    //蓝牙手机话路走另外的流程
    if (iAccountId == Bluetooth_GetBtAccountID())
    {
        return talklogic_CallMobileOut(strNumber, strName);
    }
#endif

    TALK_INFO("talklogic_CallOut Num[%s] Name[%s] AccountID[%d]", strNumber.c_str(), strName.c_str(),
              iAccountId);
    // 默认切换通道
    bool bSucc = (_TalklogicManager.EnterCallOut(strNumber, strName, iAccountId, false, NULL,
                  nCallId) == TALK_CALLOUT_SUCCESS);
    return bSucc;
}

bool talklogic_CallOutByGPickup()
{
    yl::string strCallOutNumber = acc_GetPickupCodeByType(acc_GetDefaultAccount(), PT_GROUP);

    if (!strCallOutNumber.empty())
    {
        return talklogic_CallOut(strCallOutNumber, "", acc_GetDefaultAccount());
    }

    return false;
}

// 是否存在话路
bool talklogic_SessionExist(bool bIngoreAutoRedialSession/* = false*/)
{
    int iSessionNum = _SessionManager.GetSessionListSize();
    // 如果忽略autoredial session,则当现有话路个数和autoredial session个数一样时，返回false
    if (bIngoreAutoRedialSession && iSessionNum == _SessionManager.GetAutoRedialSessionNum())
    {
        return false;
    }
    else
    {
        return iSessionNum > 0;
    }
}

// 获取话路数
int talklogic_GetSessionNum(bool bExceptHide /*= true*/)
{
    return _SessionManager.GetSessionListSize(bExceptHide);
}

// 根据callid,获取当前路的号码和账号
bool talklogic_GetOriginRemoteInfoByCallID(int iCallID, yl::string& strDisplayName,
        yl::string& strRemoteNumber, yl::string& strRemoteServer)
{
    CSingleSession* pSession = _SessionManager.GetRelateSessionByCallID(iCallID);
    if (NULL == pSession)
    {
        return false;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();
    strDisplayName = pCallInfo->m_tOriginalRemoteInfo.sDisplayName;
    strRemoteNumber = pCallInfo->m_tOriginalRemoteInfo.sSIPName;
    strRemoteServer = pCallInfo->m_tOriginalRemoteInfo.sServerName;
    return true;
}


#if IF_FEATURE_PAGING
//  Paging的呼出
bool talklogic_CallPagingOut(const char* cszDisplay, const char* cszNum, int iDsskey/* = NULL*/,
                             const char* cszChannel/* = ""*/)
{
    if (cszNum == NULL
            || 0 == strlen(cszNum)
            || !commonUnits_BeMulticastIP(cszNum))
    {
        return false;
    }

    if (_SessionManager.BeHavePageOutCall())
    {
        return false;
    }

    return _MulticastPage.CallPageOut(cszDisplay, cszNum, iDsskey, cszChannel);
}

// Paging的呼出
bool talklogic_PagingTalkSwitch(int iCallID, bool bStart)
{
    return _MulticastPage.PagingTalkSwitch(iCallID, bStart);
}

// 重启等待中的Paging请求
bool talklogic_RestartWaitPaging()
{
    return _MulticastPage.RestartWaitPage();
}
#endif

bool talklogic_GetIncomingCallLineType(CALL_LINE_TYPE& eCallLineType)
{
    CSingleSession* pRingSession = _SessionManager.GetFirstIncomingCallSession();
    if (pRingSession == NULL)
    {
        return false;
    }

    CCallInfo* pInfo = pRingSession->GetCallInfo();
    if (NULL == pInfo)
    {
        return false;
    }
    eCallLineType = pInfo->GetCallLineType();
    return true;
}

// 获取通话是否被MUTE
bool talklogic_GetMuteStatus(bool bVideo/* = false*/)
{
    return talklogic_GetPersistentMuteStatus();
}

bool talklogic_IsAllSubSessionHold(int iHostSessionID)
{
    CBaseSession* pSession = _SessionManager.GetSession(iHostSessionID);
    if (pSession == NULL)
    {
        return false;
    }

    YLVector<CBaseSession*> vec;
    if (!pSession->GetSubSessionList(vec))
    {
        return false;
    }

    for (YLVector<CBaseSession*>::iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (!(*it)->IsInHold())
        {
            return false;
        }
    }

    return true;
}

bool talklogic_IsTalkingSessionExist()
{
    return _SessionManager.IsTalkingSessionExist();
}

bool talklogic_IsLocalConfSessionExist()
{
    return _SessionManager.IsLocalConfExist();
}

bool talklogic_IsSessionExist(ROUTINE_STATE eRoutine, int iExceptID/* = INVALID_SESSION_ID*/)
{
    CBaseSession* pSession = NULL;
    if (iExceptID != INVALID_SESSION_ID)
    {
        pSession = _SessionManager.GetSession(iExceptID);
    }

    return _SessionManager.IsSessionExist(eRoutine, pSession);
}

bool talklogic_IsTalkSessionExist()
{
    return talklogic_IsSessionExist(ROUTINE_TALKING);
}

// 是否存在来电Session
bool talklogic_IsRingSessionExist()
{
    return talklogic_IsSessionExist(ROUTINE_RINGING);
}

// 是否存在拨号界面(包括预拨号)
bool talklogic_IsDialSessionExist()
{
    return talklogic_IsSessionExist(ROUTINE_DIALING);
}

// 是否存在去电Session
bool talklogic_IsConnectingSessionExist()
{
    return talklogic_IsSessionExist(ROUTINE_CONNECTING);
}

// 是否存在预拨号界面
bool talklogic_IsPreDialSessionExist()
{
    return talklogic_IsSessionExist(ROUTINE_PREDIAL);
}

// 所有话路都在响铃
bool talklogic_IsAllSessionRinging()
{
    return _SessionManager.IsAllSessionRinging();
}


#ifdef IF_BT_SUPPORT_PHONE
//蓝牙手机账号呼出
bool talklogic_CallMobileOut(const yl::string& strNumber, const yl::string& strName/* = ""*/)
{
    AUTO_WND_UPDATE;
    TALK_INFO("talklogic_CallMobileOut Num[%s]", strNumber.c_str());
    return _BTMobileManager.CallMobileOut(strNumber, strName, -1);
}

//进入蓝牙手机账号拨号界面
bool talklogic_EnterMobileDialUI(const yl::string& strNumber /* = "" */)
{
    AUTO_WND_UPDATE;
    return _BTMobileManager.EnterDialUI(strNumber);
}

//是否允许蓝牙通话
bool talklogic_BeAllowNewBTCall()
{
    return _BTMobileManager.IsAllowNewBTCall();
}

bool talklogic_IsBtSessionExist()
{
    return _BTMobileManager.GetBTSessionNumber(false) > 0;
}

bool talklogic_IsBluetoothSessionBySessionID(int iSessionID)
{
    CBaseSession* pSession = _SessionManager.GetSession(iSessionID);

    if (NULL == pSession)
    {
        return false;
    }

    return pSession->IsBtSession();
}

bool talklogic_IsBluetoothSessionByCallID(int iCallID)
{
#ifdef IF_BT_SUPPORT_PHONE
    CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(iCallID);
    if (NULL == pCallInfo)
    {
        return false;
    }

    return pCallInfo->IsBtTalk();
#else
    return false;
#endif
}
#endif

bool talklogic_HoldSession(int iCallID)
{
    CBaseSession* pSession = _SessionManager.GetSessionByCallID(iCallID);
    if (NULL == pSession)
    {
        return false;
    }

    pSession->HoldSession(true);
    return true;
}

bool talklogic_UnHoldSession(int iCallID)
{
    CBaseSession* pSession = _SessionManager.GetSessionByCallID(iCallID);
    if (NULL == pSession)
    {
        return false;
    }

    pSession->HoldSession(false);
    return true;
}

bool talklogic_AnswerCall(int iCallID)
{
    CSingleSession* pSession = _SessionManager.GetRelateSessionByCallID(iCallID);
    if (NULL == pSession)
    {
        return false;
    }

    return talklogic_UIEvent(pSession->GetSessionID(), TALK_ACTION_ANSWER);
}

bool talklogic_RefuseCall(int iCallID, REFUSE_TYPE eRefuseType/* = RT_NORMAL*/)
{
    CSingleSession* pSession = _SessionManager.GetRelateSessionByCallID(iCallID);
    if (NULL == pSession)
    {
        return false;
    }
    DataUI2Logic data;
    data.eAction = TALK_ACTION_REJECT;
    data.wActionParam = eRefuseType;
    return talklogic_UIEvent(pSession->GetSessionID(), data);
}

bool talklogic_BlindTransfer(int iCallID, const char* pNumber)
{
    CSingleSession* pSession = _SessionManager.GetRelateSessionByCallID(iCallID);
    if (NULL == pSession
#if IF_FEATURE_PAGING
            || pSession->BeMulticastPage()
#endif
       )
    {
        return false;
    }

    return pSession->IsInherits(ROUTINE_TALKING)
           && pSession->Redirectcall(pNumber);
}

// 处理SIP消息
bool talklogic_OnSIPMsg(msgObject& msg)
{
    return _TalklogicManager.OnSIPMsg(msg);
}

bool talklogic_Transfer(int iHeldId, int iTransId)
{
    CSingleSession* pActiveSession = _SessionManager.GetRelateSessionByCallID(iTransId);
    if (NULL == pActiveSession)
    {
        return false;
    }

    CSingleSession* pHeldSession = _SessionManager.GetRelateSessionByCallID(iHeldId);
    if (NULL == pHeldSession)
    {
        return false;
    }

    CTalkRoutine* pTalk = pHeldSession->GetTalkRoutine();
    if (pTalk == NULL)
    {
        // tran发起者必须是通话routine
        return false;
    }

    // 如果followsession是talking中，则咨询转,否则半咨询转
    if (pActiveSession->GetRoutineState() == ROUTINE_TALKING)
    {
        pTalk->AttendedTransfer(pActiveSession);
    }
    else
    {
        pTalk->SemiAttendedTransfer(pActiveSession);
    }

    return true;
}

// 转移来电方
bool talklogic_ForwardIncomingCall(int iCallID, const yl::string& strNumber)
{
    if (strNumber.empty())
    {
        return false;
    }

    CSingleSession* pSession = _SessionManager.GetRelateSessionByCallID(iCallID);
    if (NULL == pSession
            || !pSession->IsInherits(ROUTINE_RINGING))
    {
        return false;
    }

    return pSession->Redirectcall(strNumber.c_str());
}

// 根据SessionID获取Session State
SESSION_STATE talklogic_GetSessionStateBySessionID(int iSessionID)
{
    CBaseSession* pSession = _SessionManager.GetSession(iSessionID);
    if (NULL != pSession)
    {
        return pSession->GetSessionState();
    }
    return SESSION_UNKNOWN;
}

/*******************************************************************
** 函数描述:   获取session数量
**
** 返回:       话路数
********************************************************************/
int talklogic_GetAmountOfSession(bool bExceptHide/* = true*/)
{
    return _SessionManager.GetSessionListSize(bExceptHide);
}

bool talklogic_AlternateCall(int iActiveCallId, int iHeldCallId)
{
    CSingleSession* pActiveSession = _SessionManager.GetRelateSessionByCallID(iHeldCallId);
    if (NULL == pActiveSession)
    {
        return false;
    }

    CSingleSession* pHeldSession = _SessionManager.GetRelateSessionByCallID(iActiveCallId);
    if (NULL == pHeldSession)
    {
        return false;
    }

    pActiveSession->HoldSession(true);
    pHeldSession->HoldSession(false);
    int iSessionID = pHeldSession->GetSessionID();
    talklogic_UIEvent(iSessionID, TALK_ACTION_FOCUSE_CHANGE, iSessionID);
    return true;
}

bool talklogic_ReconnectCall(int iEndCallId, int iRetrievedCallId)
{
    CSingleSession* pActiveSession = _SessionManager.GetRelateSessionByCallID(iEndCallId);
    if (NULL == pActiveSession)
    {
        return false;
    }

    pActiveSession->ExitSession(true);

    CSingleSession* pRetrievedSession = _SessionManager.GetRelateSessionByCallID(iRetrievedCallId);
    if (NULL == pRetrievedSession)
    {
        return false;
    }

    pRetrievedSession->HoldSession(FALSE);
    return true;
}

bool talklogic_ConferenceCall(int iActiveCallId, int iHeldCallId/* = -1*/)
{
    CSingleSession* pActiveSession = _SessionManager.GetRelateSessionByCallID(iActiveCallId);
    if (NULL == pActiveSession)
    {
        return false;
    }

    CBaseSession* pSession = pActiveSession->GetAnotherSession();
    if (pSession == NULL)
    {
        return false;
    }

    CSingleSession* pAnotherSession = pSession->ToSingleSession();
    if (pAnotherSession != NULL)
    {
        pActiveSession->SetSponsorSession(SPONSOR_CONF, pAnotherSession);
    }

    return _SessionManager.EstablishLocalConf(pActiveSession);
}

bool talklogic_IsLocalConf(int nCallId)
{
    CBaseSession* pSession = _SessionManager.GetSessionByCallID(nCallId);
    return pSession && pSession->IsLocalConfSession();
}

bool talklogic_IsLocalConfBySessionID(int iSessionID, bool bHost/* = true*/)
{
    CBaseSession* pSession = _SessionManager.GetSession(iSessionID);
    if (pSession == NULL)
    {
        return false;
    }
    else if (!bHost)
    {
        pSession = pSession->GetThisOrTopestHostSession();
    }

    return pSession && pSession->IsLocalConfSession();
}

bool talklogic_NetConferenceCall(int iActiveCallId, int iHeldCallId, int& iConfSessionID)
{
    CSingleSession* pFirstSession = _SessionManager.GetRelateSessionByCallID(iActiveCallId);
    if (NULL == pFirstSession)
    {
        return false;
    }

    CSingleSession* pSecondSession = _SessionManager.GetRelateSessionByCallID(iHeldCallId);
    if (NULL == pSecondSession)
    {
        return false;
    }

    CTalkRoutine* pTalkRoutine = pFirstSession->GetTalkRoutine();
    if (NULL == pTalkRoutine)
    {
        return false;
    }

    pTalkRoutine->AddNetworkConf(pSecondSession);
    return true;
}

bool talklogic_GetLocalConfCallInfo(int nCallId, CCallInfo** pFirst, CCallInfo** pSencond)
{
    CBaseSession* pSession = _SessionManager.GetSessionByCallID(nCallId);
    if (pSession == NULL || !pSession->IsLocalConfSession())
    {
        return false;
    }

    CBaseSession* pFirstSession = pSession->GetSubSessionByIndex(0);
    if (pFirstSession != NULL && pFirstSession->IsSingleSession())
    {
        *pFirst = pFirstSession->GetCallInfoAsSub();
    }

    CBaseSession* pAnotherSession = pSession->GetSubSessionByIndex(1);
    if (pAnotherSession != NULL && pAnotherSession->IsSingleSession())
    {
        *pSencond = pAnotherSession->GetCallInfoAsSub();
    }

    return *pFirst && *pSencond;
}

CCallInfo* talklogic_GetCallInfoByCallId(yl::string& strCallId)
{
    int nCallId = atoi(strCallId.c_str());
    return talklogic_GetCallInfoByCallId(nCallId);
}

CCallInfo* talklogic_GetCallInfoByCallId(const int nCallId)
{
    CSingleSession* pSession = _SessionManager.GetRelateSessionByCallID(nCallId, false);
    if (NULL == pSession)
    {
        return NULL;
    }

    return pSession->GetCallInfo();
}

CCallInfo* talklogic_GetCallInfoBySessionID(int iSessionId)
{
    CBaseSession* pSession = _SessionManager.GetSession(iSessionId);
    if (NULL == pSession)
    {
        return NULL;
    }

    return pSession->GetCallInfoAsSub();
}

// 获取来电呼叫的账号id
int talklogic_GetIncomingCallAccountId()
{
    CSingleSession* pRingSession = _SessionManager.GetFirstIncomingCallSession();
    if (pRingSession == NULL)
    {
        return INVALID_ACCOUNTID;
    }

    CCallInfo* pInfo = pRingSession->GetCallInfo();

    if (NULL != pInfo)
    {
        return pInfo->GetAccountID();
    }

    return INVALID_ACCOUNTID;
}

// 获取第一路来电话路的sessionid
int talklogic_GetIncomingCallSessionId()
{
    CSingleSession* pRingSession = _SessionManager.GetFirstIncomingCallSession();
    if (pRingSession == NULL)
    {
        return -1;
    }

    return pRingSession->GetSessionID();
}

#ifdef IF_SUPPORT_LINE_DEVICE
// 进入外接设备通话
bool talklogic_EnterLineDeviceTalk()
{
    CallOut_Param param;
    param.eRoutineType = ROUTINE_LINE_DEVICE;
    param.iAccountID = _TalklogicManager.GetUsableAccount(AUTO_SELECT_LINE_ID);
    CCallInfo* pCallInfo = talklogic_CreateSession(param);
    if (pCallInfo)
    {
        pCallInfo->SetCallLineType(CALL_LINE_DEVICE);
        return true;
    }

    return false;
}

// 退出外接设备通话
void talklogic_ExitLineDeviceTalk()
{
    talklogic_ExitSession(ROUTINE_LINE_DEVICE);
}

// 判断是否是外接设备话路
bool talklogic_IsLineDevice(int iCallID)
{
    CBaseSession* pSession = _SessionManager.GetSessionByCallID(iCallID);

    return (NULL != pSession && SESSION_LINEDEVICE == pSession->GetSessionState());
}
#endif

CCallInfo* talklogic_FindCallInfoByCallId(int iCallId)
{
    return talklogic_GetCallInfoByCallId(iCallId);
}

/*******************************************************************
** 函数描述:   获取通话中的话路数量
** 返回:       话路数
********************************************************************/
int talklogic_GetTalkingNumber(bool bVideo/* = false*/)
{
    return _SessionManager.GetTalkingNumber(bVideo);
}

int talklogic_UnHoldSessionID()
{
    LIST_CALL_SESSION& listSession = _SessionManager.GetSessionList();

    int iSessionID = -1;

    for (LIST_CALL_SESSION::iterator itBeg = listSession.begin();
            itBeg != listSession.end();
            ++ itBeg)
    {
        CBaseSession* pSession = (*itBeg);

        if (NULL == pSession
                || pSession->GetHostSession() != NULL
                || !pSession->IsTalkSession())
        {
            continue;
        }

        if (!pSession->IsInHold())
        {
            iSessionID = pSession->GetSessionID();
            break;
        }
    }

    return iSessionID;
}

// 通过sessionid返回callid
int talklogic_GetCallIdBySessionID(int iSessionID)
{
    CBaseSession* pSession = _SessionManager.GetSession(iSessionID);
    if (NULL == pSession)
    {
        return 0;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfoAsSub();
    if (NULL == pCallInfo)
    {
        return 0;
    }

    return pCallInfo->GetCallID();
}

// 通过CallId返回SessionId
int talklogic_GetSessionIdByCallID(int iCallID, bool bHostSession/* = true*/)
{
    CBaseSession* pSession = _SessionManager.GetSessionByCallID(iCallID, bHostSession);
    if (NULL == pSession)
    {
        return INVALID_SESSION_ID;
    }

    return pSession->GetSessionID();
}

bool talklogic_Redial()
{
    yl::string strNumber = configParser_GetConfigString(kszRedialNumber);
    if (!strNumber.empty())
    {
        // 设置redial的名称
        yl::string strName = configParser_GetConfigString(kszRedialName);
        yl::string strFullNumber(strNumber);
        yl::string strServer = configParser_GetConfigString(kszRedialServer);
        yl::string strUser = configParser_GetConfigString(kszRedialUser);

        if (!strServer.empty())
        {
            strFullNumber.append("@");
            strFullNumber.append(strServer);
        }

        int iAccountID = acc_GetAccountIDByName(strUser.c_str(), strServer.c_str());

#if IF_BUG_27560
        if (configParser_GetConfigInt(kszIsRedialHideSipName) == 1)
        {
            actURI_SetHideTalkInfo(true);
        }
#endif

        // 拨出.
        return talklogic_CallOut(strFullNumber, strName.empty() ? strNumber : strName, iAccountID);
    }
    return false;
}

bool talklogic_GetFollowSessionId(int iHostSessionId, int& iFollowSessionId,
                                  SPONSOR_TYPE eSpType /*= SPONSOR_TRAN*/)
{
    CSingleSession* pSession = _SessionManager.GetSingleSession(iHostSessionId);
    if (pSession != NULL)
    {
        bool bInStatus = false;
        switch (eSpType)
        {
        case SPONSOR_TRAN:
            bInStatus = pSession->InTransfer();
            break;
        case SPONSOR_CONF:
            bInStatus = pSession->InPreConf();
            break;
        case SPONSOR_ALL:
            bInStatus = pSession->InTransfer() || pSession->InPreConf();
            break;
        default:
            break;
        }

        if (bInStatus)
        {
            CSingleSession* pFollowSession = pSession->GetFollowSession(eSpType);
            if (pFollowSession != NULL)
            {
                iFollowSessionId = pFollowSession->GetSessionID();
                return iFollowSessionId != INVALID_SESSION_ID;
            }
        }
    }

    return false;
}

bool talklogic_GetSponsorSessionId(int iFollowSessionId, int& iHostSessionId,
                                   SPONSOR_TYPE eSpType/* = SPONSOR_TRAN*/)
{
    CSingleSession* pFollowSession = _SessionManager.GetSingleSession(iFollowSessionId);
    if (NULL == pFollowSession)
    {
        return false;
    }

    CSingleSession* pHostSession = NULL;
    switch (eSpType)
    {
    case SPONSOR_TRAN:
    case SPONSOR_CONF:
        pHostSession = pFollowSession->GetSponsorSession(eSpType);
        break;
    case SPONSOR_ALL:
        pHostSession = pFollowSession->GetSponsorSession(SPONSOR_TRAN);
        pHostSession = (NULL == pHostSession) ? pFollowSession->GetSponsorSession(
                           SPONSOR_CONF) : pHostSession;
        break;
    default:
        break;
    }

    if (pHostSession)
    {
        iHostSessionId = pHostSession->GetSessionID();
        return iFollowSessionId != INVALID_SESSION_ID;
    }

    return false;
}

bool talklogic_GetPersistentMuteStatus()
{
    return modtklsn_get_mute_status();
}

void talklogic_SetPersistentMuteStatus()
{
    talklogic_UIEvent(-1, TALK_ACTION_MUTE);
}

bool talklogic_IsRingingSessionAnswered(int iSessionID)
{
    CSingleSession* pSession = _SessionManager.GetSingleSession(iSessionID);
    if (pSession != NULL)
    {
        if (pSession->IsInherits(ROUTINE_RINGING))
        {
            return pSession->GetCallInfo()->m_bAnswered;
        }

        return pSession->IsInherits(ROUTINE_TALKING);
    }
    return false;
}

bool talklogic_GetSubSesseionID(int iSessionID, YLVector<int>& vec)
{
    if (iSessionID == INVALID_SESSION_ID)
    {
        return false;
    }

    CBaseSession* pHostSession = _SessionManager.GetSession(iSessionID);
    if (pHostSession == NULL)
    {
        return false;
    }

    YLVector<CBaseSession*> vecSession;
    if (!pHostSession->GetSubSessionList(vecSession))
    {
        return false;
    }

    for (YLVector<CBaseSession*>::iterator it = vecSession.begin();
            it != vecSession.end(); ++it)
    {
        vec.push_back((*it)->GetSessionID());
    }

    return vec.size() > 0;
}

int talklogic_GetLocalConfSessionID()
{
    CBaseSession* pSession = _SessionManager.GetConfSession();
    if (pSession == NULL)
    {
        return INVALID_SESSION_ID;
    }

    return pSession->GetSessionID();
}

bool talklogic_BeHavePageOutCall()
{
    return _SessionManager.BeHavePageOutCall();
}

int talklogic_GetHostSessionID(int iSessionID)
{
    if (iSessionID == INVALID_SESSION_ID)
    {
        return INVALID_SESSION_ID;
    }

    CBaseSession* pSession = _SessionManager.GetSession(iSessionID);
    if (pSession == NULL)
    {
        return INVALID_SESSION_ID;
    }

    if (pSession->IsHide())
    {
        return INVALID_SESSION_ID;
    }

    CBaseSession* pHostSession = pSession->GetTopestHostSession();
    if (pHostSession == NULL)
    {
        return INVALID_SESSION_ID;
    }

    return pHostSession->GetSessionID();
}

bool talklogic_IsHoldSession(int iSessionID)
{
    if (iSessionID == INVALID_SESSION_ID)
    {
        return false;
    }

    CBaseSession* pSession = _SessionManager.GetSession(iSessionID);
    if (pSession != NULL)
    {
        return pSession->IsInHold();
    }

    return false;
}

int talklogic_GetCallNumberByAccountID(int iAccountID)
{
    int iCount = 0;
    FOR_SESSION_LIST_EXPORT(it)
    {
        CSingleSession* pSession = (*it)->ToSingleSession();
        if (NULL == pSession)
        {
            continue;
        }

        if (iAccountID == pSession->GetCallInfo()->GetAccountID())
        {
            iCount++;
        }
    }
    return iCount;
}

#ifdef IF_FEATURE_EMERGENCY
bool talklogic_IsEmergencyRecall()
{
    return false; // _SessionManager.IsEmergencyRecall();
}
#endif

int talklogic_MaxSessionNum()
{
    if (devicelib_GetPhoneType() == PT_T19)
    {
        return 2;
    }

    return 50;
}

void talklogic_HoldAllSession(int iExceptID/* = INVALID_SESSION_ID*/, HOLD_TYPE eHold/* = HT_NORMAL*/)
{
    _SessionManager.HoldAllSession(iExceptID, eHold);
}

CCallInfo* talklogic_TraverseCallInfo(CBaseInfoTraver& vistor, bool bExcexpHide/* = true*/)
{
    FOR_SESSION_LIST_EXPORT(it)
    {
        CSingleSession* pSession = (*it)->ToSingleSession();
        if (pSession == NULL || (bExcexpHide && pSession->IsHide()))
        {
            continue;
        }

        CCallInfo* pCallInfo = pSession->GetCallInfo();
        if (vistor(pCallInfo))
        {
            return pCallInfo;
        }
    }

    return NULL;
}

CSessionProxy talklogic_TraverseSession(CBaseSessionTraver& vistor, bool bExcexpHide /*= true*/)
{
    FOR_SESSION_LIST_EXPORT(it)
    {
        CBaseSession* pSession = (*it);
        if (bExcexpHide && pSession->IsHide())
        {
            continue;
        }

        CSessionProxy proxy(pSession);
        if (vistor(proxy))
        {
            return proxy;
        }
    }

    return CSessionProxy();
}

CCallInfo* talklogic_CreateNewRoutine(ROUTINE_STATE eRoutine, int iBasedSession /*= INVALID_SESSION_ID*/)
{
    AUTO_WND_UPDATE;

    if (iBasedSession == INVALID_SESSION_ID)
    {
        CBaseSession* pSession = _SessionManager.GetSession(iBasedSession);
        if (pSession != NULL)
        {
            CSingleSession* pSingle = pSession->CreateNewDial(eRoutine);
            return pSingle ? pSingle->GetCallInfo() : NULL;
        }
    }

    // 新建Session
    CSingleSession* pSession = _SessionManager.AllocSession();
    if (pSession == NULL)
    {
        return NULL;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();
    pCallInfo->SetAccountID(acc_GetDefaultAccount());
    _TalklogicManager.TryEnterRoutine(pSession, eRoutine);
    return pCallInfo;
}

bool talklogic_UIEvent(int iSessionID, DataUI2Logic& action)
{
    CBaseSession* pSession = _SessionManager.GetSession(iSessionID);
    return pSession && pSession->OnActionEvent(&action);
}

CCallInfo* talklogic_ProcessCallOut(const CallOut_Param& param)
{
    AUTO_WND_UPDATE;

    SignalParams sigp;
    sigp.wParam.pValue = &param;

    if (modtkflt_emit(TALK_SIG_BEFORE_CALLOUT, sigp))
    {
        return NULL;
    }

    CSingleSession* pSession = _SessionManager.AllocSession();
    if (pSession == NULL)
    {
        return NULL;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();
    pCallInfo->GetCallOutParam() = param;
    pCallInfo->SetAccountID(param.iAccountID);

    _TalklogicManager.ProcessCallOut(pSession, param.strNumber.c_str(), "");
    return pCallInfo;
}

CCallInfo* talklogic_CreateSession(const CallOut_Param& param)
{
    AUTO_WND_UPDATE;

#ifdef IF_BT_SUPPORT_PHONE
    if (param.eRoutineType == ROUTINE_BTDIALING)
    {
        // 没有通话存在时,进入拨号界面
        CSingleSession* pSession = _BTMobileManager.EnterDialUI(param.strNumber);
        return pSession ? pSession->GetCallInfo() : NULL;
    }
#endif

    CSingleSession* pSession = _SessionManager.AllocSession();
    if (pSession == NULL)
    {
        return NULL;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();
    pCallInfo->GetCallOutParam() = param;
    pCallInfo->SetAccountID(param.iAccountID);

    pCallInfo->m_tRemoteInfo.sDisplayName = param.strName;
    pCallInfo->m_tRemoteInfo.sSIPName = param.strNumber;
    pCallInfo->m_eTalkCallOutType = param.eCallOutType;
    pCallInfo->m_errorInfo.iReason = param.iErrorInfo;

#if IF_BUG_27560
    if (actURI_IsHideTalkInfo())
    {
        pCallInfo->m_bHideInfo = true;
    }
#endif //IF_BUG_27560

    _TalklogicManager.TryEnterRoutine(pSession, param.eRoutineType);
    return pCallInfo;
}

bool talklogic_Redirect(int iCallID, LPCSTR lpTarget)
{
    if (textEMPTY(lpTarget))
    {
        return false;
    }

    CSingleSession* pTargetSession = _SessionManager.GetRelateSessionByCallID(iCallID);
    if (NULL == pTargetSession)
    {
        return false;
    }

    return pTargetSession->Redirectcall(lpTarget);
}
