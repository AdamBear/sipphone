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
*    �޸�ʱ��:              �޸���  :
*    ԭ�汾��:              �°汾��:
*    �޸�����:
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
#define SIP_MAX_NAME_LEN                    128     /* �������128�ַ� */
#define SIP_MAX_PATH_LEN                    512     /* �ļ�·������󳤶� */
#define SIP_MAX_URI_LEN                     512     /* SIP URI���ȣ���"sip:xxx@yyy.com:5060" */
#define SIP_MAX_MSGKIT_BODY_LEN             3072    /* SIP->UI, UI->SIP  body msg max len*/
#define SIP_MAX_XML_BODY_LEN                4096    /* xml body����󳤶� */

#define SIP_MAX_SRTP_KEY_LEN                96      /* SRTP KEY���� */
#define SIP_MAX_CODEC_NAME_LEN              32      /* codec���ֳ��ȣ���"PCMU" */
#define SIP_MAX_RTCP_FB_LEN                 48      /* rtpc feedback len */

#define SIP_MAX_ADDRESS_LEN                 48      /* IP��ַ���� */
#define SIP_MAX_HEADER_NAME_LEN             32      /* SIP����Header��󳤶� */
#define SIP_MAX_HEADER_VALUE_LEN            128     /* SIP����Header������󳤶� */

#define SIP_MAX_CONF_LINE                   12      /* ���֧�ֻ���·�� */

#define SIP_MAX_CODEC_LIST                  8       
#define SIP_MAX_MEDIA_LIST                  4       

#define SIP_MAX_UUID_LEN                    32      /* ���uuid���� */
#define SIP_MAX_LIFETIME_LEN                32      /* srtp lifetime ��������󳤶� */


#define SIPUA_MAX_TIME_ZONE_LEN             128     /* time zone ���� */
#define SIPUA_MAX_ACCESS_TYPE_LEN           32      /* access type ���� */
#define SIP_MAX_SSRC_CNAME_LEN              256     /*Cname��󳤶�*/
#define SIP_MAX_RTP_KEY_LEN                 128     /*���������rtp key��rtcp key��󳤶�*/
/*
 *------------------------------------
 *      blf/bla define
 *------------------------------------
 */

    /* BLF״ֵ̬��Χ��0~99 */
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

    /* 50~100����BLF�ͻ����Ʒ�Χ */
#define SIP_BLF_STATUS_TALKING_ME           50  /* CDC�ͻ����� */
#define SIP_BLF_STATUS_RINGING_ME           51
#define SIP_BLF_STATUS_CALLING_ME           52
#define SIP_BLF_STATUS_HELD_ME              53
#define SIP_BLF_STATUS_PARKED               54

    /* Broadsoft SCA״ֵ̬��Χ��100~199 */
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

    /* BLA Draft״ֵ̬��Χ��200~299 */
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
#define SIP_CALL_ID_NONE                    0  /* ��ʾû���κ����� */
#define SIP_CALL_ID_VIDEO_URL               1  /* string,like "http://1.2.3.4/c.h264",����������Ƶ���� */
#define SIP_CALL_ID_REASON                  2  /* string,like "user busy",�����ϱ��Է�����reason���� */
#define SIP_CALL_ID_EVENT                   3  /* string,like "Event:mic=x,speaker=y",�������ݣ������ϱ�Tiptel PBX�����¼� */
#define SIP_CALL_ID_SKILL_NAME              4  /* string,like "english",�������ݣ�����LCD��ʾ"English" */
#define SIP_CALL_ID_BW_CALL_CENTER          5  /* string,xml body������Broadworks call center���� */
#define SIP_CALL_ID_EMERGENCY_CALL          6  /* string,like "type normal" ��ʾ��ͨͨ��, "type emergency" ��ʾ����ͨ�� */
#define SIP_CALL_ID_PHONE_TEXT              7  /* string,like "helloworld",�������ݣ�����LCD��ʾ�Է�����PHONE_DISPLAY_TEXT���� */
#define SIP_CALL_ID_CALLID                  8  /* string,like "zd13dc@10.2.3.1",�����ϱ�SIP����ʹ�õ�Call-ID */
#define SIP_CALL_ID_INFO_TEXT               9  /* string,xml body,����ͨ�����յ�INFO��Ϣ���½�����ʾ���� */

    /* sip_channel_t->channel_type */
#define SIP_CALL_CHANNEL_TYPE_AUDIO         1
#define SIP_CALL_CHANNEL_TYPE_VIDEO         2
#define SIP_CALL_CHANNEL_TYPE_EXTVIDEO      3
#define SIP_CALL_CHANNEL_TYPE_H224          4
#define SIP_CALL_CHANNEL_TYPE_FAX           5
#define SIP_CALL_CHANNEL_TYPE_FILE          6
#define SIP_CALL_CHANNEL_TYPE_BFCP          7

    /* sip_channels_t.reserved.type, �� sip_channels_map_type_e ͬ�� */
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
#define SIP_CALL_HOLD_DEFAULT               0        /* ����ͨ��״̬ */
#define SIP_CALL_HOLD_BY_LOCAL              (1<<0)   /* ����HOLD�Է� */
#define SIP_CALL_HOLD_BY_REMOTE             (1<<1)   /* �Է�HOLD���� */

    /* call out call_mask define */
#define SIP_CALL_MASK_NORMAL                0
#define SIP_CALL_MASK_VIDEO                 (1<<0) /* 0x00000001 ��־��ǰ�Ƿ�����Ƶ���� */
#define SIP_CALL_MASK_INTERCOM              (1<<1) /* 0x00000002 ��־��ǰ�Ƿ���intercom���� */
#define SIP_CALL_MASK_HUAWEI_EMERGENCY      (1<<2) /* 0x00000004 ��־��ǰ�Ƿ��ǻ�Ϊemergency call���� */
#define SIP_CALL_MASK_VOICE_MAIL            (1<<3) /* 0x00000008 ��ǵ�ǰ�Ƿ�Ϊvoice mail call */
#define SIP_CALL_MASK_TYPE_NUMBER           (1<<4) /* 0x00000010 number call */
#define SIP_CALL_MASK_E911_CALL             (1<<5) /* 0x00000020 E.911 call (verizon����)*/
#define SIP_CALL_MASK_LYNC_CALLPARK         (1<<6)
#define SIP_CALL_MASK_LYNC_CONF_MCU         (1<<7)
#define SIP_CALL_MASK_LOOPBACK_CALL         (1<<8) /* 0x00000100 Loopback call*/
#define SIP_CALL_MASK_BROADSOFT_SILENT_BARGIN (1<<9) /* 0x00000200 brosoft silent bargin */
#define SIP_CALL_MASK_EMERGENCY             (1<<10) /* 0x00000400 emergency call */
#define SIP_CALL_MASK_UC_INFO               (1<<11) /* 0x00000800 Я��UC���� */
#define SIP_CALL_MASK_TRANS_CODING          (1<<12) /* 0x00001000 BugID=34949 */
#define SIP_CALL_MASK_APOLLO_CONF_MCU       (1<<13) /*0x00002000 ��־��ǰ�Ƿ���invite apollo mcu����*/
#define SIP_CALL_MASK_VERIZON_BARGIN        (1<<14) /* 0x00004000 verizon bargin */

    /* call establish call_flag */
