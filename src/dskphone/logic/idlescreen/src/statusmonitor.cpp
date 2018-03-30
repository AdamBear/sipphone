#include "statusmonitor.h"
#include "statusmanager.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "talk/fwd_dnd/include/fwddndmsgdefine.h"
#ifdef IF_FEATURE_IDLESCREEN
#ifndef IF_NOTIFICATION_CENTER_ON_IDLE

#define RTSERVER_THREAD_NAME "sys"

IMPLEMENT_GETINSTANCE(CStatusMonitor)
CStatusMonitor::CStatusMonitor()
    : m_lastNetworkStatus(NS_OK)
{
    etl_RegisterMsgHandle(SYS_MSG_NOTIFY_PHY_STATUS, SYS_MSG_NOTIFY_TIME_CHANGED,
                          &CStatusMonitor::OnStatusMessage);
    etl_RegisterMsgHandle(KEYGUARD_LOCK_CHANGED, KEYGUARD_LOCK_CHANGED,
                          &CStatusMonitor::OnStatusMessage);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &CStatusMonitor::OnStatusMessage);
    etl_RegisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED,
                          &CStatusMonitor::OnStatusMessage);
    etl_RegisterMsgHandle(ACCOUNT_VOICE_MAIL_COUNT_CHANGED, ACCOUNT_VOICE_MAIL_COUNT_CHANGED,
                          &CStatusMonitor::OnStatusMessage);
    etl_RegisterMsgHandle(LOCAL_CALLLOG_MESSAGE_MISSED, LOCAL_CALLLOG_MESSAGE_MISSED,
                          &CStatusMonitor::OnMissCallMsg);
    etl_RegisterMsgHandle(LOCAL_CALLLOG_MESSAGE_FORWARD, LOCAL_CALLLOG_MESSAGE_FORWARD,
                          &CStatusMonitor::OnForwardMsg);
    etl_RegisterMsgHandle(SYS_MSG_OPENVPN_STATUS, SYS_MSG_OPENVPN_STATUS,
                          &CStatusMonitor::OnStatusMessage);
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED,
                          &CStatusMonitor::OnAccountStatusChange);
    etl_RegisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, &CStatusMonitor::OnWifiStatusMsg);
#ifdef IF_SUPPORT_BLUETOOTH
    etl_RegisterMsgHandle(BLUETOOTH_MSG_CONNECT_STATE, BLUETOOTH_MSG_CONNECT_STATE,
                          &CStatusMonitor::OnBtStatusMsg);
    etl_RegisterMsgHandle(BLUETOOTH_MSG_STATE_CHANGE, BLUETOOTH_MSG_STATE_CHANGE,
                          &CStatusMonitor::OnBtStatusMsg);
#endif //#ifdef IF_SUPPORT_BLUETOOTH
    SingleMsgReg(DND_MESSAGE_STATUS_CHANGED, CStatusMonitor::OnDndStatusMsg);
    SingleMsgReg(FWD_MESSAGE_STATUS_CHANGED, CStatusMonitor::OnFwdStatusMsg);
#ifdef IF_FEATURE_PSTN
    etl_RegisterMsgHandle(ACCOUNT_PSTN_CREATED, ACCOUNT_PSTN_CREATED, &CStatusMonitor::OnStatusMessage);
    etl_RegisterMsgHandle(ACCOUNT_PSTN_REMOVED, ACCOUNT_PSTN_REMOVED, &CStatusMonitor::OnStatusMessage);
#endif
    InitVoiceStatus();

    // 初始化 DHCP option
    InitDHCPOption();

    // 注册notify, 监听网络即将改变消息
    msgPostMsgToThread(msgGetThreadByName(RTSERVER_THREAD_NAME),
                       MKIT_MSG_REQ_NOTIFY, SYS_MSG_NET_WOULD_CHANGED, 0);
    SingleMsgReg(SYS_MSG_NET_WOULD_CHANGED, &CStatusMonitor::OnStatusMessage);
}

