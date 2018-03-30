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
#ifndef __DECT_DSR_H__
#define __DECT_DSR_H__

#include "dect_la.h"

/************************************************************************/
/*     for dsr packet type                                              */
/************************************************************************/
//#pragma  pack(push,1)

/************************************************************************/
/*                                                                      */
/************************************************************************/


/* Notify Date Time Coding*/
#define  NOTIFY_DT_TIME                          0x1
#define  NOTIFY_DT_DATE                          0x2
#define  NOTIFY_DT_DATE_TIME                     0x3

/* Notify Date Time Interpretation*/
#define  NOTIFY_DT_CURRENT_TIME                 0x00
#define  NOTIFY_DT_TIME_DURATION                0x01
#define  NOTIFY_DT_MMS_DELIVER                  0x20
#define  NOTIFY_DT_MMS_DATA_CREATED             0x21
#define  NOTIFY_DT_MMS_DATA_MODIFIED            0x22
#define  NOTIFY_DT_RECEIVED_MCE                 0x23
#define  NOTIFY_DT_END_ENTITY                   0x24
#define  NOTIFY_DT_IDENTIFIER                   0x28
/* Notify Missed Call SubType */
#define  NOTIFY_MC_SUBTYPE_UNKNOWN              0x80
#define  NOTIFY_MC_SUBTYPE_NEW                  0x81
#define  NOTIFY_MC_SUBTYPE_UPDATED              0x82

/* Notify message waiting SubType */
#define  NOTIFY_MW_SUBTYPE_UNKNOWN              0x80
#define  NOTIFY_MW_SUBTYPE_VOICE_MSG            0x81
#define  NOTIFY_MW_SUBTYPE_SMS_MSG              0x82
#define  NOTIFY_MW_SUBTYPE_EMAIL_MSG            0x83

/* phone cfg mask */
#define  PC_MK_BIND_ON_HOOK              0x00000001
#define  PC_MK_SA_ON_HOOK                0x00000002
#define  PC_MK_EMERGENCY_NUMB            0x00000004
#define  PC_MK_NETWORK_CALLLOG           0x00000008
#define  PC_MK_SUPPRESS_DTMF_DISPLAY            0x00000010
#define  PC_MK_SUPPRESS_DTMF_DISPLAY_DELAY      0x00000020
#define  PC_MK_DISABLE_CALL_END_TONE            0x00000040
#define  PC_MK_AUTO_INTERCOM_TO_HEADSET         0x00000080
#define  PC_MK_DISP_RESET_LOCAL_CFG             0x00000100
#define  PC_MK_DISABLE_VOICE_MAIL_TONE          0x00000200
#define  PC_MK_ENABLE_CF_CHANNEL                0x00000400
#define  PC_MK_CALL_END_TONE_5S                 0x00000800
#define  PC_MK_DISABLE_ANONYMOUS_CALL           0x00001000
#define  PC_MK_ENABLE_CALL_INFO_EVENT           0x00002000
#define  PC_MK_ENABLE_RING_CENTRAL_VOICE_MAIL   0x00004000
#define  PC_MK_DISP_LOGON_WIZARD                0x00008000
#define  PC_MK_ENABLE_GAMMA_PINCODE             0x00010000
#define  PC_MK_DISABLE_ANON_CALL_REJECT         0x00020000
#define  PC_MK_DISABLE_CALL_WAITING             0x00040000
#define  PC_MK_ENABLE_REPEATER_PINCODE          0x00080000
#define  PC_MK_ENABLE_POUND_AS_SEND             0x00100000
#define  PC_MK_DISABLE_BLACK_CALL_LIST          0x00200000
#define  PC_MK_DISABLE_SET_VCM                  0x00400000
#define  PC_MK_DISABLE_PRIVATE_HOLD             0x00800000
#define  PC_MK_ENABLE_STAR_AS_SEND              0x01000000
#define  PC_MK_DISABLE_SCA_BARGE_IN             0x02000000
#define  PC_MK_ENABLE_HELD_CONFERENCE           0x04000000
#define  PC_MK_DISABLE_DEFUALT_LINES_SET        0x08000000
#define  PC_MK_DISABLE_INCOMING_LINES_SET       0x10000000

#define  PC_MK_ALL                       (PC_MK_BIND_ON_HOOK|PC_MK_SA_ON_HOOK|PC_MK_EMERGENCY_NUMB|PC_MK_NETWORK_CALLLOG|PC_MK_SUPPRESS_DTMF_DISPLAY|PC_MK_SUPPRESS_DTMF_DISPLAY_DELAY|PC_MK_DISABLE_CALL_END_TONE|PC_MK_AUTO_INTERCOM_TO_HEADSET|PC_MK_DISP_RESET_LOCAL_CFG|PC_MK_DISABLE_VOICE_MAIL_TONE|PC_MK_ENABLE_CF_CHANNEL|PC_MK_CALL_END_TONE_5S|PC_MK_DISABLE_ANONYMOUS_CALL|PC_MK_ENABLE_CALL_INFO_EVENT|PC_MK_ENABLE_RING_CENTRAL_VOICE_MAIL|PC_MK_DISP_LOGON_WIZARD|PC_MK_ENABLE_GAMMA_PINCODE|PC_MK_DISABLE_ANON_CALL_REJECT|PC_MK_DISABLE_CALL_WAITING|PC_MK_ENABLE_REPEATER_PINCODE|PC_MK_ENABLE_POUND_AS_SEND|PC_MK_DISABLE_BLACK_CALL_LIST|PC_MK_DISABLE_SET_VCM|PC_MK_DISABLE_PRIVATE_HOLD|PC_MK_ENABLE_STAR_AS_SEND|PC_MK_DISABLE_SCA_BARGE_IN|PC_MK_ENABLE_HELD_CONFERENCE|PC_MK_DISABLE_DEFUALT_LINES_SET|PC_MK_DISABLE_INCOMING_LINES_SET)

/*Phone Cfg2 Mask*/
#define  PC2_MK_ENABLE_DIAL_NOW                 0x00000001
#define  PC2_MK_ENABLE_RING_LIGHT               0x00000002
#define  PC2_MK_ENABLE_POWER_LIGHTENCE          0x00000004
#define  PC2_MK_ENABLE_PREDIAL_SEARCHE          0x00000008
#define  PC2_MK_ENABLE_MISSCALL_LIGHT           0x00000010
#define  PC2_MK_ENABLE_MAIL_LIGHT               0x00000020
#define  PC2_MK_ENABLE_PLUG_CHARGE_END_CALL     0x00000040
#define  PC2_MK_SEARCH_CONTACT_INPUT_METHOD     0x00000080
#define  PC2_MK_ENABLE_RING_MESSAGE_DISPLAY     0x00000100
#define  PC2_MK_CALL_DISPLAY_METHOD             0x00000200

#define  PC2_MK_ALL                      (PC2_MK_ENABLE_DIAL_NOW|PC2_MK_ENABLE_RING_LIGHT|PC2_MK_ENABLE_POWER_LIGHTENCE|PC2_MK_ENABLE_PREDIAL_SEARCHE|PC2_MK_ENABLE_MISSCALL_LIGHT|PC2_MK_ENABLE_MAIL_LIGHT|PC2_MK_ENABLE_PLUG_CHARGE_END_CALL|PC2_MK_SEARCH_CONTACT_INPUT_METHOD|PC2_MK_ENABLE_RING_MESSAGE_DISPLAY|PC2_MK_CALL_DISPLAY_METHOD)

/* handset local cfg mask */
#define  HSC_MK_TIME_FORMAT                      0x00000001
#define  HSC_MK_DATE_FORMAT                      0x00000002
#define  HSC_MK_GROUP_DATE_TIME                  (HSC_MK_TIME_FORMAT|HSC_MK_DATE_FORMAT)

#define  HSC_MK_LANGUAGE                         0x00000004
#define  HSC_MK_INPUT_METHOD                     0x00000008
#define  HSC_MK_GROUP_LANGUAGE                   (HSC_MK_LANGUAGE|HSC_MK_LANGUAGE)

#define  HSC_MK_SCREEN_SAVER                     0x00000010
#define  HSC_MK_COLOR_SCHEMES                    0x00000020
#define  HSC_MK_DISP_BACKLIGHT_IN_CHARGER        0x00000040
#define  HSC_MK_DISP_BACKLIGHT_OUTOF_CHARGER     0x00000080
#define  HSC_MK_KEYPAD_LIGHT                     0x00000100
#define  HSC_MK_VOICE_MAIL_NOTIFY_LIGHT          0x00000200
#define  HSC_MK_MISS_CALL_NOTIFY_LIGHT           0x00000400
#define  HSC_MK_GROUP_DISPLAY                    (HSC_MK_SCREEN_SAVER|HSC_MK_COLOR_SCHEMES|HSC_MK_DISP_BACKLIGHT_IN_CHARGER|HSC_MK_DISP_BACKLIGHT_IN_CHARGER|HSC_MK_KEYPAD_LIGHT|HSC_MK_VOICE_MAIL_NOTIFY_LIGHT|HSC_MK_MISS_CALL_NOTIFY_LIGHT)

#define  HSC_MK_KEYPAD_TONE                      0x00000800
#define  HSC_MK_CONFIRMATION_TONE                0x00001000
#define  HSC_MK_LOW_BATTERY_TONE                 0x00002000
#define  HSC_MK_GROUP_TONE                       (HSC_MK_KEYPAD_TONE|HSC_MK_CONFIRMATION_TONE|HSC_MK_LOW_BATTERY_TONE)

#define  HSC_MK_AUTO_ANSWER                      0x00004000
#define  HSC_MK_AUTO_INTERCOM                    0x00008000
#define  HSC_MK_GROUP_TELEPHONY                  (HSC_MK_AUTO_ANSWER|HSC_MK_AUTO_INTERCOM)

