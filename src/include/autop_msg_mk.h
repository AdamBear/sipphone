
/********************************************************************
*
*    DESCRIPTION:Copyright(c) 2017-2025 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:shenxg@yealink.com
*
*    HISTORY:
*
*    DATE:2017-10-20
*
*******************************************************************/
#ifndef __ATP_MSG_MK_H__
#define __ATP_MSG_MK_H__

#if !USE_ETL


#include <msgkit/mkmessage.h>
#include <msgkit/mkservice.h>
#include <service_autop.h>

#define ATP_MSG_SIZE                          256
#define ATP_SRV_ID_NOTIFY                     (ATP_MSG_SIZE*0) /* 外部订阅的消息 */
#define ATP_SRV_ID_TMR                        (ATP_MSG_SIZE*1)
#define ATP_SRV_ID_REQ                        (ATP_MSG_SIZE*2)
#define ATP_MSG(sub,id)                        MKSRV_MSG(MKSRV_ID_AUTOP,(sub)+(id))


/**
*   @brief  : 触发默认顺序更新，先DHCP后静态配置URL或先静态配置URL后DHCP更新
    @wparam :
    @lparam : 2 - 强制重启
*/
#define ATP_MSG_REQ_START_BY_DEFAULT          ATP_MSG(ATP_SRV_ID_REQ, 1)

/**
*   @brief  : 通知AutoP使用指定的URL更新，URL可以带有必要的论证信息
    @wparam :
    @lparam :
    @data_ptr: URL
*/
#define ATP_MSG_REQ_START_BY_URL              ATP_MSG(ATP_SRV_ID_REQ, 2)

/**
*   @brief  : 通知AutoP使用atp_info_t的结构体协带的信息更新，
        atp_info_t结构主要可以包括一个URL、username和password，解密的KEY，强制重启等信息
    @wparam :
    @lparam :
    @data_ptr: atp_info_t
*/
#define ATP_MSG_REQ_START_BY_INFO             ATP_MSG(ATP_SRV_ID_REQ, 3)

/**
*   @brief  : 通知AutoP使用指定的URL更新固件，URL可以带有必要的论证信息
    @wparam :
    @lparam :
    @data_ptr: 固件URL
*/
#define ATP_MSG_REQ_UPGRADE_ROM               ATP_MSG(ATP_SRV_ID_REQ, 4)

/**
*   @brief  : 查询上电更新是否完成，0 - 未完成 ，1 - 已完成
    @wparam :
    @lparam :
    @data_ptr:
*/
#define ATP_MSG_REQ_GET_POWER_ON_STATUS       ATP_MSG(ATP_SRV_ID_REQ, 5)

/**
*   @brief  : 查询更新进度，是否还在更新状态，0 - 空闲 , 1 - 正在更新，-1 - 其他
    @wparam :
    @lparam :
    @data_ptr:
*/
#define ATP_MSG_REQ_GET_UPGRADE_STATUS        ATP_MSG(ATP_SRV_ID_REQ, 6)

/**
*   @brief  : 通知AutoP定时更新和周更新配置改变
    @wparam :
    @lparam :
*/
#define ATP_MSG_REQ_POLICY_CFG_CHANGED        ATP_MSG(ATP_SRV_ID_REQ, 7)
/**
*   @brief  : 通知AutoP更新用户导入的配置
    @wparam : 0 - 用户配置；1 - 所有修改的配置(V80版本)
              0 - 用户配置，如果不区分网页，导入为Phone配置，如果区分网页，导入为Web配置(V81版本)
    @lparam :
    @data_ptr: 配置文件路径
*/
#define ATP_MSG_REQ_UPGRADE_USERCFG           ATP_MSG(ATP_SRV_ID_REQ, 8)

/**
*   @brief  : 通知AutoP上传用户配置
    @wparam : 0 - 上传用户配置； 1 - 用户配置需要被清空，上传配置后触发AutoP更新(V80版本)
              0 - 上传Phone配置；1 - 上传Web配置(V81版本)
    @lparam :
*/

#define ATP_MSG_REQ_UPLOAD_USERCFG            ATP_MSG(ATP_SRV_ID_REQ, 9)

/**
*   @brief  : Lync升级固件使用的消息
    @wparam :
    @lparam :
    @data_ptr: LYNC鉴权信息，使用结构体ex_rom_info
*/
#define ATP_MSG_REQ_UPGRADE_ROM_BY_INFO       ATP_MSG(ATP_SRV_ID_REQ, 10)

