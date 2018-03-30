/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2011-07-12
*
*******************************************************************/
#ifndef __DECT_API_H__
#define __DECT_API_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/
#include "dect_config.h"
#include "dect_type.h"
#include "dect_msg.h"
#include "dect_tone.h"
#include "dect_fwup.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifndef NETLINK_VOIP
#define NETLINK_VOIP   20
#endif
#ifndef NETLINK_CONFIG
#define NETLINK_CONFIG 23
#endif
#ifndef NETLINK_DSR
#define NETLINK_DSR    26
#endif
#ifndef NETLINK_MMI
#define NETLINK_MMI    27
#endif
#ifndef NETLINK_DH
#define NETLINK_DH  29
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
typedef struct 
{
#define  DECT_NET_NONE      0x00000000
#define  DECT_NET_RECV      0x00000001
#define  DECT_NET_SEND      0x00000002
#define  DECT_NET_ALL       (DECT_NET_SEND|DECT_NET_RECV)

    int  flags;
    int  af_type;
    int  sa_len;

    union
    {
        struct sockaddr_in  sa4;
        struct sockaddr_in6 sa6;
    }src;
    union
    {
        struct sockaddr_in  sa4;
        struct sockaddr_in6 sa6;
    }dst;

}DECT_ADDR_T;

typedef struct 
{
#define  DECT_OPT_VALID         0x80000000
#define  DECT_OPT_PTIME         0x00000001
#define  DECT_OPT_PTEVENT       0x00000002
#define  DECT_OPT_JIB           0x00000010
#define  DECT_OPT_JIB_DIS       0x00000020
#define  DECT_OPT_JIB_FIX       0x00000040
#define  DECT_OPT_DTMF          0x00000100
#define  DECT_OPT_DTMF_2833     0x00000200
#define  DECT_OPT_DTMF_ENDS     0x00000400
#define  DECT_OPT_DTMF_PARAM    0x00000800
#define  DECT_OPT_CNG           0x00001000
#define  DECT_OPT_PLC_DIS       0x00002000
#define  DECT_OPT_VAD_DIS       0x00004000
#define  DECT_OPT_RTCP          0x00010000
#define  DECT_OPT_G726_REVERSED 0x00020000
#define  DECT_OPT_DETECT_RTP_PKG_LEN 0x00040000
#define  DECT_OPT_QOS           0x00080000

    int  opts;
    /*ptime*/
    int  opt_ptime;
    int  opt_tx_pt_event;
    int  opt_rx_pt_event;
    /*JIB*/
    int  opt_jb_max;
    int  opt_jb_min;
    /*DTMF*/
    int  opt_dtmf_2833ends;
    int  opt_dtmf_volume;
    int  opt_dtmf_duration;
    int  opt_dtmf_duration_evt;
    int  opt_dtmf_duration_max;
    int  opt_dtmf_mark;

    /*CNG*/
    int  opt_cng_level_tx;
    int  opt_cng_level_rx;
    int  opt_cng_mode_tx;
    int  opt_cng_mode_rx;
    int  opt_cng_vad_detect_level;
    int  opt_cng_vad_hangover;
    int  opt_cng_sid_update_max;

    /*RTCP*/
    int  opt_rtcp_interval;
    int  opt_rtcp_xr_session_report_enable;
    int  opt_rtcp_xr_interval_report_enable;
    int  opt_rtcp_xr_interval_period;
    int  opt_rtcp_xr_moslq_warning;
    int  opt_rtcp_xr_moslq_critical;
    int  opt_rtcp_xr_delay_warning;
    int  opt_rtcp_xr_delay_critical;

    /*Qos*/
    int  opt_qos;
}DECT_OPTS_T;

typedef struct 
{
#define  DECT_TALK_NONE   0x00000000
#define  DECT_TALK_VALID  0x80000000
#define  DECT_TALK_RTP    0x00000001
#define  DECT_TALK_RTCP   0x00000002
#define  DECT_TALK_USER   0x00000010
#define  DECT_TALK_KERN   0x00000020
#define  DECT_TALK_DIRC   0x00000040
#define  DECT_TALK_MASK   0x000000F0

    unsigned int  flags;
    /*address*/
    DECT_ADDR_T   addr_rtp;
    DECT_ADDR_T   addr_rtcp;
    /*codec*/
    char          enc_name[16];
    int           enc_payload; 
    char          dec_name[16];
    int           dec_payload;
    /*srtp*/
    char          enc_srtp_opt[128];
    char          dec_srtp_opt[128];

    /* codec params */
    DECT_OPTS_T   options;
}DECT_TALK_T;

