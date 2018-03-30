/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2012-12-10
*
*******************************************************************/

#ifndef __DECT_LA_H__
#define __DECT_LA_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/
/* Nack Reasons */
#define  LA_NACK_INVALID_RANGE              0
#define  LA_NACK_ENTRY_NOT_AVAILABLE        1
#define  LA_NACK_INVALID_SESSION            2
#define  LA_NACK_TEMP_NOT_POSSIBLE          3
#define  LA_NACK_INCORRECT_FORMAT           4
#define  LA_NACK_INVALID_START_INDEX        5
#define  LA_NACK_PROC_NOT_SUPTD             6
#define  LA_NACK_PROC_NOT_ALLOWED           7
#define  LA_NACK_CONTENT_NOT_ACCEPTED       8
#define  LA_NACK_LIST_FULL                  9
#define  LA_NACK_INCORRECT_PIN              10
#define  LA_NACK_PIN_CODE_REQ               11
#define  LA_NACK_USER_NAME_PASSWD_REQ       12
#define  LA_NACK_USER_NAME_REQ              13
#define  LA_NACK_PASSWD_REQ                 14
#define  LA_NACK_INCORRECT_USER_NAME_PASSWD 15
#define  LA_NACK_INCORRECT_USER_NAME        16
#define  LA_NACK_INCORRECT_PASSWD           17
#define  LA_NACK_SAME_NAME_NUMBER           18
#define  LA_NACK_INVALID                    0xFF

/* call type */
#define  LA_CALLTYPE_MISSED_CALL            0x20
#define  LA_CALLTYPE_ACCEPTED_CALL          0x10
#define  LA_CALLTYPE_OUTGOING_CALL          0x08

/* Session Reject Reasons */
#define  LA_REJECT_NO_ERR                   -1
#define  LA_REJECT_NO_RESOURCES             0
#define  LA_REJECT_LIST_IN_USE              1
#define  LA_REJECT_LIST_NOT_SUPTD           2
#define  LA_REJECT_MAX_SESSIONS             3
#define  LA_REJECT_PIN_CODE_REQ             4
#define  LA_REJECT_BUSY                     5
#define  LA_REJECT_CONNECT_ERROR            6
#define  LA_REJECT_DOWNLOADING              7
#define  LA_REJECT_FWUP_NOT_END             8
#define  LA_REJECT_USER_NAME_PASSWD_REQ     9
#define  LA_REJECT_USER_NAME_REQ            10
#define  LA_REJECT_PASSWD_REQ               11
#define  LA_REJECT_XSI_INFO_INCOMPLETE      12
#define  LA_REJECT_SIP_XSI_AUTH_FAILED      13
#define  LA_REJECT_URL_EMPTY                14
#define  LA_REJECT_DIR_LOADING_ERR          15
#define  LA_REJECT_MTSW_URL_EMPTY           16
#define  LA_REJECT_MTSW_AUTH_ERR            17
#define  LA_REJECT_AUTH_ING_ERR             18


#define  LA_INVALID_LINEID                  0xFF
#define  LA_MAX_MULTI_VM_CNT                6
#define  LA_MAX_MULTI_MC_CNT                6
#define  LA_MAX_MULTI_LC_CNT                5
#define  LA_MAX_LIST_IDENTIFY_CODE_LEN      4 /* U32*/
#define  LA_MAX_MULTI_DTI_CNT               6
#define  LA_MAX_BASE_STATUS_LEN             40 

#define  LA_MAX_LINE_CNT                    5
#define  LA_SET_LA_TIMER_START_SESSION_ID   0x00
#define  LA_MORE_DATA_START_SESSION_ID      0x00

#define  LA_MAX_MAX_CONT_DIRS          	    28 /* cache phonebook dir max count */

/* LA Setting List Entry */
#define  LA_IP_TYPE_DHCP                        0x01
#define  LA_DNS_TYPE_DHCP                       0x02
#define  LA_IP_TYPE_PPPOE                       0x04

#define  LA_IP_PROP_IPV4                        0
#define  LA_IP_PROP_IPV6                        1



#define  LA_LINES_ST_DISCONNEC              0
#define  LA_LINES_ST_IDLE                   1
#define  LA_LINES_ST_DATA                   2
#define  LA_LINES_ST_BUSY                   3
#define  LA_LINES_ST_ALERT                  4
#define  LA_LINES_ST_CONFERENCE             5


