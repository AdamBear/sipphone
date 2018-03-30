#include "statuscontroller.h"

#include "account/include/modaccount.h"
#include "settinguilogic/include/common.h"
#include "devicelib/networkserver.h"
#include "ETLLib.hpp"


//////////////////////////////////////////////////////////////////////////
// Class CStatusController.
CStatusController * CStatusController::m_pStatusController = NULL;

CStatusController * CStatusController::GetInstance()
{
    // Instantiates the singleton instance if not exists.
    if (NULL == m_pStatusController)
    {
        m_pStatusController = new CStatusController();
    }
    return m_pStatusController;
}

void CStatusController::ReleaseInstance()
{
    // Release the singleton instance if exists.
    if (NULL != m_pStatusController)
    {
        delete m_pStatusController;
        m_pStatusController = NULL;
    }
}

bool CStatusController::GetPhoneStatusData(PhoneStatusData * pData) const
{
    if (NULL == pData)
    {
        return false;
    }

    // Call model's method.
    pData->m_strModel = GetModel();
    pData->m_strProductId = GetProductId();
#if IF_MAC_SEPARATE_BY_COLON
    pData->m_strMac = netGetMACText(':');
#else
    pData->m_strMac = netGetMACText('-');
#endif
    pData->m_strHardWare = GetHardWare();
    pData->m_strFirmWare = GetFirmware();
#ifdef IF_SUPPORT_WIFI
    pData->m_strWifiMac = configParser_GetConfigString(kszWifiAdapterMac);
#endif
#ifdef IF_SUPPORT_BLUETOOTH
    pData->m_strBluetoothMac = configParser_GetConfigString(kszBluetoothAdapterMac);
#endif
    return true;
}

bool CStatusController::GetPhoneNetworkStatus(PhoneNetworkStatusData * pNetworkData) const
{
    if (IPAM_IPV6 == netGetNetworkMode())
    {
        return GetPhoneIPv6NetworkStatus(pNetworkData);
    }
    else
    {
        return GetPhoneIPv4NetworkStatus(pNetworkData);
    }
}

// Get phone network status.
// | pNetworkData | is the network status data retrieved.
// Returns true if retrieved successfully, otherwise returns false.
bool CStatusController::GetPhoneIPv4NetworkStatus(PhoneNetworkStatusData * pNetworkData) const
{
    if (NULL == pNetworkData)
    {
        return false;
    }

    // Get phone network status.
    switch (netGetNetworkType())
    {
    case WPT_DHCP:
        pNetworkData->m_iWanType = WAN_DHCP;
        break;
    case WPT_PPPOE:
        pNetworkData->m_iWanType = WAN_PPPOE;
        break;
    case WPT_STATIC_IP:
        pNetworkData->m_iWanType = WAN_STATIC;
        break;
    default:
        pNetworkData->m_iWanType = WAN_TYPE_NONE;
        break;
    }

    yl::string strTmp;

    strTmp.clear();
    netGetLocalIPv4(strTmp);
    pNetworkData->m_strWanIP = strTmp;

    strTmp.clear();
    netGetWanNetmaskIPv4(strTmp);
    pNetworkData->m_strWanMask = strTmp;

    strTmp.clear();
    netGetWanGatewayIPv4(strTmp);
    pNetworkData->m_strGateway = strTmp;

    strTmp.clear();
    netGetWanPrimaryDNSIPv4(strTmp);
    pNetworkData->m_strDnsPrimary = strTmp;

    strTmp.clear();
    netGetWanSecondaryDNSIPv4(strTmp);
    pNetworkData->m_strDnsSecond = strTmp;

#ifdef IF_SUPPORT_WIFI
    pNetworkData->m_strWifiMac = configParser_GetConfigString(kszWifiAdapterMac);
#endif

    return true;
}

