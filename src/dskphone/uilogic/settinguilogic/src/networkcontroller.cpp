#include "networkcontroller.h"

#include "path_define.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "commonunits/modcommonunits.h"
#include "settinguilogic/include/common.h"
#include "devicelib/phonedevice.h"
#include "interfacedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "devicelib/networkserver.h"

//////////////////////////////////////////////////////////////////////////
// Class CNetworkController.
CNetworkController * CNetworkController::m_pController = NULL;

// Static.
CNetworkController * CNetworkController::GetInstance()
{
    // Instantiate the singleton instance if not existed.
    if (m_pController == NULL)
    {
        m_pController = new CNetworkController();
    }
    return m_pController;
}

// Static.
void CNetworkController::ReleaseInstance()
{
    // Release the singleton instance if existed.
    DELETE_OBJ(m_pController);
}

int CNetworkController::GetWanIPMode()
{
    return configParser_GetConfigInt(kszNetworkIPAddressMode);
}

// SetWanIpMode
// ;0-IPv4,1-IPv6,2-IPv4 and IPv6
bool CNetworkController::SetWanIPMode(int iMode)
{
    return configParser_SetConfigInt(kszNetworkIPAddressMode, iMode, CONFIG_LEVEL_PHONE);
}

bool CNetworkController::GetWanTypeData(NetworkWanTypeData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // Get network WAN type.
    pData->m_eType =
        static_cast<WanType>(configParser_GetConfigInt(kszNetworkWanType));
    return true;
}

bool CNetworkController::GetIPv6WanTypeData(NetworkWanTypeData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // Get network WAN type.
    pData->m_eType =
        static_cast<WanType>(configParser_GetConfigInt(kszNetworkIPv6WANType));
    return true;
}

void CNetworkController::SetWanTypeData(const NetworkWanTypeData & refData)
{
    // Set network WAN type.
    configParser_SetConfigInt(kszNetworkWanType, refData.m_eType, CONFIG_LEVEL_PHONE);
}

void CNetworkController::SetIPv6WanTypeData(const NetworkWanTypeData & refData)
{
    // Set IPv6 network WAN type.
    configParser_SetConfigInt(kszNetworkIPv6WANType, refData.m_eType, CONFIG_LEVEL_PHONE);
}

bool CNetworkController::GetWanStaticDnsServerData(NetworkWanDnsServerData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    if (IPAM_IPV4 == pData->m_nMode)
    {
        // Get static ip.
        pData->m_bStaticDns = (configParser_GetConfigInt(kszNetworkWanStaticDNSEnable) == 1);
        // Get primary dns.
        pData->m_strWanPrimaryDns = configParser_GetConfigString(kszNetworkWanStaticPriDns);
        // Get secondary dns.
        pData->m_strWanSendaryDns = configParser_GetConfigString(kszNetworkWanStaticSecDns);
    }
    else
    {
        // Get static ip.
        pData->m_bStaticDns = (configParser_GetConfigInt(kszNetworkIPv6WanStaticDNSEnable) == 1);
        // Get primary dns.
        pData->m_strWanPrimaryDns = configParser_GetConfigString(kszNetworkIPv6WanPrimaryDNS);
        // Get secondary dns.
        pData->m_strWanSendaryDns = configParser_GetConfigString(kszNetworkIPv6WanSecondaryDNS);
    }

    return true;
}

bool CNetworkController::SetWanStaticDnsServerData(const NetworkWanDnsServerData & refData)
{
    bool bChanged = false;

    if (IPAM_IPV4 == refData.m_nMode)
    {
        // Set static dns enable.
        bChanged = configParser_SetConfigInt(kszNetworkWanStaticDNSEnable, refData.m_bStaticDns,
                                             CONFIG_LEVEL_PHONE) || bChanged;
        // Set primary dns.
        bChanged = configParser_SetConfigString(kszNetworkWanStaticPriDns,
                                                refData.m_strWanPrimaryDns.c_str(), CONFIG_LEVEL_PHONE) || bChanged;
        // Set secondary dns.
        bChanged = configParser_SetConfigString(kszNetworkWanStaticSecDns,
                                                refData.m_strWanSendaryDns.c_str(), CONFIG_LEVEL_PHONE) || bChanged;
    }
    else
    {
        // Set static dns enable.
        bChanged = configParser_SetConfigInt(kszNetworkIPv6WanStaticDNSEnable, refData.m_bStaticDns,
                                             CONFIG_LEVEL_PHONE) || bChanged;
        // Set primary dns.
        bChanged = configParser_SetConfigString(kszNetworkIPv6WanPrimaryDNS,
                                                refData.m_strWanPrimaryDns.c_str(), CONFIG_LEVEL_PHONE) || bChanged;
        // Set secondary dns.
        bChanged = configParser_SetConfigString(kszNetworkIPv6WanSecondaryDNS,
                                                refData.m_strWanSendaryDns.c_str(), CONFIG_LEVEL_PHONE) || bChanged;
    }

    return bChanged;
}

