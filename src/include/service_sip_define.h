/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2007-2010 Xiamen  Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    DATE:
*
*    HISTORY:
*------------------------------------------------------------------
*    修改时间:              修改者  :
*    原版本号:              新版本号:
*    修改问题:
*
*------------------------------------------------------------------
*
*******************************************************************/
#ifndef __SERVICE_SIP_DEFINE_H__
#define __SERVICE_SIP_DEFINE_H__

#ifdef WIN32
    #define _WINSOCKAPI_
    #include <windows.h>
    #undef _WINSOCKAPI_
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define inline  __inline
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

#include <msgkit/mkservice.h>
#include <msgkit/mkmessage.h>
/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*
 *------------------------------------
 *      limit define
 *------------------------------------
 */
#define SIP_MAX_NAME_LEN                    128     /* 名字最多128字符 */
#define SIP_MAX_PATH_LEN                    512     /* 文件路径的最大长度 */
#define SIP_MAX_URI_LEN                     512     /* SIP URI长度，如"sip:xxx@yyy.com:5060" */
#define SIP_MAX_MSGKIT_BODY_LEN             3072    /* SIP->UI, UI->SIP  body msg max len*/
#define SIP_MAX_XML_BODY_LEN                4096    /* xml body的最大长度 */

#define SIP_MAX_SRTP_KEY_LEN                96      /* SRTP KEY长度 */
#define SIP_MAX_CODEC_NAME_LEN              32      /* codec名字长度，如"PCMU" */
#define SIP_MAX_RTCP_FB_LEN                 48      /* rtpc feedback len */

#define SIP_MAX_ADDRESS_LEN                 48      /* IP地址长度 */
#define SIP_MAX_HEADER_NAME_LEN             32      /* SIP信令Header最大长度 */
#define SIP_MAX_HEADER_VALUE_LEN            128     /* SIP信令Header内容最大长度 */

#define SIP_MAX_CONF_LINE                   12      /* 最大支持会议路数 */

#define SIP_MAX_CODEC_LIST                  8       
#define SIP_MAX_MEDIA_LIST                  4       

#define SIP_MAX_UUID_LEN                    32      /* 最大uuid长度 */
#define SIP_MAX_LIFETIME_LEN                32      /* srtp lifetime 参数的最大长度 */


#define SIPUA_MAX_TIME_ZONE_LEN             128     /* time zone 长度 */
#define SIPUA_MAX_ACCESS_TYPE_LEN           32      /* access type 长度 */
#define SIP_MAX_SSRC_CNAME_LEN              256     /*Cname最大长度*/
#define SIP_MAX_RTP_KEY_LEN                 128     /*额外参数中rtp key和rtcp key最大长度*/
/*
 *------------------------------------
 *      blf/bla define
 *------------------------------------
 */

    /* BLF状态值范围：0~99 */
#define SIP_BLF_STATUS_FAIL                -1
#define SIP_BLF_STATUS_UNKOWN               0
#define SIP_BLF_STATUS_IDLE                 1
#define SIP_BLF_STATUS_TALKING              2
#define SIP_BLF_STATUS_RINGING              3
#define SIP_BLF_STATUS_CALLOUT              4
#define SIP_BLF_STATUS_HOLD                 5
#define SIP_BLF_STATUS_GENBEN_UNKOWN        6
#define SIP_BLF_STATUS_DND                  7
#define SIP_BLF_STATUS_UNREGISTER           8

    /* 50~100用于BLF客户定制范围 */
#define SIP_BLF_STATUS_TALKING_ME           50  /* CDC客户定制 */
#define SIP_BLF_STATUS_RINGING_ME           51
#define SIP_BLF_STATUS_CALLING_ME           52
#define SIP_BLF_STATUS_HELD_ME              53
#define SIP_BLF_STATUS_PARKED               54

    /* Broadsoft SCA状态值范围：100~199 */
#define SIP_BLA_STATUS_UNKOWN               100
#define SIP_BLA_STATUS_IDLE                 101
#define SIP_BLA_STATUS_SEIZED               102
#define SIP_BLA_STATUS_PROGRESSING          103
#define SIP_BLA_STATUS_ALERTING             104
#define SIP_BLA_STATUS_ACTIVE               105
#define SIP_BLA_STATUS_HELD                 106
#define SIP_BLA_STATUS_HELD_PRIVATE         107
#define SIP_BLA_STATUS_BRIDGE_ACITVE        108
#define SIP_BLA_STATUS_BRIDGE_HELD          109

    /* BLA Draft状态值范围：200~299 */
#define SIP_BLA_DRAFT_STATUS_UNKOWN         200
#define SIP_BLA_DRAFT_STATUS_IDLE           201
#define SIP_BLA_DRAFT_STATUS_SEIZED         202
#define SIP_BLA_DRAFT_STATUS_OUTGOING       203
#define SIP_BLA_DRAFT_STATUS_RINGING        204
#define SIP_BLA_DRAFT_STATUS_TALKING        205
#define SIP_BLA_DRAFT_STATUS_HELD_PUBLIC    206

/*
 *------------------------------------
 *      call info define
 *------------------------------------
 */
    /* call indentity map type */
#define SIP_CALL_ID_NONE                    0  /* 表示没有任何数据 */
#define SIP_CALL_ID_VIDEO_URL               1  /* string,like "http://1.2.3.4/c.h264",用于下载视频播放 */
#define SIP_CALL_ID_REASON                  2  /* string,like "user busy",用于上报对方发的reason内容 */
#define SIP_CALL_ID_EVENT                   3  /* string,like "Event:mic=x,speaker=y",定制内容，用于上报Tiptel PBX控制事件 */
#define SIP_CALL_ID_SKILL_NAME              4  /* string,like "english",定制内容，用于LCD显示"English" */
#define SIP_CALL_ID_BW_CALL_CENTER          5  /* string,xml body，用于Broadworks call center功能 */
#define SIP_CALL_ID_EMERGENCY_CALL          6  /* string,like "type normal" 表示普通通话, "type emergency" 表示紧急通话 */
#define SIP_CALL_ID_PHONE_TEXT              7  /* string,like "helloworld",定制内容，用于LCD显示对方发的PHONE_DISPLAY_TEXT内容 */
#define SIP_CALL_ID_CALLID                  8  /* string,like "zd13dc@10.2.3.1",用于上报SIP信令使用的Call-ID */
#define SIP_CALL_ID_INFO_TEXT               9  /* string,xml body,用于通话中收到INFO消息更新界面显示内容 */

    /* sip_channel_t->channel_type */
#define SIP_CALL_CHANNEL_TYPE_AUDIO         1
#define SIP_CALL_CHANNEL_TYPE_VIDEO         2
#define SIP_CALL_CHANNEL_TYPE_EXTVIDEO      3
#define SIP_CALL_CHANNEL_TYPE_H224          4
#define SIP_CALL_CHANNEL_TYPE_FAX           5
#define SIP_CALL_CHANNEL_TYPE_FILE          6
#define SIP_CALL_CHANNEL_TYPE_BFCP          7

    /* sip_channels_t.reserved.type, 与 sip_channels_map_type_e 同步 */
#define SIP_CALL_CHANNELS_RESERVED_TYPE_CHN3        0
#define SIP_CALL_CHANNELS_RESERVED_TYPE_CHN4        1
#define SIP_CALL_CHANNELS_RESERVED_TYPE_CHN5        2
#define SIP_CALL_CHANNELS_RESERVED_TYPE_RTCP_KEY    3

    /* sip_channel_t->media_direction */
#define SIP_CALL_SEND_RECV                  0
#define SIP_CALL_SEND_ONLY                  1
#define SIP_CALL_RECV_ONLY                  2
#define SIP_CALL_INACTIVE                   3
#define SIP_CALL_DTMF_SENDONLY              4 /*can not send audio, but can send dtmf*/

    /* sip_channels_t->hold_state */
#define SIP_CALL_HOLD_DEFAULT               0        /* 正常通话状态 */
#define SIP_CALL_HOLD_BY_LOCAL              (1<<0)   /* 本地HOLD对方 */
#define SIP_CALL_HOLD_BY_REMOTE             (1<<1)   /* 对方HOLD本地 */

    /* call out call_mask define */
#define SIP_CALL_MASK_NORMAL                0
#define SIP_CALL_MASK_VIDEO                 (1<<0) /* 0x00000001 标志当前是否是视频呼叫 */
#define SIP_CALL_MASK_INTERCOM              (1<<1) /* 0x00000002 标志当前是否是intercom操作 */
#define SIP_CALL_MASK_HUAWEI_EMERGENCY      (1<<2) /* 0x00000004 标志当前是否是华为emergency call操作 */
#define SIP_CALL_MASK_VOICE_MAIL            (1<<3) /* 0x00000008 标记当前是否为voice mail call */
#define SIP_CALL_MASK_TYPE_NUMBER           (1<<4) /* 0x00000010 number call */
#define SIP_CALL_MASK_E911_CALL             (1<<5) /* 0x00000020 E.911 call (verizon复用)*/
#define SIP_CALL_MASK_LYNC_CALLPARK         (1<<6)
#define SIP_CALL_MASK_LYNC_CONF_MCU         (1<<7)
#define SIP_CALL_MASK_LOOPBACK_CALL         (1<<8) /* 0x00000100 Loopback call*/
#define SIP_CALL_MASK_BROADSOFT_SILENT_BARGIN (1<<9) /* 0x00000200 brosoft silent bargin */
#define SIP_CALL_MASK_EMERGENCY             (1<<10) /* 0x00000400 emergency call */
#define SIP_CALL_MASK_UC_INFO               (1<<11) /* 0x00000800 携程UC联动 */
#define SIP_CALL_MASK_TRANS_CODING          (1<<12) /* 0x00001000 BugID=34949 */
#define SIP_CALL_MASK_APOLLO_CONF_MCU       (1<<13) /*0x00002000 标志当前是否是invite apollo mcu操作*/
#define SIP_CALL_MASK_VERIZON_BARGIN        (1<<14) /* 0x00004000 verizon bargin */

    /* call establish call_flag */