CStatusMonitor::~CStatusMonitor()
{
    etl_UnregisterMsgHandle(SYS_MSG_NOTIFY_PHY_STATUS, SYS_MSG_NOTIFY_TIME_CHANGED,
                            &CStatusMonitor::OnStatusMessage);
    etl_UnregisterMsgHandle(KEYGUARD_LOCK_CHANGED, KEYGUARD_LOCK_CHANGED,
                            &CStatusMonitor::OnStatusMessage);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            &CStatusMonitor::OnStatusMessage);
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED,
                            &CStatusMonitor::OnAccountStatusChange);
    etl_UnregisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED,
                            &CStatusMonitor::OnStatusMessage);
    etl_UnregisterMsgHandle(ACCOUNT_VOICE_MAIL_COUNT_CHANGED, ACCOUNT_VOICE_MAIL_COUNT_CHANGED,
                            &CStatusMonitor::OnStatusMessage);
    etl_UnregisterMsgHandle(LOCAL_CALLLOG_MESSAGE_MISSED, LOCAL_CALLLOG_MESSAGE_MISSED,
                            &CStatusMonitor::OnMissCallMsg);
    etl_UnregisterMsgHandle(LOCAL_CALLLOG_MESSAGE_FORWARD, LOCAL_CALLLOG_MESSAGE_FORWARD,
                            &CStatusMonitor::OnForwardMsg);
    etl_UnregisterMsgHandle(SYS_MSG_OPENVPN_STATUS, SYS_MSG_OPENVPN_STATUS,
                            &CStatusMonitor::OnStatusMessage);
    etl_UnregisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, &CStatusMonitor::OnWifiStatusMsg);
#ifdef IF_SUPPORT_BLUETOOTH
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_CONNECT_STATE, BLUETOOTH_MSG_CONNECT_STATE,
                            &CStatusMonitor::OnBtStatusMsg);
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_STATE_CHANGE, BLUETOOTH_MSG_STATE_CHANGE,
                            &CStatusMonitor::OnBtStatusMsg);
#endif //#ifdef IF_SUPPORT_BLUETOOTH
    SingleMsgUnReg(DND_MESSAGE_STATUS_CHANGED, CStatusMonitor::OnDndStatusMsg);
    SingleMsgUnReg(FWD_MESSAGE_STATUS_CHANGED, CStatusMonitor::OnFwdStatusMsg);
#ifdef IF_FEATURE_PSTN
    etl_UnregisterMsgHandle(ACCOUNT_PSTN_CREATED, ACCOUNT_PSTN_CREATED,
                            &CStatusMonitor::OnStatusMessage);
    etl_UnregisterMsgHandle(ACCOUNT_PSTN_REMOVED, ACCOUNT_PSTN_REMOVED,
                            &CStatusMonitor::OnStatusMessage);
#endif
}

// IP被别人冲突的提示语句.
static const char kszHintIpConflicted[] = "IP Conflicted";
// IP冲突别人的提示语句.
static const char kszHintIpConflicting[] = "IP conflict";
// 网络不可用.
static const char kszHintNetworkUnavailable[] = "Network Unavailable";
// 网线不可用.
static const char kszHintCableDisconnect[] = "Cable Disconnect";

static const char kszHintObtainingIP[] = "Obtaining IP Address";

