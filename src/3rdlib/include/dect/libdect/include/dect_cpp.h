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
#ifndef __DECT_CPP_H__
#define __DECT_CPP_H__

#include "dect_api.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
class CDectBase
{
public:
    CDectBase()
    {

    };
    virtual ~CDectBase()
    {

    };

public:

    /*
    * create/destroy
    */
public:
    int Initialize(int log_sys=TRUE,int log_std=0,int log_time=0)
    {
        return service_Initialize(log_sys,log_std,log_time);
    };
    int Create(int poll_type,NETSRV_CB ss7_cb=NULL,NETSRV_CB dsr_cb=NULL,NETSRV_CB cfg_cb=NULL,NETSRV_CB mmi_cb=NULL,NETSRV_CB dh_cb=NULL)
    {
        m_poll_type = poll_type;

        if (ss7_cb == NULL)
            ss7_cb = (NETSRV_CB)NetSrv_CB_SS7;
        if (dsr_cb == NULL)
            dsr_cb = (NETSRV_CB)NetSrv_CB_DSR;
		#if DECT_CONFIG_MMI_SERVICE
        if (mmi_cb == NULL)
            mmi_cb = (NETSRV_CB)NetSrv_CB_MMI;
		#endif

        return service_Create(&m_handle,poll_type,ss7_cb,dsr_cb,cfg_cb,mmi_cb,this);
    };
    int Destroy(void)
    {
        return service_Destroy(m_handle);
    };

