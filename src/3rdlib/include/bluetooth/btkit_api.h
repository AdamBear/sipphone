/***********************************************************************
*
*     DESCRIPTION:Copyright(c) 2018-2028 Xiamen Yealink Network Technology Co,.Ltd
*
*     AUTHOR:wanghb
*
*     HISTORY:
*
*     DATE:2018-03-05
*
*************************************************************************/
#ifndef __BTKIT_PUB_API_H__
#define __BTKIT_PUB_API_H__

#ifdef __cplusplus
extern "C" {
#endif
/************************************************************************/
/* MACRO                                                               */
/************************************************************************/

/*
*service uuid define
*/
#define BT_HF_AGW_UUID                   0x111f /*handfree gateway service*/
#define BT_HF_UUID                       0x111e /*handfree service*/
#define BT_AD_SRC_UUID                   0x110a /*audio source service*/
#define BT_AD_SNK_UUID                   0x110b /*audio sink service*/
#define BT_HS_UUID                       0x1108 /*headset service*/

/*
* BT MACRO
*/
#define BT_MAX_ID                        2
#define BT_MAX_INSTANCE                  1
#define BT_MAX_DEVICE                    35
#define BT_ADDR_STR_LEN                  18
#define BT_MAX_PATH_LEN                  64
#define BT_MAX_INTERFACE_LEN             64
#define BT_MAX_NAME_LEN                  64
#define BT_MAX_STATE_LEN                 64
#define BT_MAX_PIN_LEN                   20
#define BT_LOG_NAME_LEN                  5
#define BT_MAX_CALL_NUMBER_LEN           20
#define BT_MAX_UUID_LEN                  38
#define BT_MAX_SERVICES                  69

#define BT_PF_HS_MIN_VOLUME              0
#define BT_PF_HS_MAX_VOLUME              16

/*
 *msg from manager
 */
#define  BT_DEBUG_LOG_SET                0x00000000
#define  BT_DEBUG_LOG_DUMP               0x00000001

#define  BT_LOG_MODE_NONE                0x0000
#define  BT_LOG_MODE_SYS                 0x0001
#define  BT_LOG_MODE_CONS                0x0002
#define  BT_LOG_MODE_TIME                0x0004

#define  BT_LOG_INV                      -1
#define  BT_LOG_ALL                      0
#define  BT_LOG_LEVEL_EMERG              0 /* system is unusable */
#define  BT_LOG_LEVEL_ALERT              1 /* action must be taken immediately */
#define  BT_LOG_LEVEL_CRIT               2 /* critical conditions */
#define  BT_LOG_LEVEL_ERR                3 /* 3*/
#define  BT_LOG_LEVEL_WARN               4 /* 4*/
#define  BT_LOG_LEVEL_NOTICE             5 /* 5*/
#define  BT_LOG_LEVEL_INFO               6 /* 6*/
#define  BT_LOG_LEVEL_DEBUG              7 /* 7*/
#define  BT_LOG_LEVEL_SPY                8
#define  BT_LOG_LEVEL_ANY                9

/*
 *msg from adapter
 */
#define BT_ADAPTER_ADDED                 0x00010000 /* callback msg: BT_MSG.bt_instance*/
#define BT_ADAPTER_REMOVED               0x00010001 /* callback msg: BT_MSG.bt_instance*/
#define BT_FOUND_DEV                     0x00020002 /* callback msg: BT_MSG.bt_callback_param.dev_hnd*/
#define BT_REQ_PIN                       0x00020004
#define BT_PIN_CONFIRM                   0x00020005 /* callback msg: BT_MSG.bt_callback_param.pin_info*/
#define BT_PAIR_CANCEL                   0x00020006 /* callback msg: BT_MSG.bt_callback_param.dev_hnd*/
#define BT_AUTHORIZE                     0x00020007 /* callback msg: BT_MSG.bt_callback_param.auth_info*/
#define BT_POWER_ON                      0x00020008
#define BT_POWER_OFF                     0x00020009
#define BT_DISCOVERABLE_ON               0x0002000a
#define BT_DISCOVERABLE_OFF              0x0002000b

/*
 *msg from device
 */
#define BT_DEV_DISCONNECT_REQUESTED      0x00030001 /* callback msg: BT_MSG.bt_callback_param.dev_hnd*/
#define BT_DEV_PROPERTY_CHANGED          0x00030002
#define BT_FOUND_DEV_DISAPPEARED         0x00030003
#define BT_DEV_PAIR_CHANGED              0x00030004 /* callback msg: BT_MSG.bt_callback_param.paired*/
#define BT_DEV_DISCONNECTED              0x00040011
#define BT_DEV_CONNECTED                 0x00040012
#define BT_DEV_CONNECTED_CANCEL          0x00040013
#define BT_DEV_CONNECTED_FAILE           0x00040014

/*
 *msg from profile
 */
#define BT_DEV_PROFILE_CONNECTED         0x00040015
#define BT_DEV_PROFILE_CONNECT_FAIL      0x00040016
#define BT_DEV_PROFILE_DISCONNECTED      0x00040017

#define BT_PROFILE_SUPPORTED             1
#define BT_PROFILE_NOTSUPPORTED         -1
#define BT_PROFILE_ON                    1
#define BT_PROFILE_OFF                   0

/*
 *msg from handfree
 */
#define BT_HF_ANSWER_REQUEST             0x00040002 /* callback msg: BT_MSG.bt_callback_param.dev_hnd*/
#define BT_HF_CALL_TERMINATED            0x00040003
#define BT_HF_CALL_REDIAL                0x00040007
#define BT_HF_CALL_ANSWER                0x00040008
#define BT_HF_CALL_OUT                   0x00040009
#define BT_HF_SCO_PLAY_START             0x0004000a
#define BT_HF_SCO_PLAY_STOP              0x0004000b
#define BT_HF_SPK_GAIN_CHANGED           0x00040004 /* callback msg: BT_MSG.bt_callback_param.pf_hs_volume */
#define BT_HF_MIC_GAIN_CHANGED           0x00040005 /* callback msg: BT_MSG.bt_callback_param.pf_hs_volume */

/*
 *msg from AG
 */
#define BT_AG_SCO_PLAY_START             0x00050003 /* estabilsh the audio channel with mobile phone,send the msg to GUI
                                                      * If GUI get the msg£¬control vpm establish virtual line session.
                                                      * callback msg: BT_MSG.bt_callback_param.dev_hnd
                                                      */

#define BT_AG_SCO_PLAY_STOP              0x00050004 /* disconnect the audio channel with mobile, send the msg to GUI
                                                      * callback msg: BT_MSG.bt_callback_param.dev_hnd
                                                      */

#define BT_AG_CALL_STATUS_CHANGED        0x00050005 /* status changed when call make ,send the msg to GUI£¬
                                                      * import msg to GUI, get the status changed msg
                                                      * callback msg: BT_MSG.bt_callback_param.mb_call
                                                      */

#define BT_AG_INDICATOR_CHANGED          0x00050007 /* mobile phone condition changed,send the msg to GUI
                                                      * callback msg: BT_MSG.bt_callback_param.mb_cond
                                                      */

/*
* msg from A2DP
*/
#define BT_A2DP_PLAY_START               0x00060001
#define BT_A2DP_PLAY_STOP                0x00060002

/*
* msg from phone book
*/
#define BT_PBA_DOWNLOAD_COMPLETE         0x00070001 /* callback msg to GUI,download phonebook success,phonebook need remove by GUI 
                                                      * callback msg params: BT_MSG.bt_callback_param.pb_file_path
                                                      * the phonebook vcf file's format is vcard3.0, refer to rfc2426,get attributes { N, FN and TEL }
                                                      */

#define BT_PBA_DOWNLOAD_FAIL             0x00070002 /* callback msg to GUI, download phonebook failed */


typedef unsigned long  btkit_adp_handle_t;
typedef unsigned long  btkit_dev_handle_t;

/************************************************************************/
/*bluetooth device type                                                                 */
/************************************************************************/
typedef enum
{
    BT_UNKNOWN_TYPE,     /*0*/
    BT_COMPUTER,         /*1*/
    BT_PHONE,            /*2*/
    BT_MODEM,            /*3*/
    BT_NETWORK_WIRELESS, /*4*/
    BT_HEADSET,          /*5*/
    BT_HEADPHONE,        /*6*/
    BT_CAMERA_VIDEO,     /*7*/
    BT_AUDIO_CARD,       /*8*/
    BT_INPUT_GAMING,     /*9*/
    BT_INPUT_KEYBOARD,   /*10*/
    BT_INPUT_TABLET,     /*11*/
    BT_INPUT_MOUSE,      /*12*/
    BT_PRINTER,          /*13*/
    BT_CAMERA_PHOTO,     /*14*/
} btkit_dev_type_t;

/*
* callback param define
*/
typedef struct
{
    int                log_level;
    char               log_name[BT_LOG_NAME_LEN];
} btkit_cb_log_param_t;

typedef struct
{
    btkit_dev_handle_t dev_hnd;
    int                paired;
} btkit_cb_paired_param_t;

typedef struct
{
    btkit_dev_handle_t dev_hnd;
    char               pin[BT_MAX_PIN_LEN];
} btkit_pin_info_t;

typedef struct
{
    btkit_dev_handle_t dev_hnd;
    int                uuid;
} btkit_auth_info_t;

/*
* property define for adapter
*/
typedef struct
{
    char               bt_addr[BT_ADDR_STR_LEN];
    char               name[BT_MAX_NAME_LEN];
    unsigned int       bt_class;
    int                powered;
    int                discoverable;
    int                Pairable;
    int                discoverable_to;
    int                pairable_to;
    int                discovering;
    char               devices[BT_MAX_DEVICE][BT_MAX_PATH_LEN];
    int                local_service[BT_MAX_SERVICES]; /* ±¾µØÖ§³ÖµÄUUID*/
} btkit_property_t;

/*
* property define for device
*/
typedef struct
{
    char               bt_dev_addr[BT_ADDR_STR_LEN]; /* ¸ñÊ½:7C:B1:5D:84:8B:7C*/
    char               name[BT_MAX_NAME_LEN];
    unsigned int       bt_class;
    char               alias[BT_MAX_NAME_LEN];
    unsigned short     vendor;
    unsigned short     vendor_source;
    unsigned short     product;
    unsigned short     version;
    int                paired;
    int                trusted;
    int                blocked;
    int                connected;
    int                auto_reconnect;
    int                uuids[BT_MAX_SERVICES];
    int                con_status;
} btkit_dev_t;

/*
* property define for headset
*/
typedef struct
{
    int                playing;
    char               state[BT_MAX_STATE_LEN];
    int                connected;
    unsigned short     spk_gain;
    unsigned short     mic_gain;
} btkit_headset_property_t;

/*
* property define for a2dp.
*/
typedef enum
{
    A2DP_STATE_DISCONNECTED,
    A2DP_STATE_CONNECTING,
    A2DP_STATE_CONNECTED,
    A2DP_STATE_PLAYING,
    A2DP_STATE_STOPED,
} a2dp_state_t;

typedef struct
{
    char              state[BT_MAX_STATE_LEN];/*½öÄÚ²¿Ê¹ÓÃ*/
    int               playing;  /*½öÄÚ²¿Ê¹ÓÃ*/
    int               connected;/*½öÄÚ²¿Ê¹ÓÃ*/
    a2dp_state_t      pub_state;/*Íâ²¿Ê¹ÓÃ*/
} btkit_a2dp_property_t;

/*
* property define for mobile
*/
typedef struct
{
    btkit_dev_handle_t dev_hnd;
    int                signal_level; /* mobile signal level[0 - 5]*/
    int                battery_level;/* mobile battery level[0 - 5]*/
    int                in_roam;      /* mobile roaming[0 - 1]*/
    int                in_service;   /* mobile service[0 - 1]*/
} btkit_mb_cond_t;

typedef struct
{
    btkit_dev_handle_t dev_hnd;
    btkit_mb_cond_t    cond;

    int                call;
    int                call_setup;
    int                call_hold;
    char               state[BT_MAX_STATE_LEN];
    char               local_phonenum[BT_MAX_CALL_NUMBER_LEN];
} btkit_mb_property_t;

typedef struct
{

    btkit_dev_handle_t dev_hnd;

    int                callid;
    int                status;
#define     BT_MB_CALL_STATUS_UNKNOWN            0x00
#define     BT_MB_CALL_STATUS_TRYING             0x01 //sip phone call out start flag
#define     BT_MB_CALL_STATUS_MOBILE_TRYING     0x02 //mobile call out start flag
#define     BT_MB_CALL_STATUS_RINGBACK          0x03
#define     BT_MB_CALL_STATUS_ESTABLISHED        0x04  //call finshed or held/hold
    //finshed: status=-1, hold=0
    //hold:    status=1,  hold=1
    //held:    status=0,  hold=0
#define     BT_MB_CALL_STATUS_FAIL                0x05
#define     BT_MB_CALL_STATUS_INCOME              0x06
#define     BT_MB_CALL_STATUS_FINISHED            0x07


    /*BT_MB_CALL_STATUS_ESTABLISHED will use this param*/
    int                 hold;

    char               number[BT_MAX_CALL_NUMBER_LEN];

} btkit_mb_call_status_t;

/*
* phonebook define
*/
typedef struct
{
    char               name[BT_MAX_NAME_LEN];
    char               filename[BT_MAX_NAME_LEN];
    int                size;
    int                progress;
    char               reverse[4];
} btkit_pba_property_t;

typedef struct
{
    btkit_dev_handle_t dev_hnd;
    char               file[BT_MAX_NAME_LEN]; /*location the phonebook saved ,such as: /tmp/pabp_tmp.vcf*/
    int                type;                  /*phonebook type*/
    int                photo;                 /*download phonebook with photo or no*/
} btkit_pba_data_t;

typedef struct
{
    btkit_dev_handle_t dev_hnd;
    char               file[BT_MAX_NAME_LEN]; /*location the phonebook saved*/
} btkit_pb_file_t;

/*phonebook type*/
#define     BT_PB_TYPE_MIN          0x00
#define     BT_PB_TYPE_SIM_PB       0x01
#define     BT_PB_TYPE_SIM_ICH      0x02
#define     BT_PB_TYPE_SIM_OCH      0x03
#define     BT_PB_TYPE_SIM_MCH      0x04
#define     BT_PB_TYPE_PHONE_PB     0x05 /*phonebook for the saved contacts*/
#define     BT_PB_TYPE_PHONE_ICH    0x06 /*incoming call history*/
#define     BT_PB_TYPE_PHONE_OCH    0x07 /*outgoing call history*/
#define     BT_PB_TYPE_PHONE_MCH    0x08 /*missing call history*/
#define     BT_PB_TYPE_MAX          0x09

typedef struct
{
    btkit_dev_handle_t dev_hnd;
    const char*        code;
    const char*        msg;
} btkit_pb_error_t;

/*
 * profiles property define for common
 */
typedef union
{
    btkit_mb_property_t       pty_mb;
    btkit_headset_property_t  pty_hs;
    btkit_a2dp_property_t     pty_a2dp_source;
    btkit_pba_property_t      pty_pba;/*ÔÝ²»Ê¹ÓÃ*/
} profile_property_t;


/*
* profiles define
*/
typedef struct
{
    btkit_dev_handle_t dev_hnd;  /*instance for device,so that we can find the dev by it.*/
    int                active;   /*indicate if the profile is actived.ÄÚ²¿Ê¹ÓÃ£¬²»ÓÃ½âÎö*/
    int                uuid;     /*indicate what service of the profile,and it's descripted in UUID.*/
    int                connected;/*indicate the if this service connected to remote service or not.*/
    profile_property_t  pty;     /*property of profile*/
} btkit_profiles_t;

/*
* callback param
*/
typedef union
{
    int                      bt_id;
    btkit_cb_paired_param_t  pair_info;
    btkit_cb_log_param_t     log_info;

    btkit_dev_handle_t       dev_hnd;
    btkit_pin_info_t         pin_info;

    btkit_auth_info_t        auth_info;
    int                      pf_hs_spk_volume;
    int                      pf_hs_mic_volume;
    char                     pf_hf_call_num[BT_MAX_CALL_NUMBER_LEN];

    /*handsfree profile of ag role param*/
    btkit_mb_cond_t          mb_cond;
    /*when BT_AG_INDICATOR_CHANGED,sync mb_cond to GUI */


    btkit_mb_call_status_t   mb_call;
    /*when BT_AG_CALL_STATUS_CHANGED,send mb_call to GUI;
    if mb_call.status == BT_MB_CALL_STATUS_FINISHED,mean call ending.
       else if mb_call.status == BT_MB_CALL_STATUS_MOBILE_TRYING,mean inviting someone,wait call make
       else if mb_call.status == BT_MB_CALL_STATUS_INCOME ,mean call incoming,need to answer
    */

    /*phonebook access profile*/
    btkit_pb_file_t          pb_file;
    /*when T_PBA_DOWNLOAD_COMPLETE,send pb_file to GUI*/

    btkit_pb_error_t         pb_err;

    btkit_profiles_t         pf_info;/*profile information include connected status and property*/
} btkit_callback_param_t;

typedef struct
{
    btkit_adp_handle_t       bt_instance;      /*bt adapater(dongle) instance*/
    int                      bt_event;         /*bt adapater(dongle) evect*/
    btkit_callback_param_t   bt_callback_param;
} btkit_msg_t;

typedef void           btkit_event_t(const btkit_msg_t* msg, void* user_data);

typedef struct
{
#define BTKIT_FLG_NONE      0x00000000 /* None */
    unsigned int       flags;           /* extension flags */
    int                log_type;
    int                log_level;
    const char*        log_file;
} btkit_cfg_t;

typedef struct
{
    btkit_event_t*     event_cb;
    void*              user_data;
} btkit_callback_t;

typedef struct
{
    unsigned int       id;
    const char*        name;
    const char*        alias;
} btkit_create_t;

/************************************************************************/
/* declare                                                             */
/************************************************************************/
void btkit_set_log_level(int level);
int  bt_init(const btkit_cfg_t *cfg, btkit_callback_t* cb);
int  bt_exit(void);
const char *  btkit_build(void);
const char *  btkit_version(void);
int  btkit_create(btkit_adp_handle_t *hnd, btkit_create_t *create);
int  btkit_destroy(btkit_adp_handle_t hnd);
int  btkit_power_set(btkit_adp_handle_t hnd,  int enable);
int  btkit_discoverable_set(btkit_adp_handle_t hnd, int enable, int time_out);

/************************************************************************
function:    set bluetooth adapter(dongle) name
params:     name
return: 0 --success
          other  ---failure
************************************************************************/
int  btkit_name_set(btkit_adp_handle_t hnd, const char* name);


/************************************************************************
function:    get bluetooth adapter(dongle) property
params:     property(btkit_property_t--get)
return: 0 --success
          other  ---failure
************************************************************************/
int  btkit_property_get(btkit_adp_handle_t hnd, btkit_property_t* property);


/************************************************************************
function:    get bluetooth adapter(dongle) type
params:
return: btkit_dev_type_t
************************************************************************/
btkit_dev_type_t  btkit_type_get(btkit_adp_handle_t hnd);


/************************************************************************
function:    get  how many bluetooth devices the bluetooth adapter(dongle) have
params:
return: the total of bluetooth devices from dongle(bt adapter)
************************************************************************/
int  btkit_devices_sum_get(btkit_adp_handle_t hnd);


/************************************************************************
function:    get  how many bluetooth devices the bluetooth adapter(dongle) have
params:
return: 0 success, otherwise failure
************************************************************************/
int  btkit_devices_get(btkit_adp_handle_t hnd, btkit_dev_handle_t *dev_hnd);

int  btkit_scan_start(btkit_adp_handle_t hnd);
int  btkit_scan_stop(btkit_adp_handle_t hnd);
int  btkit_dev_cancel_pair(btkit_adp_handle_t hnd, btkit_dev_handle_t  dev_hnd);
int  btkit_dev_pair(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd);
int  btkit_dev_unpair(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd);


/************************************************************************
* @function:  delete bt device
* @params:
* @return:     0 --success
                    other  ---failure
************************************************************************/
int  btkit_dev_delete(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd);


btkit_dev_type_t  btkit_dev_type_get(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd);


/************************************************************************
* @function:   answer pin to paire bt device
* @params:
* @return:     0 --success
                    other  ---failure
************************************************************************/
int  btkit_dev_pin_answer(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, const char*pin);


int  btkit_dev_pin_authorize(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int right);
int  btkit_dev_pin_confirm(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int right);
int  btkit_dev_connected(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd);

/************************************************************************
* @function:   paire bt device
* @params:
* @return:     0 --success
                    other  ---failure
************************************************************************/
int  btkit_dev_paired(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd);


int  btkit_dev_addr_get(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, char *addr);

/************************************************************************
* @function:   set bt device alias
* @params:
* @return:     0 --success
                    other  ---failure
************************************************************************/
int  btkit_dev_alias_set(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, const char*name);


/************************************************************************
* @function:   connect bt device
* @params:
* @return:     0 --success
                    other  ---failure
************************************************************************/
int  btkit_dev_connect(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd);

int  btkit_dev_reconnect(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd);

/************************************************************************
* @function:   disconnect bt device
* @params:
* @return:     0 --success
                    other  ---failure
************************************************************************/
int  btkit_dev_release(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd);

/************************************************************************
* @function:   set mic gain
* @params:
* @return:     0 --success
                    other  ---failure
************************************************************************/
int  btkit_dev_mic_gain_set(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd,  int gain);

/************************************************************************
* @function:   get mic gain
* @params:
* @return:     0 --success
                    other  ---failure
************************************************************************/
int  btkit_dev_mic_gain_get(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int*gain);


/************************************************************************
* @function:   set spk gain
* @params:
* @return:     0 --success
                    other  ---failure
************************************************************************/
int  btkit_dev_spk_gain_set(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int gain);

/************************************************************************
* @function:   get spk gain
* @params:
* @return:     0 --success
                    other  ---failure
************************************************************************/
int  btkit_dev_spk_gain_get(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int*gain);

/************************************************************************
* @function:   headset ring start
* @params:
* @return:     0 --success
                    other  ---failure
************************************************************************/
int  btkit_dev_ring_start(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd);

/************************************************************************
* @function:   headset ring stop
* @params:
* @return:     0 --success
                    other  ---failure
************************************************************************/
int  btkit_dev_ring_stop(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd);


/************************************************************************
* @function:   headset play start
* @params:
* @return:     0 --success
                    other  ---failure
************************************************************************/
int  btkit_dev_play_start(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd);

/************************************************************************
* @function:   headset play stop
* @params:
* @return:     0 --success
                    other  ---failure
************************************************************************/
int  btkit_dev_play_stop(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd);

/************************************************************************
* @function:   headset or headfree invite  call
* @params:    number
* @return:     0 --success
                    other  ---failure
************************************************************************/
int  btkit_dev_call_invite(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd,  const char*number, int *callid);


/************************************************************************
* @function:   notice headset ring alter
* @params:     number
* @return: 0 --success
          other  ---failure
************************************************************************/
int  btkit_dev_call_in(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, const char*number);


/************************************************************************
* @function:  answer call
* @params:   callid
* @return: 0 --success
          other  ---failure
************************************************************************/
int  btkit_dev_call_answer(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int callid);


/************************************************************************
* @function:  hangup call
* @params:   callid
* @return: 0 --success
          other  ---failure
************************************************************************/
int  btkit_dev_call_hangup(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int callid);


/************************************************************************
* @function:  notice headfree hold or held call.
* @params:   callid
              is_hold 1--hold,0 --held
* @return: 0 --success
          other  ---failure
************************************************************************/
int  btkit_dev_call_hold(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int callid, int is_hold);


/************************************************************************
* @function:  get  total of call session
* @params:     number
* @return:  >=0 --success
                  other  ---failure
************************************************************************/
int  btkit_dev_call_total(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int *total);

/************************************************************************
* @function:    get call status from mobile
* @params:
* @return: call status --> BT_MB_CALL_STATUS_XXX
************************************************************************/
int  btkit_dev_call_status(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int callid, int *status);

/************************************************************************
* @function:    will send each digit to mobile
* @params:     dtfm
* @return: 0 --success
          other  ---failure
************************************************************************/
int  btkit_dev_call_send_dtmf(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int callid, const char dtmf);


/************************************************************************
* @function:    get bluetooth device property
* @params:     property(btkit_dev_t--get)
* @return: 0 --success
          other  ---failure
************************************************************************/
int  btkit_dev_property_get(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, btkit_dev_t* property);

/************************************************************************
* @functionL:get mobile property
* @params:
* @return: 0 -->success
          other -- failure
************************************************************************/
int btkit_mb_get_property(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, btkit_mb_property_t* property);

/************************************************************************
* @function:    start to download phonebook from phone
* @params:     data(btkit_pba_data_t--put)     addr  ---- phone addr
                                                     type  ----phonebook type
                                                     file    ----locate the phonebook saved ,such as: /tmp/pabp_tmp.vcf
                                                     picuture --phonebook include picture ,1: have picture ,0:no picture

* @return: 0 --success
             other  ---failure
callback msg: BT_PBA_DOWNLOAD_FAIL or BT_PBA_DOWNLOAD_COMPLETE
************************************************************************/
int  btkit_pba_download_start(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, void* data);

/************************************************************************
* @function:  download phonebook finshed, need to exec this func to release session resource
* @params:
* @return: 0 --success
          other  ---failure
************************************************************************/
int  btkit_pba_download_stop(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd);


#define BT_SND_MODE_LOC_TO_MB           0x00070002
#define BT_SND_MODE_MB_TO_LOC           0x00070001
#define BT_SND_MODE_UNKNOWN             0x00070000
/************************************************************************
* @function:  set audio channel change ,change sip phone to mobile phone ,otherwise mobile phone to sip phone
* @params:
* @return: 0 --success
          other  ---failure
************************************************************************/
int  btkit_snd_mode_set(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int callid, int mode);


/************************************************************************
* @function:   if the profile supported depend on both local device and remote device
* @params:    uuid: kind of profile
                *value:PROFILE_SUPPORTED/PROFILE_NOTSUPPORTED
* @return:     0 --success
                    other  ---failure
************************************************************************/
int btkit_dev_profile_supported_get(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int uuid, int *value);


/************************************************************************
* @function:   get profile activity,which indicate profile's ON/OFF
* @params:   uuid: kind of profile
               *active:PROFILE_ON/PROFILE_OFF
* @return:     0 --success
                    other  ---failure
************************************************************************/
int btkit_dev_profile_activity_get(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int uuid, int *active);


/************************************************************************
* @function:   set profile activity,which indicate  profile's ON/OFF
* @params:   uuid: kind of profile
               active:PROFILE_ON/PROFILE_OFF
* @return:     0 --success
                    other  ---failure
************************************************************************/
int btkit_dev_profile_activity_set(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int uuid, int active);

/************************************************************************
* @function:   check if a profile is connected
* @params:   uuid: kind of profile
* @return:     0 -- profile was disconnected
                    1 -- profile was connected
                    other  ---failure
************************************************************************/
int btkit_dev_profile_connected(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int uuid);


/************************************************************************
* @function:  connect to a profile service
* @params:   uuid: kind of profile
* @return:     0 --success
                    other  ---failure
************************************************************************/
int btkit_dev_profile_connect(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int uuid);


/************************************************************************
* @function:  disconnect  a profile service
* @params:   uuid: kind of profile
* @return:     0 --success
                    other  ---failure
************************************************************************/
int btkit_dev_profile_disconnect(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int uuid);


/************************************************************************
* @function:   get a profile property
* @params:   uuid: kind of profile
               *property:property
* @return:     0 --success
                    other  ---failure
************************************************************************/
int btkit_dev_profile_property_get(btkit_adp_handle_t hnd, btkit_dev_handle_t dev_hnd, int uuid, profile_property_t* property);

#ifdef __cplusplus
}
#endif

#endif

