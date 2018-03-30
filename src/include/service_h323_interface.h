/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen  Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:
*
*******************************************************************/
#ifndef __SERVICE_H323_INTERFACE__
#define __SERVICE_H323_INTERFACE__

#include "service_h323_define.h"
#include "service_h323_message.h"


#define H323_INTERFACE_VERSION              "1.0.0.0"
/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __H323_BASIC_API_EXPORT_xxx__
/*
 *------------------------------------
 *  服务基本API
 *------------------------------------
 */

/**
* @brief : 初始化参数句柄
* @param : params       参数句柄的地址
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark:
*/
H323_DLL_DECLARE int h323_ext_params_init(h323_ext_params_t ** params);

/**
* @brief : 往参数句柄里添加参数
* @param : params       参数句柄的地址
* @param : type         参数类型
* @param : buf          参数内存的起始地址
* @param : type         参数的内存长度
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 因为参数过程中可能会调整内存大小，所以params类型为（h323_ext_params_t**）
*/
H323_DLL_DECLARE int h323_ext_params_add(h323_ext_params_t ** params, int type, void * buf,
        int buf_len);

/**
* @brief : 克隆参数列表
* @param : dst          目的参数句柄的地址
* @param : src          源参数列表
* @return:
* @remark: 克隆出来的参数列表，也要使用h323_ext_params_free释放
*/
H323_DLL_DECLARE int h323_ext_params_clone(h323_ext_params_t ** dst, h323_ext_params_t * src);

/**
* @brief : 通过参数类型提取参数
* @param : params       参数句柄的地址
* @param : type         参数类型
* @param : data_len     参数实际长度，如果不需要知道参数长度，传NULL
* @return: 非NULL，为取到参数的地址，可以直接读取，NULL，表示没有读取到参数
* @remark:
*/
H323_DLL_DECLARE void * h323_ext_params_get(h323_ext_params_t * params, int type, int * data_len);

/**
* @brief : 删除参数列表
* @param : params
* @param :
* @return:
* @remark:
*/
H323_DLL_DECLARE int h323_ext_params_del(h323_ext_params_t * params, int type);

/**
* @brief : 释放参数列表
* @param : params       参数句柄的地址
* @return:
* @remark:
*/
H323_DLL_DECLARE int h323_ext_params_free(h323_ext_params_t ** params);

/**
* @brief : get H323 service version
* @param : type  [IN][INT]       version type
* @param : buf   [OUT][string]   output version buf
* @param : size  [IN][U_INT]     version buf size
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
#define H323_API_VERSION        0
#define H323_SERVER_VERSION     1

H323_DLL_DECLARE int h323_service_get_version(mkit_handle_t caller, int type, char * buf,
        unsigned int size);

/**
* @brief : H323 service init
* @param :
* @param :
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_service_init(mkit_handle_t caller);

/**
* @brief : H323 service start
* @param :
* @param :
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_service_start(mkit_handle_t caller);

/**
* @brief : H323 service stop
* @param :
* @param :
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_service_stop(mkit_handle_t caller);

/**
* @brief : H323 service exit
* @param :
* @param :
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_service_exit(mkit_handle_t caller);


/**
* @brief :get H323 service option
* @param :option    [IN][INT]       option ID
* @param :buf       [OUT][VOID]     output buf
* @param :size      [IN][U_INT]     buf size
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_service_get_option(mkit_handle_t caller, int option, void * buf,
        unsigned int size);

#define H323_SERVICE_OPT_NONE                0
#define H323_SERVICE_OPT_VIDEO_CAP       1
/*
* 设置网络参数：value:h323_network_t, size:sizeof(h323_network_t)
* 注意：这个必须在h323_service_start前调用，如果网络有变更需重新设置，并在设置后调用网络变更h323_network_change()
*/
#define H323_SERVICE_OPT_NETWORK_INFO    2