    /*
    * static APIs
    */
public:
    static void NetSrv_CB_SS7(CDectBase * this_dect,const void *msg_data,int msg_len)
    {this_dect->NetSrv_ON_SS7(msg_data,msg_len);};
    static void NetSrv_CB_DSR(CDectBase * this_dect,const void *msg_data,int msg_len)
    {this_dect->NetSrv_ON_DSR(msg_data,msg_len);};
	#if DECT_CONFIG_MMI_SERVICE
    static void NetSrv_CB_MMI(CDectBase * this_dect,const void *msg_data,int msg_len)
    {this_dect->NetSrv_ON_MMI(msg_data,msg_len);};
	#endif

public:
    void NetSrv_ON_SS7(const void *msg_data,int msg_len)
    {
        ss7_msg *ss7 = (ss7_msg *)msg_data;

#if DBG_LOG_ANY_DUP
        ss7_dump(0, ss7);
#endif

        switch (ss7->hdr.cmd)
        {
        case DECT_SS7_CMD_IAM:
            SS7_OnDECT_Iam(ss7->body.iam);
            break;
        case DECT_SS7_CMD_SAM:
            SS7_OnDECT_Sam(ss7->body.sam);
            break;
        case DECT_SS7_CMD_ACM:
            SS7_OnDECT_Acm(ss7->body.acm);
            break;
        case DECT_SS7_CMD_ANM:
            SS7_OnDECT_Anm(ss7->body.anm);
            break;
        case DECT_SS7_CMD_REL:
            SS7_OnDECT_Rel(ss7->body.rel);
            break;
        case DECT_SS7_CMD_SUS:
            SS7_OnDECT_Sus(ss7->body.sus);
            break;
        case DECT_SS7_CMD_RES:
            SS7_OnDECT_Res(ss7->body.res);
            break;
        case DECT_SS7_CMD_INF:
            SS7_OnDECT_Inf(ss7->body.inf);
            break;
        case DECT_SS7_CMD_CHST:
            SS7_OnDECT_Chst(ss7->body.chst);
            break;
        case DECT_SS7_CMD_SUS_ACK:
            SS7_OnDECT_SusAck(ss7->body.sus_ack);
            break;
        case DECT_SS7_CMD_RES_ACK:
            SS7_OnDECT_ResAck(ss7->body.res_ack);
            break;
        case DECT_SS7_CMD_RTP_EVT_RES:
            SS7_OnDECT_RtpEvtRes(ss7->body.evt_res);
            break;
        default:
            ;
            //SS7_ERR("cmd err = %d\n", ss7->hdr.cmd);
        }
    };
    void NetSrv_ON_DSR(const void *msg_data,int msg_len)
    {
        dsr_msg *msg = (dsr_msg *)msg_data;

        dsr_dump(0, msg);

        switch (msg->hdr.cmd)
        {
        case DSR_CMD_API_RESPONSE:
            {
                DSR_OnResponse(msg->body.Response);
                break;
            }

        case DSR_CMD_COMA_ALIVE_NOTIFY:
            {
                DSR_OnComa_Alive();
                break;
            }
        
        case DSR_CMD_COMA_SHUTDOWN_NOTIFY:
            {
                DSR_OnComa_Shutdown();
                break;
            }
        
        case DSR_CMD_HS_DEREG_NOTIF:
            {
                DSR_OnHS_DeRegEntry(msg->body.HSInfo);
                break;
            }

        case DSR_CMD_HS_REG_NOTIF:
            {
                DSR_OnHS_DoRegEntry(msg->body.HSInfo);
                break;
            }

        case DSR_CMD_HS_ALIVE_NOTIFY:
            {
                DSR_OnHS_Alive(msg->body.HSInfo);
                break;
            }

        case DSR_CMD_HS_SHUTDOWN_NOTIFY:
            {
                DSR_OnHS_Shutdown(msg->body.HSInfo);
                break;
            }
        
        case DSR_CMD_DEBUG_LOG:
            {
                DSR_OnDebugLog(msg->body.Generic);
                break;
            }

        case DSR_CMD_KEY_PRESS:
            {
                DSR_OnKeyPressEvent(msg->body.Generic);
                break;
            }

        case DSR_CMD_EEPROM_RESET_DONE:
            {
                DSR_OnEEPROM_ResetDone(msg->body.Generic);
                break;
            }

        case DSR_CMD_LA_SESSION_START:
            {
                DSR_OnLAStartSession(msg->body.StartSession);
                break;
            }

        case DSR_CMD_LA_SESSION_END:
            {
                DSR_OnLAEndSession(msg->body.EndSession);
                break;
            }

        case DSR_CMD_LA_QUERY_SUP_ENTRY_FIELDS:
            {
                DSR_OnLAQuerySupEntryFields(msg->body.QuerySupEntryFileds);
                break;
            }

        case DSR_CMD_LA_SEARCH_ENTRIES:
            {
                DSR_OnLASearchEntries(msg->body.SearchEntries);
                break;
            }

        case DSR_CMD_LA_SEARCH_START:
            {
                DSR_OnLASearchStart(msg->body.SearchEntries);
                break;
            }
        case DSR_CMD_LA_SEARCH_END:
            {
                DSR_OnLASearchEnd(msg->body.SearchEntries);
                break;
            }
        case DSR_CMD_LA_SEARCH_MATCH:
            {
                DSR_OnLASearchMatch(msg->body.SearchEntries);
                break;
            }

        case DSR_CMD_LA_READ_ENTRIES:
            {
                DSR_OnLAReadEntries(msg->body.ReadEntries);
                break;
            }

        case DSR_CMD_LA_EDIT_ENTRY:
            {
                DSR_OnLAEditEntry(msg->body.EditEntry);
                break;
            }

        case DSR_CMD_LA_SAVE_ENTRY:
            {
                DSR_OnLASaveEntry(msg->body.SaveEntry);
                break;
            }

        case DSR_CMD_LA_DELETE_ENTRY:
            {
                DSR_OnLADeleteEntry(msg->body.DeleteEntry);
                break;
            }

        case DSR_CMD_LA_DELETE_LIST:
            {
                DSR_OnLADeleteList(msg->body.DeleteList);
                break;
            }

        case DSR_CMD_LA_LIST_CHANGE_REQUEST:
            {
                DSR_OnLAListChange(msg->body.ChangeListRequest);
                break;
            }

        case DSR_CMD_LA_RECV_ENTRY:
            {
                DSR_FillsFiledId(msg->body.Entry);
                DSR_OnLARecvEntry(msg->body.Entry);
                break;
            }

        case DSR_CMD_LA_GEN_NOTIFY_RESULT:
            {
                DSR_OnLAGeneralEventResult(msg->body.GenericEventResult);
                break;
            }

        case DSR_CMD_LA_SESSION_END_FROM_BASE_RESPONSE:
            {
                DSR_OnLAEndSessionFromBaseResponse(msg->body.EndSession);
                break;
            }

        case DSR_CMD_HS_GENERIC_EVENT:
            {
                DSR_OnHS_GenericEvent(msg->body.HSGenericEvent);
                break;
            }
        case DSR_CMD_LA_UPDATE:
            {
                DSR_OnLAUpdate(msg->body.Update);
                break;
            }
            
        default:
            {
                //DSR_ERR("invalid msg cmd=%d\n", msg->hdr.cmd);
                break;
            }
        }
    };
	#if DECT_CONFIG_MMI_SERVICE
    void NetSrv_ON_MMI(const void *msg_data,int msg_len)
    {
        //mmi_msg *mmi = (mmi_msg *)msg_data;

#if DBG_LOG_ANY_DUMP
        mmi_dump(0, mmi);
#endif

        //MMI_ERR("callback %d do nothing\n", mmi->hdr.cmd);
    };
	#endif
    /*
    * --------------------------
    * APIs for vq report
    * --------------------------
    */
    int VQReportInit(VQREPORT_CB vq_cb=NULL)
    {
        return service_VQReportInit(m_handle, vq_cb);
    };

    /*
    * --------------------------
    * APIs for poll
    * --------------------------
    */
    int SetFD (fd_set &fdset)
    {
        return service_SetFD(m_handle, &fdset);
    };
    int PollFD(fd_set &fdset)
    {
        return service_PollFD(m_handle, &fdset);
    };

    /*
    * public APIs for enduser
    */
public:
    /*
    *----------------------------
    * APIs for syslog
    *----------------------------
    */
    int  SetLogLevel(const char*module,int log_level)
    {
        return service_SetLogLevel(module,log_level);
    };
    int  SetLogLevel(int module,int log_level)
    {
        return service_SetLogLevel2(module,log_level);
    };
    int  GetLogLevel(int module)
    {
        return service_GetLogLevel(module);
    };

    /*
    * --------------------------
    * APIs for reg&paging
    * --------------------------
    */
    int Paging(int hsMask=0xFFFF,int opt=FALSE,u8 *ip=NULL,u32 len=0,int end=TRUE)
    {
        return service_Paging(m_handle,hsMask,opt,ip,len,end);
    };
    int Reging(int hsMask=0xFFFF,int opt=FALSE)
    {
        return service_Reging(m_handle,hsMask,opt);
    };

