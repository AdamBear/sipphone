/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2010-10-21
*
*******************************************************************/
#ifndef __DECT_SS7_H__
#define __DECT_SS7_H__

/************************************************************************/
/*     for ss7 packet type                                              */
/************************************************************************/
#define SS70_INSTANCE_TYPE_AP   1          /*!< instance ID set by the access point      */
#define SS70_INSTANCE_TYPE_SCM  2          /*!< instance ID set by the session call manager */
#define SS70_INSTANCE_TYPE_BD   3          /*!< instance ID used for a broadcast service     */
#define SS70_INSTANCE_AUTOMATIC 0xFFF      /* instance ID is selected automatically */

#define SS70_INSTANCE_AP_PSTN_TE        1   /*!< access point PSTN analogue side FXO*/
#define SS70_INSTANCE_AP_SIP            2   /*!< access point VoIP SIP network    */
#define SS70_INSTANCE_AP_DECT           3   /*!< access point DECT            */
#define SS70_INSTANCE_AP_SMS            4   /*!< access point SMS application       */
#define SS70_INSTANCE_AP_PSTN_NT        5   /*!< access point PSTN analogue side FXS*/
#define SS70_INSTANCE_AP_DTAM           6   /*!< access point for DTAM */
#define SS70_INSTANCE_AP_ISDN_TE        7   /*!< access point PSTN ISDN side TE    */
#define SS70_INSTANCE_AP_ISDN_NT        8   /*!< access point PSTN ISDN side NT    */
#define SS70_INSTANCE_AP_CONFIG         9   /*!< access point of configuration part    */
#define SS70_INSTANCE_AP_SPKPHONE       10  /*!< access point Speakerphone appl.    */
#define SS70_INSTANCE_AP_PBX            11  /*!< access point PBX / SS7 router    */
#define SS70_INSTANCE_AP_CCC            12
#define SS70_INSTANCE_AP_CMBS           13
#define SS70_INSTANCE_AP_MAX            14

#define SS70_INSTANCE_TYPE_SET(_x, _type)   ( (_x) |= (((_type) & 0x000F)<< 28) )
#define SS70_INSTANCE_AP_SET(_x, _type)     ( (_x) |= (((_type) & 0x0FFF)<< 16) )
#define SS70_INSTANCE_UID_SET(_x, _uid)     ( (_x) |= (((_uid)  & 0xFFFF)<< 0 ) )

/*
* screening indicator
*/
#define SS70_ADDR_TYPE_SET(_x, _type)   ( (_x) |= ( ((_type) & 0x0F)<< 4 ))
#define SS70_ADDR_PLAN_SET(_x, _plan)   ( (_x) |= ( ((_plan) & 0x0F)<< 0 ))
#define SS70_ADDR_PRES_SET(_x, _PRES)   ( (_x) |= ( ((_PRES) & 0x0F)<< 4 ))
#define SS70_ADDR_SCRN_SET(_x, _SCRN)   ( (_x) |= ( ((_SCRN) & 0x0F)<< 0 ))

/*
* blind transfer LineType
*/
#define SS70_INT_CALL_REQ             0x17
#define SS70_CALL_LINE_CMD            0x15

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* ss7 channel
*/
#define SS70_CH_STATE_UP        0       /*!< media/payload channel is up                 */
#define SS70_CH_STATE_DOWN      1       /*!< media/payload channel is down                 */
#define SS70_CH_STATE_SUS       2       /*!< media/payload channel is suspended            */
#define SS70_CH_STATE_RES       3       /*!< media/payload channel is resumed            */
#define SS70_CH_STATE_UPDATE    4       /*!< media/payload channel is updated on the fly*/
#define SS70_CH_STATE_RESERVE   5       /*!< media/payload channel is already reserved */

/************************************************************************/
/*                                                                      */
/************************************************************************/
#define SS70_REL_NORMAL                   0x50
#define SS70_REL_LINE_INVALID             0x60
#define SS70_REL_LINE_OFFLINE             0x61
#define SS70_REL_LINE_FORBIDDEN           0x62
#define SS70_REL_LINE_BUSY                0x63
#define SS70_REL_LINE_REJECT              0x64
#define SS70_REL_LINE_TIMEOUT             0x65
#define SS70_REL_LINE_ERROR               0x66
#define SS70_REL_DECT_UNREACHABLE         0x67  /*找不到手柄信号*/
#define SS70_REL_DECT_NOT_FOUND           0x68  /*对应的所有手柄都不在线.*/
#define SS70_REL_DECT_PAGE_BUSY           0x69  /* page 间隔时间控制.*/