#define SIP_CALL_FLAG_CAN_HOLD              (1<<0)
#define SIP_CALL_FLAG_LYNC_CONF             (1<<1)      /* Lync会议通话 */
#define SIP_CALL_FLAG_TYPE_BTOE             (1<<2)      /* Lync BTE 音频通话 */
#define SIP_CALL_FLAG_HAVE_FECC             (1<<3)      /* 当前通话有fecc */
#define SIP_CALL_FLAG_LYNC_PRIVATE_LINE     (1<<4)
#define SIP_CALL_TYPE_LYNC_CPS              (1<<5)
#define SIP_CALL_TYPE_LYNC_VM               (1<<6)      /*Lync通话进入语音信箱的类型*/
#define SIP_CALL_TYPE_LYNC_CONF_ATTENDANT   (1<<7)      /*呼叫Lync会议id，进入会议*/
#define SIP_CALL_FLAG_SERVER_CONF           (1<<8)      /* 服务器会议标志位 */
#define SIP_CALL_FLAG_ENCRYPT               (1<<9)      /* 加密通话，客户特制，非针对中性 */
#define SIP_CALL_FLAG_ICE                   (1<<10)  	/* ICE 通话 */
#define SIP_CALL_TYPE_LYNC_RESPONSE_GROUP   (1<<11) 	/*Lync 呼叫响应组的通话类型*/
#define SIP_CALL_FLAG_HAVE_BFCP             (1<<12) 	/*存在BFCP协商流程*/
#define SIP_CALL_FLAG_APOLLO_CONF           (1<<13)     /*Apollo 会议通话标志位*/
#define SIP_CALL_FLAG_LOOPBACK              (1<<14)     /* 来电是Loopback通话 */

    /* map method */
#define SIP_MAP_METHOD_IDENTIFY                 0
#define SIP_MAP_METHOD_CHANNELS                 1
#define SIP_MAP_METHOD_LYNC_PROVISION           2
#define SIP_MAP_METHOD_LYNC_CONTACT_CARD        3
#define SIP_MAP_METHOD_LYNC_BOSS_PROVISION      4
#define SIP_MAP_METHOD_LYNC_CONF_INFO           5
#define SIP_MAP_METHOD_APOLLO_CONF_USER_MSG     6
#define SIP_MAP_METHOD_APOLLO_VMR               7
#define SIP_MAP_METHOD_CHANNELS_V20             8

    /* 配合SIP_MSG_UPDATE_BLFLIST使用，作为lparam */
#define SIP_BLF_LIST_NOTIFY_TERMINATED      0   /* 表示notify已结束，UI应该灭灯 */
#define SIP_BLF_LIST_NOTIFY_IN_MSG          1   /* 表示notify由消息发送 */
#define SIP_BLF_LIST_NOTIFY_IN_FILE         2   /* 表示notify已写到文件中 */

    /*lync conference call mask*/
#define SIP_CALL_LYNC_CONF_NORMAL           0
#define SIP_CALL_LYNC_CONF_VIDEO            (1<<0)
#define SIP_CALL_LYNC_CONF_IM               (1<<1)
#define SIP_CALL_LYNC_CONF_GROUP            (1<<2)

typedef enum
{
    SIP_CONTROL_FROM_DEFAULT,                       /* 中性默认值 */
    SIP_CONTROL_FROM_CSTA,                          /* CSTA */
    SIP_CONTROL_FROM_TPCP,                          /* Lync联动TPCP */
    SIP_CONTROL_FROM_LYNC_AUTOMATION_TEST,          /* Lync自动化测试 */
    SIP_CONTROL_FROM_APOLLO,                        /* Apollo服务器 */
}sip_cmd_control_protocol_e;

typedef enum
{
    SIP_CAMERA_ACTION_PANLEFT,
    SIP_CAMERA_ACTION_PANRIGHT,
    SIP_CAMERA_ACTION_TILTDOWN,
    SIP_CAMERA_ACTION_TILTUP,
    SIP_CAMERA_ACTION_ZOOMOUT,
    SIP_CAMERA_ACTION_ZOONIN,
    SIP_CAMERA_ACTION_FOCUSOUT,
    SIP_CAMERA_ACTION_FOCUSIN,
    SIP_CAMERA_ACTION_ILLEGAL,
} sip_camera_action_e;

/* SIP lync define*/
typedef enum
{
    /* presence userState */
    SIP_LYNC_PRES_USER_UNKNOWN,
    SIP_LYNC_PRES_USER_AVALIABLE,
    SIP_LYNC_PRES_USER_BUSY,
    SIP_LYNC_PRES_USER_DND,
    SIP_LYNC_PRES_USER_BEBACK,
    SIP_LYNC_PRES_USER_OFFWROK,
    SIP_LYNC_PRES_USER_APPERA_AWAY,
    SIP_LYNC_PRES_USER_BUSY_DND_OFF,
    SIP_LYNC_PRES_USER_OFFLINE,

    /* presence machineState */
    SIP_LYNC_PRES_MACHINE_ACTIVE,
    SIP_LYNC_PRES_MACHINE_INACTIVE,
    SIP_LYNC_PRES_MACHINE_AWAY,
    SIP_LYNC_PRES_MACHINE_OFFLINE,/* no used */

    /* presence phoneState */
    SIP_LYNC_PRES_PHONE_ON_PHONE,
    SIP_LYNC_PRES_PHONE_OFF_PHONE,
    SIP_LYNC_PRES_PHONE_ON_CONFERENCE,
    SIP_LYNC_PRES_PHONE_OFF_CONFERENCE,

    /* for reset status */
    SIP_LYNC_PRES_RESET_STATUS,

    SIP_LYNC_PRES_STATE_MAX
}sip_lync_presence_state_e;

typedef enum
{
    SIP_LYNC_ROUTING_CANCEL,
    SIP_LYNC_ROUTING_TO_VM,                  /* routing to voice mail */
    SIP_LYNC_ROUTING_TO_MY_NUMBER,           /* container work/mobile/home/other/lastPhone number / */
    SIP_LYNC_ROUTING_TO_MY_CONTACT,          /* routing to lastConatct what is in user information */
    SIP_LYNC_ROUTING_TO_NEW_NUMBER,          /* new number */
    SIP_LYNC_ROUTING_TO_NEW_CONTACT,         /* new contact */
    SIP_LYNC_ROUTING_TO_SIP_URI,             /* full sip uri */

    SIP_LYNC_ROUTING_TO_SIMULT_CANCEL,
    SIP_LYNC_ROUTING_TO_SIMULT_MY_NUMBER,    /* container work/mobile/home/other/lastPhone number / */
    SIP_LYNC_ROUTING_TO_SIMULT_NEW_NUMBER,   /* new number */
    SIP_LYNC_ROUTING_TO_SIMULT_PROXY,        /* proxy */
    SIP_LYNC_ROUTING_TO_SIMULT_TEAM,         /* team */

    SIP_LYNC_ROUTING_TO_E911_ACTIVE,            /* set e911 active to make fwd off when call e911 event in fwd off */
    SIP_LYNC_ROUTING_TO_E911_INACTIVE,          /* set e911 inactive when DUT is in e911 active */

    SIP_LYNC_ROUTING_FIRST,                  /* route when account first used */

    SIP_LYNC_ROUTING_MAX
}sip_lync_routing_type_e;

typedef enum
{
    SIP_LYNC_ROUTING_FWD_OFF,

    SIP_LYNC_ROUTING_FWD_TO_VM,
    SIP_LYNC_ROUTING_FWD_TO_NUM,
    SIP_LYNC_ROUTING_FWD_TO_DELEGATE,

    SIP_LYNC_ROUTING_SIMUL_TO_NUM,
    SIP_LYNC_ROUTING_SIMUL_TO_DELEGATE,
    SIP_LYNC_ROUTING_SIMUL_TO_TEAM,
    SIP_LYNC_ROUTING_E911_ACTIVE,        /* 设置 e911 flag 为 active，同时更新 route 为 FWD OFF，设置 route 时发送 inactive */
    SIP_LYNC_ROUTING_E911_INACTIVE       /* 设置 e911 flag 为 inactive，同时更新 route 为 FWD OFF */
}sip_lync_routing_update_type_e;

typedef enum
{
    SIP_LYNC_USER_INFO_LAST_CONTACT,
    SIP_LYNC_USER_INFO_LAST_PHONE,
    SIP_LYNC_USER_INFO_WORK,
    SIP_LYNC_USER_INFO_MOBILE,
    SIP_LYNC_USER_INFO_HOME,
    SIP_LYNC_USER_INFO_OTHER,
    SIP_LYNC_USER_INFO_MAX
}sip_lync_userinfomation_type_e;

typedef enum
{
    SIP_LYNC_PRIVACY_NOSELECT,
    SIP_LYNC_PRIVACY_PRIVATE,
    SIP_LYNC_PRIVACY_STANDARD,
    SIP_LYNC_PRIVACY_MIGRAT_TO_PRIVACY,
    SIP_LYNC_PRIVACY_MIGRAT_TO_STANDARD,
    SIP_LYNC_PRIVACY_MAX
}sip_lync_pirvacy_mode_e;


typedef enum
{
    SIP_LYNC_CONTACT_PRES_SUB_POLLING,          /* 一次性订阅，expires 为 0 */
    SIP_LYNC_CONTACT_PRES_SUB_PRESISTENT        /* 永久订阅，expires 非 0 */
}sip_lync_contact_pres_sub_expries_type_e;


typedef enum
{
    SIP_LYNC_CONTACT_FULL_LIST,             /* 上报响应的body，一般为完整的列表，UCS的情况下列表为空 */
    SIP_LYNC_CONTACT_DELTA_LIST,            /* 上报NOTIFY的body，一般为联系人列表变更的内容 */
    SIP_LYNC_CONTACT_PRES,                  /* 上报联系人状态信息，联系人类型通过UI传入的sub_id区分 */
} sip_lync_contact_info_type_e;

typedef enum
{
    SIP_LYNC_BTOE_PAIRING_FAILED,          /*联动配对失败*/
    SIP_LYNC_BTOE_STATUS_UN_PAIR,          /* 联动未配对*/ 
    SIP_LYNC_BTOE_STATUS_PAIR_CODE_CHECK,  /*联动配对码匹配*/
    SIP_LYNC_BTOE_STATUS_PAIRED,           /* 联动已配对（未登陆）*/  
    SIP_LYNC_BTOE_STATUS_SIGN_IN,           /* 联动已配对（已登陆）*/  
    SIP_LYNC_BTOE_GETTING_CFG,             /* 联动已配对正在获取配置*/
    SIP_LYNC_BTOE_CFG_CANCEL,              /*Lync客户端弹框被取消*/
    SIP_LYNC_BTOE_CLIENT_SIGN_OUT,      /*Lync客户端登出*/
}sip_lync_BToE_status_e;