#define SIP_CALL_FLAG_CAN_HOLD              (1<<0)
#define SIP_CALL_FLAG_LYNC_CONF             (1<<1)      /* Lync����ͨ�� */
#define SIP_CALL_FLAG_TYPE_BTOE             (1<<2)      /* Lync BTE ��Ƶͨ�� */
#define SIP_CALL_FLAG_HAVE_FECC             (1<<3)      /* ��ǰͨ����fecc */
#define SIP_CALL_FLAG_LYNC_PRIVATE_LINE     (1<<4)
#define SIP_CALL_TYPE_LYNC_CPS              (1<<5)
#define SIP_CALL_TYPE_LYNC_VM               (1<<6)      /*Lyncͨ�������������������*/
#define SIP_CALL_TYPE_LYNC_CONF_ATTENDANT   (1<<7)      /*����Lync����id���������*/
#define SIP_CALL_FLAG_SERVER_CONF           (1<<8)      /* �����������־λ */
#define SIP_CALL_FLAG_ENCRYPT               (1<<9)      /* ����ͨ�����ͻ����ƣ���������� */
#define SIP_CALL_FLAG_ICE                   (1<<10)  	/* ICE ͨ�� */
#define SIP_CALL_TYPE_LYNC_RESPONSE_GROUP   (1<<11) 	/*Lync ������Ӧ���ͨ������*/
#define SIP_CALL_FLAG_HAVE_BFCP             (1<<12) 	/*����BFCPЭ������*/
#define SIP_CALL_FLAG_APOLLO_CONF           (1<<13)     /*Apollo ����ͨ����־λ*/
#define SIP_CALL_FLAG_LOOPBACK              (1<<14)     /* ������Loopbackͨ�� */

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

    /* ���SIP_MSG_UPDATE_BLFLISTʹ�ã���Ϊlparam */
#define SIP_BLF_LIST_NOTIFY_TERMINATED      0   /* ��ʾnotify�ѽ�����UIӦ����� */
#define SIP_BLF_LIST_NOTIFY_IN_MSG          1   /* ��ʾnotify����Ϣ���� */
#define SIP_BLF_LIST_NOTIFY_IN_FILE         2   /* ��ʾnotify��д���ļ��� */

    /*lync conference call mask*/
#define SIP_CALL_LYNC_CONF_NORMAL           0
#define SIP_CALL_LYNC_CONF_VIDEO            (1<<0)
#define SIP_CALL_LYNC_CONF_IM               (1<<1)
#define SIP_CALL_LYNC_CONF_GROUP            (1<<2)

typedef enum
{
    SIP_CONTROL_FROM_DEFAULT,                       /* ����Ĭ��ֵ */
    SIP_CONTROL_FROM_CSTA,                          /* CSTA */
    SIP_CONTROL_FROM_TPCP,                          /* Lync����TPCP */
    SIP_CONTROL_FROM_LYNC_AUTOMATION_TEST,          /* Lync�Զ������� */
    SIP_CONTROL_FROM_APOLLO,                        /* Apollo������ */
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
    SIP_LYNC_ROUTING_E911_ACTIVE,        /* ���� e911 flag Ϊ active��ͬʱ���� route Ϊ FWD OFF������ route ʱ���� inactive */
    SIP_LYNC_ROUTING_E911_INACTIVE       /* ���� e911 flag Ϊ inactive��ͬʱ���� route Ϊ FWD OFF */
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
    SIP_LYNC_CONTACT_PRES_SUB_POLLING,          /* һ���Զ��ģ�expires Ϊ 0 */
    SIP_LYNC_CONTACT_PRES_SUB_PRESISTENT        /* ���ö��ģ�expires �� 0 */
}sip_lync_contact_pres_sub_expries_type_e;


typedef enum
{
    SIP_LYNC_CONTACT_FULL_LIST,             /* �ϱ���Ӧ��body��һ��Ϊ�������б�UCS��������б�Ϊ�� */
    SIP_LYNC_CONTACT_DELTA_LIST,            /* �ϱ�NOTIFY��body��һ��Ϊ��ϵ���б��������� */
    SIP_LYNC_CONTACT_PRES,                  /* �ϱ���ϵ��״̬��Ϣ����ϵ������ͨ��UI�����sub_id���� */
} sip_lync_contact_info_type_e;

typedef enum
{
    SIP_LYNC_BTOE_PAIRING_FAILED,          /*�������ʧ��*/
    SIP_LYNC_BTOE_STATUS_UN_PAIR,          /* ����δ���*/ 
    SIP_LYNC_BTOE_STATUS_PAIR_CODE_CHECK,  /*���������ƥ��*/
    SIP_LYNC_BTOE_STATUS_PAIRED,           /* ��������ԣ�δ��½��*/  
    SIP_LYNC_BTOE_STATUS_SIGN_IN,           /* ��������ԣ��ѵ�½��*/  
    SIP_LYNC_BTOE_GETTING_CFG,             /* ������������ڻ�ȡ����*/
    SIP_LYNC_BTOE_CFG_CANCEL,              /*Lync�ͻ��˵���ȡ��*/
    SIP_LYNC_BTOE_CLIENT_SIGN_OUT,      /*Lync�ͻ��˵ǳ�*/
}sip_lync_BToE_status_e;

typedef enum 
{
    SIP_CALL_OUT_DEFAULT,
    SIP_CALL_OUT_BTOE_ACCOUNT,             /* ������ϵ�˺��� */
    SIP_CALL_OUT_BTOE_ANONYMOUS            /* ������������ */
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
    unsigned short type;    /* �������� */
    unsigned short size;    /* ���ݴ�С */
    void *ptr;              /* �������� */
}sip_map_t;