#define  HSC_MK_ECO_MODE                         0x00010000
#define  HSC_MK_OTHER_SETTING                    (HSC_MK_ECO_MODE)
/*handset status mask . en_HandsetStatusMask.*/
#define HSS_MASK_IN_CALL            0x0001
#define HSS_MASK_IN_REPEATER        0x0002
#define HSS_MASK_BAD_DFLASH_IMAGE   0x0004
#define HSS_MASK_ALL                0xffff


/*handset capability mask. en_HandsetCapabilityMask.*/
#define HSC_MASK_CATIQ_1            0x0001
#define HSC_MASK_CATIQ_2            0x0002
#define HSC_MASK_CATIQ_3            0x0004
#define HSC_MASK_SUOTA              0x0008
#define HSC_MASK_ALL                0xffff


#define  RPS_START                       0
#define  RPS_END                         1
#define  RPS_NOTIFY_FAILED               2
#define  RPS_NOTIFY_SUCCESS              3

#define  RPS_KEEPALIVE                   0
#define  RPS_SUCCESS                     1
#define  RPS_FAILED                      2
#define  RPS_REG_FAILED                  3
#define  RPS_NW_UNAVAILABLE              4
#define  RPS_TIME_OUT                    5


#define  NET_ST_UNLINK                   0   /* unlink */
#define  NET_ST_LINK                     1   /* link */
#define  NET_ST_REQUEST_PROC             2   /* request processing */
#define  NET_ST_REQUEST_FAIL             3   /* request fail */
#define  NET_ST_REQUEST_DONE             4   /* request done */
#define  NET_ST_IP_INVALID               5   /* ip invalid */
#define  NET_ST_IP_CONFLICT              6   /* ip conflict */
#define  NET_ST_IP_VALID                 7   /* ip valid */
#define  NET_ST_IP_VPN                   8   /* VPN Type */

#define  BASE_RESET_MODE_ALL             0  /* 全恢复出厂设置:清除使用记录和对码信息.*/
#define  BASE_RESET_MODE_PARTLY          1  /* 半恢复出厂设置:只清除记录，保留对码信息.*/
#define  BASE_RESET_LOCAL_CONFIG         2  /* Reset Local config.*/

#define  DSR_HS_TYPE_GAP                 0    /*!< GAP handset */
#define  DSR_HS_TYPE_CATIQ_1             1    /*!< CATiq compliant handset */
#define  DSR_HS_TYPE_CATIQ_2             2   /*!< CATiq compliant handset */
#define  DSR_HS_TYPE_CATIQ_3             3   /*!< CATiq compliant handset */
#define  DSR_HS_TYPE_MAX                 4


#define  DSR_HS_CAPABILITY_TYPE_NONE           0x000000000
#define  DSR_HS_CAPABILITY_TYPE_GAP            (1<<0)   /*!< GAP handset */
#define  DSR_HS_CAPABILITY_TYPE_CATIQ_1        (1<<1)   /*!< CATiq compliant handset */
#define  DSR_HS_CAPABILITY_TYPE_CATIQ_2        (1<<2)   /*!< CATiq compliant handset */
#define  DSR_HS_CAPABILITY_TYPE_CATIQ_3        (1<<3)   /*!< CATiq compliant handset */

#define  DSR_HS_LOCAT_TYPE_FIRST_CONNECT       1
#define  DSR_HS_LOCAT_TYPE_NORMAL_RECONNECT    2
#define  DSR_HS_LOCAT_TYPE_QUICK_RECONNECT     3

#define  DSR_DTM_TYPE_STANDARD                 0    /*!< standard-dial-tone */
#define  DSR_DTM_TYPE_SPECIAL_CONDITION        1    /*!< special-condition-tone */
#define  DSR_DTM_TYPE_MESSAGE_WATING           2   /*!< message-waiting-tone */

#define  SU_CMD_BS_REQ_UPGRADE_QUERY        1
#define  SU_CMD_BS_REQ_UPGRADE_FORCE        2
#define  SU_CMD_HS_RESPONSE_UPGRADE_ACK     3
#define  SU_CMD_HS_RESPONSE_UPGRADE_NACK    4
#define  SU_CMD_BS_REQ_SHOW_NEW_VER_ON      5
#define  SU_CMD_BS_REQ_SHOW_NEW_VER_OFF     6

#define  SU_CMD_BS_REQ_HS_VER                   8
#define  SU_CMD_HS_RESPONSE_HS_VER              9

#define  SU_CMD_HS_REQ_UPGRADE                  10
#define  SU_CMD_BS_RESPONSE_UPGRADE_ACK_QUERY   11
#define  SU_CMD_BS_RESPONSE_UPGRADE_ACK_FORCE   12
#define  SU_CMD_BS_RESPONSE_UPGRADE_NACK        13 /* base not allow to upgrade */
#define  SU_CMD_BS_RESPONSE_UPGRADE_NOT_AVAL    14 /* handset rom not avaliable */
#define  SU_CMD_BS_RESPONSE_UPGRADE_NEWEST_VER  15 /* handset's version is newest*/
#define  SU_CMD_BS_RESPONSE_UPGRADE_OEM_NOT_MATCH 16/* OEM not match*/
#define  SU_CMD_BS_RESPONSE_UPGRADE_BASE_TALKING  17/* Base is talking*/
#define  SU_CMD_BS_RESPONSE_UPGRADE_NOT_AVAL_WEB_UPD    22

#define  SU_CMD_HS_REQ_NEW_VER                  18
#define  SU_CMD_BS_RESPONSE_HS_NEW_VER          19

#define  SU_CMD_BS_REQ_HS_REBOOT                20
#define  SU_CMD_HS_RESPONSE_HS_REBOOT           21

#define  SU_MAX_VER_LEN                 16 
#define  SU_MAX_NAME_LEN                16 


#define  NOTIFY_BS_MAX_NAME_LEN                 40 
#define  NOTIFY_BS_TYPE_AUTO_P_CODE             1

#define  NOTIFY_BS_PRIORITY_1                   0
#define  NOTIFY_BS_PRIORITY_2                   1
#define  NOTIFY_BS_PRIORITY_3                   2
#define  NOTIFY_BS_PRIORITY_4                   3

#define  DSR_BSS_MAGIC                          0xa6e78c5A
#define  DSR_MAX_HS_COUNT                       5
#define  DSR_MAX_GENERIC_BUF_SIZE               512

#define  DEV_CERT_NOT_INSTALLED                 0   /* not installed */
#define  DEV_CERT_INVALID                       1   /* invalid*/
#define  DEV_CERT_FACTORY_INSTALLED             2   /* factory installed*/

#define  DSR_LOGIN_REQ_CANCEL                   0
#define  DSR_LOGIN_REQ_USER_NAME                1
#define  DSR_LOGIN_REQ_PASSWD                   2
#define  DSR_LOGIN_REQ_USER_NAME_PASSWD         3

#define  DSR_LOGIN_TYPE_RPS                     1
#define  DSR_LOGIN_TYPE_PPPOE                   2
#define  DSR_LOGIN_TYPE_AUTOP_CODE              3
#define  DSR_LOGIN_TYPE_BROADSOFT_BASE          10
#define  DSR_LOGIN_TYPE_BROADSOFT_CHI           11
#define  DSR_LOGIN_TYPE_BROADSOFT_DIR           12
#define  DSR_LOGIN_TYPE_BROADSOFT_SIP_CHI       13
#define  DSR_LOGIN_TYPE_BROADSOFT_SIP_DIR       14
#define  DSR_LOGIN_TYPE_BROADSOFT_MAX           20
#define  DSR_LOGIN_TYPE_WIZARD                  21
#define  DSR_LOGIN_TYPE_COMMPORTAL              22

#define  DSR_LOGIN_TYPE_USER_MODE_ADMIN         23
#define  DSR_LOGIN_TYPE_USER_MODE_VAR           24
#define  DSR_LOGIN_TYPE_USER_MODE_USER          25

#define  DSR_PINCODE_TYPE_NORMAL                35
#define  DSR_PINCODE_TYPE_DEREGISTER            36
#define  DSR_PINCODE_TYPE_MAX                   50
/************************************************************************/
/*                                                                      */
/************************************************************************/

#define E_STORAGE_TYPE_UNKNOWN 0
#define E_STORAGE_TYPE_EEPROM  1
#define E_STORAGE_TYPE_MAX     2

#define E_PARAM_TAG_UNKNOWN            0
#define E_PARAM_TAG_RFPI               1
#define E_PARAM_TAG_RVBG               2
#define E_PARAM_TAG_RVREF              3        /*!< VREF register */
#define E_PARAM_TAG_RXTUN              4        /*!< RTUN register */
#define E_PARAM_TAG_MASTER_PIN         5        /*!< Base master PIN code */
#define E_PARAM_TAG_AUTH_PIN           6        /*!< Authentication PIN code */
#define E_PARAM_TAG_COUNTRY            7        /*!< Configure cordless module to specific country settings */
#define E_PARAM_TAG_SIGNALTONE_DEFAULT 8        /*!< Define the default behavior for outgoing calls */
#define E_PARAM_TAG_CLMS_CATIQ         9
#define E_PARAM_TAG_TEST_MODE          10       /*!< Test mode. 0x00: Normal operation; 0x81: TBR6; 0x82: TBR10 */
#define E_PARAM_TAG_SUB_DATA           11
#define E_PARAM_TAG_RVDDC              12
#define E_PARAM_TAG_US_DECT            13       /*!< US DECT*/
#define E_PARAM_TAG_ALLOC_LINE1        14
#define E_PARAM_TAG_ALLOC_LINE2        15
#define E_PARAM_TAG_ALLOC_LINE3        16
#define E_PARAM_TAG_DSP_CLOCK          17
#define E_PARAM_TAG_FLEXIBLE           18      /*!< User defined parameter position */
#define E_PARAM_CUSTOMER_EXTENSION_1   19
#define E_PARAM_CUSTOMER_EXTENSION_2   20
#define E_PARAM_CUSTOMER_EXTENSION_N   21
#define E_PARAM_TAG_MAX                22

