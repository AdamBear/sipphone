#include "idlescreen_inc.h"
#include "statusmanager.h"
#include "statusmonitor.h"
#include "dsskeyswitchaccountaction.h"
#include "wireless/wifi/include/modwifi.h"
#include "adapterbox/include/modadapterbox.h"

// idle处理的初始化
void idleScreen_Init(bool bIsXwin/* = true*/)
{
    _StatusManager.SetIsXwin(bIsXwin);

#ifndef IF_NOTIFICATION_CENTER_ON_IDLE
    if (!bIsXwin)
    {
        //flush net status
        _StatusMonitor.FlushNetStatus();

        // 判断VPM是否连接
        bool bIsVPNLink = (configParser_GetConfigInt(kszOpenvpnStatus) != 0);
        if (bIsVPNLink)
        {
            _StatusManager.AddStatus(PS_STATE_VPN, "");
        }
    }
#endif

    // Turn ON.
    _StatusManager.TurnNotify(true);

    //Idle Dsskey Init
    new CDsskeySwitchAccountAction(DT_IDLE_SWITCHACCOUNT, true);
    new CDsskeySwitchAccountAction(DT_IDLE_SWITCHACCOUNT_UP, false);
    new CDsskeySwitchAccountAction(DT_IDLE_SWITCHACCOUNT_DOWN, true);
}

#ifdef IF_FEATURE_IDLESCREEN
#ifndef IF_NOTIFICATION_CENTER_ON_IDLE
void idleScreen_RegisterStatus()
{
    bool IsPopWind = !configParser_GetConfigInt(kszStatusChangeIsNotPopWind); //状态改变
    PopupWindowType wintypeStatusChange = IsPopWind ? POPUP_WINDOW_TEXT_NOTIFY : POPUP_WINDOW_NO_POPUP;
    //注册状态
    _StatusManager.RegisterStatus(PS_STATE_RING_MUTE, POPUP_WINDOW_NO_POPUP,
                                  1); //状态固定在第一位
    _StatusManager.RegisterStatus(PS_STATE_DND, POPUP_WINDOW_NO_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_AA, POPUP_WINDOW_NO_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_FWD, POPUP_WINDOW_NO_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_LOCK, POPUP_WINDOW_NO_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_MUTE, POPUP_WINDOW_NO_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_VPN, POPUP_WINDOW_NO_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_NETWORK_FAILED, POPUP_WINDOW_TEXT_NOTIFY, 0);
    _StatusManager.RegisterStatus(PS_STATE_NETWORK_OK_BUT_CONFLICT, POPUP_WINDOW_TEXT_NOTIFY, 0);
    _StatusManager.RegisterStatus(PS_STATE_CALLFORWARD, wintypeStatusChange, 0);
    _StatusManager.RegisterStatus(PS_STATE_MISSCALL, wintypeStatusChange, 0);
    _StatusManager.RegisterStatus(PS_STATE_VOICEMAIL, POPUP_WINDOW_NO_POPUP,
                                  0); //VoiceMail弹窗配另外处理
    _StatusManager.RegisterStatus(PS_STATE_TEXTMESSAGE, wintypeStatusChange, 0);
    _StatusManager.RegisterStatus(PS_STATE_HEADSET, POPUP_WINDOW_NO_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_REMOTE_CONTACT_DOWNLOAD_FAILED, POPUP_WINDOW_NO_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_BLUETOOTH, POPUP_WINDOW_NO_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_BLUETOOTH_CONNECT, POPUP_WINDOW_NO_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_BLUETOOTH_MOBILECONNECT, POPUP_WINDOW_NO_POPUP, 0);

    _StatusManager.RegisterStatus(PS_STATE_TALK_MUTE, POPUP_WINDOW_NO_POPUP, 2);


#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    _StatusManager.RegisterStatus(PS_STATE_USBDEVICE, POPUP_WINDOW_NO_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_USBDEVICE_DETECTING, POPUP_WINDOW_NO_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_USBDEVICE_DETECTFAILED, wintypeStatusChange, 0);
    _StatusManager.RegisterStatus(PS_STATE_SAVE_RECORD_FILE, POPUP_WINDOW_POPUP, 0);
#endif
    //弹出框注册
    _StatusManager.RegisterStatus(PS_STATE_CTR_REQUEST, POPUP_WINDOW_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_AUTO_PROVISION, POPUP_WINDOW_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_CALL_COMPLETION, POPUP_WINDOW_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_NOTE_HOTDESK, POPUP_WINDOW_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_PICKUP_MENU, POPUP_WINDOW_POPUP, 0);
#if IF_BUG_23548
    _StatusManager.RegisterStatus(PS_STATE_CALL_COMPLETION_CALLBACK, POPUP_WINDOW_POPUP, 0);
#endif
#ifdef IF_FEATURE_PSTN
    _StatusManager.RegisterStatus(PS_STATE_PSTN_ACCOUNT_ICON, POPUP_WINDOW_NO_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_PSTN_ACCOUNT_NOTE, POPUP_WINDOW_POPUP, 0);
#endif
    _StatusManager.RegisterStatus(PS_STATE_AUTO_PROVISION_UPDATING, POPUP_WINDOW_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_AUTO_PROVISION_UPDATE_SUCCESS, POPUP_WINDOW_POPUP, 0);
    _StatusManager.RegisterStatus(PS_STATE_AUTO_PROVISION_UPDATE_FAILED, POPUP_WINDOW_POPUP, 0);
#if IF_FEATURE_XMLBROWSER_HOTELING
    _StatusManager.RegisterStatus(PS_STATE_HOTELING_LOGOUT_PROMPT, POPUP_WINDOW_POPUP, 0);
#endif
}

