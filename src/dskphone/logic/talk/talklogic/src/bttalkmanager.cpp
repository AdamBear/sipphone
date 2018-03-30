#include "talklogic_inc.h"

#define MAX_MOBILESESSION_SIZE  2

#ifdef IF_BT_SUPPORT_PHONE
LRESULT OnBluetoothMsg(msgObject& objMsg)
{
    if (objMsg.message != BLUETOOTH_MSG_MOBILE_STATECHANGE)
    {
        return FALSE;
    }

    if (1 == objMsg.wParam)
    {
        return _BTMobileManager.OnMobileMsg(objMsg);
    }
    else if (0 == objMsg.wParam)
    {
        if (0 == objMsg.lParam)
        {
            _BTMobileManager.ExitMobileSync();
        }
        return TRUE;
    }

    return FALSE;
}

CBTTalkManager::CBTTalkManager()
{
    m_bChannelLinked = false;
    etl_RegisterMsgHandle(BLUETOOTH_MSG_MOBILE_STATECHANGE, BLUETOOTH_MSG_MOBILE_STATECHANGE,
                          OnBluetoothMsg);
}

CBTTalkManager::~CBTTalkManager()
{
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_MOBILE_STATECHANGE, BLUETOOTH_MSG_MOBILE_STATECHANGE,
                            OnBluetoothMsg);
}

void CBTTalkManager::TranslateMsgToSip(CBaseSession* pSession, msgObject& msg,
                                       BTTalkCallUserInfo& btCallInfo)
{
    if (NULL == pSession
            || !pSession->IsLocalConfSession())
    {
        return ;
    }

    msg.lParam = btCallInfo.iCallID;
}

//处理手机通话状态改变
bool CBTTalkManager::ProcessMBCallStatusChange(msgObject& msg)
{
    if (NULL == msg.GetExtraData())
    {
        return false;
    }

    BTTalkCallUserInfo callInfo;
    MapBTCallUserInfo(callInfo, (const void*)msg.GetExtraData());

    TALK_INFO("Mobile Call Status CallID[%d], Call Status[%d], Hold Status[%d], Number[%s]"
              , callInfo.iCallID
              , callInfo.iCallStatus
              , callInfo.iHoldStatus
              , callInfo.strNum.c_str()
             );

    bool bHandle = false;
    switch (callInfo.iCallStatus)
    {
    case BT_MB_CALL_STATUS_INCOME: //手机来电
        {
            ProcessIncommingCall(callInfo.strNum, callInfo.iCallID);
            bHandle = true;
        }
        break;
    case BT_MB_CALL_STATUS_MOBILE_TRYING: //手机去电
        {
            bHandle = CallMobileOut(callInfo.strNum, "", callInfo.iCallID);
        }
        break;
    case BT_MB_CALL_STATUS_ESTABLISHED: //通话建立
        {
            CBaseSession* pSession = _SessionManager.GetSessionByCallID(callInfo.iCallID, false);

            TranslateMsgToSip(pSession, msg, callInfo);

            if (NULL != pSession)
            {
                pSession->OnSIPMsg(msg);
            }
            else
            {
                //直接进入通话状态
                //如：手机在通话中与话机建立连接
                EstablishTalk((void*)&callInfo);
            }
            bHandle = true;
        }
        break;
    case BT_MB_CALL_STATUS_RINGBACK:
        {
            CBaseSession* pSession = _SessionManager.GetSessionByCallID(callInfo.iCallID, false);
            TranslateMsgToSip(pSession, msg, callInfo);
            if (NULL != pSession)
            {
                bHandle = pSession->OnSIPMsg(msg);
            }
            else
            {
                // 直接进入呼出回铃状态
                bHandle = CallMobileOut(callInfo.strNum, "", callInfo.iCallID);
            }
        }
        break;
    default:
        {
            CBaseSession* pSession = _SessionManager.GetSessionByCallID(callInfo.iCallID, false);
            TranslateMsgToSip(pSession, msg, callInfo);

            if (NULL != pSession)
            {
                bHandle = pSession->OnSIPMsg(msg);
            }
            else
            {
                TALK_WARN("no session own call id[%d]", callInfo.iCallID);
            }
        }
        break;
    }

    return bHandle;
}

