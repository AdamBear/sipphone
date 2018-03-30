#include "extended/event/src/eventmanager.h"
#include "devicelib/networkserver.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

#include "commonunits/modcommonunits.h" // commonAPI_FormatString

#include "extended/actionuri_url/include/modactionuri_url.h"
#include "extended/csta/include/modcsta.h"

#include "service_system.h"
#if IF_FEATURE_CTRIP_CTI
#include "extended/ctripcti/include/modctripcti.h"
#endif
#include "extended/diagnosis/include/moddiagnosis.h"
#ifdef IF_SUPPORT_WIFI
#include "wireless/wifi/include/modwifi.h"
#endif
#include "usb_headset/include/modusbheadset.h"
#if IF_FEATURE_EMERGENCY_HELD
#include "talk/emergency/include/modemergencyheld.h"
#endif
#if IF_FEATURE_TALKINFO_REPORT
#include "extended/talkreport/include/modtalkreport.h"
#endif

#include "talk/talklogic/include/talklogic_common.h"

#include "exp/src/expdrive/deviceplugin/modexpscan.h"
#include "record/include/modrecord.h"

#include "wireless/bluetoothNew/include/modbluetooth.h"

#if TALK_EXTEND_LOG
#define CASE_VALE_RET(value) case value: return #value