bool idleScreen_RegisterStatus(PhoneState nStatus, PopupWindowType ePopupStatus, int nLoc)
{
    return _StatusManager.RegisterStatus(nStatus, ePopupStatus, nLoc);
}

YLList<IdleStatusItem> idleScreen_GetStatusList(int iCount /*= -1*/)
{
    return _StatusManager.GetTopStateList(iCount);
}

IdleStatusItem idleScreen_GetTopStatusNotifyType()
{
    IdleStatusItem stTopStatusNotify;
    YLList<IdleStatusItem> listStatus = idleScreen_GetStatusList();

    for (YLList<IdleStatusItem>::ListIterator iter = listStatus.begin();
            iter != listStatus.end(); ++ iter)
    {
        if ((*iter).m_nPopupType == POPUP_WINDOW_TEXT_NOTIFY)
        {
            stTopStatusNotify = *iter;
            break;
        }
    }

    return stTopStatusNotify;
}

IdleStatusItem idleScreen_GetTopStatusPopType()
{
    IdleStatusItem stTopStatusNotify;
    YLList<IdleStatusItem> listStatus = idleScreen_GetStatusList();

    for (YLList<IdleStatusItem>::ListIterator iter = listStatus.begin();
            iter != listStatus.end(); ++iter)
    {
        if ((*iter).m_nPopupType == POPUP_WINDOW_POPUP
                || (*iter).m_nPopupType == POPUP_WINDOW_POPUPING)
        {
            stTopStatusNotify = *iter;
            break;
        }
    }

    return stTopStatusNotify;
}

YLVector<int> idleScreen_GetStatusToPopup()
{
    return _StatusManager.GetStatusToPopup();
}

bool idleScreen_SetNotifyFunByCount(StatusNotifyFunction func, int nStatusCount)
{
    return _StatusManager.SetNotifyFunByCount(func, nStatusCount);
}

bool idleScreen_SetPopupStatus(int nStatus, int nPopupStatus)
{
    _StatusManager.SetPopupStatus(nStatus, nPopupStatus);
    return true;
}