#define SS70_REL_NWE_CABLE                0x70
#define SS70_REL_NWE_IP                   0x71
#define SS70_REL_NWE_DNS                  0x72
#define SS70_REL_NWE_ERROR                0x73

#define SS70_REL_BAD_REQUEST              0x74/* sip 400 */
#define SS70_REL_SIP_FORBIDDEN            0x75/* sip 403 */
#define SS70_REL_NOT_FOUND                0x76/* sip 404 */
#define SS70_REL_SIP_TIMEOUT              0x77/* sip 408 */
#define SS70_REL_BUSY_HERE                0x78/* sip 486 */
#define SS70_REL_NOT_ACCEPT               0x79/* sip 488 */
#define SS70_REL_SERVER_ERROR             0x7a/* sip 5xx */
#define SS70_REL_GLOBAL_ERROR             0x7b/* sip 6xx */
#define SS70_REL_NO_ANSWER                0x7c/* sip 481 */

#define SS70_REL_NUM_INVALID              0x80
#define SS70_REL_NUM_BLOCK                0x81
#define SS70_REL_NUM_BUSY                 0x82
#define SS70_REL_NUM_DND                  0x83
#define SS70_REL_NUM_REJECT               0x84
#define SS70_REL_NUM_TIMEOUT              0x85
#define SS70_REL_NUM_REDIRECT             0x86
#define SS70_REL_NUM_ERROR                0x87
#define SS70_REL_MEDIA_UNSUPPORTED        0x90
#define SS70_REL_MEDIA_SHUTDOWN           0x91
#define SS70_REL_MEDIA_ERROR              0x92

#define SS70_REL_FWUP_NOT_END             0x93
#define SS70_REL_AUTOP_CODE               0x94
#define SS70_REL_EXT_ANONYMITY_DISALLOWED 0xc0

/************************************************************************/
/*                                                                      */
/************************************************************************/
#define SS70_INF_TYPE_CPG                   0   /*!< info array contains session/call progress indicator,information    */
#define SS70_INF_TYPE_DISPLAY               1   /*!< info array contains display information*/
#define SS70_INF_TYPE_MWI                   2   /*!< info array contains message waiting information    */
#define SS70_INF_TYPE_ADDR                  3   /*< info array contains address such as CLIP/COLP, etc.    */
#define SS70_INF_TYPE_IMS_MSSG              4
#define SS70_INF_TYPE_IMS_REPORT            5
#define SS70_INF_TYPE_VOICEBOX_MSSG         6   /*!< message type for voice box */
#define SS70_INF_TYPE_SMS                   7
#define SS70_INF_TYPE_FXO_DTAM              8   /* DTAM type for FXO case */
#define SS70_INF_TYPE_FXO_ALERT_ON          9
#define SS70_INF_TYPE_BLIND_TRANSFER        10
#define SS70_INF_TYPE_ATTENDED_TRANSFER     11
#define SS70_INF_LINE_CFM                   12
#define SS70_INF_DIAL_PLAN_CFM              13
#define SS70_INF_CLIP_UPDATE                14
#define SS70_INF_CODEC_UPDATE               15
#define SS70_INF_TRANSFER_STATUS            16  /* Status for Transfer*/
#define SS70_INF_PROP_CALL_STATUS           17
#define SS70_INF_PROP_CALL_ENCRYPT          18  /*CallEncrypt*/
#define SS70_INF_CNIP_UPDATE                19  /*CNIP_UPDATE*/
#define SS70_INF_AUTO_ANSWER                20  /*AUTO ANSWER */
#define SS70_INF_PROP_TONE_IND              21  /*PROP TONE IND */
#define SS70_INF_TYPE_CONF_RELEASE          22  /*ConfRel*/
#define SS70_INF_CALL_NET_CONF              23
#define SS70_INF_CALL_INFO_EVENT            24
#define SS70_INF_PROP_CLIP_NAME             25
#define SS70_INF_TYPE_MAX                   26



