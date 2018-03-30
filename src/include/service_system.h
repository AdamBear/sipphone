/* ******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2014-04-11
*
* for export APIs header file , system serice
*
****************************************************************** */
#ifndef __SRV_SYSTEM_H__
#define __SRV_SYSTEM_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef _MSGKIT
#include <msgkit/mkmessage.h>
#include <msgkit/mkservice.h>
#include <arpa/inet.h>
#include <sys/socket.h>       /* for AF_INET */
#include <time.h>
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/************************************************************************/
/* common&limit define                                                  */
/************************************************************************/
/*@@Limitation@@
* max of name
*/
#define SYS_IPv4_BUF_LEN    16  /* eg: 192.168.100.200 */
#define SYS_IPv6_BUF_LEN    48  /* eg: 2001:0DB8:0000:0000:0000:0000:1428:57ab */
#define SYS_MAC_BUF_LEN     24  /* eg: 000C299FA9E4 */

#define SYS_STR_LEN_MAX     64

#define  SYS_NAMAGE_NAME        "sys"

/*
 *@@DeviceLayer@@
 * type of
 */
/*@@PhysicalLayer@@
* type of Physical port, internet/pc/wifi/3g/4g/bt
*/
#define  SYS_PHY_TYPE_INV          -1      /*invalid type ,should NOT happen*/
#define  SYS_PHY_TYPE_WAN          0       /*ethernet WAN port or internet port*/
#define  SYS_PHY_TYPE_LAN          1       /*ethernet LAN port or PC port*/
#define  SYS_PHY_TYPE_WIFI         2       /*WIFI connection port, can be USB wifi or build-in wifi*/
#define  SYS_PHY_TYPE_3G4G         3       /*3G/4G connection port, should be USB module*/
#define  SYS_PHY_TYPE_BT           4       /*blue-tooth network,out of consideration*/
#define  SYS_PHY_TYPE_VPN          5       /* vpn interface */


/*
* type of speed definination
*/
#define  SYS_SPEED_INV           0x0000        /*invalid or none*/
#define  SYS_SPEED_AUTO          0x0001        /*auto negotiation*/
#define  SYS_SPEED_10_HD         0x0002        /*10M half duplex*/
#define  SYS_SPEED_10_FD         0x0004        /*10M full duplex*/
#define  SYS_SPEED_100_HD        0x0008        /*100M half duplex*/
#define  SYS_SPEED_100_FD        0x0010        /*100M full duplex*/
#define  SYS_SPEED_1000_HD       0x0020        /*1000M half duplex*/
#define  SYS_SPEED_1000_FD       0x0040        /*1000M full duplex*/

/*
* type of status for Physical port
*/
#define  SYS_PHY_STATUS_INV        -1       /*invalid status ,should NOT happen*/
#define  SYS_PHY_STATUS_OFF        0        /*physical connection is off*/
#define  SYS_PHY_STATUS_NEG        1        /*connection is on negotiation or authentication*/
#define  SYS_PHY_STATUS_ON         2        /*connection is ready*/
#define  SYS_PHY_STATUS_RESET      3        /*connection is reset by peer*/
#define  SYS_PHY_STATUS_ERROR      8        /*connection is under error*/

/*
* type of status of wifi port
*/
#define SYS_WIFI_STATUS_CONNECTED            0x0/* 成功连接上 */
#define SYS_WIFI_STATUS_DISCONNECTED         0x1/* 包括UI主动断开的连接、AP信号消失等 */

/*@@LinkLayer@@
* type of LinkLayer type, LLDP/VLAN/802.1x
*/
#define  SYS_LL_TYPE_INV          -1      /*invalid status ,should NOT happen*/
#define  SYS_LL_TYPE_NONE         0       /*link-layer is under default*/
#define  SYS_LL_TYPE_LLDP         1       /*link-layer running LLDP*/
#define  SYS_LL_TYPE_CDP          2       /*link-layer running CDP*/
#define  SYS_LL_TYPE_8021X        4       /*link-layer running 802.1x*/

/*
* type of status for LinkLayer
*/
#define  SYS_LL_STATUS_INV        -1       /*invalid status ,should NOT happen*/
#define  SYS_LL_STATUS_OFF        0        /*link-layer connection is off*/
#define  SYS_LL_STATUS_NEG        1        /*link-layer is on negotiation or authentication*/
#define  SYS_LL_STATUS_ON         2        /*link-layer is ready*/
#define  SYS_LL_STATUS_RESET      3        /*link-layer is reset by peer*/
#define  SYS_LL_STATUS_ERROR      8        /*link-layer is under error*/

/*@@NetworkLayer@@
* type of Network type (available for DHCP/Static/PPPoE)
*/
#define  SYS_NET_TYPE_INV          -1      /*invalid status ,should NOT happen*/
#define  SYS_NET_TYPE_NONE         0       /*network is under default*/
#define  SYS_NET_TYPE_DHCP         1       /*network running DHCP*/
#define  SYS_NET_TYPE_STATIC       2       /*network running Static*/
#define  SYS_NET_TYPE_PPPOE        4       /*network running PPPoE*/

/*
* type of status for Network
*/
#define  SYS_NET_STATUS_INV        -1       /*invalid status ,should NOT happen*/
#define  SYS_NET_STATUS_OFF         0        /*network is hang*/
#define  SYS_NET_STATUS_NEG         1        /*network is on negotiation or authentication*/
#define  SYS_NET_STATUS_READY       2        /*network is ready*/
#define  SYS_NET_STATUS_EXIST       3        /*network is IP exist*/
#define  SYS_NET_STATUS_CONFLICT    4        /*network is confilct by other*/
#define  SYS_NET_STATUS_RESET       5        /*network is reset by peer*/
#define  SYS_NET_STATUS_NO_CONFLICT 6        /*network recover from conflict */
#define  SYS_NET_STATUS_ERROR       8        /*network is under error*/