bool idleScreen_ClearPopupFlag(int nStatus)
{
    _StatusManager.ClearPopupFlag(nStatus);
    return true;
}

int idleScreen_GetPopupStatus(int nStatus)
{
    return _StatusManager.GetPopupStatus(nStatus);
}

bool idleScreen_SetPopupType(int nStatus, PopupWindowType ePopupType)
{
    CStatusManager::StatusConfigItem * pConfig = _StatusManager.GetConfigItem(nStatus);
    if (pConfig)
    {
        pConfig->m_nPopupType = ePopupType;
        return true;
    }
    return false;
}
#endif // IF_NOTIFICATION_CENTER_ON_IDLE

bool idleScreen_RegisterStatus(PhoneState nStatus, StateNotifyType eNotifyType,
                               int iPriority /*= 0*/)
{
    return _StatusManager.RegisterStatus(nStatus, eNotifyType, iPriority);
}

YLList<IdleStatusItem> idleScreen_GetStatusList(StateNotifyType eNotifyType)
{
    return _StatusManager.GetNotifyStateList(eNotifyType);
}

IdleStatusItem idleScreen_GetTopStatus(StateNotifyType eNotifyType)
{
    YLList<IdleStatusItem> StatusItemList = _StatusManager.GetNotifyStateList(eNotifyType);
    if (StatusItemList.size() == 0)
    {
        return IdleStatusItem();
    }
    else
    {
        return (*(StatusItemList.begin()));
    }
}

bool idleScreen_SetNotifyFunction(StatusNotifyFunction func, StateNotifyType eNotifyType)
{
    return _StatusManager.SetNotifyFunction(func, eNotifyType);
}

bool idleScreen_ChangeHint(int nStatus, const yl::string strHint/* = ""*/,
                           unsigned int uNotifyType/*= SNT_ICON*/)
{
    return _StatusManager.ChangeHint(nStatus, strHint, uNotifyType);
}

bool idleScreen_ClearNotifyTypeFlag(int nStatus, unsigned int iNotifyType)
{
    _StatusManager.ClearNotifyTypeFlag(nStatus, iNotifyType);
    return true;
}

bool idleScreen_ClearRegStatusTypeFlag(int nStatus, unsigned int iNotifyType)
{
    _StatusManager.ClearCfgStatusFlag(nStatus, iNotifyType);
    return true;
}
unsigned int idleScreen_GetNotifyType(int nStatus)
{
    return _StatusManager.GetNotifyType(nStatus);
}

bool idleScreen_SetNotifyType(int nStatus, StateNotifyType eNotifyType, int iPriority)
{
    return _StatusManager.SetNotifyType(nStatus, eNotifyType, iPriority);
}
////////////////////////////
// Status 部分            //
////////////////////////////

bool idleScreen_SetStatus(int nStatus, const yl::string strHint /*= ""*/)
{
    _StatusManager.AddStatus(nStatus, strHint);
    return _StatusManager.NotifyStatusChange();
}

bool idleScreen_DropStatus(int nStatus, bool bNeedNotify/* = true*/)
{
    bool bFlush = !bNeedNotify;
    if (_StatusManager.RemoveStatus(nStatus, bFlush) && bNeedNotify)
    {
        //没有改变就不通知
        return _StatusManager.NotifyStatusChange();
    }

    return false;
}

bool idleScreen_TurnStatusNotify(bool bNotified /*= true*/)
{
    _StatusManager.TurnNotify(bNotified);
    return true;
}

