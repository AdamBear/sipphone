#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiDriver.cpp
//  Implementation of the Class CWifiDriver
//  Created on:      15-五月-2014 9:18:03
//  Original author: songk
///////////////////////////////////////////////////////////
#include "wifi_inc.h"
#include "sub-interface.h"

IMPLEMENT_GETINSTANCE(CWifiDriver)
#define VP_RTSERVER_THREAD_NAME "netServer"

#define  _WIFI_GETMKHANDLE mkGetHandle()
#define  WIFI_SYSTIMEOUT     10*1000
#define  WIFI_SCAN_CYCLE     10         //s 为单位

#if IF_WIFI_SIMULATION_DRIVER
#define INTERFACE_WIFI_SETGLOBAL_FUNC    _WiFiTestDriver.SetGlobalFunc
#define INTERFACE_WIFI_SYNCSTATUS        _WiFiTestDriver.SyncStatus
#define INTERFACE_WIFI_CONFIGCHANGED     _WiFiTestDriver.ConfigChanged
#define INTERFACE_WIFI_GETAPID           _WiFiTestDriver.GetAPID
#define INTERFACE_WIFI_CONNECT_ACTION    _WiFiTestDriver.ConnectAction
#define INTERFACE_WIFI_SCAN_ACTION       _WiFiTestDriver.ScanAction
#define INTERFACE_WIFI_WPS_ACTION        _WiFiTestDriver.WpsAction
#define INTERFACE_WIFI_GETPORTSTATUS     _WiFiTestDriver.GetPortStatus
#define INTERFACE_WIFI_GETNETWORK_STATUS _WiFiTestDriver.GetNetworkStatus
#define INTERFACE_WIFI_GETAPLIST         _WiFiTestDriver.GetAPList
#define INTERFACE_WIFI_GETMAC            _WiFiTestDriver.GetMacString
#else
#define INTERFACE_WIFI_SETGLOBAL_FUNC    sys_wifi_set_global_func
#define INTERFACE_WIFI_SYNCSTATUS        sys_wifi_sync_status
#define INTERFACE_WIFI_CONFIGCHANGED     sys_wifi_config_changed
#define INTERFACE_WIFI_GETAPID           sys_wifi_get_ap_id
#define INTERFACE_WIFI_CONNECT_ACTION    sys_wifi_connect_action
#define INTERFACE_WIFI_SCAN_ACTION       sys_wifi_scan_action
#define INTERFACE_WIFI_PERIOD_SCAN       sys_wifi_period_scan
#define INTERFACE_WIFI_WPS_ACTION        sys_wifi_wps_action
#define INTERFACE_WIFI_GETPORTSTATUS     sys_wifi_get_port_staus
#define INTERFACE_WIFI_GETNETWORK_STATUS sys_wifi_get_network_status
#define INTERFACE_WIFI_GETAPLIST         sys_wifi_get_ap_list
#define INTERFACE_WIFI_GETMAC            sys_get_MAC_string
#endif //IF_WIFI_SIMULATION_DRIVER

CWifiDriver::CWifiDriver()
{
#if IF_WIFI_SIMULATION_DRIVER
    _WiFiTestDriver;
#endif

#ifdef _T49
    SetDongleEnable(CheckDongleStatus());
#endif
}

CWifiDriver::~CWifiDriver()
{

}

//* 关闭有线网路
bool CWifiDriver::CloseCable()
{
    return false;
}

//* 关闭无线网路
bool CWifiDriver::CloseWifi()
{
    return 0 == INTERFACE_WIFI_SETGLOBAL_FUNC(_WIFI_GETMKHANDLE, SYS_WIFI_GLOBAL_OFF, WIFI_SYSTIMEOUT);
}

//* 切换为有线网路
bool CWifiDriver::OpenCable()
{
    return false;
}

// 切换为wifi网路
bool CWifiDriver::OpenWifi()
{
    return 0 == INTERFACE_WIFI_SETGLOBAL_FUNC(_WIFI_GETMKHANDLE, SYS_WIFI_GLOBAL_ON, WIFI_SYSTIMEOUT);
}

// 刷新数据
bool CWifiDriver::RefreshState()
{
    return 0 == INTERFACE_WIFI_SYNCSTATUS(_WIFI_GETMKHANDLE, WIFI_SYSTIMEOUT);
}

// 保存配置
bool CWifiDriver::SaveConfig(int iSectionID, int iNetID)
{
    WIRE_INFO("wifi[%d],section[%d] config change", iNetID, iSectionID);

    return INTERFACE_WIFI_CONFIGCHANGED(_WIFI_GETMKHANDLE, WIFI_SYSTIMEOUT) >= 0;
}

