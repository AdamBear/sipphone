/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2007-2010 Xiamen  Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2013-3-7
*
*******************************************************************/
#ifndef __SERVICE_H323_DEFINE__
#define __SERVICE_H323_DEFINE__

#if WIN32
#define _WINSOCKAPI_
#include <windows.h>
#undef _WINSOCKAPI_
#include <winsock2.h>
#include <ws2tcpip.h>
#define inline  __inline
#define H323_DLL_DECLARE __declspec(dllexport)
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define H323_DLL_DECLARE
#endif

/* 所有平台都依赖msgkit接口,使用到里面的一些数据结构 */
#include <msgkit/mkmessage.h>
#include <msgkit/mkservice.h>
/************************************************************************/
/*                                                                      */
/************************************************************************/
#define H323_MAX_NAME_LEN                   128
#define H323_MAX_URI_LEN                    512
#define H323_MAX_PATH_LEN                   512

#define H323_MAX_IP_ADDRES_LEN              48
#define H323_MAX_SRTP_KEY_LEN               80
#define H323_MAX_RTCP_FB_LEN                48

#define H323_MAX_CODEC_MIME_LEN             32
#define H323_MAX_CODEC_PARAM_LEN            128
#define H323_MAX_SDP_PARA_LEN               (H323_MAX_CODEC_PARAM_LEN + H323_MAX_CODEC_PARAM_LEN)
#define H323_MAX_EXTRA_CODEC_COUNT          2

#ifndef H323_SUCCESS
#define H323_SUCCESS                        0
#endif

#ifndef H323_FAIL
#define H323_FAIL                          -1
#endif


/************************************************************************/
/*                调用函数需要使用的相关数据结构                        */
/************************************************************************/
#define H323_EXT_PARAMS_DELETED              -1
#define H323_EXT_PARAMS_UNDEFINED          0

/* 呼叫参数 128~255 */
#define H323_MAKE_CALL_MASK                           128/*呼叫类型，音频或视频*/
#define H323_MAKE_CALL_CALLEE                         129
#define H323_MAKE_CALL_VIDEO_BANDWIDTH     130
#define H323_MAKE_CALL_H264_PARAM                131/*主流H264参数*/
#define H323_MAKE_CALL_EXT_H264_PARAM        132/*辅流H264参数*/
#define H323_MAKE_CALL_DISABLE_H239                133/*呼叫不携带辅流*/

typedef enum
{
    H323_AUDIO_CALL,
    H323_VIDEO_CALL,
} h323_call_type_e;

typedef struct
{
    unsigned char level;
    unsigned char reserved[3];
    unsigned int max_mbps;
    unsigned int max_fs;
} h323_h264_param_t;

typedef struct
{
    int type;
    int total_len;
    int payload_len;
    char payload[4];
} h323_data_header_t;

typedef struct
{
    h323_data_header_t overview_header;
} h323_ext_params_t;
/************************************************************************/
/*                  上报消息至UI使用的enum类型                          */
/************************************************************************/

typedef enum
{
    H323_AUDIO_CHANNEL = 1,
    H323_VIDEO_CHANNEL,
    H323_FAX_CHANNEL,
    H323_H224_CHANNEL,
    H323_EXT_VIDEO_CHANNEL,
    H323_FILE_CHANNEL
} h323_logic_channel_type_e;