/************************************************************************/
/* APIs define                                                          */
/************************************************************************/
/*
*-----------------------
* for APIs on c
*-----------------------
*/
#ifdef __cplusplus
extern "C"{
#endif 

#define  DECT_POLL_NONE    0
#define  DECT_POLL_SINGLE  1
#define  DECT_POLL_SELF    2

    /*
    * net service
    */
    typedef unsigned long NETSRV_H;
    typedef void (*NETSRV_CB)(void * priv,const void *msg_data,int msg_len);

    int netsrv_init (int sock_max,int sock_loop,int poll_type);
    int netsrv_exit (void);

    int netsrv_open  (NETSRV_H*ph_srv,const char *name, int protocol,NETSRV_CB process,void*private_data,int poll_type);
    int netsrv_close (NETSRV_H h_srv);
    int netsrv_fdset (NETSRV_H h_srv,fd_set *fdset);
    int netsrv_fdpoll(NETSRV_H h_srv,fd_set *fdset);
    int netsrv_send  (NETSRV_H h_srv,const void *data, int len);

    /*
    * log control
    */
    int         dect_log_get (int module);
    const char* dect_log_name(int module);
    int         dect_log_set (int module,int log_level);
    int         dect_log_set2(const char*module,int log_level);
    void        dect_log     (int module,int log_level,const char* fmt,...);

    /*
    * voice stream
    */
    int  dect_chan_init  (int poll_type);
    void dect_chan_exit  (void);
    int  dect_chan_config(DECT_OPTS_T *opts);

    int  dect_chan_alloc (int nChan);
    int  dect_chan_free  (int nChan);
    int  dect_chan_active(int nChan);
    int  dect_chan_start (int nChan, DECT_TALK_T *cfg,int self_poll);
    int  dect_chan_stop  (int nChan);
    int  dect_chan_update(int nChan, DECT_TALK_T *cfg);
    int  dect_chan_route (int nChan, int rtp_mode, int rtcp_mode);
    int  dect_chan_route2(int nChan, int rtp_mode, int rtcp_mode);
    int  dect_chan_DTMF  (int nChan,char key,int to_net,int to_hs);
    int  dect_chan_2ptype(const char* cocec);

    int dect_chan_write(int chan,const void *p_data, int n_len);
    int dect_chan_read (int chan,      void *p_data, int n_len);

    /*
    * get decode /encode latency
    * return :
    * < 0 :error
    * >=0 :latency as ms
    */
    int dect_chan_decLatency(int chan);
    int dect_chan_encLatency(int chan);

    /*
    * country tone
    */
    int         dect_tone_profile(const DECT_TONE_T* tone,int nr);
    int         dect_tone_verify(unsigned short *verify0,unsigned short *verify1);
    const void* dect_tone_retrive(int start,int end);
    int         dect_tone_size(int nr);
    void        dect_tone_dump(int start,int end);

    /*
    * ss7 service
    */
    u32  ss7_ap_instance(int ap);
    int  ss7_ap_address (char *addr, int type, int plan, int pres, int scrn);

	/*
	* DSR common api
	*/
	int service_DSR_GetLoginFieldFromFields(u32 aFields[],u32 nFields);
	int service_DSR_DelLoginFieldFromFields(u32 aFields[],u32 nFields,int nLoginField);
	int service_DSR_GetLoginFieldFromFieldMask(u32 u32_PropFields);
	int service_DSR_DelLoginFieldFromFieldMask(u32 u32_PropFields,int nLoginField);


    /*
    * fwup service
    */
    typedef unsigned long FWUPINST_H;
    typedef void (*FWUPSRV_CB)(void * priv,int hs_nr,int even,int param);
    
    int dect_dh_init(int poll_type);
    int dect_dh_exit(void);

    int fwup_init(const char* rom_file,FWUPSRV_CB event_cb,void *private_data);
    int fwup_exit(void);
    int fwup_get_info(void);
    
    int fwup_inst_create(FWUPINST_H *handle,int hs_nr,int flags);
    int fwup_inst_destroy(FWUPINST_H handle);
    FWUPINST_H fwup_inst_get_handle(int hs_nr);
    
    int fwup_inst_start(FWUPINST_H handle);
    int fwup_inst_stop(FWUPINST_H handle);

    int fwup_inst_get_info(void);
    
    void fwup_rom_StrVerToHexVer(char *strVersion,unsigned char *hexVersion);
    void fwup_rom_HexVerToStrVer(unsigned char *hexVersion, char *strVersion, unsigned char hex_len);
    int fwup_rom_get_rom_version(const char* p_rom_file,const char* p_rom_buf,int   n_rom_len, 
                                         char* p_dev_name,char* p_oem_name,char* p_rom_version ,int *p_dev_id);


    /*
    * vq report service
    */
    typedef void (*VQREPORT_CB)(int chan, int vq_type, void *vq_report);



    /*
    * error
    */
    const char * dsr_ReasonString(int code);

    int dect_initilize(int log_sys,int log_std,int log_time);

    /*
    *-----------------------
    * end APIs on c
    *-----------------------
    */
#ifdef __cplusplus
}
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
typedef unsigned long       srv_handle_t;

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
*-----------------------
* for APIs on c
*-----------------------
*/
#ifdef __cplusplus
extern "C"{
#endif 
    /*
    * service for service
    */
    int  service_Initialize     (int log_sys,int log_std,int log_time);
    int  service_Create         (srv_handle_t*handle,int poll_type,NETSRV_CB ss7_cb,NETSRV_CB dsr_cb,NETSRV_CB cfg_cb,NETSRV_CB mmi_cb,void*pri);
    int  service_Destroy        (srv_handle_t handle);
    int  service_SetLogLevel    (const char*module,int log_level);
    int  service_SetLogLevel2   (int module,int log_level);
    int  service_GetLogLevel    (int module);
    int  service_SetFD          (srv_handle_t handle,fd_set *fdset);
    int  service_PollFD         (srv_handle_t handle,fd_set *fdset);
    int  service_VQReportInit   (srv_handle_t handle,VQREPORT_CB vq_cb);

    /*
    * service for SS7
    */
    int  service_SS7_MsgSend            (srv_handle_t handle,const ss7_msg *ss7);
    int  service_SS7_ReqStart           (srv_handle_t handle);
    int  service_SS7_ReqIam             (srv_handle_t handle,u32 instance,u32 line, u32 hsmask,const char *dst, const char *src,const char *name, const char *multi_name[DECT_SS7_INST_NUM_MAX],const u8 *codec, u8 codec_len,u8 AlertPattern);
    int  service_SS7_ReqAcm             (srv_handle_t handle,u32 instance,u8 RingType);
    int  service_SS7_ReqSam             (srv_handle_t handle,u32 instance, const char *num);
    int  service_SS7_ReqRel             (srv_handle_t handle,u32 instance, u8 reason);
    int  service_SS7_ReqAnm             (srv_handle_t handle,u32 instance, const u8 *codec, u8 codec_len);
    int  service_SS7_ReqInf             (srv_handle_t handle,u32 instance, u32 infotype, const u8 *name, const u8*src,u32 line, const u8 *codec, u8 codec_len, u8 call_possible);
    int  service_SS7_ReqSus             (srv_handle_t handle,u32 instance);
    int  service_SS7_ReqRes             (srv_handle_t handle,u32 instance);
    int  service_SS7_ReqChst            (srv_handle_t handle,u32 instance, u32 state);
    int  service_SS7_ReqResAck          (srv_handle_t handle,u32 instance);
    int  service_SS7_ReqSusAck          (srv_handle_t handle,u32 instance);
    int  service_SS7_RspLineCfm         (srv_handle_t handle,u32 instance,u32 line,int bLinePossible);
    int  service_SS7_RspDialCfm         (srv_handle_t handle,u32 instance,u32 line,int bDialPossible, u8*name,u32 name_len,  u8* p_multi_name[DECT_SS7_INST_NUM_MAX],u32 multi_len[DECT_SS7_INST_NUM_MAX]);
    int  service_SS7_CallEncrypt        (srv_handle_t handle,u32 instance,u8 CallEncrypt);
    int  service_SS7_ClipCnipUpdateCfm  (srv_handle_t handle,u32 instance,u32 line,int bDialPossible,u32 infotype, u8*Number, u32 Number_len, u8* p_multi_name[DECT_SS7_INST_NUM_MAX],u32 multi_len[DECT_SS7_INST_NUM_MAX]);
    int  service_SS7_CallAutoAnswer     (srv_handle_t handle,u32 instance,u8 delay_seconds /*not used now*/);
    int  service_SS7_PropToneInd        (srv_handle_t handle,u32 instance,u8 tone_id);
    int  service_SS7_GetAPInstance      (void);
    int  service_SS7_GetAPAddress       (char *addr, int type, int plan, int pres, int scrn);
    int  service_SS7_ReqRtpEvt          (srv_handle_t handle, u16 chan, u32 opts);
    int  service_SS7_RtpEvtPkt          (const ss7_msg_rtp_evt_res *evt_res);
    int  service_SS7_PropClipNameInd(srv_handle_t handle,u32 instance,u8* p_multi_name[DECT_SS7_INST_NUM_MAX],u32 multi_len[DECT_SS7_INST_NUM_MAX]);

	#if DECT_CONFIG_MMI_SERVICE
    /*
    * service for MMI
    */
    int  service_MMI_MsgSend        (srv_handle_t handle,const mmi_msg *mmi);
    int  service_MMI_MsgSendTo      (srv_handle_t handle,int cmd,void *body,int len);
    int  service_MMI_ReqStart       (srv_handle_t handle);

    int  service_MMI_ReqSetChannel  (srv_handle_t handle,mmi_channel *chan);
    int  service_MMI_ReqSetProfile  (srv_handle_t handle,mmi_profile *prof);

    int  service_MMI_ReqTonePlay    (srv_handle_t handle,mmi_tone *tone);
    int  service_MMI_ReqCallStart   (srv_handle_t handle,int line ,int callid);
    int  service_MMI_ReqCallHold    (srv_handle_t handle,int line,int callid,int inst, int dect_chan);
    int  service_MMI_ReqCallResume  (srv_handle_t handle,int line,int callid,int inst, int dect_chan);
    int  service_MMI_ReqCallEnd     (srv_handle_t handle,int line,int callid,int inst, int dect_chan);
    int  service_MMI_ReqCallConfer  (srv_handle_t handle,int line[],int nr);
    int  service_MMI_ReqCallSplite  (srv_handle_t handle,int line[],int nr);
    int  service_MMI_ReqReadAFE     (srv_handle_t handle,int addr[],int size);
    int  service_MMI_ReqWriteAFE    (srv_handle_t handle,int addr[],int value[],int size,int delay);
    int  service_MMI_ReqReadDSP     (srv_handle_t handle,int instance,int offset[],int size);
    int  service_MMI_ReqWriteDSP    (srv_handle_t handle,int instance,int offset[],int value[],int size,int delay);
    int  service_MMI_ReqReadREG     (srv_handle_t handle,int addr[],int size);
    int  service_MMI_ReqWriteREG    (srv_handle_t handle,int addr[],int value[],int size,int delay);
	#endif /* DECT_CONFIG_MMI_SERVICE */
	
    /*
    * service for DSR
    */
    void service_DSR_FillsFiledId   (srv_handle_t handle,DSR_LA_Entry * LA_Entry);
    int  service_DSR_MsgSend        (srv_handle_t handle,const dsr_msg *dsr);
    int  service_DSR_SeqNumber      (srv_handle_t handle);
    int  service_DSR_RequestGeneric (srv_handle_t handle,int cmd,void * data,int len,int sync_timeout);
    int  service_DSR_ReqDoReg       (srv_handle_t handle,int en, int sync_timeout);
    int  service_DSR_ReqDeReg       (srv_handle_t handle,u16 HSBitMap, int sync_timeout);
    int  service_DSR_ReqPage        (srv_handle_t handle,u16 HSBitMap, int en, int end, u8*data, u32 len, int sync_timeout);
    int  service_DSR_ReqParam_Get   (srv_handle_t handle,u32 type, int  tag, u32 addr, u16 len, int sync_timeout);
    int  service_DSR_ReqParam_Set   (srv_handle_t handle,u32 type, int  tag, u32 addr, u8 *data, u16 len, int sync_timeout);
    int  service_DSR_RspParam_Get   (srv_handle_t handle,s32 eParam, int  result, u8 *data, u16 len, int sync_timeout);
    int  service_DSR_RspParam_Set   (srv_handle_t handle,s32 eParam, int  result, int sync_timeout);

    int  service_DSR_ReqLASessionStartRes       (srv_handle_t handle,u16 SessionID,u16 ListID,u16 EntryNr,LA_FieldIds*SortFields,u16 HsNr,s32 Response, LA_Res_MoreData *MoreData,int sync_timeout);
    int  service_DSR_ReqLAQuerySupEntryFieldsRes(srv_handle_t handle,u16 SessionId,LA_FieldIds*EditFields,LA_FieldIds*NonEditFields,u16 HsNr,s32 Response,int sync_timeout);
    int  service_DSR_ReqLASessionEndRes         (srv_handle_t handle,u16 SessionId, u16 HsNr , int sync_timeout);
    int  service_DSR_ReqLASearchEntriesRes      (srv_handle_t handle,u16 SessionId, u16 HsNr, u16 StartIndex,u16 Counter, int sync_timeout);
    int  service_DSR_ReqLASearchStartRes        (srv_handle_t handle,u16 SessionId, u16 HsNr,int sync_timeout);
    int  service_DSR_ReqLASearchEndRes          (srv_handle_t handle,u16 SessionId, u16 HsNr, int sync_timeout);
    int  service_DSR_ReqLASearchMatchRes        (srv_handle_t handle,u16 SessionId, u16 HsNr,u16 Counter, u16 u16_Status, int sync_timeout);
    int  service_DSR_ReqLASetHsLaTimer          (srv_handle_t handle,u16 SessionId, u16 HsNr, u16 TimeoutValue, int sync_timeout);
    int  service_DSR_ReqLAUpdateRes             (srv_handle_t handle,u16 SessionId, u16 HsNr,u16 UpdateResult,int sync_timeout);
    int  service_DSR_HsPermissionDataConvert    (srv_handle_t handle, LA_HsPermission *pst_hsPerMission, u16 u16_Index, u8 u8_Value);

    int  service_DSR_GenNotifyDT(srv_handle_t handle,u16 nHsMask,struct tm *tmlocal,int nRetryCnt);
    int  service_DSR_GenNotifyMH(srv_handle_t handle,u16 nHsMask,u8 nMusicOnHold,int nRetryCnt);
    int  service_DSR_GenNotifyNS(srv_handle_t handle,u16 nHsMask,u8 nNetStatus,int nRetryCnt);
    int  service_DSR_GenNotifyRPS(srv_handle_t handle,u16 nHsMask,u8 nRpsStatus,int nRetryCnt);
    int  service_DSR_GenNotifyRpsResult         (srv_handle_t handle,u16 nHsMask,u8 nRpsResult,int nRetryCnt);
    int  service_DSR_GenNotifyPhoneGlobalCfg    (srv_handle_t handle,u16 nHsMask,DSR_Notify_PhoneCfg *PhoneGlobalCfg,int nRetryCnt);
    int  service_DSR_GenNotifyPhoneGlobalCfg2   (srv_handle_t handle,u16 nHsMask,DSR_Notify_PhoneCfg2 *PhoneGlobalCfg2,int nRetryCnt);
    int  service_DSR_GenNotifyMultiVoiceMail    (srv_handle_t handle,u16 nHsMask,DSR_Notify_MultiVM *pMultiVM,int nRetryCnt);
    int  service_DSR_GenNotifyMultiMissedCall   (srv_handle_t handle,u16 nHsMask,DSR_Notify_MultiMC *pMultiMC,int nRetryCnt);
    int  service_DSR_GenNotifyMultiListChange   (srv_handle_t handle,u16 nHsMask,DSR_Notify_MultiLC *pMultiLC,int nRetryCnt);
    int  service_DSR_GenNotifyMultiDailToneId   (srv_handle_t handle,u16 nHsMask,DSR_Notify_MultiDailToneId *pMultiDailToneId,int nRetryCnt);
    int  service_DSR_GenNotifySU                (srv_handle_t handle,u16 nHsMask,DSR_Notify_Upgrade *pUpgrade,int nRetryCnt);
    int  service_DSR_GenNotifyBS(srv_handle_t handle,u16 nHsMask,DSR_Notify_BaseStatus *pBaseStatus,int nRetryCnt);
    int  service_DSR_GenNotifyHsDefaultCfg      (srv_handle_t handle,u16 nHsMask,DSR_Notify_HsDefaultCfg *HsDefaultCfg,int nRetryCnt);
    int  service_DSR_GenNotifyMultiSCA          (srv_handle_t handle,u16 nHsMask,DSR_Notify_MultiSCA * pMultiSCA,int nRetryCnt);
    int  service_DSR_GenNotifyLogin             (srv_handle_t handle,u16 nHsMask,DSR_Notify_Login *Login,int nRetryCnt);
    int  service_DSR_GenNotifyCallParkCfg       (srv_handle_t handle,u16 nHsMask,DSR_Notify_CallParkCfg *pCallParkCfg,int nRetryCnt);
    int  service_DSR_GenNotifyCallPark          (srv_handle_t handle,u16 nHsMask,DSR_Notify_CallPark *pCallPark,int nRetryCnt);

    int  service_DSR_ReqLASendGenericEventNotificationLC(srv_handle_t handle,DSR_Notify_ListChange *st_LA_GE_LCNotification, u16 u16_HsNr, u16  u16_RetryCnt, int sync_timeout);
    int  service_DSR_ReqLASendGenericEventNotificationMW(srv_handle_t handle,DSR_Notify_VMWIN *st_LA_GE_VMWINotification, u16 u16_HsNr, u16  u16_RetryCnt, int sync_timeout);

    int  service_DSR_GenNotifyMC(srv_handle_t handle,u16 nHsMask,u16 nLineID,u16 nNumOfCalls,u16 nListEntries,int nRetryCnt);

    int  service_DSR_ReqLAReadEntriesRes    (srv_handle_t handle,u16 SessionId, u16 StartIndex, u16 Counter, u16 HsNr, s32 Response, int sync_timeout);
    int  service_DSR_ReqLAEditEntryRes      (srv_handle_t handle,u16 SessionId, u16 HsNr, s32 Response, int sync_timeout);
    int  service_DSR_ReqLADeleteEntryRes    (srv_handle_t handle,u16 SessionId, u16 EntryNr, u16 HsNr, s32 Response, int sync_timeout);
    int  service_DSR_ReqLASaveEntryRes      (srv_handle_t handle,u16 SessionId, u16 EntryID, u16 EntryPosition, u16 EntryNr, u16 HsNr, s32 Response, int sync_timeout);
    int  service_DSR_ReqLADeleteListRes     (srv_handle_t handle,u16 SessionId, u16 HsNr, s32 Response, int sync_timeout);
    int  service_DSR_ReqLASendEntry         (srv_handle_t handle,u16 SessionId, u16 LastPacket, u16 ListID, u16 EntryId, u16 HsNr,LA_ListEntry *Entry, LA_FieldIds *FieldsSent , int sync_timeout);
    int  service_DSR_ReqLASendNack          (srv_handle_t handle,u16 SessionId, u16 Response, u16 HsNr, int sync_timeout);
    int  service_DSR_ReqLAEndSessionFromBase(srv_handle_t handle,u16 SessionID, u16 HsNr, int sync_timeout);

    int  service_Paging         (srv_handle_t handle,int hsMask,int opt,u8 *ip,u32 len,int end);
    int  service_Reging         (srv_handle_t handle,int hsMask,int opt);
    int  service_Version_GetComa(srv_handle_t handle);
    int  service_Version_GetDLib(srv_handle_t handle,char *p_ver ,int n_len);
    int  service_HS_GetStatus   (srv_handle_t handle);
    int  service_HS_GetTables   (srv_handle_t handle);
    int  service_BS_GetStatus   (srv_handle_t handle);


    /*service for DH */
    int service_DSR_SendBaseSUEvent(srv_handle_t handle,DSR_Notify_Upgrade *pst_Upgrade);
    
    int service_DSR_ReqFunctionParam_Set(srv_handle_t handle,u32 type, u32 value);

    int service_DH_MsgSend(NETSRV_H srv_handle,const dh_msg *dh);


#ifdef __cplusplus
}
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /*__DECT_API_H__*/