typedef struct  
{
 /* ��cfgserver����һ�£�ע��������Ҫ����READY��SIP�Ż�ʹ���ⲿ���õ�������� */
#define SIP_NET_STATUS_OFF              0        /*network is hang*/
#define SIP_NET_STATUS_READY         2        /*network is ready*/

    int network_status;             /* ����״̬(��ǰδʹ�ã���0����) */
    int has_vpn;                    /* �Ƿ���ʹ��IPV4 VPN */
    int has6_vpn;                   /* �Ƿ���ʹ��IPV6 VPN(��λ��ǰδʹ�ã���0����) */
    struct in_addr  a4_addr;        /* IPV4��ַ */
    struct in_addr  subnet_addr;    /* IPV4��ָ������ */
    struct in6_addr a6_addr;        /* IPV6��ַ */
    struct in_addr  vpn_addr;       /* IPV4 VPN��ַ */
    struct in6_addr vpn6_addr;      /* IPV6 VPN��ַ(��ǰδʹ�ã���0����) */
    struct in_addr  dns_addr[2];    /* IPV4��DNS��ַ */
    struct in6_addr dns6_addr[2];   /* IPV6��DNS��ַ */
    char   mac_addr[32];            /*MAC��ַ*/
    char   access_type[32];         /* access type���ͣ���ǰ�����ᳬ��32���ַ� */
    char   reserved[32];            /* Ԥ�� */
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

/* device pairing������pairing url��pairing code */
typedef struct
{
    char pair_url[SIP_MAX_URI_LEN];
    char pair_code[SIP_MAX_UUID_LEN];
}sip_dev_pairing_info_t;

/* SIP����Headerͷ������ݣ�����"User-Agent: Yealink xxx" */
typedef struct
{
    char  name[SIP_MAX_HEADER_NAME_LEN];    /* ����������"User-Agent" */
    char value[SIP_MAX_HEADER_VALUE_LEN];   /* ����������"Yealink xxx" */
}sip_header_info_t;

/* SIP�յ�SMSʱ�ϱ������� */
typedef struct
{
    int             type;                           /* sms���ͣ��������𲿷ֿͻ����� */
    sip_user_info_t user_info;                      /* �Է��û���Ϣ */
    char            p_flash[SIP_MAX_NAME_LEN];      /* P_FLASH_SMS���� */
    char            xml_body[SIP_MAX_XML_BODY_LEN]; /* xml body */
}sip_sms_info_t;


/* sip�յ�re-inviteʱ�ϱ������� */
typedef struct
{
    int tid;                            /* re-invite��tid */

#define SIP_CALL_REINVITE_FLAG_AUDIO_UPGRADE_TO_VIDEO            1 << 0  /* ��Ƶ����Ƶ */
#define SIP_CALL_REINVITE_FLAG_BW_EMERGENCY_RING_BACK            1 << 1  /* broadworks �������лغ� */

    int type;                           /* re-invite������ */
} sip_reinvite_t;

/* �ϱ�SIP�յ�MWIʱ�Ľṹ�� */
typedef struct
{
    int new_msg;                        /* ����Ϣ���� */
    int old_msg;                        /* ����Ϣ���� */
    int new_urgent;
    int old_urgent;
    int new_fax;
    int old_fax;
    int msg_waiting;                    /* �Ƿ�message waiting */
    char msg_account[SIP_MAX_URI_LEN];     /* �������·���notify body */
} sip_mwi_info_t;

/* verizon Visual Voice Mail (VVM)�е���Ϣ���͡�1-New Message��2-Mailbox Update Message*/
#define SIP_MSG_VVM_TYPE_NM     1
#define SIP_MSG_VVM_TYPE_MBU    2

/* �ϱ�verizon Visual Voice Mail (VVM) �е� "New Message" */
#define SIP_VVX_ID_MAX_LEN      22
#define SIP_VVX_DATE_MAX_LEN    23

typedef struct
{
    int  unheard_msg;                   /* ����Ϣ/δ����Ϣ���� */
    char id[SIP_VVX_ID_MAX_LEN];        /* ��ϢID������ȡ��Ϣ */
    char type;                          /* ��Ϣ����;v = Voice;o = Video;f = Fax;i = Infotainment;e = ECC */
    char msg_sender[SIP_MAX_URI_LEN];   /* ��Ϣ�Ĳ����� */
    char date[SIP_VVX_DATE_MAX_LEN];    /* ʱ������� */
    int  len;                           /* ��Ϣ�ĳ��ȣ�����Ϊ��λ��������5s��δ��������Ϣ */
} sip_vvm_nm_t;

/* �ϱ�verizon Visual Voice Mail (VVM) �е� "Mailbox Update Message" */
typedef struct
{
    int unheard_msg;                    /* ����Ϣ/δ����Ϣ���� */
    char reserved[32];                  /* Ԥ��32���ֽڣ����������չ */
} sip_vvm_mbu_t;

/* ����ԭ��ṹ */
typedef struct
{
    int  uuid;                          /* Ψһ��ʾ��������������Ϣ��Դ */
    int  code;                          /* ������,Ϊ0��ʾ�ɹ� */
    char reason[SIP_MAX_NAME_LEN];      /* ����ԭ�� */
}sip_reason_info_t;

/* UI�ظ�reinvite */
typedef struct
{
    int tid;                            /* reinvite �����ID */
    sip_reason_info_t response;         /* �������Լ�ԭ�� */
} sip_reinvite_info_t;

/* �ϱ�Dsskey��״̬������BLF/BLA�� */
typedef struct
{
    int index;                          /* �ڼ���DSSKEY */
    int status;                         /* ��KEY��״̬ */

    #define SIP_BLF_AUTO_PICKUP_MASK            1 << 0  /* mask it can be pickup */
    #define SIP_BLF_WILDIX_LED_MODE0_MASK       1 << 1
    #define SIP_BLF_WILDIX_LED_MODE1_MASK       1 << 2
    int flag; /* ȡ����ֵ */
    void *js;/* ע�⣺V80�̼��汾UI��ע�͵����������V81��֮��Ĺ̼��汾��ʹ�ã�����ӿڼ��������� */
}sip_dsskey_info_t;

/* �ϱ�Dsskey BLF����Ϣ�����ڽ�����ʾ��ЩDSSKEY����pickup */
typedef struct
{
    sip_dsskey_info_t key;
    char              local_name[SIP_MAX_NAME_LEN];
    char              local_target_uri[SIP_MAX_URI_LEN];
    char              remote_name[SIP_MAX_NAME_LEN];
    char              remote_target_uri[SIP_MAX_URI_LEN];
}sip_dsskey_blf_info_t;

/* GCS�յ�NOTIFYʱ�ϱ��Ľṹ�� */
typedef struct
{
    char user_name[SIP_MAX_NAME_LEN];
    char      body[SIP_MAX_XML_BODY_LEN];
}sip_gcs_info_t;

/* SCAÿ��line��״̬ */
typedef struct
{
    int appearance_index;
    int appearance_state;           /* SIP_BLA_STATUS_UNKOWN */
    sip_user_info_t remote_user_info;
} sip_sca_status_t;


typedef struct
{
    int call_index;                     /* ͨ����·��index */
    char conf_uri[SIP_MAX_URI_LEN];     /* ͨ���Ļ���uri */
} sip_bw_conf_index_t;


/* ACD״̬ */
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

    int             answer_after;       /* -1:δָ����0:����������>0:ָ��ʱ����Զ����� */
    unsigned char   is_emergency;
    unsigned char   force_auto_answer;
    unsigned char   force_no_forward;   /* ǿ�Ʋ�forward */

        #define SIP_CALL_AUTO_ANSWER_DEVICE_UNKNOWN     0   /* û�н���������ֶ� */
        #define SIP_CALL_AUTO_ANSWER_DEVICE_ACTIVE      1   /* ����ʹ���е��豸 */
        #define SIP_CALL_AUTO_ANSWER_DEVICE_SPEAKER     2   /* ������ */
        #define SIP_CALL_AUTO_ANSWER_DEVICE_HEADSET     3   /* ���� */
        #define SIP_CALL_AUTO_ANSWER_DEVICE_HANDSET     4   /* �ֱ� */
    unsigned char auto_answer_device;   /* ȡ����ֵ */

        #define SIP_CALL_P_ANTO_ANSWER_UNKNOWN      0   /* û�н���������ֶ� */
        #define SIP_CALL_P_ANTO_ANSWER_NORMAL       1   /* normal */
        #define SIP_CALL_P_ANTO_ANSWER_URGENT       2   /* urgent */
        #define SIP_CALL_P_ANTO_ANSWER_IMPERIOUS    3   /* imperious */
        #define SIP_CALL_P_ANTO_ANSWER_SILENT       4   /* silent */
    unsigned char p_auto_answer;        /* ȡ����ֵ */

        #define SIP_CALL_ENABLE_MIC_UNKNOWN         -1  /* û�н���������ֶ� */
        #define SIP_CALL_ENABLE_MIC_FALSE           0   /* 0 */
        #define SIP_CALL_ENABLE_MIC_TRUE            1   /* 1 */
    char enable_mic;                    /* ȡ����ֵ */

        #define SIP_CALL_ENABLE_SPEAKER_UNKNOWN     -1  /* û�н���������ֶ� */
        #define SIP_CALL_ENABLE_SPEAKER_FALSE       0   /* 0 */
        #define SIP_CALL_ENABLE_SPEAKER_TRUE        1   /* 1 */
    char enable_speaker;                /* ȡ����ֵ */

        #define SIP_CALL_INCOMING_TYPE_NORMAL                (1<<0)   
        #define SIP_CALL_INCOMING_TYPE_LYNC_CONF             (1<<1)   /* lync conference call*/
        #define SIP_CALL_INCOMING_TYPE_LYNC_BTOE             (1<<2)   /* lync btoe call*/
        #define SIP_CALL_INCOMING_TYPE_LYNC_PRIVATE_LINE     (1<<4)   /* lync private line */
        #define SIP_CALL_INCOMING_TYPE_LYNC_CPS              (1<<5)   /* lync call park service call */
        #define SIP_CALL_INCOMING_TYPE_APOLLO_CONF           (1<<6)   /* apollo conference call*/
        #define SIP_CALL_INCOMING_TYPE_LOOPBACK              (1<<7)   /* loopback call*/
    unsigned char inc_call_type;                /* ȡ����ֵ */

        #define SIP_CALL_NO_FORWARDING                      0
        #define SIP_CALL_FORWARDING                         1
        #define SIP_CALL_LYNC_FORWARDING_BOSS                    2
        #define SIP_CALL_LYNC_FORWARDING_DELEGATE                3
        #define SIP_CALL_LYNC_FORWARDING_TEAM_LEADER             4
        #define SIP_CALL_LYNC_FORWARDING_TEAM_MEMBER             5
        #define SIP_CALL_LYNC_FORWARDING_RESPONSE_GROUP          6
    unsigned char   retarget_reason;      /* ȡ����ֵ */

        #define SIP_CALL_INCOMING_IS_AUDIO    0
        #define SIP_CALL_INCOMING_IS_VIDEO     1
    unsigned char   inc_video; /*��Ƶ����*/

        #define SIP_CALL_INCOMING_MEDIA_NORMAL   0
        #define SIP_CALL_INCOMING_MEDIA_FILE     1
    unsigned char   inc_media_type;/*V81SP2 UI�ѱ��룬SIP�ڲ�δʹ��*/

    unsigned char sca_appearance_index;

    unsigned char   reserved_pad[4];/*reserved_pad[0] ȡ����ֵ*/

    sip_map_t       display_name;
    sip_map_t       user_name;
    sip_map_t       server_name;

    sip_map_t       diversion_diaplay_name;
    sip_map_t       diversion_user_name;
    sip_map_t       diversion_server_name;

    sip_map_t       to_diaplay_name;
    sip_map_t       to_user_name;
    sip_map_t       to_server_name;

    sip_map_t       alert_local;        /* �������� */
    sip_map_t       alert_web;          /* ��ҳƥ������ */
    sip_map_t       alert_url;          /* �������� */
    sip_map_t       pict_info;
    sip_map_t       from_user_name;
    sip_map_t       diversion_line;     /* NFON�ж��� */
    sip_map_t       lync_on_behalf;
    sip_map_t       doorline_uri;
    sip_map_t       doorline_dtmf;
    sip_map_t       custom_info;
    sip_map_t       apollo_conf_data;

    sip_map_t       reserved[1];
}sip_call_identify_info_t;