typedef enum 
{
    SIP_CALL_OUT_DEFAULT,
    SIP_CALL_OUT_BTOE_ACCOUNT,             /* 联动联系人呼叫 */
    SIP_CALL_OUT_BTOE_ANONYMOUS            /* 联动匿名呼叫 */
} sip_cmd_call_out_type_e;

typedef enum
{
    SIP_CALL_TYPE_NORMAL,
    SIP_CALL_TYPE_CONFERENCE,
    SIP_CALL_TYPE_VOICEMAIL,
    SIP_CALL_TYPE_PSTN
} sip_call_info_type;
/************************************************************************/
/*                                                                      */
/************************************************************************/

/* map struct for data transmit */
typedef struct
{
    unsigned short type;    /* 数据类型 */
    unsigned short size;    /* 数据大小 */
    void *ptr;              /* 数据内容 */
}sip_map_t;

typedef struct  
{
 /* 和cfgserver保持一致，注意这里需要设置READY，SIP才会使用外部设置的网络参数 */
#define SIP_NET_STATUS_OFF              0        /*network is hang*/
#define SIP_NET_STATUS_READY         2        /*network is ready*/

    int network_status;             /* 网络状态(当前未使用，置0即可) */
    int has_vpn;                    /* 是否有使用IPV4 VPN */
    int has6_vpn;                   /* 是否有使用IPV6 VPN(该位当前未使用，置0即可) */
    struct in_addr  a4_addr;        /* IPV4地址 */
    struct in_addr  subnet_addr;    /* IPV4的指望掩码 */
    struct in6_addr a6_addr;        /* IPV6地址 */
    struct in_addr  vpn_addr;       /* IPV4 VPN地址 */
    struct in6_addr vpn6_addr;      /* IPV6 VPN地址(当前未使用，置0即可) */
    struct in_addr  dns_addr[2];    /* IPV4的DNS地址 */
    struct in6_addr dns6_addr[2];   /* IPV6的DNS地址 */
    char   mac_addr[32];            /*MAC地址*/
    char   access_type[32];         /* access type类型，当前看不会超过32个字符 */
    char   reserved[32];            /* 预留 */
}sip_network_t;


/* user infomation for basic call display */
typedef struct
{
    char display_name[SIP_MAX_NAME_LEN];
    char    user_name[SIP_MAX_NAME_LEN];
    char  server_name[SIP_MAX_NAME_LEN];
}sip_user_info_t;

typedef struct
{
    char sipuri[SIP_MAX_URI_LEN];
    char authuri[SIP_MAX_URI_LEN];
    char password[SIP_MAX_NAME_LEN];
}sip_account_cfg_t;

typedef struct  
{
    sip_user_info_t user_info;
}sip_call_be_forward_t;

typedef struct  
{
    int new_cid;
    sip_user_info_t user_info;
}sip_call_be_transfer_t;

typedef struct 
{
    int new_cid;
    sip_user_info_t user_info;
}sip_call_be_replace_t;

typedef struct 
{
    sip_call_be_forward_t base_forward;
    int new_type;
    char reserved[128];
}sip_call_be_forward_ext_t;

typedef struct 
{
    sip_call_be_transfer_t base_transfer;
#define SIP_CALL_SERVER_TRANSFER         (1<<0)
    int new_type;
    char reserved[128];
}sip_call_be_transfer_ext_t;

typedef struct 
{
    sip_call_be_replace_t base_replace;
    int new_type;
    char reserved[128];
}sip_call_be_replace_ext_t;

/* device pairing功能中pairing url和pairing code */
typedef struct
{
    char pair_url[SIP_MAX_URI_LEN];
    char pair_code[SIP_MAX_UUID_LEN];
}sip_dev_pairing_info_t;

/* SIP信令Header头域的内容，比如"User-Agent: Yealink xxx" */
typedef struct
{
    char  name[SIP_MAX_HEADER_NAME_LEN];    /* 比如上述的"User-Agent" */
    char value[SIP_MAX_HEADER_VALUE_LEN];   /* 比如上述的"Yealink xxx" */
}sip_header_info_t;

/* SIP收到SMS时上报的内容 */
typedef struct
{
    int             type;                           /* sms类型，用于区别部分客户定制 */
    sip_user_info_t user_info;                      /* 对方用户信息 */
    char            p_flash[SIP_MAX_NAME_LEN];      /* P_FLASH_SMS定制 */
    char            xml_body[SIP_MAX_XML_BODY_LEN]; /* xml body */
}sip_sms_info_t;


/* sip收到re-invite时上报的内容 */
typedef struct
{
    int tid;                            /* re-invite的tid */

#define SIP_CALL_REINVITE_FLAG_AUDIO_UPGRADE_TO_VIDEO            1 << 0  /* 音频切视频 */
#define SIP_CALL_REINVITE_FLAG_BW_EMERGENCY_RING_BACK            1 << 1  /* broadworks 紧急呼叫回呼 */

    int type;                           /* re-invite的类型 */
} sip_reinvite_t;

/* 上报SIP收到MWI时的结构体 */
typedef struct
{
    int new_msg;                        /* 新消息个数 */
    int old_msg;                        /* 旧消息个数 */
    int new_urgent;
    int old_urgent;
    int new_fax;
    int old_fax;
    int msg_waiting;                    /* 是否message waiting */
    char msg_account[SIP_MAX_URI_LEN];     /* 服务器下发的notify body */
} sip_mwi_info_t;

/* verizon Visual Voice Mail (VVM)中的消息类型。1-New Message；2-Mailbox Update Message*/
#define SIP_MSG_VVM_TYPE_NM     1
#define SIP_MSG_VVM_TYPE_MBU    2

/* 上报verizon Visual Voice Mail (VVM) 中的 "New Message" */
#define SIP_VVX_ID_MAX_LEN      22
#define SIP_VVX_DATE_MAX_LEN    23

typedef struct
{
    int  unheard_msg;                   /* 新消息/未听消息个数 */
    char id[SIP_VVX_ID_MAX_LEN];        /* 消息ID，用于取消息 */
    char type;                          /* 消息类型;v = Voice;o = Video;f = Fax;i = Infotainment;e = ECC */
    char msg_sender[SIP_MAX_URI_LEN];   /* 消息的产生者 */
    char date[SIP_VVX_DATE_MAX_LEN];    /* 时间和日期 */
    int  len;                           /* 消息的长度，以秒为单位，比如有5s的未读语音消息 */
} sip_vvm_nm_t;

/* 上报verizon Visual Voice Mail (VVM) 中的 "Mailbox Update Message" */
typedef struct
{
    int unheard_msg;                    /* 新消息/未听消息个数 */
    char reserved[32];                  /* 预留32个字节，方便后续扩展 */
} sip_vvm_mbu_t;

/* 操作原因结构 */
typedef struct
{
    int  uuid;                          /* 唯一标示符，用于区分信息来源 */
    int  code;                          /* 错误码,为0表示成功 */
    char reason[SIP_MAX_NAME_LEN];      /* 错误原因 */
}sip_reason_info_t;

/* UI回复reinvite */
typedef struct
{
    int tid;                            /* reinvite 事务的ID */
    sip_reason_info_t response;         /* 错误码以及原因 */
} sip_reinvite_info_t;

/* 上报Dsskey的状态，用于BLF/BLA等 */
typedef struct
{
    int index;                          /* 第几个DSSKEY */
    int status;                         /* 该KEY的状态 */

    #define SIP_BLF_AUTO_PICKUP_MASK            1 << 0  /* mask it can be pickup */
    #define SIP_BLF_WILDIX_LED_MODE0_MASK       1 << 1
    #define SIP_BLF_WILDIX_LED_MODE1_MASK       1 << 2
    int flag; /* 取以上值 */
    void *js;/* 注意：V80固件版本UI需注释掉这个参数，V81及之后的固件版本才使用，避免接口兼容性问题 */
}sip_dsskey_info_t;

/* 上报Dsskey BLF的信息，用于界面显示哪些DSSKEY可以pickup */
typedef struct
{
    sip_dsskey_info_t key;
    char              local_name[SIP_MAX_NAME_LEN];
    char              local_target_uri[SIP_MAX_URI_LEN];
    char              remote_name[SIP_MAX_NAME_LEN];
    char              remote_target_uri[SIP_MAX_URI_LEN];
}sip_dsskey_blf_info_t;

/* GCS收到NOTIFY时上报的结构体 */
typedef struct
{
    char user_name[SIP_MAX_NAME_LEN];
    char      body[SIP_MAX_XML_BODY_LEN];
}sip_gcs_info_t;

/* SCA每条line的状态 */
typedef struct
{
    int appearance_index;
    int appearance_state;           /* SIP_BLA_STATUS_UNKOWN */
    sip_user_info_t remote_user_info;
} sip_sca_status_t;


typedef struct
{
    int call_index;                     /* 通话线路的index */
    char conf_uri[SIP_MAX_URI_LEN];     /* 通话的会议uri */
} sip_bw_conf_index_t;


/* ACD状态 */
typedef struct
{
    int key_index;
    int status;
    int login_retry_after;
}sip_acd_status_info_t;

