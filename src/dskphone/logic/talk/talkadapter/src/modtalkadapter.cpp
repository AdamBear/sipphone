#include "talkadapter_inc.h"
#include "talk/talklogic/src/talklogic_inc.h"
#include "talk/talkdsskey/include/dsskeybindmanager.h"

void talklogic_Init()
{
    AUTO_SCOPED_TIMER;

    talklogic_Core_Init();
    modtklsn_init();

    _TalkAdapter;
    _TalkUIData;
}

CBaseSession* GetFocusedSession()
{
    return _SessionManager.GetSession(_TalkAdapter.GetFocusedSessionID());
}

CSingleSession* GetFocusedSingleSession()
{
    return _SessionManager.GetSingleSession(_TalkAdapter.GetFocusedSessionID());
}

// 退出焦点通话
void talklogic_ExitFocusSession()
{
    AUTO_WND_UPDATE;

    CBaseSession* pFocusSession = GetFocusedSession();
    if (NULL == pFocusSession)
    {
        return;
    }

    if (pFocusSession->IsInherits(ROUTINE_RINGING))
    {
        int iCallid = talklogic_GetCallIdBySessionID(pFocusSession->GetSessionID());
        talklogic_RefuseCall(iCallid);
    }
    else
    {
        //// http://10.3.5.199/Bug.php?BugID=61681 三方会议中主持方无法用蓝牙挂断通话
        pFocusSession->ExitSessionAndSub(true);
    }
}

// 获取焦点话路的Session在list中的序号
int talklogic_GetFocusedSessionIndex()
{
    return _TalkAdapter.GetFocusedSessionIndex();
}

// 获取焦点话路的SessionID
int talklogic_GetFocusedSessionID()
{
    return _TalkAdapter.GetFocusedSessionID();
}

// 根据SessionID获取话路的数据
bool talklogic_GetDataBySessionID(int iSessionID, DataLogic2UI& dataToUI)
{
    return _TalkUIData.GetDataBySessionID(iSessionID, dataToUI);
}

bool talklogic_GetBaseDataBySessionID(int iSessionID, DataLogic2UI& dataToUI)
{
    if (configParser_GetConfigInt(kszIncomingCallIgnor) != 0)
    {
        // 非焦点的RingSession不显示
        CBaseSession* pSession = GetFocusedSession();
        if (pSession != NULL
                && pSession->GetSessionID() != iSessionID
                && pSession->IsInherits(ROUTINE_RINGING))
        {
            return false;
        }
    }

    return _TalkUIData.GetDataBySessionID(iSessionID, dataToUI, true);
}

// 函数描述:   根据SessionBlaIndex获取话路的数据
bool talklogic_GetDataByAccountBlaIndex(int iAccount, int iBlaIndex, DataLogic2UI& dataToUI)
{
    CSessionStateProxy proxy = talklogic_GetSessionByBLAIndex(iAccount, iBlaIndex);
    return talklogic_GetDataBySessionID(proxy, dataToUI);
}

TALK_STATE talklogic_GetFocusedSessionTalkState()
{
    CBaseSession* pSession = GetFocusedSession();
    if (NULL == pSession)
    {
        return TS_UNKNOW;
    }

    bool bHold = pSession->IsInHold();
    bool bHeld = pSession->IsInHeld();

    if (bHold && bHeld)
    {
        return TS_HOLD_HELD;
    }
    else if (bHold)
    {
        return TS_HOLD;
    }
    else if (bHeld)
    {
        return TS_HELD;
    }

    return TS_TALK;
}

bool talklogic_GetFocusedSessionHideState()
{
    CBaseSession* pSession = GetFocusedSession();
    return pSession == NULL || pSession->IsHide();
}

// 获取允许作为TransferTo Target的话路
int talklogic_GetNumberOfAllowTranTo()
{
    CSessionStateProxy proxy;
    return proxy.GetNumberOfAllowTranTo();
}