typedef struct
{
    char call_id[SIP_MAX_HEADER_VALUE_LEN];     /* SIP������Call-ID��һ��Ϊ����ַ��� */
    char remote_tag[SIP_MAX_HEADER_VALUE_LEN];  /* SIP�����ר�� */
    char local_tag[SIP_MAX_HEADER_VALUE_LEN];   /* SIP�����ר�� */
}sip_call_proto_info_t;

typedef struct
{
    char call_id[SIP_MAX_HEADER_VALUE_LEN];     /* SIP������Call-ID��һ��Ϊ����ַ��� */
    char remote_tag[SIP_MAX_HEADER_VALUE_LEN];  /* SIP�����ר�� */
    char local_tag[SIP_MAX_HEADER_VALUE_LEN];   /* SIP�����ר�� */
    char from[SIP_MAX_URI_LEN];                 /* SIP������From��ַ */
    char to[SIP_MAX_URI_LEN];                   /* SIP������To��ַ */
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
#define SIP_UACSTA_TYPE_CONNECT         0           /* ���� uaCSTA ���Ƶ����� */
#define SIP_UACSTA_TYPE_CONTROL         1           /* ���� uaCSTA ���Ƶ��¼� */
#define SIP_UACSTA_TYPE_DISCON          2           /* ���� uaCSTA ���ƵĶϿ� */

    int             type;                           /* �������ͣ����ã� */
    int             tid;                            /* tid ����id */
    int             body_len;                       /* body�Ĵ�С */
    char            body[SIP_MAX_XML_BODY_LEN];     /* ��Ϣ��body��post��UIʱֻ��body_len�Ĵ�С */
}sip_uacsta_req_info_t;

/* Я�������������ϱ�Զ�˿������INFO��ʽ�������� */
typedef struct
{
    int             tid;                            /* tid ����id */
    int             body_len;                       /* body�Ĵ�С */
    char            body[SIP_MAX_XML_BODY_LEN];     /* ��Ϣ��body��post��UIʱֻ��body_len�Ĵ�С */
}sip_uc_req_info_t;

/* Я�������������ϱ�Զ�˿������NOTIFY��ʽ������������INFO�ṹ�� */
typedef sip_uc_req_info_t   sip_uc_req_notify_t;


/* Я������������UI����INFO */
typedef struct
{
    int type;                                       /* �������ͣ����ã�*/
    int tid;                                        /* tid ����id */
    int uuid;                                       /* uuid*/
    char status[SIP_MAX_URI_LEN];                   /* �ϱ���������״̬*/
    char cmd[SIP_MAX_URI_LEN];                      /* hold/unhold������Ҫʹ��cmd�ڵ� */
    char cause[SIP_MAX_URI_LEN];                    /* �ϱ�������ʧ��ԭ��*/
    char calltag[SIP_MAX_NAME_LEN];                 /* �ϱ�������������*/
    char reserve[2 * SIP_MAX_URI_LEN];              /* Ԥ���ֶ� */
}sip_uc_req_send_info_t;

/* 
* ����kpml��UI��¼did��tag�����ڷ���kpml notifyʹ��(����dtmf, ���볬ʱ���ر�kpml���notify)
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
    int payload;                        /* ����, like 109 */
    int samplerate;                    /* ������ */
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

    unsigned char   media_direction;/* ý��RTP���ͷ�����sendrecv/sendonly... */
    unsigned char   rtp_ptime;      /* ptime */

    unsigned char   rtpev_payload;  /* rtp event payload, like dtmf 101 */
#define SRTP_UNENCRYPTED_RTP    (1<<0)
#define SRTP_UNENCRYPTED_RTCP   (1<<1)
#define SRTP_NO_AUTH            (1<<2)
    unsigned char   srtp_flag;      /* ʹ��λ��ʾSRTP�Ƿ񲻼���RTP��������RTCP����Я��Auth Tag */

    unsigned        bandwidth;      /* ���� */

    sip_codec_t     codec; /* codec param */

    char            srtp_key[SIP_MAX_SRTP_KEY_LEN]; /* ����unencryped���Ժ�Ԥ����96���ֽ��Ѿ��޷������ϱ��������Ҫ���srtp_flag */
    char            rtcp_fb [SIP_MAX_RTCP_FB_LEN];

    signed char     looback_type;
    signed char     loopback_role;
    signed char     loopback_pkt_mode;
    signed char     loopback_pkt_dynamic_pl;

    int             dtmf_samplerate;   /* ��Ŀǰ���������ƽ̨��Int�������ֽڣ�Ӧ��û��ǰ��������� */
    unsigned char   cvo_identifier;    /* cvo�ı�ʶ�룺�μ�http://www.potaroo.net/ietf/idref/rfc5285/ */
    char            reserved_ex[7];    /* Ԥ����չ */
}sip_media_t;

/*
 * logic channel (both local & remote medial data)
 */
typedef struct
{
    unsigned char rtcp_rsize_support;                   /*�Ƿ�֧��rtcp-rsize*/
    unsigned char reserved[3];
    uint32_t      ssrc_id;                              /*��Ӧ��ssrc��ssrc id��rfc5576�ж���Ϊ32λ�޷������� */
    char          ssrc_cname[SIP_MAX_SSRC_CNAME_LEN];   /*��Ӧ��ssrc�е�cname */
    char          rtp_key[SIP_MAX_RTP_KEY_LEN];
    char          rtcp_key[SIP_MAX_RTP_KEY_LEN];

    int           reserved_int[4];
    char          reserved_ext[32];                     /*�����ȷ��Ԥ������*/
}sip_media_ext_t;