/* LA Line Id SubType */
#define  LA_LINEID_SUBTYPE_EXT_CALL             0x0
#define  LA_LINEID_SUBTYPE_SRC_INT_CALL         0x1
#define  LA_LINEID_SUBTYPE_TRG_INT_CALL         0x2
#define  LA_LINEID_SUBTYPE_RELATING_TO          0x3
#define  LA_LINEID_SUBTYPE_ALL_LINES            0x4
#define  LA_LINEID_SUBTYPE_TYPE_INFO            0x5
#define  LA_LINEID_SUBTYPE_INVALID_SUBTYPE      0x6

/* LA advance search option / status */
#define  LA_SEARCH_PREV_PAGE             0x01
#define  LA_SEARCH_NEXT_PAGE             0x02


/* LA General Event Result */
#define  LA_EVENT_RES_SUCCESSFUL              0
#define  LA_EVENT_RES_FAIL                    1
#define  LA_EVENT_RES_MSG_LEN_EXCEEDED        2
#define  LA_EVENT_RES_IE_REPEATED             3
#define  LA_EVENT_RES_IE_FOUND                4
#define  LA_EVENT_RES_IE_NOT_FOUND            5
#define  LA_EVENT_RES_IE_REPLACED             6
#define  LA_EVENT_RES_IE_FITS                 7
#define  LA_EVENT_RES_IE_DOESNT_FIT           8
#define  LA_EVENT_RES_FACILITY_BUSY           9     //Seeding messages too quickly,handset FACILITY is busy
#define  LA_EVENT_RES_OTHER_LINE_IN_PROGRESS  10
#define  LA_EVENT_RES_INVALID_LINEID          11
#define  LA_EVENT_RES_VALID_LINEID            12
#define  LA_EVENT_RES_INVALID_HSMASK          13    //Invalid handset mask or handset not in line
#define  LA_EVENT_RES_FACILITY_Q_EMPTY        14    //handset FACILITY is idle
#define  LA_EVENT_RES_NON_CATIQ_HS            15
#define  LA_EVENT_RES_HS_NOT_ALIVE            16
#define  LA_EVENT_RES_FACILITY_FAIL           17

/* LA Event Type */
#define  LA_EVENT_TYPE_MESSAGE_WAITING        0
#define  LA_EVENT_TYPE_MISSED_CALL            1
#define  LA_EVENT_TYPE_WEB_CONTENT            2
#define  LA_EVENT_TYPE_LIST_CHANGE            3
#define  LA_EVENT_TYPE_SW_UPGRADE             4
#define  LA_EVENT_TYPE_DATETIME               5
#define  LA_EVENT_TYPE_CLIP_NAME              6 /* iwu : base -> hs*/
#define  LA_EVENT_TYPE_MUSICONHOLD            7 /* event: base -> hs. iwu: hs -> base */
#define  LA_EVENT_TYPE_INCALL_NUMBER          8 /* iwu : base -> hs*/
#define  LA_EVENT_TYPE_NET_STATUS             9 
#define  LA_EVENT_TYPE_REBOOT                 10 /* iwu : hs -> base*/
#define  LA_EVENT_TYPE_RESET                  11 /* iwu : hs -> base*/
#define  LA_EVENT_TYPE_RPS_STATUS             12 
#define  LA_EVENT_TYPE_RPS_RESULT             13 /* iwu */
#define  LA_EVENT_TYPE_PHONE_CFG              14 /* global data cfg for phone */
#define  LA_EVENT_TYPE_HS_KEEP_ALIVE          15 /* event : hs -> base */
#define  LA_EVENT_TYPE_MULTI_VOICE_MAIL       16 
#define  LA_EVENT_TYPE_MULTI_MISSED_CALL      17
#define  LA_EVENT_TYPE_MULTI_LINE_STATUS      18
#define  LA_EVENT_TYPE_MULTI_LIST_CHANGE      19
#define  LA_EVENT_TYPE_MULTI_DAIL_TONE_ID     20 
#define  LA_EVENT_TYPE_HS_UPGRADE             21

#define  LA_EVENT_TYPE_BASE_STATUS            22 
#define  LA_EVENT_TYPE_HS_DEFAULT_CFG         23
#define  LA_EVENT_TYPE_MULTI_SCA              24
#define  LA_EVENT_TYPE_HS_INFO                25
#define  LA_EVENT_TYPE_LOGIN                  26 
#define  LA_EVENT_TYPE_CALL_PARK_CFG          32
#define  LA_EVENT_TYPE_CALL_PARK              33
#define  LA_EVENT_TYPE_PHONE_CFG2             34
#define  LA_EVENT_TYPE_INVALID                0xFF 