/**
* @brief :set H323 service option
* @param :option    [IN][INT]       option ID
* @param :buf       [OUT][VOID]     output buf
* @param :size      [IN][U_INT]     buf size
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_service_set_option(mkit_handle_t caller, int option, void * value,
        unsigned int size);

/**
* @brief : request Notify
* @param : evt      [IN][INT] request notify type
* @param :
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @mark  : now only support H323_REQUEST_NOTIFY_ALL
*/
#define H323_REQUEST_NOTIFY_ALL            -1
#define H323_REQUEST_NOTIFY_REG            (1<<0)
#define H323_REQUEST_NOTIFY_CALL           (1<<1)
#define H323_REQUEST_NOTIFY_SUB            (1<<2)
#define H323_REQUEST_NOTIFY_MSG            (1<<3)
H323_DLL_DECLARE int h323_request_notify(mkit_handle_t caller, int evt);

/**
* @brief : cancel Notify
* @param : evt      [IN][INT] cancel notify type
* @param : lparam   [IN][INT] reserve
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @mark  : now only support H323_REQUEST_NOTIFY_ALL
*/
H323_DLL_DECLARE int h323_cancel_notify(mkit_handle_t caller, int evt);

#endif/* __H323_BASIC_API_EXPORT_xxx__ */


#ifndef __H323_REG_API_EXPORT_xxx__

/*
 *------------------------------------
 *  注册相关API
 *------------------------------------
 */

/**
* @brief : query H323 register status
* @param :
* @param :
* @return register status
*/
#define H323_REG_NONE       0
#define H323_REG_REGING     1
#define H323_REG_REGED      2
#define H323_REG_FAILED     3
#define H323_REG_UNREGING   4
#define H323_REG_UNREGED    5
H323_DLL_DECLARE int h323_query_reg_status(mkit_handle_t caller);

#endif/* __H323_REG_API_EXPORT_xxx__ */


#ifndef __H323_CALL_API_EXPORT_xxx__

/*
 *------------------------------------
 *  通话相关API
 *------------------------------------
 */

/**
* @brief : new a call id
* @param :
* @param :
* @return:call id
*/
H323_DLL_DECLARE int h323_call_new_cid(mkit_handle_t caller);