LRESULT CStatusMonitor::OnWifiStatusMsg(msgObject & msg)
{
#ifdef IF_SUPPORT_WIFI
    IDLE_INFO("\n WifiMessage %d  %d  \n ", msg.wParam, msg.lParam);
    if (WIFI_MSG_UPDATE_UI == msg.message)
    {
        if (msg.wParam == OP_WIFI_OPEN_RESULT
                && (msg.lParam == WIRELESS_OP_OPEN_RESULT_SUCCESS
                    || msg.lParam == WIRELESS_OP_OPEN_RESULT_ING))
        {
            // 开启成功
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTTING);
            if (WIFI_IsEnable())
            {
                idleScreen_DropStatus(PS_STATE_WIFI_ADAPTER_ADD);
                idleScreen_SetStatus(PS_STATE_WIFI);
            }
        }
        if (msg.wParam == OP_WIFI_CLOSE_RESULT)
        {
            //关闭成功
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTTING);
            idleScreen_DropStatus(PS_STATE_WIFI);
        }
        else if (msg.wParam == OP_CONNECT_WIFI_RESULT
                 && msg.lParam == WIRELESS_OP_CONNECTING)
        {
            //连接中
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT);
            idleScreen_DropStatus(PS_STATE_WIFI);
            idleScreen_SetStatus(PS_STATE_WIFI_CONNECTTING);
        }
        else if (msg.wParam == OP_CONNECT_WIFI_RESULT
                 && msg.lParam == WIRELESS_OP_CONNECT_SUCCESS)
        {
            //连接成功
            idleScreen_DropStatus(PS_STATE_WIFI);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTTING);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
            idleScreen_DropStatus(PS_STATE_WIFI_ADAPTER_ADD);

            IDLE_INFO("Current active network port[%d] ", sys_get_active_port());
            /**
             * sys_get_active_port说明：配合网络自适应功能
             *   1. 原有体验：打开/关闭wifi则意味着切换网络, 因此打开/关闭wifi后设置到wifi/有线
             *   2. 配置wifi/有线网络优先：实际数据通道切换之后, 设置到wifi/有线
             */
            if (SYS_PHY_TYPE_WIFI == sys_get_active_port())
            {
                idleScreen_SetStatus(PS_STATE_WIFI_CONNECT);
            }
        }
        else if (msg.wParam == OP_CONNECT_WIFI_RESULT
                 && msg.lParam == WIRELESS_OP_CONNECT_FAILED)
        {
            //连接失败
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTTING);
            if (WIFI_IsEnable())
            {
                idleScreen_SetStatus(PS_STATE_WIFI);
            }
        }
        else if (msg.wParam == OP_DISCONNECT_WIFI_RESULT
                 && msg.lParam == WIRELESS_OP_DISCONNECT_SUCCESS)
        {
            //断开成功
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTTING);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
            if (WIFI_IsEnable())
            {
                idleScreen_SetStatus(PS_STATE_WIFI);
            }
        }
        else if (OP_AUTOCONNECT_TIMEROUT == msg.wParam)
        {
            //wifi连接超时
            IDLE_INFO("wifi over time ");
            idleScreen_SetStatus(PS_STATE_WIFI_CONNECTOVERTIME);
            if (WIFI_IsEnable())
            {
                idleScreen_SetStatus(PS_STATE_WIFI);
            }
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTTING);
        }
        else if (WIRELESS_OP_CONNECT_CANCLE == msg.lParam)
        {
            IDLE_INFO("cancel wifi connect ");
            if (WIFI_IsEnable())
            {
                idleScreen_SetStatus(PS_STATE_WIFI);
            }
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTTING);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
        }
        else if (OP_WIFI_DONGLE_STATUS_MODIFY == msg.wParam
                 && WIRELESS_OP_DONGLE_ADD == msg.lParam)
        {
            idleScreen_DropStatus(PS_STATE_WIFI_ADAPTER_REMOVE);

            //dongle插入不进行弹框，直接开启
            if (WIFI_GetSavedNetworkNum() != 0 && configParser_GetConfigInt(kszWifiShowScanPrompt) != 1)
            {
                WIFI_SetEnable(true, true);
            }
            else
            {
                idleScreen_SetStatus(PS_STATE_WIFI_ADAPTER_ADD, "");
            }
        }
        else if (OP_WIFI_DONGLE_STATUS_MODIFY == msg.wParam
                 && WIRELESS_OP_DONGLE_REMOVE == msg.lParam)
        {
            idleScreen_DropStatus(PS_STATE_WIFI_ADAPTER_ADD);
            idleScreen_SetStatus(PS_STATE_WIFI_ADAPTER_REMOVE, "");
        }
    }
#endif
    return 0;
}