static LPCSTR GetEventName(int iAction)
{
    switch (iAction)
    {
        CASE_VALE_RET(EVENT_TYPE_NONE);
        CASE_VALE_RET(EVENT_SETUP_COMPLETED);
        CASE_VALE_RET(EVENT_LOGIN_ON);
        CASE_VALE_RET(EVENT_LOGIN_OFF);
        CASE_VALE_RET(EVENT_REGISTER_FAILED);
        CASE_VALE_RET(EVENT_OFFHOOK);
        CASE_VALE_RET(EVENT_ONHOOK);
        CASE_VALE_RET(EVENT_INCOMING_CALL);
        CASE_VALE_RET(EVENT_REJECT_INCOMING_CALL);
        CASE_VALE_RET(EVENT_FORWARD_INCOMING_CALL);
        CASE_VALE_RET(EVENT_CALL_REMOTE_CANCELED);
        CASE_VALE_RET(EVENT_ANSWER_NEW_IN_CALL);
        CASE_VALE_RET(EVENT_REJECT_NEW_IN_CALL);
        CASE_VALE_RET(EVENT_CALLOUT);
        CASE_VALE_RET(EVENT_CANCEL_CALLOUT);
        CASE_VALE_RET(EVENT_REMOTE_BUSY);
        CASE_VALE_RET(EVENT_ESTABLISHED);
        CASE_VALE_RET(EVENT_TERMINATED);
        CASE_VALE_RET(EVENT_OPEN_DND);
        CASE_VALE_RET(EVENT_CLOSE_DND);
        CASE_VALE_RET(EVENT_OPEN_ALWAYS_FORWARD);
        CASE_VALE_RET(EVENT_CLOSE_ALWAYS_FORWARD);
        CASE_VALE_RET(EVENT_OPEN_BUSY_FORWARD);
        CASE_VALE_RET(EVENT_CLOSE_BUSY_FORWARD);
        CASE_VALE_RET(EVENT_OPEN_NO_ANSWER_FORWARD);
        CASE_VALE_RET(EVENT_CLOSE_NO_ANSWER_FORWARD);
        CASE_VALE_RET(EVENT_TRANSFER_CALL);
        CASE_VALE_RET(EVENT_BLIND_TRANSFER);
        CASE_VALE_RET(EVENT_ATTENDED_TRANSFER);
        CASE_VALE_RET(EVENT_TRANSFER_FINISHED);
        CASE_VALE_RET(EVENT_TRANSFER_FAILED);
        CASE_VALE_RET(EVENT_HOLD);
        CASE_VALE_RET(EVENT_UNHOLD);
        CASE_VALE_RET(EVENT_MUTE);
        CASE_VALE_RET(EVENT_UNMUTE);
        CASE_VALE_RET(EVENT_MISSED_CALL);
        CASE_VALE_RET(EVENT_BUSY_TO_IDLE);
        CASE_VALE_RET(EVENT_IDLE_TO_BUSY);
        CASE_VALE_RET(EVENT_IP_CHANGED);
        CASE_VALE_RET(EVENT_REMOTE_FORWARD);
        CASE_VALE_RET(EVENT_REMOTE_HOLD);
        CASE_VALE_RET(EVENT_REMOTE_UNHOLD);
        CASE_VALE_RET(EVENT_AUTOP_FINISH);
        CASE_VALE_RET(EVENT_HANDFREE);
        CASE_VALE_RET(EVENT_HEADSET);
        CASE_VALE_RET(EVENT_OPEN_CALLWAIT);
        CASE_VALE_RET(EVENT_CLOSE_CALLWAIT);
        CASE_VALE_RET(EVENT_ENTER_DIAL);
        CASE_VALE_RET(EVENT_END_DIAL);
        CASE_VALE_RET(EVENT_RINGBACK);
        CASE_VALE_RET(EVENT_CONFERENCE);
        CASE_VALE_RET(EVENT_DTMF);
        CASE_VALE_RET(EVENT_CALL_FAIL);
        CASE_VALE_RET(EVENT_BLIND_TRANSFER_FAILED);
        CASE_VALE_RET(EVENT_ATTENDED_TRANSFER_FAILED);
        CASE_VALE_RET(EVENT_CALL_REMOTE_NO_ANSWER);
        CASE_VALE_RET(EVENT_VIEW_MISSCALL);
        CASE_VALE_RET(EVENT_VIEW_FORWARD);
        CASE_VALE_RET(EVENT_CALL_BY_REPLACED);
        CASE_VALE_RET(EVENT_CALL_INFO_UPDATE);
        CASE_VALE_RET(EVENT_CALL_TRANSFERED_BY_REMOTE);
        CASE_VALE_RET(EVENT_PERIPHERAL_INFORMATION);
        CASE_VALE_RET(EVENT_AUTOP_START);
        CASE_VALE_RET(EVENT_AUTOP_STATUS);
        CASE_VALE_RET(EVENT_AUTOP_END);
        CASE_VALE_RET(EVENT_AUTOP_ZERO_TOUCH_START);
        CASE_VALE_RET(EVENT_AUTOP_ZERO_TOUCH_STATUS);
        CASE_VALE_RET(EVENT_AUTOP_ZERO_TOUCH_END);
        CASE_VALE_RET(EVENT_AUTOP_RPS_START);
        CASE_VALE_RET(EVENT_AUTOP_RPS_STATUS);
        CASE_VALE_RET(EVENT_AUTOP_RPS_END);
        CASE_VALE_RET(EVENT_ENTER_PAGE);
        CASE_VALE_RET(EVENT_EXIT_PAGE);
        CASE_VALE_RET(EVENT_CONTACT_SEARCH);
        CASE_VALE_RET(EVENT_CONTACT_MODIFY);
        CASE_VALE_RET(EVENT_HOTPLUG_ADD);
        CASE_VALE_RET(EVENT_HOTPLUG_REMOVE);
        CASE_VALE_RET(EVENT_HOTPLUG_USB_RECORDED);
        CASE_VALE_RET(EVENT_MEDIA_VOICE_STOP);
        CASE_VALE_RET(EVENT_SUPER_SEARCH);
        CASE_VALE_RET(EVENT_TYPE_MAX);
    default:
        return NULL;
    }
}
#endif

extern bool g_bAfterInit;

CEventManager * CEventManager::GetInstance()
{
    static CEventManager instance;
    return &instance;
}

CEventManager::CEventManager()
{
    ListEventModule.clear();
    m_bFirstSendEvent = true;
    m_bPowerOnChangeIP = false;
    // todo  Conpleted
    m_bPhoneSetupConpleted = false;
}

CEventManager::~CEventManager()
{
    timerKillThreadTimer((UINT)&m_bFirstSendEvent);
}