/************************************************************************/
/*                                                                      */
/************************************************************************/
/* List IDs as defined in CAT-iq-2.0 Standard */
#define  LA_ID_LIST_OF_SUPPORTED            0x00
#define  LA_ID_MISSED_CALL                  0x01
#define  LA_ID_OUTGOING_CALL                0x02
#define  LA_ID_INCOMING_ACPTD_CALL          0x03
#define  LA_ID_ALL_CALL                     0x04
#define  LA_ID_CONTACT_LIST                 0x05
#define  LA_ID_INTERNAL_NAMES               0x06
#define  LA_ID_SYSTEM_SETTING               0x07
#define  LA_ID_LINE_SETTING                 0x08
#define  LA_ID_BLACK_CALLER                 0x0a

#define  LA_ID_COUNTRY_TONE_LIST            0x85

#define  LA_ID_CONTACT_MIN                  0xA0
#define  LA_ID_CONTACT_ROOT_DIR             LA_ID_CONTACT_MIN
#define  LA_ID_CONTACT_MAX                  0xDF
#define	 LA_ID_INVALID                      0xFF

#define  LA_ID_NET_MISSED_CALL              0xE0
#define  LA_ID_NET_OUTGOING_CALL            0xE1
#define  LA_ID_NET_INCOMING_ACPTD_CALL      0xE2
#define  LA_ID_NET_ALL_CALL                 0xE3
#define  LA_ID_PRE_DIAL_LIST                0xE4
#define  LA_ID_DIAL_NOW_LIST                0xE5
#define  LA_ID_SCA_LIST                     0xE6

/* LA_LIST_DECT_SETTING_LISTS fields */
#define LA_DSSL_FLD_PINCODE             1
#define LA_DSSL_FLD_CLCK_MASTER         2
#define LA_DSSL_FLD_BASE_RESET          3
#define LA_DSSL_FLD_IP_ADDR_TYPE        4
#define LA_DSSL_FLD_IP_ADDR_VAL         5
#define LA_DSSL_FLD_IP_ADDR_SUBNET_MASK 6
#define LA_DSSL_FLD_IP_GATEWAY          7
#define LA_DSSL_FLD_IP_DNS_SERVER       8
#define LA_DSSL_FLD_FP_FW_VER           9
#define LA_DSSL_FLD_FP_EEPROM_VER       10
#define LA_DSSL_FLD_FP_HW_VER           11
#define LA_DSSL_FLD_EMISSION_MODE       12
#define LA_DSSL_FLD_NEW_PINCODE         13
#define LA_DSSL_ECO_MODE                0x81
#define LA_DSSL_REPEATER_MODE           0x82
#define LA_DSSL_MAC                     0x83
#define LA_DSSL_RFPI                    0x84
#define LA_DSSL_IP_DNS_SERVER_SECOND    0x85
#define LA_DSSL_IP_CURRENT              0x86

#define LA_DSSL_FLD_LOGIN_USER           0x87
#define LA_DSSL_FLD_LOGIN_PASSWD         0x88
#define LA_DSSL_FLD_LOGIN_TYPE_RPS       0x89
#define LA_DSSL_FLD_LOGIN_TYPE_PPPOE     0x8A
#define LA_DSSL_FLD_LOGIN_TYPE_BROADSOFT 0x8B

#define LA_DSSL_FLD_STATUS               0x8C
#define LA_DSSL_FLD_LOGIN_TYPE           0x8D
#define LA_DSSL_FLD_NOT_USED_14          0x8E



/* LA_LIST_DECT_SETTING_LISTS Login Type fields */
/* only can be used in session start / read, 只胜于读,写则通过LoginType传给GUI*/
#define LA_DSSL_LOGINFLD_TYPE_USER_MODE_ADMIN   0x81
#define LA_DSSL_LOGINFLD_TYPE_USER_MODE_VAR     0x82
#define LA_DSSL_LOGINFLD_TYPE_USER_MODE_USER    0x83
#define LA_DSSL_LOGINFLD_TYPE_NOT_USED_4 0x84
#define LA_DSSL_LOGINFLD_TYPE_NOT_USED_5 0x85
#define LA_DSSL_LOGINFLD_TYPE_NOT_USED_6 0x86
#define LA_DSSL_LOGINFLD_LOGIN_USER      0x87
#define LA_DSSL_LOGINFLD_LOGIN_PASSWD    0x88
#define LA_DSSL_LOGINFLD_TYPE_RPS        0x89
#define LA_DSSL_LOGINFLD_TYPE_PPPOE      0x8A
#define LA_DSSL_LOGINFLD_TYPE_BROADSOFT  0x8B