#define E_FUNCTION_PARAM_TAG_CALL_DISABLE 1
#define E_FUNCTION_PARAM_TAG_TRANS_OTHER_AFTER_CONF_ENABLE 2
#define E_FUNCTION_PARAM_TAG_CODEC_CONFIG 3
#define E_FUNCTION_PARAM_TAG_DATETIME_ENABLE 4
#define E_FUNCTION_PARAM_TAG_NOTIFY_UNREACHABLE_CNT_THRD 5
#define E_FUNCTION_PARAM_TAG_RTCP_BYE_DISABLE 6
#define E_FUNCTION_PARAM_TAG_IAM_DECT_PAGE_INTERVAL_TICK 7
#define E_FUNCTION_PARAM_TAG_SUOTA_ENABLE 8


/************************************************************************/
/*                                                                      */
/************************************************************************/

/*COMMPORTAL TIP TYPE*/
#define COMMPORTAL_NONE             0
#define COMMPORTAL_USER_PWD_ERR     1


/*more data to hs*/
#define MORE_DATA_TO_HS_MAGIC       0xC1288E50
#define MORE_DATA_TO_HS_MASK_SIP_XSI_LINE_ID        0X00000001
#define MORE_DATA_TO_HS_MASK_COMMPORTAL_POP_TIP_TYPE     0X00000002

/*more data from hs*/
#define MORE_DATA_FROM_HS_MAGIC     0xA1E590AC
#define MORE_DATA_FROM_HS_MASK_CANCEL_LOGIN_TYPE    0X00000001




/************************************************************************/
/*                                                                      */
/************************************************************************/
#define DSR_API_RESULT_SUCCESS   1
#define DSR_API_RESULT_FAIL     -1

#define DSR_API_FAILURE_OK       0
#define DSR_API_FAILURE_INVALID  1
#define DSR_API_FAILURE_GENERIC  2
#define DSR_API_FAILURE_UNKNOWN  3


/************************************************************************/
/*                                                                      */
/************************************************************************/

#define   DSR_CMD_HELLO                               0
#define   DSR_CMD_PAGE_HS                             1
#define   DSR_CMD_PAGEOFF_HS                          2
#define   DSR_CMD_OPEN_BASE_REG_MODE                  3
#define   DSR_CMD_CLOSE_BASE_REG_MODE                 4
#define   DSR_CMD_DEREG_HS                            5
#define   DSR_CMD_GET_BASE_OPERATION_STATUS           6
#define   DSR_CMD_GET_BASE_SOFTWARE_VERSION           7
#define   DSR_CMD_CLMS_TO_HS                          8
#define   DSR_CMD_IWU_TO_HS                           9
#define   DSR_CMD_GET_LIST_OF_EEPROM_VALUES           10
#define   DSR_CMD_SET_LIST_OF_EEPROM_VALUES 	      11
#define   DSR_CMD_EEPROM_RESET                        12
#define   DSR_CMD_GET_PARAM                           13
#define   DSR_CMD_SET_PARAM                           14
#define   DSR_CMD_UPDATE_PARAM                        15
#define   DSR_CMD_LINE_STATUS                         16
#define   DSR_CMD_HS_STATUS                           17
#define   DSR_CMD_HS_TABLE                            18
#define   DSR_CMD_BASE_REG_LED_OFF                    19
#define   DSR_CMD_BASE_REG_LED_ON                     20
#define   DSR_CMD_CLEAR_COMMAND_STATUS                21
#define   DSR_CMD_ENTER_LOW_POWER_MODE                22
#define   DSR_CMD_EXIT_LOW_POWER_MODE                 23
#define   DSR_CMD_API_RESPONSE                        24   // response originates from coma space	24
#define   DSR_CMD_DEBUG_LOG                           28   // Debug logs to userspace	28
#define   DSR_CMD_KEY_PRESS                           29   // handeling the key press events	29
#define   DSR_CMD_EEPROM_RESET_DONE                   30   // handling eeprom reset done event	30
#define   DSR_CMD_NO_LINES                            32   // handling No of Lines[PSTN+IOM+SIP+USB]	32
#define   DSR_CMD_CALL_MODE                           33   // Setting Call Mode to Multi / Single	33
#define   DSR_CMD_HS_REG_NOTIF               	      34
#define   DSR_CMD_HS_DEREG_NOTIF             	      35
#define   DSR_CMD_LA_SESSION_START                    36
#define   DSR_CMD_LA_SESSION_START_RESPONSE 	      37
#define   DSR_CMD_LA_SESSION_END                      38
#define   DSR_CMD_LA_SESSION_END_RESPONSE   	      39
#define   DSR_CMD_LA_SESSION_END_FROM_BASE  	      40
#define   DSR_CMD_LA_SESSION_END_FROM_BASE_RESPONSE   41
#define   DSR_CMD_LA_QUERY_SUP_ENTRY_FIELDS           42
#define   DSR_CMD_LA_QUERY_SUP_ENTRY_FIELDS_RESPONSE  43
#define   DSR_CMD_LA_SEARCH_ENTRIES                   44
#define   DSR_CMD_LA_SEARCH_ENTRIES_RESPONSE          45
#define   DSR_CMD_LA_SEARCH_START                     46
#define   DSR_CMD_LA_SEARCH_START_RESPONSE            47
#define   DSR_CMD_LA_SEARCH_END                       48
#define   DSR_CMD_LA_SEARCH_END_RESPONSE              49
#define   DSR_CMD_LA_SEARCH_MATCH                     50
#define   DSR_CMD_LA_SEARCH_MATCH_RESPONSE            51
#define   DSR_CMD_LA_READ_ENTRIES                     52
#define   DSR_CMD_LA_READ_ENTRIES_RESPONSE            53
#define   DSR_CMD_LA_EDIT_ENTRY                       54
#define   DSR_CMD_LA_EDIT_ENTRY_RESPONSE              55
#define   DSR_CMD_LA_SAVE_ENTRY                       56
#define   DSR_CMD_LA_SAVE_ENTRY_REPONSE               57
#define   DSR_CMD_LA_DELETE_ENTRY                     58
#define   DSR_CMD_LA_DELETE_ENTRY_RESPONSE            59
#define   DSR_CMD_LA_DELETE_LIST                      60
#define   DSR_CMD_LA_DELETE_LIST_RESPONSE             61
#define   DSR_CMD_LA_LIST_CHANGE_REQUEST              62
#define   DSR_CMD_LA_GEN_NOTIFY                       63
#define   DSR_CMD_LA_SEND_ENTRY                       64
#define   DSR_CMD_LA_RECV_ENTRY                       65
#define   DSR_CMD_LA_RESPONSE                         66
#define   DSR_CMD_LA_NACK                             67
#define   DSR_CMD_LA_GEN_NOTIFY_RESULT                68
#define   DSR_CMD_LINE_SETTING                        69
#define   DSR_CMD_GET_EP_CHANNEL_ID                   70
#define   DSR_CMD_LA_RAW_PACKET                       71
#define   DSR_CMD_PAGING_RELEASE                      72
#define   DSR_CMD_HS_GENERIC_EVENT                    73
#define   DSR_CMD_SET_FUNCTION_PARAM                  74
#define   DSR_CMD_GET_FUNCTION_PARAM                  75
#define   DSR_CMD_COMA_ALIVE_NOTIFY                   76
#define   DSR_CMD_COMA_SHUTDOWN_NOTIFY                77
#define   DSR_CMD_HS_ALIVE_NOTIFY                     78
#define   DSR_CMD_HS_SHUTDOWN_NOTIFY                  79
#define   DSR_CMD_LA_SET_HS_LA_TIMER                  80
#define   DSR_CMD_LA_UPDATE                           81
#define   DSR_CMD_LA_UPDATE_RESPONSE                  82
#define   DSR_CMD_COMA_DUMP_STAT                      83

#define   DSR_CMD_UNKNOWN                             84

/************************************************************************/
/*                                                                      */
/************************************************************************/
#define   GET_BALANCE_SUCCESS    0x1
#define   GET_BALANCE_FAILED     0x0

/************************************************************************/
/*            update type                                         */
/************************************************************************/
#define   DSR_UPDATE_PLCM_DIR             1
#define   DSR_UPDATE_MTSW_NETWORK_DIR     2

#define LA_CP_CODE_LEN           8 /*CALL PARK CODE LEN*/
#define LA_CP_NUMBER_LEN         24 /*CALL PARK NUMBER LEN*/

/************************************************************************/
/*                 Hs Permission ITEM                                   */
/************************************************************************/