bool CNetworkController::GetWanStaticData(NetworkWanStaticIpData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // Get static ip.
    pData->m_strWanIp = configParser_GetConfigString(kszNetworkWanStaticIp);
    // Get static mask.
    pData->m_strWanMask = configParser_GetConfigString(kszNetworkWanStaticMask);
    // Get static gateway.
    pData->m_strWanGateWay = configParser_GetConfigString(kszNetworkWanStaticGateWay);
    // Get primary dns.
    pData->m_strWanPrimaryDns = configParser_GetConfigString(kszNetworkWanStaticPriDns);
    // Get secondary dns.
    pData->m_strWanSendaryDns = configParser_GetConfigString(kszNetworkWanStaticSecDns);
    return true;
}

void CNetworkController::SetWanStaticData(const NetworkWanStaticIpData & refData)
{
    // Set static ip.
    configParser_SetConfigString(kszNetworkWanStaticIp, refData.m_strWanIp.c_str(), CONFIG_LEVEL_PHONE);
    // Set static mask.
    configParser_SetConfigString(kszNetworkWanStaticMask, refData.m_strWanMask.c_str(),
                                 CONFIG_LEVEL_PHONE);
    // Set static gateway.
    configParser_SetConfigString(kszNetworkWanStaticGateWay, refData.m_strWanGateWay.c_str(),
                                 CONFIG_LEVEL_PHONE);
    // Set primary dns.
    configParser_SetConfigString(kszNetworkWanStaticPriDns, refData.m_strWanPrimaryDns.c_str(),
                                 CONFIG_LEVEL_PHONE);
    // Set secondary dns.
    configParser_SetConfigString(kszNetworkWanStaticSecDns, refData.m_strWanSendaryDns.c_str(),
                                 CONFIG_LEVEL_PHONE);
}

// Get WAN static IPv6 data.
bool CNetworkController::GetWanStaticIPv6Data(NetworkWanStaticIpData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // Get static ip.
    pData->m_strWanIp = configParser_GetConfigString(kszNetworkWanIPv6WanIP);
    // Get static mask.
    pData->m_strWanMask = configParser_GetConfigString(kszNetworkWanIPv6Prefix);
    // Get static gateway.
    pData->m_strWanGateWay = configParser_GetConfigString(kszNetworkIPv6WanGateway);
    // Get primary dns.
    pData->m_strWanPrimaryDns = configParser_GetConfigString(kszNetworkIPv6WanPrimaryDNS);
    // Get secondary dns.
    pData->m_strWanSendaryDns = configParser_GetConfigString(kszNetworkIPv6WanSecondaryDNS);
    return true;
}

// Set WAN static IPv6 data.
bool CNetworkController::SetWanStaticIPv6Data(const NetworkWanStaticIpData & refData)
{
    bool bChanged = false;
    // Set static ip.
    bChanged = configParser_SetConfigString(kszNetworkWanIPv6WanIP, refData.m_strWanIp.c_str(),
                                            CONFIG_LEVEL_PHONE) || bChanged;
    // Set static mask.
    bChanged = configParser_SetConfigString(kszNetworkWanIPv6Prefix, refData.m_strWanMask.c_str(),
                                            CONFIG_LEVEL_PHONE) || bChanged;
    // Set static gateway.
    bChanged = configParser_SetConfigString(kszNetworkIPv6WanGateway, refData.m_strWanGateWay.c_str(),
                                            CONFIG_LEVEL_PHONE) || bChanged;
    // Set primary dns.
    bChanged = configParser_SetConfigString(kszNetworkIPv6WanPrimaryDNS,
                                            refData.m_strWanPrimaryDns.c_str(), CONFIG_LEVEL_PHONE) || bChanged;
    // Set secondary dns.
    bChanged = configParser_SetConfigString(kszNetworkIPv6WanSecondaryDNS,
                                            refData.m_strWanSendaryDns.c_str(), CONFIG_LEVEL_PHONE) || bChanged;

    return bChanged;
}