// 获取Aoc的信息
bool talklogic_GetFocusedSessionTalkState(DataAoc& tAocData, int iSessionId)
{
    CBaseSession* pSession = _SessionManager.GetSession(iSessionId);
    if (NULL == pSession)
    {
        return false;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfoAsSub();
    if (pCallInfo == NULL)
    {
        return false;
    }

    return _AOCManager.GetAocInfo(tAocData, pCallInfo->GetCallID());
}

bool talklogic_IsFoucsInSession(ROUTINE_STATE eState)
{
    CBaseSession* pSession = GetFocusedSession();
    if (pSession == NULL)
    {
        return false;
    }

    return pSession->IsInherits(eState);
}

bool talklogic_IsFoucsInConnectingSession()
{
    return talklogic_IsFoucsInSession(ROUTINE_CONNECTING);
}

bool talklogic_IsFocusInTalkSession()
{
    return talklogic_IsFoucsInSession(ROUTINE_TALKING);
}

bool talklogic_IsFocusInPreDialSession()
{
    return talklogic_IsFoucsInSession(ROUTINE_PREDIAL);
}

bool talklogic_IsFoucsInRingSession()
{
    return talklogic_IsFoucsInSession(ROUTINE_TALKING);
}

/*******************************************************************
** 函数描述:    切换焦点通话
** 参数:      [in] bSwitchDown:          true为往下切换，false为往上切换
** 返回值:       无
********************************************************************/
bool talklogic_SwitchFocusSession(bool bSwitchDown)
{
    YLVector<int> vctSectionId;
    talklogic_GetAllSessionID(vctSectionId);
    int iCurrentFocus = talklogic_GetFocusedSessionIndex();
    int iFocusIndex = iCurrentFocus;

    if (vctSectionId.size() > 1)
    {
        if (bSwitchDown)
        {
            if (iFocusIndex < vctSectionId.size() - 1)
            {
                iFocusIndex++;
            }
            else
            {
                iFocusIndex = 0;
            }
        }
        else
        {
            if (iFocusIndex > 0)
            {
                iFocusIndex--;
            }
            else
            {
                iFocusIndex = vctSectionId.size() - 1;
            }
        }

        if (configParser_GetConfigInt(kszIncomingCallIgnor) != 0)
        {
            CBaseSession* pSession = _SessionManager.GetSession(vctSectionId[iFocusIndex]);
            while (pSession)
            {
                if (bSwitchDown)
                {
                    if (iFocusIndex < vctSectionId.size() - 1)
                    {
                        iFocusIndex++;
                    }
                    else
                    {
                        iFocusIndex = 0;
                    }
                }
                else
                {
                    if (iFocusIndex > 0)
                    {
                        iFocusIndex--;
                    }
                    else
                    {
                        iFocusIndex = vctSectionId.size() - 1;
                    }
                }

                if (iCurrentFocus == iFocusIndex)
                {
                    TALK_WARN("can not find next focus session");
                    return false;
                }
            }
        }
    }

    // 如果焦点Session有切换则更新界面
    return _TalkAdapter.SetFocusSession(vctSectionId[iFocusIndex], true);
}

// UI给Logic发送UI事件
bool talklogic_UIEvent(int iSessionID, int iActionType, WPARAM wActionParam/* = 0 */,
                       LPARAM lActionParam/* = 0 */, void* pExtraData/* = NULL */)
{
    TALK_INFO("Talkui Action to logic SessionID[%d] ActionType[%d] wP[%d] lP[%d]",
              iSessionID, iActionType, wActionParam, lActionParam);

    //构造传递给Logic层数据对象
    DataUI2Logic objUI2LogicData;
    //进行相应赋值
    objUI2LogicData.eAction = (TALK_ACTION_TYPE)iActionType;
    objUI2LogicData.wActionParam = wActionParam;
    objUI2LogicData.lActionParam = lActionParam;
    objUI2LogicData.pData = pExtraData;
    return _TalkAdapter.ProcessAction(iSessionID, objUI2LogicData);
}

bool talklogic_SetFocusSessionByID(int iSessionID)
{
    int iFocusIndex = _TalkAdapter.GetFocusedSessionID();

    if (_TalkAdapter.GetSessionListSize() <= 1
            || iFocusIndex == iSessionID)
    {
        TALK_WARN("Not need set focus session");
        return false;
    }

    // 如果焦点Session有切换则更新界面
    return _TalkAdapter.SetFocusSession(iSessionID, true);
}

// 刷新当前话路
bool talklogic_UpdateWnd()
{
    CScopedWNDUpdate::SetMethod();
    AUTO_WND_UPDATE;
    return true;
}

// 获取来电号码
bool talklogic_GetIncomingCallNum(yl::string& strNum)
{
    return configParser_GetConfigInt(kszIncomingCallIgnor) == 0
           && _TalkUIData.GetIncomingCallName(strNum);
}

// 是否允许新建通话
bool talklogic_BeAllowNewCall(LPCSTR lpNum/* = NULL*/, bool bCheckForIncomingCall /* = false */)
{
    return !talklogic_IsMaxSessionNum(bCheckForIncomingCall) || Emergency_IsEmergencyMode(lpNum);
}

// 是否允许两个session互转
bool talklogic_IsAllowTranTo(int iSessionIDFirst, int iSessionIDSec)
{
    CBaseSession* pSession1 = _SessionManager.GetSession(iSessionIDFirst);
    CBaseSession* pSession2 = _SessionManager.GetSession(iSessionIDSec);

    if (pSession2 == NULL || pSession1 == NULL)
    {
        return false;
    }

    return CSessionStateProxy(pSession1).IsAllowTranTo(pSession2->GetSessionID());
}

#if IF_FEATURE_START2START_ACD
bool talkLogic_SetCalloutType(TALK_CALLOUT_TYPE eType)
{
    CBaseSession* pBaseSession = GetFocusedSession();
    if (pBaseSession)
    {
        CCallInfo* pCallInfo = pBaseSession->GetCallInfoAsSub();
        if (pCallInfo)
        {
            pCallInfo->m_eTalkCallOutType = eType;
            return true;
        }
    }

    return false;
}
#endif  //IF_FEATURE_START2START_ACD

bool talklogic_IsAllowNewCallByAccount(int iAccountID)
{
    return _DsskeyBindManager.IsAllowNewCallByAccount(iAccountID);
}

// 获取允许加入会议SessionID
bool talklogic_GetAllowToConfSessionID(YLVector<int>& vecSessionID)
{
    return CSessionStateProxy().GetSessionToConf(vecSessionID);
}

//是否可以加入本地会议的话路数超过本地会议最大路数
bool talklogic_IsAllowAddLocalConfNumOverMax()
{
    return CSessionStateProxy().IsAllowAddLocalConfNumOverMax();
}

bool talklogic_CallRecord(int nCallId /*= -1*/)
{
    // http://192.168.1.99/Bug.php?BugID=20749该技术支持中的需求
    CSingleSession* pSession = _SessionManager.GetRelateSessionByCallID(nCallId);
    if (pSession == NULL)
    {
        pSession = GetFocusedSingleSession();
    }

    if (pSession == NULL)
    {
        return false;
    }

    return talklogic_UIEvent(pSession->GetSessionID(), TALK_ACTION_RECORD_RESULT, -1, DT_RECORD);
}

bool talklogic_IsDsskeyBindSession(int iDsskey)
{
    return _DsskeyBindManager.GetSessionByDsskey(iDsskey);
}

int talklogic_GetBindDsskeyBySession(int iSessionID, bool bLinekey /*= true*/)
{
    CBaseSession* pSession = _SessionManager.GetSession(iSessionID);
    if (pSession == NULL)
    {
        return INVALID_DSSKEY_ID;
    }

    return _DsskeyBindManager.GetDsskeyBySession(pSession, bLinekey);
}

//更新远程显示信息
bool talklogic_UpdateRemoteDispaly(int iCallID, const yl::string& strDisplay, yl::string strURI/* = ""*/)
{
    CBaseSession* pSession = _SessionManager.GetSessionByCallID(iCallID, false);

    if (NULL == pSession)
    {
        return false;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfoAsSub();

    if (NULL == pCallInfo)
    {
        return false;
    }

    pCallInfo->m_tRemoteInfo.sDisplayName = strDisplay;
    if (!strURI.empty())
    {
        pCallInfo->m_tRemoteInfo.sSIPName = strURI;
    }
    return true;
}

#ifdef IF_BT_SUPPORT_PHONE
bool talklogic_IsFoucsInBluetoothDialSession()
{
    return talklogic_IsFoucsInSession(ROUTINE_BTDIALING);
}
#endif //IF_BT_SUPPORT_PHONE

yl::string talklogic_HidePSW(yl::string strCallNum)
{
    yl::string::const_pointer lpszContent = strCallNum.c_str();
    int iLength = strCallNum.size();

    yl::string strPrefix = configParser_GetConfigString(kszPSWDialPrefix);
    int iPrefixLength = strPrefix.size();

    if (configParser_GetConfigInt(kszEnablePSWDial) == 0
            || iLength == 0
            || iLength < iPrefixLength)
    {
        return strCallNum;
    }

    if (configParser_GetConfigInt(kszEnablePSWDial) != 0
            && !strPrefix.empty()
            && strstr(lpszContent, strPrefix.c_str()) == lpszContent)
    {
        strCallNum.reserve(iLength);
        for (int i = iPrefixLength; i < iLength
                && i < iPrefixLength + configParser_GetConfigInt(kszPSWDialLength); ++i)
        {
            strCallNum[i] = '*';
        }
    }

    return strCallNum;
}

//根据SessionID获取网络会议member信息
YLList<DataNetConfUserInfo> talklogic_GetNetUserInfo(int iSessionID)
{
    CListenInfo* priv = modtklsn_get_privdata(iSessionID);
    if (priv != NULL)
    {
        return priv->m_listNetUserInfo;
    }
    return YLList<DataNetConfUserInfo>();
}

//根据SessionID判断网络会议是否为本地创建
bool talklogic_IsNetConfCreator(int iSessionID)
{
    CSingleSession* pSession = _SessionManager.GetSingleSession(iSessionID);
    return pSession != NULL && pSession->IsInherits(ROUTINE_NETWORKCONF);
}

//根据SessionID获取网络会议title信息
yl::string talklogic_GetNetConfTitle(int iSessionID)
{
    CListenInfo* priv = modtklsn_get_privdata(iSessionID);
    if (priv != NULL)
    {
        return priv->m_strConfName;
    }

    return "";
}

SESSION_STATE talklogic_GetCurrentStatus(yl::string& strCallId, yl::string& strAccountInfo)
{
    SESSION_STATE eStatus = SESSION_UNKNOWN;

    CBaseSession* pSession = GetFocusedSession();
    if (pSession == NULL)
    {
        return eStatus;
    }

    char szBuf[256] = { 0 };
    CCallInfo* pCallInfo = pSession->GetCallInfoAsSub();
    if (pCallInfo == NULL || pCallInfo->GetAccountID() == INVALID_ACCOUNTID)
    {
        return eStatus;
    }

    if (!pCallInfo->m_strConfId.empty())
    {
        strCallId = pCallInfo->m_strConfId;
    }
    else
    {
        strCallId = commonAPI_FormatString("%d", pCallInfo->GetCallID());
    }

    strAccountInfo = "sip:";
    strAccountInfo.append(acc_GetFullName(pCallInfo->GetAccountID()));
    return pSession->GetSessionState();
}

// 如果允许transfer,则进行transfer呼出,准备进行咨询转
void talklogic_AttendedTransfer(const yl::string& strNumber)
{
    if (strNumber.empty())
    {
        return;
    }

    CSingleSession* pSession = GetFocusedSingleSession();
    if (NULL == pSession)
    {
        return;
    }

    talklogic_AttendedTransfer(pSession->GetCallInfo()->GetCallID(), strNumber);
}

// 咨询转号码
bool talklogic_AttendedTransfer(int iCallID, const yl::string& strNumber)
{
    if (strNumber.empty())
    {
        return false;
    }

    CSingleSession* pSponorSession = _SessionManager.GetRelateSessionByCallID(iCallID);
    if (NULL == pSponorSession)
    {
        return false;
    }

    CSessionStateProxy proxy(pSponorSession);
    CallOut_Param param;
    param.strNumber = strNumber;
    return proxy.AttendedTransfer(param);
}

// 如果允许transfer,盲转当前话路到lpNum
void talklogic_BlindTransfer(const yl::string& strNumber)
{
    if (strNumber.empty())
    {
        return;
    }

    CSingleSession* pSession = GetFocusedSingleSession();
    if (NULL == pSession)
    {
        return;
    }

    if (NULL == pSession
#if IF_FEATURE_PAGING
            || pSession->BeMulticastPage()
#endif
       )
    {
        return;
    }

    talklogic_BlindTransfer(pSession->GetCallInfo()->GetCallID(), strNumber.c_str());
}

// 接起第一路来电
bool talklogic_AcceptIncomingCall(int iCallID /*= -1*/)
{
    CSingleSession* pSession = _SessionManager.GetRelateSessionByCallID(iCallID);
    if (pSession == NULL)
    {
        pSession = GetFocusedSingleSession();
    }

    if (NULL == pSession)
    {
        return false;
    }

    return pSession->ProcessIncomingCall(true);
}

// 用lpNum填充拨号界面
void talklogic_FillCallNum(LPCSTR lpNum, int iAccount /*= -1*/)
{
    if (textEMPTY(lpNum))
    {
        return;
    }

    CBaseSession* pFocusSession = GetFocusedSession();
    if (NULL == pFocusSession)
    {
        talklogic_EnterDialUI(lpNum, iAccount);
        return;
    }

    CSingleSession* pSingleSession = pFocusSession->ToSingleSession();
    if (NULL != pSingleSession)
    {
        pSingleSession->GetCallInfo()->SetDialedNumber(lpNum);
    }
}

// 获取通话和彩铃的状态
void talklogic_GetTalkAndColorRingbackStatus(bool& bTalk, bool& bColorRingback, bool& bPaging)
{
    bPaging = false;
    CBaseSession* pSession = GetFocusedSession();
    if (pSession == NULL)
    {
        // 如果不存在焦点Session，则表示不存在通话
        bTalk = false;
        bColorRingback = false;
    }
    else
    {
        if (pSession->IsTalkSession())
        {
            bTalk = true;

            // BeTransferSession需要特殊处理
            CBeTransferredSession* pBetranSession = pSession->ToBeTransferredSession();
            if (pBetranSession != NULL)
            {
                CSingleSession* pSubSession = pBetranSession->GetSession();
                if (pSubSession != NULL)
                {
                    bTalk = pSubSession->IsInherits(ROUTINE_TALKING);
                    bColorRingback = pSubSession->GetCallInfo()->m_bIsColorRingback;
                }

                return;
            }
        }
        // 如果焦点是来电Session但是又存在通话中的话路，则音量用通话的
        else if (pSession->IsInherits(ROUTINE_RINGING)
                 && _SessionManager.IsSessionExist(ROUTINE_TALKING))
        {
            bTalk = true;
        }

        // 如果存在通话，则根据当前界面的通话状态来判断音量类型
        CSingleSession* pSingleSession = pSession->ToSingleSession();
        if (pSingleSession != NULL)
        {
#if IF_FEATURE_PAGING
            bPaging = pSingleSession->BeMulticastPage()
                      && pSingleSession->IsInTalking();
#endif
            CCallInfo* pCallInfo = pSingleSession->GetCallInfo();
            if (pCallInfo->m_bIsColorRingback)
            {
                bColorRingback = true;
            }
            else if (pSession->IsInherits(ROUTINE_RINGING)
                     && pCallInfo->IsTalkStart())
            {
                bTalk = true;
            }
        }
    }
}

bool talklogic_SendDtmf(int nCallId, const yl::string& strKey, bool bPlayVoice/* = true*/, int interval/* = 0*/)
{
    CBaseSession* pSession = _SessionManager.GetSessionByCallID(nCallId);
    if (pSession == NULL)
    {
        pSession = GetFocusedSession();
    }

    if (pSession == NULL)
    {
        return false;
    }

    CSingleSession* pActiveSession = pSession->GetSession();
    if (NULL == pActiveSession)
    {
        return false;
    }

    pActiveSession->SendDTMF(strKey, interval);
    return true;
}

bool talkLogic_IsSessionInTalking(int iSessionID/* = INVALID_SESSION_ID*/, bool bSkipHoldHeld/* = false*/)
{
    CBaseSession* pSession = _SessionManager.GetSession(iSessionID);
    if (pSession == NULL)
    {
        pSession = GetFocusedSession();
    }

    if (NULL == pSession)
    {
        return false;
    }

    // 单路或者
    return pSession->IsInherits(ROUTINE_TALKING)
           || (!bSkipHoldHeld || (!pSession->IsInHold() && !pSession->IsInHeld()));
}

bool talkLogic_IsSessionCallIncoming()
{
    CBaseSession* pSession = GetFocusedSession();
    return pSession && pSession->IsInherits(ROUTINE_RINGING);
}

int talklogic_GetSessionAccount(int iSessionID/* = INVALID_SESSION_ID*/)
{
    CBaseSession* pSession = _SessionManager.GetSession(iSessionID);
    if (pSession == NULL)
    {
        pSession = GetFocusedSession();
    }

    CCallInfo* pCallInfo = pSession->GetCallInfoAsSub();
    if (pCallInfo == NULL)
    {
        return INVALID_ACCOUNTID;
    }

    return pCallInfo->GetAccountID();
}

#if IF_FEATURE_PAGING
int talklogic_GetMulticastPageSessionID(int iCallID /*= -1*/)
{
    CSingleSession* pSession = NULL;
    if (iCallID != -1)
    {
        pSession = _SessionManager.GetRelateSessionByCallID(iCallID);
    }
    else
    {
        if (_SessionManager.IsTalkingSessionExist())
        {
            CBaseSession* pTalkRoutine = _SessionManager.GetTalkSession();
            if (pTalkRoutine != NULL)
            {
                pSession = pTalkRoutine->ToSingleSession();
            }
        }
        else
        {
            pSession = GetFocusedSingleSession();
        }
    }

    if (pSession == NULL)
    {
        return INVALID_SESSION_ID;
    }

    return pSession->GetSessionID();
}

bool talklogic_IsMulticastPage(int iCallID/* = -1*/)
{
    int iSessionID = talklogic_GetMulticastPageSessionID(iCallID);
    CSingleSession* pSession = _SessionManager.GetSingleSession(iSessionID);
    return pSession != NULL && pSession->BeMulticastPage();
}

int talklogic_GetPagingVolume(int iCallID/* = -1*/)
{
    int iSessionID = talklogic_GetMulticastPageSessionID(iCallID);
    CSingleSession* pSession = _SessionManager.GetSingleSession(iSessionID);

    if (pSession == NULL || !pSession->BeMulticastPage())
    {
        return 0;
    }

    int iID = pSession->GetCallInfo()->GetCallID();
    int iVolume = configParser_GetCfgItemIntValue(kszMulticastListenVolume, iID);
    if (iVolume <= 0
            || iVolume > 15)
    {
        iVolume = 0;
    }

    return iVolume;
}
#endif

bool talklogic_IsMaxSessionNum(bool bCheckForIncomingCall /*= false*/)
{
    int iCallNum = 0;
    FOR_SESSION_LIST_EXPORT(it)
    {
        CBaseSession* pSession = (*it);
        if (pSession->IsHide())
        {
            continue;
        }

        // 判断单路通话路数
        if (pSession->IsSingleSession())
        {
            // 预拨号、拨号等状态不算入通话
            if (pSession->IsInherits(ROUTINE_DIALING)
                    || pSession->IsInherits(ROUTINE_FINISHED)
                    || pSession->IsInherits(ROUTINE_AOC_FINISH))
            {
                continue;
            }
        }
        else
        {
            if (pSession->IsLocalConfSession())
            {
                // 本地会议的话路算2个Session，因为还可以再拆开
                iCallNum += pSession->GetSubSessionSize();
                continue;
            }
        }

        // 其余情况记入1路
        ++iCallNum;
    }

    int iMax = talklogic_MaxSessionNum();
    // 来电时判断是否超出Session限制，此时来电尚未创建Session，因此当前会话数等于限制数视为超过。
    // 去电时判断是否超出Session限制，此时已创建去电Session，因此当前会话数等于限制数视为未超过。
    return iCallNum > iMax || (bCheckForIncomingCall && iCallNum == iMax);
}

bool talklogic_ExitSession(int iCallID)
{
    CBaseSession* pSession = _SessionManager.GetSessionByCallID(iCallID);
    if (NULL == pSession)
    {
        return false;
    }

    if (pSession->IsInherits(ROUTINE_RINGING_CLS))
    {
        talklogic_RefuseCall(iCallID);
    }
    else
    {
        pSession->ExitSessionAndSub(true);
    }
    return true;
}

bool talklogic_ExitSession(ROUTINE_STATE eRoutine)
{
    return _SessionManager.ExitSession(eRoutine);
}

void talklogic_ExitAllSession()
{
    _SessionManager.ExitAllSession();
}

bool talklogic_IsBlaIndexUsing(int iIndex)
{
    return _TalkAdapter.IsBlaIndexUsing(iIndex);
}

int talklogic_GetSessionByBLAIndex(int iAccountID, int iIndex)
{
    return _TalkAdapter.GetSessionByBLAIndex(iAccountID, iIndex);
}