typedef enum
{
    DSR_HS_PM_RESERVE   = 0,

    /*Status*/
    DSR_HS_PM_ST        = 1,
    DSR_HS_PM_ST_BASE,
    DSR_HS_PM_ST_HS,
    DSR_HS_PM_ST_LINE_STATUS,

    /*Intercom*/
    DSR_HS_PM_IN        = 21,
    DSR_HS_PM_IN_HS1_HS5,
    DSR_HS_PM_IN_ALL_HS,

    /*Voice Mail*/
    DSR_HS_PM_VM        = 31,
    DSR_HS_PM_VM_PLAY,
    DSR_HS_PM_VM_SET,
    DSR_HS_PM_VM_SET_KEY_1,
    
    /*Call Feature*/
    DSR_HS_PM_CF        = 41,
    DSR_HS_PM_CF_CF,
    DSR_HS_PM_CF_CF_ALWAYS,
    DSR_HS_PM_CF_CF_BUSY,
    DSR_HS_PM_CF_CF_NO_ANSWER,
    DSR_HS_PM_CF_DND,
    DSR_HS_PM_CF_CW,
    DSR_HS_PM_CF_ANON_CALL,
    DSR_HS_PM_CF_ANON_REJ,
    DSR_HS_PM_CF_GET_BALANCE,

    /*PhoneBook*/
    DSR_HS_PM_DIR       = 61,
    DSR_HS_PM_DIR_LOCAL,
    DSR_HS_PM_DIR_BS,
    DSR_HS_PM_DIR_LDAP,
    DSR_HS_PM_DIR_REMOTE,
    DSR_HS_PM_DIR_SEARCH,
    DSR_HS_PM_DIR_NETWORK_DIR,
    DSR_HS_PM_DIR_VIEW,
    DSR_HS_PM_DIR_OPTION,
    DSR_HS_PM_DIR_OPTION_NEW,
    DSR_HS_PM_DIR_OPTION_EDIT,
    DSR_HS_PM_DIR_OPTION_DELETE,
    DSR_HS_PM_DIR_OPTION_DELETE_ALL,
    DSR_HS_PM_DIR_OPTION_ADD_TO_LOCAL,
    DSR_HS_PM_DIR_OPTION_ADD_TO_BLACKLIST,
    DSR_HS_PM_DIR_OPTION_ADD_TO_PERSONAL,
    DSR_HS_PM_DIR_OPTION_UPDATE,
    DSR_HS_PM_DIR_OPTION_EDIT_BEFORE_CALL,

    /*Settings*/
    DSR_HS_PM_SETTINGS  = 91,
    DSR_HS_PM_SET_DATE_TIME,
        /*Audio*/
    DSR_HS_PM_SET_AUDIO,
    DSR_HS_PM_SET_AUD_AT,
    DSR_HS_PM_SET_AUD_RT,
    DSR_HS_PM_SET_AUD_RT_VOL,
    DSR_HS_PM_SET_AUD_RT_MEL,
        /*Display*/
    DSR_HS_PM_SET_DISP,
    DSR_HS_PM_SET_DISP_SAVER,
    DSR_HS_PM_SET_DISP_SCHEMES,
    DSR_HS_PM_SET_DISP_WALLPAPER,
    DSR_HS_PM_SET_DISP_BACK_LIGHT,
    DSR_HS_PM_SET_DISP_KEY_LIGHT,
    DSR_HS_PM_SET_DISP_NOTIFY_LIGHT,
    DSR_HS_PM_SET_DISP_TIME_FORMAT,
    DSR_HS_PM_SET_DISP_DATE_FORMAT,
    DSR_HS_PM_SET_DISP_INPUT_METHOD,
        /*Shortcut*/
    DSR_HS_PM_SET_SHORTCUT,
    DSR_HS_PM_SET_SHORTCUT_SOFT_LEFT,
    DSR_HS_PM_SET_SHORTCUT_SOFT_RIGHT,
    DSR_HS_PM_SET_SHORTCUT_UP,
    DSR_HS_PM_SET_SHORTCUT_DOWN,
    DSR_HS_PM_SET_SHORTCUT_LEFT,
    DSR_HS_PM_SET_SHORTCUT_RIGHT,
        /*Language*/
    DSR_HS_PM_SET_LANGUAGE,
        /*Register*/
    DSR_HS_PM_SET_REG,
    DSR_HS_PM_SET_REG_HANSET,
    DSR_HS_PM_SET_REG_DE_REG,
    DSR_HS_PM_SET_REG_SEL_BASE,
        /*Telephony*/
    DSR_HS_PM_SET_TEL,
    DSR_HS_PM_SET_TEL_AUTO_ANSWER,
    DSR_HS_PM_SET_TEL_AUTO_INTERCOM,
    DSR_HS_PM_SET_TEL_DEFAULT_LINE,
    DSR_HS_PM_SET_TEL_INCOMING_LINE,
    DSR_HS_PM_SET_TEL_SPEED_DIRAL,
    DSR_HS_PM_SET_TEL_BLACKLIST,
        /*System Settings*/
    DSR_HS_PM_SET_SYS,
    DSR_HS_PM_SET_SYS_NETWORK,
    DSR_HS_PM_SET_SYS_HS_RESET,
    DSR_HS_PM_SET_SYS_CHANGE_PIN,
    DSR_HS_PM_SET_SYS_BASE_RESET,
    DSR_HS_PM_SET_SYS_BASE_RESET_LOCAL_CFG,
    DSR_HS_PM_SET_SYS_BASE_RESTART,
    DSR_HS_PM_SET_SYS_AUTOP,
    DSR_HS_PM_SET_SYS_ECO_PLUS,
    DSR_HS_PM_SET_SYS_ECO,
    DSR_HS_PM_SET_SYS_REPEATER,
    DSR_HS_PM_SET_SYS_LOGON_WIZARD,
    DSR_HS_PM_SET_SYS_HS_USE_DECT_INVERT,
    DSR_HS_PM_SET_SYS_BASE_USE_DECT_INVERT,
        /*Handset Name*/
    DSR_HS_PM_SET_HS_NAME,
        /*Upgrade Firmware*/
    DSR_HS_PM_SET_UPGRADE,

    /*History*/
    DSR_HS_PM_HISTORY   = 171,
    DSR_HS_PM_HISTORY_LOCAL,
    DSR_HS_PM_HISTORY_NETWORK,
    DSR_HS_PM_HISTORY_ALL,
    DSR_HS_PM_HISTORY_MISSED,
    DSR_HS_PM_HISTORY_PLACED,
    DSR_HS_PM_HISTORY_RECIEVED,
    DSR_HS_PM_HISTORY_OPTION,
    DSR_HS_PM_HISTORY_VIEW,

    /*User Mode*/
    DSR_HS_PM_USER_MODE = 191,

    /*MAX*/
    DSR_HS_PM_INDEX_MAX = 199,
}en_HS_PermissionItem;


#define   DSR_HS_PM_VALUE_READ_WRITE     0
#define   DSR_HS_PM_VALUE_READ_ONLY      1
#define   DSR_HS_PM_VALUE_HIDE           2
#define   DSR_HS_PM_VALUE_MAX            3

#define	  DSR_HS_PM_TYPE_OFF			 0
#define	  DSR_HS_PM_TYPE_ADMIN			 1
#define	  DSR_HS_PM_TYPE_VAR			 2
#define	  DSR_HS_PM_TYPE_USER			 3

typedef struct
{
    D_BYTE u8_IPProp;
    D_BYTE u8_IPAddress[32];
}LA_IP_Address;

//enum for status of base
typedef struct
{
    D_UINT magic;
    D_BYTE RegistrationModeActive:1;    // is base is in subscription mode
    D_BYTE VoiceCallsActive:1;          // is voice call is in Active
    D_BYTE DataCallsActive:1;           // is data call is in Active
    D_BYTE FatalError:1;
    D_BYTE ReserveBits:4;
    D_BYTE ReserveBytes[3];
}LA_BS_Status;

typedef struct
{
    /* length of data buffer */
    D_BYTE HsNo;
    D_BYTE HsSubscribe;
    D_BYTE HsAlive;
    D_BYTE HsLink;
    D_BYTE SessionStatus;
    D_BYTE SessionType;
    D_BYTE ReserveBytes[2];
}LA_HS_Status;

typedef struct _dsr_hs_table
{
    LA_HS_Status  hsStatus;
    D_BYTE        IPUI[5];
    D_BYTE        TPUI[3];
    D_BYTE        RSSI;
}LA_HS_Table;

typedef struct
{
    D_WORD  date:      5;
    D_WORD  month:     4;
    D_WORD  year:      5; // to year 2031
    D_WORD  sec:       6;
    D_WORD  min:       6;
    D_WORD  hour:      5;
    D_WORD  timeZone:  8;
    D_WORD  spare:     9;
} LA_DateTime;

typedef struct
{
    D_BYTE	Sec; /* Seconds */
    D_BYTE	Min; /* Minute */
    D_BYTE	Hour; /* Hour */
} LA_Duration ;

typedef struct
{
    D_WORD  pNumber[LA_PHONE_NUM_LEN];
    D_WORD  numLen         : 8;
    D_WORD  numAttr        : 8;
} LA_ContactNumber;

typedef struct
{
    D_BYTE  bForceAdd      : 1;
    D_BYTE  Space          : 7;
    D_BYTE  Reserve1;
    D_BYTE  Reserve2;
    D_BYTE  Reserve3;
} LA_ContactInfo;

typedef struct
{
    D_WORD numListIds;
    D_WORD pListIds[LA_LIST_ID_COUNT];
} LA_SupportedListEntry;

typedef struct
{
    D_WORD  pNumber[LA_PHONE_NUM_LEN];
    D_WORD  pName[LA_PHONE_NAME_LEN];
    //Date and time
    LA_DateTime dateTime;
    D_WORD  lineId         :8; // mandatory
    D_WORD  pLineName[LA_LINE_NAME_LEN]; //mandatory
    D_WORD  lineNameLen    :3;
    D_WORD  numCalls:      4;
    D_WORD  numberLength:  8;
    D_WORD  nameLength:    8;
    D_WORD  readStatus     :1;
    D_WORD  Spare          :12;

} LA_MissedCallsListEntry ;

/* for Handset cfg,only change by web/base */
typedef struct  
{
    D_BYTE bEnableMTSWCallLog      :1;
    D_BYTE bMTSWNetworkCallLog     :1;

    D_BYTE spare                   :6;
    D_BYTE Reserve1;
    D_BYTE Reserve2;
    D_BYTE Reserve3;
}LA_HandsetCfg;

typedef struct
{
    D_BYTE u8_Type;
    D_BYTE u8_Data[LA_HS_PERMISSION_SIZE];
}LA_HsPermission;


typedef struct
{
    D_WORD  Number;
    D_WORD  pName[LA_PHONE_NAME_LEN];
    D_WORD  Spare              :15;
    D_WORD  callInterception   :1;
    D_WORD  nameLength         :8; // 16 chars
    D_WORD  numberLength       :8; // 32 digits
    LA_HandsetCfg  HandsetCfg;/*Handset config*/
    LA_HsPermission HsPermission;
} LA_InternalNamesListEntry ;