#define LA_DSSL_LOGINFLD_TYPE_NOT_USED_12   0x8C
#define LA_DSSL_LOGINFLD_LOGIN_TYPE         0x8D
#define LA_DSSL_LOGINFLD_TYPE_COMPORTAL  	0x8E
#define LA_DSSL_LOGINFLD_TYPE_NOT_USED_15   0x8C



// DECT SYS STATUS MASK BITS define:
#define LA_DSSL_STATUS_MASK_US_DECT_INVERT  0x01


/* LA_LIST_SUPPORTED_LISTS fields */
#define LA_SL_FLD_LIST_ID               1
#define LA_FLD_UNDEFINED                -1
#define LA_ALL_LIST_FLD_PROP_START_FLAG 0x8F

/* LA_LIST_MISSED_CALLS fields */
#define LA_MCL_FLD_NUMBER               1
#define LA_MCL_FLD_NAME                 2
#define LA_MCL_FLD_DATETIME             3
#define LA_MCL_FLD_NEW                  4
#define LA_MCL_FLD_LINE_NAME            5
#define LA_MCL_FLD_LINE_ID              6
#define LA_MCL_FLD_NR_OF_CALLS          7
#define LA_MCL_FLD_MAX                  7

#define LA_MCL_FLD_PROP_BLACKCALLER     0x81
#define LA_MCL_FLD_PROP_INFO            0x82

/* LA_LIST_OUTGOING_CALLS fields */
#define LA_OCL_FLD_NUMBER               1
#define LA_OCL_FLD_NAME                 2
#define LA_OCL_FLD_DATETIME             3
#define LA_OCL_FLD_LINE_NAME            4
#define LA_OCL_FLD_LINE_ID              5
#define LA_OCL_FLD_MAX                  5

#define LA_OCL_FLD_PROP_BLACKCALLER     0x81
#define LA_OCL_FLD_PROP_NUM_CALLS       0x82
#define LA_OCL_FLD_PROP_DURATION        0x83
#define LA_OCL_FLD_PROP_INFO            0x84

/* LA_LIST_INCOMING_ACC fields */
#define LA_IACL_FLD_NUMBER              1
#define LA_IACL_FLD_NAME                2
#define LA_IACL_FLD_DATETIME            3
#define LA_IACL_FLD_LINE_NAME           4
#define LA_IACL_FLD_LINE_ID             5
#define LA_IACL_FLD_MAX                 5

#define LA_IACL_FLD_PROP_BLACKCALLER    0x81
#define LA_IACL_FLD_PROP_NUM_CALLS      0x82
#define LA_IACL_FLD_PROP_DURATION       0x83
#define LA_IACL_FLD_PROP_INFO           0x84

/* LA_LIST_ALL_CALLS fields */
#define LA_ACL_FLD_CALL_TYPE            1
#define LA_ACL_FLD_NUMBER               2
#define LA_ACL_FLD_NAME                 3
#define LA_ACL_FLD_DATETIME             4
#define LA_ACL_FLD_LINE_NAME            5
#define LA_ACL_FLD_LINE_ID              6
#define LA_ACL_FLD_MAX                  6

#define LA_ACL_FLD_PROP_BLACKCALLER     0x81
#define LA_ACL_FLD_PROP_NUM_CALLS       0x82
#define LA_ACL_FLD_PROP_DURATION        0x83
#define LA_ACL_FLD_PROP_INFO            0x84

/* LA_LIST_CONTACT fields */
#define LA_CL_FLD_NAME                  1
#define LA_CL_FLD_FIRST_NAME            2
#define LA_CL_FLD_CONTACT_NR            3
#define LA_CL_FLD_ASSOC_MDY             4
#define LA_CL_FLD_LINE_ID               5
#define LA_CL_FLD_MAX                   5