#ifdef IF_SUPPORT_BLUETOOTH
LRESULT CStatusMonitor::OnBtStatusMsg(msgObject & msg)
{
    if (BLUETOOTH_MSG_CONNECT_STATE == msg.message)
    {
        if (msg.wParam == 1) //connected
        {
            idleScreen_DropStatus(PS_STATE_BLUETOOTH);
            idleScreen_DropStatus(PS_STATE_BLUETOOTH_CONNECTING);
            idleScreen_SetStatus(_ISBTPHONE(msg.lParam) ? PS_STATE_BLUETOOTH_MOBILECONNECT :
                                 PS_STATE_BLUETOOTH_CONNECT);
        }
        else if (msg.wParam == 2) //connectting
        {
            //idleScreen_DropStatus(PS_STATE_BLUETOOTH);
            //idleScreen_DropStatus(PS_STATE_BLUETOOTH_CONNECT);
            //idleScreen_DropStatus(PS_STATE_BLUETOOTH_MOBILECONNECT);

            //idleScreen_SetStatus(PS_STATE_BLUETOOTH_CONNECTING);
        }
        else if (msg.wParam == 0) //disconnect
        {
            idleScreen_DropStatus(PS_STATE_BLUETOOTH_CONNECT);
            idleScreen_DropStatus(PS_STATE_BLUETOOTH_MOBILECONNECT);
            if (Bluetooth_IsEnable())
            {
                idleScreen_SetStatus(PS_STATE_BLUETOOTH);
            }
        }
    }
    else if (BLUETOOTH_MSG_STATE_CHANGE == msg.message)
    {
        if (msg.wParam == 1) //ENABLE
        {
            idleScreen_SetStatus(PS_STATE_BLUETOOTH);
        }
        else if (msg.wParam == 0)//DISABLE
        {
            idleScreen_DropStatus(PS_STATE_BLUETOOTH);
        }
    }
    return true;
}
#endif //#ifdef IF_SUPPORT_BLUETOOTH

LRESULT CStatusMonitor::OnDndStatusMsg(msgObject & msg)
{
    // 全局模式或者默认账号DND状态改变
    if (msg.wParam == FWDDND_ACCOUNT_GLOBAL || msg.wParam == acc_GetDefaultAccount())
    {
        bool bEnable = *((bool *)msg.GetExtraData());
        if (bEnable)
        {
            idleScreen_SetStatus(PS_STATE_DND);
        }
        else
        {
            idleScreen_DropStatus(PS_STATE_DND);
        }
    }
    return TRUE;
}

LRESULT CStatusMonitor::OnFwdStatusMsg(msgObject & msg)
{
    // 全局模式或者默认账号FWD状态改变
    int iAccountId = msg.wParam;
    if (iAccountId == FWDDND_ACCOUNT_GLOBAL || iAccountId == acc_GetDefaultAccount())
    {
        FwdMessageStatusData * pFwdData = (FwdMessageStatusData *)msg.GetExtraData();
        bool bEnable = ((pFwdData && pFwdData->bEnabled)
                        || fwd_GetInfo(iAccountId, FWD_TYPE_ALL));
        if (bEnable)
        {
            idleScreen_SetStatus(PS_STATE_FWD);
        }
        else
        {
            idleScreen_DropStatus(PS_STATE_FWD);
        }

    }
    return TRUE;
}