    /*
    * --------------------------
    * APIs for version
    * --------------------------
    */
    int Version_GetComa(void)
    {
        return service_Version_GetComa(m_handle);
    };
    int Version_GetDLib(char *p_ver ,int n_len)
    {
        return service_Version_GetDLib(m_handle,p_ver,n_len);
    };

    /*
    * --------------------------
    * APIs for handset
    * --------------------------
    */
    int HS_GetStatus(void)
    {
        return service_HS_GetStatus(m_handle);
    };
    int HS_GetTables(void)
    {
        return service_HS_GetTables(m_handle);
    };

    /*
    * --------------------------
    * APIs for base
    * --------------------------
    */
    int BS_GetStatus(void)
    {
        return service_BS_GetStatus(m_handle);
    };

    /*
    * public APIs for temp
    */
public:

    /*
    *----------------------------
    * APIs for lock
    *----------------------------
    */
#if 0 /*FIXME:do we need this?*/
    virtual void Get_Data(void){};
    virtual void Put_Data(void){};

    virtual void Get_Trans(void){};
    virtual void Put_Trans(void){};
#endif


    /*
    *----------------------------
    * APIs for ss7 virtual event
    *----------------------------
    */
    virtual int SS7_OnDECT_Iam      (const ss7_msg_iam         & Iam)     {return 0;};
    virtual int SS7_OnDECT_Sam      (const ss7_msg_sam         & Sam)     {return 0;};
    virtual int SS7_OnDECT_Rel      (const ss7_msg_rel         & Rel)     {return 0;};
    virtual int SS7_OnDECT_Acm      (const ss7_msg_acm         & Acm)     {return 0;};
    virtual int SS7_OnDECT_Anm      (const ss7_msg_anm         & Anm)     {return 0;};
    virtual int SS7_OnDECT_Inf      (const ss7_msg_inf         & Info)    {return 0;};
    virtual int SS7_OnDECT_Sus      (const ss7_msg_sus         & Sus)     {return 0;};
    virtual int SS7_OnDECT_Res      (const ss7_msg_res         & Res)     {return 0;};
    virtual int SS7_OnDECT_Chst     (const ss7_msg_chst        & Chst)    {return 0;};
    virtual int SS7_OnDECT_SusAck   (const ss7_msg_sus_ack     & SusAck)  {return 0;};
    virtual int SS7_OnDECT_ResAck   (const ss7_msg_res_ack     & ResAck)  {return 0;};
    virtual int SS7_OnDECT_RtpEvtRes(const ss7_msg_rtp_evt_res & EvtRes)  {return 0;};
    
    /*
    *----------------------------
    * APIs for ss7 request
    *----------------------------
    */
    int  SS7_GetAPInstance(void)
    {return ss7_ap_instance(SS70_INSTANCE_AP_SIP);};
    int  SS7_GetAPAddress(char *addr, int type, int plan, int pres, int scrn)
    {return ss7_ap_address(addr,type,plan,pres,scrn);};

    int  SS7_ReqStart  (void)
    {
        return service_SS7_ReqStart(m_handle);
    };
    int  SS7_ReqIam    (u32 instance,u32 line, u32 hsmask,const char *dst, const char *src, 
                        const char *name,const char *multi_name[DECT_SS7_INST_NUM_MAX], const u8 *codec, u8 codec_len,u8 AlertPattern)
    {
        return service_SS7_ReqIam(m_handle,instance,line,hsmask,dst,src,name,multi_name,codec,codec_len,AlertPattern);
    };
    int  SS7_ReqAcm    (u32 instance,u8 RingType)
    {
        return service_SS7_ReqAcm(m_handle,instance,RingType);
    };
    int  SS7_ReqSam    (u32 instance, const char *num)
    {
        return service_SS7_ReqSam(m_handle,instance,num);
    };
    int  SS7_ReqRel    (u32 instance, u8 reason)
    {
        return service_SS7_ReqRel(m_handle,instance,reason);
    };
    int  SS7_ReqAnm    (u32 instance, const u8 *codec, u8 codec_len)
    {
        return service_SS7_ReqAnm(m_handle,instance,codec,codec_len);
    };
    int  SS7_ReqInf    (u32 instance, u32 infotype, const u8 *name,
                        const u8*src, u32 line, const u8 *codec, u8 codec_len, u8 call_possible)
    {
        return service_SS7_ReqInf(m_handle,instance,infotype,name,src,line,codec,codec_len,call_possible);
    };
    int  SS7_ReqSus    (u32 instance)
    {
        return service_SS7_ReqSus(m_handle,instance);
    };
    int  SS7_ReqRes    (u32 instance)
    {
        return service_SS7_ReqRes(m_handle,instance);
    };
    int  SS7_ReqChst   (u32 instance, u32 state)
    {
        return service_SS7_ReqChst(m_handle,instance,state);
    };
    int  SS7_ReqResAck (u32 instance)
    {
        return service_SS7_ReqResAck(m_handle,instance);
    };
    int  SS7_ReqSusAck (u32 instance)
    {
        return service_SS7_ReqSusAck(m_handle,instance);
    };
    int  SS7_ReqRtpEvt (u16 chan, u32 opts)
    {        
        return service_SS7_ReqRtpEvt(m_handle, chan, opts);
    };