/* call indentity */
typedef struct
{
    unsigned short  version;
    unsigned short  size;

    int             answer_after;       /* -1:未指定，0:立即接听，>0:指定时间后自动接听 */
    unsigned char   is_emergency;
    unsigned char   force_auto_answer;
    unsigned char   force_no_forward;   /* 强制不forward */

        #define SIP_CALL_AUTO_ANSWER_DEVICE_UNKNOWN     0   /* 没有解析到相关字段 */
        #define SIP_CALL_AUTO_ANSWER_DEVICE_ACTIVE      1   /* 正在使用中的设备 */
        #define SIP_CALL_AUTO_ANSWER_DEVICE_SPEAKER     2   /* 扬声器 */
        #define SIP_CALL_AUTO_ANSWER_DEVICE_HEADSET     3   /* 耳机 */
        #define SIP_CALL_AUTO_ANSWER_DEVICE_HANDSET     4   /* 手柄 */
    unsigned char auto_answer_device;   /* 取以上值 */

        #define SIP_CALL_P_ANTO_ANSWER_UNKNOWN      0   /* 没有解析到相关字段 */
        #define SIP_CALL_P_ANTO_ANSWER_NORMAL       1   /* normal */
        #define SIP_CALL_P_ANTO_ANSWER_URGENT       2   /* urgent */
        #define SIP_CALL_P_ANTO_ANSWER_IMPERIOUS    3   /* imperious */
        #define SIP_CALL_P_ANTO_ANSWER_SILENT       4   /* silent */
    unsigned char p_auto_answer;        /* 取以上值 */

        #define SIP_CALL_ENABLE_MIC_UNKNOWN         -1  /* 没有解析到相关字段 */
        #define SIP_CALL_ENABLE_MIC_FALSE           0   /* 0 */
        #define SIP_CALL_ENABLE_MIC_TRUE            1   /* 1 */
    char enable_mic;                    /* 取以上值 */

        #define SIP_CALL_ENABLE_SPEAKER_UNKNOWN     -1  /* 没有解析到相关字段 */
        #define SIP_CALL_ENABLE_SPEAKER_FALSE       0   /* 0 */
        #define SIP_CALL_ENABLE_SPEAKER_TRUE        1   /* 1 */
    char enable_speaker;                /* 取以上值 */

        #define SIP_CALL_INCOMING_TYPE_NORMAL                (1<<0)   
        #define SIP_CALL_INCOMING_TYPE_LYNC_CONF             (1<<1)   /* lync conference call*/
        #define SIP_CALL_INCOMING_TYPE_LYNC_BTOE             (1<<2)   /* lync btoe call*/
        #define SIP_CALL_INCOMING_TYPE_LYNC_PRIVATE_LINE     (1<<4)   /* lync private line */
        #define SIP_CALL_INCOMING_TYPE_LYNC_CPS              (1<<5)   /* lync call park service call */
        #define SIP_CALL_INCOMING_TYPE_APOLLO_CONF           (1<<6)   /* apollo conference call*/
        #define SIP_CALL_INCOMING_TYPE_LOOPBACK              (1<<7)   /* loopback call*/
    unsigned char inc_call_type;                /* 取以上值 */

        #define SIP_CALL_NO_FORWARDING                      0
        #define SIP_CALL_FORWARDING                         1
        #define SIP_CALL_LYNC_FORWARDING_BOSS                    2
        #define SIP_CALL_LYNC_FORWARDING_DELEGATE                3
        #define SIP_CALL_LYNC_FORWARDING_TEAM_LEADER             4
        #define SIP_CALL_LYNC_FORWARDING_TEAM_MEMBER             5
        #define SIP_CALL_LYNC_FORWARDING_RESPONSE_GROUP          6
    unsigned char   retarget_reason;      /* 取以上值 */

        #define SIP_CALL_INCOMING_IS_AUDIO    0
        #define SIP_CALL_INCOMING_IS_VIDEO     1
    unsigned char   inc_video; /*视频来电*/

        #define SIP_CALL_INCOMING_MEDIA_NORMAL   0
        #define SIP_CALL_INCOMING_MEDIA_FILE     1
    unsigned char   inc_media_type;/*V81SP2 UI已编译，SIP内部未使用*/

    unsigned char sca_appearance_index;

    unsigned char   reserved_pad[4];/*reserved_pad[0] 取以上值*/

    sip_map_t       display_name;
    sip_map_t       user_name;
    sip_map_t       server_name;

    sip_map_t       diversion_diaplay_name;
    sip_map_t       diversion_user_name;
    sip_map_t       diversion_server_name;

    sip_map_t       to_diaplay_name;
    sip_map_t       to_user_name;
    sip_map_t       to_server_name;

    sip_map_t       alert_local;        /* 本地铃声 */
    sip_map_t       alert_web;          /* 网页匹配铃声 */
    sip_map_t       alert_url;          /* 下载铃声 */
    sip_map_t       pict_info;
    sip_map_t       from_user_name;
    sip_map_t       diversion_line;     /* NFON中定制 */
    sip_map_t       lync_on_behalf;
    sip_map_t       doorline_uri;
    sip_map_t       doorline_dtmf;
    sip_map_t       custom_info;
    sip_map_t       apollo_conf_data;

    sip_map_t       reserved[1];
}sip_call_identify_info_t;

typedef struct
{
    char call_id[SIP_MAX_HEADER_VALUE_LEN];     /* SIP信令层的Call-ID，一般为随机字符串 */
    char remote_tag[SIP_MAX_HEADER_VALUE_LEN];  /* SIP信令层专用 */
    char local_tag[SIP_MAX_HEADER_VALUE_LEN];   /* SIP信令层专用 */
}sip_call_proto_info_t;

typedef struct
{
    char call_id[SIP_MAX_HEADER_VALUE_LEN];     /* SIP信令层的Call-ID，一般为随机字符串 */
    char remote_tag[SIP_MAX_HEADER_VALUE_LEN];  /* SIP信令层专用 */
    char local_tag[SIP_MAX_HEADER_VALUE_LEN];   /* SIP信令层专用 */
    char from[SIP_MAX_URI_LEN];                 /* SIP信令层的From地址 */
    char to[SIP_MAX_URI_LEN];                   /* SIP信令层的To地址 */
}sip_call_proto_info2_t;

typedef struct
{

#define SIP_CALL_TRANSPORT_NONE         0
#define SIP_CALL_TRANSPORT_UDP_HOST     1
#define SIP_CALL_TRANSPORT_UDP_SRFLX    2
#define SIP_CALL_TRANSPORT_UDP_PRFLX    3
#define SIP_CALL_TRANSPORT_UDP_RELAY    4
#define SIP_CALL_TRANSPORT_TCP_SRFLX    5
#define SIP_CALL_TRANSPORT_TCP_RELAY    6
    unsigned char mode;

    unsigned char call_type;

    unsigned char reserved[6];

    char body[SIP_MAX_NAME_LEN];
}sip_call_transport_info_t;

typedef struct
{
#define SIP_UACSTA_TYPE_CONNECT         0           /* 用于 uaCSTA 控制的连接 */
#define SIP_UACSTA_TYPE_CONTROL         1           /* 用于 uaCSTA 控制的事件 */
#define SIP_UACSTA_TYPE_DISCON          2           /* 用于 uaCSTA 控制的断开 */

    int             type;                           /* 功能类型（备用） */
    int             tid;                            /* tid 事务id */
    int             body_len;                       /* body的大小 */
    char            body[SIP_MAX_XML_BODY_LEN];     /* 消息的body，post给UI时只带body_len的大小 */
}sip_uacsta_req_info_t;

/* 携程联动：用于上报远端控制命令（INFO形式的联动） */
typedef struct
{
    int             tid;                            /* tid 事务id */
    int             body_len;                       /* body的大小 */
    char            body[SIP_MAX_XML_BODY_LEN];     /* 消息的body，post给UI时只带body_len的大小 */
}sip_uc_req_info_t;

/* 携程联动：用于上报远端控制命令（NOTIFY形式的联动，复用INFO结构） */
typedef sip_uc_req_info_t   sip_uc_req_notify_t;


/* 携程联动：用于UI推送INFO */
typedef struct
{
    int type;                                       /* 功能类型（备用）*/
    int tid;                                        /* tid 事务id */
    int uuid;                                       /* uuid*/
    char status[SIP_MAX_URI_LEN];                   /* 上报服务器的状态*/
    char cmd[SIP_MAX_URI_LEN];                      /* hold/unhold命令需要使用cmd节点 */
    char cause[SIP_MAX_URI_LEN];                    /* 上报服务器失败原因*/
    char calltag[SIP_MAX_NAME_LEN];                 /* 上报服务器主被叫*/
    char reserve[2 * SIP_MAX_URI_LEN];              /* 预留字段 */
}sip_uc_req_send_info_t;

/* 
* 开启kpml后，UI记录did和tag，用于发送kpml notify使用(包括dtmf, 输入超时，关闭kpml后的notify)
*/
typedef struct
{
    int  kpml_did;                /* dialog id */
    char kpml_tag[32];            /* kpml regex tag attr */
}sip_kpml_info_t;

/*
 *------------------------------------
 *      call media param define
 *------------------------------------
 */

typedef struct _sip_h264_param {
    unsigned char level;/**/
    unsigned char reserved[3];
    unsigned int max_mbps;
    unsigned int max_fs;
} sip_h264_param_t;

typedef struct _sip_h265_param 
{
    char enable; /*-1: for init 0:disable 1:enable 2:modify*/
    unsigned char profile_space;
    unsigned char profile_id;
    unsigned char level_id;
    unsigned int max_fps;
    unsigned int reserved[4];
}sip_h265_param_t;

typedef struct _h264_fmtp 
{
    unsigned char profile_idc;
    unsigned char profile_iop;
    unsigned char level;
    unsigned char packetization_mode;

    unsigned max_mbps;
    unsigned max_fs;
    unsigned max_cpb;
    unsigned max_dpb;
    unsigned max_br;
} h264_fmtp_t;

typedef struct _h265_fmtp
{
    unsigned char profile_space;
    unsigned char profile_id;
    unsigned char level_id;
    unsigned char tier_flag;

    unsigned max_fps;
    unsigned max_cpb;
    unsigned max_dpb;
    unsigned max_br;
    unsigned max_lps;
    unsigned max_lsr;
} h265_fmtp_t;


typedef struct _h263_fmtp 
{
    unsigned char SQCIF;
    unsigned char QCIF;
    unsigned char CIF;
    unsigned char CIF4;
} h263_fmtp_t;

typedef struct _amr_fmtp 
{
    char mode_set[18];/*0,1,2,3,4,5,6,7,8*/

    unsigned char octet_align;/*0-1*/
    unsigned char mode_change_capability;/*1-2*/
    unsigned char mode_change_neighbor;/*0-1*/
    unsigned char mode_change_period;/*1-2*/
    unsigned char crc;/*0-1*/
    unsigned char robust_sorting;/*0-1*/
    unsigned char interleaving;
    unsigned char pad[3];

    unsigned max_red;
} amr_fmtp_t;

typedef struct _opus_fmtp {
    unsigned int max_playback_rate;
    unsigned int sprop_max_capture_rate;
    unsigned int max_average_bitrate;

    unsigned char stereo;
    unsigned char sprop_stereo;
    unsigned char cbr;
    unsigned char use_inband_fec;
} opus_fmtp_t;

/* audio extension param */
typedef struct
{
#define SIP_AUDIO_TYPE_NONE    0
#define SIP_AUDIO_TYPE_AMR     1
#define SIP_AUDIO_TYPE_AMR_WB  2
#define SIP_AUDIO_TYPE_OPUS    3
    int bitrate;
    int annexb;

    int audio_codec_type;
    union 
    {
        amr_fmtp_t amr; 
        opus_fmtp_t opus;
    } param;

}sip_param_audio_t;