/************************************************************************/
/*                                                                      */
/************************************************************************/
#define DECT_SS7_MODULE_VOIP        0
#define DECT_SS7_MODULE_CMBS        1
#define DECT_SS7_MODULE_PSTN_EXT    2

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* cmd
*/
#define DECT_SS7_CMD_START       0
#define DECT_SS7_CMD_IAM         1
#define DECT_SS7_CMD_SAM         2
#define DECT_SS7_CMD_ACM         3
#define DECT_SS7_CMD_ANM         4
#define DECT_SS7_CMD_REL         5
#define DECT_SS7_CMD_SUS         6
#define DECT_SS7_CMD_RES         7
#define DECT_SS7_CMD_INF         8
#define DECT_SS7_CMD_CHST        9
#define DECT_SS7_CMD_SUS_ACK     10
#define DECT_SS7_CMD_RES_ACK     11
#define DECT_SS7_CMD_RTP_EVT_REQ 12
#define DECT_SS7_CMD_RTP_EVT_RES 13


/************************************************************************/
/*                                                                      */
/************************************************************************/
#define DECT_SS7_INST_NUM_MAX   5
#define DECT_SS7_SRC_SIZE_MAX   32
#define DECT_SS7_DST_SIZE_MAX   32
#define DECT_SS7_NUM_SIZE_MAX   32
#define DECT_SS7_NAME_SIZE_MAX  32
#define DECT_SS7_INFO_SIZE_MAX  64
#define DECT_SS7_CODEC_SIZE_MAX 8


/************************************************************************/
/*                                                                      */
/************************************************************************/

#define DECT_SS7_ALERT_PATTERN_0            0x40
#define DECT_SS7_ALERT_PATTERN_1            0x41
#define DECT_SS7_ALERT_PATTERN_2            0x42
#define DECT_SS7_ALERT_PATTERN_3            0x43
#define DECT_SS7_ALERT_PATTERN_4            0x44
#define DECT_SS7_ALERT_PATTERN_5            0x45
#define DECT_SS7_ALERT_PATTERN_6            0x46
#define DECT_SS7_ALERT_PATTERN_7            0x47
#define DECT_SS7_ALERT_CONTINUOUS           0x48
#define DECT_SS7_ALERT_AUTO_INTERCOM        0x4E
#define DECT_SS7_ALERT_OFF                  0x4F

/************************************************************************/
/*                                                                      */
/************************************************************************/
#define DECT_SS7_HOLD                     0
#define DECT_SS7_HOLD_TYPE_PUBLIC         1
#define DECT_SS7_HOLD_TYPE_PRIVATE        2

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* RTP EVENT
*/
#define DECT_SS7_RTP_EVT_RFC2833_RX                  0X00000001
#define DECT_SS7_RTP_EVT_RTCP_RX                     0X00000002
#define DECT_SS7_RTP_EVT_RTCP_TX                     0X00000004
#define DECT_SS7_RTP_EVT_VOICE_STATISTICS            0X00000008
#define DECT_SS7_RTP_EVT_NSE_RX                      0x00000010
#define DECT_SS7_RTP_EVT_ICMP_ERR                    0x00000020
#define DECT_SS7_RTP_EVT_RTP_VIOLATE                 0x00000040

#define DECT_SS7_RTP_EVT_RTCP_ALL                    0x00000006
#define DECT_SS7_RTP_EVT_ALL                         0x0000007F
#define DECT_SS7_RTP_EVT_MASK                        0x0000FFFF

/* RTCP_XR */                                    
#define DECT_SS7_RTP_EVT_RTCP_XR_RX                  0x00010000
#define DECT_SS7_RTP_EVT_RTCP_XR_TX                  0x00020000

#define DECT_SS7_RTP_EVT_RTCP_XR_ALL                 0x00030000
#define DECT_SS7_RTP_EVT_RTCP_XR_MASK                0x00FF0000

/* RTCP-XR-RB */
#define DECT_SS7_RTP_EVT_RTCP_XR_RB_VOIP             0x01000000
#define DECT_SS7_RTP_EVT_RTCP_XR_RB_DLRR             0x02000000
#define DECT_SS7_RTP_EVT_RTCP_XR_RB_PKTRT            0x04000000
#define DECT_SS7_RTP_EVT_RTCP_XR_RB_STAT             0x08000000
#define DECT_SS7_RTP_EVT_RTCP_XR_RB_RECREFT          0x10000000
#define DECT_SS7_RTP_EVT_RTCP_XR_RB_LOSS_RLE         0x20000000
#define DECT_SS7_RTP_EVT_RTCP_XR_RB_DUPLICATE_RLE    0x40000000

#define DECT_SS7_RTP_EVT_RTCP_XR_RB_ALL              0x7F000000
#define DECT_SS7_RTP_EVT_RTCP_XR_RB_MASK             0xFF000000