LRESULT CStatusMonitor::OnStatusMessage(msgObject & msg)
{
    bool bStatusChanged = false;
    switch (msg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (msg.wParam == ST_FORCEVOICE_SWITCH)
            {
                if (voice_GetVolume(VT_RING) == 0)
                {
                    _StatusManager.AddStatus(PS_STATE_RING_MUTE);
                }
                else
                {
                    _StatusManager.RemoveStatus(PS_STATE_RING_MUTE);
                }
                bStatusChanged = true;
            }
        }
        break;
    case SYS_MSG_NOTIFY_NETWORK_STATUS:
    case SYS_MSG_NOTIFY_PHY_STATUS:
        {
            if (_StatusMonitor.FlushNetStatus())
            {
                backlight_RegisterEvent(BACK_LIGHT_NETWORK, false);
            }
            else
            {
                backlight_RegisterEvent(BACK_LIGHT_NETWORK, true);
            }

            if (SYS_MSG_NOTIFY_NETWORK_STATUS == msg.message && msg.lParam == SYS_NET_STATUS_READY)
            {
                // IP改变时设置第一个账号的sip server为DHCP Option 120
                _StatusMonitor.SetAccountServerByDHCP();
#if IF_FEATURE_EVENT_MODULE
                event_SendPhoneInfoEvent(EVENT_IP_CHANGED);
#endif
            }
        }
        break;
    case KEYGUARD_LOCK_CHANGED:
        {
            if (msg.wParam == 1)
            {
                _StatusManager.AddStatus(PS_STATE_LOCK, "");
            }
            else
            {
                _StatusManager.RemoveStatus(PS_STATE_LOCK);
            }
            bStatusChanged = true;
        }
        break;
    case SYS_MSG_OPENVPN_STATUS:
        {
            if (msg.wParam == 1)
            {
                _StatusManager.AddStatus(PS_STATE_VPN, "");
            }
            else
            {
                _StatusManager.RemoveStatus(PS_STATE_VPN);
            }
            bStatusChanged = true;
        }
        break;
    case ACCOUNT_VOICE_MAIL_COUNT_CHANGED:
        {
            if (acc_GetUnReadVoiceMailNum() != 0)
            {
                yl::string strHint = "";
                if (configParser_GetConfigInt(kszHideUnreadVMNumber) != 1)
                {
                    strHint = commonAPI_FormatString("%d", acc_GetUnReadVoiceMailNum());
                }
                //更新数量
                //新消息，直接增加状态
                if (msg.wParam == 1)
                {
                    _StatusManager.AddStatus(PS_STATE_VOICEMAIL, strHint);
#if !IF_BUG_27040
                    ledlamp_PostEvent(LAMP_EVENTTYPE_SMS, true);
#endif
                    if (configParser_GetConfigInt(kszEnableVoiceMailTone) != 0)
                    {
                        voice_PlayTone(TONE_VOICEMAIL, 300, CHANNEL_AUTO, 1, true);
                    }

                    // 点亮背光灯
                    backlight_RegisterEvent(BACK_LIGHT_DEFAULT, true);
                }
                //网络状态改变 状态需要重新添加
                else if (msg.wParam == 2)
                {
                    _StatusManager.AddStatus(PS_STATE_VOICEMAIL, strHint);
                    // 点亮背光灯
                    backlight_RegisterEvent(BACK_LIGHT_DEFAULT, true);
                }
                else //数量减少，只更新数量
                {
                    _StatusManager.ChangeHint(PS_STATE_VOICEMAIL, strHint);
                }
            }
            else
            {
                //没有未读消息，删除状态
                _StatusManager.RemoveStatus(PS_STATE_VOICEMAIL);
#if !IF_BUG_27040
                ledlamp_PostEvent(LAMP_EVENTTYPE_SMS, false);
#endif
            }

            etl_NotifyApp(false, POWER_LED_VOICE_MAIL, 0, 0);

            bStatusChanged = true;
            break;
        }
    case ACCOUNT_DEFAULT_CHANGED:
        {
            // 默认账号变化，需要重新设置FWD&DND状态
            if (fwd_GetMode() == FWDDND_MODE_ACCOUNT)
            {
                if (fwd_GetInfo(acc_GetDefaultAccount(), FWD_TYPE_ALL))
                {
                    idleScreen_SetStatus(PS_STATE_FWD);
                }
                else
                {
                    idleScreen_DropStatus(PS_STATE_FWD);
                }
                bStatusChanged = true;
            }
            if (dnd_GetMode() == FWDDND_MODE_ACCOUNT)
            {
                if (dnd_IsEnable(acc_GetDefaultAccount()))
                {
                    idleScreen_SetStatus(PS_STATE_DND);
                }
                else
                {
                    idleScreen_DropStatus(PS_STATE_DND);
                }
                bStatusChanged = true;
            }
        }
        break;
#ifdef IF_FEATURE_PSTN
    case ACCOUNT_PSTN_CREATED:
    case ACCOUNT_PSTN_REMOVED:
        {
            bStatusChanged = _StatusMonitor.ProcessPstnAccountMsg(msg);
        }
        break;
#endif
#ifdef IF_SUPPORT_WIFI
    case SYS_MSG_NET_WOULD_CHANGED:
        {
            /**
             * 网络自适应需要处理该事件
             *   1. 原有体验：打开/关闭wifi则意味着切换网络, 因此打开/关闭wifi前接收到此消息
             *   2. 配置wifi/优先网络优先：实际数据通道切换前接收到此消息
             */
            if (WIFI_IsConnected())
            {
                // 即将切换为wifi网络
                if (0 == msg.wParam)
                {
                    idleScreen_SetStatus(PS_STATE_WIFI_CONNECT);
                }
                // 即将切换为有线网络
                else if (1 == msg.wParam)
                {
                    idleScreen_DropStatus(PS_STATE_WIFI_CONNECT);
                }
            }
        }
        break;
#endif // IF_SUPPORT_WIFI
    default:
        break;
    }

    if (bStatusChanged)
    {
        _StatusManager.NotifyStatusChange();
        return TRUE;
    }

    return FALSE;
}

