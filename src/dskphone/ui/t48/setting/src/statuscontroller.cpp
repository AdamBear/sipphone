#include "statuscontroller.h"

#include "account/include/modaccount.h"
#include "setting/include/common.h"
#include "devicelib/networkserver.h"

#include "wireless/bluetoothNew/include/modbluetooth.h"
#include "wireless/wifi/include/modwifi.h"

#include "basemethod.h"
#include "statusmodel.h"
#include "ETLLib.hpp"

namespace NS_SETTING
{

//////////////////////////////////////////////////////////////////////////
// Class CStatusController.
CStatusController * CStatusController::m_pStatusController = NULL;

CStatusController * CStatusController::GetInstance()
{
    // Instantiates the singleton instance if not exists.
    if (m_pStatusController == NULL)
    {
        m_pStatusController = new CStatusController();
        if (m_pStatusController != NULL)
        {
            // Do initialization.
            m_pStatusController->MyInit();
        }
    }
    return m_pStatusController;
}

void CStatusController::ReleaseInstance()
{
    // Release the singleton instance if exists.
    if (m_pStatusController != NULL)
    {
        // Do finalize.
        m_pStatusController->MyFinal();

        delete m_pStatusController;
        m_pStatusController = NULL;
    }
}

bool CStatusController::GetPageMenuListByClickAction(
    MenuItemListData * pData, const yl::string & strClickAction) const
{
    // Check the click action.
    if (strClickAction.empty() || pData == NULL)
    {
        return false;
    }

    // Call the base method.
    return NS_SETTING::GetPageMenuListByClickAction(pData, strClickAction);
}

const yl::string CStatusController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CStatusController::GetPhoneStatusData(PhoneStatusData * pData) const
{
    if (pData == NULL || m_pStatusModel == NULL)
    {
        return false;
    }

    // Call model's method.
    pData->m_strModel = m_pStatusModel->GetModel();
    pData->m_strProductId = m_pStatusModel->GetProductId();
    pData->m_strMac = netGetMACText(':');
    pData->m_strHardWare = m_pStatusModel->GetHardWare();
    pData->m_strFirmWare = m_pStatusModel->GetFirmware();
    return true;
}

bool CStatusController::GetExternalDeviceData(ExternalDeviceData * pExternalDeviceData) const
{
    if (NULL == pExternalDeviceData)
    {
        return false;
    }

    WIFI_GetLocalMac(pExternalDeviceData->m_strWifiMac);

    DeviceInfo stDeviceInfo;
    if (Bluetooth_GetDeviceInfo(stDeviceInfo))
    {
        pExternalDeviceData->m_strBluetoothMac = stDeviceInfo.strMac;
    }

    return true;
}

bool CStatusController::GetPhoneNetworkStatus(PhoneNetworkStatusData * pNetworkData) const
{
    if (netGetNetworkMode() == IPAM_IPV6)
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
    if (pNetworkData == NULL)
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

    return true;
}

bool CStatusController::GetPhoneIPv6NetworkStatus(PhoneNetworkStatusData * pNetworkData) const
{
    if (pNetworkData == NULL)
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


    yl::string strTmp;
    strTmp.clear();
    char strIPv6StatusInfo[256] = {0};
    yl::string strPrefix;

    strTmp.clear();
    netGetWanPrefixIPv6(strPrefix);
    pNetworkData->m_strPrefix = strPrefix;

    strTmp.clear();
    netGetLocalIPv6(strTmp);
    snprintf(strIPv6StatusInfo, sizeof(strIPv6StatusInfo), "%s/%s", strTmp.c_str(), strPrefix.c_str());
    pNetworkData->m_strWanIP = (LPCTSTR)(strIPv6StatusInfo);

    pNetworkData->m_strWanMask = "";

    strTmp.clear();
    netGetWanGatewayIPv6(strTmp);
    snprintf(strIPv6StatusInfo, sizeof(strIPv6StatusInfo), "%s/%s", strTmp.c_str(), strPrefix.c_str());
    pNetworkData->m_strGateway = (LPCSTR)(strIPv6StatusInfo);

    strTmp.clear();
    netGetWanPrimaryDNSIPv6(strTmp);
    pNetworkData->m_strDnsPrimary = strTmp;

    strTmp.clear();
    netGetWanSecondaryDNSIPv6(strTmp);
    pNetworkData->m_strDnsSecond = strTmp;

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

CStatusController::CStatusController()
    : m_pStatusModel(NULL)
{

}

CStatusController::~CStatusController()
{

}

bool CStatusController::MyInit()
{
    // Initializes the model instance.
    if (m_pStatusModel == NULL)
    {
        m_pStatusModel = new CStatusModel();
    }
    return m_pStatusModel != NULL;
}

bool CStatusController::MyFinal()
{
    // Release the model instance.
    if (m_pStatusModel != NULL)
    {
        delete m_pStatusModel;
        m_pStatusModel = NULL;
    }
    return true;
}

}  // namespace NS_SETTING