/*@@InternetLayer@@
* type of IP version (IPv4/IPv6)
*/
#define  SYS_IP_PROTO_INV          0x0000   /*invalid type ,should NOT happen*/
#define  SYS_IP_PROTO_IPV4         0x0001   /*IPv4*/
#define  SYS_IP_PROTO_IPV6         0x0002   /*IPv6*/
#define  SYS_IP_PROTO_ALL          (SYS_IP_PROTO_IPV4|SYS_IP_PROTO_IPV6)

/*
* type of IP Address attribute
*/
#define  SYS_IP_ADDR_INV           0x0000   /*invalid address type ,should NOT happen*/
#define  SYS_IP_ADDR_IP            0x0001   /*master address*/
#define  SYS_IP_ADDR_MASK          0x0002   /*master address*/
#define  SYS_IP_ADDR_GATEWAY       0x0004   /*master address*/
#define  SYS_IP_ADDR_DNS           0x0008   /*master address*/

/*@@DNSLayer@@
* type of DNS index
*/
#define  SYS_DNS_INDEX_INV         0x0000   /*invalid or none*/
#define  SYS_DNS_INDEX_PRI         0x0001   /*primary DNS*/
#define  SYS_DNS_INDEX_SEC         0x0002   /*secondary DNS*/
#define  SYS_DNS_INDEX_THI         0x0003   /*third DNS*/

/*@@ApplicationLayer@@
* type of Application type
*/
#define  SYS_NET_TYPE_INV          -1       /*invalid or none*/
#define  SYS_NET_TYPE_VPN          0        /*Application is VPN*/
#define  SYS_NET_TYPE_SNMP         1        /*Application is snmp*/

/*
* type of status for Application
*/
#define  SYS_APP_STATUS_INV        -1       /*invalid status ,should NOT happen*/
#define  SYS_APP_STATUS_OFF        0        /*Application is off*/
#define  SYS_APP_STATUS_NEG        1        /*Application is on negotiation or authentication*/
#define  SYS_APP_STATUS_ON         2        /*Application is on*/
#define  SYS_APP_STATUS_ERROR      8        /*Application is under error*/

/*
* type of system environment
*/
#define SYS_ENV_INV                -1       /*invalid, should NOT happen*/
#define SYS_ENV_RTP_QOS            0        /* rtp qos */
#define SYS_ENV_VLAN_PRI           1        /* vlan priority */
#define SYS_ENV_VLAN_ID            2        /* vlan id */


/* type of rom information */
#define SYS_ROM_INFO_INV           -1       /* invalid */
#define SYS_ROM_INFO_DEVICE_NAME    0       /* get DEVICENAME  */
#define SYS_ROM_INFO_PACKED_TIME    1       /* get packed time */

/************************************************************************/
/* basic message define                                                 */
/************************************************************************/
/***********************************************************************/
/* notification define id from 0x20000 to 0x2003f (sent by rtServer)   */
/***********************************************************************/
/*
* usage   : to notify that Physical port status has changed!
* id      : 0
* target  : sys server
* wparam  : Physical port type, reference to enum type int
* lparam  : Physical port statys, reference to enum type int
* data_ptr: none
* data_len: none
*/
#define SYS_MSG_NOTIFY_PHY_STATUS           MKSRV_MSG(MKSRV_ID_SYSTEM, 0)

/*
* usage   : to notify that IPv4/IPv6 net status has changed!
* id      : 1
* target  : sys server
* wparam  : IP version(IPv4/IPv6) type, reference to enum type int
* lparam  : Network status, reference to enum type int
* data_ptr: if lparam = SYS_NET_STATUS_EXIST/SYS_NET_STATUS_CONFLICT
*           is mac(format is xx:xx:xx:xx:xx:xx), otherwise is none
* data_len: if lparam = SYS_NET_STATUS_EXIST/SYS_NET_STATUS_CONFLICT
*           is data_ptr's length, otherwise is none
*/
#define SYS_MSG_NOTIFY_NETWORK_STATUS       MKSRV_MSG(MKSRV_ID_SYSTEM, 1)

/*
* usage   : to notify that IPv4/IPv6 address has changed!
* id      : 2
* target  : sys server
* wparam  : IP version(IPv4/IPv6) type, reference to enum type int
* lparam  : IP Address attribute, reference to enum int,
*           eg: (SYS_IP_ADDR_IP | SYS_IP_ADDR_GATEWAY)
* data_ptr: none
* data_len: none
*/
#define SYS_MSG_NOTIFY_IP_ADDRESS_CHANGED   MKSRV_MSG(MKSRV_ID_SYSTEM, 2)

/*
* usage   : to notify that NTP sync time or time has changed by daylight saving time!
* id      : 3
* target  : sys server
* wparam  : time zero value
* lparam  : daylight saving time value
* data_ptr: none
* data_len: none
*/
#define SYS_MSG_NOTIFY_TIME_CHANGED         MKSRV_MSG(MKSRV_ID_SYSTEM, 3)


/*
* usage   : to notify that system environment changed.
* id      : 6
* target  : sys server
* wparam  : Such as SYS_ENV_RTP_QOS and etc. reference to sys env type define.
* lparam  : 0
* data_ptr: none
* data_len: none
*/
#define SYS_MSG_NOTIFY_SYSENV_CHANGED       MKSRV_MSG(MKSRV_ID_SYSTEM, 6)