bool CNetworkController::GetWanPppoeData(NetworkWanPppoeData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // Get pppoe user.
    pData->m_strPppoeUser = configParser_GetConfigString(kszNetworkPppoeUser);
    // Get pppoe pin.
    pData->m_strPppoePin = configParser_GetConfigString(kszNetworkPppoePin);
    return true;
}

void CNetworkController::SetWanPppoeData(const NetworkWanPppoeData & refData)
{
    // Set pppoe user.
    configParser_SetConfigString(kszNetworkPppoeUser, refData.m_strPppoeUser.c_str(),
                                 CONFIG_LEVEL_PHONE);
    // Set pppoe pin.
    configParser_SetConfigString(kszNetworkPppoePin, refData.m_strPppoePin.c_str(), CONFIG_LEVEL_PHONE);
}

// Get PC Port Mode
// 0-Router, 1-Bridge
int CNetworkController::GetPCPortMode()
{
    return configParser_GetConfigInt(kszNetworkLanType);
}

// Get network PC Router data.
bool CNetworkController::GetRouterData(NetworkRouterData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    pData->m_strIPAddress = configParser_GetConfigString(kszNetworkLanRouterIP);
    pData->m_strNetMask = configParser_GetConfigString(kszNetworkLanSubnetMask);
    pData->m_bDHCPServer = (configParser_GetConfigInt(kszNetworkLanEnableDHCP) != 0);
    pData->m_strStartIP = configParser_GetConfigString(kszNetworkLanDHCPStartIP);
    pData->m_strEndIP = configParser_GetConfigString(kszNetworkLanDHCPEndIP);
    return true;
}

// Set PC Port Mode
// 0-Router, 1-Bridge
bool CNetworkController::SetPCPortMode(int iMode)
{
    return configParser_SetConfigInt(kszNetworkLanType, iMode, CONFIG_LEVEL_PHONE);
}

// Set network PC Router data.
// | pData | [out] is the buffer.
// Return true if successfully, otherwise return false.
bool CNetworkController::SetRouterData(NetworkRouterData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    configParser_SetConfigString(kszNetworkLanRouterIP, pData->m_strIPAddress.c_str(),
                                 CONFIG_LEVEL_PHONE);;
    configParser_SetConfigString(kszNetworkLanSubnetMask, pData->m_strNetMask.c_str(),
                                 CONFIG_LEVEL_PHONE);;
    configParser_SetConfigInt(kszNetworkLanEnableDHCP, pData->m_bDHCPServer, CONFIG_LEVEL_PHONE);;
    configParser_SetConfigString(kszNetworkLanDHCPStartIP, pData->m_strStartIP.c_str(),
                                 CONFIG_LEVEL_PHONE);
    configParser_SetConfigString(kszNetworkLanDHCPEndIP, pData->m_strEndIP.c_str(), CONFIG_LEVEL_PHONE);
    ;
    return true;
}

bool CNetworkController::GetVlanData(NetworkVlanData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // Get VLAN WAN status.
    pData->m_isWanEnable = configParser_GetConfigInt(kszVlanWanSwitch) == 1;
    // Get VLAN WAN VID.
    pData->m_nWanVid = configParser_GetConfigInt(kszVlanWanVid);
    // Get VLAN WAN priority.
    pData->m_nWanPriority = configParser_GetConfigInt(kszVlanWanPriority);

    // Get VLAN PC status.
    pData->m_isPcEnable = configParser_GetConfigInt(kszVlanPcSwitch) == 1;
    // Get VLAN PC VID.
    pData->m_nPcVid = configParser_GetConfigInt(kszVlanPcVid);
    // Get VLAN PC priority.
    pData->m_nPcPriority = configParser_GetConfigInt(kszVlanPcPriority);
    return true;
}