#define LA_CL_FLD_DIR_INFO              0x81
#define LA_CL_FLD_ENTRY_ID              0x82
#define LA_CL_FLD_INFO                  0x83
#define LA_CL_FLD_DESCRIPTION           0x84 /* Description String*/

/* LA_LIST_INTERNAL_NAMES fields */
#define LA_INL_FLD_NUMBER               1
#define LA_INL_FLD_NAME                 2
#define LA_INL_FLD_CALL_INTERCEPTION    3
#define LA_INL_FLD_MAX                  3

#define LA_INL_FLD_HANDSET_CONFIG       0x81
#define LA_INL_FLD_HS_PERMISSION        0x82




/* LA_LIST_LINE_SETTINGS fields */
#define LA_LSL_FLD_LINE_NAME                       1
#define LA_LSL_FLD_LINE_ID                         2
#define LA_LSL_FLD_ATTACHED_HANDSETS               3
#define LA_LSL_FLD_DIALING_PREFIX                  4
#define LA_LSL_FLD_FP_MELODY                       5
#define LA_LSL_FLD_FP_VOLUME                       6
#define LA_LSL_FLD_BLOCKED_NUMBER                  7
#define LA_LSL_FLD_MULTI_CALL_MODE                 8
#define LA_LSL_FLD_INTRUSION_CALL                  9
#define LA_LSL_FLD_PERMANENT_CLIR                  10
#define LA_LSL_FLD_CALL_FORWARDING                 11
#define LA_LSL_FLD_CFNA                            12
#define LA_LSL_FLD_CFB                             13
#define LA_LSL_FLD_MAX                             13

#define LA_LSL_FLD_PROP_BALANCE_TITLE              0x81
#define LA_LSL_FLD_PROP_BALANCE_ACCOUNT            0x82
#define LA_LSL_FLD_PROP_BALANCE_VALUE_CURR         0x83

#define LA_LSL_FLD_PROP_LINE_STATUS                0x84
#define LA_LSL_FLD_PROP_VOICE_MAIL                 0x85
#define LA_LSL_FLD_PROP_OUTGOING_ATTACH_HANDSETS   0x86
#define LA_LSL_FLD_PROP_OUTGOING_DEFAULT_HANDSETS  0x87
#define LA_LSL_FLD_PROP_CW                         0x88
#define LA_LSL_FLD_PROP_ANON_CALL                  0x89
#define LA_LSL_FLD_PROP_ANON_REJ                   0x8a
#define LA_LSL_FLD_PROP_DND                        0x8b

#define LA_LSL_FLD_PROP_SCA                        0x8c
#define LA_LSL_FLD_PROP_SCA_STATUS                 0x8d

/*LA_ID_SCA_LIST fields*/
#define LA_SCA_FLD_INDEX                           1
#define LA_SCA_FLD_STATUS                          2
#define LA_SCA_FLD_LINE_ID                         3
#define LA_SCA_FLD_DISP_STR                        4


/* LA_LIST_BLACKCALLER fields */
#define LA_BLC_FLD_NUMBER               1

//LA_ID_COUNTRY_TONE_LIST 
#define LA_FIELD_COUNTRY_TONE           1

//LA_ID_PRE_DIAL_LIST 
#define LA_FIELD_PRE_DIAL_NAME          1
#define LA_FIELD_PRE_DIAL_NUMBER        2

//LA_ID_DIAL_NOW_LIST
#define LA_FIELD_DIAL_NOW_LINE_MASK     1
#define LA_FIELD_DIAL_NOW_RULE_STR      2

/* All SMS lists fields */
#define LA_ASMS_FLD_NUMBER              129
#define LA_ASMS_FLD_NAME                130
#define LA_ASMS_FLD_DATETIME            131
#define LA_ASMS_FLD_READSTAT            132
#define LA_ASMS_FLD_SENDSTAT            5
#define LA_ASMS_FLD_LINE_NAME           133
#define LA_ASMS_FLD_LINE_ID             134
#define LA_ASMS_FLD_FRAGS               135
#define LA_ASMS_FLD_SMS_TYPE            8
#define LA_ASMS_FLD_SMS_TEXT            136
#define LA_ASMS_FLD_MAX                 9


#define LA_BALANCE_TITLE_LEN            18
#define LA_BALANCE_ACCOUNT_LEN          18
#define LA_BALANCE_CUR_LEN              6
#define LA_BALANCE_VAL_LEN              22


/*LA session*/
#define LA_SESSION_MGR_MAX_FIELDS       32