bool CEventManager::OnTimeoutMessage(unsigned int uTimerID)
{
    if (uTimerID == (UINT)&m_bFirstSendEvent && g_bAfterInit)
    {
        if (!netIsNetworkReady())
        {
            return false;
        }
        timerKillThreadTimer((UINT)&m_bFirstSendEvent);

        SendPhoneInfoEvent(EVENT_SETUP_COMPLETED);
        timerSetThreadTimer((UINT)&m_bPhoneSetupConpleted, 10 * 1000);
        return true;
    }
    else if (uTimerID == (UINT)&m_bPowerOnChangeIP)
    {
        timerKillThreadTimer((UINT)&m_bPowerOnChangeIP);
        m_bPowerOnChangeIP = false;
    }
    else if (uTimerID == (UINT)&m_bPhoneSetupConpleted)
    {
        timerKillThreadTimer((UINT)&m_bPhoneSetupConpleted);

        m_ConnectedDeviceData.m_eEventType = EVENT_PERIPHERAL_INFORMATION;

        GetConnectedDeviceInfo(m_ConnectedDeviceData);
        SendEvent(&m_ConnectedDeviceData);
        m_bPhoneSetupConpleted = true;
        return true;
    }

    return false;
}

bool CEventManager::MyInit()
{
    // get phone type from cfg file
    char bufProductName[512] = {0};
    sys_replace_wildcards((char *)configParser_GetConfigString(kszProductName).c_str(), 0,
                          bufProductName, sizeof(bufProductName) / sizeof(char));
    m_phoneInfoData.m_strPhoneType = bufProductName;

    // cfgserver 不能使用configParser获取这个配置，统一使用sys服务获取

    char szFirmware[64] = { 0 };
    if (0 == sys_get_sw_version(szFirmware, 64))
    {
        m_phoneInfoData.m_strFirmware = szFirmware;
    }

    // get mac
    m_phoneInfoData.m_strMAC = netGetMACText('\0');

    if (!netIsNetworkReady())
    {
        ACTION_WARN("CEventManager::MyInit() network is unavailable!");

        timerSetThreadTimer((UINT)&m_bFirstSendEvent, 3 * 1000);
    }
    else
    {
        SendPhoneInfoEvent(EVENT_SETUP_COMPLETED);

        SendPhoneInfoEvent(EVENT_IP_CHANGED);

        timerSetThreadTimer((UINT)&m_bPhoneSetupConpleted, 10 * 1000);

        m_bPowerOnChangeIP = true;

        timerSetThreadTimer((UINT)&m_bPowerOnChangeIP, 30 * 1000);
    }

    return true;
}
void CEventManager::InitEventModule()
{
    actURL_ActionURLInit();
    csta_Init();
#if IF_FEATURE_CTRIP_CTI
    ctrip_Init();
#endif

#ifdef IF_FEATURE_DIAGNOSE
    diag_Init();
#endif //IF_FEATURE_DIAGNOSE
#if IF_FEATURE_EMERGENCY_HELD
    EmergencyHELD_Init();
#endif
#if IF_FEATURE_TALKINFO_REPORT
    TalkInfoReport_Init();
#endif
}

bool CEventManager::RegisterModule(CEventBase * pEvent)
{
    if (pEvent == NULL)
    {
        return false;
    }

    ACTION_INFO("Event Manager register Module[%p] Type[%d]", pEvent, pEvent->GetModuleType());
    FOR_EVENT_MODULE_LIST(itBegin)
    {
        if ((*itBegin) == NULL)
        {
            continue;
        }

        CEventBase * pTempEventModule = (*itBegin);
        //模块已被注册
        if (pTempEventModule == pEvent
                || pTempEventModule->GetModuleType() == pEvent->GetModuleType())
        {
            ACTION_WARN("RegisterModule Fail! The Module has been Register.");
            return false;
        }
    }

    ListEventModule.push_back(pEvent);
    return true;
}