typedef struct
{
    int             channel_type;

    sip_media_ext_t local_media_ext;                    /* audio ������� */
    sip_media_ext_t remote_media_ext;                   /* video ������� */

}sip_channel_ext_t;

typedef struct
{
    int             channel_type;

    sip_media_t     local_media;    /* ����ý��������ڱ��� */
    sip_media_t     remote_media;   /* Զ��ý��������ڽ��� */
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
#define SIP_RINGBACK_TYPE_NORMAL                0   /* ��ͨ���� */
#define SIP_RINGBACK_TYPE_RB_MUSIC              1   /* ���� */
#define SIP_RINGBACK_TYPE_QUEUED                2   /* 182 queued (call waiting) */
#define SIP_RINGBACK_TYPE_HW_ACD_LOGIN          3   /* ��ΪACDǩ��ɹ� */
#define SIP_RINGBACK_TYPE_PREVIEW               4   /* ��Ƶ����Ԥ�� */
#define SIP_RINGBACK_SILENT                     5   /* ������ */
    int type;
    sip_channel_t audio;     /* ����ʱ�����ͨ��ý����Ϣ */
    sip_channel_t video;    /*��Ƶ������Ҫ֧����Ƶ����*/
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
    sip_rtcp_key_t ext_video;         /* ext_video rtcp key pair����δʹ�ã� */

    sip_rtcp_key_t reserved;          /* ��δʹ�� */
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
    unsigned char   allow_simultaneous_ringing;             /* �Ƿ�����ͬʱ���� */
    unsigned char   allow_call_forwarding;                  /* �Ƿ�����fwd���������ϵ�� */
    unsigned char   enable_call_park;                       /* �Ƿ�����callpark */
    unsigned char   enable_call_transfer;                   /* �Ƿ�����transfer */
    unsigned char   enable_delegation;                      /* �Ƿ�����fwd������ */
    unsigned char   enable_teamcall;                        /* �Ƿ������ŶӺ��� */

#define SIP_LYNC_VDM_ONPREM                       0         /* OnPrem */
#define SIP_LYNC_VDM_ONLINE                       1         /* Online */
#define SIP_LYNC_VDM_ONLINE_BASIC                 2         /* OnlineBasic */
#define SIP_LYNC_VDM_ONPREM_ONLINE_HYBRID         3         /* OnPremOnlineHybrid */
    unsigned char   voice_deployment_mode;                  /* ָʾ�ͻ��˵�ǰ���ӵ�PSTN���� */

    /* ���²�����ΪԤ�� */
    unsigned char   enable_malicious_call_trace;            /* �Ƿ����ö�����и���(Ԥ��) */
    unsigned char   enable_bw_policy_override;              /* �Ƿ����ô�����Ը���(Ԥ��) */
    unsigned char   uc_enabled;                             /* Ignored(Ԥ��) */
    unsigned char   reserved_pad[2];                        /* ���ֽڶ��� */
} sip_lync_uc_policy_t;


typedef struct
{
    unsigned char   allow_ip_audio;             /* Specifies whether IP Audio functionality is enabled on the client */

    /* ���²�����ΪԤ�� */
    unsigned char   allow_ip_video;             /* Specifies whether IP Video functionality is enabled on the client */
    unsigned short  audio_bit_rate;             /* Set the bit rate for Audio */
    unsigned short  video_bit_rate;             /* Set the bit rate for Video */
    unsigned short  meeting_size;               /*  Maximum number of attendees allowed in meetings */
    unsigned char   allow_anon_participants;    /* Boolean value that allows anonymous participants to join the meeting */
    unsigned char   require_passcode;           /* Controls whether a pass code is required to join dial-in conferences */
    unsigned char   enable_pstn_conferencing;   /* Enable conferencing audio call over a PSTN network */
    unsigned char   trusted_conf_pin_required;  /* Require that trusted conferencing PIN be specified to join the conference */
    unsigned char   reserved_pad[8];            /* Ԥ�������ֽڶ��� */
} sip_lync_meeting_policy_t;

typedef struct
{
    unsigned char   e911_enable;                    /* �Ƿ����� E911 */
    unsigned char   loc_for_e911;                   /* λ����Ϣ�Ƿ�ֻ���ڽ������� */
    unsigned short  loc_refresh_hour;               /* ���»�ȡλ����Ϣ�ļ��ʱ�䣬��λСʱ��1-12 */

#define SIP_LYNC_LOCATION_REQUIRED_YES         0    /* ��Ҫλ����Ϣ */
#define SIP_LYNC_LOCATION_REQUIRED_NO          1    /* ����Ҫλ����Ϣ */
#define SIP_LYNC_LOCATION_REQUIRED_DISCLAIMER  2    /* ��Ҫ��ʾλ����Ϣδ���õľ��� */
    unsigned char   loc_required;                   /* ����ͻ����޷�ͨ��LIS��ȡ��λ����Ϣ�����ݸ�ֵ����Ӧ����ʾ���� */

    unsigned char   reserved_pad[3];                /* Ԥ�������ֽڶ��� */
}sip_lync_location_policy_t;

typedef struct
{
    char   policy_flag;                    /* ����Ƿ��ĵ�ipPhonePolicy������ֵ0,1 */

/* �����������δ���ĵ�������ΪSIP_LYNC_PROVISION_VALUE_UNKNOW����ʹ�û������� */
#define  SIP_LYNC_PROVISION_VALUE_UNKNOW    -1
    char   enable_one_touch_voicemail;     /* һ��ʽ�������� */
    char   date_time_format;               /* ʱ���ʽ */
    char   enable_device_update;           /* �����豸���� */
    char   enable_exchange_calendaring;    /* ����ExchangeCalendaring */
    char   enable_btoe;                    /* ����BTOE */
    char   enable_power_save_mode;         /* ����ʡ��ģʽ */
    int    power_save_during_office_timeout;   /* �ϰ�ʱ�仰������Ծ������ʡ��ģʽʱ�� */
    int    power_save_post_office_timeout;     /* �°໰������Ծ������ʡ��ģʽʱ�� */
    int    user_dial_timeout;              /* ���ŵȴ���ʱʱ�� */
#define    BTOE_PAIRING_MODE_AUTO    0
#define    BTOE_PAIRING_MODE_MANUAL   1
    char   btoe_pairing_mode;
    char   reserved_pad[16];               /* Ԥ�������ֽڶ��� */
}sip_lync_ipphone_policy_t;

typedef struct
{
    unsigned short  min_pin_len;                    /* ��СPIN���ȣ�4-15 */
    unsigned short  phone_timeout;                  /* ��ʱ����ʱ�䣬��λ�� 0-1440 */
    unsigned char   enforce_pin_lock;               /* �Ƿ�ǿ���豸���� */
    unsigned char   max_pin_retry;                  /* �������볢�Դ�����3-10�����Ϊ0��ʹ��Ԥ��Ĭ��ֵ */

    unsigned char   reserved_pad[6];                /* Ԥ�������ֽڶ��� */
}sip_lync_phone_lock_policy_t;

typedef struct
{
    unsigned int    hotdesking_timeout;             /* �л� CAP �˺ŵĳ�ʱʱ�䣬��λ�룬��С30������� */
    unsigned char   enable_hotdesking;              /* ���� CAP �Ƿ�֧�� hotdesking���������Ƿ��ṩ��ͨ�˺��л� */
    unsigned char   common_area_phone;              /* �����Ƿ��� Common Area phone.*/

    unsigned char   reserved_pad[2];                /* Ԥ�������ֽڶ��� */
}sip_lync_cap_cfg_t;


