#ifndef __NETWORK_DEFINE_H__
#define __NETWORK_DEFINE_H__

#include "netmsgtypedefine.h"

// !! 此头文件提供给网络进程或网络库使用的公用数据结构. 由于有特定的类型，所以受包含头文件的限制

// !! network shared memory id
#define szSystemNetworkInfo         "networkInfo"
#define szHostnameManager           "hostTaskList"

#define _IPV6

#ifdef _IPV6
struct networkInfo
{
    TASK_ID idDaemonTask;
    BYTE bzMAC[6]; // "\x00\x15\x65\x12\x34\x56"
    enum NETWORK_ERROR_CODE eErrorCode;

    // WAN port
    enum WAN_PORT_STA eIpv4PortSta;
    enum WAN_PORT_STA eIpv6PortSta;
    // ipv4 Port Type
    enum WAN_PORT_TYPE eIpv4PortType;
    enum WAN_6_Port_Type eIpv6PortType;

    enum IP_Addr_Mode eIPAddrMode;

    BOOL bIPv6Conflicted;

    BOOL bWanCableConnected; //网线是否连接
    BOOL bIPConflicted; //IP是否冲突
    struct in_addr addWanIP; //WAN的IP
    struct in_addr addWanMask; //子网掩码
    struct in_addr addWanGateway; //网关
    struct in_addr addWanPrimaryDNS; //主服务器
    struct in_addr addWanSecondaryDNS; //备用服务器
    // LAN port
    BOOL bBridgeMode; //
    struct in_addr addLanIP; //LAN的IP
    struct in_addr addLanMask; //LAN的子网掩码
    // local time
    BOOL bIsTimeTrusted; //时间是否可信
    int offsetDSTSecond; // 夏令时偏移时间 offset for Daylight Saving Time
    int offsetTZSeconds; // 时区偏移时间 offset for time zone

    // auto upgrading
    BOOL bIsAutoUpgrading; //是否自动升级

    // T3X add
    BOOL bIPV6Enable; /*在staticService::OnDelayToResetIP中检测是否为ipv6并设置此值*/
    BOOL bWanVlanEnabled; //
    UINT nWanVlanID; //
    UINT nWanVlanCFI; //
    UINT nWanVlanPRI; //

    UINT nQos;

    UINT nPrefix;

    struct in6_addr in6_addWanIP;
    struct in6_addr in6_addWanMask;
    struct in6_addr in6_addWanGateway;
    struct in6_addr in6_addWanPrimaryDNS;
    struct in6_addr in6_addWanSecondaryDNS;

    BOOL bLanVlanEnabled;
    UINT nLanVlanID;
    UINT nLanVlanCFI;
    UINT nLanVlanPRI;
};

#else
// network shared memory data struct
struct networkInfo
{
    TASK_ID                     idDaemonTask;
    BYTE                        bzMAC[6];           // "\x00\x15\x65\x12\x34\x56"
    enum NETWORK_ERROR_CODE     eErrorCode;
    // WAN port
    enum WAN_PORT_STA           eWanPortSta;        //port的状态
    enum WAN_PORT_TYPE          eWanPortType;       //port的类型
    BOOL                        bWanCableConnected; //网线是否连接
    BOOL                        bIPConflicted;      //IP是否冲突
    struct in_addr              addWanIP;           //WAN的IP
    struct in_addr              addWanMask;         //子网掩码
    struct in_addr              addWanGateway;      //网关
    struct in_addr              addWanPrimaryDNS;   //主服务器
    struct in_addr              addWanSecondaryDNS; //备用服务器
    // LAN port
    BOOL                        bBridgeMode;        //
    struct in_addr              addLanIP;           //LAN的IP
    struct in_addr              addLanMask;         //LAN的子网掩码
    // local time
    BOOL                        bIsTimeTrusted;     //时间是否可信
    int
    offsetDSTSecond;    // 夏令时偏移时间 offset for Daylight Saving Time
    int                         offsetTZSeconds;    // 时区偏移时间 offset for time zone

    // auto upgrading
    BOOL                        bIsAutoUpgrading;   //是否自动升级

    // T3X add
    BOOL
    bIPV6Enable;        /*在staticService::OnDelayToResetIP中检测是否为ipv6并设置此值*/
    BOOL                        bWanVlanEnabled;    //
    UINT                        nWanVlanID;         //
    UINT                        nWanVlanCFI;        //
    UINT                        nWanVlanPRI;        //

    struct in6_addr             in6_addWanIP;
    struct in6_addr             in6_addWanMask;
    struct in6_addr             in6_addWanGateway;
    struct in6_addr             in6_addWanPrimaryDNS;
    struct in6_addr             in6_addWanSecondaryDNS;

    BOOL                        bLanVlanEnabled;
    UINT                        nLanVlanID;
    UINT                        nLanVlanCFI;
    UINT                        nLanVlanPRI;
};
#endif


#endif //__NETWORK_DEFINE_H__
