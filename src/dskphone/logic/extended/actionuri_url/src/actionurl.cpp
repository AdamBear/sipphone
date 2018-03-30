#include "actionuri_url_inc.h"

#if IF_FEATURE_START2START_ACD
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#endif

extern bool g_bAfterInit;

#if !IF_FEATURE_EVENT_MODULE
#ifdef IF_SUPPORT_UME
LRESULT OnConfigChangeMsg(msgObject & objMsg)
{
    ACTION_INFO("OnConfigChangeMsg objMsg.message[%d],objMsg.wParam[%d]", objMsg.message,
                objMsg.wParam);
    if (objMsg.message == SYS_MSG_NOTIFY_NETWORK_STATUS)
    {
        if (g_pActionUrl->IsIPChange())
        {
            g_pActionUrl->SendPhoneInfo();
        }

    }
    else if (objMsg.message == SIP_REG_UPDATE_REGISTER)
    {
        g_pActionUrl->SendPhoneInfo();
    }

    return true;
}
#endif
#endif

namespace NS_AU
{
#ifdef IF_SUPPORT_UME
static const char * const kszDoCallInComing =
    "incomingCall.do?callNumber=$callNumber&display_remote=$display_remote&csta_id=$call_id";

static const char * const kszDoRejectCall = "rejectIncomingCall.do?csta_id=$call_id";

static const char * const kszDoForwardCall = "forwardIncomingCall.do?csta_id=$call_id";

static const char * const kszDoRemoteCancleCall = "callRemoteCanceled.do?csta_id=$call_id";

static const char * const kszDoAnswerNewCall =
    "answerNewInCall.do?callNumber=$callNumber&display_remote=$display_remote&csta_id=$call_id";

static const char * const kszDoRejectNewCall = "rejectNewInCall.do?csta_id=$call_id";

static const char * const kszDoCallOut =
    "callOut.do?callNumber=$callNumber&display_remote=$display_remote&csta_id=$call_id";

static const char * const kszDoCancelCallOut = "cancelCallOut.do?csta_id=$call_id";

static const char * const kszDoRemoteBusy = "remoteBusy.do?csta_id=$call_id";

static const char * const kszDoCallEstablished =
    "established.do?callNumber=$callNumber&display_remote=$display_remote&csta_id=$call_id";

static const char * const kszDoCallTerminated =
    "terminated.do?callNumber=$callNumber&display_remote=$display_remote&csta_id=$call_id";

static const char * const kszDoCallTransfer =
    "transferCall.do?callNumber=$callNumber&display_remote=$display_remote&csta_id=$call_id";

static const char * const kszDoBlindTransfer =
    "blindTransfer.do?callNumber=$callNumber&display_remote=$display_remote&csta_id=$call_id";

static const char * const kszDoAttendedTransfer =
    "attendedTranfer.do?callNumber=$callNumber&display_remote=$display_remote&csta_id=$call_id";

static const char * const kszDoTransferFinished = "transferFineshed.do?csta_id=$call_id";

static const char * const kszDoTransferFailed = "transferFailed.do?csta_id=$call_id";

static const char * const kszDoHold =
    "hold.do?callNumber=$callNumber&display_remote=$display_remote&csta_id=$call_id";

static const char * const kszDoUnHold =
    "unHold.do?callNumber=$callNumber&display_remote=$display_remote&csta_id=$call_id";

static const char * const kszDoRemoteHold =
    "RemoteHold.do?callNumber=$callNumber&display_remote=$display_remote&csta_id=$call_id";

static const char * const kszDoRemoteUnHold =
    "RemoteUnHold.do?callNumber=$callNumber&display_remote=$display_remote&csta_id=$call_id";

static const char * const kszDoMute =
    "mute.do?callNumber=$callNumber&display_remote=$display_remote&csta_id=$call_id";

static const char * const kszDoUnMute =
    "unMute.do?callNumber=$callNumber&display_remote=$display_remote&csta_id=$call_id";

static const char * const kszDoOffHook = "offHook.do";

static const char * const kszDoOnHook = "onHook.do";

//Remote Forward
static const char * const kszDoRemoteTransfer =
    "remoteTransfer.do?callNumber=$callNumber&display_remote=$display_remote&csta_id=$call_id";

static const char * const kszDoIpOk = "OK.do?cstaSupport=1";


static const char * const kszPhoneInfo =
    "YealinkIPPhoneMate.do?phoneIP=$ip&phoneType=$model&account=$AccountInfo";


#endif

//////////////////////////////////////////////////////////////////////////
// Class CActionUrl.
CActionUrl * CActionUrl::m_pInst = NULL;

// Static.
CActionUrl * CActionUrl::GetInstance()
{
    // 创建单键.
    if (m_pInst == NULL)
    {
        m_pInst = new CActionUrl();
        if (m_pInst != NULL)
        {
            m_pInst->MyInit();
        }
    }
    return m_pInst;
}

// Static.
void CActionUrl::ReleaseInstance()
{
    // 销毁单键.
    if (m_pInst != NULL)
    {
        m_pInst->MyFinal();

        delete m_pInst;
        m_pInst = NULL;
    }
}

void CActionUrl::OnTaskActionCallback(NS_TA::CTaskAction * pTaskAction)
{
    if (pTaskAction == NULL
            || pTaskAction->GetReqType() != NS_TA::TA_REQUEST_NETWORK_FILE)
    {
        return;
    }

    // 回调后直接删除即可.
    CNetworkFileAction * pAction =
        static_cast<CNetworkFileAction *>(pTaskAction);

#if IF_FEATURE_START2START_ACD
    if (pAction && pTaskAction->GetTaskActionErrorType() == NS_TA::TA_NO_ERROR)
    {
        // Start2Start SETUP_COMPLETED 被用于获取ACD初始状态
#if IF_FEATURE_EVENT_MODULE
        if (pAction->GetExtraParam() == EVENT_SETUP_COMPLETED)
#else
        if (pAction->GetExtraParam() == ACTION_URL_SETUP_COMPLETED)
#endif
            // && CallCenter_IsACDOn() && Get_ACDType() == ACD_START2START) // 刚上电ACD状态不对
        {
            xmlbrowser_EnableTips(false);
            xmlbrowser_ParseFile(pAction->GetFilePath());
        }
    }
#endif
}

bool CActionUrl::Send(int nType)
{
    // 只发送状态和系统数据.
    ActionUrlData stAUD;
    return MySend(nType, stAUD);
}

bool CActionUrl::Send(int nType, const ActionUrlData & refData)
{
    // 发送状态, 系统数据以及用户传入的数据.
    return MySend(nType, refData);
}

CActionUrl::CActionUrl()
    : CTaskActionCallbackBase()
#if IF_FEATURE_EVENT_MODULE
    , CEventBase(EMT_ACTION_URL)
#endif
{
#if IF_FEATURE_EVENT_MODULE
    RegisterModule();
#endif
}

CActionUrl::~CActionUrl()
{

}

#if !IF_FEATURE_EVENT_MODULE
LRESULT CActionUrl::OnMessage(msgObject & msg)
{
#ifdef IF_SUPPORT_UME
    OnConfigChangeMsg(msg);
#endif

    switch (msg.message)
    {
    case SYS_MSG_NOTIFY_NETWORK_STATUS:
        {
            if (msg.lParam == SYS_NET_STATUS_READY)
            {
                IDLE_INFO("CActionUrl::OnMessage ACTION_URL_IP_CHANGED");

                // 如果是第一次启动时，已发送IPchang，收到消息网络状态改变的消息后就不用处理了
                if (g_pActionUrl->IsPowerOnChangeIP())
                {
                    g_pActionUrl->ResetPowerOnChangeIP();
                    break;
                }
                // 非启动时收到网络状态改变则需要发送 IP change
                ActionUrlData refIPChangeData;
                g_pActionUrl->MySend(ACTION_URL_IP_CHANGED, refIPChangeData);
            }
        }
        break;
    default:
        break;
    }
    return TRUE;
}
#endif

bool CActionUrl::MyInit()
{
    struct stTemp
    {
        int m_nType;  // 类型.
        yl::string m_strCfg;  // 配置文件中的项.
    };
#if IF_FEATURE_EVENT_MODULE
    static stTemp arrCfgList[] =
    {
        {EVENT_SETUP_COMPLETED, kszAUSetupCompleted},
        {EVENT_LOGIN_ON, kszAULogOn},
        {EVENT_LOGIN_OFF, kszAULogOff},
        {EVENT_REGISTER_FAILED, kszAURegisterFailed},
        {EVENT_OFFHOOK, kszAUOffHook},
        {EVENT_ONHOOK, kszAUOnHook},
        {EVENT_INCOMING_CALL, kszAUIncomingCall},
        {EVENT_REJECT_INCOMING_CALL, kszAURejectIncomingCall},
        {EVENT_FORWARD_INCOMING_CALL, kszAUForwardCall},
        {EVENT_CALL_REMOTE_CANCELED, kszAUCallRemoteCancel},
        {EVENT_ANSWER_NEW_IN_CALL, kszAUAnswerNewCall},
        {EVENT_REJECT_NEW_IN_CALL, kszAURejectNewInCall},
        {EVENT_CALLOUT, kszAUCallOut},
        {EVENT_CANCEL_CALLOUT, kszAUCallInterrupt},
        {EVENT_REMOTE_BUSY, kszAUCallRemoteBusy},
        {EVENT_ESTABLISHED, kszAUEstablished},
        {EVENT_TERMINATED, kszAUTerminated},
        {EVENT_OPEN_DND, kszAUOpenDnd},
        {EVENT_CLOSE_DND, kszAUCloseDnd},
        {EVENT_OPEN_ALWAYS_FORWARD, kszAUOpenAlwaysForward},
        {EVENT_CLOSE_ALWAYS_FORWARD, kszAUCloseAlwaysForward},
        {EVENT_OPEN_BUSY_FORWARD, kszAUOpenBusyForward},
        {EVENT_CLOSE_BUSY_FORWARD, kszAUCloseBusyForward},
        {EVENT_OPEN_NO_ANSWER_FORWARD, kszAUOpenNoAnswerForward},
        {EVENT_CLOSE_NO_ANSWER_FORWARD, kszAUCloseNoAnswerForward},
        {EVENT_TRANSFER_CALL, kszAUTransferCall},
        {EVENT_BLIND_TRANSFER, kszAUBlindTransfer},
        {EVENT_ATTENDED_TRANSFER, kszAUAttendedTransfer},
        {EVENT_TRANSFER_FINISHED, kszAUTransferFinished},
        {EVENT_TRANSFER_FAILED, kszAUTransferFailed},
        {EVENT_HOLD, kszAUHold},
        {EVENT_UNHOLD, kszAUUnHold},
        {EVENT_MUTE, kszAUMute},
        {EVENT_UNMUTE, kszAUUnMute},
        {EVENT_MISSED_CALL, kszAUMissedCall},
        {EVENT_BUSY_TO_IDLE, kszAUBusyToIdle},
        {EVENT_IDLE_TO_BUSY, kszAUIdleToBusy},
        {EVENT_IP_CHANGED, kszAUIpChanged},
        {EVENT_REMOTE_FORWARD, kszAURemoteForward},
        {EVENT_AUTOP_FINISH, kszAUAutopFinish},
        {EVENT_REMOTE_HOLD, kszAURemoteHold},
        {EVENT_REMOTE_UNHOLD, kszAURemoteUnHold},
        {EVENT_HANDFREE, kszAUHandFree},
        {EVENT_HEADSET, kszAUHeadSet},
        {EVENT_OPEN_CALLWAIT, kszAUOpenCallWait},
        {EVENT_CLOSE_CALLWAIT, kszAUCloseCallWait },
        {EVENT_ENTER_DIAL, kszAUEnterDial },
        {EVENT_END_DIAL, kszAUCEndDial },
        {EVENT_RINGBACK, kszAURingBack },
        {EVENT_CONFERENCE, kszAUConference },
        {EVENT_DTMF, kszAUDTMF },
        {EVENT_CALL_FAIL, kszAUCallFial },
        {EVENT_BLIND_TRANSFER_FAILED, kszAUBlindTransferFailed },
        {EVENT_ATTENDED_TRANSFER_FAILED, kszAUAttendedTransferFailed },
        {EVENT_CALL_REMOTE_NO_ANSWER, kszAURemoteNoAnswer },
        {EVENT_VIEW_MISSCALL,  kszAUViewMissCall},
        {EVENT_VIEW_FORWARD,  kszAUViewForWard},
        {EVENT_CALL_BY_REPLACED,  kszAUCallByReplaced },
        {EVENT_CALL_INFO_UPDATE,  kszAUCallInfoUpdate },
        {EVENT_CALL_TRANSFERED_BY_REMOTE,  kszAUCallTransferedByRemote },
        {EVENT_PERIPHERAL_INFORMATION,  kszAUPeripheralInformation },
    };
#else
    static stTemp arrCfgList[] =
    {
        {ACTION_URL_SETUP_COMPLETED, kszAUSetupCompleted},
        {ACTION_URL_LOGIN_ON, kszAULogOn},
        {ACTION_URL_LOGIN_OFF, kszAULogOff},
        {ACTION_URL_REGISTER_FAILED, kszAURegisterFailed},
        {ACTION_URL_OFFHOOK, kszAUOffHook},
        {ACTION_URL_ONHOOK, kszAUOnHook},
        {ACTION_URL_INCOMING_CALL, kszAUIncomingCall},
        {ACTION_URL_REJECT_INCOMING_CALL, kszAURejectIncomingCall},
        {ACTION_URL_FORWARD_INCOMING_CALL, kszAUForwardCall},
        {ACTION_URL_CALL_REMOTE_CANCELED, kszAUCallRemoteCancel},
        {ACTION_URL_ANSWER_NEW_IN_CALL, kszAUAnswerNewCall},
        //to do syl
        //{ACTION_URL_REJECT_NEW_IN_CALL, kszAURejectNewInCall},
        {ACTION_URL_CALLOUT, kszAUCallOut},
        {ACTION_URL_CANCEL_CALLOUT, kszAUCallInterrupt},
        {ACTION_URL_REMOTE_BUSY, kszAUCallRemoteBusy},
        {ACTION_URL_ESTABLISHED, kszAUEstablished},
        {ACTION_URL_TERMINATED, kszAUTerminated},
        {ACTION_URL_OPEN_DND, kszAUOpenDnd},
        {ACTION_URL_CLOSE_DND, kszAUCloseDnd},
        {ACTION_URL_OPEN_ALWAYS_FORWARD, kszAUOpenAlwaysForward},
        {ACTION_URL_CLOSE_ALWAYS_FORWARD, kszAUCloseAlwaysForward},
        {ACTION_URl_OPEN_BUSY_FORWARD, kszAUOpenBusyForward},
        {ACTION_URL_CLOSE_BUSY_FORWARD, kszAUCloseBusyForward},
        {ACTION_URL_OPEN_NO_ANSWER_FORWARD, kszAUOpenNoAnswerForward},
        {ACTION_URL_CLOSE_NO_ANSWER_FORWARD, kszAUCloseNoAnswerForward},
        {ACTION_URL_TRANSFER_CALL, kszAUTransferCall},
        {ACTION_URL_BLIND_TRANSFER, kszAUBlindTransfer},
        {ACTION_URL_ATTENDED_TRANSFER, kszAUAttendedTransfer},
        {ACTION_URL_TRANSFER_FINISHED, kszAUTransferFinished},
        {ACTION_URL_TRANSFER_FAILED, kszAUTransferFailed},
        {ACTION_URL_HOLD, kszAUHold},
        {ACTION_URL_UNHOLD, kszAUUnHold},
        {ACTION_URL_MUTE, kszAUMute},
        {ACTION_URL_UNMUTE, kszAUUnMute},
        {ACTION_URL_MISSED_CALL, kszAUMissedCall},
        {ACTION_URL_BUSY_TO_IDLE, kszAUBusyToIdle},
        {ACTION_URL_IDLE_TO_BUSY, kszAUIdleToBusy},
        {ACTION_URL_IP_CHANGED, kszAUIpChanged},
        {ACTION_URL_REMOTE_FORWARD, kszAURemoteForward},
        {ACTION_URL_AUTOP_FINISH, kszAUAutopFinish},
        {ACTION_URL_REMOTE_HOLD, kszAURemoteHold},
        {ACTION_URL_REMOTE_UNHOLD, kszAURemoteUnHold},
        {ACTION_URL_HANDFREE, kszAUHandFree},
        {ACTION_URL_HEADSET, kszAUHeadSet},
        {ACTION_URL_OPEN_CALLWAIT, kszAUOpenCallWait},
        {ACTION_URL_CLOSE_CALLWAIT, kszAUCloseCallWait },
        {ACTION_URL_VIEW_MISSCALL,  kszAUViewMissCall},
        {ACTION_URL_VIEW_FORWARD,  kszAUViewForWard},
    };
#endif
    // 初始化类型和配置项的map.
    for (int i = 0; i < sizeof(arrCfgList) / sizeof(arrCfgList[0]); ++i)
    {
        m_mapCfg[arrCfgList[i].m_nType] = arrCfgList[i].m_strCfg;
    }  // for.

#if !IF_FEATURE_EVENT_MODULE
    // get phone type from cfg file
    char bufProductName[512] = {0};
    sys_replace_wildcards((char *)configParser_GetConfigString(kszProductName).c_str(), 0,
                          bufProductName, sizeof(bufProductName) / sizeof(char));
    m_strPhoneType = bufProductName;

    // get firmware from cfg file

    char szFirmware[32] = {0};

    if (0 == sys_get_sw_version(szFirmware, 32))
    {
        m_strFirmware = szFirmware;
    }

    // get mac
    m_strMAC = netGetMACText('\0');

    // is not first change ip
    m_bPowerOnChangeIP = false;

    if (!netIsNetworkReady())
    {
        ACTION_WARN("InitActionURL() network is unavailable!");

        timerSetThreadTimer((UINT)&m_mapCfg, 10 * 1000);
    }
    else
    {
        netGetWanIP(m_strIp);

        // 启动完后发送ActionURL
        ActionUrlData refData;
        MySend(ACTION_URL_SETUP_COMPLETED, refData);

#ifdef IF_SUPPORT_UME
        SendPhoneInfo();
#endif

        // 启动时，网络状态可用，发送 IPChange
        ActionUrlData refIPChangeData;
        MySend(ACTION_URL_IP_CHANGED, refIPChangeData);
        ACTION_INFO("InitActionURL() Send IP Change");
        m_bPowerOnChangeIP = true;
    }

#ifdef IF_SUPPORT_UME
    etl_RegisterMsgHandle(SIP_REG_UPDATE_REGISTER, SIP_REG_UPDATE_REGISTER, OnConfigChangeMsg);
    //etl_RegisterMsgHandle(SYS_MSG_NOTIFY_NETWORK_STATUS, SYS_MSG_NOTIFY_NETWORK_STATUS, OnConfigChangeMsg);
#endif
    etl_RegisterMsgHandle(SYS_MSG_NOTIFY_NETWORK_STATUS, SYS_MSG_NOTIFY_NETWORK_STATUS,
                          &CActionUrl::OnMessage);

#endif
    return true;
}

bool CActionUrl::MyFinal()
{
#if !IF_FEATURE_EVENT_MODULE
#ifdef IF_SUPPORT_UME
    etl_UnregisterMsgHandle(SIP_REG_UPDATE_REGISTER, SIP_REG_UPDATE_REGISTER, OnConfigChangeMsg);
    //etl_UnregisterMsgHandle(SYS_MSG_NOTIFY_NETWORK_STATUS, SYS_MSG_NOTIFY_NETWORK_STATUS, OnConfigChangeMsg);
#endif
    etl_UnregisterMsgHandle(SYS_MSG_NOTIFY_NETWORK_STATUS, SYS_MSG_NOTIFY_NETWORK_STATUS,
                            &CActionUrl::OnMessage);
#endif
    // Nothing to do.
    return true;
}

#if IF_FEATURE_EVENT_MODULE
bool CActionUrl::SendEvent(EventDataBase * pData)
{
    ACTION_INFO("ActionUrl SendEvent[%d]", pData->m_eEventType);
    ActionUrlData refData;
    switch (pData->m_eEventDataType)
    {
    case EDT_BASE:
        {
            if (pData->m_wParam != -1)
            {
                GetAccountData(pData->m_wParam, refData);
            }

            if (pData->m_eEventType == EVENT_LOGIN_ON
                    || pData->m_eEventType == EVENT_LOGIN_OFF
                    || pData->m_eEventType == EVENT_REGISTER_FAILED)
            {
                SendPhoneInfo();
            }
        }
        break;
    case EDT_TALK:
        {
            GetTalkEventData(pData, refData);
        }
        break;
    case EDT_REMOTE_INFO:
        {
            GetRemoteInfoData(pData, refData);
        }
        break;
    case EDT_FWD:
        {
            GetForwardEventData(pData, refData);
        }
        break;
    case EDT_PHONE:
        {
            GetPhoneInfoData(pData);
            if (pData->m_eEventType == EVENT_IP_CHANGED)
            {
                SendPhoneInfo();
            }
        }
        break;
    case EDT_Connected_Device:
        {
            GetConnectedDeviceDate(pData, refData);
        }
        break;
    default:
        break;
    }

    return MySend(pData->m_eEventType, refData);
}

void CActionUrl::GetTalkEventData(EventDataBase * pData, ActionUrlData & refData)
{
    TalkEventData * pTalkEventData = static_cast<TalkEventData *>(pData);
    if (pTalkEventData == NULL)
    {
        return;
    }

    //目前ActionURL只有用第一路CallInfo,后续需要扩展在这里获取多路CallInfo并提取CallInfo信息
    int nCallID = -1;
    if (pTalkEventData->m_vecCallID.size() > 0)
    {
        nCallID = pTalkEventData->m_vecCallID[0];
    }

    const CCallInfo * pCallInfo = talklogic_GetCallInfoByCallId(nCallID);
    if (pCallInfo == NULL)
    {
        ACTION_WARN("Can not Get CallInfo by CallID[%d]", nCallID);
        return;
    }

    yl::string strRemoteSipName = pCallInfo->m_tRemoteInfo.sSIPName;
    yl::string strRemoteDisplayName = pCallInfo->m_tRemoteInfo.sDisplayName;
    yl::string strServerName = pCallInfo->m_tRemoteInfo.sServerName;

#if IF_BUG_35381
    if (pCallInfo->m_tRemoteInfo.sSIPName.find("anonymous") != yl::string::npos)
    {
        if (!pCallInfo->m_tRealRemoteInfoByAnonymous.sDisplayName.empty())
        {
            strRemoteDisplayName = pCallInfo->m_tRealRemoteInfoByAnonymous.sDisplayName;
        }

        if (!pCallInfo->m_tRealRemoteInfoByAnonymous.sSIPName.empty())
        {
            strRemoteSipName = pCallInfo->m_tRealRemoteInfoByAnonymous.sSIPName;
        }

        if (!pCallInfo->m_tRealRemoteInfoByAnonymous.sServerName.empty())
        {
            strServerName = pCallInfo->m_tRealRemoteInfoByAnonymous.sServerName;
        }
    }
#endif

    refData.m_strDisplayRemote = strRemoteDisplayName;
    if (refData.m_strDisplayRemote.empty())
    {
        refData.m_strDisplayRemote = strRemoteSipName;
    }
    refData.m_strRemote = "";
    refData.m_strRemote = refData.m_strRemote + "sip:"
                          + strRemoteSipName
                          + "@"
                          + strServerName;

    refData.m_strCallID = commonAPI_FormatString("%d", pCallInfo->GetCallID());
    refData.m_strCalledNum = pCallInfo->m_tRemoteInfo.sSIPName;

    refData.m_strDuration = commonAPI_FormatString("%d", pCallInfo->m_iCallDuration);
    refData.m_strCallDir = ((pCallInfo->m_eCallType == CT_OUTGOING) ? "out" : "in");
    refData.m_strDiversionNum = pCallInfo->m_strDiversionNumber;

    GetAccountData(pTalkEventData->m_wParam, refData);
}

void CActionUrl::GetRemoteInfoData(EventDataBase * pData, ActionUrlData & refData)
{
    RemoteInfoData * pRemoteInfoData = static_cast<RemoteInfoData *>(pData);
    if (pRemoteInfoData == NULL)
    {
        return;
    }

    refData.m_strCallID = pRemoteInfoData->m_strCallID;
    refData.m_strCallDir = pRemoteInfoData->m_strCallDir;
    refData.m_strDisplayRemote = pRemoteInfoData->m_strDisplayName;
    if (refData.m_strDisplayRemote.empty())
    {
        refData.m_strDisplayRemote = pRemoteInfoData->m_strSIPName;
    }
    refData.m_strRemote = "";
    refData.m_strRemote = refData.m_strRemote + "sip:"
                          + pRemoteInfoData->m_strSIPName
                          + "@"
                          + pRemoteInfoData->m_strServerName;
    refData.m_strCalledNum = pRemoteInfoData->m_strSIPName;

    GetAccountData(pRemoteInfoData->m_wParam, refData);
}

void CActionUrl::GetForwardEventData(EventDataBase * pData, ActionUrlData & refData)
{
    FWDEventData * pFWDEventData = static_cast<FWDEventData *>(pData);
    if (pFWDEventData == NULL)
    {
        return;
    }

    refData.m_strFwdLine = commonAPI_FormatString("%d", pFWDEventData->m_wParam);
    refData.m_strFwdNum = pFWDEventData->m_strFwdNum;
    refData.m_strFwdNoAnswerTime = pFWDEventData->m_strFwdNoAnswerTime;
    GetAccountData(pFWDEventData->m_wParam, refData);
}

void CActionUrl::GetAccountData(int nLineID, ActionUrlData & refData)
{
    refData.m_strSIPURLUser = acc_GetUsername(nLineID);
    refData.m_strSIPURLHost = acc_GetServerName(nLineID);
    refData.m_strSIPURL = "sip:" + acc_GetFullName(nLineID);
    refData.m_strLocal = refData.m_strSIPURL;
    refData.m_strDisplayLocal = configParser_GetCfgItemStringValue(kszAccountDisplayName, nLineID);
    if (refData.m_strDisplayLocal.empty())
    {
        refData.m_strDisplayLocal = refData.m_strSIPURLUser;
    }
}

void CActionUrl::GetPhoneInfoData(EventDataBase * pData)
{
    PhoneInfoEventData * pPhoneInfoData = static_cast<PhoneInfoEventData *>(pData);
    if (pPhoneInfoData == NULL)
    {
        return;
    }

    m_strMAC = pPhoneInfoData->m_strMAC;
    m_strPhoneType = pPhoneInfoData->m_strPhoneType;
    m_strFirmware = pPhoneInfoData->m_strFirmware;
    m_strIp = pPhoneInfoData->m_strIp;
}

//获取外围设备数据
void CActionUrl::GetConnectedDeviceDate(EventDataBase * pData, ActionUrlData & refData)
{
    ConnectedDeviceEventData * pDeviceEventData = static_cast<ConnectedDeviceEventData *>(pData);
    if (pDeviceEventData == NULL)
    {
        return;
    }

    m_strExpNum = pDeviceEventData->m_strEXPNum;
    m_strEhsNum = pDeviceEventData->m_strEHSNum;
    m_strUsbNum = pDeviceEventData->m_strUSBNum;
    m_strWifiDongleNum = pDeviceEventData->m_strWifiDongleNum;
    m_strBlueToothDongleNum = pDeviceEventData->m_strBlueToothDongleNum;
    m_strUSBHeadSet = pDeviceEventData->m_strUSBHeadSet;
}
#endif

bool CActionUrl::MySend(int nType, const ActionUrlData & refData)
{
    if (!g_bAfterInit)
    {
        return false;
    }


    // 查找类型是否有配置.
    ConfigureMap::iterator citer = m_mapCfg.find(nType);
    if (citer != m_mapCfg.end())
    {
        // 查找配置文件中的配置.
        yl::string strCfgVal = configParser_GetConfigString(citer->second.c_str());

#ifdef IF_SUPPORT_UME
        yl::string strRemoteIp = configParser_GetConfigString(kszAURemoteIP);

        if (!strRemoteIp.empty())
        {
            //容错，补头
            if (strRemoteIp.find("://") == yl::string::npos)
            {
                strRemoteIp = "http://" + strRemoteIp;
            }

            //容错，补尾
            if (strRemoteIp.substr(strRemoteIp.length() - 1) != "/")
            {
                strRemoteIp.append("/");
            }

            const char * pstrDo = GetOperationString(nType);
            if (pstrDo != NULL)
            {
                strRemoteIp.append(pstrDo);

                strRemoteIp = MyReplaceVar(strRemoteIp, refData);

                // 直接去服务器上下载文件, 不关心结果.
                CNetworkFileAction * pActionRemote = TaskAction_CreateNetworkFileAction(
                        TEMP_PATH "action_url_temp.xml", strRemoteIp.c_str());
                if (pActionRemote != NULL)
                {
                    pActionRemote->SetSync(false);  // 设置异步.
                    pActionRemote->SetRead(true);  // 设置下载.
                    pActionRemote->SetCallback(this);  // 设置无回调.

                    // 开始下载.
                    TaskAction_ExecAction(pActionRemote);
                }
            }
        }
#endif

        // 去前后空格
        strCfgVal = commonAPI_TrimString(strCfgVal, " ", TRIM_ALL);

        if (!strCfgVal.empty())
        {
            strCfgVal = MyReplaceVar(strCfgVal, refData);

            // 容错, 补头.
            if (strCfgVal.find("://") == yl::string::npos)
            {
                strCfgVal = "http://" + strCfgVal;
            }
            // 直接去服务器上下载文件, 不关心结果.
            CNetworkFileAction * pAction = TaskAction_CreateNetworkFileAction(
                                               TEMP_PATH "action_url_temp.xml", strCfgVal.c_str());
            if (pAction != NULL)
            {
                pAction->SetSync(false);  // 设置异步.
                pAction->SetRead(true);  // 设置下载.
                pAction->SetCallback(this);  // 设置无回调.
#if IF_FEATURE_START2START_ACD
                pAction->SetExtraParam(nType);
#endif
                // 开始下载.
                TaskAction_ExecAction(pAction);

                return true;
            }
        }
    }

    return false;
}

yl::string CActionUrl::MyReplaceVar(const yl::string & strInput,
                                    const ActionUrlData & refData)
{
    yl::string strURLTemp = strInput;

    string_replace(strURLTemp, VAR_MAC, m_strMAC);
    string_replace(strURLTemp, VAR_IP, m_strIp);
    string_replace(strURLTemp, VAR_PHONE_TYPE, m_strPhoneType);
    string_replace(strURLTemp, VAR_FIRMWARE, m_strFirmware);
    string_replace(strURLTemp, VAR_ACTIVE_URL, refData.m_strSIPURL);
    string_replace(strURLTemp, VAR_ACTIVE_USER, refData.m_strSIPURLUser);
    string_replace(strURLTemp, VAR_ACTIVE_HOST, refData.m_strSIPURLHost);
    string_replace(strURLTemp, VAR_LOCAL, refData.m_strLocal);
    string_replace(strURLTemp, VAR_REMOTE, refData.m_strRemote);
    string_replace(strURLTemp, VAR_DISPLAY_LOCAL, refData.m_strDisplayLocal);
    string_replace(strURLTemp, VAR_DISPLAY_REMOTE, refData.m_strDisplayRemote);
    string_replace(strURLTemp, VAR_CALLID, refData.m_strCallID);
    string_replace(strURLTemp, VAR_FWD_LINE, refData.m_strFwdLine);
    string_replace(strURLTemp, VAR_FWD_NUM, refData.m_strFwdNum);
    string_replace(strURLTemp, VAR_NOANS_FWDTIME, refData.m_strFwdNoAnswerTime);
    string_replace(strURLTemp, VAR_FWD_NUM_OLD, refData.m_strFwdNum);
    string_replace(strURLTemp, VAR_NOANS_FWDTIME_OLD, refData.m_strFwdNoAnswerTime);
    string_replace(strURLTemp, VAR_CALLER_ID, refData.m_strDisplayRemote);
    string_replace(strURLTemp, VAR_DIAL_NUM, refData.m_strCalledNum);

    string_replace(strURLTemp, kszCallNumber, refData.m_strCalledNum);

    string_replace(strURLTemp, VAR_CSTAID, refData.m_strCSTAID);

    string_replace(strURLTemp, VAR_EXP_MODULE, refData.m_strEXPModule);

    string_replace(strURLTemp, VAR_ACTIVE_KEY, refData.m_strActiveKey);
    string_replace(strURLTemp, VAR_DURATION, refData.m_strDuration);
    string_replace(strURLTemp, VAR_CALL_DIR, refData.m_strCallDir);
    string_replace(strURLTemp, VAR_DIVERSION_NUM, refData.m_strDiversionNum);

    string_replace(strURLTemp, VAR_USB_HEADSET_NUMBER, m_strUSBHeadSet);
    string_replace(strURLTemp, VAR_WIFIDONGEL_NUM, m_strWifiDongleNum);
    string_replace(strURLTemp, VAR_BLUETOOTHDONGLE_NUM, m_strBlueToothDongleNum);
    string_replace(strURLTemp, VAR_EXP_NUM, m_strExpNum);
    string_replace(strURLTemp, VAR_EHS_NUM, m_strEhsNum);
    string_replace(strURLTemp, VAR_USB_NUM, m_strUsbNum);

    string_replace(strURLTemp, VAR_USB_HEADSET, m_strUSBHeadSet);
    string_replace(strURLTemp, VAR_WIFIDONGEL, m_strWifiDongleNum);
    string_replace(strURLTemp, VAR_BLUETOOTHDONGLE, m_strBlueToothDongleNum);
    string_replace(strURLTemp, VAR_EXP, m_strExpNum);
    string_replace(strURLTemp, VAR_EHS, m_strEhsNum);
    string_replace(strURLTemp, VAR_USB, m_strUsbNum);
    return strURLTemp;
}

#ifdef IF_SUPPORT_UME
const char * CActionUrl::GetOperationString(int nType)
{
#if IF_FEATURE_EVENT_MODULE
    switch (nType)
    {
    case EVENT_OFFHOOK:
        return kszDoOffHook;

    case EVENT_ONHOOK:
        return kszDoOnHook;

    case EVENT_INCOMING_CALL:
        return kszDoCallInComing;

    case EVENT_REJECT_INCOMING_CALL:
        return kszDoRejectCall;

    case EVENT_FORWARD_INCOMING_CALL:
        return kszDoForwardCall;

    case EVENT_CALL_REMOTE_CANCELED:
        return kszDoRemoteCancleCall;

    case EVENT_ANSWER_NEW_IN_CALL:
        return kszDoAnswerNewCall;

    case EVENT_REJECT_NEW_IN_CALL:
        return kszDoRejectNewCall;

    case EVENT_CALLOUT:
        return kszDoCallOut;

    case EVENT_CANCEL_CALLOUT:
        return kszDoCancelCallOut;

    case EVENT_REMOTE_BUSY:
        return kszDoRemoteBusy;

    case EVENT_ESTABLISHED:
        return kszDoCallEstablished;

    case EVENT_TERMINATED:
        return kszDoCallTerminated;

    case EVENT_TRANSFER_CALL:
        return kszDoCallTransfer;

    case EVENT_BLIND_TRANSFER:
        return kszDoBlindTransfer;

    case EVENT_ATTENDED_TRANSFER:
        return kszDoAttendedTransfer;

    case EVENT_TRANSFER_FINISHED:
        return kszDoTransferFinished;

    case EVENT_TRANSFER_FAILED:
        return kszDoTransferFailed;

    case EVENT_HOLD:
        return kszDoHold;

    case EVENT_UNHOLD:
        return kszDoUnHold;

    case EVENT_MUTE:
        return kszDoMute;

    case EVENT_UNMUTE:
        return kszDoUnMute;

    case EVENT_REMOTE_FORWARD:
        return kszDoRemoteTransfer;

    case EVENT_REMOTE_HOLD:
        return kszDoRemoteHold;

    case EVENT_REMOTE_UNHOLD:
        return kszDoRemoteUnHold;
    default:
        return NULL;
    }
#else
    switch (nType)
    {
    case ACTION_URL_OFFHOOK:
        return kszDoOffHook;

    case ACTION_URL_ONHOOK:
        return kszDoOnHook;

    case ACTION_URL_INCOMING_CALL:
        return kszDoCallInComing;

    case ACTION_URL_REJECT_INCOMING_CALL:
        return kszDoRejectCall;

    case ACTION_URL_FORWARD_INCOMING_CALL:
        return kszDoForwardCall;

    case ACTION_URL_CALL_REMOTE_CANCELED:
        return kszDoRemoteCancleCall;

    case ACTION_URL_ANSWER_NEW_IN_CALL:
        return kszDoAnswerNewCall;

    case ACTION_URL_REJECT_NEW_IN_CALL:
        return kszDoRejectNewCall;

    case ACTION_URL_CALLOUT:
        return kszDoCallOut;

    case ACTION_URL_CANCEL_CALLOUT:
        return kszDoCancelCallOut;

    case ACTION_URL_REMOTE_BUSY:
        return kszDoRemoteBusy;

    case ACTION_URL_ESTABLISHED:
        return kszDoCallEstablished;

    case ACTION_URL_TERMINATED:
        return kszDoCallTerminated;

    case ACTION_URL_TRANSFER_CALL:
        return kszDoCallTransfer;
    case ACTION_URL_BLIND_TRANSFER:
        return kszDoBlindTransfer;


    case ACTION_URL_ATTENDED_TRANSFER:
        return kszDoAttendedTransfer;

    case ACTION_URL_TRANSFER_FINISHED:
        return kszDoTransferFinished;

    case ACTION_URL_TRANSFER_FAILED:
        return kszDoTransferFailed;

    case ACTION_URL_HOLD:
        return kszDoHold;

    case ACTION_URL_UNHOLD:
        return kszDoUnHold;

    case ACTION_URL_MUTE:
        return kszDoMute;

    case ACTION_URL_UNMUTE:
        return kszDoUnMute;

    case ACTION_URL_REMOTE_FORWARD:
        return kszDoRemoteTransfer;

    case ACTION_URL_REMOTE_HOLD:
        return kszDoRemoteHold;

    case ACTION_URL_REMOTE_UNHOLD:
        return kszDoRemoteUnHold;
#if IF_BUG_23094
    case ACTION_URL_HANDFREE:
        return kszAUHandFree;
    case ACTION_URL_HEADSET:
        return kszAUHeadSet;
#endif
    default:
        return NULL;
    }
#endif
}

void CActionUrl::SendIpOk()
{
    yl::string strRemoteIp = configParser_GetConfigString(kszAURemoteIP);
    if (!strRemoteIp.empty())
    {
        strRemoteIp.append(kszDoIpOk);

        // 直接去服务器上下载文件, 不关心结果.
        CNetworkFileAction * pActionRemote = TaskAction_CreateNetworkFileAction(
                TEMP_PATH "action_url_temp.xml", strRemoteIp.c_str());
        if (pActionRemote != NULL)
        {
            pActionRemote->SetSync(false);  // 设置异步.
            pActionRemote->SetRead(true);  // 设置下载.
            pActionRemote->SetCallback(this);  // 设置无回调.
            // 开始下载.
            TaskAction_ExecAction(pActionRemote);
        }
    }
}

void CActionUrl::SendPhoneInfo()
{
    //for test
    //configParser_SetConfigString(kszAUUCServer,"http://10.3.3.19:80");
    yl::string strUCServer = configParser_GetConfigString(kszAUUCServer);

    if (strUCServer.empty())
    {
        return;
    }

    if (strUCServer.substr(strUCServer.size() - 1) != "/")
    {
        strUCServer.append("/");
    }
    strUCServer.append(kszPhoneInfo);

    yl::string strAcountInfo;
    GetAccountList(strAcountInfo);

    string_replace(strUCServer, VAR_ACCOUNT_URLS, strAcountInfo);

    string_replace(strUCServer, VAR_IP, m_strIp);

    string_replace(strUCServer, VAR_PHONE_TYPE, m_strPhoneType);

    ACTION_INFO("Send info to UME server!  [%s] ", strUCServer.c_str());

    // 直接去服务器上下载文件, 不关心结果.
    CNetworkFileAction * pActionRemote = TaskAction_CreateNetworkFileAction(
            TEMP_PATH "action_url_temp.xml", strUCServer.c_str());
    if (pActionRemote != NULL)
    {
        pActionRemote->SetSync(false);  // 设置异步.
        pActionRemote->SetRead(true);  // 设置下载.
        pActionRemote->SetCallback(this);  // 设置无回调.
        // 开始下载.
        TaskAction_ExecAction(pActionRemote);
    }
}


void CActionUrl::GetAccountList(yl::string & strList)
{
    ListAccounts listAccounts = acc_GetAvailableAccountList();

    for (ListAccounts::iterator it = listAccounts.begin();
            it != listAccounts.end();
            it++)
    {
        int iAccount = *it;

        //只上报enable账号
        if (!acc_IsAccountEnable(iAccount) || acc_IsDirectIPAccount(iAccount))
        {
            continue;
        }
        if (!acc_GetServerName(iAccount).empty() && !acc_GetUsername(iAccount).empty())
        {
            yl::string strAccountInfo = "sip:" + acc_GetFullName(iAccount);
            strList += strAccountInfo;
            strList += ",";
        }
    }
    trim_right(strList, ",");
}
#if !IF_FEATURE_EVENT_MODULE
bool CActionUrl::IsIPChange()
{
    yl::string strAddr;
    netGetWanIP(strAddr);

    if (m_strIp.compare(strAddr) == 0)
    {
        return false;
    }

    m_strIp = strAddr;
    return true;
}
#endif
#endif

#if !IF_FEATURE_EVENT_MODULE
bool CActionUrl::OnTimeoutMessage(unsigned int uTimerID)
{
    if (uTimerID == (UINT)&m_mapCfg && g_bAfterInit)
    {
        timerKillThreadTimer((UINT)&m_mapCfg);
        // 上电初始化时发送URL
        Send(ACTION_URL_SETUP_COMPLETED);

#ifdef IF_SUPPORT_UME
        SendPhoneInfo();
#endif

        netGetWanIP(m_strIp);
        return true;
    }
    return false;
}

bool CActionUrl::IsPowerOnChangeIP()
{
    return m_bPowerOnChangeIP;
}

void CActionUrl::ResetPowerOnChangeIP()
{
    m_bPowerOnChangeIP = false;
}
#endif

}  // namespace NS_AU