//获取net id
int CWifiDriver::GetNetIDBySectionID(int iSectionID, const yl::string & strSSID)
{
    if (strSSID.empty())
    {
        return WIFI_NET_ID_INVAILED;
    }

    return INTERFACE_WIFI_GETAPID(mkGetHandle(), iSectionID, (char *)strSSID.c_str(),
                                  strSSID.size() + 1, WIFI_SYSTIMEOUT);
}

//开始连接网络
bool CWifiDriver::StartConnect(int iNetID, const AuthInfo & authInfo)
{
    //拷贝psk码
    sys_wifi_nw_auth auth ;
    memset(&auth, 0, sizeof(sys_wifi_nw_auth));

    strncpy(auth.identity, authInfo.strEapName.c_str(), WIFI_PASSWD_MAX - 1);
    strncpy(auth.passwd, authInfo.strEapPwd.c_str(), WIFI_NAME_MAX - 1);
    strncpy(auth.psk, authInfo.strPsk.c_str(), WIFI_PASSWD_MAX - 1);
    auth.identity[WIFI_NAME_MAX - 1] = '\0';
    auth.passwd[WIFI_PASSWD_MAX - 1] = '\0';
    auth.psk[WIFI_PASSWD_MAX - 1] = '\0' ;

    WIRE_INFO("connect ap[%d]", iNetID);
    WIRE_INFO("eap name[%s], eap pwd[%s], psk[%s]", auth.identity, auth.passwd, auth.psk);

    return INTERFACE_WIFI_CONNECT_ACTION(_WIFI_GETMKHANDLE, SYS_WIFI_OP_CONNECT_START, iNetID, &auth,
                                         WIFI_SYSTIMEOUT) >= 0;
}

//开始断开网络
bool CWifiDriver::StartDisconnect(int iNetID)
{
    sys_wifi_nw_auth auth ;
    return INTERFACE_WIFI_CONNECT_ACTION(_WIFI_GETMKHANDLE, SYS_WIFI_OP_CONNECT_STOP, iNetID, &auth,
                                         WIFI_SYSTIMEOUT) >= 0;
}

//开始搜索
bool CWifiDriver::StartScan(int iLoop, bool bRest)
{
    return 0 == INTERFACE_WIFI_SCAN_ACTION(_WIFI_GETMKHANDLE, SYS_WIFI_OP_SCAN_START, WIFI_SYSTIMEOUT);
}

//停止搜素
bool CWifiDriver::StopScan()
{
    return 0 == INTERFACE_WIFI_SCAN_ACTION(_WIFI_GETMKHANDLE, SYS_WIFI_OP_SCAN_STOP, WIFI_SYSTIMEOUT);
}

//开始周期扫描
bool  CWifiDriver::StartCycleScan()
{
    return 0 == INTERFACE_WIFI_PERIOD_SCAN(_WIFI_GETMKHANDLE, SYS_WIFI_PERIOD_SCAN_START,
                                           WIFI_SCAN_CYCLE, WIFI_SYSTIMEOUT);
}

//停止周期扫描
bool  CWifiDriver::StopCycleScan()
{
    return 0 == INTERFACE_WIFI_PERIOD_SCAN(_WIFI_GETMKHANDLE, SYS_WIFI_PERIOD_SCAN_STOP, 0,
                                           WIFI_SYSTIMEOUT);
}

//wps功能开启
bool CWifiDriver::StartWps()
{
    return 0 == INTERFACE_WIFI_WPS_ACTION(_WIFI_GETMKHANDLE, SYS_WIFI_OP_WPS_START,
                                          SYS_WIFI_WPS_TYPE_PBC, WIFI_SYSTIMEOUT);
}

//刷新wps-pin 功能
bool CWifiDriver::RefreshPinCode()
{
    return 0 == INTERFACE_WIFI_WPS_ACTION(_WIFI_GETMKHANDLE, SYS_WIFI_OP_WPS_GET_PIN,
                                          SYS_WIFI_WPS_TYPE_PIN, WIFI_SYSTIMEOUT);
}

//wps-pin功能开启
bool CWifiDriver::StartWpspin()
{
    return 0 == INTERFACE_WIFI_WPS_ACTION(_WIFI_GETMKHANDLE, SYS_WIFI_OP_WPS_START,
                                          SYS_WIFI_WPS_TYPE_PIN, WIFI_SYSTIMEOUT);
}

//关闭wps功能
bool CWifiDriver::StopWps(int iType)
{
    return 0 == INTERFACE_WIFI_WPS_ACTION(_WIFI_GETMKHANDLE, SYS_WIFI_OP_WPS_STOP, iType,
                                          WIFI_SYSTIMEOUT);
}