    /*response*/
    int  SS7_RspLineCfm(u32 instance,u32 line,int bLinePossible)
    {
        return service_SS7_RspLineCfm(m_handle,instance,line,bLinePossible);
    };
    int  SS7_RspDialCfm(u32 instance,u32 line,int bDialPossible, u8*name, 
                        u32 name_len, u8* p_multi_name[DECT_SS7_INST_NUM_MAX],u32 multi_len[DECT_SS7_INST_NUM_MAX])
    {
        return service_SS7_RspDialCfm(m_handle,instance,line,bDialPossible,name,name_len, p_multi_name,multi_len);
    };
    int  SS7_ClipCnipUpdateCfm(u32 instance,u32 line,int bDialPossible, u32 infotype,
                                u8*Number, u32 Number_len,u8* p_multi_name[DECT_SS7_INST_NUM_MAX],u32 multi_len[DECT_SS7_INST_NUM_MAX])
    {
        return service_SS7_ClipCnipUpdateCfm(m_handle,instance,line,bDialPossible,
                                             infotype,Number,Number_len,p_multi_name,multi_len);
    };
    int  SS7_CallEncrypt(u32 instance,u8 CallEncrypt)
    {
        return service_SS7_CallEncrypt(m_handle,instance,CallEncrypt);
    };
    int  SS7_CallAutoAnswer (u32 instance,u8 delay_seconds /*not used now*/)
    {
        return service_SS7_CallAutoAnswer(m_handle,instance,delay_seconds);
    };
    int  SS7_PropToneInd (u32 instance,u8 tone_id)
    {
        return service_SS7_PropToneInd(m_handle,instance,tone_id);
    };

    /*data handle*/
    int  SS7_RtpEvtPkt(const ss7_msg_rtp_evt_res & EvtRes)
    {
        return service_SS7_RtpEvtPkt(&EvtRes);
    };

    int  SS7_PropClipNameInd(u32 instance,u8* p_multi_name[DECT_SS7_INST_NUM_MAX],u32 multi_len[DECT_SS7_INST_NUM_MAX])
    {
        return service_SS7_PropClipNameInd(m_handle,instance,p_multi_name,multi_len);
    };

    
    /*
    *----------------------------
    * APIs for dsr virtual event
    *----------------------------
    */
    /*event for generic*/
    virtual int DSR_OnResponse(const DSR_API_Result & param){return 0;};
    virtual int DSR_OnResponsePagingRelease(void){return 0;};
    virtual int DSR_OnComa_Alive(void){return 0;};
    virtual int DSR_OnComa_Shutdown(void){return 0;};
    
    /*event for EEPROM*/
    virtual int DSR_OnEEPROM_ResetDone(const DSR_Generic & param){return 0;};

    /*event for handset*/
    virtual int DSR_OnHS_DeRegEntry(const DSR_HS_Info & param){return 0;};
    virtual int DSR_OnHS_DoRegEntry(const DSR_HS_Info & param){return 0;};
    virtual int DSR_OnHS_GenericEvent(const DSR_HS_GenEvent & param){return 0;};
    virtual int DSR_OnHS_Alive(const DSR_HS_Info & param){return 0;};
    virtual int DSR_OnHS_Shutdown(const DSR_HS_Info & param){return 0;};

     /*event for misc*/
    virtual int DSR_OnDebugLog(const DSR_Generic & param){return 0;};
    virtual int DSR_OnKeyPressEvent(const DSR_Generic & param){return 0;};

    /*LA events*/
    virtual int DSR_OnLAStartSession(const DSR_LA_Start & param){return 0;};
    virtual int DSR_OnLAEndSession  (const DSR_LA_End   & param){return 0;};

    virtual int DSR_OnLAQuerySupEntryFields(const DSR_LA_Get_EntryFields & param){return 0;};

    virtual int DSR_OnLASearchEntries(const DSR_LA_Search & param){return 0;};

    /*Search new api*/
    virtual int DSR_OnLASearchStart(const DSR_LA_Search & param){return 0;};
    virtual int DSR_OnLASearchEnd  (const DSR_LA_Search & param){return 0;};
    virtual int DSR_OnLASearchMatch(const DSR_LA_Search & param){return 0;};

    virtual int DSR_OnLAReadEntries  (const DSR_LA_Read   & param){return 0;};
    virtual int DSR_OnLAEditEntry    (const DSR_LA_Edit     & param){return 0;};
    virtual int DSR_OnLASaveEntry    (const DSR_LA_Save     & param){return 0;};
    virtual int DSR_OnLADeleteEntry  (const DSR_LA_Delete   & param){return 0;};
    virtual int DSR_OnLARecvEntry    (const DSR_LA_Entry          & param){return 0;};

    virtual int DSR_OnLADeleteList(const DSR_LA_Empty         & param){return 0;};
    virtual int DSR_OnLAListChange(const DSR_LA_ChangeReq & param){return 0;};

    virtual int DSR_OnLAGeneralEventResult        (const DSR_LA_GenEventResult & param){return 0;};
    virtual int DSR_OnLAEndSessionFromBaseResponse(const DSR_LA_End          & param){return 0;};

    virtual int DSR_OnLAUpdate    (const DSR_LA_Update & param){return 0;};
    /*
    *----------------------------
    * APIs for dsr request
    *----------------------------
    */
    /*
    * <<<<<Generic request>>>>>
    */
    int DSR_RequestGeneric(int cmd,void * to_data =NULL,int to_len =0,int sync_timeout=0)
    {
        return service_DSR_RequestGeneric(m_handle,cmd,to_data,to_len,sync_timeout);
    };

    #if DECT_CONFIG_DATA_HANDLE
    