typedef struct
{
    D_WORD  pPinCode[( LA_PIN_CODE_LEN+3 ) /4]; // 2 = ( PIN_CODE_LEN+3 ) / 4
    D_WORD  NewPinCode[(LA_PIN_CODE_LEN+3 ) /4];

    // IP Address
    D_BYTE IPType;
    LA_IP_Address IPValue;
    LA_IP_Address CurrIPValue;
    LA_IP_Address IPSubnetMask;
    LA_IP_Address IPGateway;
    LA_IP_Address IPDNSServer[2];

    // FP version
    D_WORD  pFirmwareVersion[LA_FP_VER_LEN];
    D_WORD  pEepromVersion[LA_FP_VER_LEN];
    D_WORD  pHardwareVersion[LA_FP_VER_LEN];
    D_WORD  Mac[LA_FP_VER_LEN];
    D_WORD  Rfpi[LA_FP_VER_LEN];

    D_WORD  LoginType;
    D_WORD  LoginUserName[LA_LOGIN_MAX_LEN];
    D_WORD  LoginPassWd[LA_LOGIN_MAX_LEN];

    D_WORD  IPDNSServerCount:8;
    D_WORD  pinCodeLength   :4;
    D_WORD  NewPinLen       :4;

    D_WORD  fwareVerLen     :8;
    D_WORD  eepromVerLen    :8;
    D_WORD  hwVerLen        :8;
    D_WORD  MacLen          :8;

    D_WORD  LoginUserLen    :8;
    D_WORD  LoginPassWDLen  :8;

    D_WORD  RfpiLen         :8;
    D_WORD  clockMaster     :1;
    D_WORD  baseReset       :1;
    D_WORD  EmissionMode    :1;
    D_WORD  EcoMode         :1;
    D_WORD  RepeateMode     :1;
	D_WORD  US_DECT_INVERT  :1;
    D_WORD  Spare2          :2;
    D_WORD  DeviceCertStatus:8;
    D_WORD  Spare3          :8;
	D_WORD  Spare4          :16;
	D_WORD  StatusMask      :16;
} LA_DectSettingsListEntry;


typedef struct
{
    D_BYTE u8_SCA_On;
    D_BYTE u8_Dummy1;
    D_BYTE u8_Dummy2;
    D_BYTE u8_Dummy3;
}st_la14_LineSetSCA;

typedef struct
{
    D_WORD  pu16_Title[LA_BALANCE_TITLE_LEN+1];
    D_WORD  pu16_Account[LA_BALANCE_ACCOUNT_LEN+1];
    D_WORD  pu16_Currency[LA_BALANCE_CUR_LEN+1];
    D_BYTE  pu8_BalanceValue[LA_BALANCE_VAL_LEN+1];
    D_BYTE  TitleLength;
    D_BYTE  AccountLength;
    D_BYTE  CurrencyLength;
    D_BYTE  ResultFlag;
}st_la14_BalanceListEntry;


typedef struct
{ 
    D_BYTE  u8_Index;
    D_BYTE  u8_Status;
    D_BYTE  u8_LineId;
    D_BYTE  u8_DispStrLen;
    D_WORD  pu16_DispStr[LA_PHONE_NUM_LEN+1];
}LA_SCAListEntry;


typedef struct
{
    D_BYTE u8_Reserve;
    D_BYTE u8_SCA_Status[LA14_SCA_MAX_LINES_PER_ACCOUNT];
}st_la14_LineSetSCAStatus;

typedef struct
{
    D_WORD  lineId                 :8;

    D_WORD  lineNameLen            :8;
    D_WORD  pLineName[LA_LINE_NAME_LEN];

    D_WORD  bmAttachedHandsets;

    D_WORD  pCLIRActCode[LA_ACTIVATION_CODE_MAX_LEN];              //CLIR activation code
    D_WORD  pCLIRDeactCode[LA_ACTIVATION_CODE_MAX_LEN];            //CLIR de-activation cod
    D_WORD  permanentCLIR          :1;
    D_WORD  CLIRActCodeLen         :3;
    D_WORD  CLIRDeactCodeLen       :3;

    D_WORD  intrusionCall          :1; 
    D_WORD  callMode               :1;     // single or multiple calls

    //Call forward unconditional
    D_WORD  callFwdUnconditional   :1;
    D_WORD  CFUActCodeLen          :3;
    D_WORD  CFUDeactCodeLen        :3;
    D_WORD  pCFUActCode[LA_ACTIVATION_CODE_MAX_LEN];               //CLIR activation code
    D_WORD  pCFUDeactCode[LA_ACTIVATION_CODE_MAX_LEN];             //CLIR de-activation code
    D_WORD  pCFUNumber[LA_PHONE_NUM_LEN];
    D_WORD  CFUNumberLen           :6;

    //Dial mode-proprietary field
    D_WORD  dialMode               :1;
    D_WORD  Spare1                 :1;
    D_WORD  CFNAActCodeLen         :3;
    D_WORD  CFNADeactCodeLen       :3;
    D_WORD  Spare2                 :2;

    D_WORD  CFNANumberLen          :6;

    //Call forward on no answer
    D_WORD  callFwdNoAnswer        :1;
    D_WORD  Spare3                 :1;
    D_WORD  secondsCFNA;
    D_WORD  pCFNAActCode[LA_ACTIVATION_CODE_MAX_LEN];              //CLIR activation code
    D_WORD  pCFNADeactCode[LA_ACTIVATION_CODE_MAX_LEN];            //CLIR de-activation code
    D_WORD  pCFNANumber[LA_PHONE_NUM_LEN];

    //Call forward on busy subscriber
    D_WORD  pCFBActCode[LA_ACTIVATION_CODE_MAX_LEN];               //CLIR activation code
    D_WORD  pCFBDeactCode[LA_ACTIVATION_CODE_MAX_LEN];             //CLIR de-activation code
    D_WORD  pCFBNumber[LA_PHONE_NUM_LEN];
    D_WORD  callFwdBusy            :1;
    D_WORD  CFBNumberLen           :6;
    D_WORD  Spare4                 :1;
    D_WORD  CFBActCodeLen          :3;
    D_WORD  CFBDeactCodeLen        :3;
    //ring mode-proprietary
    D_WORD  ringMode               :1;
    D_WORD  EnableTls              :1;
    //FP Volume
    D_WORD  FPVolume               :8;
    //FP Melody
    D_WORD  FPMelody               :8;
    //Unused 
    D_WORD  EnableBalance          :1;
    D_WORD  dialPrefixLen          :3;
    D_WORD  blkNumLen              :4;
    //Dialling Prefix
    D_WORD  pDialPrefix[LA_DIAL_PREFIX_MAX_LEN];
    //Block Number
    D_WORD  pBlkNum[LA_PHONE_NUM_LEN];
    //Flash Time-proprietary
    D_WORD  flashTime;

    /* LA14_LINE_SET_PROP_FIELD_LINE_STATUS */
    D_WORD pu8_LineStatus_TalkingNumber[LA_PHONE_NUM_LEN];    /*LA14_PHONE_NUM_LEN*/
    D_WORD u8_LineStatus_TalkingNumberLength:8;
    D_WORD u8_LineStatus_TalkingNumberAttr:8;

    D_WORD u8_LineStatus_Status :8;
    D_WORD LineStatus_Disable   :1;
    D_WORD LineStatus_Spare     :7;

    D_WORD u8_MusicOnHoldAttr   :8;
    D_WORD u8_CwAttr            :8;
    D_WORD u8_AnonCallAttr      :8;
    D_WORD u8_AnonRejAttr       :8;

    D_WORD u8_DndAttr           :8;
    D_WORD MUSIC_ON_HOLD        :1;     /* Music On Hold */
    D_WORD CALL_WAITING         :1;     /* call waiting enable/disable */
    D_WORD CALL_WAITING_TONE    :1;     /* call waiting tone enable/disable */
    D_WORD ANON_CALL            :1;     /* anonymous enable/disable */
    D_WORD ANON_REJ             :1;     /* anonymous rejection enable/disable */
    D_WORD DND                  :1;     /* do not distrub. DND enable/disable */
    D_WORD NET_CONFERECE        :1;     /* network conference enable/disable */    
    D_WORD NET_CONF_URI         :1;     /* URI valid = 1*/
    D_WORD u16_NumOfMissedCall;
    D_WORD u16_DialToneId;

    /* LA14_LINE_SET_PROP_FIELD_VOICE_MAIL */
    D_WORD pu8_VoiceMail_Number[LA_PHONE_NUM_LEN];
    D_WORD u8_VoiceMail_NumberLength    :8;
    D_WORD u8_VoiceMail_NumberAttr      :8;

    D_WORD u16_VoiceMail_Enable;    /* voice mail enable/disable */
    D_WORD u16_VoiceMail_Count;

    /* LA14_LINE_SET_PROP_FIELD_OUTGOING_ATTACH_HANDSETS */
    D_WORD u16_OutgoingAttachedHS;

    /* LA14_LINE_SET_PROP_FIELD_OUTGOING_DEFAULT_HANDSETS */
    D_WORD u16_OutgoingDefaultHS;
    D_WORD u8_OutgoingAttachHSAttr      :8;
    D_WORD u8_OutgoingDefaultHSAttr     :8;

    D_WORD pLineNumber[LA_PHONE_NUM_LEN];
    D_WORD lineNumberLen    :8;
    D_WORD u8_LineNumberAttr:8;
    st_la14_LineSetSCA          st_SCA;
    st_la14_LineSetSCAStatus    st_SCA_Status;
    st_la14_BalanceListEntry    st_Balance;
} LA_LineSettingsListEntry ;