/*
* usage   : to notify the status of certificate request
* id      : 7
* target  : sys server
* wparam  : 0 - completed
* lparam  : 0
* data_ptr: none
* data_len: none
*/
#define SYS_MSG_NOTIFY_CERT_REQUEST        MKSRV_MSG(MKSRV_ID_SYSTEM, 7)

#define SYS_MSG_NET_WOULD_CHANGED           MKSRV_MSG(MKSRV_ID_SYSTEM, 8)

#define SYS_MSG_NET_WARNNING_MSG           MKSRV_MSG(MKSRV_ID_SYSTEM, 9)

/*
* usage   : to notify power state changed
* id      : 8
* target  : sys server
* wparam  : power type, see sys_power_type_e
* lparam  : changed info, see sys_power_info_type_e
* data_ptr: new value
* data_len: new value len
*/
#define SYS_MSG_POWER_CHANGED               MKSRV_MSG(MKSRV_ID_SYSTEM, 10)




/*************************************************************************/
/* notification define id from 0x20040 to 0x2007f (for WiFi) */
/*************************************************************************/
/*
*-----------------------------------------------------------
*  part 1: UI to rtServer id from 0x20040 to 0x2005f
*-----------------------------------------------------------
*/


/*
*-----------------------------------------------------------
*  part 2: rtServer to UI: id from 0x20060 to 0x2007f
*-----------------------------------------------------------
*/
/*
* usage   : SCAN result got
* id      : 4
* target  : sys server
* wparam  : 0
* lparam  : 0/1 success/failure
* data_ptr: none
* data_len: none
*/
#define SYS_MSG_WIFI_SCAN_RESULT            MKSRV_MSG(MKSRV_ID_SYSTEM, 0x60)
/* connect status change wParm: current ap id, lParam: 0/1/2/3/4/5/6 connected/disconnected/connecting/auth_failure/connect_fail/interface_disabled/interface_eanbled */
#define SYS_MSG_WIFI_CONNECT_STATUS         MKSRV_MSG(MKSRV_ID_SYSTEM, 0x61)

/*
* usage   : WPS result got, wParam: 0, lParam: 0/1 success/failure or Timeout
* id      : 5
* target  : sys server
* wparam  : 0
* lparam  : 0/1 success/failure or Timeout
* data_ptr: none
* data_len: none
*/
#define SYS_MSG_WIFI_WPS_RESULT             MKSRV_MSG(MKSRV_ID_SYSTEM, 0x62)
/* catch user push wps pbc,Param: 0, lParam: 0  */
#define SYS_MSG_WIFI_WPS_PBC_CATCH          MKSRV_MSG(MKSRV_ID_SYSTEM, 0x63)


/*************************************************************************/
/* notification define id from 0x20100 to 0x201ff (sent by other module) */
/*************************************************************************/

/*
* usage   : send for update rom
* id      : 256
* target  :
* wparam  :
* lparam  :
* data_ptr:
* data_len:
*/
#define SYS_MSG_UPDATE_FIRWARE             MKSRV_MSG(MKSRV_ID_SYSTEM, 256)

/*
* usage   : send for device reboot
* id      : 257
* target  :
* wparam  :
* lparam  :
* data_ptr:
* data_len:
*/
#define SYS_MSG_DEVICE_REBOOT             MKSRV_MSG(MKSRV_ID_SYSTEM, 257)

/*
* usage   : send for update config
* id      : 258
* target  :
* wparam  :
* lparam  :
* data_ptr:
* data_len:
*/
#define SYS_MSG_UPDATE_CONFIG             MKSRV_MSG(MKSRV_ID_SYSTEM, 258)

/*
* usage   :  send for reset factory
* id      :  259
* target  :
* wparam  :
* lparam  :
* data_ptr:
* data_len:
*/
#define SYS_MSG_RESET_FACTORY             MKSRV_MSG(MKSRV_ID_SYSTEM, 259)

/*
* usage   :  send for reset factory
* id      :  260
* target  :
* wparam  :  1:connected, 0: connect fail
* lparam  :
* data_ptr:
* data_len:
*/
#define SYS_MSG_OPENVPN_STATUS             MKSRV_MSG(MKSRV_ID_SYSTEM, 260)

/************************************************************************/
/* common define                                                        */
/************************************************************************/

/*
* type of IP address
*/
typedef struct sys_IP
{
    int    flags;                       /*flags of control*/
#define  SYS_ADDR_FL_NONE      000000000    /*should be invalid*/

    int    af_type;                     /*AF_INET or AF_INET6*/
    int    prefix;                      /*ipv6 prefix len*/
    union
    {
        struct sockaddr_in  a4;
        struct sockaddr_in6 a6;
    } addr;
} sys_IP_t;


/*
* type of time
*/
typedef struct
{
    short  st_size;                     /*the structure size*/
    int    flags;                       /*flags of control*/
#define  SYS_TIME_FL_NONE      000000000    /*should be invalid*/

    int    ntp_enable;                  /*use NTP*/
    int    trusted;                     /*have sync times from NTP*/
    int    interval;                    /*NTP update interval*/
    int    dst_offset;                  /*offset for daylight saving time (second)*/
    int    tz_offset;                   /*offset for time zone (second)*/
    struct tm sys_tm;                   /*system time*/
} sys_Time_t;