/**
*   @brief  : 通知AutoP配置改变，针对Autop内部使用的配置
    @wparam : atp_msg_t 配置类型
    @lparam :
*/
#define ATP_MSG_REQ_INTER_CFG_CHANGED         ATP_MSG(ATP_SRV_ID_REQ, 11)

/**
*   @brief  : 通知AutoP导出用户配置
    @wparam : 0 - 用户配置； 1 - 所有修改的配置 (V80版本)
              0 - Phone配置；1 - Web配置；2 - AutoP配置；3 - Device配置；4 - 非Device配置；5 - 所有修改的配置 (V81版本)
    @lparam :
    @data_ptr: 导出的配置文件路径
    @data_len: 文件路径数据长度
*/
#define ATP_MSG_REQ_EXPORT_USERCFG            ATP_MSG(ATP_SRV_ID_REQ, 12)

/**
*   @brief  : 通知AutoP DSSKey配置修改
    @wparam : 0:linekey, 1:memorykey，2:exp, 3:programkey
    @lparam : 扩展台的index,index从0开始
    @data_ptr: 修改的XML配置节点index列表,index从1开始
    @data_len: 节点index列表长度
*/
#define ATP_MSG_REQ_UPGRADE_DSSKEY            ATP_MSG(ATP_SRV_ID_REQ, 13)

/**
*   @brief  : 通知AutoP下载DECT手柄固件
    @wparam : 0 - 下载固件 1 - 下载固件头
    @lparam :
*/
#define ATP_MSG_REQ_LOAD_HANDSET_ROM          ATP_MSG(ATP_SRV_ID_REQ, 14)
/**
*   @brief  : 通知AutoP清空配置 (V81版本)
    @wparam : 0 - Phone配置；1 - Web配置；2 - AutoP配置；3 - Device配置；4 - 非Device配置；5 - 所有修改的配置
    @lparam :
*/
#define ATP_MSG_REQ_CLEAR_CFG                 ATP_MSG(ATP_SRV_ID_REQ, 15)
/**
*   @brief  : 查询RPS更新进度，是否还在更新状态，0 - RPS成功 , 1 - RPS失败，2 - 正在更新
    @wparam :
    @lparam :
    @data_ptr:
*/
#define ATP_MSG_REQ_GET_RPS_UPGRADE_STATUS        ATP_MSG(ATP_SRV_ID_REQ, 16)
/**
*   @brief  : 查询UI是否返回鉴权弹框信息(AutoP工作线程用)，0 - 没有返回 , 1 - 成功获取鉴权信息返回，2 - 用户点击cancel
    @wparam :
    @lparam :
    @data_ptr:
*/
#define ATP_MSG_REQ_GET_AUTH_INFO_STATUS      ATP_MSG(ATP_SRV_ID_REQ, 17)
/**
*   @brief  : UI通知AutoP获取鉴权弹框界面的信息
    @wparam : 1- 用户点击cancel
    @lparam :
    @data_ptr: 用户鉴权信息，用户名和密码用冒号分割
*/
#define ATP_MSG_REQ_RETURN_AUTH_INFO          ATP_MSG(ATP_SRV_ID_REQ, 18)
/**
*   @brief  : 通知AutoP更新
    @wparam : 如果需要使用url更新且不需要比较MD5则option设置为(ATP_BY_URL|ATP_NO_CMP_MD5)
              如果需要使用atp_info_t更新且不需要比较MD5则option设置为(ATP_BY_URL_INFO|ATP_NO_CMP_MD5)
    @lparam :
*/
#define ATP_MSG_REQ_UPGRADE_CONFIG            ATP_MSG(ATP_SRV_ID_REQ, 19)

/* timers */
#define ATP_TMR_REPEAT_POLICY                 ATP_MSG(ATP_SRV_ID_TMR, 1)
#define ATP_TMR_WEEKLY_POLICY                 ATP_MSG(ATP_SRV_ID_TMR, 2)
#define ATP_TMR_PNP_MONITOR                   ATP_MSG(ATP_SRV_ID_TMR, 3)
#define ATP_TMR_MDNS_MONITOR                  ATP_MSG(ATP_SRV_ID_TMR, 4)
#define ATP_TMR_USER_CFG_MONITOR              ATP_MSG(ATP_SRV_ID_TMR, 5)
#define ATP_TMR_FLEXIBLE_POLICY               ATP_MSG(ATP_SRV_ID_TMR, 6)
#endif
#endif