typedef enum
{
    H323_END_BY_LOCAL_USER,           /* < Local endpoint application cleared call                   */
    H323_END_BY_NO_ACCEPT,            /* < Local endpoint did not accept call OnIncomingCall()=FALSE */
    H323_END_BY_ANSWER_DENIED,        /* < Local endpoint declined to answer call                    */
    H323_END_BY_REMOTE_USER,          /* < Remote endpoint application cleared call                  */
    H323_END_BY_REFUSAL,              /* < Remote endpoint refused call                              */
    H323_END_BY_NO_ANWSWER,           /* < Remote endpoint did not answer in required time           */
    H323_END_BY_CALLER_ABORT,         /* < Remote endpoint stopped calling                           */
    H323_END_BY_TRANSPORT_FAIL,       /* < Transport error cleared call                              */
    H323_END_BY_CONNECT_FAIL,         /* < Transport connection failed to establish call             */
    H323_END_BY_GATE_KEEPER,          /* < Gatekeeper has cleared call                               */
    H323_END_BY_NO_USER,              /* < Call failed as could not find user (in GK)                */
    H323_END_BY_NO_BANDWIDTH,         /* < Call failed as could not get enough bandwidth             */
    H323_END_BY_CAPABILITY_EXCHANGE,  /* < Could not find common capabilities                        */
    H323_END_BY_CALL_FORWARD,         /* < Call was forwarded using FACILITY message                 */
    H323_END_BY_SECURITY_DENIAL,      /* < Call failed a security check and was ended                */
    H323_END_BY_LOCAL_BUSY,           /* < Local endpoint busy                                       */
    H323_END_BY_LOCAL_CONGESTION,     /* < Local endpoint congested                                  */
    H323_END_BY_REMOTE_BUSY,          /* < Remote endpoint busy                                      */
    H323_END_BY_REMOTE_CONGESTION,    /* < Remote endpoint congested                                 */
    H323_END_BY_UNREACHABLE,          /* < Could not reach the remote party                          */
    H323_END_BY_NO_ENDPOINT,          /* < The remote party is not running an endpoint               */
    H323_END_BY_HOST_OFFLINE,         /* < The remote party host off line                            */
    H323_END_BY_TEMPORARY_FAILURE,    /* < The remote failed temporarily app may retry               */
    H323_END_BY_Q931_CAUSE,           /* < The remote ended the call with unmapped Q.931 cause code  */
    H323_END_BY_DURATION_LIMIT,       /* < Call cleared due to an enforced duration limit            */
    H323_END_BY_INVALID_CONFERENCE_ID,/* < Call cleared due to invalid conference ID                 */
    H323_END_BY_OSP_REFUSAL,          /* < Call cleared as OSP server unable or unwilling to route   */
    H323_END_BY_INVALID_NUMBER_FORMAT,/* < Call cleared as number was invalid format                 */
    H323_END_BY_UNSPECIFIED_PORTOCOL, /* < Call cleared due to unspecified protocol error            */
    H323_END_BY_NO_FEATURE_SUPPORT,     /* < Call ended due to Feature not being present.              */
    H323_END_BY_INFOMATION_ELE_MISSING, /*mandatory information element missing*/
    H323_END_BY_COMPAT_STATE_ERROR,     /*message not compatible with call state*/
    H323_END_BY_INVALID_INFOMATION,     /*invalid information element*/
    H323_END_BY_TIMER_EXPIRY,           /* EndedByTimerExpiry T301*/
    H323_END_BY_SECURITY_ERROR,         /* < Call failed a security passwd check and was ended          */
    H323_END_BY_PARAM_ERROR,            /* new msg, add by requirement                                 */
    H323_NUM_CALL_END_REASONS,
} h323_call_end_reason_code_e;

typedef enum
{
    H323_ACTION_PAN_LEFT,
    H323_ACTION_PAN_RIGHT,
    H323_ACTION_PAN_DOWN,
    H323_ACTION_PAN_UP,
    H323_ACTION_ZOOM_OUT,
    H323_ACTION_ZOOM_IN,
    H323_ACTION_FOCUS_OUT,
    H323_ACTION_FOCUS_IN,
    H323_ACTION_ILLEGAL,
} h323_camare_action_e;

typedef enum
{
    H323_AES_UNKNOWN,
    H323_AES_128_CBC,
    H323_AES_128_OFB,
} h323_AES_mode_e;

typedef enum
{
    H323_SESSION_ID_AUDIO        = 1,
    H323_SESSION_ID_VIDEO        = 2,
    H323_SESSION_ID_FAX          = 3,
    H323_SESSION_ID_H224         = H323_SESSION_ID_FAX,
    H323_SESSION_ID_EXT_VIDEO    = 4,
    H323_SESSION_ID_FILE         = 5,
    H323_SESSION_ID_UNKNOWN      = 6,
} h323_session_id_e;

/************************************************************************/
/*                       公共数据相关                                   */
/************************************************************************/
typedef struct
{
    char display_name[H323_MAX_NAME_LEN];
    char user_name   [H323_MAX_NAME_LEN];
    char server_name [H323_MAX_NAME_LEN];
    char dest_dialed_digits[H323_MAX_NAME_LEN];/* 来电的ip##abc，abc即该值，用于会议密码功能 */
} h323_call_info_t;

typedef struct
{
    h323_session_id_e sid;
    unsigned int bitrate;
} h323_flow_ctrl_para_t;

/************************************************************************/
/*                     上报给UI相关的Codec数据结构                      */
/************************************************************************/
typedef struct
{
    unsigned char SQCIF;
    unsigned char QCIF;
    unsigned char CIF;
    unsigned char CIF4;
} h323_h263_fmt_t;