typedef struct _vp8_fmtp {
    unsigned int max_fr;        /* maximum frame rate */
    unsigned int max_fs;        /* maximum frame size */

    unsigned char pad[16];       /* reserved */
} vp8_fmtp_t;


/* video extension param */
typedef struct 
{
#define SIP_VIDEO_TYPE_NONE     0
#define SIP_VIDEO_TYPE_H263     1
#define SIP_VIDEO_TYPE_H264     2
#define SIP_VIDEO_TYPE_VP8      3
#define SIP_VIDEO_TYPE_H265     4
    int video_codec_type;
    union 
    {
        h263_fmtp_t h263;
        h264_fmtp_t h264;
        vp8_fmtp_t  vp8;
        h265_fmtp_t h265;
    } param;
}sip_param_video_t;

/* h224 extension param */
typedef struct
{
    int reserved;
}sip_param_h224_t;


/* string extension param */
typedef struct
{
    char reserved[128];
}sip_param_string_t;

/* codec basic param */
typedef struct
{
    int payload;                        /* 负载, like 109 */
    int samplerate;                    /* 采样率 */
    char codec_name[SIP_MAX_CODEC_NAME_LEN];/* like "PCMU" */

    /* codec extension param */
    union
    {
        sip_param_audio_t        audio;
        sip_param_video_t        video;
        sip_param_string_t       string;
    } ext_param;
}sip_codec_t;

/* media address */
typedef union
{
    short sa_family;
    struct sockaddr_in  a4;
    struct sockaddr_in6 a6;
}sip_addr_t;

/* media data param */
typedef struct 
{
    sip_addr_t      rtp_addr;
    sip_addr_t      rtcp_addr;

    unsigned char   media_direction;/* 媒体RTP发送方向，如sendrecv/sendonly... */
    unsigned char   rtp_ptime;      /* ptime */

    unsigned char   rtpev_payload;  /* rtp event payload, like dtmf 101 */
#define SRTP_UNENCRYPTED_RTP    (1<<0)
#define SRTP_UNENCRYPTED_RTCP   (1<<1)
#define SRTP_NO_AUTH            (1<<2)
    unsigned char   srtp_flag;      /* 使用位表示SRTP是否不加密RTP、不加密RTCP、不携带Auth Tag */

    unsigned        bandwidth;      /* 带宽 */

    sip_codec_t     codec; /* codec param */

    char            srtp_key[SIP_MAX_SRTP_KEY_LEN]; /* 存在unencryped属性后，预留的96个字节已经无法完整上报，因此需要添加srtp_flag */
    char            rtcp_fb [SIP_MAX_RTCP_FB_LEN];

    signed char     looback_type;
    signed char     loopback_role;
    signed char     loopback_pkt_mode;
    signed char     loopback_pkt_dynamic_pl;

    int             dtmf_samplerate;   /* 从目前来看大多数平台的Int都是四字节，应该没有前向兼容问题 */
    unsigned char   cvo_identifier;    /* cvo的标识码：参见http://www.potaroo.net/ietf/idref/rfc5285/ */
    char            reserved_ex[7];    /* 预留扩展 */
}sip_media_t;

/*
 * logic channel (both local & remote medial data)
 */
typedef struct
{
    unsigned char rtcp_rsize_support;                   /*是否支持rtcp-rsize*/
    unsigned char reserved[3];
    uint32_t      ssrc_id;                              /*对应于ssrc的ssrc id，rfc5576中定义为32位无符号整形 */
    char          ssrc_cname[SIP_MAX_SSRC_CNAME_LEN];   /*对应于ssrc中的cname */
    char          rtp_key[SIP_MAX_RTP_KEY_LEN];
    char          rtcp_key[SIP_MAX_RTP_KEY_LEN];

    int           reserved_int[4];
    char          reserved_ext[32];                     /*额外待确认预留参数*/
}sip_media_ext_t;

typedef struct
{
    int             channel_type;

    sip_media_ext_t local_media_ext;                    /* audio 额外参数 */
    sip_media_ext_t remote_media_ext;                   /* video 额外参数 */

}sip_channel_ext_t;

typedef struct
{
    int             channel_type;

    sip_media_t     local_media;    /* 本地媒体参数用于编码 */
    sip_media_t     remote_media;   /* 远端媒体参数用于解码 */
}sip_channel_t;


typedef struct _sip_codec_array
{
    unsigned char codec_cnt;
    unsigned char codec_type;
    unsigned char reserved[2];
    
    sip_codec_t local [SIP_MAX_CODEC_LIST];
    sip_codec_t remote[SIP_MAX_CODEC_LIST];
} sip_codec_array_t;


typedef struct _sip_media_codec_array
{
    unsigned media_cnt;
    sip_codec_array_t codec_array[SIP_MAX_MEDIA_LIST];
}sip_media_codec_array_t;

/*
 * call ringback info
 */
typedef struct
{
#define SIP_RINGBACK_TYPE_NORMAL                0   /* 普通响铃 */
#define SIP_RINGBACK_TYPE_RB_MUSIC              1   /* 彩铃 */
#define SIP_RINGBACK_TYPE_QUEUED                2   /* 182 queued (call waiting) */
#define SIP_RINGBACK_TYPE_HW_ACD_LOGIN          3   /* 华为ACD签入成功 */
#define SIP_RINGBACK_TYPE_PREVIEW               4   /* 视频来电预览 */
#define SIP_RINGBACK_SILENT                     5   /* 无声铃 */
    int type;
    sip_channel_t audio;     /* 彩铃时所需的通话媒体信息 */
    sip_channel_t video;    /*视频话机需要支持视频彩铃*/
}sip_call_ringback_info_t;


typedef struct
{
    char l_rtcp_key[SIP_MAX_SRTP_KEY_LEN];      /* local rtcp key */
    char r_rtcp_key[SIP_MAX_SRTP_KEY_LEN];      /* remote rtcp key */
}sip_rtcp_key_t;

typedef struct
{
    sip_rtcp_key_t audio;             /* audio rtcp key pair */
    sip_rtcp_key_t video;             /* video rtcp key pair */
    sip_rtcp_key_t ext_video;         /* ext_video rtcp key pair（暂未使用） */

    sip_rtcp_key_t reserved;          /* 暂未使用 */
}sip_rtcp_key_pairs_t;

/*
 * call param for establish
 */
typedef struct
{
    unsigned short          version;
    unsigned short          size;

    unsigned int            hold_state;
    unsigned int            call_flag;

    sip_map_t               audio;      /* ptr=sip_channel_t */
    sip_map_t               video;      /* ptr=sip_channel_t */
    sip_map_t               ext_video;  /* ptr=sip_channel_t */

    sip_map_t               reserved[3];
} sip_channels_t;

typedef struct
{
    unsigned short          version;
    unsigned short          size;

    unsigned int            hold_state;
    unsigned int            call_flag;

    sip_map_t               audio;          /* ptr=sip_channel_t */
    sip_map_t               video;          /* ptr=sip_channel_t */
    sip_map_t               ext_video;      /* ptr=sip_channel_t */

    sip_map_t               reserved[3];

    sip_map_t               audio_ext;      /* ptr=sip_channel_ext_t */
    sip_map_t               video_ext;      /* ptr=sip_channel_ext_t */
    sip_map_t               extvideo_ext;   /* ptr=sip_channel_ext_t */

    sip_map_t               reserved_ext[3];

} sip_channels_v20_t;

typedef struct
{
    int type;
    int total_len;
    int payload_len;
    char payload[4];
} sip_data_header_t;


typedef sip_data_header_t sip_call_infomation_header_t;


typedef struct
{
    unsigned char   allow_simultaneous_ringing;             /* 是否允许同时响铃 */
    unsigned char   allow_call_forwarding;                  /* 是否允许fwd至号码或联系人 */
    unsigned char   enable_call_park;                       /* 是否启用callpark */
    unsigned char   enable_call_transfer;                   /* 是否启用transfer */
    unsigned char   enable_delegation;                      /* 是否启用fwd至代理 */
    unsigned char   enable_teamcall;                        /* 是否启用团队呼叫 */

#define SIP_LYNC_VDM_ONPREM                       0         /* OnPrem */
#define SIP_LYNC_VDM_ONLINE                       1         /* Online */
#define SIP_LYNC_VDM_ONLINE_BASIC                 2         /* OnlineBasic */
#define SIP_LYNC_VDM_ONPREM_ONLINE_HYBRID         3         /* OnPremOnlineHybrid */
    unsigned char   voice_deployment_mode;                  /* 指示客户端当前连接的PSTN类型 */

    /* 以下参数均为预留 */
    unsigned char   enable_malicious_call_trace;            /* 是否启用恶意呼叫跟踪(预留) */
    unsigned char   enable_bw_policy_override;              /* 是否启用带宽策略覆盖(预留) */
    unsigned char   uc_enabled;                             /* Ignored(预留) */
    unsigned char   reserved_pad[2];                        /* 四字节对齐 */
} sip_lync_uc_policy_t;


typedef struct
{
    unsigned char   allow_ip_audio;             /* Specifies whether IP Audio functionality is enabled on the client */

    /* 以下参数均为预留 */
    unsigned char   allow_ip_video;             /* Specifies whether IP Video functionality is enabled on the client */
    unsigned short  audio_bit_rate;             /* Set the bit rate for Audio */
    unsigned short  video_bit_rate;             /* Set the bit rate for Video */
    unsigned short  meeting_size;               /*  Maximum number of attendees allowed in meetings */
    unsigned char   allow_anon_participants;    /* Boolean value that allows anonymous participants to join the meeting */
    unsigned char   require_passcode;           /* Controls whether a pass code is required to join dial-in conferences */
    unsigned char   enable_pstn_conferencing;   /* Enable conferencing audio call over a PSTN network */
    unsigned char   trusted_conf_pin_required;  /* Require that trusted conferencing PIN be specified to join the conference */
    unsigned char   reserved_pad[8];            /* 预留，四字节对齐 */
} sip_lync_meeting_policy_t;

typedef struct
{
    unsigned char   e911_enable;                    /* 是否启用 E911 */
    unsigned char   loc_for_e911;                   /* 位置信息是否只用于紧急呼叫 */
    unsigned short  loc_refresh_hour;               /* 重新获取位置信息的间隔时间，单位小时，1-12 */

#define SIP_LYNC_LOCATION_REQUIRED_YES         0    /* 需要位置信息 */
#define SIP_LYNC_LOCATION_REQUIRED_NO          1    /* 不需要位置信息 */
#define SIP_LYNC_LOCATION_REQUIRED_DISCLAIMER  2    /* 需要提示位置信息未设置的警告 */
    unsigned char   loc_required;                   /* 如果客户端无法通过LIS获取到位置信息，根据该值做相应的提示处理 */

    unsigned char   reserved_pad[3];                /* 预留，四字节对齐 */
}sip_lync_location_policy_t;

