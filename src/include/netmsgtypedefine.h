#ifndef __NET_MSG_TYPE_DEFINE_H__
#define __NET_MSG_TYPE_DEFINE_H__

#include "service_system.h"

// !! 此头文件提供给各进程使用的公用消息和类型
/******************* network message *******************************/
//---message broadcast by rtServer
//////////   NetworkMessageDefine  ///////////
enum NETWORK_MSG_DEF
{
    MSG_NETWORK_CHANGING        =   0x00002001,
    MSG_NETWORK_CHANGED         =   0x00002002,
    MSG_WAN_CABLE_CONNECTED     =   0x00002003,
    MSG_WAN_CABLE_DISCONNECT    =   0x00002004,
    // wParam: 0/1/2 (no conflict)/ipv4 conflict/ipv6 conflict , lParam: 0/1 (冲突别人)/(被别人冲突)
    MSG_WAN_IP_CONFLICTED       =   0x00002005,
    // Des:系统时间有改变，如手动设置时间，或NTP时间获取到了
    // wParam=0, lParam=0
    MSG_SYSTEM_TIME_CHANGED     =   0x00002006,
    // Des:本地时间发生变化：重设基准UTC时间引起该消息，配置时区或者夏令时(DST)引发该消息
    // wParam=0, lParam=0 (Autop的更新策略会用到)
    MSG_LOCAL_TIME_CHANGED      =   0x00002007,
    // Des:IP冲突中，貌似没人用
    // wParam=0, lParam=0
    MSG_WAN_IP_CONFLICTING      =   0x00002008,
    // Des:rtServer 系统命令
    // wParam=0, lParam=0, Ext: command string
    MSG_RT_SYSTEM_CMD           =   0x00002010,
    // Des:更新时区时间
    // wParam=0, lParam=0
    MSG_UPDATE_TIMEZONE         =   0x00002011,

    //---command to rtServer
    MSG_RESETUP_NETWORK         =   0x00002101,     // wParam=0, lParam=0
    MSG_START_NTP_CLIENT        =   0x00002102,     // wParam=0, lParam=0
    MSG_STOP_NTP_CLIENT         =   0x00002103,     // wParam=0, lParam=0
    MSG_WAN_CHECK_IP_USED       =   0x00002104,     // wParam=IPv4, lParam=timeout
    MSG_GET_HOST_BY_NAME        =   0x00002105,     // relult=in_addr   wParam=0 lParam=timeout ex=szHostName
};
// 通过域名获取IP时通过线程池实现
#define MSG_CREATE_HOST_THREAD          MSG_GET_HOST_BY_NAME

/******************* network enum *******************************/
// WAN口类型
enum WAN_PORT_TYPE
{
    WPT_INVALID = -1,
    WPT_DHCP = 0,
    WPT_PPPOE = 1,
    WPT_STATIC_IP = 2,
    WPT_DHCP_MANUAL = 3,
    WPT_OTHER,
};
// WAN口状态
enum WAN_PORT_STA
{
    WPS_STOPED = -1,
    WPS_UNKNOW = 0,
    WPS_REQUESTING_IP,
    WPS_READY,
    WPS_ERROR,
};
// 网络错误状态
enum NETWORK_ERROR_CODE
{
    NEC_SUCCESS = 0,
    NEC_REQUEST_TIMEOUT,
    NEC_AUTHENTICATION_FAIL,
    NEC_LINKSET_FAIL,
    NEC_ALLOC_IP_FAIL,
};

enum IP_Addr_Mode
{
    IPAM_IPV4 = 0,
    IPAM_IPV6 = 1,
    IPAM_BOTH = 2,
};

enum WAN_6_Port_Type
{
    W6PT_INVALID    = -1,
    W6PT_DHCP       = 0,
    W6PT_Static_IP  = 1,
    W6PT_OTHER,
};

enum IPAddrType
{
    IAT_Unknown = -1,           //unknown
    IAT_IPv4 = 0,               //ipv4
    IAT_IPv6 = 1,               //ipv6
};


#endif //__NET_MSG_TYPE_DEFINE_H__