/**
* @brief : H323 make call out
* @param : cid              [IN][INT]   call id(get by h323_call_new_cid())
* @param : call_mask        [IN][INT]   0<<1,audio  1<<1,video
* @param : bandwidth        [IN][INT]   bandwith(-1:use default)
* @param : callee           [IN][string]callee string
* @param : h264param        [IN][struct]video param(defult:NULL,will use by config file)
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_invite(mkit_handle_t caller, int cid, int call_mask, int bandwidth,
                                      char * callee,
                                      h323_h264_param_t * h264param);

/**
* @brief : H323 make call out
* @param : cid              [IN][INT]   call id(get by h323_call_new_cid())
* @param : callee           [IN][string]callee string
* @param : params        [IN][struct] 其它额外可扩展参数，如H323_MAKE_CALL_H264_PARAM
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_invite2(mkit_handle_t caller, int cid, h323_ext_params_t * params);

/**
* @brief : send ringing for incoming call
* @param : cid              [IN][INT]   call id
* @param
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_ringing(mkit_handle_t caller, int cid);

/**
* @brief : hungup current call
* @param : cid              [IN][INT]   call id
* @param
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_hangup(mkit_handle_t caller, int cid);

/**
* @brief : hungup current call
* @param : cid              [IN][INT]   call id
* @param
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_hangup2(mkit_handle_t caller, int cid,
                                       h323_call_end_reason_code_e reason);
/**
* @brief : send response for incoming call
* @param : cid              [IN][INT]   call id
* @param : response_code    [IN][INT]   response code(such as 180/486/480)
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_response(mkit_handle_t caller, int cid, int reponse_code);

/**
* @brief : enalbe/disable video for call
*          (this API must use before ANSWER or RE_NEGOTIATE call)
* @param : cid              [IN][INT]   call id
* @param : enable           [IN][INT]   0,disable  1,enable
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_set_video_media(mkit_handle_t caller, int cid, int enable);

/**
* @brief : set call bandwidth
*          (this API must use before ANSWER or RE_NEGOTIATE call)
* @param : cid              [IN][INT]   call id
* @param : bandwidth        [IN][INT]   bandwidth
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_set_bandwidth(mkit_handle_t caller, int cid, int bandwidth);

/**
* @brief : set h264 video param
*          (this API must use before ANSWER or RE_NEGOTIATE call)
* @param : cid              [IN][INT]   call id
* @param : param            [IN][STRUCT]h264 param (default is 1080P)
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_set_h264_param(mkit_handle_t caller, int cid,
        h323_h264_param_t * param);

/**
* @brief : set video param，呼叫接起或重协商时设置视频相关参数(带宽、接起类型、是否关闭239、主辅流分辨率等)
*          (this API must use before ANSWER or RE_NEGOTIATE call)
* @param : cid              [IN][INT]   call id
* @param : param            [IN][STRUCT]额外参数，如H323_MAKE_CALL_H264_PARAM
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_set_call_param(mkit_handle_t caller, int cid,
        h323_ext_params_t * params);
/**
* @brief : answer cuurent call
* @param : cid              [IN][INT]   call id
* @param :
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_accept(mkit_handle_t caller, int cid);

/**
* @brief : reject current call
* @param : cid              [IN][INT]   call id
* @param :
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_reject(mkit_handle_t caller, int cid);

/**
* @brief : renegotiate call(use by multi video call)
*          (call it after h323_call_set_bandwidth()、h323_call_set_h264_param() function)
* @param : cid              [IN][INT]   call id
* @param :
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_renegotiate(mkit_handle_t caller, int cid);


/**
* @brief : send DTMF for current call
* @param : cid              [IN][INT]   call id
* @param : dtmf             [IN][STRING]DTMF string
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_send_dtmf(mkit_handle_t caller, int cid, const char * dtmf);

/**
* @brief : notify remote endpoint that local mute
* @param : cid              [IN][INT]   call id
* @param :
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_local_mute(mkit_handle_t caller, int cid);

/**
* @brief : notify remote endpoint that local un mute
* @param : cid              [IN][INT]   call id
* @param :
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_local_un_mute(mkit_handle_t caller, int cid);


/**
* @brief : contral remote camera action start
* @param : cid              [IN][INT]   call id
* @param : act              [IN][INT]   action event
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_camera_action_start(mkit_handle_t caller, int cid,
        h323_camare_action_e action);

/**
* @brief : control remote camera action stop
* @param : cid              [IN][INT]   call id
* @param :
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_camera_action_stop(mkit_handle_t caller, int cid);

/**
* @brief : control remote camera preset activate
* @param : cid              [IN][INT]   call id
* @param : preset_num       [IN][INT]   preset number
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_camera_preset_activate(mkit_handle_t caller, int cid,
        int preset_num);

/**
* @brief  control remote camera preset store
* @param : cid              [IN][INT]   call id
* @param : preset_num       [IN][INT]   preset number
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_camere_preset_store(mkit_handle_t caller, int cid, int preset_num);

/**
* @brief : open local ext video
* @param : cid              [IN][INT]   call id
* @param :
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_open_ext_video(mkit_handle_t caller, int cid);

/**
* @brief  close local ext video
* @param : cid              [IN][INT]   call id
* @param :
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_close_ext_video(mkit_handle_t caller, int cid);

/**
* @brief  sync video frame
* @param : cid              [IN][INT]   call id
* @param : is_ext           [IN][INT]   0:master video   1:ext video
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen

*/
H323_DLL_DECLARE int h323_call_video_frame_sync(mkit_handle_t caller, int cid, int is_ext);

/**
* @brief : bandwidth control
* @param : cid              [IN][INT]   call id
* @param : flow_para        [IN][STRUCT]current channel and bandwidth
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_flow_ctrl_rate(mkit_handle_t caller, int cid,
        h323_flow_ctrl_para_t * flow_para);

/**
* @brief : 等效于H323_CALL_UPDATE_REMOTE_USER_AGENT，VC400每路codec均需上报，增加此接口兼容
* @param : cid
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_get_remote_useragent(mkit_handle_t caller, int cid, char * remoter,
        int size);

/**
* @brief : 扩展接口，用于向对端发送自定义xml文本信息
* @param : cid
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_call_send_user_custom_info(mkit_handle_t caller, int cid, char * info,
        int info_size);

#endif/* __H323_CALL_API_EXPORT_xxx__ */




#ifdef __cplusplus
}
#endif



#endif



