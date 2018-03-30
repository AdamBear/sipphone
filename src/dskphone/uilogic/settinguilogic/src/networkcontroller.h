#ifndef SETTING_NETWORKCONTROLLER_H_
#define SETTING_NETWORKCONTROLLER_H_

#include "ETLLib.hpp"
#include <ylstring.h>

#include "commondefine.h"

struct MenuItemListData;
struct Network802Dot1xData;
struct NetworkVlanData;
struct NetworkVpnData;
struct NetworkWanPppoeData;
struct NetworkWanStaticIpData;
struct NetworkWanTypeData;
struct NetworkWebServerData;
struct NetworkRouterData;
struct NetworkWanDnsServerData;
struct NetworkLLDPData;
struct NetworkVlanDHCPData;
struct NetworkCDPData;
struct NetworkStunData;

// Class CNetworkController.
class CNetworkController
{
public:
    // Get the singleton instance.
    static CNetworkController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // GetWanIpMode
    // ;0-IPv4,1-IPv6,2-IPv4 and IPv6
    int GetWanIPMode();

    // SetWanIpMode
    // ;0-IPv4,1-IPv6,2-IPv4 and IPv6
    bool SetWanIPMode(int iMode);

    // Get network WAN type data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool GetWanTypeData(NetworkWanTypeData * pData);
    bool GetIPv6WanTypeData(NetworkWanTypeData * pData);
    // Set network WAN type data.
    // | refData | is the data to set.
    void SetWanTypeData(const NetworkWanTypeData & refData);
    void SetIPv6WanTypeData(const NetworkWanTypeData & refData);

    // Get dns server data
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool GetWanStaticDnsServerData(NetworkWanDnsServerData * pData);
    // Set dns server type data.
    // | refData | is the data to set.
    bool SetWanStaticDnsServerData(const NetworkWanDnsServerData & refData);

    // Get WAN static IP data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool GetWanStaticData(NetworkWanStaticIpData * pData);
    // Set WAN static IP data.
    // | refData | is the data to set.
    void SetWanStaticData(const NetworkWanStaticIpData & refData);

    // Get WAN static IPv6 data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool GetWanStaticIPv6Data(NetworkWanStaticIpData * pData);
    // Set WAN static IPv6 data.
    // | refData | is the data to set.
    bool SetWanStaticIPv6Data(const NetworkWanStaticIpData & refData);

    // Get WAN PPPoE data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool GetWanPppoeData(NetworkWanPppoeData * pData);
    // Set WAN PPPoE data.
    // | refData | is the data to set.
    void SetWanPppoeData(const NetworkWanPppoeData & refData);

    // Get PC Port Mode
    // 0-Router, 1-Bridge
    int GetPCPortMode();

    // Get network PC Router data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool GetRouterData(NetworkRouterData * pData);

    // Set PC Port Mode
    // 0-Router, 1-Bridge
    bool SetPCPortMode(int iMode);

    // Set network PC Router data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool SetRouterData(NetworkRouterData * pData);

    // Get VLAN data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool GetVlanData(NetworkVlanData * pData);
    // Set VLAN data.
    // | refData | is the data to set.
    void SetVlanData(const NetworkVlanData & refData);

    // Get VLAN DHCP data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool GetVlanDHCPData(NetworkVlanDHCPData * pData);

    // Set VLAN DHCP data.
    // | refData | is the data to set.
    bool SetVlanDHCPData(const NetworkVlanDHCPData & refData);

    // Get web server data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool GetWebServerData(NetworkWebServerData * pData);
    // Set web server data.
    // | refData | is the data to set.
    void SetWebServerData(const NetworkWebServerData & refData);

    // Get 802.1x data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool Get802Dot1xData(Network802Dot1xData * pData);
    // Set 802.1x data.
    // | refData | is the data to set.
    void Set802Dot1xData(const Network802Dot1xData & refData);

    // Get VPN data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool GetVpnData(NetworkVpnData * pData);
    // Set VPN data.
    // | refData | is the data to set.
    void SetVpnData(const NetworkVpnData & refData);

    // Get LLDP data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool GetLLDPData(NetworkLLDPData * pData);
    // Set LLDP data.
    // | refData | is the data to set.
    bool SetLLDPData(const NetworkLLDPData & refData);

    // Get CDP data.
    // | pData | [out] is the buffer.
    // Return true if successfully, otherwise return false.
    bool GetCDPData(NetworkCDPData * pData);
    // Set LLDP data.
    // | refData | is the data to set.
    bool SetCDPData(const NetworkCDPData & refData);

    // OpenVPN 配置是否存在
    // 返回是否存在
    bool IsOpenVPNConfigExist();

    bool GetStunData(NetworkStunData * pData);

    bool SetStunData(const NetworkStunData & refData);

private:
    // Ctor and Dtor.
    CNetworkController();
    ~CNetworkController();

private:
    static CNetworkController * m_pController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CNetworkController);
};

#define g_pNetworkController (CNetworkController::GetInstance())

#endif  // SETTING_NETWORKCONTROLLER_H_