    virtual int DH_OnReceiveHandsetSUEvent(const DSR_Notify_Upgrade & param){return 0;};
    
    #endif /* DECT_CONFIG_DATA_HANDLE */



    /*reg/paging*/
    int DSR_ReqDoReg(int en, int sync_timeout=0)
    {
        return service_DSR_ReqDoReg(m_handle,en,sync_timeout);
    };
    int DSR_ReqDeReg(u16 HSBitMap, int sync_timeout=0)
    {
        return service_DSR_ReqDeReg(m_handle,HSBitMap,sync_timeout);
    };
    int DSR_ReqPage(u16 HSBitMap, int en, int end=TRUE, u8*data=NULL, u32 len=0, int sync_timeout=0)
    {
        return service_DSR_ReqPage(m_handle,HSBitMap,en,end,data,len,sync_timeout);
    };

    /*version*/
    inline int DSR_ReqComaVersion(int sync_timeout=0)
    {
        return DSR_RequestGeneric(DSR_CMD_GET_BASE_SOFTWARE_VERSION,NULL,0,sync_timeout);
    }

    /*status request*/
    inline int DSR_ReqStatus_Line(int sync_timeout=0)
    {
        return DSR_RequestGeneric(DSR_CMD_LINE_STATUS,NULL,0,sync_timeout);
    }

    inline int DSR_ReqStatus_HS (int sync_timeout=0)
    {
        return DSR_RequestGeneric(DSR_CMD_HS_STATUS,NULL,0,sync_timeout);
    }

    inline int DSR_ReqStatus_HSEx(int sync_timeout=0)
    {
        return DSR_RequestGeneric(DSR_CMD_HS_TABLE, NULL,0,sync_timeout);
    }
    
    inline int DSR_ReqStatus_Base(int sync_timeout=0)
    {
        return DSR_RequestGeneric(DSR_CMD_GET_BASE_OPERATION_STATUS,NULL,0,0);
    }
    #if 0
    /*EEPRM request*/
    inline int DSR_ReqEEPROM_Reset(int sync_timeout=0)
    {
        return DSR_RequestGeneric(DSR_CMD_EEPROM_RESET,NULL,0,sync_timeout);
    }

    inline int DSR_ReqEEPROM_GetList(int sync_timeout=0)
    {
        return DSR_RequestGeneric(DSR_CMD_GET_LIST_OF_EEPROM_VALUES,NULL,0,sync_timeout);
    }

    inline int DSR_ReqEEPROM_SetList (u8 *vlist, u16 len, int sync_timeout=0)
    {
        return DSR_RequestGeneric(DSR_CMD_SET_LIST_OF_EEPROM_VALUES,vlist,len,sync_timeout);
    }
    #endif

    int DSR_ReqParam_Set(u32 type, int  tag, u32 addr, u8 *data, u16 len, int sync_timeout=0)
    {
        return service_DSR_ReqParam_Set(m_handle,type,tag,addr,data,len,sync_timeout);
    };
    
    #if 0
    /*get param:EEPRM only*/
    inline int DSR_ReqParam_Update(int  tag, int sync_timeout=0)
    {
        return DSR_RequestGeneric(DSR_CMD_UPDATE_PARAM,&tag,1,sync_timeout);
    }
    int DSR_ReqParam_Get(u32 type, int  tag, u32 addr, u16 len, int sync_timeout=0)
    {
        return service_DSR_ReqParam_Get(m_handle,type,tag,addr,len,sync_timeout);
    };

    int DSR_RspParam_Get(s32 eParam, int  result, u8 *data, u16 len, int sync_timeout=0)
    {
        return service_DSR_RspParam_Get(m_handle,eParam,result,data,len,sync_timeout);
    };
    int DSR_RspParam_Set(s32 eParam, int  result, int sync_timeout=0)
    {
        return service_DSR_RspParam_Set(m_handle,eParam,result,sync_timeout);
    };
    #endif
    
#if 0
    /*handset firmware upgrade*/
    int DSR_ReqCLMS2HS   (u16 HSBitMap, u16 len, void *pvdata, int sync_timeout=0);
    int DSR_ReqIWUInfo2HS(u16 HSBitMap, u16 len, void *pvdata, int sync_timeout=0);
#endif
    #if 0
    /*line/channel request*/
    inline int DSR_ReqEPandChannelID(u8 Line, int sync_timeout)
    {
        return DSR_RequestGeneric(DSR_CMD_GET_EP_CHANNEL_ID,&Line,1,sync_timeout);
    }
    inline int DSR_ReqLineNr(int sync_timeout)
    {
        return DSR_RequestGeneric(DSR_CMD_NO_LINES,NULL,0,sync_timeout);
    }