/* vlan info */
typedef struct sys_vlan_info_s
{
    int vlan_enable;    /* vlan enable or disable */
    int vlan_id;        /* vlan id */
    int vlan_pri;       /* vlan priority */
    int vlan_cfi;       /* vlan cfi */
    int vlan_dscp;      /* vlan dscp */
} sys_vlan_info_t;


/************************************************************************/
/* defination for WiFI                                                  */
/************************************************************************/
typedef enum
{
    SYS_NL_CABLE = 0,
    SYS_NL_WIFI
} sys_net_link_type_t;

typedef enum
{
    SYS_NL_STATUS_OPEN = 0,
    SYS_NL_STATUS_CLOSE
} sys_net_link_status_t;

#define WIFI_AP_LIST_MAX                    32
#define WIFI_NAME_MAX                       33
#define WIFI_PASSWD_MAX                     65/* 64+1 */
#define WIFI_WEP_KEY_MAX                    17/* 16+1 */

typedef enum
{
    SYS_WIFI_MODE_UNKNOWN = -1,
    SYS_WIFI_MODE_OPEN,
    SYS_WIFI_MODE_WEP,
    SYS_WIFI_MODE_WPA_PSK,
    SYS_WIFI_MODE_WPA2_PSK,
    SYS_WIFI_MODE_WPA_EAP,
    SYS_WIFI_MODE_WPA2_EAP
} sys_wifi_mode_t;

typedef enum sys_wifi_pairwise
{
    SYS_WIFI_CIPHER_UNKNOWN = -1,
    SYS_WIFI_CIPHER_TKIP,
    SYS_WIFI_CIPHER_CCMP,
    SYS_WIFI_CIPHER_CCMP_TKIP
} sys_wifi_pairwise_t;

typedef struct sys_wifi_network
{
    sys_wifi_mode_t mode;
    sys_wifi_pairwise_t pairwise;
    char psk[WIFI_PASSWD_MAX];
    char identity[WIFI_NAME_MAX];
} sys_wifi_network_t;

typedef struct sys_wifi_nw_auth
{
    char identity[WIFI_NAME_MAX];                       /* identity */ /* ap为EAP的，是用用户名和密码 */
    char passwd[WIFI_PASSWD_MAX];                       /* password */
    char psk[WIFI_PASSWD_MAX];                       /* psk */    /* 搜索到的ap，只是用psk，其他如identity&password通过配置配置 */
    char wep_key_idx;                               /* key idx, 1 - 4 */
    char wep_key[WIFI_WEP_KEY_MAX];                  /* wep key */
    char wps_pin[9];                                /* wps pin code, out to UI. */
} sys_wifi_nw_auth_t;


typedef struct sys_wifi_nw_stat
{
    signed char  nw_id;
    char name[WIFI_NAME_MAX];                           /* ssid name */
    sys_wifi_nw_auth_t nw_auth;

    int link_speed;                                /* network connected link speed, 不止255 */

#define WIFI_CIPHER_NONE                    0x00000001
#define WIFI_CIPHER_WEP40                   0x00000002
#define WIFI_CIPHER_WEP104                  0x00000004
#define WIFI_CIPHER_TKIP                    0x00000008
#define WIFI_CIPHER_CCMP                    0x00000010
#define WIFI_CIPHER_AES_128_CMAC            0x00000020
#define WIFI_CIPHER_GCMP                    0x00000040
#define WIFI_CIPHER_SMS4                    0x00000080
    int pairwise_cipher;/* accepted pairwise (unicast) ciphers for WPA */
    int group_cipher;/* group (broadcast/multicast) ciphers for WPA */

#define WIFI_KEY_MGMT_IEEE8021X             0x00000001
#define WIFI_KEY_MGMT_PSK                   0x00000002
#define WIFI_KEY_MGMT_NONE                  0x00000004
#define WIFI_KEY_MGMT_IEEE8021X_NO_WPA      0x00000008
#define WIFI_KEY_MGMT_WPA_NONE              0x00000010
#define WIFI_KEY_MGMT_FT_IEEE8021X          0x00000020
#define WIFI_KEY_MGMT_FT_PSK                0x00000040
#define WIFI_KEY_MGMT_IEEE8021X_SHA256      0x00000080
#define WIFI_KEY_MGMT_PSK_SHA256            0x00000100
#define WIFI_KEY_MGMT_WPS                   0x00000200
#define WIFI_KEY_MGMT_SAE                   0x00000400
#define WIFI_KEY_MGMT_FT_SAE                0x00000800
#define WIFI_KEY_MGMT_WAPI_PSK              0x00001000
#define WIFI_KEY_MGMT_WAPI_CERT             0x00002000
#define WIFI_KEY_MGMT_CCKM                  0x00004000
    int key_mgmt;/* authenticated key management protocols */

#define WIFI_WPA_DISCONNECTED               0x00
#define WIFI_WPA_INTERFACE_DISABLED         0x01
#define WIFI_WPA_INACTIVE                   0x02
#define WIFI_WPA_SCANNING                   0x03
#define WIFI_WPA_AUTHENTICATING             0x04
#define WIFI_WPA_ASSOCIATING                0x05
#define WIFI_WPA_ASSOCIATED                 0x06
#define WIFI_WPA_4WAY_HANDSHAKE             0x07
#define WIFI_WPA_GROUP_HANDSHAKE            0x08
#define WIFI_WPA_COMPLETED                  0x09
    signed char wpa_state;/* WPA state */

} sys_wifi_nw_stat_t;