typedef struct
{
    D_WORD  pName[LA_PHONE_NAME_LEN];
    D_WORD  pFirstName[LA_PHONE_NAME_LEN];
    D_WORD  nameLen                :8;
    D_WORD  fNameLen               :8;
    LA_ContactNumber  pst_ContactNumber[3];  /* 3. must equal to LA_NO_PHONE_PER_CONTACT */
    D_WORD  numLen                 :8;
    D_WORD  assocMelody            :8;
    D_WORD  lineId                 :8;
    D_WORD  contactType            :8; //bit mask for octet 3 of contact number field

    //Field Instances
    D_WORD contactNum_Inst         :2;
    D_WORD eLineSubType            :4;
    D_WORD bForceAddContact        :1;
    D_WORD spare                   :9;    

    D_WORD  DirListId              :8;
    D_WORD  DirLevel               :8;
    D_UINT  DirType;  				/* 32bit */  
    D_WORD  EntryId;
    D_WORD  DescriLen              :8;
    D_WORD  Reserve                :8;
    D_WORD  pDescriStr[LA_PHONE_NAME_LEN];

} LA_ContactListEntry;

typedef struct
{
    D_WORD pNumber[LA_PHONE_NUM_LEN];
    D_WORD pName[LA_PHONE_NAME_LEN];
    D_WORD numLen          :8;
    D_WORD nameLen         :8;
    D_WORD pLineName[LA_LINE_NAME_LEN];
    D_WORD lineId          :8;
    D_WORD lineNameLen     :8;
    //Date and time
    LA_DateTime dateTime;
} LA_OutgoingCallsListEntry;

typedef struct
{
    D_WORD pNumber[LA_PHONE_NUM_LEN];
    D_WORD pName[LA_PHONE_NAME_LEN];
    D_WORD numLen          :8;
    D_WORD nameLen         :8;
    D_WORD pLineName[LA_LINE_NAME_LEN];
    D_WORD lineId          :8;
    D_WORD lineNameLen     :8;
    //Date and time
    LA_DateTime dateTime;
} LA_AcceptedCallsListEntry;

typedef struct
{
    D_WORD pNumber[LA_PHONE_NUM_LEN];
    D_WORD pName[LA_PHONE_NAME_LEN];
    D_WORD pLineName[LA_LINE_NAME_LEN];
    D_WORD callType        :8;
    D_WORD numLen          :8;
    D_WORD nameLen         :8;
    D_WORD lineId          :8;
    D_WORD lineNameLen     :8;
    D_WORD numCalls        :8;
    D_WORD readStatus      :1;
    D_WORD bBlackCaller    :1;
    D_WORD spare           :14;
    //Date and Time
    LA_DateTime dateTime;
    LA_Duration duration;
} LA_AllCallListEntry;

/* YEALINK_LA_PROP_LIST */
typedef struct
{
    D_WORD  pNumber[LA_PHONE_NUM_LEN];
    D_WORD  numLen          :8;
    D_WORD  numAttr         :8;    
} LA_BlackCallerListEntry;


typedef struct
{
    D_BYTE ToneIndex;
    D_BYTE ToneDataLen;
    D_BYTE ToneData[LA_COUNTRY_TONE_MAX_LEN];
}LA_CountryToneListEntry;

typedef struct
{
    D_WORD  pName[LA_PHONE_NAME_LEN];
    D_WORD  nameLen          :8;
    D_WORD  nameAttr         :8;

    D_WORD  pNumber[LA_PHONE_NUM_LEN];
    D_WORD  numLen          :8;
    D_WORD  numAttr         :8;
}LA_PreDialListEntry;

typedef struct
{
    D_WORD  lineMask        :8;
    D_WORD  reserve         :8;
    D_WORD  ruleLen         :8;
    D_WORD  ruleAttr        :8;
    D_WORD  pRule[LA_DIAL_NOW_MAX_LEN];
}LA_DialNowListEntry;


typedef union
{
    LA_SupportedListEntry        SLEntry;
    LA_MissedCallsListEntry      MCLEntry;
    LA_InternalNamesListEntry    INLEntry;
    LA_DectSettingsListEntry     DSSLEntry;
    LA_LineSettingsListEntry     LSLEntry;
    LA_ContactListEntry          CLEntry;
    LA_OutgoingCallsListEntry    OCLEntry;
    LA_AcceptedCallsListEntry    IACLEntry;
    LA_AllCallListEntry          ACLEntry;
    LA_BlackCallerListEntry      BLCEntry;
    LA_CountryToneListEntry      CTLEntry;
    LA_PreDialListEntry          PDLEntry;
    LA_DialNowListEntry          DNLEntry;
    LA_SCAListEntry              SCAEntry;
} LA_ListEntry; 

typedef struct
{
    D_WORD matchingOption  :4;
    D_WORD stringLength    :5;
    D_WORD pSearchString[LA_SEARCH_STR_LEN];
    D_WORD counter;
}LA_Search;

typedef struct
{
    D_BYTE Length;                         // Length
    D_BYTE pFieldId[LA_MAX_FIELDS_COUNT*2];  // Field Id buffer - fieldId 1....fieldId N
} LA_FieldIds;

typedef struct
{
    D_WORD requestedFields;
    D_WORD requestedPropFields;
    D_WORD requestedProp09Fields;
    D_WORD defaultFieldMask;       //bit field mask used to set default values for fields
    D_WORD defaultPropFieldMask;   //bit field mask used to set default values for proprietary fields
    D_WORD defaultProp09FieldMask;   
    D_WORD u16_reserved[6];
    LA_ListEntry  listEntry;
    LA_Search     searchData;
} LA_ComData;

/************************************************************************/
/*   notification                                                       */
/************************************************************************/
typedef struct
{
    D_BYTE u8_Year;
    D_BYTE u8_Month;
    D_BYTE u8_Day;
    D_BYTE u8_Hours;
    D_BYTE u8_Mins;
    D_BYTE u8_Secs;
    D_BYTE u8_Zone : 7;
    D_BYTE u8_ZoneDirection: 1;
}DSR_DateTimeInfo;

typedef struct
{
    D_BYTE u8_LineId;
    D_INT  enSubType;
}DSR_LineId;

typedef struct
{
    D_WORD     u16_ListEntries;
    D_WORD     u16_NumOfCalls;
    D_INT      enListId;
    DSR_LineId stLineInfo;
    D_INT      enMCSubType;
}DSR_Notify_MC;

typedef struct
{
    D_WORD     u16_NumOfMsgs;
    DSR_LineId stLineInfo;
    D_INT      enMWSubType;
} DSR_Notify_VMWIN;

typedef struct
{
    D_INT enListId;
    D_WORD u16_ListEntries;
    DSR_LineId stLineInfo;
}DSR_Notify_ListChange;

typedef struct
{
    D_INT enCoding;
    D_INT enInterpretation;
    DSR_DateTimeInfo stDateTime;
}DSR_Notify_DateTime;

typedef struct
{
    D_BYTE u8_MusicOnHold;
    DSR_LineId stLineInfo;
}DSR_Notify_MusicOnHold;

typedef struct
{
    D_BYTE u8_NetStatus;
    DSR_LineId stLineInfo;
}DSR_Notify_NetStatus;

//{{BaseStatus
typedef struct
{
    D_BYTE u8_RpsStatus;
    DSR_LineId stLineInfo;
}DSR_Notify_RpsStatus;

typedef struct
{
    D_BYTE u8_RpsResult;
}DSR_Notify_RpsResult;


/* for phone global cfg,only change by web/base */
typedef struct  
{
    D_UINT ul_Mask;

    D_UINT bBlindOnHook       :1;
    D_UINT bSemiAttendOnHook  :1;
    D_UINT bNetworkCallLog    :1;
    D_UINT bSuppressDTMFDisplay       :1;
    D_UINT bSuppressDTMFDisplayDelay  :1;
    D_UINT bDisableCallEndTone        :1;
    D_UINT bAutoIntercomToHeadset     :1;
    D_UINT bDispResetLocaConfig       :1;
    D_UINT bDisableVoiceMailTone      :1;
    D_UINT bEnableCFChannel           :1;
    D_UINT bCallEndTone5s             :1;
    D_UINT bDisableAnonymousCall      :1;
    D_UINT bEnableCallInfoEvent       :1;
    D_UINT bEnableRingCentralVoiceMail:1;
    D_UINT bDispLogonWizard           :1;
    D_UINT bEnableGammaPincode        :1;
    D_UINT bDisableAnonCallReject     :1;
    D_UINT bDisableCallWaiting        :1;
    D_UINT bEnableRepeaterPinCode     :1;
    D_UINT bEnablePoundAsSend         :1;
    D_UINT bDisableBlackCallList      :1;
    D_UINT bDisableSetVCM             :1;
    D_UINT bDisablePrivateHold        :1;
    D_UINT bEnableStarAsSend          :1;
    D_UINT bDisableScaBargeIn         :1;
    D_UINT bEnableHeldConference      :1;
    D_UINT bDisableDefaultLineSet     :1;
    D_UINT bDisableIncomingLineSet    :1;
    
    D_UINT Spare              :4;
    
    D_BYTE  EmergencyNumbLen ;
    D_BYTE  ucEmergencyNumb[32];/* 32 byte */
}DSR_Notify_PhoneCfg;

typedef struct  
{
    D_BYTE  bResetAlways                ;/*预留,默认为0*/
    D_BYTE  bDisableResetOnChange       ;/*预留,默认为0*/
    D_BYTE  reserve1                    ;/*预留,默认为0*/
    D_BYTE  reserve2                    ;/*预留,默认为0*/
    D_UINT  ul_Mask                     ;
    D_WORD  InputMethod                 ;
    D_WORD  Language                    ;

    D_BYTE  TimeFormat                  ;
    D_BYTE  DateFormat                  ;
    D_BYTE  ColorScheme                 ;
    D_BYTE  AutoIntercom                ;

    D_UINT  bScreenSaver                :1;
    D_UINT  bDispBackLightInCharger     :1;
    D_UINT  bDispBackLightOutofCharger  :1;
    D_UINT  bKeypadLight                :1;

    D_UINT  bKeypadTone                 :1;
    D_UINT  bConfirmationTone           :1;
    D_UINT  bLowBatteryTone             :1;
    D_UINT  bVoiceMailNotifyLight       :1;
    D_UINT  bMissCallNotifyLight        :1;

    D_UINT  bAutoAnswer                 :1;
    D_UINT  bEcoMode                    :1;
    D_UINT  Spare                       :21;
}DSR_Notify_HsDefaultCfg;