bool CEventManager::UnRegisterModule(CEventBase * pEvent)
{
    if (pEvent == NULL)
    {
        return false;
    }
    ACTION_INFO("CEventManager::UnRegisterModule[%p] Type[%d]", pEvent, pEvent->GetModuleType());
    FOR_EVENT_MODULE_LIST(itBegin)
    {
        if ((*itBegin) == NULL)
        {
            continue;
        }
        CEventBase * pTempEventModule = (*itBegin);
        if (pTempEventModule == pEvent)
        {
            ListEventModule.erase(itBegin);
            return true;
        }
    }

    return false;
}

SEND_RESULT CEventManager::SendEvent(EventDataBase * pData, EventModuleType eType/* = EMT_ALL*/)
{
    // 网络不可用不发送任何事件
    if (!netIsNetworkReady())
    {
        return SR_NETWORK_UNAVAILABLE;
    }

    if (pData == NULL)
    {
        return SR_DATA_EMPTY;
    }

    // 无效事件不发送
    if (pData->m_eEventType <= EVENT_TYPE_NONE
            || pData->m_eEventType >= EVENT_TYPE_MAX)
    {
        return SR_UNKNOW_EVENT;
    }

    // 第一次发送事件且注册模块为空,确认相关事件模块是否注册上
    if (m_bFirstSendEvent)
    {
        m_bFirstSendEvent = false;
        InitEventModule();
    }

    ACTION_INFO("Send event type[%d] Data[%d] Module[%d]", pData->m_eEventType, pData->m_eEventDataType,
                eType);
    bool bRet = false;
    FOR_EVENT_MODULE_LIST(itBegin)
    {
        if ((*itBegin) == NULL)
        {
            continue;
        }

        CEventBase * pTempEventModule = (*itBegin);
        EventModuleType eModuleType = pTempEventModule->GetModuleType();
        if ((eModuleType & eType) != 0)
        {
            bRet |= pTempEventModule->SendEvent(pData);
        }
    }
    return (bRet ? SR_SUCCESS : SR_NO_MODULE_PROCESS);
}

SEND_RESULT CEventManager::SendPhoneInfoEvent(EventType eEventType,
        EventModuleType eType/* = EMT_ALL*/)
{
    yl::string strIp;
    netGetWanIP(strIp);

    if (eEventType == EVENT_IP_CHANGED)
    {
        netGetLocalIPv4(m_phoneInfoData.m_strIpV4);
        netGetLocalIPv6(m_phoneInfoData.m_strIpV6);
        if (m_bPowerOnChangeIP)
        {
            m_bPowerOnChangeIP = false;
            if (strIp == m_phoneInfoData.m_strIp)
            {
                return SR_SUCCESS;
            }
        }
    }

    m_phoneInfoData.m_eEventType = eEventType;
    m_phoneInfoData.m_strIp = strIp;

    return SendEvent(&m_phoneInfoData, eType);
}

bool CEventManager::SendConnectedDeviceEvent(EventType eEventType,
        EventModuleType eType/* = EMT_ALL*/)
{
    //防止重启后上报多次拔插事件,统一上报一次
    if (!m_bPhoneSetupConpleted)
    {
        return false;
    }

    //防止短时间内多次拔插上报
    timerKillThreadTimer((UINT)&m_bPhoneSetupConpleted);
    timerSetThreadTimer((UINT)&m_bPhoneSetupConpleted, 3 * 1000);
    return true;
}