typedef struct sys_wifi_ap
{
    signed char ap_id;
#define WIFI_AP_ACTIVE                      0x01
#define WIFI_AP_INACTIVE                    0x02
    char ap_status;
    signed char nw_id;                              /* network id */


    /* capability information field in host byte order */
#define WIFI_CAP_ESS                        0x00000001
#define WIFI_CAP_IBSS                       0x00000002
#define WIFI_CAP_CF_POLLABLE                0x00000004
#define WIFI_CAP_CF_POLL_REQUEST            0x00000008
#define WIFI_CAP_PRIVACY                    0x00000010
#define WIFI_CAP_SHORT_PREAMBLE             0x00000020
#define WIFI_CAP_PBCC                       0x00000040
#define WIFI_CAP_CHANNEL_AGILITY            0x00000080
#define WIFI_CAP_SPECTRUM_MGMT              0x00000100
#define WIFI_CAP_SHORT_SLOT_TIME            0x00000400
#define WIFI_CAP_DSSS_OFDM                  0x00002000
    int caps;

    /* authorized definitions */
#define WIFI_AUTH_WPA                       0x00000001
#define WIFI_AUTH_WPA2                      0x00000002
#define WIFI_AUTH_WPS_PBC                   0x00000004
#define WIFI_AUTH_WPS_AUTH                  0x00000008
#define WIFI_AUTH_WPS_PIN                   0x00000010
#define WIFI_AUTH_WPS                       0x00000020
#define WIFI_AUTH_NONE                      0x00000040
#define WIFI_AUTH_EAP                       0x00000080
#define WIFI_AUTH_PSK                       0x00000100
#define WIFI_AUTH_PREAUTH                   0x00000200
#define WIFI_AUTH_WEP40                     0x00000400
#define WIFI_AUTH_WEP104                    0x00000800
#define WIFI_AUTH_TKIP                      0x00001000
#define WIFI_AUTH_CCMP                      0x00002000
#define WIFI_AUTH_GCMP                      0x00004000
    int  auth;                                          /* authoriztion type(mask) */

    int  freq;                                          /* frequence value */
    int  signal;                                       /* signal level: 0-100 */
    char name[WIFI_NAME_MAX];                           /* ssid name */
    unsigned char ap_mac[6];                                     /* ap mac address */
} sys_wifi_ap_t;

/* definitions for upload pcap file (Abandoned) */
#define     SYS_UPP_MAX_URL_LEN     512
#define     SYS_UPP_MAX_USER_LEN    256
#define     SYS_UPP_MAX_PWD_LEN     160

/* results for start/stop upload pcap file (Abandoned)*/
typedef enum
{
    SYS_UPP_RET_UNKNOWN = 0,
    SYS_UPP_START_SUCC,             /* start upload pcap success */
    SYS_UPP_STOP_SUCC,              /* stop upload pcap success */
    SYS_UPP_PCAP_SERVICE_BUSY,      /* pcap service is busy */
    SYS_UPP_INVALID_URL,            /* url is invalid */
    SYS_UPP_START_PCAP_FAIL,        /* start pcap captrue fail */
} sys_upp_ret_e;
/* info for start/stop upload pcap file (Abandoned) */
typedef struct sys_upp_info
{
    char url[SYS_UPP_MAX_URL_LEN];
    char username[SYS_UPP_MAX_USER_LEN];
    char password[SYS_UPP_MAX_PWD_LEN];
    int  limit_size;
} sys_upp_info_t;

/* results for troubleshooting */
typedef enum
{
    SYS_TS_RET_UNKNOWN = 0,
    SYS_TS_START_SUCC,             /* start troubleshooting success */
    SYS_TS_STOP_SUCC,              /* stop troubleshooting success */
    SYS_TS_PCAP_SERVICE_BUSY,      /* pcap service is busy */
    SYS_TS_INVALID_URL,            /* url is invalid */
    SYS_TS_PCAP_START_FAIL,        /* start pcap captrue fail */
    SYS_TS_GET_CONFIG_FAIL,        /* get config file fail */
    SYS_TS_ALREADY_IN_PROC,        /* already in process */
} sys_ts_ret_e;

typedef enum
{
    SYS_PERMISSION_AMR,
} sys_permission_type_e;

typedef enum
{
    SYS_PERMISSION_INFO_PERMIT,                    /*result int*/
    SYS_PERMISSION_INFO_PERMIT_type,               /*result int, PERMANENT=0, TRIAL=1*/
    SYS_PERMISSION_INFO_PERMIT_START_TIME,         /*result time_t*/
    SYS_PERMISSION_INFO_PERMIT_END_TIME,           /*result time_t*/
} sys_permission_info_type_e;

/*log module*/
#define  SYS_LOG_ANY              0 /*fix setting*/
#define  SYS_LOG_MEM              1
#define  SYS_LOG_SRV              2 /*for xxx0*/
#define  SYS_LOG_SYS              3 /*for xxx1*/
#define  SYS_LOG_LLDP             4 /*for lldp*/
#define  SYS_LOG_CDP              5 /*for cdp */
#define  SYS_LOG_POFF             6 /*for logoff */
#define  SYS_LOG_LSYS             7 /*for libsysservice */

/*
* libsys log level
*/
#define  SYS_LOG_INV          -1
#define  SYS_LOG_ALL           0
#define  SYS_LOG_LEVEL_EMERG   0        /* Emergency: system is unusable */
#define  SYS_LOG_LEVEL_ALERT   1        /* Alert: action must be taken immediately */
#define  SYS_LOG_LEVEL_CRIT    2        /* Critical: critical conditions */
#define  SYS_LOG_LEVEL_ERR     3        /* Error: error conditions */
#define  SYS_LOG_LEVEL_WARN    4        /* Warning: warning conditions */
#define  SYS_LOG_LEVEL_NOTICE  5        /* Notice: normal but significant condition */
#define  SYS_LOG_LEVEL_INFO    6        /* Informational: informational messages */
#define  SYS_LOG_LEVEL_DEBUG   7        /* Debug: debug-level messages */
#define  SYS_LOG_LEVEL_SPY     8        /* reserved */
#define  SYS_LOG_LEVEL_ANY     9        /* reserved */