bool CStatusMonitor::FlushNetStatus()
{
    NETWORK_STATUS networkStatus = idleScreen_GetNetworkStatus(true);

#ifdef IF_FEATURE_PSTN
    bool bHasPstnAccount = false;
    if (acc_AccountNum(ACCOUNT_TYPE_PSTN) > 0)
    {
        bHasPstnAccount = true;
    }
#endif

    if (networkStatus != NS_OK
#ifdef IF_FEATURE_PSTN
            && !bHasPstnAccount
#endif
       )
    {
        //防抖，网络改变才刷新状态
        if (m_lastNetworkStatus != networkStatus)
        {
            m_lastNetworkStatus = networkStatus;

            if ((networkStatus == NS_IPV4_CONFLICT
                    || networkStatus == NS_IPV6_CONFLICT)
                    && idleScreen_IsNetworkAvailable())
            {
                _StatusManager.AddStatus(PS_STATE_NETWORK_OK_BUT_CONFLICT, "");
            }
            else
            {
                _StatusManager.AddStatus(PS_STATE_NETWORK_FAILED, "");
            }

            _StatusManager.NotifyStatusChange();
        }
        return false;
    }
    else
    {
        m_lastNetworkStatus = networkStatus;
#ifdef IF_FEATURE_PSTN
        if (bHasPstnAccount)
        {
            m_lastNetworkStatus = NS_OK;
        }
#endif
        _StatusManager.RemoveStatus(PS_STATE_NETWORK_FAILED);
        _StatusManager.RemoveStatus(PS_STATE_NETWORK_OK_BUT_CONFLICT);
        _StatusManager.NotifyStatusChange();
        return true;
    }

    return true;
}

LRESULT CStatusMonitor::OnMissCallMsg(msgObject & msg)
{
    //msg[LOCAL_CALLLOG_MESSAGE_MISSED] l[0] w[MissCallCount]
    if (msg.message != LOCAL_CALLLOG_MESSAGE_MISSED)
    {
        return FALSE;
    }

    IDLE_INFO("Miss Call Count Change, [%d] Miss Call(s).", msg.lParam);

    if (msg.lParam != 0)
    {
        _StatusManager.AddStatus(PS_STATE_MISSCALL, commonAPI_FormatString("%d", msg.lParam));
    }
    else
    {
        _StatusManager.RemoveStatus(PS_STATE_MISSCALL);
    }

    _StatusManager.NotifyStatusChange();

    powerled_FlushPowerLed();

    return TRUE;
}

LRESULT CStatusMonitor::OnForwardMsg(msgObject & msg)
{
    if (LOCAL_CALLLOG_MESSAGE_FORWARD != msg.message)
    {
        return FALSE;
    }

    IDLE_INFO("Forward Count Change, [%d] Forward(s).", msg.lParam);
    if (msg.lParam == 0)
    {
        _StatusManager.RemoveStatus(PS_STATE_CALLFORWARD);
    }

    // http://10.3.5.199/Bug.php?BugID=65757
    // 转移的状态在fwd_ForwardIncomingCall已设置
    // 由于设置的时间早于历史记录的保存时间
    // 导致获取转移的历史记录数量不正确
    // 此处先特殊处理，在收到消息后，再次刷新界面
    _StatusManager.NotifyStatusChange();
    return TRUE;
}