typedef struct
{
    D_BYTE u8_LineId;
    D_BYTE u8_NumOfMsgs;
}DSR_Notify_VM;

typedef struct
{
    D_BYTE u8_MultiCnt;
    DSR_Notify_VM pMultiVM[LA_MAX_MULTI_VM_CNT];
}DSR_Notify_MultiVM;

typedef struct
{
    D_BYTE u8_LineId;
    D_BYTE u8_NumOfMissedCall;
}DSR_Notify_SMC;

typedef struct
{
    D_BYTE u8_MultiCnt;
    DSR_Notify_SMC pMultiMC[LA_MAX_MULTI_MC_CNT];
}DSR_Notify_MultiMC;

typedef struct
{
    D_BYTE  enListId;
    D_BYTE  u8_ListEntries;
    D_BYTE  u8_IdentifyCode[LA_MAX_LIST_IDENTIFY_CODE_LEN];
}DSR_Notify_LC;

typedef struct
{
    D_BYTE u8_MultiCnt;
    DSR_Notify_LC pMultiLC[LA_MAX_MULTI_LC_CNT];
}DSR_Notify_MultiLC;

typedef struct
{
    D_BYTE u8_LineId;
    D_BYTE u8_DailToneId;
}DSR_Notify_DailToneId;

typedef struct
{
    D_BYTE u8_MultiCnt;
    DSR_Notify_DailToneId pMultiDailToneId[LA_MAX_MULTI_DTI_CNT];
}DSR_Notify_MultiDailToneId;

typedef struct
{
    D_BYTE u8_LineId;
    D_BYTE u8_SCAStatus[LA14_SCA_MAX_LINES_PER_ACCOUNT];
}DSR_SCA_LINE_STATUS;

typedef struct
{
    D_BYTE u8_MultiCnt;
    DSR_SCA_LINE_STATUS pSCALineStatus[LA_MAX_LINE_CNT];
}DSR_Notify_MultiSCA;

typedef struct
{
    unsigned char Version[5];
}DSR_VER_HEX;

typedef struct
{
    DSR_VER_HEX  NewVersion;
}DSR_Upgrade_HsNewVer;

typedef struct
{
    D_BYTE   u8_Battery;
    D_BYTE   u8_Signal;
    D_WORD   u16_Status; /* en_HandsetStatusMask.*/
    D_WORD   u16_Capability; /*en_HandsetCapabilityMask.*/
}DSR_Upgrade_HsState;

typedef struct
{
    DSR_VER_HEX ROM_version;
    DSR_VER_HEX HW_version;
    char    OEM_name[SU_MAX_NAME_LEN];
}DSR_Upgrade_HsAllVer;

typedef struct
{
    DSR_Upgrade_HsAllVer st_HsAllVersion;
    DSR_Upgrade_HsState st_HsState;
}DSR_Upgrade_HsInf;


typedef struct
{
    D_BYTE u8_UpgradeCmd;
    D_BYTE u8_hs_nr;
    D_BYTE u8_hs_mask;
    D_BYTE u8_Reserved[1];
    union
    {
        DSR_Upgrade_HsNewVer        st_NewVer;
        DSR_Upgrade_HsInf           st_HsInf;
    }u;
}DSR_Notify_Upgrade;



typedef struct
{
    D_BYTE u8_Type; /*auto p, ...*/
    D_BYTE u8_OnOff;
    D_BYTE u8_DisableOperation;
    D_BYTE u8_TimeOutSec;
    D_BYTE u8_Priority;
    D_BYTE u8_Name[NOTIFY_BS_MAX_NAME_LEN];
}DSR_Notify_BaseStatus;




typedef struct  
{
    D_WORD  u16_bInIdleMode:1            ;
    D_WORD  u16_ChargeState:3            ;
    D_WORD  u16_SignalLevel:3            ;
    D_WORD  u16_SignalMaxLvl:3           ;
    D_WORD  u16_SignalMinLvl:3           ;
    D_WORD  u16_Reserved:3               ;
    D_WORD  u16_bootCnt                  ;
    D_WORD  u16_RangeOutIndCnt           ;
    D_WORD  u16_CellChangeCnt            ;
    D_WORD  u16_MacConnectFailCnt        ;
    D_WORD  u16_QualityIndCnt            ;
    D_WORD  u16_CFRecvErrCnt             ;

    /*system*/
    D_WORD u16_IdleTime;
    D_WORD u16_RunTime;

    /*driver*/
    D_WORD u16_ChargeDectectCnt;
    D_WORD u16_BatteryCurVol;  

    /*vdsw 2*/
    D_WORD u16_NormalCallInitErrCnt;
    D_WORD u16_Frame0LostIndCnt;
    D_WORD u16_LaCallInitErrCnt;
    D_WORD u16_LinkDisWhileRunCnt;
    D_WORD u16_LaNoRespErrCnt;    

    /*app*/
    D_WORD u16_AppIncomingCallIndCnt;
    D_WORD u16_AppOutgoingCallCnt;
    
}DSR_Notify_HsInfo_Stat;


typedef struct  
{
    D_WORD  u16_type                     ;
    union
    {
        DSR_Notify_HsInfo_Stat st_HsInfoStat;
    }u;
}DSR_Notify_HsInfo;


typedef struct  
{
    D_BYTE  u8_LoginType    ;
    D_BYTE  u8_Reason       ;
    D_BYTE  u8_PopErrWin    ;
    D_BYTE  u8_Reserve[5]   ;
}DSR_Notify_Login;


typedef struct  
{
    D_BYTE u8_Mode;/*0：XSI,1：FAC, XSI暂不实现*/
    D_BYTE bEnableCallPark          :1;
    D_BYTE bEnableGroupCallPark     :1;
    D_BYTE bEnableVisualAlert       :1;
    D_BYTE bEnalbeAudioAlert        :1;
    D_BYTE bReserve                 :4;
    D_BYTE u8_Reserve[2];
    D_BYTE pCallParkCode[LA_CP_CODE_LEN];
    D_BYTE pGroupCallParkCode[LA_CP_CODE_LEN];
    D_BYTE pParkRetrieveCode[LA_CP_CODE_LEN];
}DSR_Notify_CallParkCfg;


typedef struct  
{
    D_BYTE u8_LineId;
    D_BYTE u8_State;
    D_BYTE u8_NeedReadEntry;/*是否需要启动LA去读取数据, 先不实现,预留接口*/
    D_BYTE u8_Reserve;
    D_BYTE u8_Number[LA_CP_NUMBER_LEN];
}DSR_Notify_CallPark;


/* for phone global cfg,only change by web/base */
typedef struct  
{
    D_UINT ul_Mask2;
    D_UINT ul_Mask3;

    D_UINT bEnableDialNow              :1;
    D_UINT u8_DialNowDelay             :8;
    D_UINT bRingLightEnable            :1;
    D_UINT bCommonPowerLightEnable     :1;
    D_UINT bEnablePreDialSearch        :1;
    D_UINT bMissedCallLightEnable      :1;
    D_UINT bVoiceMailLightEnable       :1;
    D_UINT bPlugChgCallEndEnable       :1;
	D_UINT u8_SearchContactDefaultIME  :4;
    D_UINT bRingUiToMessDispEnable     :1;
	D_UINT u8_CallDisplayMethod		   :3;
    D_UINT Spare   :9;

    D_UINT u32_Reserve1;
    D_UINT u32_Reserve2;
}DSR_Notify_PhoneCfg2;

//}}

/************************************************************************/
/*                                                                      */
/************************************************************************/
typedef struct
{
    D_BYTE  u8_SIP_XSI_LineId;
    D_BYTE  u8_Commportal_PopTipType;
    D_BYTE  u8_Reserve[6];
} LA_MoreDataToHs;

typedef struct
{
    D_BYTE  u8_CancelLoginType;
    D_BYTE  u8_Reserve[7];
} LA_MoreDataFromHs;

typedef struct
{
    D_UINT  u32_Magic;
    D_UINT  u32_Mask;
    LA_MoreDataToHs st_MoreDataToHs;
} LA_Res_MoreData;

typedef struct
{
    D_UINT  u32_Magic;
    D_UINT  u32_Mask;
    LA_MoreDataFromHs st_MoreDataFromHs;
} LA_Req_MoreData;


/************************************************************************/
/*                                                                      */
/************************************************************************/
typedef struct
{
    D_INT u32_storagetype;
    D_INT u32_paramTag;
    //if u32_paramTag is "E_PARAM_TAG_FLEXIBLE", then "u32_address" & "u16_numOfBytesToRead" used
    D_UINT u32_address;
    D_WORD u16_numOfBytesToRead;
    D_WORD u16_reserved[3];
} DSR_Get_ParamInfo;

typedef struct
{
    D_INT u32_storagetype;
    D_INT u32_paramTag;
    //if u32_paramTag is "E_PARAM_TAG_FLEXIBLE", then "u32_address" & "u16_numOfBytesToWrite" used
    D_UINT u32_address;
    D_WORD u16_numOfBytesToWrite;
    D_WORD u16_reserved[3];
    //~
    D_BYTE u8_Buffer[64];
} DSR_Set_ParamInfo;

