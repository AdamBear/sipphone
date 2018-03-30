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
*    �޸�ʱ��:2015-06-12
*    �޸���:zxh
*    ԭ�汾��:1.0.0.0
*    �°汾��:1.0.0.1
*    �޸�����:1��sip_service_get_version�ӿ�ʵ�ֿ��Ի�ȡinterface��sip�汾
*             2������API��sip_call_get_info2
*
*    �°汾��:1.0.0.2
*    �޸�����:1������sip_call_reject_with_reason�ӿ�
*
*------------------------------------------------------------------
*
*******************************************************************/
#ifndef __SERVICE_SIP_INTERFACE_H__
#define __SERVICE_SIP_INTERFACE_H__

#include "service_sip_define.h"

#ifdef WIN32
#define SIP_DLL_DECLARE __declspec(dllexport)
#else
#define SIP_DLL_DECLARE __attribute ((visibility("default")))
#endif

#define SIP_SERVICE_INTERFACE_VERSION         "1.0.0.2"
/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/************************************************************************/
/*                             APIs                                     */
/************************************************************************/
/*
 *------------------------------------
 *      ϵͳ���APIs
 *------------------------------------
 */
#ifndef __SIP_API_DEFINE_xxx
/**
* @brief : get sip server versin
* @param : 
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ���صĽ��ʾ����ʽ����
*          Interface:1.0.0.1\r\n
*          sipServer:8.5.0.20\r\n
*/
SIP_DLL_DECLARE int sip_service_get_version(mkit_handle_t caller, char *buf, unsigned int size);

/**
* @brief : init sip service
* @param : 
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_service_init(mkit_handle_t caller);


/**
* @brief : exit sip service
* @param : 
* @return: 
* @remark: 
*/
SIP_DLL_DECLARE int sip_service_exit(mkit_handle_t caller);

/**
* @brief : get sip server option
* @param : 
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_service_get_option(mkit_handle_t caller, int option, void *buf, unsigned int size);



#define SIP_SERVICE_OPT_NONE            0
#define SIP_SERVICE_OPT_VIDEO_CAP       1
/* 
* �������������value:sip_network_info_t, size:sizeof(sip_network_info_t) 
* ע�⣺���������sip_service_startǰ���ã���������б�����������ã��������ú����������sip_network_change()
*/
#define SIP_SERVICE_OPT_NETWORK_INFO    2

    /*
    * ��ѡ��һ�����Զ�������RPCʱ����Ҫʹ�õ�����ΪRPC�޷�ʹ���ļ����д��䣬������Ҫͳһʹ��MSG���д���
    * ������ʱIPC�ķ�ʽ��0��Ĭ�Ϸ�ʽ��1��ǿ��ʹ��MSG��2��ǿ��ʹ���ļ�
    * SIP_SERVICE_BIG_DATA_IPC_METHOD �Ĳ���ֵȡֵ
    */
    #define SIP_BIG_DATA_IPC_DEFAULT        0/* Ĭ��ʹ�õķ�ʽ�����ݲ�ͨ�ĳ���ѡ��MSG����FILE */
    #define SIP_BIG_DATA_IPC_FORCE_IN_MSG   1
    #define SIP_BIG_DATA_IPC_FORCE_IN_FILE  2

#define SIP_SERVICE_BIG_DATA_IPC_METHOD 3

    /*���ö����õķ�ʽ
    * 1. IOS/ANDROID/WIN Softphone ����UI�ṩ��V80interface.lib���ȡ���ã��ӿڸ���M7��
    * 2. V80���Ի���Ĭ�ϲ���ETL��T49��T5x ͨ��ʶ��dbx��ȷ���Ƿ�ʹ��M7��ʽ��
    * 3.�Զ�������Ĭ��ʹ��INI��ʽ��
    */
    #define SIP_READ_CFG_USE_CFGSERVER  0
    #define SIP_READ_CFG_USE_INI                 1
    #define SIP_READ_CFG_USE_ETL                2 /*Windows����ʹ�ô˷�ʽ��ͳһ��ΪINI��ȡ��ʹ��CONFIG_ACCESS_USE_ETL_LIB����*/
#define SIP_SERVICE_READ_CFG_METHOD    4
/**
* @brief : set sip server option
* @param : option, option type
* @param : value, option value
* @param : size, size of value
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_service_set_option(mkit_handle_t caller, int option, void *value, unsigned int size);

/**
* @brief : request notify to sip server
* @param : caller, who call
* @param : event , request event mask
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
    #define SIP_REQUEST_NOTIFY_ALL            -1
    #define SIP_REQUEST_NOTIFY_REG            (1<<0)
    #define SIP_REQUEST_NOTIFY_CALL           (1<<1)
    #define SIP_REQUEST_NOTIFY_SUB            (1<<2)
    #define SIP_REQUEST_NOTIFY_MSG            (1<<3)
    #define SIP_REQUEST_NOTIFY_TELEMERTY  (1<<4)
    #define SIP_REQUEST_NOTIFY_DEFAULT       (1<<31)

SIP_DLL_DECLARE int sip_request_notify(mkit_handle_t caller, int event);

/**
* @brief : cancel notify to sip server
* @param : caller, who call
* @param : event , request event mask
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_cancel_notify(mkit_handle_t caller, int event);


/**
* @brief : start sip service
* @param : 
* @return: 
* @remark: 
*/
SIP_DLL_DECLARE int sip_service_start(mkit_handle_t caller);

/**
* @brief : stop sip service
* @param : 
* @return: 
* @remark: 
*/
SIP_DLL_DECLARE int sip_service_stop(mkit_handle_t caller);

/**
* @brief : ��ȡlync״̬
* @param : line_id           [IN][0~32]: line id���˺�ID
* @param : 
* @return: < 0 means failed
* @remark: 
*/

SIP_DLL_DECLARE int sip_get_lync_status(mkit_handle_t caller, int line_id, char *path, int size);

/*
 *------------------------------------
 *      utils tool
 *------------------------------------
 */
/**
* @brief : ��ȡsip_map_t����ǰ�����ȵ��ñ�����
* @param : dest      [IN][OUT]    Ŀ������
* @return: none
* @remark: ���¼������sip_map_t��Աptr����ȷ��ַ��ӳ�䵽map_data��,֮��Ϳ���ֱ��ʹ�����������
*/
SIP_DLL_DECLARE int sip_map_info_recover(void *dest, void *src, int method);
SIP_DLL_DECLARE void sip_map_info_dump(void *dest, int method);
/*
 *------------------------------------
 *      ע�����APIs
 *------------------------------------
 */

/**
* @brief : ��ȡָ���˺�ע��״̬
* @param : line_id      [IN][0~32]      line id���˺�ID
* @return: < 0 means failed
* @return: >= 0 means register status(sip_register_status_e)
* @remark: 
*/
SIP_DLL_DECLARE int sip_line_get_state(mkit_handle_t caller, int line_id);

/**
* @brief : device pairing��¼
* @param : line_id           [IN][0~32]: line id���˺�ID
* @param : type              [IN][0~2] : 0 ע�ᣬ1 ע�� ��2 �û�ȡ��ע��
* @param : country_code      ���Ҵ���
* @return: < 0 means failed
* @remark: 
*/
SIP_DLL_DECLARE int sip_device_pairing_signin(mkit_handle_t caller, int line_id, int type, char *country_code);

/**
* @brief : ȡ��ָ���˺ŵ�¼
* @param : line_id           [IN][0~32]: line id���˺�ID
* @param : type              [IN]����
* @return: < 0 means failed
* @remark: 
*/
SIP_DLL_DECLARE int sip_line_cancel_signin(mkit_handle_t caller, int line_id, int type);

/*
 *------------------------------------
 *      ����ͨ��API
 *------------------------------------
 */

/**
* @brief : ��ȡ�µ�call_id
* @return: <= 0 means failed
* @return: > 0 means new call_id
* @remark: GUIʹ��ͨ������ǰ������Ҫ����SIP����һ��call_id
*/
SIP_DLL_DECLARE int sip_call_new_call_id(mkit_handle_t caller);


/**
* @brief : ��ʼ���������
* @param : params       ��������ĵ�ַ
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_ext_params_init(sip_ext_params_t** params);

/**
* @brief : �������������Ӳ���
* @param : params       ��������ĵ�ַ
* @param : type         ��������
* @param : buf          �����ڴ����ʼ��ַ
* @param : type         �������ڴ泤��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ��Ϊ���������п��ܻ�����ڴ��С������params����Ϊ��sip_ext_params_t**��
*/
SIP_DLL_DECLARE int sip_ext_params_add(sip_ext_params_t** params, int type, void* buf, int buf_len);

/**
* @brief : ͨ������������ȡ����
* @param : params       ��������ĵ�ַ
* @param : type         ��������
* @param : data_len     ����ʵ�ʳ��ȣ��������Ҫ֪���������ȣ���NULL
* @return: ��NULL��Ϊȡ�������ĵ�ַ������ֱ�Ӷ�ȡ��NULL����ʾû�ж�ȡ������
* @remark: 
*/
SIP_DLL_DECLARE void* sip_ext_params_get(sip_ext_params_t* params, int type, int* data_len);

/**
* @brief : ��¡�����б�
* @param : dst          Ŀ�Ĳ�������ĵ�ַ
* @param : src          Դ�����б�
* @return: 
* @remark: ��¡�����Ĳ����б�ҲҪʹ��sip_ext_params_free�ͷ�
*/
SIP_DLL_DECLARE int sip_ext_params_clone(sip_ext_params_t** dst, sip_ext_params_t* src);

/**
* @brief : ɾ�������б�
* @param : params
* @param : 
* @return: 
* @remark: 
*/
SIP_DLL_DECLARE int sip_ext_params_del(sip_ext_params_t* params, int type);

/**
* @brief : �ͷŲ����б�
* @param : params       ��������ĵ�ַ
* @return: 
* @remark: 
*/
SIP_DLL_DECLARE int sip_ext_params_free(sip_ext_params_t** params);

/**
* @brief : ����VoIP����
* @param : line_id      [IN][0~32]      �˺�ID
* @param : call_id      [IN][1~int]     �»ỰID������ʶ���·ͨ��
* @param : call_mask    [IN][0~int]     ����ѡ��翪����Ƶ��
* @param : called       [IN][64]        ���к�����ַ��123����sip:123@10.2.1.199��
* @param : params                       �����������չ�������磺SCA index��Pickup index�ȵ�
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_invite(mkit_handle_t caller, int line_id, int call_id, int call_mask, const char* callee, sip_ext_params_t* params);


/**
* @brief : �ͷ�ͨ��
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : reason       [IN][64]        ��ѡ����ΪNULLʱ�����ڸ�֪�Է�����ͨ��ԭ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_release(mkit_handle_t caller, int line_id, int call_id, const char *reason);


/**
* @brief : ������Ӧ
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : status_code  [IN][0~int]     ������(180/183��)
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_response(mkit_handle_t caller, int line_id, int call_id, int response_code);

/**
* @brief : ���ʹ�appearance_index����Ӧ
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : status_code  [IN][0~int]     ������(180/183��)
* @param ��appearance_index [IN][0~32]  ��·ID
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_response_with_appearance_index(mkit_handle_t caller, int line_id, int call_id, int response_code, int appearance_index);

/**
* @brief : ���н���
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : call_type    [IN][0~1]       �������ͣ���Ƶ/��Ƶ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ������ͨ��SIP_ON_CALL_ANSWERED�ϱ�
*/
SIP_DLL_DECLARE int sip_call_accept(mkit_handle_t caller, int line_id, int call_id, int call_type);