// 通过静态变量确认是否是Queue Status调用画屏
static bool s_bQueueStatusCusNotify = false;
// 通过静态变量保存
static typeCusNotifyList s_listCusNotifyList;
/*******************************************************************
** 函数描述:   idleui更新自定义提示
********************************************************************/
BOOL idleScreen_UpdateCustomNotify(typeCusNotifyList & listCusNotify)
{
    s_bQueueStatusCusNotify = false;
    s_listCusNotifyList.clear();

    typeCusNotifyList::iterator iter = listCusNotify.begin();
    for (; iter != listCusNotify.end(); ++iter)
    {
        s_listCusNotifyList.push_back((*iter));
    }

    // 更新idleui
    AdapterBox_XmlBrowserUpdate();

    return TRUE;
}

// idleui获取自定义提示
typeCusNotifyList * idleScreen_GetCustomNotify()
{
    return &s_listCusNotifyList;
}

bool idleScreen_GetQueueStatusCusNotify()
{
    return s_bQueueStatusCusNotify;
}

void idleScreen_SetQueueStatusCusNotify(bool bQueueStatus)
{
    s_bQueueStatusCusNotify = bQueueStatus;
}

bool idleScreen_SetStatusChangeCallback(int nStatus, StatusChangeFunction funcCallback)
{
    return _StatusManager.SetStatusChangeCallback(nStatus, funcCallback);
}

#else
void idleScreen_RegisterStatus()
{
}

// idle处理的初始化
void idleScreen_Init()
{
}

bool idleScreen_RegisterStatus(PhoneState nStatus, PopupWindowType ePopupStatus, int nLoc)
{
    return false;
}

YLVector<IdleStatusItem> idleScreen_GetStatusList(int iCount /*= -1*/)
{
    static YLVector<IdleStatusItem> sStatus;
    return sStatus;
}

IdleStatusItem idleScreen_GetTopStatusNotifyType()
{
    static IdleStatusItem stTopStatusNotify;
    return stTopStatusNotify;
}

IdleStatusItem idleScreen_GetTopStatusPopType()
{
    static IdleStatusItem stTopStatusNotify;

    return stTopStatusNotify;
}

YLVector<int> idleScreen_GetStatusToPopup()
{
    static YLVector<int> sStatus;
    return sStatus;
}

bool idleScreen_SetNotifyFunByCount(StatusNotifyFunction func, int nStatusCount)
{
    return false;
}

bool idleScreen_SetStatus(int nStatus, const yl::string strHint /*= ""*/)
{
    return false;
}

bool idleScreen_DropStatus(int nStatus)
{
    return false;
}

bool idleScreen_SetPopupStatus(int nStatus, int nPopupStatus)
{
    return false;
}

bool idleScreen_ClearPopupFlag(int nStatus)
{
    return false;
}

int idleScreen_GetPopupStatus(int nStatus)
{
    return 0;
}

bool idleScreen_TurnStatusNotify(bool bNotified /*= true*/)
{
    return false;
}

BOOL idleScreen_UpdateCustomNotify(typeCusNotifyList & listCusNotify)
{
    return false;
}

// idleui获取自定义提示
typeCusNotifyList * idleScreen_GetCustomNotify()
{
    return NULL;
}

bool idleScreen_GetQueueStatusCusNotify()
{
    return false;
}

void idleScreen_SetQueueStatusCusNotify(bool bQueueStatus)
{

}

bool idleScreen_SetPopupType(int nStatus, PopupWindowType ePopupType)
{
    return false;
}

bool idleScreen_SetStatusChangeCallback(int nStatus, StatusChangeFunction funcCallback)
{
    return false;
}

#endif // IF_FEATURE_IDLESCREEN

bool idleScreen_IsNetworkAvailable()
{
    // http://10.2.1.199/Bug.php?BugID=57723
    // IPv4 或者 IPv6有一个准备好了，就认为网络ok
    return (netIsOpenIPv4Mode() && netGetNetworkSta() == WPS_READY)
           || (netIsOpenIPv6Mode() && netGetIPv6Status() == WPS_READY);
}