typedef struct
{
    /* length of data buffer */
    D_BYTE u8_flag; // for CLMS flag=1, for IWU flag=2
    D_WORD u16_HSBitMap;
	D_WORD u16_reserved[4];
    D_WORD u16_len;
    D_BYTE  pdata[64];
} DSR_Set_CLMS_IWU;

typedef struct
{
    D_INT cmd;
    D_INT apiResult;
    D_INT reason;
	D_WORD u16_reserved[4];
    D_WORD u16_length;
    D_BYTE pdata[256];
} DSR_API_Result;

typedef struct
{
    /* length of data buffer */
    D_WORD u16_reserved[3];
	D_WORD u16_len;
    D_BYTE pdata[DSR_MAX_GENERIC_BUF_SIZE];
} DSR_Generic;

typedef struct
{
    D_WORD u16_SessionId;
    D_WORD u16_ListId;
    D_WORD u16_HsNr;
    D_WORD u16_reserved[3];
    LA_FieldIds LAFieldIdsSortRequest;
} DSR_LA_Start;

typedef struct
{
    D_WORD u16_SessionId;
    D_WORD u16_HsNr;
    D_WORD u16_reserved[4];
    LA_Req_MoreData st_MoreData;
} DSR_LA_End;

typedef struct
{
    D_WORD u16_SessionId;
    D_WORD u16_ListId;
    D_WORD u16_HsNr;
    D_WORD u16_reserved[4];
}DSR_LA_Get_EntryFields;

typedef struct
{
    D_WORD u16_Listid;
    D_WORD u16_SessionId;
    D_WORD u16_HsNr;
    D_WORD u16_reserved[3];
    LA_FieldIds   LAFields; // Added for List of Fields to be included for a search
    LA_ComData  st_LA_comData;
} DSR_LA_Search;

typedef struct
{
    D_WORD u16_Listid;
    D_WORD u16_SessionId;
    D_WORD u16_HsNr;
    D_WORD UpdateType;
    D_WORD UpdateAttr;
    D_WORD UpdateResult;
    D_WORD Reserve1;
    D_WORD Reserve2;
    D_WORD u16_reserved[4];
} DSR_LA_Update;

typedef struct
{
    D_WORD u16_SessionId;
    D_WORD u16_StartIndex;
    D_WORD u16_Counter;
    LA_FieldIds LAFields;
    D_WORD u16_mark;
    D_WORD u16_No_Ent;
    D_WORD u16_Dir;
    D_WORD u16_reserved[3];
} DSR_LA_Read; 

typedef struct
{
    D_WORD U16_SessionId;
    D_WORD u16_HsNr;
    D_WORD u16_EntryId;
    D_WORD u16_reserved[3];
} DSR_LA_Save;

typedef struct
{
    D_WORD U16_SessionId;
    D_WORD u16_HsNr;
    D_WORD u16_EntryId;
    D_WORD u16_reserved[3];
    LA_FieldIds LAFields;
} DSR_LA_Edit;

typedef struct
{
    D_WORD U16_SessionId;
    D_WORD u16_HsNr;
    D_WORD u16_EntryId;
    D_WORD u16_reserved[3];
} DSR_LA_Delete;

typedef struct
{
    D_WORD U16_SessionId;
    D_WORD u16_HsNr;
    D_WORD u16_reserved[4];
} DSR_LA_Empty;

typedef struct
{
    D_WORD U16_ListId;
    D_WORD u16_HsNr;
    LA_FieldIds LAFields;
    D_BYTE u8_ListUpdatestatus;
    D_BYTE numLen;
    D_BYTE nameLen;
    D_BYTE smsLen;
    D_WORD u16_reserved[4];
    D_WORD pNumber[LA_PHONE_NUM_LEN];
    D_BYTE  pName[LA_PHONE_NAME_LEN];
    D_UINT u32_smsEntryId; 
    D_BYTE psmsText[64];
} DSR_LA_ChangeReq;

typedef struct
{
    D_WORD u16_SessionId;
    D_WORD u16_ListId;
    D_WORD u16_HsNr;
    D_WORD u16_LastPacket;
    D_WORD u16_EntryId;
    D_WORD u16_DataPacketEmpty;
    LA_FieldIds st_LaFieldsSent;
    D_WORD requestedFields;
    D_WORD requestedPropFields;
    D_WORD requestedProp09Fields;
    D_WORD u16_reserved[7];
    LA_ListEntry st_Entry;
}DSR_LA_Entry;

typedef struct
{
    D_INT La_cmd;
    D_WORD u16_SessionId;
    D_WORD u16_NumofEntries;
    D_INT s32_response;
    D_WORD u16_EntryStartIndex;
    D_WORD u16_EntryCount;
    D_WORD u16_EndtryId;
    D_WORD u16_EntryIndex;
    D_WORD u16_ListId;
    D_WORD u16_HsNr;
    D_WORD u16_Status;
    D_INT e_Event_type;
    D_WORD u16_retryCnt;
    D_WORD u16_reserved[4];
    LA_FieldIds st_LA_FieldIds;
    LA_FieldIds st_La_EditFieldIds;
    LA_FieldIds st_La_NonEditFieldsIds;
    LA_FieldIds st_La_SortFieldIds;
    union
    {
        DSR_Notify_MC          st_LA_GE_MCNotification;
        DSR_Notify_VMWIN       st_LA_GE_VMWINotification;
        DSR_Notify_ListChange  st_LA_GE_ListChangeNotification;
        DSR_Notify_DateTime    st_LA_GE_DateTime;
        DSR_Notify_MusicOnHold st_GE_MusicOnHold;
        DSR_Notify_NetStatus   st_GE_NetStatus;
        DSR_Notify_RpsStatus   st_GE_RpsStatus;//BaseStatus
        DSR_Notify_RpsResult   st_GE_RpsResult;
        DSR_Notify_PhoneCfg    st_GE_PhoneCfg;
        DSR_Notify_MultiVM     st_GE_MultiVM;
        DSR_Notify_MultiMC     st_GE_MultiMC;
        DSR_Notify_MultiLC     st_GE_MultiLC;
        DSR_Notify_MultiDailToneId     st_GE_MultiDailToneId;
        DSR_Notify_Upgrade     st_GE_HsUpgrade;
        DSR_Notify_BaseStatus  st_GE_BaseStatus;
        DSR_Notify_HsDefaultCfg st_GE_HsDefaultCfg;
        DSR_Notify_MultiSCA     st_GE_MultiSCA;
        DSR_Notify_Login        st_GE_Login;
        DSR_Notify_CallParkCfg  st_GE_CallParkCfg;
        DSR_Notify_CallPark     st_GE_CallPark;
        DSR_Notify_PhoneCfg2   st_GE_PhoneCfg2;

        /**/
        LA_Res_MoreData     st_MoreData;
    }u;
} DSR_LA_Response;

typedef struct
{
    D_INT    en_result;
    D_WORD   u16_Handset;
    D_INT    en_FailReason;
    D_INT    en_Type;
    D_WORD   u16_mask;
    D_WORD   u16_reserved[4];
    D_INT    hs_results[DSR_MAX_HS_COUNT];
} DSR_LA_GenEventResult;

typedef struct 
{
    D_BYTE lineNum;
    D_WORD bitmask;
    D_WORD u16_reserved[4];
} DSR_HS_LineSettings;

typedef struct 
{
    D_WORD  u16_Event;
    D_WORD  u16_HsNr;
    D_BYTE  u8_len;
    D_BYTE  u8_reserved[7];
    union
    {
        DSR_Notify_DateTime    st_DateTime;
        D_BYTE              u8_music_on_hold;
        D_BYTE              u8_reset_mode;
        D_BYTE              u8_hs_alive;
        DSR_Notify_Upgrade  st_GE_HsUpgrade;
        DSR_Notify_HsInfo   st_HsInfo;
    }u;
}DSR_HS_GenEvent;


#define  DSR_IPEI_SIZE 5    /*!< \brief Length of IPEI */
typedef struct 
{
    D_WORD              u16_HsNo;                   /*!< Handset number */
    D_INT               HSState;                    /*!< State: 0 = unregistered, 1 = registered, 2 = blacklisted, 3 = subscribing */
    D_INT               e_Type;                     /*!< Handset type */
    D_UINT              ModelNo;                    /*!< Model number assigned by product owner or manufacturer */
    D_BYTE              u8_HsId[DSR_IPEI_SIZE];     /*!< Handset identity */
    D_UINT              capability;                 /*&0x1L, GAP,CATIQ_2, CATIQ_3*/
    D_BYTE              locate_type;                /*first,quick,normal*/
    D_BYTE              reserv[7];

}DSR_HS_Info;

/*
* dsr body
*/
typedef union
{
    DSR_Get_ParamInfo               GetParam;
    DSR_Set_ParamInfo               SetParam;
    DSR_Set_CLMS_IWU                ClmsIwu;
    DSR_API_Result                  Response;
    DSR_Generic                     Generic;

    DSR_LA_Start                    StartSession;
    DSR_LA_End                      EndSession;
    DSR_LA_Get_EntryFields          QuerySupEntryFileds;
    DSR_LA_Search                   SearchEntries;
    DSR_LA_Update                   Update;
    DSR_LA_Read                     ReadEntries;
    DSR_LA_Save                     SaveEntry;
    DSR_LA_Edit                     EditEntry;
    DSR_LA_Delete                   DeleteEntry;
    DSR_LA_Empty                    DeleteList;
    DSR_LA_ChangeReq                ChangeListRequest;
    DSR_LA_Entry                    Entry;
    DSR_LA_Response                 LaResponse;
    DSR_LA_GenEventResult           GenericEventResult;

    DSR_HS_LineSettings             LineSetting;
    DSR_HS_Info                     HSInfo;
    DSR_HS_GenEvent                 HSGenericEvent;
} dsr_body;

/*
* dsr msg
*/
typedef struct
{
    msg_hdr  hdr;
    dsr_body body;
} dsr_msg;

/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /* __DECT_DSR_H__ */