/**
* @brief : ���оܾ�����
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : status_code  [IN][0~int]     sip_reason_code_e,�ܾ�������,��486/480/404
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_reject(mkit_handle_t caller, int line_id, int call_id, int status_code);

/**
* @brief : ���оܾ���������ԭ��
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : status_code  [IN][0~int]     sip_reason_code_e,�ܾ�������,��486/480/404
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_reject_with_reason(mkit_handle_t caller, int line_id, int call_id, int status_code, const char *reason);

/**
* @brief : ��ͣ��HOLDһ·ͨ����
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: HOLD���ͨ��SIP_ON_CALL_PAUSED�ϱ�
*/
SIP_DLL_DECLARE int sip_call_hold(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : �ָ������ͨ��
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ��HOLD���ͨ��SIP_ON_CALL_UPDATE�ϱ�
*/
SIP_DLL_DECLARE int sip_call_resume(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : reinvite��Ӧ
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : tid          [IN][1~int]     ����ID
* @param ��code         [IN][1~int]     ��Ӧ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
SIP_DLL_DECLARE int sip_call_reinvite_response(mkit_handle_t caller, int line_id, int call_id, int tid, int code);

/**
* @brief : ֹͣ������Ƶ����ͨ����HOLD����HELDʱ���ܵ��ô˽ӿڣ�
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
SIP_DLL_DECLARE int sip_call_stop_shareling_video(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : �ָ�������Ƶ��
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
SIP_DLL_DECLARE int sip_call_start_shareling_video(mkit_handle_t caller, int line_id, int call_id);

/*
 *------------------------------------
 *      �߼�ͨ������API
 *------------------------------------
 */


/**
* @brief : broadworks call recording operation
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : op           [IN][0~int]     start,stop,pause,resume
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ��Ҫ��call recording����������ز��ܽ�����һ������
*/
#define SIP_BW_CALL_RECORD_OP_START     0
#define SIP_BW_CALL_RECORD_OP_STOP      1
#define SIP_BW_CALL_RECORD_OP_PAUSE     2
#define SIP_BW_CALL_RECORD_OP_RESUME    3
#define SIP_BW_CALL_RECORD_OP_MAX       4

SIP_DLL_DECLARE int sip_bw_call_recording(mkit_handle_t caller, int line_id, int call_id, int op);



/**
* @brief : ͨ��ǰת��������ͨ��302ת����������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : type         [IN][0~3]       forward���ͣ���ǰ�����������SIP����������Diversionͷ��
* @param : forward_to   [IN][64]        Ŀ�������ַ
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ����type��������SIP_FORWARD_TYPE_ALWAYS_FORWARD����ôSIP�������ӵ�ͷ������Ϊ��
*          Diversion: "abc" <sip:123@1.2.3.4>;reason=unconditional
*/
#define SIP_FORWARD_TYPE_NORMAL             0   /* ��ӦreasonΪdeflection */
#define SIP_FORWARD_TYPE_ALWAYS_FORWARD     1   /* ��ӦreasonΪunconditional */
#define SIP_FORWARD_TYPE_BUSY_FORWARD       2   /* ��ӦreasonΪuser-busy */
#define SIP_FORWARD_TYPE_NO_ANSWER_FORWARD  3   /* ��ӦreasonΪno-answer */

SIP_DLL_DECLARE int sip_call_forward(mkit_handle_t caller, int line_id, int call_id, int type, const char *forward_to);


/*
* @brief : ͨ��ǰת��������ͨ��302ת����������,���Ҵ���reasonԭ����Ҫ�������ΪVCD�ڻ����У�����ʱ��ͨ��302����Է��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : forward_to   [IN][64]        Ŀ�������ַ
* @param : reason:      [IN][64]        Reasonͷ��ֵ--����Ȩ����
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ����VCDע��Apollo Primise�˺ţ��ڻ����У��Է����磬��Reasonͷ�򣬷��������Reason�ж�Ȩ���Ƿ���Ȩ����Է����
*          Reason:APOLLO;XXXXXXXX
*/

SIP_DLL_DECLARE int sip_call_forward2(mkit_handle_t caller, int line_id, int call_id, const char *forward_to, const char *reason); 

/**
* @brief : ��ͨ��park��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : type         [IN][0~3]       park����
* @param : park_to   [IN][64]           Ŀ�������ַ
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark:
*/
#define SIP_PARK_TYPE_NORMAL                0
#define SIP_PARK_TYPE_LYNC                  1

SIP_DLL_DECLARE int sip_call_park(mkit_handle_t caller, int line_id, int call_id, int type, const char *park_to);

static inline int sip_lync_call_park(mkit_handle_t caller, int line_id, int call_id)
{
    return sip_call_park(caller, line_id, call_id, SIP_PARK_TYPE_LYNC, NULL);
}

/**
* @brief : ͨ����ת��ͨ��
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ��ͨ��ID���������滻����·ͨ��
* @param : target_callid[IN][0~int]  ��ͨ��ID��Ϊ0���ʾäת
* @param : refer_to     [IN][64]        Ŀ�������ַ
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ͨ�������У���һ·ͨ��ת�������˻�
*          ת�ƽ��ͨ��SIP_ON_CALL_TRANSFERED�ϱ�
*/
SIP_DLL_DECLARE int sip_call_transfer(mkit_handle_t caller, int line_id, int call_id, int target_callid, const char *refer_to);

/**
* @brief : ˢ��ͨ����RE-INVITE��
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : type         [IN][0~int]     ���RE-INVITE
* @PARAM : arg          [IN][]          ����typeֵȷ����չ������Ĭ��ΪNULL����չ����δʹ�ã�
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_reinvite2(mkit_handle_t caller, int line_id, int call_id, int type, void *arg);

static inline int sip_call_reinvite(mkit_handle_t caller, int line_id, int call_id)
{
    return sip_call_reinvite2(caller, line_id, call_id, 0, NULL);
}


/**
* @brief : �������飬���������õ�conference��������ַ
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     �»ỰID�����ں���conf_uri
* @param : call_id_cnt  [IN][1~int]     call_id_tab ����
* @param : call_id_tab  [IN][1~int]     int���飬��ǰҪconf������·call_id�����SIP_MAX_CONF_LINE·ͨ��
* @param : conf_uri     [IN][64]        ����ָ��confĿ�������ַ��Ĭ��ΪNULL����ʾ�����û�ȡ
* @param : params                       �����������չ�������磺SCA index��Pickup index�ȵ�
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ��conf����������ͨ�����ٽ�ԭͨ��refer��conf������
*/
SIP_DLL_DECLARE int sip_call_conference(mkit_handle_t caller, int line_id, int call_id, int call_mask,
                        int call_id_cnt, int callid_tab[SIP_MAX_CONF_LINE], const char *conf_uri, sip_ext_params_t* params);

/**
* @brief : ����������³�Ա��Ĭ�Ϸ�ʽ��
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : conf_callid  [IN][0~int]     conf����ͨ��ID
* @param : target       [IN][64]        �¼���ͨ����CID
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ������Huawei ATS������
*/
SIP_DLL_DECLARE int sip_call_conference_ats_add_user(mkit_handle_t caller, int line_id, int conf_callid, const char *target);
SIP_DLL_DECLARE int sip_call_conference_ats_remove_user(mkit_handle_t caller, int line_id, int conf_callid, const char *target);

/**
* @brief : ����������³�Ա������huawei IP Centrex���������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : conf_callid  [IN][0~int]     conf����ͨ��ID
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ������Broadsoft��Huawei IP Centrex������
*/
SIP_DLL_DECLARE int sip_call_conference_add_user(mkit_handle_t caller, int line_id, int call_id, int conf_callid);


/**
* @brief : ���������ַ�����
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : conf_cid     [IN][1~int]     ����ͨ��ID
* @param : user         [IN][string]    ���鶩����Ϣ�е�user�ڵ��entity����
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ������Broadsoft������
*/
SIP_DLL_DECLARE int sip_call_conference_del_user(mkit_handle_t caller, int line_id, int conf_cid, const char* user);
/**
* @brief : BLA hold
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_draft_bla_public_hold(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : BLA resume
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_draft_bla_public_resume(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : draft blaռ��
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : appearance   [IN][64]        apearance index
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_draft_bla_seize_line(mkit_handle_t caller, int line_id, int appearance_index);

/**
* @brief : draft blaȡ��ռ��
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : appearance   [IN][64]        apearance index
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_draft_bla_unseize_line(mkit_handle_t caller, int line_id, int appearance_index);

/**
* @brief : SCA���ܵ�hold
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_sca_hold(mkit_handle_t caller, int line_id, int call_id, int is_pub, int hold);

static inline int sip_call_sca_public_hold(mkit_handle_t caller, int line_id, int call_id)
{
    return sip_call_sca_hold(caller, line_id, call_id, 1, 1);
}

static inline int sip_call_sca_public_resume(mkit_handle_t caller, int line_id, int call_id)
{
    return sip_call_sca_hold(caller, line_id, call_id, 1, 0);
}

static inline int sip_call_sca_private_hold(mkit_handle_t caller, int line_id, int call_id)
{
    return sip_call_sca_hold(caller, line_id, call_id, 0, 1);
}

static inline int sip_call_sca_private_resume(mkit_handle_t caller, int line_id, int call_id)
{
    return sip_call_sca_hold(caller, line_id, call_id, 0, 0);
}



/**
* @brief : SCA���ܵ�hold
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : appearance_index   [IN][1~int]    appearance_index
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_sca_hold_ex(mkit_handle_t caller, int line_id, int call_id, int is_pub, int hold, int appearance_index);

static inline int sip_call_sca_public_hold_ex(mkit_handle_t caller, int line_id, int call_id, int appearance_index)
{
    return sip_call_sca_hold_ex(caller, line_id, call_id, 1, 1, appearance_index);
}

static inline int sip_call_sca_public_resume_ex(mkit_handle_t caller, int line_id, int call_id, int appearance_index)
{
    return sip_call_sca_hold_ex(caller, line_id, call_id, 1, 0, appearance_index);
}


/**
* @brief : BroadWorks�������йһ��ӿ�
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: Broadsoft R22 ��������on-hookʱʹ��
*/
SIP_DLL_DECLARE int sip_call_bw_emergency_hangup(mkit_handle_t caller, int line_id, int call_id);



/**
* @brief : ͨ������������audio������ֻ֧������һ��codec��ز���
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_set_audio_param(mkit_handle_t caller, int line_id, int call_id, sip_media_t *audio);

/**
* @brief : ����ʱ����H264��Ƶ����
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_set_h264_param(mkit_handle_t caller, int line_id, int call_id, sip_h264_param_t *h264);

/**
* @brief : ����ʱ���ø���H264��Ƶ����
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_set_ext_h264_param(mkit_handle_t caller, int line_id, int call_id, sip_h264_param_t *h264);
/**
* @brief : �������Э��ǰ������Ƶ������ز������������͡������������ֱ��ʡ��Ƿ��ֹ�����ȣ�
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : params     ͬcall inviteʹ�÷�����SIP_MAKE_CALL_H264_PARAM
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_set_call_param(mkit_handle_t caller, int line_id, int call_id, sip_ext_params_t *params);
/**
* @brief : ����ʱ����H264��Ƶ����
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_set_video_bandwidth(mkit_handle_t caller, int line_id, int call_id, int band_width);

/**
* @brief : ����ʱ�������߹ر���Ƶ
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_set_video_media(mkit_handle_t caller, int line_id, int call_id, int enable);

/**
* @brief : ����ʱ�������߹رո���
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_set_bfcp_media(mkit_handle_t caller, int line_id, int call_id, int enable);
/**
* @brief : ����Զ������ͷ
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_camera_action_start(mkit_handle_t caller, int line_id, int call_id, sip_camera_action_e action);

/**
* @brief : ֹͣ����Զ������ͷ
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_camera_action_stop(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : ����Զ������ͷ����
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_camera_preset_store(mkit_handle_t caller, int line_id, int call_id, int preset_num);

/**
* @brief : ����Զ������ͷ����
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_camera_preset_active(mkit_handle_t caller, int line_id, int call_id, int preset_num);

/**
* @brief : ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_extern_video_start(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : �رո���
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_extern_video_stop(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : ����DTMF,����INFO��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @param : dtmf_code    [IN][0~int]     ��Ҫ���͵�dtmfֵ
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: �ú��������ڷ�DTMF������Ҫ��INFO����ʹ��sip_send_info
*/
SIP_DLL_DECLARE int sip_call_send_dtmf(mkit_handle_t caller, int line_id, int call_id, int dtmf_code);

/**
* @brief : Broadsoft Flash,����INFO��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : uuid         [IN][0~int]     Ψһ��ʾ������������ǰ����Ϣ�Ƿ�ͬһ������
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: JOYCE����֧��,�Ĳ�ɷ���INFO��Ϣ
*/
SIP_DLL_DECLARE int sip_call_send_info_broadsoft_flash(mkit_handle_t caller, int line_id, int call_id, int uuid);

/**
* @brief : CMC,����INFO��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : uuid         [IN][0~int]     Ψһ��ʾ������������ǰ����Ϣ�Ƿ�ͬһ������
* @param : cmc_code     [IN][string]    ָ�����͵�CMC code����
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: �ͻ�Ҫ�󻰻���ʹ���������ж��Ƕ��������ʱ����԰�һ��������һ����Ϣ����������
*          �÷�������¼�������ù�����ͨ����ͨ���з���INFO��Ϣ��ʵ�ֵ�,����������
*          X-ClientCodeͷ���ֵ(cmc_code)��������м�¼
*/
SIP_DLL_DECLARE int sip_call_send_info_cmc(mkit_handle_t caller, int line_id, int call_id, int uuid, char *cmc_code);

/**
* @brief : ͨ��¼������,����INFO��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : uuid         [IN][0~int]     Ψһ��ʾ������������ǰ����Ϣ�Ƿ�ͬһ������
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ʵ����ͨ����֪ͨ��������ʼ¼����ֹͣ¼���Ĺ���
*/
SIP_DLL_DECLARE int sip_call_send_info_record_on(mkit_handle_t caller, int line_id, int call_id, int uuid);

SIP_DLL_DECLARE int sip_call_send_info_record_off(mkit_handle_t caller, int line_id, int call_id, int uuid);

/**
* @brief : ͨ��¼������,����fortinet ����INFO��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : uuid         [IN][0~int]     Ψһ��ʾ������������ǰ����Ϣ�Ƿ�ͬһ������
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ʵ����ͨ����֪ͨ��������ʼ¼����ֹͣ¼���Ĺ���
*/
SIP_DLL_DECLARE int sip_call_send_info_fortinet_record_on(mkit_handle_t caller, int line_id, int call_id, int uuid);

SIP_DLL_DECLARE int sip_call_send_info_fortinet_record_off(mkit_handle_t caller, int line_id, int call_id, int uuid);
/**
* @brief : I֡ͬ��,����INFO��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : uuid         [IN][0~int]     Ψһ��ʾ������������ǰ����Ϣ�Ƿ�ͬһ������
* @param : body         [IN][string]    ָ������I֡ͬ��������
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_send_info_frame_sync(mkit_handle_t caller, int line_id, int call_id, int uuid, char *body);

/**
* @brief : ͨ���з���Broadworks Disposition Code,����INFO��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : uuid         [IN][0~int]     Ψһ��ʾ������������ǰ����Ϣ�Ƿ�ͬһ������
* @param : bw_service   [IN][string]    "X-BroadWorks-Service-Control"ͷ�������͵�����
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ����ͷ�����ƣ�X-BroadWorks-Service-Control: sip:cc-disposition-code@as.yealink.com;code=123
*/
SIP_DLL_DECLARE int sip_call_send_info_bw_service(mkit_handle_t caller, int line_id, int call_id, int uuid, char *bw_service);

/**
* @brief : ͨ���з���RTCP-XRͳ����Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : uuid         [IN][0~int]     Ψһ��ʾ������������ǰ����Ϣ�Ƿ�ͬһ������
* @param : vqreport_data[IN][]          ��Ҫ�ϱ����������ݣ��ο�ipvp_vq_report_t
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_call_send_rtcp_xr(mkit_handle_t caller, int line_id, int call_id, int uuid, const char *vqreport_data);

/**
* @brief : ��ȡָ��ͨ����SIP��Ϣ
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : sip_call_proto_info_t[IN][]  ��Ҫ��ȡ����Ϣ
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_call_get_info(mkit_handle_t caller, int line_id, int call_id, sip_call_proto_info_t *info);

/**
* @brief : ��ȡָ��ͨ����SIP��Ϣ����չ�ӿڣ�
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : sip_call_proto_info2_t[IN][] ��Ҫ��ȡ����Ϣ
* @param : size         [IN][0~65535]   sip_call_proto_info2_t��С
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_call_get_info2(mkit_handle_t caller, int line_id, int call_id, sip_call_proto_info2_t *info, unsigned int size);

SIP_DLL_DECLARE int sip_call_get_transport_info(mkit_handle_t caller, int line_id, int call_id, sip_call_transport_info_t *info, unsigned int size);

SIP_DLL_DECLARE int sip_call_get_remote_user_agent(mkit_handle_t caller, int line_id, int call_id, char *buf, int buf_len);

/**
* @brief : ����uaCSTA INFO��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : uuid         [IN][0~int]     Ψһ��ʾ������������ǰ����Ϣ�Ƿ�ͬһ������
* @param : body         [IN][string]    ָ�����͵�uaCSTA���������
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_uacsta_send_request(mkit_handle_t caller, int line_id, int call_id, int uuid, const char *body);


/**
* @brief : ����uaCSTA��Ӧ��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ӧ
* @param : tid          [IN][0~int]     ����ID��������Ӧ��Ϣ�ı�Ҫ����
* @param : body         [IN][string]    ָ�����͵�uaCSTA��Ӧ������
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_uacsta_send_response(mkit_handle_t caller, int line_id, int call_id, int tid, const char *body);


/**
* @brief : ���� kpml notify
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : did          [IN][0~int]     dialog id������notify�ı�Ҫ����
* @param : code         [IN][0~int]     kpml status code(200: dtmf code, 423: ���볬ʱ, 487: ע�����ģ��ر������)
* @param : digits       [IN][0~16]      dtmf digits
* @param : tag          [IN][string]    kpml tag
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_kpml_send_notify(mkit_handle_t caller, int line_id, int did, int code, int digits, const char *tag);


/**
* @brief : ͨ���з���QoE��������������Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ���ʾ���ͻỰ����Ϣ
* @param : uuid         [IN][0~int]     Ψһ��ʾ������������ǰ����Ϣ�Ƿ�ͬһ������
* @param : lync_vqreport_data[IN][]         
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_call_send_qoe_report(mkit_handle_t caller, int line_id, int call_id,  const char *vqreport_data);


/**
* @brief : lync conference��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : mask         [IN][0~int]     ����ѡ�����Ƶ�����
* @param : user                         ��������ʱ��Ҫ��ӵĻ����Ա
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_create_conference(mkit_handle_t caller, int line_id, int call_id, int mask, char *user);

/**
* @brief : lync conference �������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     �ỰID
* @param : conv_id      ����ID
* @param : focus_uri                    ��������URI
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_join_conference(mkit_handle_t caller, int line_id, int call_id, char *conv_id, char *focus_uri);

/**
* @brief : lync conference��ӻ����Ա
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : user �����Ա��sip uri
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_add_user(mkit_handle_t caller, int line_id, int call_id, char *user);

/**
* @brief : lync conferenceɾ��ָ�������Ա
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : user �����Ա��sip uri
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_delete_user(mkit_handle_t caller, int line_id, int call_id, char *user);

/**
* @brief : lync conference muteָ�������Ա
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : user �����Ա��sip uri
* @param : uuid �����Ա��uuid
* @param : 1:mute 0:umute
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_mute_user(mkit_handle_t caller, int line_id, int call_id, char *user, char *uuid, char *media_id, int mute);

/**
* @brief : lync conference mute���Լ���������������Ա
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : 1:mute 0:umute
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_mute_audience(mkit_handle_t caller, int line_id, int call_id, int mute);

/**
* @brief : lync conference ���鹫��
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : 1:enable 0:disable
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_announcements(mkit_handle_t caller, int line_id, int call_id, int enable);

/**
* @brief : lync conference ����������ס���޷����������Ա
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : 1:lock 0:unlock
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_lock(mkit_handle_t caller, int line_id, int call_id, int enable);

/**
* @brief : lync conference �����ܾ���������Ĳ��뷽�������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : user_access_info ���������ܾ����뷽�Ļ����Ա��Ϣ����ʽΪXML
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_setlobby_access(mkit_handle_t caller, int line_id, int call_id, char *user_access_info);

/**
* @brief : lync conference �޸Ĳ����ߵ�Ȩ�ޣ���ʾ�߻��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : role 1:��ʾ�� 0:�����
* @param : user ���޸ĵĲ����ߵ�uriΪ sip:xxx@yealinkuc.com
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_modify_user_role(mkit_handle_t caller, int line_id, int call_id, int role, char *user);

/**
* @brief : lync conferenceͨ���ϲ��ɻ���
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : mask         [IN][0~int]     ����ѡ�����Ƶ�����
* @param : merger_cid [IN][0~int]    ���ϲ�ͨ���ĻỰID
* @param : user                               ���ϲ�ͨ�������Ա��uri
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_merge_user(mkit_handle_t caller, int line_id, int call_id, int mask, int merger_cid,char *user);

/**
* @brief : ����e911 location
* @param : line_id            [IN][0~32]      line id���˺�ID
* @param : location_info      [IN][string]    e911 location info
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_update_e911_location_info(mkit_handle_t caller, int line_id, const char *location_info);

/**
* @brief : ����״̬
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : state        [IN][]          sip_lync_presence_state_e
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_publish_presence_state(mkit_handle_t caller, int line_id, int state);

/**
* @brief : ���¸���ǩ��
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : note         [char*][strlen: 0~512]        
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_publish_presence_note(mkit_handle_t caller, int line_id, const char *note);

/**
* @brief : ֪ͨSIP����routing��Ϣ
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : type         [IN][]          sip_lync_routing_type_e
* @param : routing      [IN][string]    routing info
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_publish_routing(mkit_handle_t caller, int line_id, int type, const char *routing);

/**
* @brief : ֪ͨSIP����user infomation
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : type         [IN][]          sip_lync_userinfomation_type_e
* @param : string       [IN][string]    user infomation
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_publish_user_infomation(mkit_handle_t caller, int line_id, int type, const char *string);

/**
* @brief : ֪ͨSIP�޸���˽����
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : type         [IN][]          SIP_LYNC_PRES_PRIVACY_PRIVATE/SIP_LYNC_PRES_PRIVACY_STANDARD
* @param : NULL
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_lync_publish_privacy_mode(mkit_handle_t caller, int line_id, int type);



/**
* @brief : ֪ͨSIP���ö�����ϵ����Ϣ
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : uuid         [IN][int]       UI ���ڱ�Ƕ���Ŀ�����͵Ĳ�����SIP ���ڶ��Ľ���д��ظ�ֵ
* @param : to           [IN][string]    �״ζ���ΪNULL��resubʱ�����poolfqdn������Ϊpoolfqdn
* @param : body         [IN][string]    ������Ϣ��BODY
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_subscribe_contacts_presence_presistent(mkit_handle_t caller, int line_id, int uuid, const char *to, const char *body);

/**
* @brief : ֪ͨSIPһ���Զ�����ϵ����Ϣ
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : uuid         [IN][int]       UI ���ڱ�Ƕ���Ŀ�����͵Ĳ�����SIP ���ڶ��Ľ���д��ظ�ֵ
* @param : to           [IN][string]    �״ζ���ΪNULL��resubʱ�����poolfqdn������Ϊpoolfqdn
* @param : body         [IN][string]    ������Ϣ��BODY
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_subscribe_contacts_presence_polling(mkit_handle_t caller, int line_id, int uuid, const char *to, const char *body);

/**
* @brief : ֪ͨSIP���ö��ĵ�����ϵ����Ϣ����resubʱʹ�ã�
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : uuid         [IN][int]       UI ���ڱ�Ƕ���Ŀ�����͵Ĳ�����SIP ���ڶ��Ľ���д��ظ�ֵ
* @param : body         [IN][string]    �״ζ��Ľ����û��poolfqdn���Ե���ϵ�˰�ָ����ʽ������ַ���
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_subscribe_contacts_presence_list_presistent(mkit_handle_t caller, int line_id, int uuid, const char *contact_list);

/**
* @brief : ֪ͨSIPһ���Զ�����ϵ����Ϣ����resubʱʹ�ã�
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : uuid         [IN][int]       UI ���ڱ�Ƕ���Ŀ�����͵Ĳ�����SIP ���ڶ��Ľ���д��ظ�ֵ
* @param : body         [IN][string]    �״ζ��Ľ����û��poolfqdn���Ե���ϵ�˰�ָ����ʽ������ַ���
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_subscribe_contacts_presence_list_polling(mkit_handle_t caller, int line_id, int uuid, const char *contact_list);


/**
* @brief : ֪ͨSIP������ϵ����Ϣ
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : uuid         [IN][int]       UI ���ڱ�Ƕ���Ŀ�����͵Ĳ�����SIP ���ڶ��Ľ���д��ظ�ֵ
* @param : expire_type  [IN][enum]      ���ö��Ļ�һ���Զ��ģ�Ӱ����Ϣ�� Expires ͷ��ֵ��
*                                       sip_lync_contact_pres_sub_expries_type_e
* @param : body         [IN][string]    ������Ϣ��BODY
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
static inline int sip_lync_subscribe_contacts_presence(mkit_handle_t caller, int line_id, int uuid,
                                                sip_lync_contact_pres_sub_expries_type_e expire_type, const char *body)
{
    if (expire_type == SIP_LYNC_CONTACT_PRES_SUB_POLLING)
        return sip_lync_subscribe_contacts_presence_polling(caller, line_id, uuid, NULL, body);
    else
        return sip_lync_subscribe_contacts_presence_presistent(caller, line_id, uuid, NULL, body);
}

/**
* @brief : UI �ڴ�����ϵ��״̬���Ľ��ʱʹ�ã�����Ҫ resub �Ľ��֪ͨ SIP ���� resub
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : uuid         [IN][int]       UI ���ڱ�Ƕ���Ŀ�����͵Ĳ�����SIP ���ڶ��Ľ���д��ظ�ֵ
* @param : expire_type  [IN][enum]      ���ö��Ļ�һ���Զ��ģ�Ӱ����Ϣ�� Expires ͷ��ֵ��
*                                       sip_lync_contact_pres_sub_expries_type_e
* @param : poolfqdn     [IN][string]    �ǿ�ʱΪ����Ŀ�꣬NULL��Ŀ���body�����ж�ȡ��Ӱ����Ϣ�� To ͷ��ֵ
* @param : body         [IN][string]    ������Ϣ��BODY
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
static inline int sip_lync_subscribe_resub_contacts_presence(mkit_handle_t caller, int line_id, int uuid,
                                               sip_lync_contact_pres_sub_expries_type_e expire_type,
                                               const char *poolfqdn, const char *body)
{
    if (poolfqdn && poolfqdn[0] != '\0')
    {
        if (expire_type == SIP_LYNC_CONTACT_PRES_SUB_POLLING)
            return sip_lync_subscribe_contacts_presence_polling(caller, line_id, uuid, poolfqdn, body);
        else
            return sip_lync_subscribe_contacts_presence_presistent(caller, line_id, uuid, poolfqdn, body);
    }
    else
    {
        if (expire_type == SIP_LYNC_CONTACT_PRES_SUB_POLLING)
            return sip_lync_subscribe_contacts_presence_list_polling(caller, line_id, uuid, body);
        else
            return sip_lync_subscribe_contacts_presence_list_presistent(caller, line_id, uuid, body);
    }
}

/**
* @brief : UI֪ͨSIP�Զ�������ִ�н��(��SIP�޹صĲ���)
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : result       [IN][int]       ����ִ�н��, 0:ִ��ʧ�ܣ�1:ִ�гɹ�
* @param : code         [IN][string]    ��ѡ������ִ�н��������Ϣ
* @param : reason       [IN][string]    ��ѡ������ִ�н��������Ϣ
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_ui_report_automation_operate_result(mkit_handle_t caller, int line_id, int result, int code, const char *reason);

/**
* @brief : BTOE������¼, ʹ��Lync�ͻ������������¼
* @param : 
* @return: 
* @remark: 
*/
SIP_DLL_DECLARE int sip_lync_btoe_help_login(mkit_handle_t caller, int line_id);

/**
* @brief : BTOE������ȡ��������¼, �ر�Lync�ͻ��˵�¼����
* @param : 
* @return: 
* @remark: 
*/
SIP_DLL_DECLARE int sip_lync_btoe_cancel_help_login(mkit_handle_t caller, int line_id);


/*
 *------------------------------------
 *       apollo����ӿ�
 *------------------------------------
*/
/**
* @brief : apollo conference��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID
* @param : mask         [IN][0~int]     ����ѡ�����Ƶ����ȣ������ڻ�δʹ��
* @param : user                         �ȶ��壬����ʹ�ã�Ŀǰ��NULL
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_create_conference(mkit_handle_t caller, int line_id, int call_id, int mask, const char *user);

/**
* @brief : apollo conference��ӻ����Ա
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : call_way     [IN][0~int]     ������鷽ʽ,0:Dila-in, 1:Dail-out
* @param : user �����Ա��sip uri,Ŀǰ��Ҫui������sip��������pstn
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_conference_add_user(mkit_handle_t caller, int line_id, int call_id, int call_way, const char *user);

/**
* @brief : apollo conferenceɾ�������Ա
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : user �����Ա��sip uri,Ŀǰ��Ҫui������sip��������pstn
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_conference_delete_user(mkit_handle_t caller, int line_id, int call_id, const char *user);

/**
* @brief : apollo conference�������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : conv_id ����id
* @param : focus_uri �������ͨ��focus uri
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_join_conference(mkit_handle_t caller, int line_id, int call_id, const char *conv_id, const char *focus_uri);

/**
* @brief : apollo conference muteָ�������Ա���Լ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : user �����Ա��sip uri
* @param : uuid �����Ա��uuid
* @param : media_id: ����media��id
* @param : mute:mute���� 0��Unmute/���� 1��mute
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_conference_mute_user(mkit_handle_t caller, int line_id, int call_id, const char *user, const char *uuid, int media_id, int mute);

/**
* @brief : apollo conference service������Ϣ��ȡ
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : body  �������Ϣ
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_conference_service(mkit_handle_t caller, int line_id, int call_id, const char *body);

/**
* @brief : apollo conference finish��ϯ����ģʽ����ϯ�������飬ȫ���Ա�˳�����
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_conference_finish(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : apollo conference modify user role�޸��������ݣ�ֻ����ʾ���д�Ȩ��
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : role         ��ɫ��1������� 0����ʾ��
* @param : user         ���޸���URI
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_conference_modify_user_role(mkit_handle_t caller, int line_id, int call_id, int role, const char *user);
/*
 *------------------------------------
 *      �������APIs
 *------------------------------------
 */

/**
* @brief : BLF��ѯ
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : key_index    [IN][-1~...]  ��ѯkey��������-1��ʾ����key
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: ���øýӿڣ��ᴥ��sip�ϱ�dsskey��BLF״̬��-1��ʾ����key�����key_index����ʵ�ʵ����ֵ��
*           �������ϱ�
*/
SIP_DLL_DECLARE int sip_blf_get_key_status(mkit_handle_t caller, int line_id, int key_index);

/**
* @brief : ��ȡBLF-LIST�յ���NOTIFY����
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : buf          [IN]
* @param : size         [IN]
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: buf��size��ʱ����ʹ�ã�������ӿں󣬻ᴥ��blf list����Ϣ�ϱ�
*/
SIP_DLL_DECLARE int sip_blflist_get_body(mkit_handle_t caller, int line_id, char *buf, int size);

/**
* @brief : call completion����/ȡ������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : receiver     [IN][64]        Ŀ�������ַ
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_completion_subscribe(mkit_handle_t caller, int line_id, const char *receiver);
SIP_DLL_DECLARE int sip_call_completion_unsubscribe(mkit_handle_t caller, int line_id, const char *receiver);

/**
* @brief : SCAռ��
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : appearance   [IN][64]        SCA�����Call-infoͷ���������
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_sca_seize_line(mkit_handle_t caller, int line_id, int appearance_index);

/**
* @brief : SCAȡ��ռ��
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : appearance   [IN][64]        SCA�����Call-infoͷ���������
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_sca_unseize_line(mkit_handle_t caller, int line_id, int appearance_index);

/**
* @brief : SCA��·��ѯ��SCA������ʵʱ�ϱ���·״̬�����UIʲôʱ�򻺴涪ʧ����UI����������Ҫ���øýӿ�
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : appearance   ��ѯ����·״̬
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_sca_line_query(mkit_handle_t caller, int line_id);


/**
* @brief : ACD����������NOTIFY���ͣ�
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : action       [IN][0~3]       �������ͣ�sip_acd_action_code_e
* @param : number       [IN][0~int]     �󶨵�dsskeyֵ
* @param : user         [IN][64]        ��¼�û���
* @param : pass_word    [IN][64]        ��¼�û�������
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
#define SIP_ACD_ACTION_LOGIN            0/* action */
#define SIP_ACD_ACTION_AVAIL            1
#define SIP_ACD_ACTION_UNAVAIL          2
#define SIP_ACD_ACTION_LOGOUT           3
#define SIP_ACD_ACTION_DETACH           4
SIP_DLL_DECLARE int sip_acd_control(mkit_handle_t caller, int line_id, int action, int number, const char *user, const char *pass_word);

/**
* @brief : Broadsoft call center��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_broadsoft_call_center_subscribe(mkit_handle_t caller, int line_id);
SIP_DLL_DECLARE int sip_broadsoft_call_center_unsubscribe(mkit_handle_t caller, int line_id);

/**
* @brief : Hoteling��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : user         [IN][64]        ��¼�û���
* @param : pass_word    [IN][64]        ��¼�û�������
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_hoteling_login(mkit_handle_t caller, int line_id, const char *user, const char *pass_word);
SIP_DLL_DECLARE int sip_hoteling_logout(mkit_handle_t caller, int line_id);


/*
 *  ����ģ�����API
 */

/**
* @brief : KTS ����KINFO����
* @param : line_id      [IN][0~32]       line id���˺�ID
* @param : kts_cmd      [CHAR *][String] Kts����
* @param ��len          [IN][1~int]      �����
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
SIP_DLL_DECLARE int sip_send_kts_info(mkit_handle_t caller, int line_id, char *kts_cmd, int len);


/**
* @brief : Genband E911 UI��ȡ��λ�ú�֪ͨSIPʹ��location id����ע��ˢ�µȲ���
* @param ��line_id      [IN][1~int]      line id������Genband E911���˺�id��Ԥ���ӿڲ���
* @param ��location_id  [IN][1~int]      location id
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
SIP_DLL_DECLARE int sip_e911_set_location(mkit_handle_t caller, int line_id, int location_id);


/**
* @brief : ����SMS�ı���Ϣ
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : uuid         [IN][0~int]     Ψһ��ʾ������������ǰ����Ϣ�Ƿ�ͬһ������
* @param : receiver     [IN][64]        �����ߺ�����ַ
* @param : text         [IN][512]       ��Ҫ���͵��ı�����
* @PARAM : ext_header   [IN][string]    ��չSIP�����Զ���ͷ�����ݣ�����Ϊ�ṹ��������ʽ�����ö��ͷ��,ΪNULL�򲻴���
* @PARAM : ext_header_size [IN][string] ��Ҫ��չ�Զ���ͷ��ĸ���
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_send_sms2(mkit_handle_t caller, int line_id, int uuid, const char *receiver, const char *text, sip_ext_params_t* params);

static inline int sip_send_sms(mkit_handle_t caller, int line_id, int uuid, const char *receiver, const char *text)
{
    return sip_send_sms2(caller, line_id, uuid, receiver, text, NULL);
}

/**
* @brief : ����INFO��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : uuid         [IN][0~int]     Ψһ��ʾ������������ǰ����Ϣ�Ƿ�ͬһ������
* @param : info_type    [IN][0~int]     INFO���ͣ����������֣���ͨINFO��DTMF��INFO����SIP_INFO_SEND_TYPE_NORMAL/SIP_INFO_SEND_TYPE_DTMF��
* @param : content_type [IN][string]    ����SIP����Content-Type�����ݣ�ΪNULL�򲻴���
* @PARAM : body         [IN][string]    ����SIP����BodyЯ�������ݣ�ΪNULL�򲻴���
* @PARAM : ext_header   [IN][string]    ��չSIP�����Զ���ͷ�����ݣ�����Ϊ�ṹ��������ʽ�����ö��ͷ��,ΪNULL�򲻴���
* @PARAM : params       [sip_ext_params_t*]    ������չ����
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/

#define SIP_INFO_SEND_TYPE_NORMAL   0
#define SIP_INFO_SEND_TYPE_DTMF     1
SIP_DLL_DECLARE int sip_send_info(mkit_handle_t caller, int line_id, int call_id, int uuid, int info_type, 
                      const char* content_type,
                      const char* body,
                      const char* headers,
                      sip_ext_params_t* params);

/**
* @brief : �����Զ���INFO��Ϣ����Ӧ�յ�����ϢΪSIP_MSG_RECV_CUSTOM_INFO
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : uuid         [IN][0~int]     Ψһ��ʾ������������ǰ����Ϣ�Ƿ�ͬһ������
* @PARAM : body         [IN][string]    ����SIP����BodyЯ�������ݣ�ΪNULL�򲻴���
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
static inline int sip_send_custom_info(mkit_handle_t caller, int line_id, int call_id, int uuid, const char *body)
{
    return sip_send_info(caller, line_id, call_id, uuid, SIP_INFO_SEND_TYPE_NORMAL, "application/custom+xml", body, NULL, NULL);
}

/**
* @brief : ����NOTIFY��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : uuid         [IN][0~int]     Ψһ��ʾ������������ǰ����Ϣ�Ƿ�ͬһ������
* @param : content_type [IN][string]    ����SIP����Content-Type�����ݣ�ΪNULL�򲻴���
* @PARAM : body         [IN][string]    ����SIP����BodyЯ�������ݣ�ΪNULL�򲻴���
* @PARAM : ext_header   [IN][string]    ��չSIP�����Զ���ͷ�����ݣ�����Ϊ�ṹ��������ʽ�����ö��ͷ��,ΪNULL�򲻴���
* @PARAM : ext_header_size [IN][string] ��Ҫ��չ�Զ���ͷ��ĸ���
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_send_notify(mkit_handle_t caller, int line_id, int call_id, int uuid, 
                    char *content_type, char *body, const char* headers, sip_ext_params_t* params);

/**
* @brief : ����PUBLISH��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : uuid         [IN][0~int]     Ψһ��ʾ������������ǰ����Ϣ�Ƿ�ͬһ������
* @param : content_type [IN][string]    ����SIP����Content-Type�����ݣ�ΪNULL�򲻴���
* @PARAM : body         [IN][string]    ����SIP����BodyЯ�������ݣ�ΪNULL�򲻴���
* @PARAM : ext_header   [IN][string]    ��չSIP�����Զ���ͷ�����ݣ�����Ϊ�ṹ��������ʽ�����ö��ͷ��,ΪNULL�򲻴���
* @PARAM : ext_header_size [IN][string] ��Ҫ��չ�Զ���ͷ��ĸ���
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_send_publish(mkit_handle_t caller, int line_id, int call_id, int uuid, 
                     const char *content_type, const char *body, const char* headers, sip_ext_params_t* params);

/**
* @brief : Tiptel PBX��������������,����NOTIFY��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ0���ʾ���ͻỰ����Ϣ
* @param : uuid         [IN][0~int]     Ψһ��ʾ������������ǰ����Ϣ�Ƿ�ͬһ������
* @param : body         [IN][string]    ָ������body����
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_send_notify_tiptel_pbx_button(mkit_handle_t caller, int line_id, int call_id, int uuid, char *body);


/**
* @brief : as feature key���ܼ�ͬ����һ������broadsoft acd��
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : xml_body     [IN][512]       subscribe����Я����xml����
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_send_feature_key(mkit_handle_t caller, int line_id, const char *xml_body);


/**
* @brief : ����uc info��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ���ʾ���ͻỰ����Ϣ
* @param : buf          [IN][char *]    xml body
* @param : size         [IN][0~65535]   body��С
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_uc_send_request(mkit_handle_t caller, int line_id, int call_id, sip_uc_req_send_info_t *info, int size);

/**
* @brief : ����uc��info��Ӧ��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ���ʾ���ͻỰ����Ӧ
* @param : tid          [IN][0~int]     ����ID��������Ӧ��Ϣ�ı�Ҫ����
* @param : code         [IN][0~int]     ָ�����͵�Ӧ����
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_uc_send_response(mkit_handle_t caller, int line_id, int call_id, int tid, int code);

/**
* @brief : ����uc��notify��Ӧ��Ϣ��������
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][0~int]     ԭ�ỰID��Ϊ���ʾ���ͻỰ����Ӧ
* @param : tid          [IN][0~int]     ����ID��������Ӧ��Ϣ�ı�Ҫ����
* @param : code         [IN][0~int]     ָ�����͵�Ӧ����
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_uc_send_notify_response(mkit_handle_t caller, int line_id, int call_id, int tid, int code);

/**
* @brief : srtp lifetime reinvite
* @param : line_id      [IN][0~32]      line id���˺�ID
* @param : call_id      [IN][1~int]     ԭ�ỰID������ʶ���·ͨ��
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_srtp_lifetime_reinvite(mkit_handle_t caller, int line_id, int call_id);

#endif/* __SIP_API_DEFINE_xxx */


/****************************************************************************/
/*    Message ID, SIP report msg to client, use notify, from 0x0000~0x08ff  */
/****************************************************************************/

    /*message :from server to client,should be used for notification*/
#define SIP_MSG_S2C_MIN      0x0000
#define SIP_MSG_S2C_MAX      0x0FFF

    /* server->client use for notification */
#define SIP_MSG_S2C_SIZE        256
#define SIP_MSG_S2C_REG         (SIP_MSG_S2C_SIZE*0)   /* [0x000->0x0FF]section of register */
#define SIP_MSG_S2C_CALL        (SIP_MSG_S2C_SIZE*1)   /* [0x100->0x1FF]section of call */
#define SIP_MSG_S2C_SUB         (SIP_MSG_S2C_SIZE*2)   /* [0x200->0x2FF]section of subscribe/notify */
#define SIP_MSG_S2C_MSG         (SIP_MSG_S2C_SIZE*3)   /* [0x300->0x3FF]section of other message */
#define SIP_MSG_S2C_CMD         (SIP_MSG_S2C_SIZE*4)   /* [0x400->0x4FF]section of command for client */
#define SIP_MSG_S2C_REQ         (SIP_MSG_S2C_SIZE*5)   /* [0x500->0x5FF]section of request(send/call) msg */


#if WIN32
    #ifndef MKSRV_ID_CONFIG
    #define MKSRV_ID_CONFIG       1
    #endif

    #ifndef MKSRV_ID_SYSTEM
    #define MKSRV_ID_SYSTEM         2 /*system/network service,old name "rtService"*/
    #endif

    #ifndef MKSRV_ID_SIPUA
    #define MKSRV_ID_SIPUA          4 /*SIP service*/
    #endif

    #ifndef MKSRV_MSG
    #define MKSRV_MSG(id,msg)       ((((id)&0xFF)<<16)|((msg)&0xFFFF)<<0)
    #endif
#endif

#define SIP_S2C_MSG(sub, id)        MKSRV_MSG(MKSRV_ID_SIPUA, (SIP_MSG_S2C_MIN+(sub)+(id)))
#define SIP_S2C_RANGE(msg, range)   (msg>SIP_S2C_MSG(range, 0) && msg<SIP_S2C_MSG(range, SIP_MSG_S2C_SIZE))

#ifndef __SIP_REG_DEFINE_xxx
/*
 *------------------------------------
 *      notify message for register
 *------------------------------------
 */
    /*
    * usage   : �ϱ�ע��״̬
    *          
    * msg_id  : 0x00040001
    * target  : none
    * wparam  : line_id
    * lparam  : register status(-1~int),SIP_REG_XXX
    * data_ptr: sip_reason_info_t
    * data_len: 
    */
    #define SIP_REG_UNKNOW                     -1/* register status unknow */
    #define SIP_REG_DISABLE                     0/* 0 */
    #define SIP_REG_REGING                      1/* 1 */
    #define SIP_REG_REGED                       2/* 2 */
    #define SIP_REG_REGFAIL                     3/* 3 */
    #define SIP_REG_UNREGING                    4/* 4 ����ע�����˻� */
    #define SIP_REG_UNREGEND                    5/* 5 */
    #define SIP_REG_REGISTERED_ON_BOOT          6/* 6 �ϵ�ע��ɹ� */
    #define SIP_REG_DISABLE_REGISTER            7/* 7 �˺�Ϊactive�����ǲ�����ע�ᱨ�� */
    #define SIP_REG_CANCELLING                  8/* 8 ȡ��ע���� */
    #define SIP_REG_CANCEL_COMPLETED            9/* 9 ȡ��ע����� */

#define SIP_REG_UPDATE_REGISTER                 SIP_S2C_MSG(SIP_MSG_S2C_REG, 0x0001)


    /*
    * usage   : �ϱ��������·���ע���¼����յ�ע���NOTIFYʱ���ϱ���
    *          
    * msg_id  : 0x00040002
    * target  : none
    * wparam  : line_id
    * lparam  : register notify type(0~int)
    * data_ptr: xml_body(char *)
    * data_len: strlen(xml_body)+1
    */
    #define SIP_REG_NOTIFY_NONE                 0
    #define SIP_REG_NOTIFY_REGISTERED           1
    #define SIP_REG_NOTIFY_CREATED              2
    #define SIP_REG_NOTIFY_REFRESHED            3
    #define SIP_REG_NOTIFY_SHORTENED            4
    #define SIP_REG_NOTIFY_EXPIRED              5
    #define SIP_REG_NOTIFY_DEACTIVED            6
    #define SIP_REG_NOTIFY_PROBATION            7
    #define SIP_REG_NOTIFY_UNREGISTERED         8
    #define SIP_REG_NOTIFY_REJECTED             9
    #define SIP_REG_LYNC_SURVIVABLE_MODE        10/* keep registered in survivable mode */
    #define SIP_REG_LYNC_SERVICE_AVAIL          11/* user services available */
    #define SIP_REG_LYNC_DEREGISTER             12/* de_register with reason */

#define SIP_REG_UPDATE_REGISTER_NOTIFY          SIP_S2C_MSG(SIP_MSG_S2C_REG, 0x0002)


    /*
    * usage   : ���µ�ǰע��ķ�������ַ��һ�����ڱ��ݷ��������л�֪ͨ
    *          
    * msg_id  : 0x00040003
    * target  : none
    * wparam  : line_id
    * lparam  : active server idx
    * data_ptr: NULL
    * data_len: 0
    */

#define SIP_REG_UPDATE_SIP_SERVER               SIP_S2C_MSG(SIP_MSG_S2C_REG, 0x0003)

    /*
    * usage   : ע��ʧ������ʱ��
    *          
    * msg_id  : 0x00040004
    * target  : none
    * wparam  : line_id
    * lparam  : retry time
    * data_ptr: NULL
    * data_len: 0
    */
#define SIP_REG_UPDATE_RETRY_TIME               SIP_S2C_MSG(SIP_MSG_S2C_REG, 0x0004)

    /*
    * usage   : ����DEVICE PAIRING״̬����Ϣ
    *          
    * msg_id  : 0x00040005
    * target  : none
    * wparam  : line_id
    * lparam  : type
    * data_ptr: msg
    * data_len: 
    */
    #define SIP_REG_DEV_PAIR_NONE                       0
    #define SIP_REG_DEV_PAIR_GET_CFG_INFO               1
    #define SIP_REG_DEV_PAIR_NOTIFY_PAIR_INFO           2
    #define SIP_REG_DEV_PAIR_GET_TOKEN                  3
    #define SIP_REG_DEV_PAIR_GET_TOKEN_SUCCESS          4
    #define SIP_REG_DEV_PAIR_GET_TOKEN_TIMEOUT          5
    #define SIP_REG_DEV_PAIR_USER_CANCEL_REG            6
    #define SIP_REG_DEV_PAIR_COUNTRY_CODE_INVALID       7
    #define SIP_REG_DEV_PAIR_NO_CFG_FOR_COUNTRY         8
    #define SIP_REG_DEV_PAIR_OTHER_UNDEFINE_ERROR       9

#define SIP_REG_UPDATE_DEV_PAIR_INFO            SIP_S2C_MSG(SIP_MSG_S2C_REG, 0x0005)

    /*
    * usage   : ע�������
    *          
    * msg_id  : 0x00040006
    * target  : none
    * wparam  : line_id
    * lparam  : none
    * data_ptr: NULL
    * data_len: 0
    */

#define SIP_REG_UPDATE_ERROR_CODE               SIP_S2C_MSG(SIP_MSG_S2C_REG, 0x0006)

    /*
    * usage   : Exchange��ַ
    *          
    * msg_id  : 0x00040007
    * target  : none
    * wparam  : line_id
    * lparam  : none
    * data_ptr: url
    * data_len: 
    */

#define SIP_REG_UPDATE_EXCHANGE_URL             SIP_S2C_MSG(SIP_MSG_S2C_REG, 0x0007)


    /*
    * usage   : �ϱ��˺�ע��ɹ�ʹ�õĴ��䷽ʽ
    *          
    * msg_id  : 0x00040008
    * target  : none
    * wparam  : line_id
    * lparam  : transport
    * data_ptr: null
    * data_len: 0
    */
    #define SIP_REG_TRANSPORT_UDP           0
    #define SIP_REG_TRANSPORT_TCP           1
    #define SIP_REG_TRANSPORT_TLS           2

#define SIP_REG_TRANSPORT                 SIP_S2C_MSG(SIP_MSG_S2C_REG, 0x0008)


    /*
    * usage   : �ϱ��˺�ע��ɹ�ʹ�õķ���������
    *          
    * msg_id  : 0x00040009
    * target  : none
    * wparam  : line_id
    * lparam  : server type
    * data_ptr: null
    * data_len: 0
    */

/* ��sip_server_type_e���屣��һ�� */
#define SIP_REG_SERVER_TYPE_DEFAULT                 0
#define SIP_REG_SERVER_TYPE_LYNC                    1
#define SIP_REG_SERVER_TYPE_BROADSOFT               2
        /* undefined 3 */
#define SIP_REG_SERVER_TYPE_COSMOCOM                4
#define SIP_REG_SERVER_TYPE_TELTRONICS              5
#define SIP_REG_SERVER_TYPE_UCAP                    6
#define SIP_REG_SERVER_TYPE_HUAWEI_UC               7
#define SIP_REG_SERVER_TYPE_GENESYS                 8
#define SIP_REG_SERVER_TYPE_NOLOGY                  9
#define SIP_REG_SERVER_TYPE_GENESYS_ADV             10
#define SIP_REG_SERVER_TYPE_DEXGATE                 11
#define SIP_REG_SERVER_TYPE_START2START             12
#define SIP_REG_SERVER_TYPE_APOLLO                  13
#define SIP_REG_SERVER_TYPE_COMVERSE                14
#define SIP_REG_SERVER_TYPE_GENBAND                 15


#define SIP_REG_SERVER_TYPE                 SIP_S2C_MSG(SIP_MSG_S2C_REG, 0x0009)


    /*
    * usage   : �ϱ�web sign in��Ϣ
    *          
    * msg_id  : 0x0004000a
    * target  : none
    * wparam  : line_id
    * lparam  : none
    * data_ptr: web sign in��Ϣ�洢·��
    * data_len: 
    */
#define SIP_REG_UPDATE_WEB_SIGN_IN_INFO    SIP_S2C_MSG(SIP_MSG_S2C_REG, 0x000a)

/* ����֧�ַ�֧�����ӿ���ϢԤ�� 0x000->0x04F��Lync Ԥ�� 0x050->0x07F
   ������Ŀ����ʱ������ʵ����Ҫ���з�֧ͬ����������Ԥ����Χ */


/************************************************************************/
/*                   Lync msg id 0x050->0x07F start                     */
/************************************************************************/

//#define SIP_REG_LYNC_XXX                 SIP_S2C_MSG(SIP_MSG_S2C_REG, 0x0050)

/************************************************************************/
/*                   Lync msg id 0x050->0x07F end                       */
/************************************************************************/


#endif/* __SIP_REG_DEFINE_xxx */

#ifndef __SIP_CALL_DEFINE_xxx
/*
 *------------------------------------
 *      notify message for call
 *------------------------------------
 */

#define SIP_CALL_START                          SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0000)
    /*
    * usage   : �ϱ�������
    *          
    * msg_id  : 0x00040101
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_call_identify_info_t
    * data_len: 
    */

#define SIP_CALL_INCOMING                       SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0001)


    /*
    * usage   : �ϱ�ͨ����������
    *          
    * msg_id  : 0x00040102
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: none
    * data_len: none
    * remark  : ��δʹ��
    */

#define SIP_CALL_PROCEEDING                     SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0002)


    /*
    * usage   : �ϱ��Է���������
    *          
    * msg_id  : 0x00040003
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_call_ringback_info_t
    * data_len: 
    */

#define SIP_CALL_RINGBACK                       SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0003)


    /*
    * usage   : �ϱ�ͨ�����ܾ�
    *          
    * msg_id  : 0x00040104
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_CALL_REFUSE                         SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0004)


    /*
    * usage   : �ϱ�ͨ�������ɹ���ͨ���ս���ʱ���ϱ�����Ϣ��ͨ������ʹ��SIP_MSG_CALL_UPDATE
    *          
    * msg_id  : 0x00040105
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_channels_t
    * data_len: 
    */

#define SIP_CALL_ESTABLISH                      SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0005)


    /*
    * usage   : �ϱ�ͨ��ʧ��
    *          
    * msg_id  : 0x00040106
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_CALL_FAIL                           SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0006)


    /*
    * usage   : �ϱ�ͨ���ѽ���
    *          
    * msg_id  : 0x00040107
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */
#define SIP_RC_FAILED     -1
#define SIP_RC_SUCCESS     0
#define SIP_RC_NO_RECORD   1

#define SIP_CALL_RELEASED                       SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0007)


    /*
    * usage   : �ϱ�ͨ�����£���HOLD����HOLD��Ҳ��ͨ������Ϣʵ��
    *          
    * msg_id  : 0x00040108
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_channels_t
    * data_len: 
    */

#define SIP_CALL_UPDATE                         SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0008)


    /*
    * usage   : �ϱ�ͨ����Forward����Ҫ���º��У�ͨ���յ�3XXʱ���ϱ���
    *          
    * msg_id  : 0x00040109
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_call_be_forward_t
    * data_len: 
    */

#define SIP_CALL_FORWARDED_BY_REMOTE            SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0009)


    /*
    * usage   : �ϱ�ͨ����ת�ӣ�ͨ���յ�REFERʱ���ϱ���
    *          
    * msg_id  : 0x0004010a
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_call_be_transfer_t
    * data_len: 
    */
#define SIP_CALL_TRANSFERED_BY_REMOTE           SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x000a)


    /*
    * usage   : �ϱ�ͨ����REPLACE��ͨ���յ������粢��REPLACEͷ��ʱ���ϱ���
    *          
    * msg_id  : 0x0004010b
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_call_be_replace_t
    * data_len: 
    */
#define SIP_CALL_REPLACED_BY_REMOTE             SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x000b)


    /*
    * usage   : �ϱ�����״̬
    *          
    * msg_id  : 0x0004010c
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: xml_body(char *)
    * data_len: strlen
    * remark  : ��δʹ��
    */
#define SIP_CALL_CONFERENCE_NOTIFY              SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x000c)


    /*
    * usage   : �ϱ�ͨ����ʾ����
    *          
    * msg_id  : 0x0004010d
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_user_info_t
    * data_len: 
    */
#define SIP_CALL_UPDATE_INFO                    SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x000d)


    /*
    * usage   : �ϱ�ͨ�����յ���I֡ͬ������
    *          
    * msg_id  : 0x0004010e
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: xml_body(char *)
    * data_len: strlen
    */
#define SIP_CALL_RECV_FRAME_SYNC                SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x000e)


#define SIP_CALL_LYNC_IDENTITY_INFO_UPDATE      SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x000f)
#define SIP_CALL_LYNC_INSUFFICIENT_BW           SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0010)


    /*
    * usage   : �ϱ�ͨ�������еĶ������ݸ���
    *          
    * msg_id  : 0x00040111
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_call_infomation_header_t
    * data_len: 
    */
    #define SIP_CALL_INFORMATION_SKILLNAME                          1
    #define SIP_CALL_INFORMATION_BSFT_CALL_CENTER_INFO              2
    #define SIP_CALL_INFORMATION_IMAGE_URI                          3
    #define SIP_CALL_INFORMATION_REASON_TEXT                        4
    #define SIP_CALL_INFORMATION_SIP_CALL_ID                        5
    #define SIP_CALL_INFORMATION_IMAGE_INFO                         6
    #define SIP_CALL_INFORMATION_DISPLAY_TEXT                       7
    #define SIP_CALL_INFORMATION_INFO                               8
    #define SIP_CALL_INFORMATION_LYNC_DELEGATION                    9
    #define SIP_CALL_INFORMATION_LYNC_BOSS                          10
    #define SIP_CALL_INFORMATION_SECURITY_CLASS                     11
    #define SIP_CALL_INFORMATION_REFERRED_BY                        12
#define SIP_CALL_INFORMATION_UPDATE             SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0011)


    /*
    * usage   : �ϱ�ͨ��codec�б�
    *          
    * msg_id  : 0x00040112
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_media_codec_array_t
    * data_len: 
    */
#define SIP_CALL_CODEC_LIST                       SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0012)
    /*
    * usage   : �ϱ�ͨ����Forward����Ҫ���º��У�ͨ���յ�3XXʱ���ϱ���
    *          
    * msg_id  : 0x00040113
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_call_be_forward_ext_t
    * data_len: 
    */

#define SIP_CALL_FORWARDED_BY_REMOTE_EXT          SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0013)

    /*
    * usage   : �ϱ�ͨ����ת�ӣ�ͨ���յ�REFERʱ���ϱ���
    *          
    * msg_id  : 0x00040114
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_call_be_transfer_ext_t
    * data_len: 
    */
#define SIP_CALL_TRANSFERED_BY_REMOTE_EXT         SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0014)
    /*
    * usage   : �ϱ�ͨ����REPLACE��ͨ���յ������粢��REPLACEͷ��ʱ���ϱ�������Я����ͨ������
    *          
    * msg_id  : 0x00040115
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_call_be_replace_ext_t
    * data_len: 
    */
#define SIP_CALL_REPLACED_BY_REMOTE_EXT           SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0015)

    /*
    * usage   : �ϱ��յ���lync unpark֪ͨ
    *          
    * msg_id  : 0x00040116
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_callpark_info_t
    * data_len: 
    */
#define SIP_CALL_LYNC_UNPARK_NOTIFICATION         SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0016)

    /*
    * usage   : ͨ�������ϱ�
    *          
    * msg_id  : 0x00040117
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_call_type_info_t
    * data_len: 
    */

#define SIP_CALL_LYNC_EARLY_CALL_TYPE           SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0017)


    /*
    * usage   : �ϱ�broadworksͨ��¼��״̬
    *          
    * msg_id  : 0x00040118
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: bw_call_record_t
    * data_len: 
    */
typedef struct  
{
    int op_result;              /* call recording ���������0:�ɹ���-1:ʧ�ܣ�other: reason_code */

#define SIP_BW_CALL_RECORD_OFF          0
#define SIP_BW_CALL_RECORD_ON           1
#define SIP_BW_CALL_RECORD_PAUSED       2

    int audio_record_status;
    int video_record_status;
}bw_call_record_t;

#define SIP_CALL_BW_CALL_RECORD_STATUS           SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0018)


    /*
    * usage   : �ϱ�Q850״̬��
    *          
    * msg_id  : 0x00040119
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_Q850_info_t
    * data_len: 
    */

#define SIP_Q850_USER_BUSY      17
#define SIP_Q850_CALL_REJECTED  21
#define SIP_CALL_Q850_CAUSE_CODES                SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0019)

    /*
    * usage   : �ϱ�HOLDʧ��
    *          
    * msg_id  : 0x0004011a
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_CALL_HOLD_FAIL                      SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x001a)

    /*
    * usage   : �ϱ�RESUMEʧ��
    *          
    * msg_id  : 0x0004011b
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_CALL_RESUME_FAIL                    SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x001b)


/* ����֧�ַ�֧�����ӿ���ϢԤ�� 0x000->0x04F��Lync Ԥ�� 0x050->0x07F
   ������Ŀ����ʱ������ʵ����Ҫ���з�֧ͬ����������Ԥ����Χ */

/************************************************************************/
/*                   Lync msg id 0x050->0x07F start                     */
/************************************************************************/

//#define SIP_CALL_LYNC_XXX                 SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0050)

/************************************************************************/
/*                   Lync msg id 0x050->0x07F end                       */
/************************************************************************/


/* ��Ϊת�ӷ���ʱ�Ҷϸ�·ͨ�� */
#define SIP_TRANSFEROR_NOT_HANGUP                       0
#define SIP_TRANSFEROR_HANGUP_AFTER_200OK_NOTIYF        1
#define SIP_TRANSFEROR_HANGUP_AFTER_180RING_NOTIYF      2
#define SIP_TRANSFEROR_HANGUP_AFTER_100TRING_NOTIYF     3
#define SIP_TRANSFEROR_HANGUP_AFTER_REFER_RESPONSE      4

    /*
    * usage   : �ϱ�ͨ��ת�ӽ��
    *          
    * msg_id  : 0x00040181
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_CALL_TRANSFERED_RESULT              SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0081)

    /*
    * usage   : �ϱ�HOLDʧ��ԭ��ע�����Ϣ����HOLD�쳣/ʧ��ʱ���ϱ������HOLD�ɹ���ʹ��SIP_MSG_CALL_UPDATE
    *          
    * msg_id  : 0x00040182
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_CALL_PAUSED_RESULT                  SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0082)


    /*
    * usage   : �ϱ�Call Record On�Ƿ�ɹ�
    *          
    * msg_id  : 0x00040183
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_CALL_RECORD_ON_RESULT               SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0083)

    /*
    * usage   : �ϱ�Call Record Off�Ƿ�ɹ�
    *          
    * msg_id  : 0x00040184
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_CALL_RECORD_OFF_RESULT              SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0084)


    /*
    * usage   : �ϱ�INFO���ͽ��
    *          
    * msg_id  : 0x00040185
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_CALL_SEND_INFO_RESULT               SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0085)

    /*
    * usage   : �ϱ��յ���lync CallPark״̬
    *          
    * msg_id  : 0x00040186
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_reason_info_t
    * data_len: 
    */
#define SIP_CALL_LYNC_CALLPARK_RESULT           SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0086)


    /*
    * usage   : �ϱ�ͨ����Э��ʧ��
    *          
    * msg_id  : 0x00040187
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_reason_info_t
    * data_len: 
    */
#define SIP_CALL_REINVITE_FAIL                  SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0087)

    /*
    * usage   : �ϱ�ͨ���������
    *          
    * msg_id  : 0x00040188
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_channels_t
    * data_len: 
    */
#define SIP_CALL_CHANNEL_CHANGE                 SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0088)

  /*
    * usage   :�ϱ����齨�����
    * msg_id  : 0x00040189
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */
#define SIP_MSG_APOLLO_CREATE_CONFERENCE_RESULT   SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0089)

    /*
    * usage   : �ϱ���������ʧ��
    * msg_id  : 0x0004018a
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_reason_info_t
    * data_len: sizeof(sip_reason_info_t)
    */
#define SIP_MSG_APOLLO_CONF_REQUEST_RESULT        SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x008a)

    /*
    * usage   :�ϱ�Reinvite
    * msg_id  : 0x0004018c
    * target  : none
    * wparam  : line_id
    * lparam  : callid
    * data_ptr: sip_reinvite_t
    * data_len: 
    */
#define SIP_CALL_REINVITE_RECIEVE                  SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x008c)

    /*
    * usage   :�ϱ��������ʾ��Ϣ
    * msg_id  : 0x0004018d
    * target  : none
    * wparam  : line_id
    * lparam  : callid
    * data_ptr: sip_user_info_t
    * data_len: 
    */
#define SIP_CALL_EXT_INFO                          SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x008d)

    /*
    * usage   :cong��ӳ�Աʧ��
    * msg_id  : 0x0004018e
    * target  : none
    * wparam  : line_id
    * lparam  : conf call id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */
#define SIP_CALL_CONF_ADD_USER_FAIL                SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x008e)

    /*
    * usage   : ֪ͨUI Warning header ��Ϣ��UI����������Ҫ������ʾ
    * msg_id  : 0x0004018f
    * target  : none
    * wparam  : line id
    * lparam  : call id
    * data_ptr: char * ����ʽ��warn-code SP warn-agent SP warn-text
    *                       eg. 307 isi.edu "Session parameter 'foo' not understood"��
    * data_len: strlen + 1
    */
#define SIP_CALL_WARNING_INFO                     SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x008f)

  /*
    * usage   :�ϱ�����INFO��Ϣ
    * msg_id  : 0x00040190
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr:xml
    * data_len: 
    */
#define SIP_MSG_APOLLO_CONFERENCE_INFO_RESULT   SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0090)

 /*
    * usage   :�ϱ�����ԤԼ��Ϣ
    * msg_id  : 0x00040191
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: xml
    * data_len: 
 */
#define SIP_MSG_APOLLO_CONFERENCE_GET_MSG       SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0091)

/* 
    1�����Լ���֧��������Ϣ�ɴ�0x0050ǰ����
    2��0x0004018b δ��ʹ�ã�������Ϣʱ����ʹ�ã�ʹ����ȥ����ע��
*/

/************************************************************************/
/*                   VMR 0x00e0---->0x00FF start                        */
/************************************************************************/

 /*
    * usage   :�ϱ�VMR ������ͨ��ע����Ϣ
    * msg_id  : 0x000401e0
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_invite_t
    * data_len: sizeof(sip_apollo_vmr_invite_t)
 */
#define SIP_APOLLO_VMR_MSG_INVITE              SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e0)

 /*
    * usage   :�ϱ�VMR ����������Ϣ
    * msg_id  : 0x000401e1
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sizeof(sip_apollo_vmr_conf_data_t)
    * data_len: sip_apollo_vmr_conf_data_t
 */
#define SIP_APOLLO_VMR_MSG_CONF_CREATE         SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e1)



 /*
    * usage   :�ϱ�VMR ɾ��������Ϣ
    * msg_id  : 0x000401e2
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_conf_destroy_t
    * data_len: sizeof(sip_apollo_vmr_conf_destroy_t)
 */
#define SIP_APOLLO_VMR_MSG_CONF_DESTORY        SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e2)

 /*
    * usage   :�ϱ�VMR �޸Ļ�����Ϣ
    * msg_id  : 0x000401e3
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_conf_data_t
    * data_len: sizeof(sip_apollo_vmr_conf_data_t)
 */
#define SIP_APOLLO_VMR_MSG_CONF_MODIFT         SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e3)

 /*
    * usage   :�ϱ�VMR ͨ��������Ϣ
    * msg_id  : 0x000401e4
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_start_session_req_t
    * data_len: sizeof(sip_apollo_vmr_start_session_req_t)
 */
#define SIP_APOLLO_VMR_MSG_START_SESSION       SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e4)


 /*
    * usage   :�ϱ�VMR ͨ��������Ϣ(������)
    * msg_id  : 0x000401e5
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_call_release_t
    * data_len: sizeof(sip_apollo_vmr_call_release_t)
 */
#define SIP_APOLLO_VMR_MSG_STOP_SESSION        SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e5)

 /*
    * usage   :�ϱ�VMR ϵͳ���ò�ѯ��Ϣ
    * msg_id  : 0x000401e6
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_conf_query_req_t
    * data_len: sizeof(sip_apollo_vmr_conf_query_req_t)
 */
#define SIP_APOLLO_VMR_MSG_CONF_QUERY          SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e6)

 /*
    * usage   :�ϱ�VMR ϵͳ������ѯ��Ϣ
    * msg_id  : 0x000401e7
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_system_query_req_t
    * data_len: sizeof(sip_apollo_vmr_system_query_req_t)
 */
#define SIP_APOLLO_VMR_MSG_RESOURCE_QUERY      SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e7)

 /*
    * usage   :�ϱ�VMR ͨ��hold��Ϣ
    * msg_id  : 0x000401e8
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_hold_session_req_t
    * data_len: sizeof(sip_apollo_vmr_hold_session_req_t)
 */
#define SIP_APOLLO_VMR_MSG_HOLD_SESSION        SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e8)

 /*
    * usage   :�ϱ�VMR ͨ��resume��Ϣ
    * msg_id  : 0x000401e9
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_unhold_session_req_t
    * data_len: sizeof(sip_apollo_vmr_unhold_session_req_t)
 */
#define SIP_APOLLO_VMR_MSG_RESUME_SESSION      SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e9)

 /*
    * usage   :�ϱ�VMR Mute������Ϣ
    * msg_id  : 0x000401ea
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_mute_media_req_t
    * data_len: sizeof(sip_apollo_vmr_mute_media_req_t)
 */
#define SIP_APOLLO_VMR_MSG_MUTE_MEDIA          SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00ea)

 /*
    * usage   :�ϱ�VMR ͨ������������Ϣ
    * msg_id  : 0x000401eb
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_layout_authority_req_t
    * data_len: sizeof(sip_apollo_vmr_layout_authority_req_t)
 */
#define SIP_APOLLO_VMR_MSG_LAYOUT_AUTHORITY    SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00eb)

 /*
    * usage   :�ϱ�VMR ͨ����ʾ������Ϣ
    * msg_id  : 0x000401ec
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_demonsrator_state_req_t
    * data_len: sizeof(sip_apollo_vmr_demonsrator_state_req_t)
 */
#define SIP_APOLLO_VMR_MSG_DEMONSRATOR_STATE   SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00ec)

 /*
    * usage   :�ϱ�VMR ͨ����Ƶ�ı���Ϣ
    * msg_id  : 0x000401ed
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_video_modify_req_t
    * data_len: sizeof(sip_apollo_vmr_video_modify_req_t)
 */
#define SIP_APOLLO_VMR_MSG_VIDEO_MODIFY        SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00ed)

 /*
    * usage   :�ϱ�VMR ͨ����Ƶ�ؼ�֡������Ϣ
    * msg_id  : 0x000401ee
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_key_frame_req_t
    * data_len: sizeof(sip_apollo_vmr_key_frame_req_t)
 */
#define SIP_APOLLO_VMR_MSG_KEY_FRAME           SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00ee)

 /*
    * usage   :�ϱ�VMR ͨ��IVR ������Ƶ������Ϣ
    * msg_id  : 0x000401ef
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_set_osd_req_t
    * data_len: sizeof(sip_apollo_vmr_set_osd_req_t)
 */
#define SIP_APOLLO_VMR_MSG_SET_OSD             SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00ef)

 /*
    * usage   :�ϱ�VMR ͨ��IVR ����������ʾ��Ϣ
    * msg_id  : 0x000401f0
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_ivr_res_t
    * data_len: sizeof(sip_apollo_vmr_ivr_res_t)
 */
#define SIP_APOLLO_VMR_MSG_VOICE_PROMPT        SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00f0)

 /*
    * usage   :�ϱ�VMR ͨ��media codec list��Ϣ
    * msg_id  : 0x000401f1
    * target  : none
    * wparam  : line_id
    * lparam  : id 
    * data_ptr: sip_apollo_vmr_media_codec_array_t
    * data_len: media_cnt*sizeof(sip_codec_array_t)+12
 */
#define SIP_APOLLO_VMR_MSG_MEDIA_CODEC        SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00f1)

 /*
    * usage   :�ϱ�VMR ͨ��ͨ��establish��Ϣ
    * msg_id  : 0x000401f2
    * target  : none
    * wparam  : line_id
    * lparam  : id 
    * data_ptr: sip_c2s_apollo_vmr_channels_payload_t
    * data_len: hdr.size
 */
#define SIP_APOLLO_VMR_MSG_ESTABLISH        SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00f2)


/************************************************************************/
/*                   VMR 0x00e0---->0x00FF end                        */
/************************************************************************/


#endif/* __SIP_CALL_DEFINE_xxx */

#ifndef __SIP_SUB_DEFINE_xxx
/*
 *------------------------------------
 *      notify message for subscribe/notify
 *------------------------------------
 */

    /*
    * usage   : ����BLF״̬
    *          
    * msg_id  : 0x00040201
    * target  : none
    * wparam  : line_id
    * lparam  : update_numbers(0~int)
    * data_ptr: sip_dsskey_info_t
    * data_len: sizeof(sip_dsskey_info_t) * update_numbers
    */
#define SIP_SUB_UPDATE_BLF_STATUS               SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0001)


    /*
    * usage   : ����BLF��չ��Ϣ
    *          
    * msg_id  : 0x00040202
    * target  : none
    * wparam  : line_id
    * lparam  : update_numbers(0~int)
    * data_ptr: sip_dsskey_blf_info_t
    * data_len: sizeof(sip_dsskey_blf_info_t) * update_numbers
    */
#define SIP_SUB_UPDATE_BLF_STATUS_EXT           SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0002)


    /*
    * usage   : ����BLFLIST
    *          
    * msg_id  : 0x00040203
    * target  : none
    * wparam  : line_id
    * lparam  : reserved
    * data_ptr: xml_body/path(char *)
    * data_len: none
    */
#define SIP_SUB_UPDATE_BLFLIST                  SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0003)


    /*
    * usage   : ����BLA״̬
    *          
    * msg_id  : 0x00040204
    * target  : none
    * wparam  : line_id
    * lparam  : update_numbers(0~int)
    * data_ptr: sip_dsskey_info_t
    * data_len: sizeof(sip_dsskey_info_t) * update_numbers
    */
#define SIP_SUB_UPDATE_BLA_STATUS               SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0004)


    /*
    * usage   : ����appearance״̬
    *          
    * msg_id  : 0x00040205
    * target  : none
    * wparam  : line_id
    * lparam  : update_num
    * data_ptr: sip_sca_status_t array
    * data_len: 
    */
#define SIP_SUB_UPDATE_BROADSOFT_SCA_STATUS     SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0005)


    /*
    * usage   : �ϱ�ռ�߳ɹ�״̬
    *          
    * msg_id  : 0x00040206
    * target  : none
    * wparam  : line_id
    * lparam  : appearance-index
    * data_ptr: ָ��intָ�룬����Ϊ0��ʾʧ�ܣ�Ϊ1��ʾ�ɹ�
    * data_len: sizeof(int)
    */
#define SIP_SUB_UPDATE_BROADSOFT_SCA_SEIZE_RESULT     SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0006)


    /*
    * usage   : ����Call Completion״̬
    *          
    * msg_id  : 0x00040207
    * target  : none
    * wparam  : line_id
    * lparam  : status(0~int)
    * data_ptr: sip_user_info_t
    * data_len: 
    */
#define SIP_SUB_UPDATE_CC_STATUS                SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0007)


    /*
    * usage   : ���¹��ܼ�ͬ��״̬
    *          
    * msg_id  : 0x00040208
    * target  : none
    * wparam  : line_id
    * lparam  : type(0~int)
    * data_ptr: xml_body(char *)
    * data_len: strlen
    */
#define SIP_SUB_UPDATE_FEATURE_KEY_STATUS       SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0008)


    /*
    * usage   : ����Hoteling״̬
    *          
    * msg_id  : 0x00040209
    * target  : none
    * wparam  : line_id
    * lparam  : status(int, like SIP_HOTELING_SUBSCRIBE_SUCCESS)
    * data_ptr: user name(char *) 
    * data_len: strlen
    */

    #define SIP_HOTELING_LOGOUT_SUCCESS     0
    #define SIP_HOTELING_LOGIN_SUCCESS      1
    #define SIP_HOTELING_LOGIN_AUTH_FAIL    2
    #define SIP_HOTELING_SUBSCRIBE_SUCCESS  3
    #define SIP_HOTELING_SUBSCRIBE_FAIL     4
    #define SIP_HOTELING_REQUIRE_LOGOUT     5
#define SIP_SUB_UPDATE_HOTELING_STATUS          SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0009)


    /*
    * usage   : �ϱ��յ���Broadworks CallPark״̬
    *          
    * msg_id  : 0x0004020a
    * target  : none
    * wparam  : line_id
    * lparam  : reserved
    * data_ptr: xml_body(char *)
    * data_len: strlen
    */
#define SIP_SUB_BSFT_CALLPARK_UPDATE            SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x000a)


    /*
    * usage   : �ϱ��յ���Broadworks Call Center״̬
    *          
    * msg_id  : 0x0004020b
    * target  : none
    * wparam  : line_id
    * lparam  : reserved
    * data_ptr: xml_body(char *)
    * data_len: strlen
    */
#define SIP_SUB_UPDATE_CALL_CENTER_STATUS       SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x000b)


    /*
    * usage   : �ϱ��յ���GCS��Ϣ
    *          
    * msg_id  : 0x0004020c
    * target  : none
    * wparam  : line_id
    * lparam  : reserved
    * data_ptr: sip_gcs_info_t
    * data_len: 
    */
#define SIP_SUB_GCS_NOTIFY                      SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x000c)


    /*
    * usage   : ����ACD״̬
    *          
    * msg_id  : 0x0004020d
    * target  : none
    * wparam  : line_id
    * lparam  : update numbers(0~int)
    * data_ptr: sip_acd_status_info_t
    * data_len: 
    */
    /* ACD״ֵ̬ */
    #define SIP_ACD_STATUS_UNKNOWN              0
    #define SIP_ACD_STATUS_LOGINING             1
    #define SIP_ACD_STATUS_LOGIN_SUCCESS        2
    #define SIP_ACD_STATUS_LOGIN_FAIL           3
    #define SIP_ACD_STATUS_AVAILABLE            4
    #define SIP_ACD_STATUS_UNAVAILABLE          5
    #define SIP_ACD_STATUS_LOGOUT               6
    #define SIP_ACD_STATUS_NOTIFY_200OK         7

#define SIP_SUB_UPDATE_ACD_STATUS               SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x000d)


    /*
    * usage   : ����Dial Tone
    *          
    * msg_id  : 0x0004020e
    * target  : none
    * wparam  : line_id
    * lparam  : dial tone(0~int)
    * data_ptr: none
    * data_len: none
    */
    #define SIP_DIAL_TONE_STANDARD              0
    #define SIP_DIAL_TONE_SPECIAL_CONDITION     1
    #define SIP_DIAL_TONE_MESSAGE_WAITING       2
#define SIP_SUB_UPDATE_DIAL_TONE                SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x000e)


    /*
    * usage   : �յ�Hot Desking��Ϣ
    *          
    * msg_id  : 0x0004020f
    * target  : none
    * wparam  : line_id
    * lparam  : reserved
    * data_ptr: body(char *)
    * data_len: strlen
    */
#define SIP_SUB_HOT_DESKING_NOTIFY              SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x000f)


    /*
    * usage   : �յ�Tiptel PBX Control����
    *          
    * msg_id  : 0x00040210
    * target  : none
    * wparam  : line_id
    * lparam  : 0: led_control 1: call_invoke
    * data_ptr: body(char *)
    * data_len: strlen
    */
#define SIP_SUB_TIPTEL_PBX_CONTROL_NOTIFY       SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0010)


    /*
    * usage   : �ϱ�Lync contacts���Ľ��(������ϵ���б���ϵ����Ϣ�Ľ�����ô���Ϣ�ϱ�)
    *          
    * msg_id  : 0x00040211
    * target  : none
    * wparam  : line_id
    * lparam  : sip_lync_contact_info_type_e
    * data_ptr: sip_lync_contact_info_t
    * data_len: sizeof(sip_lync_contact_info_t) - SIP_MAX_MSGKIT_BODY_LEN 
                                                + strlen(data_ptr->buf) + 1
    */
#define SIP_SUB_LYNC_CONTACTS_UPDATE            SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0011)


    /*
    * usage   : �ϱ�Lync Provision���Ľ��
    *          
    * msg_id  : 0x00040212
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_lync_provision_t����Ҫʹ��sip_map_info_recover�ӿڶ�ȡ���ݣ�
    * data_len: sip_lync_provision_t.size
    */
#define SIP_SUB_LYNC_PROVISION_UPDATE           SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0012)

/*
    * usage   : Draft Bla״̬ͨ��
    *          
    * msg_id  : 0x00040213
    * target  : none
    * wparam  : line_id
    * lparam  : appearance id
    * data_ptr: BLA״̬��SIP_BLA_DRAFT_STATUS_SEIZED
    * data_len: 
    */
#define SIP_SUB_UPDATE_DRAFT_BLA_STATUS         SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0013)


    /*
    * usage   : �ϱ�Lync Boss Provision���Ľ��
    *          
    * msg_id  : 0x00040214
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_lync_boss_provision_t����Ҫʹ��sip_map_info_recover�ӿڶ�ȡ���ݣ�
    * data_len: sip_lync_boss_provision_t.size
    */
#define SIP_SUB_LYNC_BOSS_PROVISION_UPDATE      SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0014)

    /*
    * usage   : �ϱ�address book��notify body
    *          
    * msg_id  : 0x00040215
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: notify body
    * data_len: body len
    */
#define SIP_SUB_ADDRESS_BOOK_NOTIFY             SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0015)

    /*
    * usage   : �ϱ�Lync bossͬ�������
    *          
    * msg_id  : 0x00040216
    * target  : none
    * wparam  : line_id
    * lparam  : sip_lync_boss_simultan_setting_e 
    * data_ptr: none
    * data_len: none
    */
#define SIP_SUB_LYNC_BOSS_SIMULTANEOUS_SETTING_UPDATE      SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0016)



    /*
    * usage   : �ϱ�����BLF����NOTIFY���
    *          
    * msg_id  : 0x00040217
    * target  : none
    * wparam  : line_id
    * lparam  : none
    * data_ptr: xml_body(char *)
    * data_len: strlen
    */
#define SIP_SUB_BLF_SUB_SELF_RESULT                         SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0017)


    /*
    * usage   : �ϱ�Genband service package NOTIFY���
    *          
    * msg_id  : 0x00040218
    * target  : none
    * wparam  : line_id
    * lparam  : none
    * data_ptr: xml_body(char *)
    * data_len: strlen
    */
#define SIP_SUB_GENBAND_SERVICE_PACKAGE                     SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0018)

    /*
    * usage   : �ϱ�Broadsoft conference ������Ϣ
    *          
    * msg_id  : 0x00040219
    * target  : none
    * wparam  : line_id
    * lparam  : cid (��Ӧ�����cid��Ŀǰ����Ϊad hoc���ַ��ſ��Ի�ȡ��)
    * data_ptr: xml_body(char *)
    * data_len: strlen
    */
#define SIP_SUB_BW_NETWORK_CONF_INFO                        SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0019)


    /*
    * usage   : �ϱ�BW������뷽�����Ӧ��ͨ����·index
    *          
    * msg_id  : 0x0004021a
    * target  : none
    * wparam  : line_id
    * lparam  : update_num
    * data_ptr: sip_bw_conf_index_t
    * data_len: 
    */
#define SIP_SUB_BW_CONF_INDEX                                SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x001a)


/* ����֧�ַ�֧�����ӿ���ϢԤ�� 0x000->0x04F��Lync Ԥ�� 0x050->0x07F
   ������Ŀ����ʱ������ʵ����Ҫ���з�֧ͬ����������Ԥ����Χ */


/************************************************************************/
/*                   Lync msg id 0x050->0x07F start                     */
/************************************************************************/

//#define SIP_SUB_LYNC_XXX                 SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0050)

/************************************************************************/
/*                   Lync msg id 0x050->0x07F end                       */
/************************************************************************/



    /*
    * usage   : �ϱ�BLAռ�߽��
    *          
    * msg_id  : 0x00040281
    * target  : none
    * wparam  : line_id
    * lparam  : 0: fail  1:success
    * data_ptr: like "appearance-index=x"(char *)
    * data_len: strlen
    */
#define SIP_SUB_BLA_SEIZE_LINE_RESULT           SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0081)

    /*
    * usage   : �ϱ�Call Completion���Ľ��
    *          
    * msg_id  : 0x00040282
    * target  : none
    * wparam  : line_id
    * lparam  : 0:fail 1:success(int)
    * data_ptr: sip_user_info_t
    * data_len: 
    */

#define SIP_SUB_UPDATE_CC_SUBSCRIBE_RESULT      SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0082)


    /*
    * usage   : �ϱ����ܼ�ͬ�����Ľ��
    *          
    * msg_id  : 0x00040283
    * target  : none
    * wparam  : line_id
    * lparam  : reserved
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_SUB_FEATURE_KEY_SUBSCRIBED_RESULT   SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0083)

    /*
    * usage   : �ϱ�NOTIFY���ͽ��
    *          
    * msg_id  : 0x00040284
    * target  : none
    * wparam  : line_id
    * lparam  : call_id(Ϊ0��ʾ�Ự��NOTIFY)
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_SUB_SEND_NOTIFY_RESULT              SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0084)

    /*
    * usage   : �ϱ�Blaռ��/ȡ��ռ�߽��
    *          
    * msg_id  : 0x00040285/0x00040286
    * target  : none
    * wparam  : line_id
    * lparam  : appearance_id
    * data_ptr: 1-ռ��/ȡ��ռ�߳ɹ���0-ռ��/ȡ��ռ��ʧ��
    * data_len: 
    */
#define SIP_SUB_DRAFT_BLA_SEIZE_LINE_RESULT     SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0085)
#define SIP_SUB_DRAFT_BLA_UNSEIZE_LINE_RESULT   SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0086)
    #define SIP_FORTINET_DND_MODE_START                1
    #define SIP_FORTINET_DND_MODE_STOP                 0
#define SIP_SUB_FORTINET_DND_SUBSCRIBED_RESULT  SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0087)

    /*
    * usage   : Fortinet Dnd���Ľ���ϱ�
    *          
    * msg_id  : 0x0285/0x0286
    * target  : none
    * wparam  : line_id
    * lparam  : 1-����fortinet dndģʽ��0-�˳�fortinet dndģʽ
    * data_ptr: reserved
    * data_len: 
    */

    #define SIP_FORTINET_DND_ON                       1
    #define SIP_FORTINET_DND_OFF                      0
#define SIP_SUB_FORTINET_DND_NOTIFY_RESULT      SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0088)
    /*
    * usage   : �ϱ�KTS info��Body
    *          
    * msg_id  : 00x0289
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: body
    * data_len: body len
    */



#define SIP_SUB_KTS_NOTIFY_RESULT               SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0089)

    /*
    * usage   : �ϱ�ռ�߳ɹ�״̬������489 NoSupport֧��
    *          
    * msg_id  : 0x028a
    * target  : none
    * wparam  : line_id
    * lparam  : appearance-index
    * data_ptr: ָ��intָ�룬����Ϊ0��ʾʧ�ܣ�Ϊ1��ʾ�ɹ�
    * data_len: sizeof(int)
    */
#define SIP_SCA_SEIZE_FAIL 0
#define SIP_SCA_SEIZE_SUCC 1
#define SIP_SCA_SEIZE_NO_SUPPORT 2
#define SIP_SUB_UPDATE_BROADSOFT_SCA_SEIZE_RESULT2     SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x008a)

/* ���Լ���֧��������Ϣ�ɴ�0x0050ǰ���� */


#endif/* __SIP_SUB_DEFINE_xxx */

#ifndef __SIP_MSG_DEFINE_xxx

/*
 *------------------------------------
 *      notify other message
 *------------------------------------
 */
    /*
    * usage   : ��������������Ϣ
    *          
    * msg_id  : 0x00040301
    * target  : none
    * wparam  : line_id
    * lparam  : reserved
    * data_ptr: sip_mwi_info_t
    * data_len: 
    */

#define SIP_MSG_UPDATE_MWI_INFO                SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0001)


    /*
    * usage   : �յ�SMS��Ϣ
    *          
    * msg_id  : 0x00040302
    * target  : none
    * wparam  : line_id
    * lparam  : reserved
    * data_ptr: sip_sms_info_t
    * data_len: 
    */

#define SIP_MSG_UPDATE_SMS                      SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0002)


    /*
    * usage   : �յ���Ϊcomm-div-info���͵�MESSAGE��Ϣ������ǿ�ƺ���ǰתʱ��ҳ����ʾ
    *          
    * msg_id  : 0x00040303
    * target  : none
    * wparam  : line_id
    * lparam  : reserved
    * data_ptr: xml_body(char *)
    * data_len: strlen
    */

#define SIP_MSG_HUAWEI_COMM_DIV_MESSAGE         SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0003)



    /*
    * usage   :�ϱ��˺����ñ��
    *          
    * msg_id  : 0x0004030b
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_account_cfg_t
    * data_len: sizeof(sip_account_cfg_t)
    */
#define SIP_MSG_SIP_CFG_CHANGE                  SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x000b)


    /*
    * usage   : �ϱ�uaCSTA������Ϣ
    *
    * msg_id  : 0x0004030c
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_uacsta_req_info_t
    * data_len: 
    */
#define SIP_MSG_UACSTA_INFO                     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x000c)

    /*
    * usage   : �ϱ�DATEͷ��ֵ
    *          
    * msg_id  : 0x0004030d
    * target  : none
    * wparam  : line_id
    * lparam  : reserved
    * data_ptr: GMT(char *)
    * data_len: 
    */
#define SIP_MSG_UPDATE_DATE                     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x000d)


    /*
    * usage   : �ϱ�KPML������Ϣ
    *          
    * msg_id  : 0x0004030e
    * target  : none
    * wparam  : line_id
    * lparam  : kpml status (0���رգ� 1������)
    * data_ptr: sip_kpml_info_t (lparamΪ0ʱΪNULL)
    * data_len: 
    */
#define SIP_MSG_UPDATE_KPML_STATUS              SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x000e)

    /*
    * usage   : �ϱ�SIP��ǰ����״̬
    *          
    * msg_id  : 0x0004030f
    * target  : none
    * wparam  : service status
    * lparam  : reserved
    * data_ptr: none
    * data_len: none
    */

    #define SIP_SERVICE_STOPED                  0/* sip_start_service()�ӿڵ���֮ǰ��״̬����ʱ״̬��δ������ */
    #define SIP_SERVICE_NETWORK_UNAVALIABLE     1/* sip_start_service()���ú󣬵��ǵ�ǰûIP��ַʱ����𣬵ȴ�IP��ַ�仯 */
    #define SIP_SERVICE_RUNING                  2/* ��ʱAPP��������״̬ */
    #define SIP_SERVICE_RELOADING               3/* ����һ���м�״̬������reload APP��reload֮��״̬����ΪSIPUA_RUNING */
    #define SIP_SERVICE_STOPING                 4/* Э��ջֹͣing���� */

    /* ���ݾɰ汾���壬��ע�������޸���SIP_SERVICE_EXIT_SUCC��ֵ����Ϊ��ʱû��ʹ�� */
    #define SIP_SERVICE_UNKNOWN                 SIP_SERVICE_STOPED
    #define SIP_SERVICE_DESTORYING              SIP_SERVICE_RELOADING

    #define SIP_SERVICE_ERROR                   5 /*Э����ڲ�����*/
	#define SIP_SERVICE_EXIT_SUCC               6/*Function Run��ʽ����exit���ϱ����첽��Ϣ��֪ͨUI SIP�����˳� */

#define SIP_MSG_UPDATE_SERVICE_STATUS           SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x000f)

    /*
    * usage   : �ϱ��յ����Զ���INFO��Ϣ
    *          
    * msg_id  : 0x00040310
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: xml_body
    * data_len: 
    */
#define SIP_MSG_RECV_CUSTOM_INFO                SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0010)



    /*
    * usage   : �ϱ�apollo conference factroy uri��ȡ״̬
    *          
    * msg_id  : 0x00040311
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: 
    * data_len: sizeof() * update_numbers
    */
#define SIP_MSG_UPDATE_APOLLO_CONF_FACTROY       SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0011)

    /*
    * usage   :�ϱ�apollo�������Ա״̬
    * msg_id  : 0x00040312
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_lync_conference_user_t
    * data_len: sizeof(sip_lync_conference_user_t)
    */
#define SIP_MSG_APOLLO_CONF_USER_STATE_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0012)

    /*
    * usage   :�ϱ�apollo��������
    * msg_id  : 0x00040313
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_apollo_conference_descrip_t
    * data_len: sizeof(sip_apollo_conference_descrip_t)
    */
#define SIP_MSG_APOLLO_CONF_DESCRIPTION_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0013)

    /*
    * usage   :�ϱ�apollo����״̬
    * msg_id  : 0x00040314
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_apollo_conference_entity_view_t
    * data_len: sizeof(sip_apollo_conference_entity_view_t)
    */
#define SIP_MSG_APOLLO_CONF_VIEW_STATE_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0014)

    /*
    * usage   :�ϱ�apollo����ԤԼ��Ϣ������������Ϣ���ͣ�
    * msg_id  : 0x00040315
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_apollo_conference_entity_appoinement_t
    * data_len: sizeof(sip_apollo_conference_entity_appointment_t)
    */
#define SIP_MSG_APOLLO_CONF_APPOINTMENT_STATE_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0015)

    /*
    * usage   :�ϱ�apollo autop�����Ϣ
    * msg_id  : 0x00040316
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: GMT(char *)
    * data_len: 
    */
#define SIP_MSG_APOLLO_AUTOP_UPDATE              SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0016)

    /*
    * usage   :�ϱ�apollo phonebook��ϵ�˱����Ϣ
    * msg_id  : 0x00040317
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: GMT(char *)
    * data_len: 
    */
#define SIP_MSG_APOLLO_PHONE_BOOK_UPDATE              SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0017)

    /*
    * usage   :�ϱ�focus uri
    * msg_id  : 0x00040318
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: GMT(char *)
    * data_len: 
    */
#define SIP_MSG_APOLLO_CONF_FOCUS_URI                 SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0018)

    /*
    * usage   :�ϱ�source request
    * msg_id  : 0x00040319
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: GMT(char *)
    * data_len: 
    */
#define SIP_MSG_APOLLO_CONF_SOURCE_INFO               SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0019)

/**************************
*message for Lync 0x0320~0x033f
**************************/
    /*
    * usage   : �ϱ��Լ���״̬
    *          
    * msg_id  : 0x00040320
    * target  : none
    * wparam  : line_id
    * lparam  : sip_lync_presence_state_e
    * data_ptr: NULL
    * data_len: 0
    */
#define SIP_MSG_LYNC_STATE_UPDATE               SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0020)
    /*
    * usage   : �ϱ����Ź���
    * 
    * msg_id  : 0x00040321
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: file_path(char *)
    * data_len: strlen(file_path) + 1
    */
#define SIP_MSG_LYNC_LOCATION_UPDATE            SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0021)
    /*
    * usage   : �ϱ�LYNC MWI��Ϣ
    * 
    * msg_id  : 0x00040322
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_lync_mwi_data_t
    * data_len: sizeof(sip_lync_mwi_data_t)
    */
#define SIP_MSG_LYNC_MWI_UPDATE                 SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0022)
    /*
    * usage   : �ϱ�LYNC ROUTING��Ϣ
    *
    * msg_id  : 0x00040323
    * target  : none
    * wparam  : line_id
    * lparam  : sip_lync_routing_update_type_e
    * data_ptr: buf(char *)
    * data_len: strlen(buf) + 1
    */
#define SIP_MSG_LYNC_ROUTING_UPDATE             SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0023)
    /*
    * usage   :�ϱ�user infomation
    * 
    * msg_id  : 0x00040324
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_lync_user_infomation_t
    * data_len: sizeof(sip_lync_user_infomation_t)
    */
#define SIP_MSG_LYNC_USER_INFO_UPDATE           SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0024)

    /*
    * usage   : �ϱ��Լ�����Ƭ��Ϣ
    * 
    * msg_id  : 0x00040325
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_lync_contact_card_t ����Ҫʹ��sip_map_info_recover�ӿڶ�ȡ��
    * data_len: sip_lync_contact_card_t.size
    */
#define SIP_MSG_LYNC_CONTACT_CARD_UPDATE        SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0025)

    /*
    * usage   : �ϱ���ϵ�˹�ϵ
    * 
    * msg_id  : 0x00040326
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: file_path(char *)
    * data_len: strlen(file_path) + 1 
    */
#define SIP_MSG_LYNC_RELATIONSHIPS_UPDATE       SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0026)
    /*
    * usage   : �ϱ�����״̬
    * 
    * msg_id  : 0x00040327
    * target  : none
    * wparam  : line_id
    * lparam  : sip_lync_BToE_status_e
    * data_ptr: NULL
    * data_len: 0
    */
#define SIP_MSG_LYNC_BTOE_STATUS                 SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0027)
    /*
    * usage   : ����syslog��Ϣ
    * 
    * msg_id  : 0x00040328
    * target  : none
    * wparam  : 0
    * lparam  : 0
    * data_ptr: NULL
    * data_len: 0
    */
#define SIP_MSG_GET_SYSLOG_INFO                  SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0028)
    /*
    * usage   : ����syslog��Ϣ
    * 
    * msg_id  : 0x00040329
    * target  : none
    * wparam  : 0
    * lparam  : 0
    * data_ptr: sip_syslog_info_t
    * data_len: sizeof(sip_syslog_info_t)
    */
#define SIP_MSG_UPDATE_SYSLOG_INFO               SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0029)

    /*
    * usage   : �ϱ���˽ģʽ
    * 
    * msg_id  : 0x0004032a
    * target  : none
    * wparam  : line_id
    * lparam  : user select privacy mode:SIP_LYNC_PRIVACY_NOSELECT/PRIVATE/STANDARD
    * data_ptr: server enable privacy mode:  1:����, 0:�ر�
    * data_len: sizeof(unsigned int)
    */
#define SIP_MSG_LYNC_PRIVACY_MODE_UPDATE         SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x002a)

    /*
    * usage   :�ϱ�delegate list
    * msg_id  : 0x0004032b
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: file_path(char *)
    * data_len: strlen(file_path) + 1
    */
#define SIP_MSG_LYNC_DELEGATE_UPDATE            SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x002b)

    /*
    * usage   :�ϱ�boss list
    * msg_id  : 0x0004032c
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: file_path(char *)
    * data_len: strlen(file_path) + 1
    */
#define SIP_MSG_LYNC_BOSS_UPDATE                SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x002c)

    /*
    * usage   :�ϱ�boss line��ͨ��״̬
    * msg_id  : 0x0004032d
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: file_path(char *)
    * data_len: strlen(file_path) + 1
    */
#define SIP_MSG_LYNC_DIALOGINFO_UPDATE           SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x002d)

    /*
    * usage   :�ϱ�����ǩ��
    * msg_id  : 0x0004032e
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: note_buf(char *)
    * data_len: strlen(note_buf) + 1
    */
#define SIP_MSG_LYNC_NOTE_UPDATE                 SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x002e)


    /*
    * usage   :�ϱ���ʾ����
    * msg_id  : 0x0004032f
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: display_num(char *)
    * data_len: strlen(display_num) + 1
    */
#define SIP_MSG_LYNC_DISPLAY_NUM_UPDATE         SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x002f)


    /*
    * usage   : �ϱ������sipuri�б�
    *          
    * msg_id  : 0x00040330
    * target  : none
    * wparam  : lid
    * lparam  : reserved
    * data_ptr: uri
    * data_len: strlen(uri)+1
    */
#define SIP_MSG_REPORT_SIP_URI                  SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0030)

    /*
    * usage   :�ϱ����齨�����
    * msg_id  : 0x00040331
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */
#define SIP_MSG_LYNC_CREATE_CONFERENCE_RESULT   SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0031)

    /*
    * usage   :�ϱ��������Ա״̬
    * msg_id  : 0x00040332
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_lync_conference_user_t
    * data_len: sizeof(sip_lync_conference_user_t)
    */
#define SIP_MSG_LYNC_CONF_USER_STATE_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0032)

    /*
    * usage   :�ϱ�����״̬
    * msg_id  : 0x00040333
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: path
    * data_len: sizeof(path)
    */
#define SIP_MSG_LYNC_CONF_VIEW_STATE_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0033)

    /*
    * usage   : �ϱ���������ʧ��
    * msg_id  : 0x00040334
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_reason_info_t
    * data_len: sizeof(sip_reason_info_t)
    */
#define SIP_MSG_LYNC_CONF_REQUEST_RESULT          SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0034)

    /*
    * usage   :�ϱ���������
    * msg_id  : 0x00040335
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_lync_conference_descrip_t
    * data_len: sizeof(sip_lync_conference_descrip_t)
    */
#define SIP_MSG_LYNC_CONF_DESCRIPTION_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0035)


    /*
    * usage   :�ϱ�Lync��ϵ��״̬����ʧ�ܺ�Ĵ���
    * msg_id  : 0x00040336
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_lync_contacts_pres_sub_retry_t
    * data_len: sizeof(sip_lync_contacts_pres_sub_retry_t)
    */
#define SIP_MSG_LYNC_RETRY_PRES_SUB             SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0036)



    /*
    * usage   :�ϱ� dialog info instance
    * msg_id  : 0x00040337
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: instance(char *)
    * data_len: strlen(instance)+1
    */
#define SIP_MSG_LYNC_DIALOGINFO_INSTANCE         SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0037)


    /*
    * usage   :�ϱ��������focus����Ϣ
    * msg_id  : 0x00040338
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_lync_conference_focus_t
    * data_len: sizeof(sip_lync_conference_focus_t)
    */
#define SIP_MSG_LYNC_CONF_FOCUS_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0038)


    /*
    * usage   :�ϱ�������������Ϣ�������id��PSTN����ʽ���룬web uri�ȵ�
    * msg_id  : 0x00040339
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_lync_conference_focus_t
    * data_len: sizeof(sip_lync_conference_focus_t)
    */
#define SIP_MSG_LYNC_CONF_INFO_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0039)

    /*
    * usage   :�ϱ�Skype��ϵ����Ϣ
    * msg_id  : 0x0004033a
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: 
    * data_len: 
    */
#define SIP_MSG_PUBLIC_CLOUD_CONTACT_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x003a)

    /*
    * usage   :�ϱ�����״̬��Ҫ�ش�
    * msg_id  : 0x0004033b
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: 
    * data_len: 
    */
#define SIP_MSG_STATE_PUBLISH_RETRY     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x003b)

   /*
    * usage   :�ϱ�modify endpoint media���
    * msg_id  : 0x0004033c
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_lync_endpoint_media_t
    * data_len: 
    */
#define SIP_MSG_LYNC_CONF_MODIFY_MEDIA_RESPONSE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x003c)

   /*
    * usage   :����ϲ�ͨ���ϱ���Ҫ����ͨ����call_id
    * msg_id  : 0x0004033d
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: merge_cid
    * data_len: sizeof(int)
    */
#define SIP_MSG_LYNC_CONF_MERGE_CALL_CLOSED     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x003d)

   /*
    * usage   :�ϱ�modify conference���, mute_audience
    * msg_id  : 0x0004033e
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_lync_modify_conf_t
    * data_len: 
    */
#define SIP_MSG_LYNC_CONF_MODIFY_CONF_RESPONSE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x003e)

/**************************
*message for Lync end
**************************/



/************* FECC 0x0340-0x034F start **********/
    /*
    * usage   : �ϱ�����״̬�����ء�Զ�˿����߹�
    *          
    * msg_id  : 0x00040340
    * target  : none
    * wparam  : line id
    * lparam  : call id
    * data_ptr: int:ext video state
    * data_len: sizeof(int)
    */
    #define SIP_EXT_VIDEO_UNKNOWN          -1
    #define SIP_EXT_VIDEO_CLOSE            0
    #define SIP_EXT_VIDEO_LOCAL_OPEN       1
    #define SIP_EXT_VIDEO_REMOTE_OPEN      2
#define SIP_MSG_EXT_VIDEO_STATE                SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0040)

    /*
    * usage   :֪ͨ��������ͷ�ƶ�
    * msg_id  : 0x00040341
    * target  : none
    * wparam  : 0
    * lparam  : cid
    * data_ptr: i (ת�Ʒ���)
    * data_len: sizeof(int)
    */
#define SIP_MSG_FECC_CAMERA_START               SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0041)

    /*
    * usage   :֪ͨ�ر�����ͷ�ƶ�
    * msg_id  : 0x00040342
    * target  : none
    * wparam  : 0
    * lparam  : cid
    * data_ptr: none
    * data_len: none
    */
#define SIP_MSG_FECC_CAMERA_STOP                SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0042)

    /*
    * usage   :֪ͨ����ͷλ�ô洢
    * msg_id  : 0x00040343
    * target  : none
    * wparam  : 0
    * lparam  : cid
    * data_ptr: i (�洢����)
    * data_len: sizeof(int)
    */
#define SIP_MSG_FECC_STORE_AS_PRESET            SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0043)

    /*
    * usage   :֪ͨ����ͷλ�ü���
    * msg_id  : 0x00040344
    * target  : none
    * wparam  : 0
    * lparam  : cid
    * data_ptr: i (�����)
    * data_len: sizeof(int)
    */
#define SIP_MSG_FECC_ACTIVATE_PRESET            SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0044)


/************* FECC 0x0340-0x034F end ***********/


    /*
    * usage   : �ϱ������Ϣ��DMS
    * msg_id  : 0x00040350
    * target  : none
    * wparam  : event id��sip_dms_event_id_e
    * lparam  : ƽ̨��־��ͨ��λ���ֲ�ͬƽ̨
    * data_ptr: dms_alert_t
    * data_len: sizeof(dms_alert_t)
    */

typedef enum
{
    SIP_DMS_EVENT_NORMAL_START          = 0,
    SIP_DMS_EVENT_REGISTER_FAILURE      = SIP_DMS_EVENT_NORMAL_START,
    SIP_DMS_EVENT_HOLD_FAILURE,
    SIP_DMS_EVENT_RESUME_FAILURE,
    SIP_DMS_EVENT_NORMAL_END,

    SIP_DMS_EVENT_LYNC_START                = 512,
    SIP_DMS_EVENT_LYNC_REGISTER_FAILURE     = SIP_DMS_EVENT_LYNC_START,
    SIP_DMS_EVENT_LYNC_NETWORK_TRAV_FAILURE,
    SIP_DMS_EVENT_LYNC_TRANSFER_FAILURE,
    SIP_DMS_EVENT_LYNC_HOLD_FAILURE,
    SIP_DMS_EVENT_LYNC_RESUME_FAILURE,
    SIP_DMS_EVENT_LYNC_PLAY_VVM_FAILURE,
    SIP_DMS_EVENT_LYNC_MEETING_JOIN_FAILURE,
    SIP_DMS_EVENT_LYNC_MEETNOW_FAILURE,
    SIP_DMS_EVENT_LYNC_BTOE_PAIRING_FAILURE,
}sip_dms_event_id_e;

#define SIP_DM_PLATFORM_NORMAL          (1<<0)      /* �����豸����ƽ̨ */
#define SIP_DM_PLATFORM_LYNC            (1<<1)      /* Lync ƽ̨ Telemetry */


#define SIP_MSG_DMS_INFO_REPORT                 SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0050)

/************************************************************************/
/*                   Lync msg id 0x051->0x07F start                     */
/************************************************************************/

//#define SIP_MSG_LYNC_XXX                 SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0051)

/************************************************************************/
/*                   Lync msg id 0x051->0x07F end                       */
/************************************************************************/


    /*
    * usage   : �ϱ�SMS���ͽ��
    *          
    * msg_id  : 0x00040381
    * target  : none
    * wparam  : line_id
    * lparam  : reserved
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_MSG_SEND_SMS_RESULT                 SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0081)


    /*
    * usage   : �ϱ�INFO���ͽ��
    *          
    * msg_id  : 0x00040382
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_MSG_SEND_INFO_RESULT                  SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0082)


    /*
    * usage   : �ϱ�NOTIFY���ͽ��
    *          
    * msg_id  : 0x00040383
    * target  : none
    * wparam  : line_id
    * lparam  : call_id(Ϊ0��ʾ�Ự��NOTIFY)
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_MSG_SEND_NOTIFY_RESULT              SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0083)

    /*
    * usage   : Я���������ϱ�Զ�˿������INFO��ʽ��������
    *          
    * msg_id  : 0x00040384
    * target  : none
    * wparam  : line_id
    * lparam  : call_id(Ϊ0��ʾ�Ự��NOTIFY)
    * data_ptr: sip_uc_req_info_t
    * data_len: 
    */

#define SIP_MSG_UC_REQ_INFO                     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0084)

    /*
    * usage   : Я���������ϱ�Զ�˿������NOTIFY��ʽ��������
    *          
    * msg_id  : 0x00040385
    * target  : none
    * wparam  : line_id
    * lparam  : call_id(Ϊ0��ʾ�Ự��NOTIFY)
    * data_ptr: sip_uc_req_notify_t
    * data_len: 
    */

#define SIP_MSG_UC_REQ_NOTIFY                     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0085)

    /*
    * usage   : �Զ���������ϱ������������Զ�����
    *          
    * msg_id  : 0x00040386
    * target  : none
    * wparam  : 0
    * lparam  : 0
    * data_ptr: 
    * data_len: 
    */

#define SIP_MSG_AUTOTEST_MESS_CHECK               SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0086)


    /*
    * usage   : �ϱ�verizon vvm��Ϣ
    *          
    * msg_id  : 0x00040387
    * target  : none
    * wparam  : line id
    * lparam  : msg type��1-nm msg��2-mbu msg��
    * data_ptr: ���Ϊnm��Ϣ����ôΪsip_vvm_nm_t;���Ϊmbu��Ϣ����ôΪsip_vvm_mbu_t
    * data_len: sizeof(sip_vvm_nm_t)/sizeof(sip_vvm_mbu_t)
    */

#define SIP_MSG_VERIZON_VVM                       SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0087)

/* ���Լ���֧��������Ϣ������ʹ������δʹ�õ�ID 0x00040304-0x0004030a��0x0004031a-0x0004031f,0x00040380 */

/* ����֧�ַ�֧�����ӿ���ϢԤ�� 0x080->0x0AF��Lync Ԥ�� 0x050->0x07F
   ������Ŀ����ʱ������ʵ����Ҫ���з�֧ͬ����������Ԥ����Χ */



#endif/* __SIP_MSG_DEFINE_xxx */

#ifndef __SIP_CMD_DEFINE_xxx
/*
 *------------------------------------
 *      notify for CMD report
 *------------------------------------
 */
    /*
    * usage   : ���������ͨ��
    *          
    * msg_id  : 0x00040401
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: params(sip_ext_params_t*)
    * data_len: none
    * UI���������Ϣʱ�����ȡ��ʾ����Ϣ������ͨ��sip_ext_params_get��ȡ��Ȼ�����ֱ�ӵ���sip_call_invite��callee��������ΪNULL��paramsֱ��ʹ��
    */
#define SIP_CMD_CALL_OUT                        SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0001)
    /*
    * usage   : ��Ӧ
    *          
    * msg_id  : 0x00040402
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_common_info_t
    * data_len: sizeof(sip_cmd_common_info_t)
    */
#define SIP_CMD_CALL_RESPONSE                   SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0002)
    /*
    * usage   : �ܽ�
    *          
    * msg_id  : 0x00040403
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_common_info_t
    * data_len: sizeof(sip_cmd_common_info_t)
    */
#define SIP_CMD_CALL_REJECT                     SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0003)
    /*
    * usage   : �������ǰͨ��
    *          
    * msg_id  : 0x00040404
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_common_info_t
    * data_len: sizeof(sip_cmd_common_info_t)
    */
#define SIP_CMD_CALL_ACCEPT                     SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0004)
    /*
    * usage   : ����ͨ��
    *          
    * msg_id  : 0x00040405
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_common_info_t
    * data_len: sizeof(sip_cmd_common_info_t)
    */
#define SIP_CMD_CALL_HANGUP                     SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0005)
    /*
    * usage   : HOLD��ǰͨ��
    *          
    * msg_id  : 0x00040406
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_common_info_t
    * data_len: sizeof(sip_cmd_common_info_t)
    */
#define SIP_CMD_CALL_HOLD                       SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0006)
    /*
    * usage   : ��HOLD��ǰͨ��
    *          
    * msg_id  : 0x00040407
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_common_info_t
    * data_len: sizeof(sip_cmd_common_info_t)
    */
#define SIP_CMD_CALL_RESUME                     SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0007)
    /*
    * usage   : Ҫ��UI����DTMF
    *          
    * msg_id  : 0x00040408
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_common_info_t
    * data_len: sizeof(sip_cmd_common_info_t)
    */
#define SIP_CMD_CALL_DTMF                       SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0008)
    /*
    * usage   : ͨ��ǰת
    *          
    * msg_id  : 0x00040409
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_call_forward_info_t
    * data_len: sizeof(sip_cmd_call_forward_info_t)
    */
#define SIP_CMD_CALL_FORWARD                    SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0009)
    /*
    * usage   : ͨ����ת
    *          
    * msg_id  : 0x0004040a
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_call_transfer_info_t
    * data_len: sizeof(sip_cmd_call_transfer_info_t)
    */
#define SIP_CMD_CALL_TRANSFER                   SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x000a)
    /*
    * usage   : �������
    *          
    * msg_id  : 0x0004040b
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: NULL
    * data_len: 0
    */
#define SIP_CMD_CALL_CONFERENCE                 SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x000b)

    /*
    * usage   : ֪ͨUI����Reload�ʺ�
    *          
    * msg_id  : 0x00040410
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_cmd_reload_account_info_t
    * data_len: sizeof(sip_cmd_reload_account_info_t)
    */

#define SIP_CMD_RELOAD_ACCOUNT                  SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0010)

    /*
    * usage   : ֪ͨUI���AUTOP���и��»���ǿ����������
    *          
    * msg_id  : 0x00040411
    * target  : none
    * wparam  : line_id
    * lparam  : 0: check autop  1: check autop by cfg file  2: reboot and autop
    * data_ptr: NULL
    * data_len: 0
    */
    #define SIP_CHECK_AUTOP                       0
    #define SIP_CHECK_AUTOP_BY_CFG_FILE           1
    #define SIP_REBOOT_AND_CHECK_AUTOP            2

#define SIP_CMD_CHECK_SYNC                      SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0011)

    /*
    * usage   : ֪ͨUI��������
    *          
    * msg_id  : 0x00040412
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_cmd_reboot_phone_info_t
    * data_len: sizeof(sip_cmd_reboot_phone_info_t)
    */

#define SIP_CMD_REBOOT_PHONE                    SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0012)

    /*
    * usage   : ����ACTION URI��body���ݸ�UI
    *          
    * msg_id  : 0x00040413
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: core_body_t
    * data_len: 
    */
#define SIP_CMD_PASS_ACTION_URI                 SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0013)

    /*
    * usage   : ����ACTION URI��body���ݸ�UI
    *          
    * msg_id  : 0x00040414
    * target  : none
    * wparam  : line_id
    * lparam  : option_type
    * data_ptr: data
    * data_len: 
    */
	#define SIP_OPTION_SET_LYNC_PRESENCE_STATE		1	/* data_ptr:sip_reason_info_t */
	#define SIP_OPTION_SET_LYNC_CONTACT_CARD		2	/* data_ptr: */
	#define SIP_OPTION_SET_LYNC_NOTE				3	/* data_ptr:char* */
    #define SIP_OPTION_SET_LYNC_FORWARD				4	/* data_ptr:char* */

#define SIP_CMD_SET_OPTION						SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0014)

/*
    * usage   : ��ʼPCAP������Ϣ��ȡ
    *          
    * msg_id  : 0x00040415
    * target  : none
    * wparam  : line_id
    * lparam  : 
    * data_ptr: 
    * data_len: 
    */
#define SIP_CMD_SET_PCAP_START                  SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0015)

/*
    * usage   : ֹͣPCAP������Ϣ��ȡ
    *          
    * msg_id  : 0x00040416
    * target  : none
    * wparam  : line_id
    * lparam  : 
    * data_ptr: 
    * data_len: 
    */
#define SIP_CMD_SET_PCAP_STOP                   SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0016)

    /*
    * usage   : ����ɾ����Ա
    *          
    * msg_id  : 0x00040417
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_call_conference_info_t
                Lync�Զ�������control_protocol_type = SIP_CONTROL_FROM_LYNC_AUTOMATION_TEST
                              mode   Ԥ��
                              parter ΪҪɾ���ĳ�Աuri
    * data_len: sizeof(sip_cmd_call_conference_info_t)
    */
#define SIP_CMD_CONFERENCE_DEL_USER             SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0017)

    /*
    * usage   : ����mute��Ա
    *          
    * msg_id  : 0x00040418
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_call_conference_info_t
                Lync�Զ�������control_protocol_type = SIP_CONTROL_FROM_LYNC_AUTOMATION_TEST
                              mode   1:mute 0:umute
                              parter ΪҪmute/umte�ĳ�Աuri
    * data_len: sizeof(sip_cmd_call_conference_info_t)
    */
#define SIP_CMD_CONFERENCE_MUTE_USER            SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0018)

    /*
    * usage   : ����mute���Լ���������������Ա
    *          
    * msg_id  : 0x00040419
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: mute_type           1:mute 0:umute
    * data_len: sizeof(int)
    */
#define SIP_CMD_CONFERENCE_MUTE_AUDIENCE        SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0019)

    /*
    * usage   : ��ӵ�������������
    *          
    * msg_id  : 0x0004041a
    * target  : none
    * wparam  : line_id
    * lparam  : call_id,Lync�Զ�������Ϊ0��ʾ����meet now����
    * data_ptr: sip_cmd_call_conference_info_t
                Lync�Զ�������control_protocol_type = SIP_CONTROL_FROM_LYNC_AUTOMATION_TEST
                              mode   Ԥ��
                              parter ΪҪ����ĵ�����,Lync�Զ�������Ϊ�ձ�ʾ����meet now����
    * data_len: sizeof(sip_cmd_call_conference_info_t)
    */
#define SIP_CMD_CREAT_CONFERENCE                SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x001a)



    /*
    * usage   : ������ӳ�Ա
    *          
    * msg_id  : 0x0004041b
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_call_conference_info_t
                Lync�Զ�������control_protocol_type = SIP_CONTROL_FROM_LYNC_AUTOMATION_TEST
                              mode   Ԥ��
                              parter ΪҪ����ĳ�Աuri
    * data_len: sizeof(sip_cmd_call_conference_info_t)
    */
#define SIP_CMD_CONFERENCE_ADD_USER             SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x001b)


    /*
    * usage   : park��ǰͨ��
    *
    * msg_id  : 0x0004041c
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: NULL
    * data_len: 0
    */
#define SIP_CMD_CALL_PARK                      SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x001c)

    /*
    * usage   : retrieve parkͨ��
    *
    * msg_id  : 0x0004041d
    * target  : none
    * wparam  : line_id
    * lparam  : NULL
    * data_ptr: sip_cmd_park_info_t
                Lync�Զ�������control_protocol_type = SIP_CONTROL_FROM_LYNC_AUTOMATION_TEST
                mode      Ԥ��
                orbit     ��Ҫ������orbit�ַ���
    * data_len: sizeof(sip_cmd_park_info_t)
    */
#define SIP_CMD_CALL_PARK_RETRIEVE             SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x001d)

    /*
    * usage   : ͨ��mute��Ա
    *
    * msg_id  : 0x0004041e
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_call_conference_info_t
                Lync�Զ�������control_protocol_type = SIP_CONTROL_FROM_LYNC_AUTOMATION_TEST
                              mode   1:mute 0:umute
                              parter ΪҪmute/umte�ĳ�Աuri
    * data_len: sizeof(sip_cmd_call_conference_info_t)
    */
#define SIP_CMD_CALL_MUTE_USER                 SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x001e)


    /*
    * usage   : ֪ͨUI���û���
    *          
    * msg_id  : 0x0004041f
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: NULL
    * data_len: 0
    */

#define SIP_CMD_RESET_PHONE                    SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x001f)


/*
 *------------------------------------
 *      cmd ui operate, unrelated to sip
 *------------------------------------
 */

    /*
    * usage   : ֪ͨUI����
    *          
    * msg_id  : 0x00040440
    * target  : none
    * wparam  : line_id
    * lparam  : 
    * data_ptr: NULL
    * data_len: 0
    * Note:����Ϣ��ʱ������΢����Լܹ���Ҫ����ǰ��Ļ��ϢתΪxml��ʽ��������Ϣ��ʽ��Ҫ΢���ṩ��
    */
#define SIP_CMD_GET_UI_ELEMENTS                SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0040)

    /*
    * usage   : ֪ͨUI���������˵�
    *          
    * msg_id  : 0x00040441
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: NULL
    * data_len: 0
    */
#define SIP_CMD_NAVIGATE_TO_SCREEN             SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0041)

    /*
    * usage   : ��UI��ѯ״̬
    *          
    * msg_id  : 0x00040442
    * target  : none
    * wparam  : line_id
    * lparam  : �������
    * data_ptr: NULL
    * data_len: 0
    */
    #define SIP_PIN_LOCK_STATE		       1	/* �鿴pin����״̬,��ѯ���ͨ��sip_lync_report_ui_operate_reslut֪ͨsip��
                                                 * codecΪ0������״̬��1����״̬*/
    #define SIP_MWI_STATE                  2    /* �鿴MWI״̬,��ѯ���ͨ��sip_lync_report_ui_operate_reslut֪ͨsip��
                                                 * codecΪ0��off״̬��1��on״̬*/
#define SIP_CMD_DETECTE_UI_STATE               SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0042)


    /*
    * usage   : ��UI��ѯ��ϵ����Ϣ
    *          
    * msg_id  : 0x00040443
    * target  : none
    * wparam  : line_id
    * lparam  : ��ѯ����
    * data_ptr: (char *)contact_uri
    * data_len: strlen(contact_uri) + 1
    */
    #define SIP_LYNC_CONTACT_AVAILABILITY       1  /* ��ѯ���ͣ���ѯ���ͨ��sip_lync_ui_report_automation_operate_result֪ͨSIP */
    #define SIP_LYNC_CONTACT_ACTIVITYTOKEN      2 
    #define SIP_LYNC_CONTACT_NOTE               3 
    #define SIP_LYNC_SELF_AVAILABILITY          4 

#define SIP_CMD_QUERY_CONTACT_INFORMATION       SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0043)

    /*
    * usage   : ��UI��ȡcalendar����
    *          
    * msg_id  : 0x00040444
    * target  : none
    * wparam  : line_id
    * lparam  : ��ѯ����
    * data_ptr: (char *)organizer
    * data_len: strlen(organizer) + 1
    */
#define SIP_CMD_GET_CALENDAR_ITEM               SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0044)

    /*
    * usage   : ֪ͨUI�ϴ�syslog���������ͨ��sip_lync_ui_report_automation_operate_result֪ͨSIP
    *
    * msg_id  : 0x00040445
    * target  : none
    * wparam  : line_id
    * lparam  : time 0:��ʾ�����ϴ�log�ļ��� n:��ʾ��ʱn min���ϴ�log�ļ�
    * data_ptr: NULL
    * data_len: 0
    */
#define SIP_CMD_UPLOAD_SYSLOG                     SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0045)

    /*
    * usage   : ֪ͨUI����VoiceMail��صĲ���
    *
    * msg_id  : 0x00040046
    * target  : none
    * wparam  : line_id
    * lparam  : ��������
    * data_ptr: data_ptr=sip_cmd_voice_mail_info_t
    * data_len: sizeof(sip_cmd_voice_mail_info_t)
    */

    /* �������Ͷ��� */
    #define SIP_LYNC_CLEAR_VM_MESSAGES         1 /* �������ͨ��sip_lync_ui_report_automation_operate_result֪ͨSIP */
    #define SIP_LYNC_DELETE_VM_MESSAGE         2 /* �������ͨ��sip_lync_ui_report_automation_operate_result֪ͨSIP */
    #define SIP_LYNC_MARK_VM_AS_READ           3 /* �������ͨ��sip_lync_ui_report_automation_operate_result֪ͨSIP */
    #define SIP_LYNC_PLAY_VM_MESSAGE           4
    #define SIP_LYNC_CALL_VIA_VM_MESSAGE       5

#define SIP_CMD_VM_MESSAGE_OPERATION              SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0046)

    /*
    * usage   : ��UI��ȡVoiceMail������Ϣ
    *
    * msg_id  : 0x0004047
    * target  : none
    * wparam  : line_id
    * lparam  : ��ѯ����
    * data_ptr: NULL
    * data_len: 0
    */
    #define SIP_LYNC_GET_ALL_VM_ITEMS          1 /* ��ȡ���ͨ��sip_lync_ui_report_automation_operate_result��reason֪ͨSIP */
    #define SIP_LYNC_GET_UNREAD_VM_COUNT       2 /* ��ȡ���ͨ��sip_lync_ui_report_automation_operate_result��codec֪ͨSIP */
#define SIP_CMD_GET_VOICEMAIL_INFO        SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0047)



    /*
    * usage   : ֪ͨUI����PIN����
    *          
    * msg_id  : 0x00040450
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: pin(char *)
    * data_len: strlen(pin) + 1
    */
#define SIP_CMD_CONFIG_PIN                     SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0050)

    /*
    * usage   : ֪ͨUI�޸�PIN����
    *          
    * msg_id  : 0x00040451
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_cmd_chang_pin_lock_info_t
    * data_len: sizeof(sip_cmd_chang_pin_lock_info_t)
    */
#define SIP_CMD_CHANGE_PIN                     SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0051)

    /*
    * usage   : ֪ͨUI PIN��������
    *          
    * msg_id  : 0x00040452
    * target  : none
    * wparam  : line_id
    * lparam  : 1:lock, 0:unlock
    * data_ptr: if lparam == 1, data_ptr = NULL
                if lparam == 0, data_ptr = pin��
    * data_len: 
    */
#define SIP_CMD_LOCK_PIN                       SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0052)

    /*
    * usage   : �Զ�������֪ͨUI PIN��������
    *
    * msg_id  : 0x00040453
    * target  : none
    * wparam  : line_id
    * lparam  : 1:lock, 0:unlock
    * data_ptr: if lparam == 1, data_ptr = NULL
                if lparam == 0, data_ptr = pin��
    * data_len:
    */
#define SIP_CMD_ATT_LOCK_PIN                       SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0053)

    /*
    * usage   : ֪ͨUI����豸����
    *          
    * msg_id  : 0x00040460
    * target  : none
    * wparam  : line_id
    * lparam  : none
    * data_ptr: NULL
    * data_len: 0
    */
#define SIP_CMD_CHECK_DEVICE_UPDATE            SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0060)


    /*
    * usage   : ֪ͨUI��ʾϵͳ����
    *
    * msg_id  : 0x00040461
    * target  : none
    * wparam  : none
    * lparam  : none
    * data_ptr: notice xml������
    * data_len: strlen(notice xml)+1
    */
#define SIP_CMD_SYSTEM_NOTICE            SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0061)


/* ����֧�ַ�֧�����ӿ���ϢԤ�� 0x070->0x08F��Lync Ԥ�� 0x000->0x06F
   ������Ŀ����ʱ������ʵ����Ҫ���з�֧ͬ����������Ԥ����Χ */

/* Lync������Ϣʱ��������ʹ������δʹ�õ�ID
0x0004040b-0x0004040f
0x0004041f-0x0004043f
0x00040448-0x0004044f
0x00040454-0x0004045f
0x00040462-0x000404ff
*/

/************************************************************************/
/*                   ���Լ���֧�� msg id 0x070->0x08F start             */
/************************************************************************/

//#define SIP_CMD_XXX                 SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0070)

/************************************************************************/
/*                   ���Լ���֧�� 0x070->0x08F end                      */
/************************************************************************/


#endif/* __SIP_CMD_DEFINE_xxx */

#ifndef __SIP_REQUIRE_DEFINE_xxx
/*
 *------------------------------------
 *      notify for require
 *------------------------------------
 */
    /*
    * usage   : ��ȡ��չ̨����
    *          
    * msg_id  : 0x0601
    * target  : none
    * wparam  : line_id
    * lparam  : reserved
    * data_ptr: none
    * data_len: none
    */
#define SIP_REQUIRE_GET_EXP                      SIP_S2C_MSG(SIP_MSG_S2C_REQ, 0x0001)

    /*
    * usage   : ��ȡһЩ����ֵ
    *          
    * msg_id  : 0x0602
    * target  : none
    * wparam  : line_id
    * lparam  : option_type
    * data_ptr: data
    * data_len: none
    */
	#define SIP_OPTION_GET_LYNC_PRESENCE_STATE		1	/* sip_reason_info_t */
	#define SIP_OPTION_GET_LYNC_CONTACT_CARD		2	/*  */
	#define SIP_OPTION_GET_LYNC_NOTE				3	/* string */

#define SIP_REQUIRE_GET_OPTION					 SIP_S2C_MSG(SIP_MSG_S2C_REQ, 0x0002)


#endif/* __SIP_REQUIRE_DEFINE_xxx */

#ifdef __cplusplus
}
#endif

#endif/* __SERVICE_SIP_INTERFACE_H__ */