bool CEventManager::GetConnectedDeviceInfo(ConnectedDeviceEventData & refConnectedDeviceData)
{
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    bool bUsbConnected = modRecord_IsStorageConnect();
    refConnectedDeviceData.m_strUSBNum = commonAPI_FormatString("%d", bUsbConnected ? 1 : 0);
#else
    refConnectedDeviceData.m_strUSBNum = "NS";
#endif

#ifdef IF_SUPPORT_WIFI
    bool bWifiDongleEnabel = !WIFI_IsBuiltInWifi() && WIFI_IsDongleVaild();
    refConnectedDeviceData.m_strWifiDongleNum = commonAPI_FormatString("%d", bWifiDongleEnabel ? 1 : 0);
#else
    refConnectedDeviceData.m_strWifiDongleNum = "NS";
#endif

#ifdef IF_SUPPORT_BLUETOOTH
    bool bBlueToothDongleEnabel = !Bluetooth_IsBuiltInBluetooth() && Bluetooth_GetDeviceState() != DS_NO_DONGLE;
    refConnectedDeviceData.m_strBlueToothDongleNum = commonAPI_FormatString("%d",
            bBlueToothDongleEnabel ? 1 : 0);
#else
    refConnectedDeviceData.m_strBlueToothDongleNum = "NS";
#endif

#ifdef IF_SUPPORT_USB_AUDIO
    bool bUSBHeadset = USBHeadset_IsConnect();
    refConnectedDeviceData.m_strUSBHeadSet = commonAPI_FormatString("%d", bUSBHeadset ? 1 : 0);
#else
    refConnectedDeviceData.m_strUSBHeadSet = "NS";
#endif

    if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
    {
        refConnectedDeviceData.m_strEXPNum = commonAPI_FormatString("%d", exp_GetExpCounts(DT_ALL));
    }
    else
    {
        refConnectedDeviceData.m_strEXPNum = DEVICELIB_IS_T27(devicelib_GetPhoneType()) ?
                                             commonAPI_FormatString("%d", exp_GetExpCounts(DT_ALL)) : "NS";
    }

    if (devicelib_GetPhoneType() == PT_T19
            || devicelib_GetPhoneType() == PT_T21
            || devicelib_GetPhoneType() == PT_T23)
    {
        refConnectedDeviceData.m_strEHSNum = "NS";
    }
    else
    {
        refConnectedDeviceData.m_strEHSNum = commonAPI_FormatString("%d", exp_GetEhsIndex() != -1 ? 1 : 0);
    }

    return true;
}

#if IF_FEATURE_EVENT_MODULE
bool CEventManager::OnDndMessage(msgObject & refObj)
{
    bool bEnable = *((bool *)refObj.GetExtraData());
    EventDataBase eventData;
    eventData.m_wParam = refObj.wParam;
    eventData.m_lParam = refObj.lParam;
    eventData.m_eEventType = bEnable ? EVENT_OPEN_DND : EVENT_CLOSE_DND;
    SendEvent(&eventData);
    return true;
}

bool CEventManager::OnFwdMessage(msgObject & refObj)
{
    FwdMessageStatusData * pFwdData = (FwdMessageStatusData *)refObj.GetExtraData();
    if (!pFwdData)
    {
        ACTION_ERR("EventManager: pFwdData is NULL!");
        return false;
    }
    FWDEventData eventData;
    eventData.m_wParam = refObj.wParam;
    eventData.m_lParam = refObj.lParam;
    eventData.m_strFwdNum = pFwdData->szTarget;
    switch (pFwdData->eType)
    {
    case FWD_TYPE_ALWAYS:
        eventData.m_eEventType =
            pFwdData->bEnabled ? EVENT_OPEN_ALWAYS_FORWARD : EVENT_CLOSE_ALWAYS_FORWARD;
        break;
    case FWD_TYPE_BUSY:
        eventData.m_eEventType =
            pFwdData->bEnabled ? EVENT_OPEN_BUSY_FORWARD : EVENT_CLOSE_BUSY_FORWARD;
        break;
    case FWD_TYPE_NO_ANSWER:
        {
            char szRingDelay[8] = { 0 };
            int iRingDuration = configParser_GetConfigInt(kszNoAnswerForwardRingDuration);
            if (iRingDuration <= 0)
            {
                iRingDuration = 6;
            }
            sprintf(szRingDelay, "%d", pFwdData->iRingDelay * iRingDuration);
            eventData.m_strFwdNoAnswerTime = szRingDelay;
            eventData.m_eEventType =
                pFwdData->bEnabled ? EVENT_OPEN_NO_ANSWER_FORWARD : EVENT_CLOSE_NO_ANSWER_FORWARD;
            break;
        }
    default:
        break;
    }
    SendEvent(&eventData);
    return true;
}
#endif // IF_FEATURE_EVENT_MODULE