typedef struct
{
    char   policy_flag;                    /* 标记是否订阅到ipPhonePolicy，允许值0,1 */

/* 如果部分配置未订阅到，则置为SIP_LYNC_PROVISION_VALUE_UNKNOW，仍使用话机配置 */
#define  SIP_LYNC_PROVISION_VALUE_UNKNOW    -1
    char   enable_one_touch_voicemail;     /* 一键式语音邮箱 */
    char   date_time_format;               /* 时间格式 */
    char   enable_device_update;           /* 允许设备更新 */
    char   enable_exchange_calendaring;    /* 启用ExchangeCalendaring */
    char   enable_btoe;                    /* 启用BTOE */
    char   enable_power_save_mode;         /* 启用省电模式 */
    int    power_save_during_office_timeout;   /* 上班时间话机不活跃，进入省电模式时间 */
    int    power_save_post_office_timeout;     /* 下班话机不活跃，进入省电模式时间 */
    int    user_dial_timeout;              /* 拨号等待超时时间 */
#define    BTOE_PAIRING_MODE_AUTO    0
#define    BTOE_PAIRING_MODE_MANUAL   1
    char   btoe_pairing_mode;
    char   reserved_pad[16];               /* 预留，四字节对齐 */
}sip_lync_ipphone_policy_t;

typedef struct
{
    unsigned short  min_pin_len;                    /* 最小PIN长度，4-15 */
    unsigned short  phone_timeout;                  /* 超时锁屏时间，单位分 0-1440 */
    unsigned char   enforce_pin_lock;               /* 是否强制设备锁定 */
    unsigned char   max_pin_retry;                  /* 解锁输入尝试次数，3-10，如果为0则使用预置默认值 */

    unsigned char   reserved_pad[6];                /* 预留，四字节对齐 */
}sip_lync_phone_lock_policy_t;

typedef struct
{
    unsigned int    hotdesking_timeout;             /* 切换 CAP 账号的超时时间，单位秒，最小30，无最大 */
    unsigned char   enable_hotdesking;              /* 表明 CAP 是否支持 hotdesking，即话机是否提供普通账号切换 */
    unsigned char   common_area_phone;              /* 表明是否是 Common Area phone.*/

    unsigned char   reserved_pad[2];                /* 预留，四字节对齐 */
}sip_lync_cap_cfg_t;


typedef struct
{
    unsigned char   bypass_enabled;                 /* 服务器是否开启 bypass */
    unsigned char   int_bypass_mode;                /* 内网用户是否启用 bypass */
    unsigned char   ext_bypass_mode;                /* 外网用户是否启用 bypass */
    unsigned char   reserved_pad;                   /* 预留，四字节对齐 */
}sip_lync_media_cfg_t;



/* 其它配置，模块性关联不大的整型配置集合 */
typedef struct
{
#define SIP_LYNC_ABS_WEB_AND_FILE           0       /* WebSearchAndFileDownload */
#define SIP_LYNC_ABS_WEB_ONLY               1       /* WebSearchOnly */
#define SIP_LYNC_ABS_FILE_ONLY              2       /* FileDownloadOnly */
    unsigned char   abs_usage;                      /* 用户访问通讯簿服务器中信息的方式 */

#define SIP_LYNC_NO_PHOTO                   0       /* 不显示照片 */
#define SIP_LYNC_PHOTOS_FROM_AD_ONLY        1       /* 仅显示在 AD 的照片 */
#define SIP_LYNC_ALL_PHOTOS                 2       /* 可以显示 AD 上或者自定义的照片 */
    unsigned char   photo_usage;                    /* 是否要显示用户以及联系人的照片 */
    unsigned char   disablehandsetonlockedmachine;
    unsigned char   reserved_pad[29];                /* 预留，四字节对齐 */
}sip_lync_common_cfg_t;


/*
 * Lync provision param
 */
typedef struct
{
    unsigned short  version;
    unsigned short  size;

    sip_lync_common_cfg_t           common_cfg;                     /* 模块性关联不大的配置集合 */

    sip_lync_phone_lock_policy_t    phone_lock_policy;              /* 键盘锁相关配置 */
    sip_lync_cap_cfg_t              cap_cfg;                        /* cap 相关配置 */
    sip_lync_uc_policy_t            uc_policy;                      /* UC 语音策略 */
    sip_lync_meeting_policy_t       meeting_policy;                 /* 会议策略 */
    sip_lync_media_cfg_t            media_cfg;                      /* 媒体相关配置（预留） */

    sip_lync_location_policy_t      loc_policy;                     /* e911 功能相关的位置策略信息 */
    sip_lync_ipphone_policy_t       ipp_policy;                     /* ipphone policy配置 */

    unsigned char   enable_skype_directory_search;
    unsigned char   enable_qos;                              /*是否开启QoE*/
    unsigned char   enable_incall_qos;                       /*是否开启incall QoE*/
    unsigned short incall_qos_interval_sec;                  /*QoE上报时间间隔*/
    unsigned char   reserved_pad[27];                                /* 预留 */


    sip_map_t       loc_e911_dial_str;              /* 紧急呼叫号码，1-10个字符，str */
    sip_map_t       loc_e911_dial_mask;             /* 紧急呼叫掩码，以;分隔，最大100个字符，str */
    sip_map_t       loc_e911_disclaimer;            /* 免责声明，str */

    /* 相关服务的 URL */
    sip_map_t       vm_uri;                         /* voicemail url，str */ 
    sip_map_t       lis_url;                        /* 位置信息服务 url，str */
    sip_map_t       dlx_url;                        /* 通讯组下载url，str */
    sip_map_t       abwq_url;                       /* 联系人Web搜索 url，str */
    sip_map_t       abs_url;                        /* abs url，下载联系人在AD上的图片，str */
    sip_map_t       update_url;                     /* log上传的地址*/
    sip_map_t       user_name;                      /* 说明是哪个用户的pvs*/

    sip_map_t       priorit_codecs_list;            /* codec优先级（从高到低）*/
    sip_map_t       loc_pvs_server_user;            /* 连接配置服务器认证用户名 */
    sip_map_t       loc_pvs_server_password;        /* 连接配置服务器认证密码 */
    sip_map_t       loc_pvs_server_addr;            /* 配置服务器地址 */
    sip_map_t       loc_pvs_server_type;            /* 连接配置服务器使用的协议 */

    /* 预留的配置 */
    sip_map_t       presence_policy;                /* Presence-related configuration data（预留） */
    sip_map_t       reserved_map[48];               /* 预留 */
} sip_lync_provision_t;


/*
 * Lync boss provision param
 */
typedef struct
{
    unsigned short  version;
    unsigned short  size;

    unsigned char   reserved_pad[8];                /* 预留 */

    sip_map_t       boss_uri;                       /* 表明是哪个 boss 的 provision 信息，str */
    sip_map_t       vm_uri;                         /* voicemail url，str */

    sip_map_t       reserved_map[4];                /* 预留 */
}sip_lync_boss_provision_t;

typedef enum
{
    LYNC_BOSS_SIMULTAN_RING_NONE = 0,         /*关闭同响设置*/
    LYNC_BOSS_SIMULTAN_RING_CONTACT,         /*设置同响到新号码*/
    LYNC_BOSS_SIMULTAN_RING_DELEGATE,         /*设置同响到代理*/
    LYNC_BOSS_SIMULTAN_RING_GROUP,             /*设置同响到响应组*/
    LYNC_BOSS_FORWARD_VM,                             /*设置来电转移到语音邮箱*/
    LYNC_BOSS_FORWARD_DELEGATE,                   /*设置来电转移到代理*/
    LYNC_BOSS_FORWARD_CONTACT                    /*设置来电转移到新号码*/
}sip_lync_boss_simultan_setting_e;

typedef struct  
{
    sip_lync_boss_simultan_setting_e type;
    char boss_uri[SIP_MAX_URI_LEN];
}sip_lync_boss_routing_t;

/*
*  Lync MWI Parameter *
*/
typedef sip_mwi_info_t sip_lync_mwi_data_t;

/*
*  Lync user infomation *
*/
typedef struct
{
    char last_contact[SIP_MAX_NAME_LEN];
    char last_phone[SIP_MAX_NAME_LEN];
    char work[SIP_MAX_NAME_LEN];
    char mobile[SIP_MAX_NAME_LEN];
    char home[SIP_MAX_NAME_LEN];
    char other[SIP_MAX_NAME_LEN];
}sip_lync_user_infomation_t;

/*
*  Lync contact card *
*/
typedef struct
{
    unsigned short    version;
    unsigned short    size;

    unsigned char     show_photo;                       /* 是否显示自身头像(对应displayADPhoto属性)，0：否， 1：是*/
    unsigned char     vm_enable;                        /* 账号是否启用 VM，0：否， 1：是 */
    unsigned char     reserved_pad[6];                  /* 四字节对齐 */

    sip_map_t         display_name;                     /* 显示名 */
    sip_map_t         abs_hash;                         /* 头像 AD 下载 abs hash */
    sip_map_t         ews_url;                          /* Exchange Web Service URL */
    sip_map_t         ews_primary_smtp;                 /* Primary SMTP 地址 */
    sip_map_t         ews_hash;                         /* ews头像 hash */
    sip_map_t         web_url;                          /* Web URL */
    sip_map_t         web_hash;                         /* web头像 hash */
    sip_map_t         email;

    sip_map_t         reserved[7];
}sip_lync_contact_card_t;

/*Skype contact info*/
typedef struct  
{
    char user[SIP_MAX_URI_LEN];
    char display_name[SIP_MAX_NAME_LEN];
}sip_lync_public_cloud_contact_info_t;




/*
*  Lync contacts info *
*/


typedef struct 
{
    int uuid;                                       /* UI传入，用于区分订阅的类型，SIP将原值返回 */

#define SIP_LYNC_PRES_SUB_OK                            0
#define SIP_LYNC_PRES_SUB_FAIL                          -1
    int pres_sub_result_code;                       /* 联系人订阅结果的响应 */

    /* result code 为 OK 时才处理如下值 */
    char boundary[64];                              /* 分割字符串 */

#define SIP_LYNC_CONTACT_BODY_IN_MSG        0       /* 表示contacts body 由消息发送 */
#define SIP_LYNC_CONTACT_BODY_IN_FILE       1       /* 表示contacts body 写到文件中 */
    unsigned int buf_type;                          /* 取以上值: 1:文件名，0:BODY */
    char buf[SIP_MAX_MSGKIT_BODY_LEN];              /* 存文件名，或者BODY */
} sip_lync_contacts_info_t;