    /*misc request*/
    inline int DSR_ReqSetBaseRegLED(int en, int sync_timeout=0)
    {
        if (en)
            return DSR_RequestGeneric(DSR_CMD_BASE_REG_LED_ON,NULL,0,sync_timeout);
        else
            return DSR_RequestGeneric(DSR_CMD_BASE_REG_LED_OFF,NULL,0,sync_timeout);
    }
    inline int DSR_ReqClearCommandStatus(int sync_timeout=0)
    {
        return DSR_RequestGeneric(DSR_CMD_CLEAR_COMMAND_STATUS,NULL,0,sync_timeout);
    }
    inline int DSR_ReqLowPowerMode(int en,int sync_timeout=0)
    {
        if(en)
            return DSR_RequestGeneric(DSR_CMD_ENTER_LOW_POWER_MODE,NULL,0,sync_timeout);
        else
            return DSR_RequestGeneric(DSR_CMD_EXIT_LOW_POWER_MODE, NULL,0,sync_timeout);
    }
    #endif
    
#if 0
    int DSR_ReqUpdateLineSetting(u8 Line, u16 bitmask, int sync_timeout=0);
    int DSR_ReqCallMode(u16 val, int sync_timeout=0);
#endif
    /*
    * <<<<<LA request>>>>>
    */
    int DSR_ReqLASessionStartRes(u16 SessionID, u16 ListID, u16 EntryNr, 
                                 LA_FieldIds &SortFields, u16 HsNr, s32 Response, LA_Res_MoreData *pst_MoreData=NULL, int sync_timeout=0)
    {
        return service_DSR_ReqLASessionStartRes(m_handle,SessionID,ListID,EntryNr,&SortFields,HsNr,Response,pst_MoreData,sync_timeout);
    };
    int DSR_ReqLAQuerySupEntryFieldsRes(u16 SessionId, LA_FieldIds &EditFields,
                                 LA_FieldIds &NonEditFields, u16 HsNr, s32 Response, int sync_timeout=0)
    {
        return service_DSR_ReqLAQuerySupEntryFieldsRes(m_handle,SessionId,&EditFields,&NonEditFields,HsNr,Response,sync_timeout);
    };
    int DSR_ReqLASessionEndRes(u16 SessionId, u16 HsNr , int sync_timeout=0)
    {
        return service_DSR_ReqLASessionEndRes(m_handle,SessionId,HsNr,sync_timeout);
    };
    int DSR_ReqLASearchEntriesRes(u16 SessionId, u16 HsNr,u16 StartIndex, u16 Counter, int sync_timeout=0)
    {
        return service_DSR_ReqLASearchEntriesRes(m_handle,SessionId,HsNr,StartIndex,Counter,sync_timeout);
    };

    int DSR_ReqLASearchStartRes(u16 SessionId, u16 HsNr,int sync_timeout=0)
    {
        return service_DSR_ReqLASearchStartRes(m_handle,SessionId,HsNr,sync_timeout);
    };
    int DSR_ReqLASearchEndRes  (u16 SessionId, u16 HsNr,int sync_timeout=0)
    {
        return service_DSR_ReqLASearchEndRes(m_handle,SessionId,HsNr,sync_timeout);
    };
    int DSR_ReqLASearchMatchRes(u16 SessionId, u16 HsNr,u16 Counter, u16 u16_Status, int sync_timeout=0)
    {
        return service_DSR_ReqLASearchMatchRes(m_handle,SessionId,HsNr,Counter,u16_Status,sync_timeout);
    };

    int DSR_ReqLAReadEntriesRes(u16 SessionId, u16 StartIndex, u16 Counter,  u16 HsNr, s32 Response, int sync_timeout=0)
    {
        return service_DSR_ReqLAReadEntriesRes(m_handle,SessionId,StartIndex,Counter,HsNr,Response,sync_timeout);
    };
    int DSR_ReqLAEditEntryRes(u16 SessionId, u16 HsNr, s32 Response, int sync_timeout=0)
    {
        return service_DSR_ReqLAEditEntryRes(m_handle,SessionId,HsNr,Response,sync_timeout);
    };
    int DSR_ReqLADeleteEntryRes(u16 SessionId, u16 EntryNr, u16 HsNr, s32 Response, int sync_timeout=0)
    {
        return service_DSR_ReqLADeleteEntryRes(m_handle,SessionId,EntryNr,HsNr,Response,sync_timeout);
    };
    int DSR_ReqLASaveEntryRes(u16 SessionId, u16 EntryID, u16 EntryPosition,
                              u16 EntryNr, u16 HsNr, s32 Response, int sync_timeout=0)
    {
        return service_DSR_ReqLASaveEntryRes(m_handle,SessionId,EntryID,EntryPosition,EntryNr,HsNr,Response,sync_timeout);
    };

    int DSR_ReqLADeleteListRes(u16 SessionId, u16 HsNr, s32 Response, int sync_timeout=0)
    {
        return service_DSR_ReqLADeleteListRes(m_handle,SessionId,HsNr,Response,sync_timeout);
    };
    int DSR_ReqLASendEntry(u16 SessionId, u16 LastPacket, u16 ListID, u16 EntryId,
                           u16 HsNr, LA_ListEntry &Entry, LA_FieldIds &FieldsSent, int sync_timeout=0)
    {
        return service_DSR_ReqLASendEntry(m_handle,SessionId,LastPacket,ListID,EntryId,HsNr,&Entry,&FieldsSent,sync_timeout);
    };
    int DSR_ReqLASendNack(u16 SessionId, u16 Response, u16 HsNr, int sync_timeout=0)
    {
        return service_DSR_ReqLASendNack(m_handle,SessionId,Response,HsNr,sync_timeout);
    };
    int DSR_ReqLAEndSessionFromBase(u16 SessionID, u16 HsNr, int sync_timeout=0)
    {
        return service_DSR_ReqLAEndSessionFromBase(m_handle,SessionID,HsNr,sync_timeout);
    };
    int DSR_ReqLASetHsLaTimer(u16 SessionId, u16 HsNr,u16 TimeoutValue, int sync_timeout=0)
    {
        return service_DSR_ReqLASetHsLaTimer(m_handle,SessionId,HsNr,TimeoutValue,sync_timeout);
    };