LRESULT CStatusMonitor::OnAccountStatusChange(msgObject & msg)
{
    //状态改变才发Code
    if (msg.lParam != acc_GetState(msg.wParam))
    {
        _StatusMonitor.SendAccountStatusURL(msg.wParam, msg.lParam);
        // 账号变化, 通知UI刷新
        _StatusManager.NotifyStatusChange();
    }
    powerled_FlushPowerLed();

#ifdef IF_FEATURE_PSTN
    if (acc_IsPstnAccount(msg.wParam))
    {
        ListAccounts listAccountId = acc_GetAvailableAccountList(ACCOUNT_TYPE_PSTN);
        if (listAccountId.size() > 0)
        {
            _StatusManager.RemoveStatus(PS_STATE_PSTN_ACCOUNT_ICON);
            _StatusManager.AddStatus(PS_STATE_PSTN_ACCOUNT_ICON, "");
        }
        else
        {
            _StatusManager.RemoveStatus(PS_STATE_PSTN_ACCOUNT_ICON);
        }
        if (!netIsCableConnected())
        {
            //flush net status
            _StatusMonitor.FlushNetStatus();
        }
        else
        {
            _StatusManager.NotifyStatusChange();
        }
    }
#endif

    return TRUE;
}

void CStatusMonitor::SendAccountStatusURL(int iAccount, int eOldStatus)
{
    if (acc_GetState(iAccount) == eOldStatus)
    {
        return;
    }

#if IF_FEATURE_EVENT_MODULE
    EventDataBase eventData;
    eventData.m_wParam = iAccount;

    // 发送注册成功、注册失败、取消注册等Event
    switch (acc_GetState(iAccount))
    {
    case LS_REGISTERED:
        {
            eventData.m_eEventType = EVENT_LOGIN_ON;
            event_SendEvent(&eventData);
        }
        break;
    case LS_REGISTER_FAIL:
        {
            eventData.m_eEventType = EVENT_REGISTER_FAILED;
            event_SendEvent(&eventData);
        }
        break;
    case LS_DISABLE:
        {
            eventData.m_eEventType = EVENT_LOGIN_OFF;
            event_SendEvent(&eventData);
        }
        break;
    default:
        break;
    }
#else

    // 组成数据
    ActionUrlData objValue;
    objValue.m_strSIPURLUser = acc_GetUsername(iAccount);
    objValue.m_strSIPURLHost = acc_GetServerName(iAccount);
    objValue.m_strSIPURL = "sip:" + acc_GetFullName(iAccount);
    //ACTION_INFO("CAccount::SendActionURL[%d][%d]", nAccount, eState);
    // 发送注册成功、注册失败、取消注册等ActionURL
    switch (acc_GetState(iAccount))
    {
    case LS_REGISTERED:
        {
            actURL_SendActionURL(ACTION_URL_LOGIN_ON, objValue);
        }
        break;
    case LS_REGISTER_FAIL:
        {
            actURL_SendActionURL(ACTION_URL_REGISTER_FAILED, objValue);
        }
        break;
    case LS_DISABLE:
        {
            actURL_SendActionURL(ACTION_URL_LOGIN_OFF, objValue);
        }
        break;
    default:
        break;
    }
#endif
}

void CStatusMonitor::InitVoiceStatus()
{
    if (voice_GetVolume(VT_RING) == 0)
    {
        _StatusManager.AddStatus(PS_STATE_RING_MUTE);
    }
}


void CStatusMonitor::InitDHCPOption()
{
    // 首次上电设置第一个账号的sip server为DHCP Option 120
    SetAccountServerByDHCP();
}