typedef enum
{
    LYNC_PRES_SUB_RETRY_ALL_LOCAL_CONTACTS,                 /* UI重新订阅所有本地联系人，SIP删除相关js */
    LYNC_PRES_SUB_RETRY_CROSS_POOL_CONTACTS,                /* UI重新订阅指定pool的联系人，SIP不处理 */
    LYNC_PRES_SUB_RETRY_EXTERNAL_CONTACTS,                  /* UI不处理，SIP重新订阅指定外部联系人 */
}sip_lync_pres_sub_retry_action_e;

typedef struct
{
    unsigned short action;                                     /* sip_lync_pres_sub_retry_action_e */
    unsigned short interval;                                   /* 重新发起订阅的间隔时间 */
    char target[SIP_MAX_URI_LEN];                              /* 订阅接口的 to 参数 */

    unsigned char reserved[4];
}sip_lync_contacts_pres_sub_retry_t;



/*
*  lync conference user state
*/
typedef struct  
{
#define SIP_LYNC_CONF_ROLE_NONE         0
#define SIP_LYNC_CONF_ROLE_PRESENTER    1
#define SIP_LYNC_CONF_ROLE_ATTENDEE     2
    unsigned char role;  /*会议成员角色*/

#define SIP_LYNC_CONF_TYPE_AUDIO_VIDEO  0
#define SIP_LYNC_CONF_TYPE_IM           1
#define SIP_LYNC_CONF_TYPE_PHONE_CONF  2
    unsigned char session_type;  /*会议类型*/

#define SIP_LYNC_CONF_UNHOLD            0
#define SIP_LYNC_CONF_HOLD              1
#define SIP_LYNC_CONF_HOLD_FROM_MCU     2
    unsigned char hold_status; /*会议成员hold状态*/

#define SIP_LYNC_CONF_UNMUTE            0
#define SIP_LYNC_CONF_MUTE              1
    unsigned char mute_status; /*会议成员mute状态*/

#define SIP_LYNC_MCU_UNBLOCK            0
#define SIP_LYNC_MCU_BLOCK              1
    unsigned char mcu_direction;

#define SIP_LYNC_FOCUS_CONNECTED   0
#define SIP_LYNC_FOCUS_ON_HOLD        1
    unsigned char lobby;
    unsigned char reserved[2];

    char display[SIP_MAX_NAME_LEN]; /*会议成员显示名*/
    char user_uri[SIP_MAX_URI_LEN]; /*会议成员uri*/
    char entity[SIP_MAX_URI_LEN]; /*会议成员uuid*/
    char media_id[SIP_MAX_NAME_LEN];  /*媒体会话类型*/
}sip_lync_conference_user_t;

typedef struct
{
    unsigned char reserved[4];
    char organizer_uri[SIP_MAX_URI_LEN]; /*会议组织者uri*/
    char organizer_display[SIP_MAX_NAME_LEN]; /*会议组织者显示名*/
}sip_lync_conference_descrip_t;

typedef struct
{
    unsigned char conf_lock;
    unsigned char entry_exit_announcements;
    unsigned char modify_filter_default;    /*默认参与者允许mute和unmute操作*/
    unsigned char modify_filter_presenter; /*演示者允许mute和unmute操作*/
    unsigned char initial_filter_default;       /*默认参与者进入会议mute的状态block和unblock，决定是否需要自动mute*/
    unsigned char initial_filter_presenter;   /*演示者进入会议mute的状态，决定演示者是否需要自动mute，该权限覆盖组织者*/
    unsigned char reserved[10];
}sip_lync_conference_entity_view_t;

typedef struct
{
    unsigned char reserved[4];

    char focus_uri[SIP_MAX_URI_LEN]; /*会议focus_uri*/
    char conversation_id[SIP_MAX_NAME_LEN]; /*会议conv id*/
}sip_lync_conference_focus_t;

typedef enum
{
    SIP_LYNC_CONF_REQ_FOCUS,
    SIP_LYNC_CONF_REQ_ADD_USER,
    SIP_LYNC_CONF_REQ_DELETE_USER,
    SIP_LYNC_CONF_REQ_MUTE_USER,
    SIP_LYNC_CONF_REQ_UNMUTE_USER,
    SIP_LYNC_CONF_REQ_MUTE_AUDIENCE,
    SIP_LYNC_CONF_REQ_ANNOUNCEMENT,
    SIP_LYNC_CONF_REQ_LOCK,
    SIP_LYNC_CONF_REQ_SETLOBBY_ACCESS,
    SIP_LYNC_CONF_REQ_MODIFY_ROLE
}sip_lync_conf_req_type_e;

typedef struct 
{
    unsigned char type; /*sip_lync_conf_req_type_e*/
    unsigned char code; /*错误码*/
    unsigned char sub_req_type;             /*允许或拒绝Lobby成员加入会议，会议角色1:presenter, 0:attentee*/
    unsigned char reserved[1];

    char user[SIP_MAX_NAME_LEN];
    char reason[SIP_MAX_NAME_LEN];
}sip_lync_conf_req_result_t;

typedef struct  
{
    unsigned short          version;
    unsigned short          size;
    unsigned char       reserved_pad[12];                 /*unsigned char 类型预留*/

    sip_map_t             conf_id;
    sip_map_t             pstn_num;
    sip_map_t             reserved_map[30];                /* 预留 */
}sip_lync_conf_info_map_t;

typedef struct 
{
    sip_lync_conf_info_map_t hdr;
    char payload[1024*4];
} sip_s2c_lync_conf_info_payload_t;

typedef struct 
{
    int status;
    char reason[SIP_MAX_NAME_LEN];
    char target[SIP_MAX_NAME_LEN];
}sip_callpark_info_t;

typedef struct
{
#define SIP_SYSLOG_TEMPORARY    0
#define SIP_SYSLOG_LONG_TERM    1
    char deadline;

#define SIP_SYSLOG_METHOD_UNKNOWN   -1
#define SIP_SYSLOG_METHOD_LOCAL     0
#define SIP_SYSLOG_METHOD_REMOTE    1
    char log_method;
    char log_level;
    char reserve;

    int server_ip;/* 网络字节 */
}sip_syslog_info_t;

/* 以下参数类型只是模糊分类，并不是严格的 */
#define SIP_EXT_PARAMS_DELETED          -1
/* 公共参数 0~127 */
#define SIP_EXT_PARAMS_UNDEFINED        0
#define SIP_UUID                        1      /**/
#define SIP_DTMF_CODE                   2
#define SIP_MESSAGE_REQUEST_TYPE        3      /* 指定消息类型 */
#define SIP_MESSAGE_RECEIVER            4      /* 指定消息接收者 */
#define SIP_MESSAGE_HEADERS             5      /* 允许UI添加额外头域，如果多个，请以CRLF(\r\n)分隔，示例如：Event: dialog\r\nContent-Type: application/dialog-info+xml\r\n */
#define SIP_MESSAGE_CONTENT_TYPE        6      /* 指定content type */
#define SIP_MESSAGE_BODIES              7      /* 指定消息的body */
#define SIP_MESSAGE_TID                 8      /* 指定消息事务的tid */
#define SIP_MESSAGE_RES_CODE            9      /* 指定响应消息的code */


/* 呼叫参数 128~255 */
#define SIP_MAKE_CALL_MASK              128
#define SIP_MAKE_CALL_CALLEE            129
#define SIP_MAKE_CALL_PICKUP_INDEX      130
#define SIP_MAKE_CALL_SCA_INDEX         131
#define SIP_MAKE_CALL_HOLD              132
#define SIP_MAKE_CALL_TIPTEL_INVOKE_ID  133     /* 呼叫头域添加类似"Event: call_invoke;InvokeID=5" */
#define SIP_BSFT_DISPOSITION_CODE       134     /* 在BSFT服务器上配置时，可以输入任意字符，最长为１０个字符 */
#define SIP_MAKE_CALL_NTCL_RPID         135     /* 客户NTCL要求在主叫发出的INVITE消息中加上RPID头域，来表明主叫的身份 */
#define SIP_MAKE_CALL_VIDEO_BANDWIDTH   136     
#define SIP_MAKE_CALL_H264_PARAM        137     
#define SIP_MAKE_CALL_BLA_INDEX         138
#define SIP_MAKE_CALL_TYPE              139     /* 附加的呼叫类型，正常是SIP通过CMD通知UI呼出时告诉UI呼出的附加类型，UI可以根据该类型进行相应处理 */
#define SIP_MAKE_CALL_CALLEE_IDENTITY   140
#define SIP_CONTROL_CMD_PROROCOL        141     /* 命令呼叫，控制协议类型 */
#define SIP_MAKE_CALL_ONBEHALF          142
#define SIP_MAKE_CALL_PICKUP_VALUE      143
#define SIP_MAKE_CALL_EXT_H264_PARAM    144 /*辅流分辨率*/
#define SIP_MAKE_CALL_DISABLE_BFCP      145/*禁止辅流，慎用，容易导致与配置冲突*/
#define SIP_MAKE_CALL_VM_ITEMID         146
#define SIP_MAKE_CALL_VERIZON_ADD_P_COM 147
#define SIP_MAKE_CALL_EMERGENCY_SRV_PRI 148     /* 紧急呼叫的server优先级 */
#define SIP_MAKE_CALL_H265_PARAM        149
#define SIP_MAKE_CALL_CUSTOM_INFO       150     /* 用于传输用户自定义的一些报文*/


/* 会议参数 256~383 */
#define SIP_CONFERENCE_CID_COUNT        256
#define SIP_CONFERENCE_CID_TAB          257
#define SIP_CONFERENCE_CID_URI          258

typedef struct
{
    sip_data_header_t overview_header;
} sip_ext_params_t;


typedef struct
{
    unsigned char control_protocol_type;
    unsigned char reserved_pad[3]; /* 四字节对齐,预留 */

    int code;                      /* 预留 */
    char reason[SIP_MAX_NAME_LEN]; /* 预留 */
}sip_cmd_common_info_t;

typedef  struct
{
    unsigned char call_type;
    unsigned char reserved_pad[3]; /* 四字节对齐,预留 */

    int code;                      /* 预留 */
    char data[SIP_MAX_NAME_LEN]; /* 预留 */
}sip_call_type_info_t;