void CNetworkController::SetVlanData(const NetworkVlanData & refData)
{
    // Set VLAN WAN status.
    configParser_SetConfigInt(kszVlanWanSwitch, refData.m_isWanEnable ? 1 : 0, CONFIG_LEVEL_PHONE);
    // Set VLAN WAN VID.
    configParser_SetConfigInt(kszVlanWanVid, refData.m_nWanVid, CONFIG_LEVEL_PHONE);
    // Set VLAN WAN priority.
    configParser_SetConfigInt(kszVlanWanPriority, refData.m_nWanPriority, CONFIG_LEVEL_PHONE);

    // Set VLAN PC status.
    configParser_SetConfigInt(kszVlanPcSwitch, refData.m_isPcEnable ? 1 : 0, CONFIG_LEVEL_PHONE);
    // Set VLAN PC VID.
    configParser_SetConfigInt(kszVlanPcVid, refData.m_nPcVid, CONFIG_LEVEL_PHONE);
    // Set VLAN PC priority.
    configParser_SetConfigInt(kszVlanPcPriority, refData.m_nPcPriority, CONFIG_LEVEL_PHONE);
}

bool CNetworkController::GetLLDPData(NetworkLLDPData * pData)
{
    if (NULL == pData)
    {
        return false;
    }

    // Get lldp status.
    pData->m_bEnable = configParser_GetConfigInt(kszLLDPSwitch) == 1;
    // Get lldp Packet Interval
    pData->m_nPacketInterval = configParser_GetConfigInt(kszLLDPPacketInterval);

    return true;
}

bool CNetworkController::SetLLDPData(const NetworkLLDPData & refData)
{
    bool bChanged = false;

    // Set lldp status.
    bChanged = configParser_SetConfigInt(kszLLDPSwitch, refData.m_bEnable ? 1 : 0, CONFIG_LEVEL_PHONE)
               || bChanged;
    // Set lldp Packet Interval
    bChanged = configParser_SetConfigInt(kszLLDPPacketInterval, refData.m_nPacketInterval,
                                         CONFIG_LEVEL_PHONE) || bChanged;

    return bChanged;
}

bool CNetworkController::GetCDPData(NetworkCDPData * pData)
{
    if (NULL == pData)
    {
        return false;
    }

    // Get lldp status.
    pData->m_bEnable = configParser_GetConfigInt(kszCDPEnable) == 1;
    // Get lldp Packet Interval
    pData->m_nCDPInterval = configParser_GetConfigInt(kszCDPInterveral);

    return true;
}

bool CNetworkController::SetCDPData(const NetworkCDPData & refData)
{
    bool bChanged = false;

    // Set lldp status.
    bChanged = configParser_SetConfigInt(kszCDPEnable, refData.m_bEnable ? 1 : 0, CONFIG_LEVEL_PHONE)
               || bChanged;
    // Set lldp Packet Interval
    bChanged = configParser_SetConfigInt(kszCDPInterveral, refData.m_nCDPInterval, CONFIG_LEVEL_PHONE)
               || bChanged;

    return bChanged;
}

bool CNetworkController::GetVlanDHCPData(NetworkVlanDHCPData * pData)
{
    if (NULL == pData)
    {
        return false;
    }

    // Get VLAN DHCP status.
    pData->m_bEnable = configParser_GetConfigInt(kszVlanDhcpSwitch) == 1;
    // Get VLAN DHCP option.
    pData->m_strOption = configParser_GetConfigString(kszVlanDhcpOption);

    return true;
}

bool CNetworkController::SetVlanDHCPData(const NetworkVlanDHCPData & refData)
{
    bool bChanged = false;

    // Set VLAN DHCP status.
    bChanged = configParser_SetConfigInt(kszVlanDhcpSwitch, refData.m_bEnable ? 1 : 0,
                                         CONFIG_LEVEL_PHONE) || bChanged;
    // Set VLAN DHCP option.
    bChanged = configParser_SetConfigString(kszVlanDhcpOption, refData.m_strOption.c_str(),
                                            CONFIG_LEVEL_PHONE) || bChanged;

    return bChanged;
}


bool CNetworkController::GetWebServerData(NetworkWebServerData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // Get web server type.
    pData->m_bHTTP_On = (bool)configParser_GetConfigInt(kszHttpServerSwitch);
    pData->m_strHTTP_Port = configParser_GetConfigString(kszHttpServerPort);
    pData->m_bHTTPS_On = (bool)configParser_GetConfigInt(kszHttpsServerSwitch);
    pData->m_strHTTPS_Port = configParser_GetConfigString(kszHttpsServerPort);
#if IF_BUG_29939
    pData->m_strLoginTimer = configParser_GetConfigString(kszWebLoginTimer);
    pData->m_strActivityTimer = configParser_GetConfigString(kszWebActivityTimer);
    pData->m_strMaxTimer = configParser_GetConfigString(kszWebMaxTimer);
#endif
    return true;
}