/*
* libsys log output type
*/
#define  SYS_LOG_TYPE_NONE     0
#define  SYS_LOG_TYPE_SYS      1        /* output to syslogd */
#define  SYS_LOG_TYPE_STD      2        /* output to stdout */
#define  SYS_LOG_TYPE_FILE     3        /* output to file */
#define  SYS_LOG_TYPE_MAX      4

/*
* libsys log format mode
*/
#define  SYS_LOG_MODE_NONE     0x0000
#define  SYS_LOG_MODE_TIME     0x0001   /* log have precise time */

/************************************************************************/
/* APIs for c                                                           */
/************************************************************************/
/*
*-----------------------------------------------------------
*   debug/init APIs
*-----------------------------------------------------------
*/
/*for version*/
const char * sys_version(void);
const char * sys_build(void);

/**
* @brief  : use for user to init basic data of system service
*
* @param  : master_key, would be anything!!
*           reset,try to do reset of the framework
*
* @warning: for normal user ,do not care the params, and the master_key should be NULL!
*
* @return : 0 as success,otherwise failed.
*
*/
int sys_init(const char * master_key, int reset);

/**
* @brief  : use for user cleanup service data of system service
*
* @param  : none
*
* @warning:
*
* @return : 0 as success,otherwise failed.
*
*/
int sys_exit(void);

/**
* @brief  : use for user to init libsysservice's log
*
* @param  : type, libsys log output type
*           mode, libsys log format mode
*           file, absolute file path for log, valid when type is SYS_LOG_TYPE_FILE
*           module, log module
*           level, libsys log level
* @return : 0 as success,otherwise failed.
*
*/
int sys_init_log(int type, int mode, const char * file);
int sys_set_log_module_level(int module, int level);

/*
*-----------------------------------------------------------
*   Physical APIs
*-----------------------------------------------------------
*/
/**
* @brief  :
*
* @param  :
*
* @warning:
*
* @return : 0 as success,otherwise failed.
*
*/
int sys_enum_phyport(char * name, int name_max, int * index);
int sys_enum_adapter(char * name, int name_max, int * index);

int sys_exist_phyport(int index);
int sys_exist_adapter(int index);

/**
* @brief  : use for get phyPort mac addr
*
* @param  :
*
* @warning:
*
* @return : 0 as success,otherwise failed.
*
*/
int sys_get_MAC(int phyPort, unsigned char * mac, int len);
int sys_set_MAC(int phyPort, unsigned char * mac, int len);

int sys_get_MAC_string(int phyPort, char * buff, int buff_max);
int sys_set_MAC_string(int phyPort, const char * buff);

/**
* @brief  : use for get port link state
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_port_link(int phyportID);
int sys_set_port_link(int phyportID, int option);

/**
* @brief  : use for get active phy port id
*
* @param  :
*
* @warning:
*
* @return : active phy port id, SYS_PHY_TYPE_INV/SYS_PHY_TYPE_WAN/SYS_PHY_TYPE_WIFI.
*
*/
int sys_get_active_port();
int sys_set_active_port();

/**
* @brief  : use for get phone model name
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_model_name(char * buf, int len);

/**
* @brief  : use for get support speed
*
* @param  :
*
* @warning:
*
* @return : support speed, see speed define.
*
*/
int sys_get_support_speed(int phyportID);

int sys_get_port_speed(int phyportID);

/*
*-----------------------------------------------------------
*   Linklayer APIs
*-----------------------------------------------------------
*/
/**
* @brief  :  use for get linklayer state
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_linklayer_status(int type);
int sys_set_linklayer_status(int type, int status);

/**
* @brief  :  use for get vlan info
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_vlan_info(int phyportID, sys_vlan_info_t * info);

/**
* @brief  :  use for get lldp info
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_lldp_chassis_id(char * buf, int len);
int sys_get_lldp_port_id(char * buf, int len);

/**
* @brief  :  use for pc interface
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_bridge_enable();
int sys_get_span_to_pc_enable();
/*
*-----------------------------------------------------------
*   Network APIs
*-----------------------------------------------------------
*/
/**
* @brief  :  use for get ip version
*
* @param  :
*
* @warning:
*
* @return : ip version, reference to enum type int
*
*/
int sys_get_network_proto(int adapterID);
int sys_set_network_proto(int adapterID, int ip_proto);

/**
* @brief  : get status of network
*
* @param  :
*
* @warning:
*
* @return : Network status, reference to enum type int
*
*/
int sys_get_network_status(int adapterID, int ip_proto);
int sys_set_network_status(int adapterID, int ip_proto, int status);

/**
* @brief  :  get network type DHCP or static
*
* @param  :
*
* @warning:
*
* @return : Network type, reference to enum type int
*
*/
int sys_get_network_type(int adapterID, int ip_proto);
int sys_set_network_type(int adapterID, int ip_proto, int type);