/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* IAM
*/
typedef struct
{
    D_UINT          inst;
    D_WORD          line;
    D_WORD          hs;
    D_WORD          module;
    D_WORD          ring;
    D_CHAR          src[DECT_SS7_SRC_SIZE_MAX];
    D_CHAR          dst[DECT_SS7_DST_SIZE_MAX];
    D_CHAR          name[DECT_SS7_NAME_SIZE_MAX];
    D_BYTE          codec[DECT_SS7_CODEC_SIZE_MAX];
    D_UINT          codec_len;
    D_CHAR          multi_name[DECT_SS7_INST_NUM_MAX][DECT_SS7_NAME_SIZE_MAX]; /* CFG_SUPPORT_MULTI_CNIP */
} ss7_msg_iam;

/*
* SAM
*/
typedef struct
{
    D_UINT          inst;
    D_CHAR          num[DECT_SS7_NUM_SIZE_MAX];
} ss7_msg_sam;

/*
* ACM
*/
typedef struct
{
    D_UINT          inst;
    D_UINT          ring;
} ss7_msg_acm;

/*
* ANM
*/
typedef struct
{
    D_UINT          inst;
    D_WORD          line;
    D_WORD          hs;
    D_BYTE          codec[DECT_SS7_CODEC_SIZE_MAX];
    D_UINT          codec_len;
} ss7_msg_anm;

/*
* REL
*/
typedef struct
{
    D_UINT          inst;
    D_UINT          reason;
} ss7_msg_rel;

/*
* SUS
*/
typedef struct
{
    D_UINT          inst;
    D_UINT          holdType;
} ss7_msg_sus;

/*
* RES
*/
typedef struct
{
    D_UINT          inst;
} ss7_msg_res;

/*
* INF
*/
typedef struct
{
    D_UINT          inst;
    D_UINT          inst_other;
    D_WORD          line;
    D_BYTE          type;
    D_BYTE          call_possible;
    D_CHAR          src[DECT_SS7_SRC_SIZE_MAX];
    D_CHAR          name[DECT_SS7_NAME_SIZE_MAX];
    D_BYTE          codec[DECT_SS7_CODEC_SIZE_MAX];
    D_UINT          codec_len;
    D_BYTE          info[DECT_SS7_INFO_SIZE_MAX];
    D_UINT          info_len;
    D_CHAR          multi_name[DECT_SS7_INST_NUM_MAX][DECT_SS7_NAME_SIZE_MAX]; /* CFG_SUPPORT_MULTI_CNIP */
    D_UINT          multi_len[DECT_SS7_INST_NUM_MAX];
} ss7_msg_inf;

/*
* CHST
*/
typedef struct
{
    D_UINT  inst;
    D_WORD  chan;
    D_WORD  state;
} ss7_msg_chst;

/*
* SUS ACK
*/
typedef struct
{
    D_UINT          inst;
} ss7_msg_sus_ack;

/*
* RES ACK
*/
typedef struct
{
    D_UINT          inst;
} ss7_msg_res_ack;

/*
* RTP-EVETN REQUEST
*/
typedef struct
{
    D_UINT inst;
    D_WORD chan;
    D_WORD reserve;
    D_UINT nofify_opts;
}ss7_msg_rtp_evt_req;

/*
* RTP-EVETN RESPONSE
*/
typedef struct
{
    D_UINT inst;
    D_WORD chan;
    D_WORD evt_len;
    D_INT  evt_type;
    union
    {
        //RTCP_HDR        rtcp_pkt;
        D_BYTE          rtcp_pkt[500];
        //RTP_EVENT       rtp_evt;
        D_BYTE          rtp_evt[200];
        //RTP_VOICE_STATS rtp_voice_stats;
        D_BYTE          rtp_voice_stats[300];
    }u;
}ss7_msg_rtp_evt_res;

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* msg union
*/
typedef union
{
    ss7_msg_iam         iam;
    ss7_msg_sam         sam;
    ss7_msg_acm         acm;
    ss7_msg_anm         anm;
    ss7_msg_rel         rel;
    ss7_msg_sus         sus;
    ss7_msg_res         res;
    ss7_msg_inf         inf;
    ss7_msg_chst        chst;
    ss7_msg_sus_ack     sus_ack;
    ss7_msg_res_ack     res_ack;
    ss7_msg_rtp_evt_req evt_req;
    ss7_msg_rtp_evt_res evt_res;
} ss7_body;

/*
* msg struct
*/
typedef struct
{
    msg_hdr    hdr;
    ss7_body   body;
} ss7_msg;


/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /* __DECT_SS7_H__ */