NETWORK_STATUS idleScreen_GetNetworkStatus(bool bRefresh/* = false*/)
{
    static NETWORK_STATUS lastStatus = NS_OK;
    while (bRefresh)
    {
        yl::string strBuffer = netGetMACText(':');
        if (strBuffer == "00:00:00:00:00:00") //MAC检查中
        {
            IDLE_INFO("[Network] CHECKING_MAC");
            lastStatus = NS_CHECKING_MAC;
            break;
        }

        if (commonUnit_IsMatchTestMode()) //无效MAC
        {
            IDLE_INFO("[Network] INVALID_MAC");
            lastStatus = NS_INVALID_MAC;
            break;
        }

        if (!netIsCableConnected()
#ifdef IF_SUPPORT_WIFI
#ifdef _T49
                && !WIFI_IsEnable() //未连接
#else
                && !WIFI_IsEnable(false) //未连接
#endif
#endif
           )
        {
            IDLE_INFO("[Network] CABLE_DISCONNECTED");
            lastStatus = NS_CABLE_DISCONNECTED;
            break;
        }

        // IP冲突
        int nIpv4Satus = sys_get_network_status(SYS_PHY_TYPE_WAN, SYS_IP_PROTO_IPV4);
        int nIpv6Status = sys_get_network_status(SYS_PHY_TYPE_WAN, SYS_IP_PROTO_IPV6);
        bool bIpv4Conflict = (SYS_NET_STATUS_CONFLICT == nIpv4Satus || SYS_NET_STATUS_EXIST == nIpv4Satus);
        bool bIpv6Conflict = (SYS_NET_STATUS_CONFLICT == nIpv6Status
                              || SYS_NET_STATUS_EXIST == nIpv6Status);

        if (bIpv4Conflict && bIpv6Conflict)
        {
            IDLE_INFO("[Network] ALL_IP_CONFLICT");
            lastStatus = NS_ALL_IP_CONFLICT;
            break;
        }
        else if (bIpv4Conflict)
        {
            IDLE_INFO("[Network] IPV4_CONFLICT");
            lastStatus = NS_IPV4_CONFLICT;
            break;
        }
        else if (bIpv6Conflict)
        {
            IDLE_INFO("[Network] IPV6_CONFLICT");
            lastStatus = NS_IPV6_CONFLICT;
            break;
        }

        if (idleScreen_IsNetworkAvailable())
        {
            IDLE_INFO("[Network] OK");
            lastStatus = NS_OK;
            break;
        }

        WAN_PORT_STA v4status = netGetNetworkSta();
        WAN_PORT_STA v6status = netGetIPv6Status();

        if (v4status == WPS_REQUESTING_IP
                || v6status == WPS_REQUESTING_IP)
        {
            IDLE_INFO("[Network] REQUEST_IP");
            lastStatus = NS_REQUEST_IP;
            break;
        }
        else if (v4status == WPS_UNKNOW
                 || v6status == WPS_UNKNOW)
        {
            IDLE_INFO("[Network] UNKNOWN");
            lastStatus = NS_UNKNOWN;
            break;
        }
        else if ((v4status == WPS_STOPED || v6status == WPS_STOPED)
#ifdef IF_SUPPORT_WIFI
#ifdef _T49
                 && !WIFI_IsEnable()
#else
                 && !WIFI_IsEnable(false)
#endif
#endif //IF_SUPPORT_WIFI
                )
        {
            IDLE_INFO("[Network] WPS_STOPED");
            lastStatus = NS_NETWORK_STOP;
            break;
        }
        else
        {
            IDLE_INFO("[Network] UPDATING");
            lastStatus = NS_UPDATING;
            break;
        }
    }

    return lastStatus;
}

#ifdef IF_FEATURE_PSTN
void idleScreen_GetCurrAddPstnId(YLVector<int> & vecID)
{
    _StatusManager.GetAddPstnIdList(vecID);
}


void idleScreen_ClearPstnIdList()
{
    _StatusManager.ClearPstnIdList();
}
#endif