/**
* @brief  :
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_default_port(void);
int sys_set_default_port(int phyportID);

int sys_get_default_route(sys_IP_t * ip);
int sys_set_default_route(int adapterID, const sys_IP_t * ip);

/**
* @brief  : get IP addr
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_IP(int adapterID, int ip_proto, sys_IP_t * ip);
int sys_set_IP(int adapterID, int ip_proto, const sys_IP_t * ip);

/**
* @brief  : get broadcast IP addr
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_IP_bcast(int adapterID, int ip_proto, sys_IP_t * ip);
int sys_set_IP_bcast(int adapterID, int ip_proto, const sys_IP_t * ip);

/**
* @brief  : get IP mast
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_IP_mask(int adapterID, int ip_proto, sys_IP_t * ip);
int sys_set_IP_mask(int adapterID, int ip_proto, const sys_IP_t * ip);

/**
* @brief  : get IP gateway
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_IP_gatway(int adapterID, int ip_proto, sys_IP_t * ip);
int sys_set_IP_gatway(int adapterID, int ip_proto, const sys_IP_t * ip);

/**
* @brief  : get DNS addr
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_DNS(int adapterID, int ip_proto, int dnsID, sys_IP_t * ip);
int sys_set_DNS(int adapterID, int ip_proto, int dnsID, const sys_IP_t * ip);


/*
*-----------------------------------------------------------
*   wifi APIs
*-----------------------------------------------------------
*/

#define SYS_WIFI_GLOBAL_ON          0
#define SYS_WIFI_GLOBAL_OFF         1
int sys_wifi_set_global_func(mkit_handle_t caller, int op, int timeout);

#define SYS_WIFI_OP_SCAN_START      0
#define SYS_WIFI_OP_SCAN_STOP       1
int sys_wifi_scan_action(mkit_handle_t caller, int op, int timeout);

#define SYS_WIFI_PERIOD_SCAN_START  1
#define SYS_WIFI_PERIOD_SCAN_STOP   0
int sys_wifi_period_scan(mkit_handle_t caller, int op, int period, int timeout);

#define SYS_WIFI_OP_CONNECT_START   0
#define SYS_WIFI_OP_CONNECT_STOP    1
int sys_wifi_connect_action(mkit_handle_t caller, int op, int ap_id, sys_wifi_nw_auth_t * auth,
                            int timeout);

#define SYS_WIFI_OP_WPS_GET_PIN     0
#define SYS_WIFI_OP_WPS_START       1
#define SYS_WIFI_OP_WPS_STOP        2

#define  SYS_WIFI_WPS_TYPE_PBC      0
#define  SYS_WIFI_WPS_TYPE_PIN      1
#define  SYS_WIFI_WPS_TYPE_EXT      2
int sys_wifi_wps_action(mkit_handle_t caller, int op, int wps_type, int timeout);

int sys_wifi_sync_status(mkit_handle_t caller, int timeout);

int sys_wifi_config_changed(mkit_handle_t caller, int timeout);

int sys_wifi_get_ap_id(mkit_handle_t caller, int profile_id, char * ssid, int len, int timeout);

int sys_wifi_get_port_staus();

/**
* @brief  :  get wifi network status
*
* @param  :  stat - buffer for result,  buf_len - size of buffer
*
* @warning:
*
* @return : 0 is success, -1 failed.
*
*/
int sys_wifi_get_network_status(sys_wifi_nw_stat_t * stat, int buf_len);

/**
* @brief  :  get wifi ap list
*
* @param  :  ap_list - buffer for all ap,  buf_len - size of buffer
*
* @warning:
*
* @return : 0 is success, -1 failed.
*
*/
int sys_wifi_get_ap_list(sys_wifi_ap_t * ap_list, int buf_len);

/**
* @brief  :  set wifi country code
*
* @param  :  country : code of country,such as BR/CN/SG..., is C string type,if null,rtServer will get config by itself
*
* @warning:
*
* @return : 0 is success, -1 failed.
*
*/
int sys_wifi_set_country(mkit_handle_t caller, const char * country, int len, int timeout);
/*
*-----------------------------------------------------------
*   APP APIs
*-----------------------------------------------------------
*/
/**
* @brief  :  use for pcap
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_set_pcap_filter(mkit_handle_t caller, const char * filter, int len, int timeout);
int sys_pcap_start(mkit_handle_t caller, int timeout);
int sys_pcap_stop(mkit_handle_t caller, int timeout);
int sys_pcap_status(mkit_handle_t caller, int * state, int timeout);
int sys_pcap_export(mkit_handle_t caller, void * buf, int valid_len, int timeout);

/**
* @brief  :  start pcap and write to specialed path
*
* @param  :  path       the dir to write
*            clip_size  per file size, B
*            count      clip count, -1 as no limit
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_pcap_start_path_write(mkit_handle_t caller, const char * path, int clip_size, int count,
                              int timeout);
int sys_pcap_stop_path_write(mkit_handle_t caller, int timeout);
int sys_pcap_status_path_write(mkit_handle_t caller, int * state, int timeout);

/**
* @brief  :  use for upload pcap file
*
* @param  :  caller     user msgkit handle
*            upp_info   config info for upload
*            result     process result
*            state     upload state
*            timeout    wait time
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_upload_pcap_start(mkit_handle_t caller, sys_upp_info_t * upp_info, int timeout);
int sys_upload_pcap_stop(mkit_handle_t caller, int timeout);
int sys_upload_pcap_status(mkit_handle_t caller, int * state, int timeout);
int sys_upload_pcap_trigger(mkit_handle_t caller, sys_upp_info_t * upp_info, sys_upp_ret_e * result,
                            int timeout);


#define SYS_TS_UP_PCAP    (1<<0)
#define SYS_TS_UP_LOG     (1<<1)
#define SYS_TS_UP_CONFIG  (1<<2)
/**
* @brief  :  use for troubleshooting
*
* @param  :  caller     user msgkit handle
*            ts_info    info for troubleshooting(not used)
*            result     process result
*            state      troubleshooting state
*            timeout    wait time
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_troubleshooting_start(mkit_handle_t caller, void * ts_info, int timeout);
int sys_troubleshooting_stop(mkit_handle_t caller, int timeout);
int sys_troubleshooting_status(mkit_handle_t caller, int * state, int timeout);
int sys_troubleshooting_trigger(mkit_handle_t caller, void * ts_info, sys_ts_ret_e * result,
                                int timeout);
/**
* @brief  :  get app status
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_app_status(int type);
int sys_set_app_status(int type, int status);

/*
*-----------------------------------------------------------
*   System APIs
*-----------------------------------------------------------
*/

