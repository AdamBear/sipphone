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
*    修改时间:2015-06-12
*    修改者:zxh
*    原版本号:1.0.0.0
*    新版本号:1.0.0.1
*    修改问题:1、sip_service_get_version接口实现可以获取interface和sip版本
*             2、增加API：sip_call_get_info2
*
*    新版本号:1.0.0.2
*    修改问题:1、增加sip_call_reject_with_reason接口
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
 *      系统相关APIs
 *------------------------------------
 */
#ifndef __SIP_API_DEFINE_xxx
/**
* @brief : get sip server versin
* @param : 
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 返回的结果示例格式如下
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
* 设置网络参数：value:sip_network_info_t, size:sizeof(sip_network_info_t) 
* 注意：这个必须在sip_service_start前调用，如果网络有变更需重新设置，并在设置后调用网络变更sip_network_change()
*/
#define SIP_SERVICE_OPT_NETWORK_INFO    2

    /*
    * 该选项一般在自动化测试RPC时才需要使用到，因为RPC无法使用文件进行传输，所以需要统一使用MSG进行传输
    * 大数据时IPC的方式，0：默认方式，1：强制使用MSG，2：强制使用文件
    * SIP_SERVICE_BIG_DATA_IPC_METHOD 的参数值取值
    */
    #define SIP_BIG_DATA_IPC_DEFAULT        0/* 默认使用的方式，根据不通的场景选择MSG或者FILE */
    #define SIP_BIG_DATA_IPC_FORCE_IN_MSG   1
    #define SIP_BIG_DATA_IPC_FORCE_IN_FILE  2

#define SIP_SERVICE_BIG_DATA_IPC_METHOD 3

    /*设置读配置的方式
    * 1. IOS/ANDROID/WIN Softphone 采用UI提供的V80interface.lib库获取配置，接口复用M7。
    * 2. V80中性话机默认采用ETL，T49、T5x 通过识别dbx来确认是否使用M7方式。
    * 3.自动化测试默认使用INI方式。
    */
    #define SIP_READ_CFG_USE_CFGSERVER  0
    #define SIP_READ_CFG_USE_INI                 1
    #define SIP_READ_CFG_USE_ETL                2 /*Windows不再使用此方式，统一改为INI读取，使用CONFIG_ACCESS_USE_ETL_LIB控制*/
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
* @brief : 获取lync状态
* @param : line_id           [IN][0~32]: line id，账号ID
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
* @brief : 读取sip_map_t数据前必须先调用本函数
* @param : dest      [IN][OUT]    目标数据
* @return: none
* @remark: 重新计算各个sip_map_t成员ptr的正确地址（映射到map_data）,之后就可以直接使用里面的数据
*/
SIP_DLL_DECLARE int sip_map_info_recover(void *dest, void *src, int method);
SIP_DLL_DECLARE void sip_map_info_dump(void *dest, int method);
/*
 *------------------------------------
 *      注册相关APIs
 *------------------------------------
 */

/**
* @brief : 获取指定账号注册状态
* @param : line_id      [IN][0~32]      line id，账号ID
* @return: < 0 means failed
* @return: >= 0 means register status(sip_register_status_e)
* @remark: 
*/
SIP_DLL_DECLARE int sip_line_get_state(mkit_handle_t caller, int line_id);

/**
* @brief : device pairing登录
* @param : line_id           [IN][0~32]: line id，账号ID
* @param : type              [IN][0~2] : 0 注册，1 注销 ，2 用户取消注册
* @param : country_code      国家代码
* @return: < 0 means failed
* @remark: 
*/
SIP_DLL_DECLARE int sip_device_pairing_signin(mkit_handle_t caller, int line_id, int type, char *country_code);

/**
* @brief : 取消指定账号登录
* @param : line_id           [IN][0~32]: line id，账号ID
* @param : type              [IN]保留
* @return: < 0 means failed
* @remark: 
*/
SIP_DLL_DECLARE int sip_line_cancel_signin(mkit_handle_t caller, int line_id, int type);

/*
 *------------------------------------
 *      基本通话API
 *------------------------------------
 */

/**
* @brief : 获取新的call_id
* @return: <= 0 means failed
* @return: > 0 means new call_id
* @remark: GUI使用通话功能前，都需要先让SIP分配一个call_id
*/
SIP_DLL_DECLARE int sip_call_new_call_id(mkit_handle_t caller);


/**
* @brief : 初始化参数句柄
* @param : params       参数句柄的地址
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_ext_params_init(sip_ext_params_t** params);

/**
* @brief : 往参数句柄里添加参数
* @param : params       参数句柄的地址
* @param : type         参数类型
* @param : buf          参数内存的起始地址
* @param : type         参数的内存长度
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 因为参数过程中可能会调整内存大小，所以params类型为（sip_ext_params_t**）
*/
SIP_DLL_DECLARE int sip_ext_params_add(sip_ext_params_t** params, int type, void* buf, int buf_len);

/**
* @brief : 通过参数类型提取参数
* @param : params       参数句柄的地址
* @param : type         参数类型
* @param : data_len     参数实际长度，如果不需要知道参数长度，传NULL
* @return: 非NULL，为取到参数的地址，可以直接读取，NULL，表示没有读取到参数
* @remark: 
*/
SIP_DLL_DECLARE void* sip_ext_params_get(sip_ext_params_t* params, int type, int* data_len);

/**
* @brief : 克隆参数列表
* @param : dst          目的参数句柄的地址
* @param : src          源参数列表
* @return: 
* @remark: 克隆出来的参数列表，也要使用sip_ext_params_free释放
*/
SIP_DLL_DECLARE int sip_ext_params_clone(sip_ext_params_t** dst, sip_ext_params_t* src);

/**
* @brief : 删除参数列表
* @param : params
* @param : 
* @return: 
* @remark: 
*/
SIP_DLL_DECLARE int sip_ext_params_del(sip_ext_params_t* params, int type);

/**
* @brief : 释放参数列表
* @param : params       参数句柄的地址
* @return: 
* @remark: 
*/
SIP_DLL_DECLARE int sip_ext_params_free(sip_ext_params_t** params);

/**
* @brief : 发起VoIP呼叫
* @param : line_id      [IN][0~32]      账号ID
* @param : call_id      [IN][1~int]     新会话ID，用于识别该路通话
* @param : call_mask    [IN][0~int]     呼叫选项，如开启视频等
* @param : called       [IN][64]        呼叫号码或地址（123或者sip:123@10.2.1.199）
* @param : params                       其它额外可扩展参数，如：SCA index、Pickup index等等
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_invite(mkit_handle_t caller, int line_id, int call_id, int call_mask, const char* callee, sip_ext_params_t* params);


/**
* @brief : 释放通话
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @param : reason       [IN][64]        可选，不为NULL时，用于告知对方结束通话原因
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_release(mkit_handle_t caller, int line_id, int call_id, const char *reason);


/**
* @brief : 发送响应
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @param : status_code  [IN][0~int]     返回码(180/183等)
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_response(mkit_handle_t caller, int line_id, int call_id, int response_code);

/**
* @brief : 发送带appearance_index的响应
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @param : status_code  [IN][0~int]     返回码(180/183等)
* @param ：appearance_index [IN][0~32]  线路ID
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_response_with_appearance_index(mkit_handle_t caller, int line_id, int call_id, int response_code, int appearance_index);

/**
* @brief : 被叫接听
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @param : call_type    [IN][0~1]       呼叫类型（音频/视频）
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 接起结果通过SIP_ON_CALL_ANSWERED上报
*/
SIP_DLL_DECLARE int sip_call_accept(mkit_handle_t caller, int line_id, int call_id, int call_type);