    int DSR_ReqLAUpdateRes(u16 SessionId, u16 HsNr,u16 UpdateResult,int sync_timeout=0)
    {
        return service_DSR_ReqLAUpdateRes(m_handle,SessionId,HsNr,UpdateResult,sync_timeout);
    };
    
    int DSR_ReqHsPMDataConvert(LA_HsPermission *pst_hsPerMission, u16 u16_Index, u8 u8_Value)
    {
        return service_DSR_HsPermissionDataConvert(m_handle, pst_hsPerMission, u16_Index, u8_Value);
    };

    /*notify*/
    int DSR_GenNotifyDT(u16 nHsMask,struct tm *tmlocal,int nRetryCnt=1)
    {
        return service_DSR_GenNotifyDT(m_handle,nHsMask,tmlocal,nRetryCnt);
    };
    int DSR_GenNotifyMC(u16 nHsMask,u16 nLineID,u16 nNumOfCalls,u16 nListEntries,int nRetryCnt=1)
    {
        return service_DSR_GenNotifyMC(m_handle,nHsMask,nLineID,nNumOfCalls,nListEntries,nRetryCnt);
    };
    int DSR_GenNotifyMH(u16 nHsMask,u8 nMusicOnHold,int nRetryCnt=1)
    {
        return service_DSR_GenNotifyMH(m_handle,nHsMask,nMusicOnHold,nRetryCnt);
    };
    int DSR_GenNotifyNS(u16 nHsMask,u8 nNetStatus,int nRetryCnt=1)
    {
        return service_DSR_GenNotifyNS(m_handle,nHsMask,nNetStatus,nRetryCnt);
    };
    int DSR_GenNotifyRPS(u16 nHsMask,u8 nRpsStatus,int nRetryCnt=1)
    {
        return service_DSR_GenNotifyRPS(m_handle,nHsMask,nRpsStatus,nRetryCnt);
    };
    int DSR_GenNotifyRpsResult(u16 nHsMask,u8 nRpsResult,int nRetryCnt=1)
    {
        return service_DSR_GenNotifyRpsResult(m_handle,nHsMask,nRpsResult,nRetryCnt);
    };
    int DSR_GenNotifyPhoneGlobalCfg(u16 nHsMask,DSR_Notify_PhoneCfg *PhoneGlobalCfg,int nRetryCnt)
    {
        return service_DSR_GenNotifyPhoneGlobalCfg(m_handle,nHsMask,PhoneGlobalCfg,nRetryCnt);
    };
    int DSR_GenNotifyPhoneGlobalCfg2(u16 nHsMask,DSR_Notify_PhoneCfg2 *PhoneGlobalCfg2,int nRetryCnt)
    {
        return service_DSR_GenNotifyPhoneGlobalCfg2(m_handle,nHsMask,PhoneGlobalCfg2,nRetryCnt);
    };
    int DSR_GenNotifyMultiVoiceMail(u16 nHsMask,DSR_Notify_MultiVM *pMultiVM,int nRetryCnt)
    {
        return service_DSR_GenNotifyMultiVoiceMail(m_handle,nHsMask,pMultiVM,nRetryCnt);
    };
    int DSR_GenNotifyMultiMissedCall(u16 nHsMask,DSR_Notify_MultiMC *pMultiMC,int nRetryCnt)
    {
        return service_DSR_GenNotifyMultiMissedCall(m_handle,nHsMask,pMultiMC,nRetryCnt);
    };
    int DSR_GenNotifyMultiListChange(u16 nHsMask,DSR_Notify_MultiLC *pMultiLC,int nRetryCnt)
    {
        return service_DSR_GenNotifyMultiListChange(m_handle,nHsMask,pMultiLC,nRetryCnt);
    };
    int DSR_GenNotifyMultiDailToneId(u16 nHsMask,DSR_Notify_MultiDailToneId *pMultiDailToneId,int nRetryCnt)
    {
        return service_DSR_GenNotifyMultiDailToneId(m_handle,nHsMask,pMultiDailToneId,nRetryCnt);
    };
    int DSR_GenNotifySU(u16 nHsMask,DSR_Notify_Upgrade *pUpgrade,int nRetryCnt)
    {
        return service_DSR_GenNotifySU(m_handle,nHsMask,pUpgrade,nRetryCnt);
    };
    int DSR_GenNotifyBS(u16 nHsMask,DSR_Notify_BaseStatus *pBaseStatus,int nRetryCnt)
    {
        return service_DSR_GenNotifyBS(m_handle,nHsMask,pBaseStatus,nRetryCnt);
    };
    int DSR_GenNotifyHsDefaultCfg(u16 nHsMask,DSR_Notify_HsDefaultCfg *HsDefaultCfg,int nRetryCnt)
    {
        return service_DSR_GenNotifyHsDefaultCfg(m_handle,nHsMask,HsDefaultCfg,nRetryCnt);
    };
    int DSR_GenNotifyMultiSCA(u16 nHsMask,DSR_Notify_MultiSCA *pMultiSCA,int nRetryCnt)
    {
        return service_DSR_GenNotifyMultiSCA(m_handle,nHsMask,pMultiSCA,nRetryCnt);
    };
    int DSR_GenNotifyLogin(u16 nHsMask,DSR_Notify_Login *pLogin,int nRetryCnt)
    {
        return service_DSR_GenNotifyLogin(m_handle,nHsMask,pLogin,nRetryCnt);
    };
    int DSR_GenNotifyCallParkCfg(u16 nHsMask,DSR_Notify_CallParkCfg *pCallParkCfg,int nRetryCnt)
    {
        return service_DSR_GenNotifyCallParkCfg(m_handle,nHsMask,pCallParkCfg,nRetryCnt);
    };
    int DSR_GenNotifyCallPark(u16 nHsMask,DSR_Notify_CallPark *pCallPark,int nRetryCnt)
    {
        return service_DSR_GenNotifyCallPark(m_handle,nHsMask,pCallPark,nRetryCnt);
    };