/**
* @brief  :  get hardware / software version
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_hw_version(char * ver, int ver_max);
int sys_get_sw_version(char * ver, int ver_max);

/**
* @brief  :  get system time parameter
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_systime(sys_Time_t * time);
int sys_set_systime(const sys_Time_t * time);
int sys_set_manual_systime(mkit_handle_t caller, const sys_Time_t * time, int timeout);

int sys_update_systime(mkit_handle_t caller, int timeout);

/**
* @brief  : get env parameter
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_env(const char * key, char * value, int len);
int sys_set_env(const char * key, const char * value);

/**
* @brief  : get rom information
*
* @param  :
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int sys_get_rom_info(int type, char * buff, int len);

/**
* @brief  : replace wildcards
*
* @param  :
* str:  original string contained wildcards
* flag: can contain multiple flag ( for eg, SYS_WILDCARD_MAC_UPPER | SYS_WILDCARD_RESERVED)
* buf:  buffer for result string, can not be the 'str'!!
* len:  buffer length
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/

/* flag mask defination */
#define SYS_WILDCARD_UNSPC        0x0000       /* not specify flag */
#define SYS_WILDCARD_MAC_UPPER    0x0001       /* if this bit is set, mac will replace with upper case */
#define SYS_WILDCARD_RESERVED     0x0002       /* for extendion if the future... */

int sys_replace_wildcards(char * str, int flag, char * buf, int len);

/* call this function to report openvpn status, used by openvpn */
int sys_openvpn_status(mkit_handle_t caller, unsigned long wparam, unsigned long lparam,
                       const void * data, int data_len);

/*
*-----------------------------------------------------------
*    request APIs
*-----------------------------------------------------------
*/
/* call this function to request update firmware */
int sys_update_firmware(mkit_handle_t caller, unsigned long wparam, unsigned long lparam,
                        const void * data, int data_len);

/* call this function to request reboot device */
int sys_device_reboot(mkit_handle_t caller, unsigned long wparam, unsigned long lparam,
                      const void * data, int data_len);

/* call this function to request update config */
int sys_update_config(mkit_handle_t caller, unsigned long wparam, unsigned long lparam,
                      const void * data, int data_len);

/* call this function to request reset factory */
int sys_reset_factory(mkit_handle_t caller, unsigned long wparam, unsigned long lparam,
                      const void * data, int data_len);

/*
 * @brief  :  reset syslogd
 *
 * @param timeout: wait timeout
 * @param wparam: 0 - temporary; 1 - permanent 2 - reload from config
 * @param wparam: 20 - start usb log, 21 - stop usb log
 * @param lparam: 0 - local; 1 - remote; if wparam==20 then lparam - size of per log size (B)
 * @param buf:   when type is remote, this parameter is server address
 *               when wparam is 20, this parameter is the directory path of the log
 * @param state: capture state: 1 capturing, 0 not capturing
 * @warning:
 *
 * @return : 0 is success,otherwise failed.
 *
 */
int sys_net_config_syslogd_changed(mkit_handle_t caller, int wparam, int lparam, void * buf,
                                   int len, int timeout);

int sys_net_config_path_log_status(mkit_handle_t caller, int * state, int timeout);

/*
* @brief : set the system time
*
* @param buf: the string of the time
* @param valid_len: the length of the string
* @param timeout: timeout of the request(ms)
*
* @return : 0 as success, otherwise failed.
*/
int sys_sip_set_time(mkit_handle_t caller, void * buf, int valid_len, int timeout);

/*
* @brief : get lldp location
*
* @param buf: the string of lldp location
* @param len: the length of the string
*
* @warning: output string is formatted: Location Data Format(1 octet) + Location ID(Variable size)
*
* @return : 0 as success, otherwise failed.
*/
int sys_get_lldp_location(char * buf, int len);

/*
* @brief : adjust user time to local time (adjust by time zone and summer dst)
*
* @param x: input time pointer, the output will also storage in this space
*
* @return : 0 as success, otherwise failed.
*/
int sys_time_adust_time_offset(time_t * x);


/*
* @brief : get power permission info
*
* @param permission_type: see sys_permission_type_e
* @param info_type: see sys_power_info_type_e
* @param result: to store result, the result see sys_power_info_type_e
*
* @return : 0 as success, otherwise failed.
*/
int sys_get_permission_info(mkit_handle_t caller, int permission_type, int info_type,
                            void * result);

/*
* @brief : call sysserver to update permission
*
* @param permission_type: see sys_permission_type_e, if permission_type = -1, means all
*
* @return : 0 as success, otherwise failed.
*/
int sys_update_permission(mkit_handle_t caller, int permission_type);

#ifdef WIN32
int sys_set_model_name(const char * model_name);
int sys_set_sw_version(const char * version);
int sys_set_hw_version(const char * hwversion);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SRV_SYSTEM_H__ */