bool CStatusController::GetPhoneIPv6NetworkStatus(PhoneNetworkStatusData * pNetworkData) const
{
    if (NULL == pNetworkData)
    {
        return false;
    }

    // Get phone network status.
    switch (netGetIPv6Type())
    {
    case W6PT_INVALID:
    case W6PT_DHCP:
        pNetworkData->m_iWanType = WAN_DHCP;
        break;
    case W6PT_Static_IP:
        pNetworkData->m_iWanType = WAN_STATIC;
        break;
    default:
        // 从网络读取-1，网络处暂没有处理默认值，所以，-1时也设置为DHCP
        // Bug: http://10.2.1.199/Bug.php?BugID=45202
        pNetworkData->m_iWanType = WAN_TYPE_NONE;
        //pNetworkData->m_eWanType = WAN_TYPE_NONE;
        break;
    }

    yl::string strIPv6StatusInfo;
    yl::string strPrefix;
    yl::string strTmp;

    strTmp.clear();
    netGetWanPrefixIPv6(strPrefix);
    pNetworkData->m_strPrefix = strPrefix;

    strTmp.clear();
    netGetLocalIPv6(strTmp);

    strIPv6StatusInfo = strTmp + "/" + strPrefix;
    pNetworkData->m_strWanIP = strIPv6StatusInfo;

    pNetworkData->m_strWanMask = "";

    strTmp.clear();
    netGetWanGatewayIPv6(strTmp);
    strIPv6StatusInfo = strTmp + "/" + strPrefix;
    pNetworkData->m_strGateway = strIPv6StatusInfo;

    strTmp.clear();
    netGetWanPrimaryDNSIPv6(strTmp);
    pNetworkData->m_strDnsPrimary = strTmp;

    strTmp.clear();
    netGetWanSecondaryDNSIPv6(strTmp);
    pNetworkData->m_strDnsSecond = strTmp;

#ifdef IF_SUPPORT_WIFI
    pNetworkData->m_strWifiMac = configParser_GetConfigString(kszWifiAdapterMac);
#endif

    return true;
}

bool CStatusController::GetPhoneAccountStatus(AccountStatusListData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // 从Account模块获取账号列表.
    AccountStatusData stASD;
    int iAccountNum = acc_AccountNum();
    for (int i = 0; i < iAccountNum; i++)
    {
        stASD.Reset();
        int nAccountId = i;

        //http://10.2.1.199/Bug.php?BugID=99826 统一通过接口acc_GetAccountShowText获取显示
        stASD.m_strAccountName = acc_GetAccountShowText(nAccountId);
        stASD.m_nStatus = (int)acc_GetState(nAccountId);
        stASD.m_nID = nAccountId;
        pData->m_listStatus.push_back(stASD);
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
const yl::string CStatusController::GetModel() const
{
    yl::string strModel;
    char bufProductName[512] = { 0 };
    sys_replace_wildcards((char*)configParser_GetConfigString(kszProductName).c_str(), 0,
                          bufProductName, sizeof(bufProductName) / sizeof(char));
    strModel = bufProductName;

    return strModel;
}

const yl::string CStatusController::GetHardWare() const
{
    // Read phone hardware value.
    return devicelib_GetHardwareVersion();
}

const yl::string CStatusController::GetFirmware() const
{
    // Read phone firmware value.
    char szFirmware[32] = { 0 };
    yl::string strFirmware = "";
#ifndef WIN32
    if (0 == sys_get_sw_version(szFirmware, 32))
#endif
    {
        strFirmware = szFirmware;
    }
    return strFirmware;
}

const yl::string CStatusController::GetProductId() const
{
    // Read phone product Id.
    char buf[128];
    sys_get_rom_info(SYS_ROM_INFO_PACKED_TIME, buf, 128);
    yl::string strProductId(buf);

    strProductId.replace("-", "");
    strProductId.replace(":", "");
    strProductId.replace(" ", "");
    strProductId = commonAPI_TrimString(strProductId, "CST");

    return strProductId;
}

CStatusController::CStatusController()
{

}

CStatusController::~CStatusController()
{

}
