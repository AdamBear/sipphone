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
#ifndef __SERVICE_H323_EXTENSION_H__
#define __SERVICE_H323_EXTENSION_H__

#include "service_h323_interface.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief : 设置H323回调(这个时候H323以lib的形式存在,UI和H323通过API和CB进行数据交互)
* @param :
* @param :
* @return:
* @return:
* @return:
*/
typedef int (*h323_event_cb_t)(void * private_data, unsigned int msg, unsigned long wparam,
                               unsigned long lparam, const void * data, int data_siz);

/**
* @brief :由底层协议栈创建线程管理方式
* @param :
* @return: < 0 means failed
* @return: = 0 means success
* @remark:
*/
H323_DLL_DECLARE int h323_service_init2(mkit_handle_t caller, int argc, char * argv[],
                                        h323_event_cb_t cb, void * private_data);

/**
* @brief : Run方式直接调用协议栈，需要UI创建线程管理
* @param :
* @return: < 0 means failed
* @return: = 0 means success
* @remark:
*/
H323_DLL_DECLARE int h323_service_init3(mkit_handle_t caller, int argc, char * argv[],
                                        h323_event_cb_t cb, void * private_data);

/**
* @brief : 设置IPC方式
* @param : h323_ipc_method只能是SIP_SRV_IPC_METHOD_MSGKIT、SIP_SRV_IPC_METHOD_ETL、
            SIP_SRV_IPC_METHOD_FUNC或SIP_SRV_IPC_METHOD_RPC中的一个
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
* @remark:
*/
#define H323_SRV_IPC_METHOD_MSGKIT           1 << 0      /* 使用msgkit和SIP通信 */
#define H323_SRV_IPC_METHOD_ETL              1 << 1      /* 使用ETL和SIP通信 */
#define H323_SRV_IPC_METHOD_FUNC             1 << 2      /* 使用函数调用和SIP通信 */
#define H323_SRV_IPC_METHOD_RPC              1 << 3      /* 使用RPC和SIP通信 */
H323_DLL_DECLARE int h323_service_set_ipc_method(mkit_handle_t caller, int h323_ipc_method);


/**
* @brief : 通知H323配置变更(H323内部会重新Reload)
* @param : type                 [IN][INT] SETTING_TYPE
* @param : lparam               [IN][INT] reserve
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
H323_DLL_DECLARE int h323_config_change(mkit_handle_t caller, int type, int lparam);

/**
* @brief: 外部向H323导入配置(需要依赖H323的内部的配置接口文件)
* @type : 0:Phone cfg; 1:Line cfg
* @return: < 0 means failed
* @return: = 0 means success
* @return: > 0 will NOT happen
*/
#define H323_CFG_LOAD_PHONE    0
#define H323_CFG_LOAD_LINE     1
H323_DLL_DECLARE int h323_config_load(mkit_handle_t caller, int type, void * cfg, int size);


/**
* @brief: 网络配置变更
* @msg:SYS_MSG_NOTIFY_NETWORK_STATUS等
* @type:SYS_IP_PROTO_IPV4等
* @lparam:SYS_NET_STATUS_READY等
* @return: < 0 means failed
* @return: = 0 means success
*/
H323_DLL_DECLARE int h323_network_change(mkit_handle_t caller, int msg, int type, int lparam);

#ifdef __cplusplus
}
#endif

#endif