#define LA_FP_VER_LEN                   24
#define LA_SEARCH_STR_LEN               20
#define LA_PIN_CODE_LEN                 8
#define LA_PIN_CODE_MAX                 4
#define LA_IP_ADDR_LEN                  16  //16 bytes for IPV4, 4 for IPV4
#define LA_LINE_NAME_LEN                32  //32 if CMBS
#define LA_PHONE_NAME_LEN               32  //24,32? if CMBS
#define LA_LIST_ID_COUNT                11
#define LA_PHONE_NUM_LEN                32  //32 if CMBS
#define LA_MAX_FIELDS_COUNT             13
#define LA_NO_PHONE_PER_CONTACT         3
#define LA_LOGIN_MAX_LEN                32
#define LA_COUNTRY_TONE_MAX_LEN         32
#define LA_ACTIVATION_CODE_MAX_LEN      16
#define LA_DIAL_PREFIX_MAX_LEN          16
#define LA_INTERNAL_NUM_MAX_LAN         1
#define LA_DIAL_NOW_MAX_LEN             32

/*SCA*/
#define LA14_SCA_MAX_LINES_PER_ACCOUNT  0X07

/*SCA Status*/
#define LA14_SCA_STATUS_IDLE            0x00
#define LA14_SCA_STATUS_ACTIVE          0x01
#define LA14_SCA_STATUS_HELD            0x02
#define LA14_SCA_STATUS_ALERTING        0x03
#define LA14_SCA_STATUS_SEIZED          0x04
#define LA14_SCA_STATUS_BRIDGE_ACTIVE   0x05
#define LA14_SCA_STATUS_BRIDGE_HELD     0x06
#define LA14_SCA_STATUS_PRIVATE_HELD    0x07

/*Hs Permission*/
#define LA_HS_PERMISSION_VALUE_BITS     2
#define LA_HS_PERMISSION_SIZE           51  /*(LA_HS_PERMISSION_VALUE_BITS*(DSR_HS_PM_INDEX_MAX+1)/8+1)*/
                                            /*如果LA_HS_PERMISSION_VALUE_BITS修改, 则只能DSR_HS_PM_INDEX_MAX,因为
                                            LA_HS_PERMISSION_SIZE基本上已经是一个FIELD能传的最大值了,或者通过多个FIELD传输*/

/* LA contact */
#define LA_CL_DIR                       0x00000001
#define LA_CL_SHOURTCUT                 0x00000002
#define LA_CL_DOWNLOADING               0x00000004  /*本目录正在下载更新，无法访问.*/
#define LA_CL_EDITABLE                  0x00000008  /*本目录支持，添加/修改/删除记录.*/
#define LA_CL_SEARCHABLE                0x00000010  /*本目录支持，搜索记录.*/
#define LA_CL_ADVANCE_SEARCH            0x00000020  /*BT Search dir.*/
#define LA_CL_PLCM_DIR                  0x00000040  /*PLCM dir.*/
#define LA_CL_DIR_ATTRS                 0x000000FF


#define LA_CL_LOCAL                     0x00000100
#define LA_CL_BS                        0x00000200
#define LA_CL_LDAP                      0x00000400
#define LA_CL_REMOTE                    0x00000800
#define LA_CL_SEARCH                    0x00001000
#define LA_CL_MTSW                      0x00002000  /*MTSW net dir.*/

#define LA_CL_DIR_MAIN_TYPES            0x0000FF00

#define LA_CL_DIR_CALL_NUMBER           0x00010000  /*默认呼叫号码为Number*/
#define LA_CL_DIR_CALL_MOBILE           0x00020000  /*默认呼叫号码为Mobile*/
#define LA_CL_DIR_CALL_GROUP            0x00040000  /*默认呼叫号码为Group*/
#define LA_CL_DIR_MTSW_CONTACT_GROUP    0x00080000  /*号码成员:home,work,mobile*/
#define LA_CL_DIR_MTSW_EXTENSION_GROUP  0x00100000  /*号码成员:Number,Extension,Department*/
#define LA_CL_DIR_MTSW_MLHGS_GROUP      0x00200000 /*号码成员:Number,Department*/
#define LA_CL_DIR_SUB_TYPES             0xFFFF0000

#define LA_CL_DIR_TYPES                 0xFFFFFF00

#define LA_HS_MMI_TIMEOUT               60

/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /* __DECT_LA_H__ */