//查询适配器状态
bool CWifiDriver::CheckDongleStatus()
{
    return (INTERFACE_WIFI_GETPORTSTATUS() == 1);
}

bool CWifiDriver::IsDongleValid(bool bCheck /* = false */)
{
    if (!bCheck)
    {
        return m_bAdapterValid;
    }
    else
    {
        return CheckDongleStatus();
    }
}

//获取网络状态
bool CWifiDriver::GetNetworkStatus(_networkStatusInfo & statusInfo)
{
    sys_wifi_nw_stat_t stat_t;
    if (0 == INTERFACE_WIFI_GETNETWORK_STATUS(&stat_t, sizeof(sys_wifi_nw_stat_t)))
    {
        statusInfo.iDeviceID = stat_t.nw_id;
        statusInfo.iGroupCipher = stat_t.group_cipher;
        statusInfo.iKeyMgmt = stat_t.key_mgmt;
        statusInfo.iLinkSpeed = stat_t.link_speed;
        statusInfo.iPairCipher = stat_t.pairwise_cipher;
        statusInfo.iWpaState = stat_t.wpa_state;
        statusInfo.strName = stat_t.name;
        statusInfo.authInfo.strWpsPin = stat_t.nw_auth.wps_pin;

#if 0
        statusInfo.authInfo.iWepKeyIndex = stat_t.nw_auth.wep_key_idx;
        statusInfo.authInfo.strIdentity = stat_t.nw_auth.identity;
        statusInfo.authInfo.strPasswd = stat_t.nw_auth.passwd;
        statusInfo.authInfo.strPsk = stat_t.nw_auth.psk;
        statusInfo.authInfo.strWepKey = stat_t.nw_auth.wep_key;
#endif // 0
        return true;
    }

    return false;
}

//获取扫描列表
bool CWifiDriver::GetNeworkList(YLList<_networkPropertyInfo> & networkList)
{
    sys_wifi_ap_t apList[WIFI_AP_LIST_MAX] = {0};
    _networkPropertyInfo netProperty;
    if (0 == INTERFACE_WIFI_GETAPLIST(apList, WIFI_AP_LIST_MAX * sizeof(sys_wifi_ap_t)))
    {
        for (int i = 0 ; i < WIFI_AP_LIST_MAX; i++)
        {
            if ((WIFI_NET_ID_INVAILED == apList[i].ap_id)
                    || WIFI_AP_INACTIVE == apList[i].ap_status
                    || strcmp("", apList[i].name) == 0)
            {
                continue;
            }

            netProperty.Reset();

            /*ap_id作为唯一标示*/
            netProperty.iDeviceID = apList[i].nw_id;    //nw id
            netProperty.iNetID = apList[i].ap_id;       // ap id
            netProperty.iCaps = apList[i].caps;         //capibility
            netProperty.iSignal = apList[i].signal;     //信号强度
            netProperty.iFreq = apList[i].freq;         //网络频段
            netProperty.iAuth = apList[i].auth;
            netProperty.iStatus = apList[i].ap_status;

            const unsigned char * pMac = apList[i].ap_mac;
            char szMac[18] = { 0 };
            snprintf(szMac, sizeof(szMac), "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", pMac[0], pMac[1], pMac[2], pMac[3],
                     pMac[4], pMac[5]);
            netProperty.strMac = szMac;
            netProperty.strName = apList[i].name;

            networkList.push_back(netProperty);
        }

        return true;
    }

    return false;
}

//获取dongle 设备地址
bool CWifiDriver::GetDongleMac(yl::string & strMac)
{
    char szMac[SYS_MAC_BUF_LEN] = {0};
    if (0 == INTERFACE_WIFI_GETMAC(SYS_PHY_TYPE_WIFI, szMac, SYS_MAC_BUF_LEN))
    {
        yl::string strTempMac = szMac;
        if (strTempMac.length() != 12)
        {
            return false;
        }

        memset(szMac, 0, SYS_MAC_BUF_LEN);
        sprintf(szMac, "%s:%s:%s:%s:%s:%s",
                strTempMac.substr(0, 2).c_str(),
                strTempMac.substr(2, 2).c_str(),
                strTempMac.substr(4, 2).c_str(),
                strTempMac.substr(6, 2).c_str(),
                strTempMac.substr(8, 2).c_str(),
                strTempMac.substr(10, 2).c_str());

        strMac = szMac;
        return true;
    }

    return false;
}

#endif // IF_SUPPORT_WIFI