typedef struct
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
} h323_h264_fmt_t;

typedef struct
{
#define H323_VIDEO_CODEC_TYPE_NONE  0
#define H323_VIDEO_CODEC_TYPE_H263  1
#define H323_VIDEO_CODEC_TYPE_H264  2
    int video_codec_type;
    union
    {
        h323_h264_fmt_t h264;
        h323_h263_fmt_t h263;
    } param;
} h323_video_param_t;

typedef struct
{
    int bitrate;
    int annexb;
} h323_audio_param_t;

typedef struct
{
    char data[128];
} h323_string_param_t;

typedef union
{
    short sa_family;
    struct sockaddr_in  a4;
    struct sockaddr_in6 a6;
} h323_addr_t;

typedef struct
{
    unsigned char   payload;
    unsigned char   reserved[3];
    int             sample_rate;
    char            codec_name[H323_MAX_CODEC_MIME_LEN];

    union
    {
        h323_audio_param_t      audio_param;
        h323_video_param_t      video_param;
        h323_string_param_t nostandard_param;
    } ext_param;

} h323_codec_t;

typedef struct
{
    h323_addr_t     rtp_addr;
    h323_addr_t     rtcp_addr;

    unsigned char   rtp_ptime;
    unsigned char   rtpev_payload;  /* rtp event payload, like dtmf 101 */
    unsigned char   reserved[2];

    unsigned int    bandwidth;
    h323_codec_t    codec;
    char            srtp_key[H323_MAX_SRTP_KEY_LEN];
    char            rtcp_fb[H323_MAX_RTCP_FB_LEN];

    unsigned int multiplex_id;  /*h46019M multiplex mode, noly support encode now*/
    char            reserved_ex[28];    /* 预留扩展 */
} h323_media_t;



typedef struct
{
    unsigned char  channel_type;
    unsigned char  reserved[1];
    unsigned short h460_keepalive_sec;/*  keep alive time*/

    h323_media_t   local_media;
    h323_media_t   remote_media;
} h323_channel_t;

/************************************************************************/
/*                     诊断信息                  */
/************************************************************************/
typedef enum _h323_router_type
{
    H323_ROUTER_TYPE_UNKNOWN,
    H323_ROUTER_TYPE_COM,           /*非ALG或纯公网，普通类型*/
    H323_ROUTER_TYPE_ALG,             /*ALG类型*/
    H323_ROUTER_TYPE_SPECIAL,      /*特殊类型，有网关转发，需要开启强制NAT才可互通*/
} h323_router_type_e;

typedef struct _h323_diagnose_sig
{
    int
    router_type;                               /*h323_router_type_e 路由器是否开启ALG*/

    unsigned short local_port;                                  /*本地信令使用的端口*/
    unsigned short sig_port;                                     /*setup信令中携带端口*/
    unsigned short peer_port;                                  /*出路由后实际端口*/

    char    local_addr[H323_MAX_IP_ADDRES_LEN];    /*本地信令使用的IP*/
    char    sig_addr[H323_MAX_IP_ADDRES_LEN];       /*setup信令中携带的IP*/
    char    peer_addr[H323_MAX_IP_ADDRES_LEN];     /*出路由后实际的IP*/
} h323_diagnose_sig_t;


typedef struct _h323_diagnose_media
{
    int                    channel_type;                                   /*H323LogicChannelTypeE*/
    unsigned short sdp_rtpport, sdp_rtcpport;         /*sdp中携带的媒体端口*/
    char                 sdp_addr[H323_MAX_IP_ADDRES_LEN];  /*sdp中携带的IP*/
} h323_diagnose_media_t;

typedef struct
{
    /* 和cfgserver保持一致，注意这里需要设置READY，SIP才会使用外部设置的网络参数 */
#define H323_NET_STATUS_OFF              0        /*network is hang*/
#define H323_NET_STATUS_NEG              1        /*network is on negotiation or authentication*/
#define H323_NET_STATUS_READY         2        /*network is ready*/

    int network_status;
    int has_vpn;
    int has6_vpn;
    struct in_addr  a4_addr;
    struct in_addr  subnet_addr;
    struct in6_addr a6_addr;
    struct in_addr  vpn_addr;
    struct in6_addr vpn6_addr;
    struct in_addr  dns_addr[2];
    char   mac_addr[32];                    /*MAC地址*/
} h323_network_t;

#endif