typedef struct
{
    unsigned char   bypass_enabled;                 /* �������Ƿ��� bypass */
    unsigned char   int_bypass_mode;                /* �����û��Ƿ����� bypass */
    unsigned char   ext_bypass_mode;                /* �����û��Ƿ����� bypass */
    unsigned char   reserved_pad;                   /* Ԥ�������ֽڶ��� */
}sip_lync_media_cfg_t;



/* �������ã�ģ���Թ���������������ü��� */
typedef struct
{
#define SIP_LYNC_ABS_WEB_AND_FILE           0       /* WebSearchAndFileDownload */
#define SIP_LYNC_ABS_WEB_ONLY               1       /* WebSearchOnly */
#define SIP_LYNC_ABS_FILE_ONLY              2       /* FileDownloadOnly */
    unsigned char   abs_usage;                      /* �û�����ͨѶ������������Ϣ�ķ�ʽ */

#define SIP_LYNC_NO_PHOTO                   0       /* ����ʾ��Ƭ */
#define SIP_LYNC_PHOTOS_FROM_AD_ONLY        1       /* ����ʾ�� AD ����Ƭ */
#define SIP_LYNC_ALL_PHOTOS                 2       /* ������ʾ AD �ϻ����Զ������Ƭ */
    unsigned char   photo_usage;                    /* �Ƿ�Ҫ��ʾ�û��Լ���ϵ�˵���Ƭ */
    unsigned char   disablehandsetonlockedmachine;
    unsigned char   reserved_pad[29];                /* Ԥ�������ֽڶ��� */
}sip_lync_common_cfg_t;


/*
 * Lync provision param
 */
typedef struct
{
    unsigned short  version;
    unsigned short  size;

    sip_lync_common_cfg_t           common_cfg;                     /* ģ���Թ�����������ü��� */

    sip_lync_phone_lock_policy_t    phone_lock_policy;              /* ������������� */
    sip_lync_cap_cfg_t              cap_cfg;                        /* cap ������� */
    sip_lync_uc_policy_t            uc_policy;                      /* UC �������� */
    sip_lync_meeting_policy_t       meeting_policy;                 /* ������� */
    sip_lync_media_cfg_t            media_cfg;                      /* ý��������ã�Ԥ���� */

    sip_lync_location_policy_t      loc_policy;                     /* e911 ������ص�λ�ò�����Ϣ */
    sip_lync_ipphone_policy_t       ipp_policy;                     /* ipphone policy���� */

    unsigned char   enable_skype_directory_search;
    unsigned char   enable_qos;                              /*�Ƿ���QoE*/
    unsigned char   enable_incall_qos;                       /*�Ƿ���incall QoE*/
    unsigned short incall_qos_interval_sec;                  /*QoE�ϱ�ʱ����*/
    unsigned char   reserved_pad[27];                                /* Ԥ�� */


    sip_map_t       loc_e911_dial_str;              /* �������к��룬1-10���ַ���str */
    sip_map_t       loc_e911_dial_mask;             /* �����������룬��;�ָ������100���ַ���str */
    sip_map_t       loc_e911_disclaimer;            /* ����������str */

    /* ��ط���� URL */
    sip_map_t       vm_uri;                         /* voicemail url��str */ 
    sip_map_t       lis_url;                        /* λ����Ϣ���� url��str */
    sip_map_t       dlx_url;                        /* ͨѶ������url��str */
    sip_map_t       abwq_url;                       /* ��ϵ��Web���� url��str */
    sip_map_t       abs_url;                        /* abs url��������ϵ����AD�ϵ�ͼƬ��str */
    sip_map_t       update_url;                     /* log�ϴ��ĵ�ַ*/
    sip_map_t       user_name;                      /* ˵�����ĸ��û���pvs*/

    sip_map_t       priorit_codecs_list;            /* codec���ȼ����Ӹߵ��ͣ�*/
    sip_map_t       loc_pvs_server_user;            /* �������÷�������֤�û��� */
    sip_map_t       loc_pvs_server_password;        /* �������÷�������֤���� */
    sip_map_t       loc_pvs_server_addr;            /* ���÷�������ַ */
    sip_map_t       loc_pvs_server_type;            /* �������÷�����ʹ�õ�Э�� */

    /* Ԥ�������� */
    sip_map_t       presence_policy;                /* Presence-related configuration data��Ԥ���� */
    sip_map_t       reserved_map[48];               /* Ԥ�� */
} sip_lync_provision_t;


/*
 * Lync boss provision param
 */
typedef struct
{
    unsigned short  version;
    unsigned short  size;

    unsigned char   reserved_pad[8];                /* Ԥ�� */

    sip_map_t       boss_uri;                       /* �������ĸ� boss �� provision ��Ϣ��str */
    sip_map_t       vm_uri;                         /* voicemail url��str */

    sip_map_t       reserved_map[4];                /* Ԥ�� */
}sip_lync_boss_provision_t;

typedef enum
{
    LYNC_BOSS_SIMULTAN_RING_NONE = 0,         /*�ر�ͬ������*/
    LYNC_BOSS_SIMULTAN_RING_CONTACT,         /*����ͬ�쵽�º���*/
    LYNC_BOSS_SIMULTAN_RING_DELEGATE,         /*����ͬ�쵽����*/
    LYNC_BOSS_SIMULTAN_RING_GROUP,             /*����ͬ�쵽��Ӧ��*/
    LYNC_BOSS_FORWARD_VM,                             /*��������ת�Ƶ���������*/
    LYNC_BOSS_FORWARD_DELEGATE,                   /*��������ת�Ƶ�����*/
    LYNC_BOSS_FORWARD_CONTACT                    /*��������ת�Ƶ��º���*/
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

    unsigned char     show_photo;                       /* �Ƿ���ʾ����ͷ��(��ӦdisplayADPhoto����)��0���� 1����*/
    unsigned char     vm_enable;                        /* �˺��Ƿ����� VM��0���� 1���� */
    unsigned char     reserved_pad[6];                  /* ���ֽڶ��� */

    sip_map_t         display_name;                     /* ��ʾ�� */
    sip_map_t         abs_hash;                         /* ͷ�� AD ���� abs hash */
    sip_map_t         ews_url;                          /* Exchange Web Service URL */
    sip_map_t         ews_primary_smtp;                 /* Primary SMTP ��ַ */
    sip_map_t         ews_hash;                         /* ewsͷ�� hash */
    sip_map_t         web_url;                          /* Web URL */
    sip_map_t         web_hash;                         /* webͷ�� hash */
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
    int uuid;                                       /* UI���룬�������ֶ��ĵ����ͣ�SIP��ԭֵ���� */

#define SIP_LYNC_PRES_SUB_OK                            0
#define SIP_LYNC_PRES_SUB_FAIL                          -1
    int pres_sub_result_code;                       /* ��ϵ�˶��Ľ������Ӧ */

    /* result code Ϊ OK ʱ�Ŵ�������ֵ */
    char boundary[64];                              /* �ָ��ַ��� */

#define SIP_LYNC_CONTACT_BODY_IN_MSG        0       /* ��ʾcontacts body ����Ϣ���� */
#define SIP_LYNC_CONTACT_BODY_IN_FILE       1       /* ��ʾcontacts body д���ļ��� */
    unsigned int buf_type;                          /* ȡ����ֵ: 1:�ļ�����0:BODY */
    char buf[SIP_MAX_MSGKIT_BODY_LEN];              /* ���ļ���������BODY */
} sip_lync_contacts_info_t;


typedef enum
{
    LYNC_PRES_SUB_RETRY_ALL_LOCAL_CONTACTS,                 /* UI���¶������б�����ϵ�ˣ�SIPɾ�����js */
    LYNC_PRES_SUB_RETRY_CROSS_POOL_CONTACTS,                /* UI���¶���ָ��pool����ϵ�ˣ�SIP������ */
    LYNC_PRES_SUB_RETRY_EXTERNAL_CONTACTS,                  /* UI������SIP���¶���ָ���ⲿ��ϵ�� */
}sip_lync_pres_sub_retry_action_e;