yl::string ParseDHCPOption120(const char * szValue)
{
    if (szValue == NULL)
    {
        return "";
    }
    int iLen = strlen(szValue);
    int iPos = 0;

    unsigned int nType = 0;
    sscanf(szValue + 2 * iPos, "%2x", &nType);
    iPos++;

    yl::string strRet;

    unsigned int nTmp = 0;
    if (nType == 0) //域名，ascii 对应关系
    {
        //先不处理
    }
    else if (nType == 1) // ipv4
    {
        for (; iPos < iLen / 2; iPos++)
        {
            sscanf(szValue + 2 * iPos, "%2x", &nTmp);
            char szBuf[4] = {0};
            sprintf(szBuf, "%d", nTmp);

            strRet.append(szBuf);

            if (iPos < 4)
            {
                strRet.append(".");
            }
            else
            {
                break;
            }
        }
    }

    return strRet;
}

void CStatusMonitor::SetAccountServerByDHCP()
{
    //Option 120模式新增2选项，用于SIP Server
    if (configParser_GetConfigInt(kszDhcpOptionMode) != 2
            || IPAM_IPV6 == netGetNetworkMode()
            || WPT_DHCP != netGetNetworkType())
    {
        return;
    }

    yl::string strOption120 = configParser_GetConfigString(kszDHCP120);
    // 转为字符串
    strOption120 = ParseDHCPOption120(strOption120.c_str());

    IDLE_INFO("DHCP Option[%s]", strOption120.c_str());

    yl::string strAccServer = configParser_GetCfgItemStringValue(kszAccountSIPServer1Host, 0);
    if (!strOption120.empty()
            && strAccServer != strOption120
            && configParser_SetCfgItemStringValue(kszAccountSIPServer1Host, 0, strOption120.c_str()))
    {
        ;//Notify Config Change
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, 0);
        //异步通知账号配置改变
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, 0);
    }
}

#ifdef IF_FEATURE_PSTN
BOOL CStatusMonitor::ProcessPstnAccountMsg(msgObject & objMsg)
{
    IDLE_INFO("CStatusMonitor::ProcessPstnAccountMsg %d  %d ", objMsg.wParam, objMsg.lParam);
    if (ACCOUNT_PSTN_CREATED != objMsg.message
            && ACCOUNT_PSTN_REMOVED != objMsg.message)
    {
        return FALSE;
    }

    switch (objMsg.message)
    {
    case ACCOUNT_PSTN_CREATED:
        {
			if (_StatusManager.GetStatusItem(PS_STATE_PSTN_ACCOUNT_NOTE) == NULL)
			{
				_StatusManager.ClearPstnIdList();
			}
			_StatusManager.AddPstnIdToList(objMsg.wParam);
			_StatusManager.AddStatus(PS_STATE_PSTN_ACCOUNT_ICON, "");
			_StatusManager.AddStatus(PS_STATE_PSTN_ACCOUNT_NOTE, "");

			if (!netIsCableConnected())
			{
				//flush net status
				_StatusMonitor.FlushNetStatus();
			}
			etl_NotifyApp(FALSE, ACCOUNT_PSTN_STATUS_CHANGED, objMsg.wParam, 0);
		}
		break;
	case ACCOUNT_PSTN_REMOVED:
		{
			_StatusManager.RemovePstnIdFromList(objMsg.wParam);
			YLVector<int> vecId;
			_StatusManager.GetAddPstnIdList(vecId);
			if (vecId.size() <= 0)
			{
				_StatusManager.RemoveStatus(PS_STATE_PSTN_ACCOUNT_NOTE);
			}
			if (!acc_AccountNum(ACCOUNT_TYPE_PSTN))
			{
				_StatusManager.RemoveStatus(PS_STATE_PSTN_ACCOUNT_ICON);
			}

			if (!netIsCableConnected())
			{
				//flush net status
				_StatusMonitor.FlushNetStatus();
			}
			etl_NotifyApp(FALSE, ACCOUNT_PSTN_STATUS_CHANGED, objMsg.wParam, 0);
		}
		break;
	default:
		break;
	}

	return TRUE;
}
#endif

#endif // ndef T49

#endif // IF_FEATURE_IDLESCREEN