typedef struct 
{
    unsigned char control_protocol_type;
    unsigned char reserved_pad[3]; /* 四字节对齐,预留 */

    int  mode;                     /* 预留 */
    char forward_to[SIP_MAX_URI_LEN];
}sip_cmd_call_forward_info_t;

typedef struct 
{
    unsigned char control_protocol_type;
    unsigned char reserved_pad[3]; /* 四字节对齐,预留 */

    int  mode;                     /* 预留 */
    int  target_cid;
    char refer_to[SIP_MAX_URI_LEN];
} sip_cmd_call_transfer_info_t;

typedef sip_cmd_call_forward_info_t  sip_cmd_set_call_forward_info_t;

typedef struct 
{
    unsigned char control_protocol_type;
    unsigned char reserved_pad[3]; /* 四字节对齐,预留 */

    int  mode;                     /* 预留 */
    char parter[SIP_MAX_URI_LEN];
} sip_cmd_call_conference_info_t;

typedef struct 
{
    char  old_pin[SIP_MAX_NAME_LEN];
    char  new_pin[SIP_MAX_NAME_LEN];
} sip_cmd_chang_pin_lock_info_t;

typedef struct 
{
    char  organizer[SIP_MAX_NAME_LEN];
    char    subject[SIP_MAX_NAME_LEN];
} sip_cmd_calendar_item_info_t;

typedef struct _sip_cmd_reload_account_info
{
    unsigned char control_protocol_type;
    unsigned char reserved_pad[3];  /* 四字节对齐,预留 */

    int force;                      /* 1/0 是否强制执行，即使处于通话中 */
    int random_time_min;            /* random随机时间范围低值, @exp: 5 */
    int random_time_max;            /* random随机时间范围高值, @exp: 45 */

    int code;                       /* 预留 */
    char reason[SIP_MAX_NAME_LEN];  /* 预留 */
}sip_cmd_reload_account_info_t;

typedef struct _sip_cmd_reboot_phone_info
{
    unsigned char control_protocol_type;
    unsigned char reserved_pad[3];  /* 四字节对齐,预留 */

    int force;                      /* 1/0 是否强制执行，即使处于通话中 */
    int random_time_min;            /* random随机时间范围低值, @exp: 5 */
    int random_time_max;            /* random随机时间范围高值, @exp: 45 */

    int code;                       /* 预留 */
    char reason[SIP_MAX_NAME_LEN];  /* 预留 */
}sip_cmd_reboot_phone_info_t;

typedef enum
{
    SIP_APOLLO_CONF_REQ_FOCUS,
    SIP_APOLLO_CONF_REQ_ADD_USER,
    SIP_APOLLO_CONF_REQ_DELETE_USER,
    SIP_APOLLO_CONF_REQ_MUTE_USER,
    SIP_APOLLO_CONF_REQ_UNMUTE_USER,
    SIP_APOLLO_CONF_REQ_MUTE_AUDIENCE,
    SIP_APOLLO_CONF_REQ_GET_MSG,
    SIP_APOLLO_CONF_REQ_LOCK,
    SIP_APOLLO_CONF_REQ_SETLOBBY_ACCESS,
    SIP_APOLLO_CONF_REQ_MODIFY_ROLE
}sip_apollo_conf_req_type_e;

typedef struct 
{
    unsigned char type; /*sip_apollo_conf_req_type_e*/
    unsigned char sub_req_type;             /*允许或拒绝Lobby成员加入会议，会议角色1:presenter, 0:attentee*/
    unsigned char media_id;/*media_id*/
#define SIP_APOLLO_CONF_MEDIA_BLOCK        1
#define SIP_APOLLO_CONF_MEDIA_UNBLOCK      2
#define SIP_APOLLO_CONF_MEDIA_UNBLOCKING   3
    unsigned char media_fiter;/*mute 的状态 block表示mute unblock表示Unmute unblocking表示正在举手 需要主持人同意*/
    int  code; /*错误码*/
    char user[SIP_MAX_NAME_LEN];
    char reason[SIP_MAX_NAME_LEN];
    char entity[SIP_MAX_NAME_LEN];/*Uuid*/
}sip_apollo_conf_req_result_t;

typedef struct apollo_conf_media_data
{
#define SIP_APOLLO_CONF_TYPE_AUDIO  1
#define SIP_APOLLO_CONF_TYPE_VIDEO  2
#define SIP_APOLLO_CONF_TYPE_APPLICATIONSHARING 3
    unsigned char media_type;  /*会议媒体类型,最多有2个media 如auido和video,UI需要对两个进行判断，默认值为0，表示没有*/

#define SIP_APOLLO_CONF_UNHOLD            1
#define SIP_APOLLO_CONF_HOLD              2
#define SIP_APOLLO_CONF_HOLD_FROM_MCU     3  /*成员本身hold并且MCU block*/
    unsigned char hold_status; /*会议成员hold状态，HOLD表示sendonly或inactive，即成员的hold状态*/

#define SIP_APOLLO_CONF_UNMUTE            1
#define SIP_APOLLO_CONF_MUTE              2
#define SIP_APOLLO_CONF_FLOOR             3
    unsigned char mute_status; /*会议成员mute状态，表示是否允许音频混入，block即表示Mute，unblock表示取消Mute，FLOOR 表示申请举手，该方向是client to MCU*/

#define SIP_APOLLO_MCU_UNBLOCK            1
#define SIP_APOLLO_MCU_BLOCK              2
    unsigned char mcu_direction;

#define SIP_APOLLO_BLOCK_BY_CLIENT        1
#define SIP_APOLLO_BLOCK_BY_SERVER        2
    unsigned char blockby;
    unsigned char reserve[1];

    unsigned int media_id;  /*媒体audio会话id*/

    unsigned int reserve2[4];

}apollo_conference_media_data_t;


typedef struct  
{
#define SIP_APOLLO_CONF_ROLE_DELETE         1
    unsigned char user_quit;/*会议退出操作,为1表示退出，默认为0*/
#define SIP_APOLLO_CONF_ROLE_PRESENTER    1
#define SIP_APOLLO_CONF_ROLE_ATTENDEE     2
    unsigned char role;  /*会议成员角色*/

#define SIP_APOLLO_CONF_ADD_USER_CONNECTED  1
#define SIP_APOLLO_CONF_ADD_USER_DISCONNECT  2
#define SIP_APOLLO_CONF_ADD_USER_PREPARE  3
    unsigned char user_state;/*被邀请者的状态*/

#define SIP_APOLLO_FOCUS_CONNECTED   1
#define SIP_APOLLO_FOCUS_ON_HOLD        2
    unsigned char lobby;/*lobby 大厅*/

#define SIP_APOLLO_FOCUS_CHAIRMAN_AUDIENCE    1
#define SIP_APOLLO_FOCUS_CHAIRMAN_DEMONSTRATOR   2
    unsigned char chairmam_role;
    unsigned char reserved[3];

    char display[SIP_MAX_NAME_LEN]; /*会议成员显示名*/
    char user_uri[SIP_MAX_URI_LEN]; /*会议成员uri*/
    char entity[SIP_MAX_URI_LEN]; /*会议成员uuid*/
    unsigned int reserved2[12];

    unsigned int cnt;
    apollo_conference_media_data_t media[3];/*media类型，如audio 、video、applicationsharing、chat*/
}sip_apollo_conference_user_t;

typedef struct
{
#define SIP_APOLLO_CONF_PATTERN_DEFAULT         1
#define SIP_APOLLO_CONF_PATTERN_DEMONSTRATOR    2
    unsigned char pattern;/*h会议模式,1:普通模式， 2:主席模式*/
    char conference_uri[SIP_MAX_NAME_LEN];/*会议uri*/
    char mcu_uri[SIP_MAX_NAME_LEN];/*MCU aduio-video uri*/
    int conference_number;/*会议的Number,用于IVR接入*/
    char organizer_uri[SIP_MAX_URI_LEN]; /*会议组织者uri*/
    char organizer_display[SIP_MAX_NAME_LEN]; /*会议组织者显示名*/
}sip_apollo_conference_descrip_t;

typedef struct
{
    unsigned char conf_lock;/*focus 的状态 1表示lock 0表示unlock*/
    unsigned char entry_exit_announcements_enable;/*成员加入、退出语言通告是否可以开启*/
    unsigned char reserved[5];
    char video_layout[SIP_MAX_NAME_LEN];/*会议布局*/
}sip_apollo_conference_entity_view_t;

/*map*/
typedef struct
{
    unsigned short          version;
    unsigned short          size;

    unsigned char            role;
    unsigned char            user_state;
    unsigned char            lobby;
    unsigned char            reserved[5];

    sip_map_t               display;      /* ptr= char*/
    sip_map_t               user_uri;      /* ptr= char* */
    sip_map_t               entity;  /* ptr=char* */
    sip_map_t               audio;  /*ptr=apollo_conference_media_data_t*/
    sip_map_t               video;  /*ptr=apollo_conference_media_data_t*/
    sip_map_t               applicationsharing;/*ptr=apollo_conference_media_data_t*/

    sip_map_t               reserved2[3];
} sip_apollo_conf_notify_user_t;

/************************************************************
*                     auto test                             *
************************************************************/
typedef struct
{
    int     uuid;
    int     type;
    char    value_number[SIP_MAX_NAME_LEN];//等于1存储的号码

    int     line_num;

    int     size_num;
}sip_dsskey_sub_blf_t;


typedef struct sip_disskey_index_l
{
  int  index;
  char value_num[SIP_MAX_NAME_LEN];
  int  line_id;
  int  status;
  struct sip_disskey_index_l *next;
}sip_disskey_index_t;

typedef struct
{
    int    acount;
    sip_disskey_index_t *next;
}sip_dsskey_list_t;

typedef struct
{
    int    uuid;
    int    type;
    char   value_number[SIP_MAX_NAME_LEN];
    int   line_num;
    int   size_num;
    int   key_index;
}sip_dsskey_unsub_blf_t;

typedef struct
{
    int    lid;
    char   m7_head[SIP_MAX_NAME_LEN];
    char   m7_data[SIP_MAX_NAME_LEN];
}sip_m7_config_t;

/*message检测结果上报*/
typedef struct
{
    int type;
    float cpu_usage;
    int  mtotal;
    int  mfree;
    int  msip;
}sip_check_info_t;

#ifdef __cplusplus
}
#endif

#endif/* __SERVICE_SIP_DEFINE_H__ */