typedef struct
{
    unsigned short action;                                     /* sip_lync_pres_sub_retry_action_e */
    unsigned short interval;                                   /* ���·����ĵļ��ʱ�� */
    char target[SIP_MAX_URI_LEN];                              /* ���Ľӿڵ� to ���� */

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
    unsigned char role;  /*�����Ա��ɫ*/

#define SIP_LYNC_CONF_TYPE_AUDIO_VIDEO  0
#define SIP_LYNC_CONF_TYPE_IM           1
#define SIP_LYNC_CONF_TYPE_PHONE_CONF  2
    unsigned char session_type;  /*��������*/

#define SIP_LYNC_CONF_UNHOLD            0
#define SIP_LYNC_CONF_HOLD              1
#define SIP_LYNC_CONF_HOLD_FROM_MCU     2
    unsigned char hold_status; /*�����Աhold״̬*/

#define SIP_LYNC_CONF_UNMUTE            0
#define SIP_LYNC_CONF_MUTE              1
    unsigned char mute_status; /*�����Աmute״̬*/

#define SIP_LYNC_MCU_UNBLOCK            0
#define SIP_LYNC_MCU_BLOCK              1
    unsigned char mcu_direction;

#define SIP_LYNC_FOCUS_CONNECTED   0
#define SIP_LYNC_FOCUS_ON_HOLD        1
    unsigned char lobby;
    unsigned char reserved[2];

    char display[SIP_MAX_NAME_LEN]; /*�����Ա��ʾ��*/
    char user_uri[SIP_MAX_URI_LEN]; /*�����Աuri*/
    char entity[SIP_MAX_URI_LEN]; /*�����Աuuid*/
    char media_id[SIP_MAX_NAME_LEN];  /*ý��Ự����*/
}sip_lync_conference_user_t;

typedef struct
{
    unsigned char reserved[4];
    char organizer_uri[SIP_MAX_URI_LEN]; /*������֯��uri*/
    char organizer_display[SIP_MAX_NAME_LEN]; /*������֯����ʾ��*/
}sip_lync_conference_descrip_t;

typedef struct
{
    unsigned char conf_lock;
    unsigned char entry_exit_announcements;
    unsigned char modify_filter_default;    /*Ĭ�ϲ���������mute��unmute����*/
    unsigned char modify_filter_presenter; /*��ʾ������mute��unmute����*/
    unsigned char initial_filter_default;       /*Ĭ�ϲ����߽������mute��״̬block��unblock�������Ƿ���Ҫ�Զ�mute*/
    unsigned char initial_filter_presenter;   /*��ʾ�߽������mute��״̬��������ʾ���Ƿ���Ҫ�Զ�mute����Ȩ�޸�����֯��*/
    unsigned char reserved[10];
}sip_lync_conference_entity_view_t;

typedef struct
{
    unsigned char reserved[4];

    char focus_uri[SIP_MAX_URI_LEN]; /*����focus_uri*/
    char conversation_id[SIP_MAX_NAME_LEN]; /*����conv id*/
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
    unsigned char code; /*������*/
    unsigned char sub_req_type;             /*�����ܾ�Lobby��Ա������飬�����ɫ1:presenter, 0:attentee*/
    unsigned char reserved[1];

    char user[SIP_MAX_NAME_LEN];
    char reason[SIP_MAX_NAME_LEN];
}sip_lync_conf_req_result_t;

typedef struct  
{
    unsigned short          version;
    unsigned short          size;
    unsigned char       reserved_pad[12];                 /*unsigned char ����Ԥ��*/

    sip_map_t             conf_id;
    sip_map_t             pstn_num;
    sip_map_t             reserved_map[30];                /* Ԥ�� */
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

    int server_ip;/* �����ֽ� */
}sip_syslog_info_t;

/* ���²�������ֻ��ģ�����࣬�������ϸ�� */
#define SIP_EXT_PARAMS_DELETED          -1
/* �������� 0~127 */
#define SIP_EXT_PARAMS_UNDEFINED        0
#define SIP_UUID                        1      /**/
#define SIP_DTMF_CODE                   2
#define SIP_MESSAGE_REQUEST_TYPE        3      /* ָ����Ϣ���� */
#define SIP_MESSAGE_RECEIVER            4      /* ָ����Ϣ������ */
#define SIP_MESSAGE_HEADERS             5      /* ����UI��Ӷ���ͷ��������������CRLF(\r\n)�ָ���ʾ���磺Event: dialog\r\nContent-Type: application/dialog-info+xml\r\n */
#define SIP_MESSAGE_CONTENT_TYPE        6      /* ָ��content type */
#define SIP_MESSAGE_BODIES              7      /* ָ����Ϣ��body */
#define SIP_MESSAGE_TID                 8      /* ָ����Ϣ�����tid */
#define SIP_MESSAGE_RES_CODE            9      /* ָ����Ӧ��Ϣ��code */


/* ���в��� 128~255 */
#define SIP_MAKE_CALL_MASK              128
#define SIP_MAKE_CALL_CALLEE            129
#define SIP_MAKE_CALL_PICKUP_INDEX      130
#define SIP_MAKE_CALL_SCA_INDEX         131
#define SIP_MAKE_CALL_HOLD              132
#define SIP_MAKE_CALL_TIPTEL_INVOKE_ID  133     /* ����ͷ���������"Event: call_invoke;InvokeID=5" */
#define SIP_BSFT_DISPOSITION_CODE       134     /* ��BSFT������������ʱ���������������ַ����Ϊ�������ַ� */
#define SIP_MAKE_CALL_NTCL_RPID         135     /* �ͻ�NTCLҪ�������з�����INVITE��Ϣ�м���RPIDͷ�����������е���� */
#define SIP_MAKE_CALL_VIDEO_BANDWIDTH   136     
#define SIP_MAKE_CALL_H264_PARAM        137     
#define SIP_MAKE_CALL_BLA_INDEX         138
#define SIP_MAKE_CALL_TYPE              139     /* ���ӵĺ������ͣ�������SIPͨ��CMD֪ͨUI����ʱ����UI�����ĸ������ͣ�UI���Ը��ݸ����ͽ�����Ӧ���� */
#define SIP_MAKE_CALL_CALLEE_IDENTITY   140
#define SIP_CONTROL_CMD_PROROCOL        141     /* ������У�����Э������ */
#define SIP_MAKE_CALL_ONBEHALF          142
#define SIP_MAKE_CALL_PICKUP_VALUE      143
#define SIP_MAKE_CALL_EXT_H264_PARAM    144 /*�����ֱ���*/
#define SIP_MAKE_CALL_DISABLE_BFCP      145/*��ֹ���������ã����׵��������ó�ͻ*/
#define SIP_MAKE_CALL_VM_ITEMID         146
#define SIP_MAKE_CALL_VERIZON_ADD_P_COM 147
#define SIP_MAKE_CALL_EMERGENCY_SRV_PRI 148     /* �������е�server���ȼ� */
#define SIP_MAKE_CALL_H265_PARAM        149
#define SIP_MAKE_CALL_CUSTOM_INFO       150     /* ���ڴ����û��Զ����һЩ����*/


/* ������� 256~383 */
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
    unsigned char reserved_pad[3]; /* ���ֽڶ���,Ԥ�� */

    int code;                      /* Ԥ�� */
    char reason[SIP_MAX_NAME_LEN]; /* Ԥ�� */
}sip_cmd_common_info_t;