    int DSR_ReqLASendGenericEventNotificationLC(DSR_Notify_ListChange &st_LA_GE_LCNotification,
        u16 u16_HsNr, u16  u16_RetryCnt, int sync_timeout=0)
    {
        return service_DSR_ReqLASendGenericEventNotificationLC(m_handle,&st_LA_GE_LCNotification,u16_HsNr,u16_RetryCnt,sync_timeout);
    };
    int DSR_ReqLASendGenericEventNotificationMW(DSR_Notify_VMWIN &st_LA_GE_VMWINotification, 
        u16 u16_HsNr, u16  u16_RetryCnt, int sync_timeout=0)
    {
        return service_DSR_ReqLASendGenericEventNotificationMW(m_handle,&st_LA_GE_VMWINotification,u16_HsNr,u16_RetryCnt,sync_timeout);
    };

    void DSR_FillsFiledId(DSR_LA_Entry &LA_Entry)
    {
        return service_DSR_FillsFiledId(m_handle,&LA_Entry);
    };

    int DSR_LAStartSessionGetStartFlag(const LA_FieldIds &LAFieldIdsSortRequest)
    {
        int i;

        for (i=0; i<LAFieldIdsSortRequest.Length; i++)
        {
            if (LAFieldIdsSortRequest.pFieldId[i] == LA_ALL_LIST_FLD_PROP_START_FLAG)
                return 1;
        }
    
        return 0;
    };
    
    int DH_HS_SendBaseSUEvent(DSR_Notify_Upgrade *pst_Upgrade)
    {
        return service_DSR_SendBaseSUEvent(m_handle,pst_Upgrade);
    };

    /* Disable all external call, internal call, paging call.*/
    int DSR_Call_Disable(void)
    {
        return service_DSR_ReqFunctionParam_Set(m_handle,E_FUNCTION_PARAM_TAG_CALL_DISABLE,1);
    };

    /* Enable all external call, internal call, paging call.*/
    int DSR_Call_Enable(void)
    {
        return service_DSR_ReqFunctionParam_Set(m_handle,E_FUNCTION_PARAM_TAG_CALL_DISABLE,0);
    };

    int DSR_Trans_After_Conf_Enable(void)
    {
        return service_DSR_ReqFunctionParam_Set(m_handle,E_FUNCTION_PARAM_TAG_TRANS_OTHER_AFTER_CONF_ENABLE,1);
    };
    
    int DSR_Trans_After_Conf_Disable(void)
    {
        return service_DSR_ReqFunctionParam_Set(m_handle,E_FUNCTION_PARAM_TAG_TRANS_OTHER_AFTER_CONF_ENABLE,0);
    };

    int DSR_RtcpBye_Disable(void)
    {
        return service_DSR_ReqFunctionParam_Set(m_handle,E_FUNCTION_PARAM_TAG_RTCP_BYE_DISABLE,1);
    };

    int DSR_RtcpBye_Enable(void)
    {
        return service_DSR_ReqFunctionParam_Set(m_handle,E_FUNCTION_PARAM_TAG_RTCP_BYE_DISABLE,0);
    };

    int DSR_DectPageIntervalTimeMs_Set(u32 u32_TimeMs)
    {
        if (u32_TimeMs > 2550) /* max*/
            u32_TimeMs = 2550;
        
        u32_TimeMs = u32_TimeMs/10;
        return service_DSR_ReqFunctionParam_Set(m_handle,E_FUNCTION_PARAM_TAG_IAM_DECT_PAGE_INTERVAL_TICK,u32_TimeMs);
    };
    
    int DSR_SUOTA_Enable(D_INT bEnable)
    {
        return service_DSR_ReqFunctionParam_Set(m_handle,E_FUNCTION_PARAM_TAG_SUOTA_ENABLE,bEnable);
    };


    /*
    *----------------------------
    * APIs for mmi request
    *----------------------------
    */
    /*
    * protected APIs
    */
protected:


    /* ss7 */
    int  SS7_MsgSend(const ss7_msg *ss7)
    {
        return service_SS7_MsgSend(m_handle,ss7);
    };

    /* dsr */
    int  DSR_MsgSend(const dsr_msg *dsr)
    {
        return service_DSR_MsgSend(m_handle,dsr);
    };

#if DECT_CONFIG_MMI_SERVICE
    /* mmi */
    int  MMI_MsgSend(const mmi_msg *mmi)
    {
        return service_MMI_MsgSend(m_handle,mmi);
    };
#endif

    int  DSR_SeqNumber(void)
    {
        return service_DSR_SeqNumber(m_handle);
    };

    /*
    * protected data
    */
protected:

    /*poll type*/
    int          m_poll_type;

    /*
    * private data
    */
private:
    /* service handle */
    srv_handle_t     m_handle;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /*__DECT_CPP_H__*/