/**
* @brief : 被叫拒绝接听
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @param : status_code  [IN][0~int]     sip_reason_code_e,拒绝返回码,如486/480/404
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_reject(mkit_handle_t caller, int line_id, int call_id, int status_code);

/**
* @brief : 被叫拒绝接听（带原因）
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @param : status_code  [IN][0~int]     sip_reason_code_e,拒绝返回码,如486/480/404
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_reject_with_reason(mkit_handle_t caller, int line_id, int call_id, int status_code, const char *reason);

/**
* @brief : 暂停（HOLD一路通话）
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: HOLD结果通过SIP_ON_CALL_PAUSED上报
*/
SIP_DLL_DECLARE int sip_call_hold(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : 恢复挂起的通话
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 解HOLD结果通过SIP_ON_CALL_UPDATE上报
*/
SIP_DLL_DECLARE int sip_call_resume(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : reinvite回应
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @param : tid          [IN][1~int]     事务ID
* @param ：code         [IN][1~int]     回应码
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
SIP_DLL_DECLARE int sip_call_reinvite_response(mkit_handle_t caller, int line_id, int call_id, int tid, int code);

/**
* @brief : 停止发送视频流（通话被HOLD或则HELD时不能调用此接口）
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
SIP_DLL_DECLARE int sip_call_stop_shareling_video(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : 恢复发送视频流
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
SIP_DLL_DECLARE int sip_call_start_shareling_video(mkit_handle_t caller, int line_id, int call_id);

/*
 *------------------------------------
 *      高级通话功能API
 *------------------------------------
 */


/**
* @brief : broadworks call recording operation
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @param : op           [IN][0~int]     start,stop,pause,resume
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 需要等call recording操作结果返回才能进行下一个操作
*/
#define SIP_BW_CALL_RECORD_OP_START     0
#define SIP_BW_CALL_RECORD_OP_STOP      1
#define SIP_BW_CALL_RECORD_OP_PAUSE     2
#define SIP_BW_CALL_RECORD_OP_RESUME    3
#define SIP_BW_CALL_RECORD_OP_MAX       4

SIP_DLL_DECLARE int sip_bw_call_recording(mkit_handle_t caller, int line_id, int call_id, int op);



/**
* @brief : 通话前转，将来电通过302转给其他号码
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @param : type         [IN][0~3]       forward类型，当前会根据类型在SIP信令里增加Diversion头域
* @param : forward_to   [IN][64]        目标号码或地址
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 对于type，比如是SIP_FORWARD_TYPE_ALWAYS_FORWARD，那么SIP信令增加的头域类似为：
*          Diversion: "abc" <sip:123@1.2.3.4>;reason=unconditional
*/
#define SIP_FORWARD_TYPE_NORMAL             0   /* 对应reason为deflection */
#define SIP_FORWARD_TYPE_ALWAYS_FORWARD     1   /* 对应reason为unconditional */
#define SIP_FORWARD_TYPE_BUSY_FORWARD       2   /* 对应reason为user-busy */
#define SIP_FORWARD_TYPE_NO_ANSWER_FORWARD  3   /* 对应reason为no-answer */

SIP_DLL_DECLARE int sip_call_forward(mkit_handle_t caller, int line_id, int call_id, int type, const char *forward_to);


/*
* @brief : 通话前转，将来电通过302转给其他号码,并且带有reason原因，主要解决场景为VCD在会议中，来电时将通过302邀请对方加入会议
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @param : forward_to   [IN][64]        目标号码或地址
* @param : reason:      [IN][64]        Reason头域值--邀请权限码
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 对于VCD注册Apollo Primise账号，在会议中，对方来电，带Reason头域，服务器检测Reason判断权限是否有权邀请对方入会
*          Reason:APOLLO;XXXXXXXX
*/

SIP_DLL_DECLARE int sip_call_forward2(mkit_handle_t caller, int line_id, int call_id, const char *forward_to, const char *reason); 

/**
* @brief : 将通话park到服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @param : type         [IN][0~3]       park类型
* @param : park_to   [IN][64]           目标号码或地址
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
* @brief : 通话中转移通话
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     旧通话ID，即将被替换的那路通话
* @param : target_callid[IN][0~int]  新通话ID，为0则表示盲转
* @param : refer_to     [IN][64]        目标号码或地址
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 通话过程中，把一路通话转到其他账户
*          转移结果通过SIP_ON_CALL_TRANSFERED上报
*/
SIP_DLL_DECLARE int sip_call_transfer(mkit_handle_t caller, int line_id, int call_id, int target_callid, const char *refer_to);

/**
* @brief : 刷新通话（RE-INVITE）
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @param : type         [IN][0~int]     如何RE-INVITE
* @PARAM : arg          [IN][]          根据type值确定扩展参数，默认为NULL（扩展，暂未使用）
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
* @brief : 建立会议，即呼叫配置的conference服务器地址
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     新会话ID，用于呼叫conf_uri
* @param : call_id_cnt  [IN][1~int]     call_id_tab 个数
* @param : call_id_tab  [IN][1~int]     int数组，当前要conf的其他路call_id，最多SIP_MAX_CONF_LINE路通话
* @param : conf_uri     [IN][64]        用于指定conf目标号码或地址，默认为NULL，表示从配置获取
* @param : params                       其它额外可扩展参数，如：SCA index、Pickup index等等
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 与conf服务器建立通话后，再将原通话refer到conf服务器
*/
SIP_DLL_DECLARE int sip_call_conference(mkit_handle_t caller, int line_id, int call_id, int call_mask,
                        int call_id_cnt, int callid_tab[SIP_MAX_CONF_LINE], const char *conf_uri, sip_ext_params_t* params);

/**
* @brief : 会议中添加新成员（默认方式）
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : conf_callid  [IN][0~int]     conf会议通话ID
* @param : target       [IN][64]        新加入通话的CID
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 适用于Huawei ATS服务器
*/
SIP_DLL_DECLARE int sip_call_conference_ats_add_user(mkit_handle_t caller, int line_id, int conf_callid, const char *target);
SIP_DLL_DECLARE int sip_call_conference_ats_remove_user(mkit_handle_t caller, int line_id, int conf_callid, const char *target);

/**
* @brief : 会议中添加新成员（用于huawei IP Centrex类服务器）
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @param : conf_callid  [IN][0~int]     conf会议通话ID
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 适用于Broadsoft和Huawei IP Centrex服务器
*/
SIP_DLL_DECLARE int sip_call_conference_add_user(mkit_handle_t caller, int line_id, int call_id, int conf_callid);


/**
* @brief : 会议中主持方踢人
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : conf_cid     [IN][1~int]     会议通话ID
* @param : user         [IN][string]    会议订阅信息中的user节点的entity属性
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 适用于Broadsoft服务器
*/
SIP_DLL_DECLARE int sip_call_conference_del_user(mkit_handle_t caller, int line_id, int conf_cid, const char* user);
/**
* @brief : BLA hold
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_draft_bla_public_hold(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : BLA resume
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_draft_bla_public_resume(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : draft bla占线
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : appearance   [IN][64]        apearance index
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_draft_bla_seize_line(mkit_handle_t caller, int line_id, int appearance_index);

/**
* @brief : draft bla取消占线
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : appearance   [IN][64]        apearance index
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_draft_bla_unseize_line(mkit_handle_t caller, int line_id, int appearance_index);

/**
* @brief : SCA功能的hold
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
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
* @brief : SCA功能的hold
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
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
* @brief : BroadWorks紧急呼叫挂机接口
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: Broadsoft R22 紧急呼叫on-hook时使用
*/
SIP_DLL_DECLARE int sip_call_bw_emergency_hangup(mkit_handle_t caller, int line_id, int call_id);



/**
* @brief : 通话过程中设置audio参数，只支持设置一个codec相关参数
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_set_audio_param(mkit_handle_t caller, int line_id, int call_id, sip_media_t *audio);

/**
* @brief : 来电时设置H264视频参数
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_set_h264_param(mkit_handle_t caller, int line_id, int call_id, sip_h264_param_t *h264);

/**
* @brief : 来电时设置辅流H264视频参数
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_set_ext_h264_param(mkit_handle_t caller, int line_id, int call_id, sip_h264_param_t *h264);
/**
* @brief : 接起或重协商前设置视频能力相关参数（接起类型、带宽、主辅流分辨率、是否禁止辅流等）
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @param : params     同call invite使用方法，SIP_MAKE_CALL_H264_PARAM
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_set_call_param(mkit_handle_t caller, int line_id, int call_id, sip_ext_params_t *params);
/**
* @brief : 来电时设置H264视频带宽
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_set_video_bandwidth(mkit_handle_t caller, int line_id, int call_id, int band_width);

/**
* @brief : 来电时开启或者关闭视频
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_set_video_media(mkit_handle_t caller, int line_id, int call_id, int enable);

/**
* @brief : 来电时开启或者关闭辅流
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_set_bfcp_media(mkit_handle_t caller, int line_id, int call_id, int enable);
/**
* @brief : 控制远端摄像头
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_camera_action_start(mkit_handle_t caller, int line_id, int call_id, sip_camera_action_e action);

/**
* @brief : 停止控制远端摄像头
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_camera_action_stop(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : 保存远端摄像头设置
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_camera_preset_store(mkit_handle_t caller, int line_id, int call_id, int preset_num);

/**
* @brief : 启用远端摄像头设置
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_camera_preset_active(mkit_handle_t caller, int line_id, int call_id, int preset_num);

/**
* @brief : 开启辅流
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_extern_video_start(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : 关闭辅流
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_extern_video_stop(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : 发送DTMF,发送INFO消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
* @param : dtmf_code    [IN][0~int]     将要发送的dtmf值
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 该函数仅用于发DTMF，其他要发INFO的请使用sip_send_info
*/
SIP_DLL_DECLARE int sip_call_send_dtmf(mkit_handle_t caller, int line_id, int call_id, int dtmf_code);

/**
* @brief : Broadsoft Flash,发送INFO消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : uuid         [IN][0~int]     唯一标示符，用于区分前后信息是否同一个事务
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: JOYCE技术支持,拍插簧发送INFO消息
*/
SIP_DLL_DECLARE int sip_call_send_info_broadsoft_flash(mkit_handle_t caller, int line_id, int call_id, int uuid);

/**
* @brief : CMC,发送INFO消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : uuid         [IN][0~int]     唯一标示符，用于区分前后信息是否同一个事务
* @param : cmc_code     [IN][string]    指定发送的CMC code内容
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 客户要求话机在使用者主观判断是恶意来电的时候可以按一个键发送一个信息到服务器，
*          让服务器记录下来，该功能是通过在通话中发送INFO消息来实现的,服务器根据
*          X-ClientCode头域的值(cmc_code)对来电进行记录
*/
SIP_DLL_DECLARE int sip_call_send_info_cmc(mkit_handle_t caller, int line_id, int call_id, int uuid, char *cmc_code);

/**
* @brief : 通话录音功能,发送INFO消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : uuid         [IN][0~int]     唯一标示符，用于区分前后信息是否同一个事务
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 实现在通话中通知服务器开始录音、停止录音的功能
*/
SIP_DLL_DECLARE int sip_call_send_info_record_on(mkit_handle_t caller, int line_id, int call_id, int uuid);

SIP_DLL_DECLARE int sip_call_send_info_record_off(mkit_handle_t caller, int line_id, int call_id, int uuid);

/**
* @brief : 通话录音功能,发送fortinet 定制INFO消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : uuid         [IN][0~int]     唯一标示符，用于区分前后信息是否同一个事务
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 实现在通话中通知服务器开始录音、停止录音的功能
*/
SIP_DLL_DECLARE int sip_call_send_info_fortinet_record_on(mkit_handle_t caller, int line_id, int call_id, int uuid);

SIP_DLL_DECLARE int sip_call_send_info_fortinet_record_off(mkit_handle_t caller, int line_id, int call_id, int uuid);
/**
* @brief : I帧同步,发送INFO消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : uuid         [IN][0~int]     唯一标示符，用于区分前后信息是否同一个事务
* @param : body         [IN][string]    指定发送I帧同步的内容
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_send_info_frame_sync(mkit_handle_t caller, int line_id, int call_id, int uuid, char *body);

/**
* @brief : 通话中发送Broadworks Disposition Code,发送INFO消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : uuid         [IN][0~int]     唯一标示符，用于区分前后信息是否同一个事务
* @param : bw_service   [IN][string]    "X-BroadWorks-Service-Control"头域所发送的内容
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 增加头域类似：X-BroadWorks-Service-Control: sip:cc-disposition-code@as.yealink.com;code=123
*/
SIP_DLL_DECLARE int sip_call_send_info_bw_service(mkit_handle_t caller, int line_id, int call_id, int uuid, char *bw_service);

/**
* @brief : 通话中发送RTCP-XR统计信息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : uuid         [IN][0~int]     唯一标示符，用于区分前后信息是否同一个事务
* @param : vqreport_data[IN][]          需要上报的数据内容，参考ipvp_vq_report_t
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_call_send_rtcp_xr(mkit_handle_t caller, int line_id, int call_id, int uuid, const char *vqreport_data);

/**
* @brief : 获取指定通话的SIP信息
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : sip_call_proto_info_t[IN][]  需要获取的信息
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_call_get_info(mkit_handle_t caller, int line_id, int call_id, sip_call_proto_info_t *info);

/**
* @brief : 获取指定通话的SIP信息（扩展接口）
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : sip_call_proto_info2_t[IN][] 需要获取的信息
* @param : size         [IN][0~65535]   sip_call_proto_info2_t大小
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_call_get_info2(mkit_handle_t caller, int line_id, int call_id, sip_call_proto_info2_t *info, unsigned int size);

SIP_DLL_DECLARE int sip_call_get_transport_info(mkit_handle_t caller, int line_id, int call_id, sip_call_transport_info_t *info, unsigned int size);

SIP_DLL_DECLARE int sip_call_get_remote_user_agent(mkit_handle_t caller, int line_id, int call_id, char *buf, int buf_len);

/**
* @brief : 发送uaCSTA INFO消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : uuid         [IN][0~int]     唯一标示符，用于区分前后信息是否同一个事务
* @param : body         [IN][string]    指定发送的uaCSTA请求的内容
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_uacsta_send_request(mkit_handle_t caller, int line_id, int call_id, int uuid, const char *body);


/**
* @brief : 发送uaCSTA响应消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外响应
* @param : tid          [IN][0~int]     事务ID，构造响应消息的必要参数
* @param : body         [IN][string]    指定发送的uaCSTA响应的内容
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_uacsta_send_response(mkit_handle_t caller, int line_id, int call_id, int tid, const char *body);


/**
* @brief : 发送 kpml notify
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : did          [IN][0~int]     dialog id，构造notify的必要参数
* @param : code         [IN][0~int]     kpml status code(200: dtmf code, 423: 输入超时, 487: 注销订阅，关闭输入框)
* @param : digits       [IN][0~16]      dtmf digits
* @param : tag          [IN][string]    kpml tag
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_kpml_send_notify(mkit_handle_t caller, int line_id, int did, int code, int digits, const char *tag);


/**
* @brief : 通话中发送QoE语音质量报告信息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为则表示发送会话外信息
* @param : uuid         [IN][0~int]     唯一标示符，用于区分前后信息是否同一个事务
* @param : lync_vqreport_data[IN][]         
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_call_send_qoe_report(mkit_handle_t caller, int line_id, int call_id,  const char *vqreport_data);


/**
* @brief : lync conference创建会议
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : mask         [IN][0~int]     会议选项，如视频会议等
* @param : user                         创建会议时需要添加的会议成员
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_create_conference(mkit_handle_t caller, int line_id, int call_id, int mask, char *user);

/**
* @brief : lync conference 加入会议
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     会话ID
* @param : conv_id      会议ID
* @param : focus_uri                    加入会议的URI
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_join_conference(mkit_handle_t caller, int line_id, int call_id, char *conv_id, char *focus_uri);

/**
* @brief : lync conference添加会议成员
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : user 会议成员的sip uri
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_add_user(mkit_handle_t caller, int line_id, int call_id, char *user);

/**
* @brief : lync conference删除指定会议成员
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : user 会议成员的sip uri
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_delete_user(mkit_handle_t caller, int line_id, int call_id, char *user);

/**
* @brief : lync conference mute指定会议成员
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : user 会议成员的sip uri
* @param : uuid 会议成员的uuid
* @param : 1:mute 0:umute
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_mute_user(mkit_handle_t caller, int line_id, int call_id, char *user, char *uuid, char *media_id, int mute);

/**
* @brief : lync conference mute除自己以外的其他会议成员
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : 1:mute 0:umute
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_mute_audience(mkit_handle_t caller, int line_id, int call_id, int mute);

/**
* @brief : lync conference 会议公告
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : 1:enable 0:disable
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_announcements(mkit_handle_t caller, int line_id, int call_id, int enable);

/**
* @brief : lync conference 会议锁，锁住后无法添加其他成员
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : 1:lock 0:unlock
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_lock(mkit_handle_t caller, int line_id, int call_id, int enable);

/**
* @brief : lync conference 允许或拒绝会议大厅的参与方进入会议
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : user_access_info 包含允许或拒绝参与方的会议成员信息，格式为XML
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_setlobby_access(mkit_handle_t caller, int line_id, int call_id, char *user_access_info);

/**
* @brief : lync conference 修改参与者的权限：演示者或者与会者
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : role 1:演示者 0:与会者
* @param : user 被修改的参与者的uri为 sip:xxx@yealinkuc.com
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_modify_user_role(mkit_handle_t caller, int line_id, int call_id, int role, char *user);

/**
* @brief : lync conference通话合并成会议
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : mask         [IN][0~int]     会议选项，如视频会议等
* @param : merger_cid [IN][0~int]    被合并通话的会话ID
* @param : user                               被合并通话会议成员的uri
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_conference_merge_user(mkit_handle_t caller, int line_id, int call_id, int mask, int merger_cid,char *user);

/**
* @brief : 更新e911 location
* @param : line_id            [IN][0~32]      line id，账号ID
* @param : location_info      [IN][string]    e911 location info
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_update_e911_location_info(mkit_handle_t caller, int line_id, const char *location_info);

/**
* @brief : 更新状态
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : state        [IN][]          sip_lync_presence_state_e
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_publish_presence_state(mkit_handle_t caller, int line_id, int state);

/**
* @brief : 更新个性签名
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : note         [char*][strlen: 0~512]        
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_publish_presence_note(mkit_handle_t caller, int line_id, const char *note);

/**
* @brief : 通知SIP推送routing消息
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : type         [IN][]          sip_lync_routing_type_e
* @param : routing      [IN][string]    routing info
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_publish_routing(mkit_handle_t caller, int line_id, int type, const char *routing);

/**
* @brief : 通知SIP推送user infomation
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : type         [IN][]          sip_lync_userinfomation_type_e
* @param : string       [IN][string]    user infomation
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_publish_user_infomation(mkit_handle_t caller, int line_id, int type, const char *string);

/**
* @brief : 通知SIP修改隐私设置
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : type         [IN][]          SIP_LYNC_PRES_PRIVACY_PRIVATE/SIP_LYNC_PRES_PRIVACY_STANDARD
* @param : NULL
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_lync_publish_privacy_mode(mkit_handle_t caller, int line_id, int type);



/**
* @brief : 通知SIP永久订阅联系人信息
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : uuid         [IN][int]       UI 用于标记订阅目标类型的参数，SIP 需在订阅结果中带回该值
* @param : to           [IN][string]    首次订阅为NULL，resub时如果有poolfqdn参数则为poolfqdn
* @param : body         [IN][string]    订阅消息的BODY
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_subscribe_contacts_presence_presistent(mkit_handle_t caller, int line_id, int uuid, const char *to, const char *body);

/**
* @brief : 通知SIP一次性订阅联系人信息
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : uuid         [IN][int]       UI 用于标记订阅目标类型的参数，SIP 需在订阅结果中带回该值
* @param : to           [IN][string]    首次订阅为NULL，resub时如果有poolfqdn参数则为poolfqdn
* @param : body         [IN][string]    订阅消息的BODY
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_subscribe_contacts_presence_polling(mkit_handle_t caller, int line_id, int uuid, const char *to, const char *body);

/**
* @brief : 通知SIP永久订阅单个联系人信息（仅resub时使用）
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : uuid         [IN][int]       UI 用于标记订阅目标类型的参数，SIP 需在订阅结果中带回该值
* @param : body         [IN][string]    首次订阅结果中没有poolfqdn属性的联系人按指定格式构造的字符串
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_subscribe_contacts_presence_list_presistent(mkit_handle_t caller, int line_id, int uuid, const char *contact_list);

/**
* @brief : 通知SIP一次性订阅联系人信息（仅resub时使用）
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : uuid         [IN][int]       UI 用于标记订阅目标类型的参数，SIP 需在订阅结果中带回该值
* @param : body         [IN][string]    首次订阅结果中没有poolfqdn属性的联系人按指定格式构造的字符串
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_subscribe_contacts_presence_list_polling(mkit_handle_t caller, int line_id, int uuid, const char *contact_list);


/**
* @brief : 通知SIP订阅联系人信息
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : uuid         [IN][int]       UI 用于标记订阅目标类型的参数，SIP 需在订阅结果中带回该值
* @param : expire_type  [IN][enum]      永久订阅或一次性订阅，影响消息的 Expires 头域值，
*                                       sip_lync_contact_pres_sub_expries_type_e
* @param : body         [IN][string]    订阅消息的BODY
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
* @brief : UI 在处理联系人状态订阅结果时使用，对需要 resub 的结果通知 SIP 进行 resub
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : uuid         [IN][int]       UI 用于标记订阅目标类型的参数，SIP 需在订阅结果中带回该值
* @param : expire_type  [IN][enum]      永久订阅或一次性订阅，影响消息的 Expires 头域值，
*                                       sip_lync_contact_pres_sub_expries_type_e
* @param : poolfqdn     [IN][string]    非空时为订阅目标，NULL则目标从body参数中读取，影响消息的 To 头域值
* @param : body         [IN][string]    订阅消息的BODY
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
* @brief : UI通知SIP自动化测试执行结果(与SIP无关的操作)
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : result       [IN][int]       操作执行结果, 0:执行失败，1:执行成功
* @param : code         [IN][string]    可选参数：执行结果附加信息
* @param : reason       [IN][string]    可选参数：执行结果附加信息
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_lync_ui_report_automation_operate_result(mkit_handle_t caller, int line_id, int result, int code, const char *reason);

/**
* @brief : BTOE联动登录, 使用Lync客户端输入密码登录
* @param : 
* @return: 
* @remark: 
*/
SIP_DLL_DECLARE int sip_lync_btoe_help_login(mkit_handle_t caller, int line_id);

/**
* @brief : BTOE话机端取消联动登录, 关闭Lync客户端登录窗口
* @param : 
* @return: 
* @remark: 
*/
SIP_DLL_DECLARE int sip_lync_btoe_cancel_help_login(mkit_handle_t caller, int line_id);


/*
 *------------------------------------
 *       apollo会议接口
 *------------------------------------
*/
/**
* @brief : apollo conference创建会议
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID
* @param : mask         [IN][0~int]     会议选项，如视频会议等，但现在还未使用
* @param : user                         先定义，后期使用，目前传NULL
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_create_conference(mkit_handle_t caller, int line_id, int call_id, int mask, const char *user);

/**
* @brief : apollo conference添加会议成员
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : call_way     [IN][0~int]     加入会议方式,0:Dila-in, 1:Dail-out
* @param : user 会议成员的sip uri,目前需要ui区分是sip话机还是pstn
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_conference_add_user(mkit_handle_t caller, int line_id, int call_id, int call_way, const char *user);

/**
* @brief : apollo conference删除会议成员
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : user 会议成员的sip uri,目前需要ui区分是sip话机还是pstn
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_conference_delete_user(mkit_handle_t caller, int line_id, int call_id, const char *user);

/**
* @brief : apollo conference加入会议
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : conv_id 会议id
* @param : focus_uri 会议控制通道focus uri
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_join_conference(mkit_handle_t caller, int line_id, int call_id, const char *conv_id, const char *focus_uri);

/**
* @brief : apollo conference mute指定会议成员或自己申请举手
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : user 会议成员的sip uri
* @param : uuid 会议成员的uuid
* @param : media_id: 会议media的id
* @param : mute:mute操作 0：Unmute/举手 1：mute
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_conference_mute_user(mkit_handle_t caller, int line_id, int call_id, const char *user, const char *uuid, int media_id, int mute);

/**
* @brief : apollo conference service会议信息获取
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : body  信令的消息
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_conference_service(mkit_handle_t caller, int line_id, int call_id, const char *body);

/**
* @brief : apollo conference finish主席会议模式下主席结束会议，全体成员退出会议
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_conference_finish(mkit_handle_t caller, int line_id, int call_id);

/**
* @brief : apollo conference modify user role修改与会者身份，只有演示者有此权限
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : role         角色，1：与会者 0：演示者
* @param : user         被修改者URI
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_apollo_conference_modify_user_role(mkit_handle_t caller, int line_id, int call_id, int role, const char *user);
/*
 *------------------------------------
 *      订阅相关APIs
 *------------------------------------
 */

/**
* @brief : BLF查询
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : key_index    [IN][-1~...]  查询key的索引，-1表示所有key
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 调用该接口，会触发sip上报dsskey的BLF状态，-1表示所有key，如果key_index大于实际的最大值，
*           将不会上报
*/
SIP_DLL_DECLARE int sip_blf_get_key_status(mkit_handle_t caller, int line_id, int key_index);

/**
* @brief : 获取BLF-LIST收到的NOTIFY内容
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : buf          [IN]
* @param : size         [IN]
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: buf和size暂时不能使用，调这个接口后，会触发blf list的消息上报
*/
SIP_DLL_DECLARE int sip_blflist_get_body(mkit_handle_t caller, int line_id, char *buf, int size);

/**
* @brief : call completion订阅/取消订阅
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : receiver     [IN][64]        目标号码或地址
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_call_completion_subscribe(mkit_handle_t caller, int line_id, const char *receiver);
SIP_DLL_DECLARE int sip_call_completion_unsubscribe(mkit_handle_t caller, int line_id, const char *receiver);

/**
* @brief : SCA占线
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : appearance   [IN][64]        SCA信令的Call-info头域带的内容
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_sca_seize_line(mkit_handle_t caller, int line_id, int appearance_index);

/**
* @brief : SCA取消占线
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : appearance   [IN][64]        SCA信令的Call-info头域带的内容
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_sca_unseize_line(mkit_handle_t caller, int line_id, int appearance_index);

/**
* @brief : SCA线路查询，SCA本来会实时上报线路状态，如果UI什么时候缓存丢失（如UI重启）才需要调用该接口
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : appearance   查询的线路状态
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_sca_line_query(mkit_handle_t caller, int line_id);


/**
* @brief : ACD操作函数（NOTIFY类型）
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : action       [IN][0~3]       操作类型，sip_acd_action_code_e
* @param : number       [IN][0~int]     绑定的dsskey值
* @param : user         [IN][64]        登录用户名
* @param : pass_word    [IN][64]        登录用户的密码
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
* @brief : Broadsoft call center操作函数
* @param : line_id      [IN][0~32]      line id，账号ID
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_broadsoft_call_center_subscribe(mkit_handle_t caller, int line_id);
SIP_DLL_DECLARE int sip_broadsoft_call_center_unsubscribe(mkit_handle_t caller, int line_id);

/**
* @brief : Hoteling操作函数
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : user         [IN][64]        登录用户名
* @param : pass_word    [IN][64]        登录用户的密码
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_hoteling_login(mkit_handle_t caller, int line_id, const char *user, const char *pass_word);
SIP_DLL_DECLARE int sip_hoteling_logout(mkit_handle_t caller, int line_id);


/*
 *  其他模块相关API
 */

/**
* @brief : KTS 发送KINFO命令
* @param : line_id      [IN][0~32]       line id，账号ID
* @param : kts_cmd      [CHAR *][String] Kts命令
* @param ：len          [IN][1~int]      命令长度
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
SIP_DLL_DECLARE int sip_send_kts_info(mkit_handle_t caller, int line_id, char *kts_cmd, int len);


/**
* @brief : Genband E911 UI获取到位置后，通知SIP使用location id进行注册刷新等操作
* @param ：line_id      [IN][1~int]      line id，开启Genband E911的账号id，预留接口参数
* @param ：location_id  [IN][1~int]      location id
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
SIP_DLL_DECLARE int sip_e911_set_location(mkit_handle_t caller, int line_id, int location_id);


/**
* @brief : 发送SMS文本信息
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : uuid         [IN][0~int]     唯一标示符，用于区分前后信息是否同一个事务
* @param : receiver     [IN][64]        接收者号码或地址
* @param : text         [IN][512]       将要发送的文本内容
* @PARAM : ext_header   [IN][string]    扩展SIP信令自定义头域内容，可以为结构体数组形式来设置多个头域,为NULL则不处理
* @PARAM : ext_header_size [IN][string] 需要扩展自定义头域的个数
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
* @brief : 发送INFO消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : uuid         [IN][0~int]     唯一标示符，用于区分前后信息是否同一个事务
* @param : info_type    [IN][0~int]     INFO类型，现在有两种，普通INFO和DTMF的INFO，（SIP_INFO_SEND_TYPE_NORMAL/SIP_INFO_SEND_TYPE_DTMF）
* @param : content_type [IN][string]    设置SIP信令Content-Type的内容，为NULL则不处理
* @PARAM : body         [IN][string]    设置SIP信令Body携带的内容，为NULL则不处理
* @PARAM : ext_header   [IN][string]    扩展SIP信令自定义头域内容，可以为结构体数组形式来设置多个头域,为NULL则不处理
* @PARAM : params       [sip_ext_params_t*]    各种扩展参数
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
* @brief : 发送自定义INFO信息，对应收到的消息为SIP_MSG_RECV_CUSTOM_INFO
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : uuid         [IN][0~int]     唯一标示符，用于区分前后信息是否同一个事务
* @PARAM : body         [IN][string]    设置SIP信令Body携带的内容，为NULL则不处理
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
* @brief : 发送NOTIFY消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : uuid         [IN][0~int]     唯一标示符，用于区分前后信息是否同一个事务
* @param : content_type [IN][string]    设置SIP信令Content-Type的内容，为NULL则不处理
* @PARAM : body         [IN][string]    设置SIP信令Body携带的内容，为NULL则不处理
* @PARAM : ext_header   [IN][string]    扩展SIP信令自定义头域内容，可以为结构体数组形式来设置多个头域,为NULL则不处理
* @PARAM : ext_header_size [IN][string] 需要扩展自定义头域的个数
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_send_notify(mkit_handle_t caller, int line_id, int call_id, int uuid, 
                    char *content_type, char *body, const char* headers, sip_ext_params_t* params);

/**
* @brief : 发送PUBLISH消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : uuid         [IN][0~int]     唯一标示符，用于区分前后信息是否同一个事务
* @param : content_type [IN][string]    设置SIP信令Content-Type的内容，为NULL则不处理
* @PARAM : body         [IN][string]    设置SIP信令Body携带的内容，为NULL则不处理
* @PARAM : ext_header   [IN][string]    扩展SIP信令自定义头域内容，可以为结构体数组形式来设置多个头域,为NULL则不处理
* @PARAM : ext_header_size [IN][string] 需要扩展自定义头域的个数
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_send_publish(mkit_handle_t caller, int line_id, int call_id, int uuid, 
                     const char *content_type, const char *body, const char* headers, sip_ext_params_t* params);

/**
* @brief : Tiptel PBX服务器按键控制,发送NOTIFY消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为0则表示发送会话外信息
* @param : uuid         [IN][0~int]     唯一标示符，用于区分前后信息是否同一个事务
* @param : body         [IN][string]    指定发送body内容
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_send_notify_tiptel_pbx_button(mkit_handle_t caller, int line_id, int call_id, int uuid, char *body);


/**
* @brief : as feature key功能键同步（一般用于broadsoft acd）
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : xml_body     [IN][512]       subscribe信令携带的xml内容
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: 
*/
SIP_DLL_DECLARE int sip_send_feature_key(mkit_handle_t caller, int line_id, const char *xml_body);


/**
* @brief : 发送uc info消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为则表示发送会话外信息
* @param : buf          [IN][char *]    xml body
* @param : size         [IN][0~65535]   body大小
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_uc_send_request(mkit_handle_t caller, int line_id, int call_id, sip_uc_req_send_info_t *info, int size);

/**
* @brief : 发送uc的info响应消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为则表示发送会话外响应
* @param : tid          [IN][0~int]     事务ID，构造响应消息的必要参数
* @param : code         [IN][0~int]     指定发送的应答码
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_uc_send_response(mkit_handle_t caller, int line_id, int call_id, int tid, int code);

/**
* @brief : 发送uc的notify响应消息给服务器
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][0~int]     原会话ID，为则表示发送会话外响应
* @param : tid          [IN][0~int]     事务ID，构造响应消息的必要参数
* @param : code         [IN][0~int]     指定发送的应答码
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark: param must not NULL
*/
SIP_DLL_DECLARE int sip_uc_send_notify_response(mkit_handle_t caller, int line_id, int call_id, int tid, int code);

/**
* @brief : srtp lifetime reinvite
* @param : line_id      [IN][0~32]      line id，账号ID
* @param : call_id      [IN][1~int]     原会话ID，用于识别该路通话
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
    * usage   : 上报注册状态
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
    #define SIP_REG_UNREGING                    4/* 4 正在注销该账户 */
    #define SIP_REG_UNREGEND                    5/* 5 */
    #define SIP_REG_REGISTERED_ON_BOOT          6/* 6 上电注册成功 */
    #define SIP_REG_DISABLE_REGISTER            7/* 7 账号为active，但是不发送注册报文 */
    #define SIP_REG_CANCELLING                  8/* 8 取消注册中 */
    #define SIP_REG_CANCEL_COMPLETED            9/* 9 取消注册完成 */

#define SIP_REG_UPDATE_REGISTER                 SIP_S2C_MSG(SIP_MSG_S2C_REG, 0x0001)


    /*
    * usage   : 上报服务器下发的注册事件（收到注册的NOTIFY时会上报）
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
    * usage   : 更新当前注册的服务器地址，一般用于备份服务器的切换通知
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
    * usage   : 注册失败重试时间
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
    * usage   : 更新DEVICE PAIRING状态及信息
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
    * usage   : 注册错误码
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
    * usage   : Exchange地址
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
    * usage   : 上报账号注册成功使用的传输方式
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
    * usage   : 上报账号注册成功使用的服务器类型
    *          
    * msg_id  : 0x00040009
    * target  : none
    * wparam  : line_id
    * lparam  : server type
    * data_ptr: null
    * data_len: 0
    */

/* 与sip_server_type_e定义保存一致 */
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
    * usage   : 上报web sign in信息
    *          
    * msg_id  : 0x0004000a
    * target  : none
    * wparam  : line_id
    * lparam  : none
    * data_ptr: web sign in信息存储路径
    * data_len: 
    */
#define SIP_REG_UPDATE_WEB_SIGN_IN_INFO    SIP_S2C_MSG(SIP_MSG_S2C_REG, 0x000a)

/* 技术支持分支新增接口消息预留 0x000->0x04F，Lync 预留 0x050->0x07F
   其它项目新增时，根据实际需要进行分支同步或者新增预留范围 */


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
    * usage   : 上报新来电
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
    * usage   : 上报通话正在连接
    *          
    * msg_id  : 0x00040102
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: none
    * data_len: none
    * remark  : 暂未使用
    */

#define SIP_CALL_PROCEEDING                     SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0002)


    /*
    * usage   : 上报对方正在响铃
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
    * usage   : 上报通话被拒绝
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
    * usage   : 上报通话建立成功，通话刚建立时会上报该消息，通话中则使用SIP_MSG_CALL_UPDATE
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
    * usage   : 上报通话失败
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
    * usage   : 上报通话已结束
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
    * usage   : 上报通话更新，被HOLD、解HOLD等也是通过该消息实现
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
    * usage   : 上报通话被Forward，需要重新呼叫（通话收到3XX时会上报）
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
    * usage   : 上报通话被转接（通话收到REFER时会上报）
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
    * usage   : 上报通话被REPLACE（通话收到新来电并带REPLACE头域时会上报）
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
    * usage   : 上报会议状态
    *          
    * msg_id  : 0x0004010c
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: xml_body(char *)
    * data_len: strlen
    * remark  : 暂未使用
    */
#define SIP_CALL_CONFERENCE_NOTIFY              SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x000c)


    /*
    * usage   : 上报通话显示更新
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
    * usage   : 上报通话中收到的I帧同步请求
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
    * usage   : 上报通话过程中的定制内容更新
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
    * usage   : 上报通话codec列表
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
    * usage   : 上报通话被Forward，需要重新呼叫（通话收到3XX时会上报）
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
    * usage   : 上报通话被转接（通话收到REFER时会上报）
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
    * usage   : 上报通话被REPLACE（通话收到新来电并带REPLACE头域时会上报）可以携带新通话类型
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
    * usage   : 上报收到的lync unpark通知
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
    * usage   : 通话类型上报
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
    * usage   : 上报broadworks通话录音状态
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
    int op_result;              /* call recording 操作结果：0:成功，-1:失败，other: reason_code */

#define SIP_BW_CALL_RECORD_OFF          0
#define SIP_BW_CALL_RECORD_ON           1
#define SIP_BW_CALL_RECORD_PAUSED       2

    int audio_record_status;
    int video_record_status;
}bw_call_record_t;

#define SIP_CALL_BW_CALL_RECORD_STATUS           SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0018)


    /*
    * usage   : 上报Q850状态码
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
    * usage   : 上报HOLD失败
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
    * usage   : 上报RESUME失败
    *          
    * msg_id  : 0x0004011b
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_CALL_RESUME_FAIL                    SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x001b)


/* 技术支持分支新增接口消息预留 0x000->0x04F，Lync 预留 0x050->0x07F
   其它项目新增时，根据实际需要进行分支同步或者新增预留范围 */

/************************************************************************/
/*                   Lync msg id 0x050->0x07F start                     */
/************************************************************************/

//#define SIP_CALL_LYNC_XXX                 SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0050)

/************************************************************************/
/*                   Lync msg id 0x050->0x07F end                       */
/************************************************************************/


/* 作为转接方何时挂断该路通话 */
#define SIP_TRANSFEROR_NOT_HANGUP                       0
#define SIP_TRANSFEROR_HANGUP_AFTER_200OK_NOTIYF        1
#define SIP_TRANSFEROR_HANGUP_AFTER_180RING_NOTIYF      2
#define SIP_TRANSFEROR_HANGUP_AFTER_100TRING_NOTIYF     3
#define SIP_TRANSFEROR_HANGUP_AFTER_REFER_RESPONSE      4

    /*
    * usage   : 上报通话转接结果
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
    * usage   : 上报HOLD失败原因，注意该消息仅在HOLD异常/失败时才上报，如果HOLD成功则使用SIP_MSG_CALL_UPDATE
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
    * usage   : 上报Call Record On是否成功
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
    * usage   : 上报Call Record Off是否成功
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
    * usage   : 上报INFO发送结果
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
    * usage   : 上报收到的lync CallPark状态
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
    * usage   : 上报通话中协商失败
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
    * usage   : 上报通道参数变更
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
    * usage   :上报会议建立结果
    * msg_id  : 0x00040189
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */
#define SIP_MSG_APOLLO_CREATE_CONFERENCE_RESULT   SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0089)

    /*
    * usage   : 上报会议请求失败
    * msg_id  : 0x0004018a
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_reason_info_t
    * data_len: sizeof(sip_reason_info_t)
    */
#define SIP_MSG_APOLLO_CONF_REQUEST_RESULT        SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x008a)

    /*
    * usage   :上报Reinvite
    * msg_id  : 0x0004018c
    * target  : none
    * wparam  : line_id
    * lparam  : callid
    * data_ptr: sip_reinvite_t
    * data_len: 
    */
#define SIP_CALL_REINVITE_RECIEVE                  SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x008c)

    /*
    * usage   :上报额外的显示信息
    * msg_id  : 0x0004018d
    * target  : none
    * wparam  : line_id
    * lparam  : callid
    * data_ptr: sip_user_info_t
    * data_len: 
    */
#define SIP_CALL_EXT_INFO                          SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x008d)

    /*
    * usage   :cong添加成员失败
    * msg_id  : 0x0004018e
    * target  : none
    * wparam  : line_id
    * lparam  : conf call id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */
#define SIP_CALL_CONF_ADD_USER_FAIL                SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x008e)

    /*
    * usage   : 通知UI Warning header 信息，UI根据配置需要进行显示
    * msg_id  : 0x0004018f
    * target  : none
    * wparam  : line id
    * lparam  : call id
    * data_ptr: char * （格式：warn-code SP warn-agent SP warn-text
    *                       eg. 307 isi.edu "Session parameter 'foo' not understood"）
    * data_len: strlen + 1
    */
#define SIP_CALL_WARNING_INFO                     SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x008f)

  /*
    * usage   :上报会议INFO消息
    * msg_id  : 0x00040190
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr:xml
    * data_len: 
    */
#define SIP_MSG_APOLLO_CONFERENCE_INFO_RESULT   SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0090)

 /*
    * usage   :上报会议预约消息
    * msg_id  : 0x00040191
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: xml
    * data_len: 
 */
#define SIP_MSG_APOLLO_CONFERENCE_GET_MSG       SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x0091)

/* 
    1、中性技术支持新增消息可从0x0050前增加
    2、0x0004018b 未被使用，新增消息时可先使用，使用完去除该注释
*/

/************************************************************************/
/*                   VMR 0x00e0---->0x00FF start                        */
/************************************************************************/

 /*
    * usage   :上报VMR 长连接通道注册消息
    * msg_id  : 0x000401e0
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_invite_t
    * data_len: sizeof(sip_apollo_vmr_invite_t)
 */
#define SIP_APOLLO_VMR_MSG_INVITE              SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e0)

 /*
    * usage   :上报VMR 创建会议消息
    * msg_id  : 0x000401e1
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sizeof(sip_apollo_vmr_conf_data_t)
    * data_len: sip_apollo_vmr_conf_data_t
 */
#define SIP_APOLLO_VMR_MSG_CONF_CREATE         SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e1)



 /*
    * usage   :上报VMR 删除会议消息
    * msg_id  : 0x000401e2
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_conf_destroy_t
    * data_len: sizeof(sip_apollo_vmr_conf_destroy_t)
 */
#define SIP_APOLLO_VMR_MSG_CONF_DESTORY        SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e2)

 /*
    * usage   :上报VMR 修改会议消息
    * msg_id  : 0x000401e3
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_conf_data_t
    * data_len: sizeof(sip_apollo_vmr_conf_data_t)
 */
#define SIP_APOLLO_VMR_MSG_CONF_MODIFT         SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e3)

 /*
    * usage   :上报VMR 通话建立消息
    * msg_id  : 0x000401e4
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_start_session_req_t
    * data_len: sizeof(sip_apollo_vmr_start_session_req_t)
 */
#define SIP_APOLLO_VMR_MSG_START_SESSION       SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e4)


 /*
    * usage   :上报VMR 通话结束消息(含辅流)
    * msg_id  : 0x000401e5
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_call_release_t
    * data_len: sizeof(sip_apollo_vmr_call_release_t)
 */
#define SIP_APOLLO_VMR_MSG_STOP_SESSION        SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e5)

 /*
    * usage   :上报VMR 系统配置查询消息
    * msg_id  : 0x000401e6
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_conf_query_req_t
    * data_len: sizeof(sip_apollo_vmr_conf_query_req_t)
 */
#define SIP_APOLLO_VMR_MSG_CONF_QUERY          SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e6)

 /*
    * usage   :上报VMR 系统能力查询消息
    * msg_id  : 0x000401e7
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_system_query_req_t
    * data_len: sizeof(sip_apollo_vmr_system_query_req_t)
 */
#define SIP_APOLLO_VMR_MSG_RESOURCE_QUERY      SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e7)

 /*
    * usage   :上报VMR 通话hold消息
    * msg_id  : 0x000401e8
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_hold_session_req_t
    * data_len: sizeof(sip_apollo_vmr_hold_session_req_t)
 */
#define SIP_APOLLO_VMR_MSG_HOLD_SESSION        SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e8)

 /*
    * usage   :上报VMR 通话resume消息
    * msg_id  : 0x000401e9
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_unhold_session_req_t
    * data_len: sizeof(sip_apollo_vmr_unhold_session_req_t)
 */
#define SIP_APOLLO_VMR_MSG_RESUME_SESSION      SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00e9)

 /*
    * usage   :上报VMR Mute功能消息
    * msg_id  : 0x000401ea
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_mute_media_req_t
    * data_len: sizeof(sip_apollo_vmr_mute_media_req_t)
 */
#define SIP_APOLLO_VMR_MSG_MUTE_MEDIA          SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00ea)

 /*
    * usage   :上报VMR 通话布局设置消息
    * msg_id  : 0x000401eb
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_layout_authority_req_t
    * data_len: sizeof(sip_apollo_vmr_layout_authority_req_t)
 */
#define SIP_APOLLO_VMR_MSG_LAYOUT_AUTHORITY    SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00eb)

 /*
    * usage   :上报VMR 通话演示设置消息
    * msg_id  : 0x000401ec
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_demonsrator_state_req_t
    * data_len: sizeof(sip_apollo_vmr_demonsrator_state_req_t)
 */
#define SIP_APOLLO_VMR_MSG_DEMONSRATOR_STATE   SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00ec)

 /*
    * usage   :上报VMR 通话视频改变消息
    * msg_id  : 0x000401ed
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_video_modify_req_t
    * data_len: sizeof(sip_apollo_vmr_video_modify_req_t)
 */
#define SIP_APOLLO_VMR_MSG_VIDEO_MODIFY        SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00ed)

 /*
    * usage   :上报VMR 通话视频关键帧申请消息
    * msg_id  : 0x000401ee
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_key_frame_req_t
    * data_len: sizeof(sip_apollo_vmr_key_frame_req_t)
 */
#define SIP_APOLLO_VMR_MSG_KEY_FRAME           SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00ee)

 /*
    * usage   :上报VMR 通话IVR 设置视频文字消息
    * msg_id  : 0x000401ef
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_set_osd_req_t
    * data_len: sizeof(sip_apollo_vmr_set_osd_req_t)
 */
#define SIP_APOLLO_VMR_MSG_SET_OSD             SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00ef)

 /*
    * usage   :上报VMR 通话IVR 语音播放提示消息
    * msg_id  : 0x000401f0
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_apollo_vmr_ivr_res_t
    * data_len: sizeof(sip_apollo_vmr_ivr_res_t)
 */
#define SIP_APOLLO_VMR_MSG_VOICE_PROMPT        SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00f0)

 /*
    * usage   :上报VMR 通话media codec list消息
    * msg_id  : 0x000401f1
    * target  : none
    * wparam  : line_id
    * lparam  : id 
    * data_ptr: sip_apollo_vmr_media_codec_array_t
    * data_len: media_cnt*sizeof(sip_codec_array_t)+12
 */
#define SIP_APOLLO_VMR_MSG_MEDIA_CODEC        SIP_S2C_MSG(SIP_MSG_S2C_CALL, 0x00f1)

 /*
    * usage   :上报VMR 通话通道establish消息
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
    * usage   : 更新BLF状态
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
    * usage   : 更新BLF扩展信息
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
    * usage   : 更新BLFLIST
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
    * usage   : 更新BLA状态
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
    * usage   : 更新appearance状态
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
    * usage   : 上报占线成功状态
    *          
    * msg_id  : 0x00040206
    * target  : none
    * wparam  : line_id
    * lparam  : appearance-index
    * data_ptr: 指向int指针，内容为0表示失败，为1表示成功
    * data_len: sizeof(int)
    */
#define SIP_SUB_UPDATE_BROADSOFT_SCA_SEIZE_RESULT     SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0006)


    /*
    * usage   : 更新Call Completion状态
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
    * usage   : 更新功能键同步状态
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
    * usage   : 更新Hoteling状态
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
    * usage   : 上报收到的Broadworks CallPark状态
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
    * usage   : 上报收到的Broadworks Call Center状态
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
    * usage   : 上报收到的GCS信息
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
    * usage   : 更新ACD状态
    *          
    * msg_id  : 0x0004020d
    * target  : none
    * wparam  : line_id
    * lparam  : update numbers(0~int)
    * data_ptr: sip_acd_status_info_t
    * data_len: 
    */
    /* ACD状态值 */
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
    * usage   : 更新Dial Tone
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
    * usage   : 收到Hot Desking信息
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
    * usage   : 收到Tiptel PBX Control请求
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
    * usage   : 上报Lync contacts订阅结果(订阅联系人列表、联系人信息的结果都用此消息上报)
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
    * usage   : 上报Lync Provision订阅结果
    *          
    * msg_id  : 0x00040212
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_lync_provision_t（需要使用sip_map_info_recover接口读取数据）
    * data_len: sip_lync_provision_t.size
    */
#define SIP_SUB_LYNC_PROVISION_UPDATE           SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0012)

/*
    * usage   : Draft Bla状态通告
    *          
    * msg_id  : 0x00040213
    * target  : none
    * wparam  : line_id
    * lparam  : appearance id
    * data_ptr: BLA状态如SIP_BLA_DRAFT_STATUS_SEIZED
    * data_len: 
    */
#define SIP_SUB_UPDATE_DRAFT_BLA_STATUS         SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0013)


    /*
    * usage   : 上报Lync Boss Provision订阅结果
    *          
    * msg_id  : 0x00040214
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_lync_boss_provision_t（需要使用sip_map_info_recover接口读取数据）
    * data_len: sip_lync_boss_provision_t.size
    */
#define SIP_SUB_LYNC_BOSS_PROVISION_UPDATE      SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0014)

    /*
    * usage   : 上报address book的notify body
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
    * usage   : 上报Lync boss同响的配置
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
    * usage   : 上报自身BLF订阅NOTIFY结果
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
    * usage   : 上报Genband service package NOTIFY结果
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
    * usage   : 上报Broadsoft conference 会议信息
    *          
    * msg_id  : 0x00040219
    * target  : none
    * wparam  : line_id
    * lparam  : cid (对应会议的cid，目前仅作为ad hoc主持方才可以获取到)
    * data_ptr: xml_body(char *)
    * data_len: strlen
    */
#define SIP_SUB_BW_NETWORK_CONF_INFO                        SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0019)


    /*
    * usage   : 上报BW会议参与方会议对应的通话线路index
    *          
    * msg_id  : 0x0004021a
    * target  : none
    * wparam  : line_id
    * lparam  : update_num
    * data_ptr: sip_bw_conf_index_t
    * data_len: 
    */
#define SIP_SUB_BW_CONF_INDEX                                SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x001a)


/* 技术支持分支新增接口消息预留 0x000->0x04F，Lync 预留 0x050->0x07F
   其它项目新增时，根据实际需要进行分支同步或者新增预留范围 */


/************************************************************************/
/*                   Lync msg id 0x050->0x07F start                     */
/************************************************************************/

//#define SIP_SUB_LYNC_XXX                 SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0050)

/************************************************************************/
/*                   Lync msg id 0x050->0x07F end                       */
/************************************************************************/



    /*
    * usage   : 上报BLA占线结果
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
    * usage   : 上报Call Completion订阅结果
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
    * usage   : 上报功能键同步订阅结果
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
    * usage   : 上报NOTIFY发送结果
    *          
    * msg_id  : 0x00040284
    * target  : none
    * wparam  : line_id
    * lparam  : call_id(为0表示会话外NOTIFY)
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_SUB_SEND_NOTIFY_RESULT              SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0084)

    /*
    * usage   : 上报Bla占线/取消占线结果
    *          
    * msg_id  : 0x00040285/0x00040286
    * target  : none
    * wparam  : line_id
    * lparam  : appearance_id
    * data_ptr: 1-占线/取消占线成功，0-占线/取消占线失败
    * data_len: 
    */
#define SIP_SUB_DRAFT_BLA_SEIZE_LINE_RESULT     SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0085)
#define SIP_SUB_DRAFT_BLA_UNSEIZE_LINE_RESULT   SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0086)
    #define SIP_FORTINET_DND_MODE_START                1
    #define SIP_FORTINET_DND_MODE_STOP                 0
#define SIP_SUB_FORTINET_DND_SUBSCRIBED_RESULT  SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0087)

    /*
    * usage   : Fortinet Dnd订阅结果上报
    *          
    * msg_id  : 0x0285/0x0286
    * target  : none
    * wparam  : line_id
    * lparam  : 1-进入fortinet dnd模式，0-退出fortinet dnd模式
    * data_ptr: reserved
    * data_len: 
    */

    #define SIP_FORTINET_DND_ON                       1
    #define SIP_FORTINET_DND_OFF                      0
#define SIP_SUB_FORTINET_DND_NOTIFY_RESULT      SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x0088)
    /*
    * usage   : 上报KTS info的Body
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
    * usage   : 上报占线成功状态，增加489 NoSupport支持
    *          
    * msg_id  : 0x028a
    * target  : none
    * wparam  : line_id
    * lparam  : appearance-index
    * data_ptr: 指向int指针，内容为0表示失败，为1表示成功
    * data_len: sizeof(int)
    */
#define SIP_SCA_SEIZE_FAIL 0
#define SIP_SCA_SEIZE_SUCC 1
#define SIP_SCA_SEIZE_NO_SUPPORT 2
#define SIP_SUB_UPDATE_BROADSOFT_SCA_SEIZE_RESULT2     SIP_S2C_MSG(SIP_MSG_S2C_SUB, 0x008a)

/* 中性技术支持新增消息可从0x0050前增加 */


#endif/* __SIP_SUB_DEFINE_xxx */

#ifndef __SIP_MSG_DEFINE_xxx

/*
 *------------------------------------
 *      notify other message
 *------------------------------------
 */
    /*
    * usage   : 更新语音留言消息
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
    * usage   : 收到SMS信息
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
    * usage   : 收到华为comm-div-info类型的MESSAGE信息，用于强制呼叫前转时，页面显示
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
    * usage   :上报账号配置变更
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
    * usage   : 上报uaCSTA控制消息
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
    * usage   : 上报DATE头域值
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
    * usage   : 上报KPML订阅信息
    *          
    * msg_id  : 0x0004030e
    * target  : none
    * wparam  : line_id
    * lparam  : kpml status (0：关闭， 1：开启)
    * data_ptr: sip_kpml_info_t (lparam为0时为NULL)
    * data_len: 
    */
#define SIP_MSG_UPDATE_KPML_STATUS              SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x000e)

    /*
    * usage   : 上报SIP当前运行状态
    *          
    * msg_id  : 0x0004030f
    * target  : none
    * wparam  : service status
    * lparam  : reserved
    * data_ptr: none
    * data_len: none
    */

    #define SIP_SERVICE_STOPED                  0/* sip_start_service()接口调用之前的状态，此时状态机未跑起来 */
    #define SIP_SERVICE_NETWORK_UNAVALIABLE     1/* sip_start_service()调用后，但是当前没IP地址时会挂起，等待IP地址变化 */
    #define SIP_SERVICE_RUNING                  2/* 此时APP处于运行状态 */
    #define SIP_SERVICE_RELOADING               3/* 这是一个中间状态，用于reload APP，reload之后状态会变更为SIPUA_RUNING */
    #define SIP_SERVICE_STOPING                 4/* 协议栈停止ing过程 */

    /* 兼容旧版本定义，备注：这里修改了SIP_SERVICE_EXIT_SUCC的值，因为暂时没人使用 */
    #define SIP_SERVICE_UNKNOWN                 SIP_SERVICE_STOPED
    #define SIP_SERVICE_DESTORYING              SIP_SERVICE_RELOADING

    #define SIP_SERVICE_ERROR                   5 /*协议层内部错误*/
	#define SIP_SERVICE_EXIT_SUCC               6/*Function Run方式调用exit，上报的异步消息，通知UI SIP进程退出 */

#define SIP_MSG_UPDATE_SERVICE_STATUS           SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x000f)

    /*
    * usage   : 上报收到的自定义INFO信息
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
    * usage   : 上报apollo conference factroy uri获取状态
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
    * usage   :上报apollo会议组成员状态
    * msg_id  : 0x00040312
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_lync_conference_user_t
    * data_len: sizeof(sip_lync_conference_user_t)
    */
#define SIP_MSG_APOLLO_CONF_USER_STATE_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0012)

    /*
    * usage   :上报apollo会议描述
    * msg_id  : 0x00040313
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_apollo_conference_descrip_t
    * data_len: sizeof(sip_apollo_conference_descrip_t)
    */
#define SIP_MSG_APOLLO_CONF_DESCRIPTION_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0013)

    /*
    * usage   :上报apollo会议状态
    * msg_id  : 0x00040314
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_apollo_conference_entity_view_t
    * data_len: sizeof(sip_apollo_conference_entity_view_t)
    */
#define SIP_MSG_APOLLO_CONF_VIEW_STATE_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0014)

    /*
    * usage   :上报apollo会议预约信息（会议邀请信息推送）
    * msg_id  : 0x00040315
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_apollo_conference_entity_appoinement_t
    * data_len: sizeof(sip_apollo_conference_entity_appointment_t)
    */
#define SIP_MSG_APOLLO_CONF_APPOINTMENT_STATE_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0015)

    /*
    * usage   :上报apollo autop变更信息
    * msg_id  : 0x00040316
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: GMT(char *)
    * data_len: 
    */
#define SIP_MSG_APOLLO_AUTOP_UPDATE              SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0016)

    /*
    * usage   :上报apollo phonebook联系人变更信息
    * msg_id  : 0x00040317
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: GMT(char *)
    * data_len: 
    */
#define SIP_MSG_APOLLO_PHONE_BOOK_UPDATE              SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0017)

    /*
    * usage   :上报focus uri
    * msg_id  : 0x00040318
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: GMT(char *)
    * data_len: 
    */
#define SIP_MSG_APOLLO_CONF_FOCUS_URI                 SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0018)

    /*
    * usage   :上报source request
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
    * usage   : 上报自己的状态
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
    * usage   : 上报拨号规则
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
    * usage   : 上报LYNC MWI信息
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
    * usage   : 上报LYNC ROUTING信息
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
    * usage   :上报user infomation
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
    * usage   : 上报自己的名片信息
    * 
    * msg_id  : 0x00040325
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_lync_contact_card_t （需要使用sip_map_info_recover接口读取）
    * data_len: sip_lync_contact_card_t.size
    */
#define SIP_MSG_LYNC_CONTACT_CARD_UPDATE        SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0025)

    /*
    * usage   : 上报联系人关系
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
    * usage   : 上报联动状态
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
    * usage   : 请求syslog信息
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
    * usage   : 更新syslog信息
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
    * usage   : 上报隐私模式
    * 
    * msg_id  : 0x0004032a
    * target  : none
    * wparam  : line_id
    * lparam  : user select privacy mode:SIP_LYNC_PRIVACY_NOSELECT/PRIVATE/STANDARD
    * data_ptr: server enable privacy mode:  1:开启, 0:关闭
    * data_len: sizeof(unsigned int)
    */
#define SIP_MSG_LYNC_PRIVACY_MODE_UPDATE         SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x002a)

    /*
    * usage   :上报delegate list
    * msg_id  : 0x0004032b
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: file_path(char *)
    * data_len: strlen(file_path) + 1
    */
#define SIP_MSG_LYNC_DELEGATE_UPDATE            SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x002b)

    /*
    * usage   :上报boss list
    * msg_id  : 0x0004032c
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: file_path(char *)
    * data_len: strlen(file_path) + 1
    */
#define SIP_MSG_LYNC_BOSS_UPDATE                SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x002c)

    /*
    * usage   :上报boss line的通话状态
    * msg_id  : 0x0004032d
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: file_path(char *)
    * data_len: strlen(file_path) + 1
    */
#define SIP_MSG_LYNC_DIALOGINFO_UPDATE           SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x002d)

    /*
    * usage   :上报个性签名
    * msg_id  : 0x0004032e
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: note_buf(char *)
    * data_len: strlen(note_buf) + 1
    */
#define SIP_MSG_LYNC_NOTE_UPDATE                 SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x002e)


    /*
    * usage   :上报显示号码
    * msg_id  : 0x0004032f
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: display_num(char *)
    * data_len: strlen(display_num) + 1
    */
#define SIP_MSG_LYNC_DISPLAY_NUM_UPDATE         SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x002f)


    /*
    * usage   : 上报登入的sipuri有变
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
    * usage   :上报会议建立结果
    * msg_id  : 0x00040331
    * target  : none
    * wparam  : line_id
    * lparam  : id
    * data_ptr: sip_reason_info_t
    * data_len: 
    */
#define SIP_MSG_LYNC_CREATE_CONFERENCE_RESULT   SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0031)

    /*
    * usage   :上报会议组成员状态
    * msg_id  : 0x00040332
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_lync_conference_user_t
    * data_len: sizeof(sip_lync_conference_user_t)
    */
#define SIP_MSG_LYNC_CONF_USER_STATE_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0032)

    /*
    * usage   :上报会议状态
    * msg_id  : 0x00040333
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: path
    * data_len: sizeof(path)
    */
#define SIP_MSG_LYNC_CONF_VIEW_STATE_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0033)

    /*
    * usage   : 上报会议请求失败
    * msg_id  : 0x00040334
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_reason_info_t
    * data_len: sizeof(sip_reason_info_t)
    */
#define SIP_MSG_LYNC_CONF_REQUEST_RESULT          SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0034)

    /*
    * usage   :上报会议描述
    * msg_id  : 0x00040335
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_lync_conference_descrip_t
    * data_len: sizeof(sip_lync_conference_descrip_t)
    */
#define SIP_MSG_LYNC_CONF_DESCRIPTION_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0035)


    /*
    * usage   :上报Lync联系人状态订阅失败后的处理
    * msg_id  : 0x00040336
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: sip_lync_contacts_pres_sub_retry_t
    * data_len: sizeof(sip_lync_contacts_pres_sub_retry_t)
    */
#define SIP_MSG_LYNC_RETRY_PRES_SUB             SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0036)



    /*
    * usage   :上报 dialog info instance
    * msg_id  : 0x00040337
    * target  : none
    * wparam  : line_id
    * lparam  : 0
    * data_ptr: instance(char *)
    * data_len: strlen(instance)+1
    */
#define SIP_MSG_LYNC_DIALOGINFO_INSTANCE         SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0037)


    /*
    * usage   :上报加入会议focus的信息
    * msg_id  : 0x00040338
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_lync_conference_focus_t
    * data_len: sizeof(sip_lync_conference_focus_t)
    */
#define SIP_MSG_LYNC_CONF_FOCUS_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0038)


    /*
    * usage   :上报加入会议相关信息，如会议id，PSTN拨入式号码，web uri等等
    * msg_id  : 0x00040339
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_lync_conference_focus_t
    * data_len: sizeof(sip_lync_conference_focus_t)
    */
#define SIP_MSG_LYNC_CONF_INFO_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0039)

    /*
    * usage   :上报Skype联系人信息
    * msg_id  : 0x0004033a
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: 
    * data_len: 
    */
#define SIP_MSG_PUBLIC_CLOUD_CONTACT_UPDATE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x003a)

    /*
    * usage   :上报发布状态需要重传
    * msg_id  : 0x0004033b
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: 
    * data_len: 
    */
#define SIP_MSG_STATE_PUBLISH_RETRY     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x003b)

   /*
    * usage   :上报modify endpoint media结果
    * msg_id  : 0x0004033c
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: sip_lync_endpoint_media_t
    * data_len: 
    */
#define SIP_MSG_LYNC_CONF_MODIFY_MEDIA_RESPONSE     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x003c)

   /*
    * usage   :会议合并通话上报需要结束通话的call_id
    * msg_id  : 0x0004033d
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: merge_cid
    * data_len: sizeof(int)
    */
#define SIP_MSG_LYNC_CONF_MERGE_CALL_CLOSED     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x003d)

   /*
    * usage   :上报modify conference结果, mute_audience
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
    * usage   : 上报辅流状态，本地、远端开或者关
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
    * usage   :通知开启摄像头移动
    * msg_id  : 0x00040341
    * target  : none
    * wparam  : 0
    * lparam  : cid
    * data_ptr: i (转移方向)
    * data_len: sizeof(int)
    */
#define SIP_MSG_FECC_CAMERA_START               SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0041)

    /*
    * usage   :通知关闭摄像头移动
    * msg_id  : 0x00040342
    * target  : none
    * wparam  : 0
    * lparam  : cid
    * data_ptr: none
    * data_len: none
    */
#define SIP_MSG_FECC_CAMERA_STOP                SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0042)

    /*
    * usage   :通知摄像头位置存储
    * msg_id  : 0x00040343
    * target  : none
    * wparam  : 0
    * lparam  : cid
    * data_ptr: i (存储方向)
    * data_len: sizeof(int)
    */
#define SIP_MSG_FECC_STORE_AS_PRESET            SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0043)

    /*
    * usage   :通知摄像头位置激活
    * msg_id  : 0x00040344
    * target  : none
    * wparam  : 0
    * lparam  : cid
    * data_ptr: i (激活方向)
    * data_len: sizeof(int)
    */
#define SIP_MSG_FECC_ACTIVATE_PRESET            SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0044)


/************* FECC 0x0340-0x034F end ***********/


    /*
    * usage   : 上报诊断信息给DMS
    * msg_id  : 0x00040350
    * target  : none
    * wparam  : event id，sip_dms_event_id_e
    * lparam  : 平台标志，通过位区分不同平台
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

#define SIP_DM_PLATFORM_NORMAL          (1<<0)      /* 中性设备管理平台 */
#define SIP_DM_PLATFORM_LYNC            (1<<1)      /* Lync 平台 Telemetry */


#define SIP_MSG_DMS_INFO_REPORT                 SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0050)

/************************************************************************/
/*                   Lync msg id 0x051->0x07F start                     */
/************************************************************************/

//#define SIP_MSG_LYNC_XXX                 SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0051)

/************************************************************************/
/*                   Lync msg id 0x051->0x07F end                       */
/************************************************************************/


    /*
    * usage   : 上报SMS发送结果
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
    * usage   : 上报INFO发送结果
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
    * usage   : 上报NOTIFY发送结果
    *          
    * msg_id  : 0x00040383
    * target  : none
    * wparam  : line_id
    * lparam  : call_id(为0表示会话外NOTIFY)
    * data_ptr: sip_reason_info_t
    * data_len: 
    */

#define SIP_MSG_SEND_NOTIFY_RESULT              SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0083)

    /*
    * usage   : 携程联动：上报远端控制命令（INFO形式的联动）
    *          
    * msg_id  : 0x00040384
    * target  : none
    * wparam  : line_id
    * lparam  : call_id(为0表示会话外NOTIFY)
    * data_ptr: sip_uc_req_info_t
    * data_len: 
    */

#define SIP_MSG_UC_REQ_INFO                     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0084)

    /*
    * usage   : 携程联动：上报远端控制命令（NOTIFY形式的联动）
    *          
    * msg_id  : 0x00040385
    * target  : none
    * wparam  : line_id
    * lparam  : call_id(为0表示会话外NOTIFY)
    * data_ptr: sip_uc_req_notify_t
    * data_len: 
    */

#define SIP_MSG_UC_REQ_NOTIFY                     SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0085)

    /*
    * usage   : 自动化检测结果上报（仅适用于自动化）
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
    * usage   : 上报verizon vvm消息
    *          
    * msg_id  : 0x00040387
    * target  : none
    * wparam  : line id
    * lparam  : msg type（1-nm msg；2-mbu msg）
    * data_ptr: 如果为nm消息，那么为sip_vvm_nm_t;如果为mbu消息，那么为sip_vvm_mbu_t
    * data_len: sizeof(sip_vvm_nm_t)/sizeof(sip_vvm_mbu_t)
    */

#define SIP_MSG_VERIZON_VVM                       SIP_S2C_MSG(SIP_MSG_S2C_MSG, 0x0087)

/* 中性技术支持新增消息可优先使用如下未使用的ID 0x00040304-0x0004030a，0x0004031a-0x0004031f,0x00040380 */

/* 技术支持分支新增接口消息预留 0x080->0x0AF，Lync 预留 0x050->0x07F
   其它项目新增时，根据实际需要进行分支同步或者新增预留范围 */



#endif/* __SIP_MSG_DEFINE_xxx */

#ifndef __SIP_CMD_DEFINE_xxx
/*
 *------------------------------------
 *      notify for CMD report
 *------------------------------------
 */
    /*
    * usage   : 请求呼出新通话
    *          
    * msg_id  : 0x00040401
    * target  : none
    * wparam  : line_id
    * lparam  : cid
    * data_ptr: params(sip_ext_params_t*)
    * data_len: none
    * UI处理这个消息时候，想获取显示等信息，可以通过sip_ext_params_get提取，然后可以直接调用sip_call_invite，callee参数可以为NULL，params直接使用
    */
#define SIP_CMD_CALL_OUT                        SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0001)
    /*
    * usage   : 响应
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
    * usage   : 拒接
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
    * usage   : 请求接起当前通话
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
    * usage   : 结束通话
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
    * usage   : HOLD当前通话
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
    * usage   : 解HOLD当前通话
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
    * usage   : 要求UI发送DTMF
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
    * usage   : 通话前转
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
    * usage   : 通话后转
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
    * usage   : 加入会议
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
    * usage   : 通知UI重新Reload帐号
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
    * usage   : 通知UI检查AUTOP进行更新或者强制重启更新
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
    * usage   : 通知UI重启话机
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
    * usage   : 传递ACTION URI的body内容给UI
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
    * usage   : 传递ACTION URI的body内容给UI
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
    * usage   : 开始PCAP调试信息获取
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
    * usage   : 停止PCAP调试信息获取
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
    * usage   : 会议删除成员
    *          
    * msg_id  : 0x00040417
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_call_conference_info_t
                Lync自动化测试control_protocol_type = SIP_CONTROL_FROM_LYNC_AUTOMATION_TEST
                              mode   预留
                              parter 为要删除的成员uri
    * data_len: sizeof(sip_cmd_call_conference_info_t)
    */
#define SIP_CMD_CONFERENCE_DEL_USER             SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0017)

    /*
    * usage   : 会议mute成员
    *          
    * msg_id  : 0x00040418
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_call_conference_info_t
                Lync自动化测试control_protocol_type = SIP_CONTROL_FROM_LYNC_AUTOMATION_TEST
                              mode   1:mute 0:umute
                              parter 为要mute/umte的成员uri
    * data_len: sizeof(sip_cmd_call_conference_info_t)
    */
#define SIP_CMD_CONFERENCE_MUTE_USER            SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0018)

    /*
    * usage   : 会议mute除自己以外的其他会议成员
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
    * usage   : 添加第三方创建会议
    *          
    * msg_id  : 0x0004041a
    * target  : none
    * wparam  : line_id
    * lparam  : call_id,Lync自动化测试为0表示创建meet now会议
    * data_ptr: sip_cmd_call_conference_info_t
                Lync自动化测试control_protocol_type = SIP_CONTROL_FROM_LYNC_AUTOMATION_TEST
                              mode   预留
                              parter 为要加入的第三方,Lync自动化测试为空表示创建meet now会议
    * data_len: sizeof(sip_cmd_call_conference_info_t)
    */
#define SIP_CMD_CREAT_CONFERENCE                SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x001a)



    /*
    * usage   : 会议添加成员
    *          
    * msg_id  : 0x0004041b
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_call_conference_info_t
                Lync自动化测试control_protocol_type = SIP_CONTROL_FROM_LYNC_AUTOMATION_TEST
                              mode   预留
                              parter 为要邀请的成员uri
    * data_len: sizeof(sip_cmd_call_conference_info_t)
    */
#define SIP_CMD_CONFERENCE_ADD_USER             SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x001b)


    /*
    * usage   : park当前通话
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
    * usage   : retrieve park通话
    *
    * msg_id  : 0x0004041d
    * target  : none
    * wparam  : line_id
    * lparam  : NULL
    * data_ptr: sip_cmd_park_info_t
                Lync自动化测试control_protocol_type = SIP_CONTROL_FROM_LYNC_AUTOMATION_TEST
                mode      预留
                orbit     需要呼出的orbit字符串
    * data_len: sizeof(sip_cmd_park_info_t)
    */
#define SIP_CMD_CALL_PARK_RETRIEVE             SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x001d)

    /*
    * usage   : 通话mute成员
    *
    * msg_id  : 0x0004041e
    * target  : none
    * wparam  : line_id
    * lparam  : call_id
    * data_ptr: sip_cmd_call_conference_info_t
                Lync自动化测试control_protocol_type = SIP_CONTROL_FROM_LYNC_AUTOMATION_TEST
                              mode   1:mute 0:umute
                              parter 为要mute/umte的成员uri
    * data_len: sizeof(sip_cmd_call_conference_info_t)
    */
#define SIP_CMD_CALL_MUTE_USER                 SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x001e)


    /*
    * usage   : 通知UI重置话机
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
    * usage   : 通知UI截屏
    *          
    * msg_id  : 0x00040440
    * target  : none
    * wparam  : line_id
    * lparam  : 
    * data_ptr: NULL
    * data_len: 0
    * Note:该消息暂时不处理。微软测试架构需要将当前屏幕信息转为xml格式，具体信息格式需要微软提供。
    */
#define SIP_CMD_GET_UI_ELEMENTS                SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0040)

    /*
    * usage   : 通知UI导航到主菜单
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
    * usage   : 向UI查询状态
    *          
    * msg_id  : 0x00040442
    * target  : none
    * wparam  : line_id
    * lparam  : 检测类型
    * data_ptr: NULL
    * data_len: 0
    */
    #define SIP_PIN_LOCK_STATE		       1	/* 查看pin码锁状态,查询结果通过sip_lync_report_ui_operate_reslut通知sip，
                                                 * codec为0：解锁状态，1：锁状态*/
    #define SIP_MWI_STATE                  2    /* 查看MWI状态,查询结果通过sip_lync_report_ui_operate_reslut通知sip，
                                                 * codec为0：off状态，1：on状态*/
#define SIP_CMD_DETECTE_UI_STATE               SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0042)


    /*
    * usage   : 向UI查询联系人信息
    *          
    * msg_id  : 0x00040443
    * target  : none
    * wparam  : line_id
    * lparam  : 查询类型
    * data_ptr: (char *)contact_uri
    * data_len: strlen(contact_uri) + 1
    */
    #define SIP_LYNC_CONTACT_AVAILABILITY       1  /* 查询类型，查询结果通过sip_lync_ui_report_automation_operate_result通知SIP */
    #define SIP_LYNC_CONTACT_ACTIVITYTOKEN      2 
    #define SIP_LYNC_CONTACT_NOTE               3 
    #define SIP_LYNC_SELF_AVAILABILITY          4 

#define SIP_CMD_QUERY_CONTACT_INFORMATION       SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0043)

    /*
    * usage   : 向UI获取calendar数据
    *          
    * msg_id  : 0x00040444
    * target  : none
    * wparam  : line_id
    * lparam  : 查询类型
    * data_ptr: (char *)organizer
    * data_len: strlen(organizer) + 1
    */
#define SIP_CMD_GET_CALENDAR_ITEM               SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0044)

    /*
    * usage   : 通知UI上传syslog，操作结果通过sip_lync_ui_report_automation_operate_result通知SIP
    *
    * msg_id  : 0x00040445
    * target  : none
    * wparam  : line_id
    * lparam  : time 0:表示立即上传log文件， n:表示定时n min后上传log文件
    * data_ptr: NULL
    * data_len: 0
    */
#define SIP_CMD_UPLOAD_SYSLOG                     SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0045)

    /*
    * usage   : 通知UI进行VoiceMail相关的操作
    *
    * msg_id  : 0x00040046
    * target  : none
    * wparam  : line_id
    * lparam  : 操作类型
    * data_ptr: data_ptr=sip_cmd_voice_mail_info_t
    * data_len: sizeof(sip_cmd_voice_mail_info_t)
    */

    /* 操作类型定义 */
    #define SIP_LYNC_CLEAR_VM_MESSAGES         1 /* 操作结果通过sip_lync_ui_report_automation_operate_result通知SIP */
    #define SIP_LYNC_DELETE_VM_MESSAGE         2 /* 操作结果通过sip_lync_ui_report_automation_operate_result通知SIP */
    #define SIP_LYNC_MARK_VM_AS_READ           3 /* 操作结果通过sip_lync_ui_report_automation_operate_result通知SIP */
    #define SIP_LYNC_PLAY_VM_MESSAGE           4
    #define SIP_LYNC_CALL_VIA_VM_MESSAGE       5

#define SIP_CMD_VM_MESSAGE_OPERATION              SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0046)

    /*
    * usage   : 向UI获取VoiceMail数据信息
    *
    * msg_id  : 0x0004047
    * target  : none
    * wparam  : line_id
    * lparam  : 查询类型
    * data_ptr: NULL
    * data_len: 0
    */
    #define SIP_LYNC_GET_ALL_VM_ITEMS          1 /* 获取结果通过sip_lync_ui_report_automation_operate_result的reason通知SIP */
    #define SIP_LYNC_GET_UNREAD_VM_COUNT       2 /* 获取结果通过sip_lync_ui_report_automation_operate_result的codec通知SIP */
#define SIP_CMD_GET_VOICEMAIL_INFO        SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0047)



    /*
    * usage   : 通知UI配置PIN码锁
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
    * usage   : 通知UI修改PIN码锁
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
    * usage   : 通知UI PIN码锁上锁
    *          
    * msg_id  : 0x00040452
    * target  : none
    * wparam  : line_id
    * lparam  : 1:lock, 0:unlock
    * data_ptr: if lparam == 1, data_ptr = NULL
                if lparam == 0, data_ptr = pin码
    * data_len: 
    */
#define SIP_CMD_LOCK_PIN                       SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0052)

    /*
    * usage   : 自动化测试通知UI PIN码锁上锁
    *
    * msg_id  : 0x00040453
    * target  : none
    * wparam  : line_id
    * lparam  : 1:lock, 0:unlock
    * data_ptr: if lparam == 1, data_ptr = NULL
                if lparam == 0, data_ptr = pin码
    * data_len:
    */
#define SIP_CMD_ATT_LOCK_PIN                       SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0053)

    /*
    * usage   : 通知UI检查设备更新
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
    * usage   : 通知UI显示系统公告
    *
    * msg_id  : 0x00040461
    * target  : none
    * wparam  : none
    * lparam  : none
    * data_ptr: notice xml流数据
    * data_len: strlen(notice xml)+1
    */
#define SIP_CMD_SYSTEM_NOTICE            SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0061)


/* 技术支持分支新增接口消息预留 0x070->0x08F，Lync 预留 0x000->0x06F
   其它项目新增时，根据实际需要进行分支同步或者新增预留范围 */

/* Lync新增消息时可以优先使用如下未使用的ID
0x0004040b-0x0004040f
0x0004041f-0x0004043f
0x00040448-0x0004044f
0x00040454-0x0004045f
0x00040462-0x000404ff
*/

/************************************************************************/
/*                   中性技术支持 msg id 0x070->0x08F start             */
/************************************************************************/

//#define SIP_CMD_XXX                 SIP_S2C_MSG(SIP_MSG_S2C_CMD, 0x0070)

/************************************************************************/
/*                   中性技术支持 0x070->0x08F end                      */
/************************************************************************/


#endif/* __SIP_CMD_DEFINE_xxx */

#ifndef __SIP_REQUIRE_DEFINE_xxx
/*
 *------------------------------------
 *      notify for require
 *------------------------------------
 */
    /*
    * usage   : 获取扩展台个数
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
    * usage   : 获取一些属性值
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