typedef  struct
{
    unsigned char call_type;
    unsigned char reserved_pad[3]; /* ���ֽڶ���,Ԥ�� */

    int code;                      /* Ԥ�� */
    char data[SIP_MAX_NAME_LEN]; /* Ԥ�� */
}sip_call_type_info_t;

typedef struct 
{
    unsigned char control_protocol_type;
    unsigned char reserved_pad[3]; /* ���ֽڶ���,Ԥ�� */

    int  mode;                     /* Ԥ�� */
    char forward_to[SIP_MAX_URI_LEN];
}sip_cmd_call_forward_info_t;

typedef struct 
{
    unsigned char control_protocol_type;
    unsigned char reserved_pad[3]; /* ���ֽڶ���,Ԥ�� */

    int  mode;                     /* Ԥ�� */
    int  target_cid;
    char refer_to[SIP_MAX_URI_LEN];
} sip_cmd_call_transfer_info_t;

typedef sip_cmd_call_forward_info_t  sip_cmd_set_call_forward_info_t;

typedef struct 
{
    unsigned char control_protocol_type;
    unsigned char reserved_pad[3]; /* ���ֽڶ���,Ԥ�� */

    int  mode;                     /* Ԥ�� */
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
    unsigned char reserved_pad[3];  /* ���ֽڶ���,Ԥ�� */

    int force;                      /* 1/0 �Ƿ�ǿ��ִ�У���ʹ����ͨ���� */
    int random_time_min;            /* random���ʱ�䷶Χ��ֵ, @exp: 5 */
    int random_time_max;            /* random���ʱ�䷶Χ��ֵ, @exp: 45 */

    int code;                       /* Ԥ�� */
    char reason[SIP_MAX_NAME_LEN];  /* Ԥ�� */
}sip_cmd_reload_account_info_t;

typedef struct _sip_cmd_reboot_phone_info
{
    unsigned char control_protocol_type;
    unsigned char reserved_pad[3];  /* ���ֽڶ���,Ԥ�� */

    int force;                      /* 1/0 �Ƿ�ǿ��ִ�У���ʹ����ͨ���� */
    int random_time_min;            /* random���ʱ�䷶Χ��ֵ, @exp: 5 */
    int random_time_max;            /* random���ʱ�䷶Χ��ֵ, @exp: 45 */

    int code;                       /* Ԥ�� */
    char reason[SIP_MAX_NAME_LEN];  /* Ԥ�� */
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
    unsigned char sub_req_type;             /*�����ܾ�Lobby��Ա������飬�����ɫ1:presenter, 0:attentee*/
    unsigned char media_id;/*media_id*/
#define SIP_APOLLO_CONF_MEDIA_BLOCK        1
#define SIP_APOLLO_CONF_MEDIA_UNBLOCK      2
#define SIP_APOLLO_CONF_MEDIA_UNBLOCKING   3
    unsigned char media_fiter;/*mute ��״̬ block��ʾmute unblock��ʾUnmute unblocking��ʾ���ھ��� ��Ҫ������ͬ��*/
    int  code; /*������*/
    char user[SIP_MAX_NAME_LEN];
    char reason[SIP_MAX_NAME_LEN];
    char entity[SIP_MAX_NAME_LEN];/*Uuid*/
}sip_apollo_conf_req_result_t;

typedef struct apollo_conf_media_data
{
#define SIP_APOLLO_CONF_TYPE_AUDIO  1
#define SIP_APOLLO_CONF_TYPE_VIDEO  2
#define SIP_APOLLO_CONF_TYPE_APPLICATIONSHARING 3
    unsigned char media_type;  /*����ý������,�����2��media ��auido��video,UI��Ҫ�����������жϣ�Ĭ��ֵΪ0����ʾû��*/

#define SIP_APOLLO_CONF_UNHOLD            1
#define SIP_APOLLO_CONF_HOLD              2
#define SIP_APOLLO_CONF_HOLD_FROM_MCU     3  /*��Ա����hold����MCU block*/
    unsigned char hold_status; /*�����Աhold״̬��HOLD��ʾsendonly��inactive������Ա��hold״̬*/

#define SIP_APOLLO_CONF_UNMUTE            1
#define SIP_APOLLO_CONF_MUTE              2
#define SIP_APOLLO_CONF_FLOOR             3
    unsigned char mute_status; /*�����Աmute״̬����ʾ�Ƿ�������Ƶ���룬block����ʾMute��unblock��ʾȡ��Mute��FLOOR ��ʾ������֣��÷�����client to MCU*/

#define SIP_APOLLO_MCU_UNBLOCK            1
#define SIP_APOLLO_MCU_BLOCK              2
    unsigned char mcu_direction;

#define SIP_APOLLO_BLOCK_BY_CLIENT        1
#define SIP_APOLLO_BLOCK_BY_SERVER        2
    unsigned char blockby;
    unsigned char reserve[1];

    unsigned int media_id;  /*ý��audio�Ựid*/

    unsigned int reserve2[4];

}apollo_conference_media_data_t;


typedef struct  
{
#define SIP_APOLLO_CONF_ROLE_DELETE         1
    unsigned char user_quit;/*�����˳�����,Ϊ1��ʾ�˳���Ĭ��Ϊ0*/
#define SIP_APOLLO_CONF_ROLE_PRESENTER    1
#define SIP_APOLLO_CONF_ROLE_ATTENDEE     2
    unsigned char role;  /*�����Ա��ɫ*/

#define SIP_APOLLO_CONF_ADD_USER_CONNECTED  1
#define SIP_APOLLO_CONF_ADD_USER_DISCONNECT  2
#define SIP_APOLLO_CONF_ADD_USER_PREPARE  3
    unsigned char user_state;/*�������ߵ�״̬*/

#define SIP_APOLLO_FOCUS_CONNECTED   1
#define SIP_APOLLO_FOCUS_ON_HOLD        2
    unsigned char lobby;/*lobby ����*/

#define SIP_APOLLO_FOCUS_CHAIRMAN_AUDIENCE    1
#define SIP_APOLLO_FOCUS_CHAIRMAN_DEMONSTRATOR   2
    unsigned char chairmam_role;
    unsigned char reserved[3];

    char display[SIP_MAX_NAME_LEN]; /*�����Ա��ʾ��*/
    char user_uri[SIP_MAX_URI_LEN]; /*�����Աuri*/
    char entity[SIP_MAX_URI_LEN]; /*�����Աuuid*/
    unsigned int reserved2[12];

    unsigned int cnt;
    apollo_conference_media_data_t media[3];/*media���ͣ���audio ��video��applicationsharing��chat*/
}sip_apollo_conference_user_t;

typedef struct
{
#define SIP_APOLLO_CONF_PATTERN_DEFAULT         1
#define SIP_APOLLO_CONF_PATTERN_DEMONSTRATOR    2
    unsigned char pattern;/*h����ģʽ,1:��ͨģʽ�� 2:��ϯģʽ*/
    char conference_uri[SIP_MAX_NAME_LEN];/*����uri*/
    char mcu_uri[SIP_MAX_NAME_LEN];/*MCU aduio-video uri*/
    int conference_number;/*�����Number,����IVR����*/
    char organizer_uri[SIP_MAX_URI_LEN]; /*������֯��uri*/
    char organizer_display[SIP_MAX_NAME_LEN]; /*������֯����ʾ��*/
}sip_apollo_conference_descrip_t;

typedef struct
{
    unsigned char conf_lock;/*focus ��״̬ 1��ʾlock 0��ʾunlock*/
    unsigned char entry_exit_announcements_enable;/*��Ա���롢�˳�����ͨ���Ƿ���Կ���*/
    unsigned char reserved[5];
    char video_layout[SIP_MAX_NAME_LEN];/*���鲼��*/
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
    char    value_number[SIP_MAX_NAME_LEN];//����1�洢�ĺ���

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

/*message������ϱ�*/
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