void CNetworkController::SetWebServerData(const NetworkWebServerData & refData)
{
    // Set web server type.
    configParser_SetConfigInt(kszHttpServerSwitch, refData.m_bHTTP_On, CONFIG_LEVEL_PHONE);
    configParser_SetConfigString(kszHttpServerPort, refData.m_strHTTP_Port.c_str(), CONFIG_LEVEL_PHONE);
    configParser_SetConfigInt(kszHttpsServerSwitch, refData.m_bHTTPS_On, CONFIG_LEVEL_PHONE);
    configParser_SetConfigString(kszHttpsServerPort, refData.m_strHTTPS_Port.c_str(),
                                 CONFIG_LEVEL_PHONE);
#if IF_BUG_29939
    configParser_SetConfigString(kszWebLoginTimer, refData.m_strLoginTimer.c_str(), CONFIG_LEVEL_PHONE);
    configParser_SetConfigString(kszWebActivityTimer, refData.m_strActivityTimer.c_str(),
                                 CONFIG_LEVEL_PHONE);
    configParser_SetConfigString(kszWebMaxTimer, refData.m_strMaxTimer.c_str(), CONFIG_LEVEL_PHONE);
#endif
}

bool CNetworkController::Get802Dot1xData(Network802Dot1xData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // Get 802.1x mode.
    pData->m_eMode =
        static_cast<Network802DotxModeType>(configParser_GetConfigInt(ksz802Dot1xMode));
    // Get 802.1x id.
    pData->m_strId = configParser_GetConfigString(ksz802Dot1xId);
    // Get 802.1x password.
    pData->m_strPassword = configParser_GetConfigString(ksz802Dot1xPassword);
    return true;
}

void CNetworkController::Set802Dot1xData(const Network802Dot1xData & refData)
{
    // Set 802.1x mode.
    configParser_SetConfigInt(ksz802Dot1xMode, refData.m_eMode, CONFIG_LEVEL_PHONE);
    // Set 802.1x id.
    configParser_SetConfigString(ksz802Dot1xId, refData.m_strId.c_str(), CONFIG_LEVEL_PHONE);
    // Set 802.1x password.
    configParser_SetConfigString(ksz802Dot1xPassword, refData.m_strPassword.c_str(),
                                 CONFIG_LEVEL_PHONE);
}

bool CNetworkController::GetVpnData(NetworkVpnData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // Get vpn status.
    pData->m_isEnable = configParser_GetConfigInt(kszVpnOnSwitch) == 1;
    return true;
}

void CNetworkController::SetVpnData(const NetworkVpnData & refData)
{
    // Set vpn status.
    configParser_SetConfigInt(kszVpnOnSwitch, refData.m_isEnable ? 1 : 0, CONFIG_LEVEL_PHONE);
}

bool CNetworkController::GetStunData(NetworkStunData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    pData->m_bEnable = configParser_GetConfigInt(kszEnableStun) == 1;
    pData->m_strStunSeverAddr = configParser_GetConfigString(kszStunServer);
    pData->m_iPort = configParser_GetConfigInt(kszStunPort);
    return true;
}

bool CNetworkController::SetStunData(const NetworkStunData & refData)
{
    configParser_SetConfigInt(kszEnableStun, refData.m_bEnable ? 1 : 0, CONFIG_LEVEL_PHONE);
    configParser_SetConfigString(kszStunServer, refData.m_strStunSeverAddr.c_str(), CONFIG_LEVEL_PHONE);
    configParser_SetConfigInt(kszStunPort, refData.m_iPort, CONFIG_LEVEL_PHONE);
    return true;
}

bool CNetworkController::IsOpenVPNConfigExist()
{
    static char kszOpenVPNConfigPath[] = CONFIG_PATH"openvpn/vpn.cnf";
#ifdef WIN32
    return false;
#else
    return (access(kszOpenVPNConfigPath, F_OK) == 0) ? true : false;
#endif
}

CNetworkController::CNetworkController()
{

}

CNetworkController::~CNetworkController()
{

}