//消息处理
bool CBTTalkManager::OnMobileMsg(msgObject& msg)
{
    if (BLUETOOTH_MSG_MOBILE_STATECHANGE != msg.message
            || NULL == msg.GetExtraData())
    {
        return false;
    }

    const btkit_msg_t* pMsg = (const btkit_msg_t*)msg.GetExtraData();

    bool bHandle = true;
    switch (pMsg->bt_event)
    {
    case BT_AG_CALL_STATUS_CHANGED:
        {
            bHandle = ProcessMBCallStatusChange(msg);
        }
        break;
    case BT_AG_SCO_PLAY_START:
    case BT_AG_SCO_PLAY_STOP:
        {
            bHandle = ProcessLinkStatusChange(pMsg);
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

//呼出
bool CBTTalkManager::CallMobileOut(const yl::string& strNum, const yl::string& strName/* = ""*/,
                                   int nCallID/* = -1*/, bool bColorRingback/* = false*/)
{
    if (strNum.empty())
    {
        return false;
    }

    int iAccountID = Bluetooth_GetBtAccountID();

    CSingleSession* pSession = _SessionManager.AllocSession(nCallID);
    if (pSession == NULL)
    {
        return false;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();
    pCallInfo->SetAccountID(iAccountID);
    pCallInfo->m_tRemoteInfo.sDisplayName = strName;
    pCallInfo->m_bIsColorRingback = bColorRingback;
    pCallInfo->SetCallLineType(CALL_LINE_BT);

    bool bResult = ProcessCallOut(pSession, pCallInfo, strNum, strName);
    return bResult;
}

//处理呼出话路
bool CBTTalkManager::ProcessCallOut(CSingleSession* pSingleSession, CCallInfo* pCallInfo,
                                    const yl::string& strNum, const yl::string& strName/* = ""*/)
{
    if (NULL == pSingleSession
            || NULL == pCallInfo
            || strNum.empty())
    {
        return false;
    }

    if (!Bluetooth_IsMobileConnected())
    {
        pCallInfo->m_errorInfo.strError = "Mobile Not Connected";
        pSingleSession->EnterRoutine(ROUTINE_BTFINISHED);
        return false;
    }

    yl::string strReason;

    if (!IsAllowNewBTCall())
    {
        pCallInfo->m_errorInfo.strError = "MaxSessionError";
        pSingleSession->EnterRoutine(ROUTINE_BTFINISHED);
    }
    else if (DoCallOut(strNum, strName, pCallInfo, strReason))
    {
        pSingleSession->EnterRoutine(ROUTINE_BTCONNECTING);
    }
    else
    {
        pCallInfo->m_errorInfo.strError = strReason;
        pSingleSession->EnterRoutine(ROUTINE_BTFINISHED);
    }

    return true;
}

//处理呼出号码及信息
bool CBTTalkManager::DoCallOut(const yl::string& strNum, const yl::string& strName,
                               CCallInfo* pCallInfo, yl::string& strReason)
{
    if (strNum.empty()
            || NULL == pCallInfo)
    {
        return false;
    }

    // 记录去电的时间
    pCallInfo->m_eCallType = CT_OUTGOING;

    //移除非法字符
    yl::string strTemp = strNum;
    RemoveInvalidChar(strTemp);
    pCallInfo->m_tRemoteInfo.sSIPName = strTemp;
    pCallInfo->m_tRemoteInfo.sDisplayName = strName;

    // 设置联系人信息,T4X需要查找头像
    pCallInfo->SetContactInfo(strName.empty());

    //获取CallID
    if (pCallInfo->GetCallID() == -1)
    {
        pCallInfo->SetCallID(Bluetooth_CallMobileInvite(pCallInfo->m_tRemoteInfo.sSIPName.c_str()));
    }

    if (pCallInfo->GetCallID() == -1)
    {
        strReason = "Connect Mobile Failed";
        return false;
    }

    TALK_INFO("callout mobile[%s][%s] call id[%d]", pCallInfo->m_tRemoteInfo.sSIPName.c_str(),
              pCallInfo->m_tRemoteInfo.sDisplayName.c_str(), pCallInfo->GetCallID());

    return true;
}

//来电处理
bool CBTTalkManager::ProcessIncommingCall(const yl::string& strNum, int nCallID)
{
    if (strNum.empty()
            || nCallID == -1
            || !Bluetooth_IsMobileConnected())
    {
        return false;
    }

    //是否开启了DND
    if (configParser_GetConfigInt(kszMobileDNDEnable) == 1
            && dnd_IsEnable(acc_GetDefaultAccount()))
    {
        return false;
    }

    //通话限制与CallWaitting 开关
    if (!IsAllowNewBTCall()
            || (!talkhelp_IsCallWaitingEnable()
                && _SessionManager.GetSessionListSize() > 0))
    {
        return false;
    }

    //新建Session与CallInfo
    CSingleSession* pSession = _SessionManager.AllocSession(nCallID);
    if (pSession == NULL)
    {
        return false;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();
    pCallInfo->m_eCallType = CT_INCOMING;
    pCallInfo->SetCallLineType(CALL_LINE_BT);

    //主叫方信息
    pCallInfo->m_tRemoteInfo.sSIPName = strNum;
    pCallInfo->SetContactInfo();

    TALK_INFO("Mobile New Incoming Call By Number [%s], Ringing", strNum.c_str());
    pSession->EnterRoutine(ROUTINE_BTRINGING);

    //被叫方信息
    const MobileProperty& mbProperty = Bluetooth_GetMobileProperty();
    pCallInfo->m_tLocalInfo.sSIPName = mbProperty.strNum;

    return true;
}

//进入拨号界面
CSingleSession* CBTTalkManager::EnterDialUI(const yl::string& strNumber /*= ""*/, ROUTINE_STATE eRoutine/* = ROUTINE_BTDIALING*/)
{
    if (!Bluetooth_IsMobileConnected())
    {
        return NULL;
    }

    CSingleSession* pSession = _SessionManager.AllocSession(-1);
    if (pSession == NULL)
    {
        return NULL;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();
    pCallInfo->SetCallLineType(CALL_LINE_BT);

    if (!IsAllowNewBTCall())
    {
        pCallInfo->m_errorInfo.strError = "MaxSessionError";
        pSession->EnterRoutine(ROUTINE_BTFINISHED);
    }
    else
    {
        pSession->EnterRoutine(eRoutine);
        pCallInfo->SetDialedNumber(strNumber);
    }

    return pSession;
}

//进入预拨号界面
CSingleSession* CBTTalkManager::EnterPredialUI(int iKey)
{
    // 获取数字键按下时对应的字符串
    yl::string strNumber = GetDigitKeyString(iKey);
    return EnterDialUI(strNumber, ROUTINE_BTPREDIAL);
}

//是否允许新建蓝牙回话
bool CBTTalkManager::IsAllowNewBTCall()
{
    return (talklogic_BeAllowNewCall()
            && GetBTSessionNumber() < MAX_MOBILESESSION_SIZE);
}

//获取当前蓝牙通话数量
int CBTTalkManager::GetBTSessionNumber(bool bExceptDial/* = true*/)
{
    LIST_CALL_SESSION& listSession = _SessionManager.GetSessionList();
    int iCount = 0;

    //bttalkroutine 被加入会议后也需要算一路会话
    for (LIST_CALL_SESSION::iterator it = listSession.begin(); it != listSession.end();
            ++it)
    {
        CSingleSession* pSingleSession = (*it)->ToSingleSession();
        if (NULL == pSingleSession)
        {
            continue;
        }

        //DIAL 和 FINISHED 不能算在其中
        SESSION_STATE eState = pSingleSession->GetSessionState();
        if (SESSION_BTCONNECTING == eState
                || SESSION_BTRINGING == eState
                || SESSION_BTTALKING == eState
           )
        {
            iCount ++;
        }

        if (!bExceptDial
                && (SESSION_BTDIALING == eState
                    || SESSION_BTFINISHED == eState))
        {
            iCount++;
        }
    }

    return iCount;
}

//进入呼叫完成状态的处理
void CBTTalkManager::ProcessAfterCallFinished(CSingleSession* pSession, void* pData,
        bool bCallFail /* = true */, bool bEndCall /* = true */)
{
    if (NULL == pSession
            || NULL == pData)
    {
        return ;
    }

    bool bExitSession = true;

    // bPlayBusyTone为false时只有当BusyToneDelay不等于0才播放忙音
    // callfailed时也根据kszBusytoneDelay来判断是否响忙音，不强制响 http://192.168.1.99/Bug.php?BugID=16475

#if 0 //蓝牙通话暂时不判断
    if (configParser_GetConfigInt(kszForceToPlayBusytoneAfterCallFailed) == 0
            && configParser_GetConfigInt(kszBusytoneDelay) == 0)
    {
        bExitSession = true;
    }
#endif // 0

    // 如果开启预通话，进入CallFinish则要关闭通话
    // 如果已开启了通话，则先停止
    CBaseRoutine* pRoutine = pSession->GetRoutine();
    if (NULL != pRoutine)
    {
        pRoutine->StopTalk();
    }

    //呼叫失败才进入Finished Routine
    if (bExitSession
            && !bCallFail)
    {
        pSession->ExitSession(bEndCall);
    }
    else
    {
        pSession->GetCallInfo()->m_errorInfo.strError = (char*)pData;
        pSession->EnterRoutine(ROUTINE_BTFINISHED);
    }
}

//退出联动
void CBTTalkManager::ExitMobileSync()
{
    //退出所有蓝牙话路
    LIST_CALL_SESSION& listSession = _SessionManager.GetSessionList();

    for (LIST_CALL_SESSION::iterator it = listSession.begin();
            it != listSession.end();)
    {
        CBaseSession* pSession = (*it);
        ++it;

        if (pSession->IsBtSession())
        {
            //话机端退出通话不能影响手机
            pSession->ExitSession(false);
        }
    }

    //声音链路会被设置回手机
    m_bChannelLinked = false;
}

//获取dsskey status
void CBTTalkManager::GetDsskeyStatus(CBaseSession* pSession, Dsskey_Status& eStatus)
{
    //蓝牙话路才进行判断
    if (NULL == pSession
            || !pSession->IsBtSession())
    {
        return;
    }

    eStatus = DS_BTLINE_USING;
}

//建立通话
bool CBTTalkManager::EstablishTalk(void* pData)
{
    BTTalkCallUserInfo* pCallUserinfo = (BTTalkCallUserInfo*)pData;
    if (NULL == pCallUserinfo
            || pCallUserinfo->iCallStatus != BT_MB_CALL_STATUS_ESTABLISHED)
    {
        return false;
    }

    //是否开启了DND
    if (configParser_GetConfigInt(kszMobileDNDEnable) == 1
            && dnd_IsEnable(acc_GetDefaultAccount()))
    {
        return false;
    }

    //通话限制与CallWaitting 开关
#if 1
    if (!IsAllowNewBTCall()
            || (!talkhelp_IsCallWaitingEnable()
                && _SessionManager.GetSessionListSize() > 0))
    {
        return false;
    }
#endif // 1

    TALK_INFO("Establish Mobile Call[%d] with Num[%s]", pCallUserinfo->iCallID,
              pCallUserinfo->strNum.c_str());

    CSingleSession* pSession = _SessionManager.AllocSession(pCallUserinfo->iCallID);
    if (pSession == NULL)
    {
        return false;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();
    pCallInfo->m_eCallType = CT_INCOMING;
    pCallInfo->m_tRemoteInfo.sSIPName = pCallUserinfo->strNum;
    pCallInfo->SetCallLineType(CALL_LINE_BT);

    //被叫方信息
    const MobileProperty& mbProperty = Bluetooth_GetMobileProperty();
    pCallInfo->m_tLocalInfo.sSIPName = mbProperty.strNum;
    pCallInfo->SetContactInfo();

    //判断HOLD状态
    if (IsHoldMsg(pCallUserinfo->iCallStatus))
    {
        pCallInfo->SetHold(0 != (BT_MB_LOCAL_HOLD & pCallUserinfo->iCallStatus));
        pCallInfo->SetHeld(0 != (BT_MB_REMOTE_HOLD & pCallUserinfo->iCallStatus));
    }

    pSession->EnterRoutine(ROUTINE_BTTALKING);
    SetVoiceParam(pCallInfo);
    pSession->GetRoutine()->StartTalk();

    return true;
}

//是否为蓝牙通话HOLD消息
bool CBTTalkManager::IsHoldMsg(int iHoldState)
{
    if ((0 != (BT_MB_LOCAL_HOLD & iHoldState)))
    {
        TALK_INFO("This is a Mobile Hold Msg");
        return true;
    }

    return false;
}

//设置通话VPM参数
void CBTTalkManager::SetVoiceParam(CCallInfo* pCallInfo)
{
    if (NULL == pCallInfo)
    {
        return ;
    }

#define BT_IPVP_TALK_MODE 0x01

    pCallInfo->GetIPVPParam().mode = BT_IPVP_TALK_MODE;
}

//从驱动结构体中获取通话信息
void CBTTalkManager::MapBTCallUserInfo(BTTalkCallUserInfo& dstInfo, const void* pSrcInfo)
{
    if (NULL == pSrcInfo)
    {
        return ;
    }

    const btkit_msg_t* pMsg = (const btkit_msg_t*)pSrcInfo;

    dstInfo.iCallID = pMsg->bt_callback_param.mb_call.callid;
    dstInfo.iCallStatus = pMsg->bt_callback_param.mb_call.status;
    dstInfo.iHoldStatus = pMsg->bt_callback_param.mb_call.hold;
    dstInfo.strNum = pMsg->bt_callback_param.mb_call.number;
}

bool CBTTalkManager::IsMobileTalkSessionExist(int iExceptCallID/* = -1*/)
{
    LIST_CALL_SESSION& listSession = _SessionManager.GetSessionList();

    for (LIST_CALL_SESSION::iterator it = listSession.begin(); it != listSession.end();
            ++it)
    {
        CBaseSession* pSession = (*it);

        if (NULL == pSession
                || !pSession->IsBtSession())
        {
            continue;
        }

        CCallInfo* pCallInfo = pSession->GetCallInfoAsSub();
        if (NULL == pCallInfo
                || (iExceptCallID != -1 && pCallInfo->GetCallID() == iExceptCallID))
        {
            continue;
        }

        //已经开启过通话
        if (pSession->IsInTalking()
                || pSession->IsInHold())
        {
            return true;
        }
    }

    return false;
}

bool CBTTalkManager::GetAllBtSessionID(YLVector<int>& vecSessionID)
{
    YLVector<int> vecSessionIDBuf;
    _SessionManager.GetAllSessionID(vecSessionIDBuf);
    for (int iIndex = 0; iIndex < vecSessionIDBuf.size(); iIndex ++)
    {
        CBaseSession* pSession = _SessionManager.GetSession(vecSessionIDBuf[iIndex]);
        if (NULL != pSession && pSession->IsBtSession())
        {
            vecSessionID.push_back(vecSessionIDBuf[iIndex]);
        }
    }
    return vecSessionID.size();
}

//处理声音通话状态改变消息
bool CBTTalkManager::ProcessLinkStatusChange(const btkit_msg_t* pMsg)
{
    if (pMsg == NULL)
    {
        return false;
    }

    DeviceInfo devInfo;
    Bluetooth_GetBluetoothInfoByHandle(pMsg->bt_callback_param.dev_hnd, devInfo);

    // 声音通道建立/断开没有话路的概念, 直接使用vpm接口设置
    if (BT_AG_SCO_PLAY_START == pMsg->bt_event)
    {
#ifndef IF_NEW_CHANNEL_MODE
        YLVector<int> vecSessionID;
        GetAllBtSessionID(vecSessionID);
        // 来电响铃会发link通道建立消息, 该场景下话机不该开启声道
        if (!(vecSessionID.size() == 1
                && talklogic_GetSessionStateBySessionID(vecSessionID[0]) == SESSION_BTRINGING))
        {
            voice_StartBtTalk();
        }
#endif

        // PC蓝牙不带通话状态, 声音直接播放
        if (BT_COMPUTER == devInfo.eType)
        {
            voice_StartBtTalk();
        }

        m_bChannelLinked = true;
    }
    else if (BT_AG_SCO_PLAY_STOP == pMsg->bt_event)
    {
#ifndef IF_NEW_CHANNEL_MODE
        voice_StopBtTalk();
#endif

        // 直接关闭声音
        if (BT_COMPUTER == devInfo.eType)
        {
            voice_StopBtTalk();
        }

        m_bChannelLinked = false;
    }

    return true;
}

#endif // IF_BT_SUPPORT_PHONE
