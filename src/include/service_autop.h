/* ******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2012-10-23
*
* for export APIs header file , message layer
*
****************************************************************** */
#ifndef __SRV_AUTOP_H__
#define __SRV_AUTOP_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef _MSGKIT
#include <msgkit/mkmessage.h>
#include <msgkit/mkservice.h>
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif


/************************************************************************/
/* basic message define                                                 */
/************************************************************************/

/*
* <<<section-1>>>
* user define message
*/

/* notify */
/**
*   @brief  : autop准备开始更新
    @wparam :
    @lparam :
*/
#define ATP_MSG_NOTIFY_AUTOP_START        MKSRV_MSG(MKSRV_ID_AUTOP, 1)

/**
*   @brief  : autop更新结束，UI可以根据是否需要重启执行重启
    @wparam : 0 - 更新失败，1 - 更新成功
    @lparam : 0 - 不需要重启，1 - 需要重启
*/
#define ATP_MSG_NOTIFY_AUTOP_END          MKSRV_MSG(MKSRV_ID_AUTOP, 2)

/**
*   @brief  : 询问UI，zero过程是否已结束，如果zero结束(或关闭状态)，
        UI应使用返回1
    @wparam :
    @lparam :
*/
#define ATP_MSG_REQ_ZERO_OVER             MKSRV_MSG(MKSRV_ID_AUTOP, 3)

/**
*   @brief  : 通知UI去进行RPS过程，UI收到这个消息应返回1
    @wparam :
    @lparam :
*/
#define ATP_MSG_REQ_START_RPS             MKSRV_MSG(MKSRV_ID_AUTOP, 4)

/**
*   @brief  : 询问UI RPS状态，如果正在RPS，那AUTOP会一直等待，
        UI回复1表示正在更新，2表示不在RPS更新状态
    @wparam :
    @lparam :
*/
#define ATP_MSG_REQ_RPS_STATUS             MKSRV_MSG(MKSRV_ID_AUTOP, 5)

/**
*   @brief  : 命令行触发URL更新时鉴权失败，即没有配置文件下载成功并且有出现过鉴权失败的情况
    @wparam :
    @lparam :
*/
#define ATP_MSG_NOTIFY_AUTH_ERR            MKSRV_MSG(MKSRV_ID_AUTOP, 6)
/**
*   @brief  :通知AutoP上传文件
    @wparam :上传状态，成功为0，否则为相应返回值
    @lparam :0-联系人， 1-callog
*/
#define ATP_MSG_REQ_UPLOAD_FILE_BY_INFO    MKSRV_MSG(MKSRV_ID_AUTOP, 7)
/**
*   @brief  :通知AutoP下载文件
    @wparam :下载成功为0，否则为相应返回值
    @lparam :0-联系人，1-calllog
*/
#define ATP_MSG_REQ_DOWNLOAD_FILE_BY_INFO  MKSRV_MSG(MKSRV_ID_AUTOP, 8)
/**
*   @brief  : AutoP开始下载
    @wparam :
    @lparam : 0-联系人， 1-callog
*/
#define ATP_MSG_NOTIFY_DOWNLOAD_START      MKSRV_MSG(MKSRV_ID_AUTOP, 9)
/**
*   @brief  : AutoP结束下载
    @wparam :
    @lparam : 0-联系人， 1-callog
*/
#define ATP_MSG_NOTIFY_DOWNLOAD_END        MKSRV_MSG(MKSRV_ID_AUTOP, 10)
/**
*   @brief  : 向UI发送消息，表示reset local config的结果，并告诉UI可以执行下一步reset user data
    @wparam : default atuop是否成功，0表示失败，1表示成功
    @lparam :
*/
#define ATP_MSG_RESET_USER_CFG2_USER_DATA  MKSRV_MSG(MKSRV_ID_AUTOP, 11)
/**
*   @brief  : 询问UI是否可以执行POWER ON的auotp更新，一直询问知道UI返回1
               UI回复1表示可以更新，回复其他表示继续等待
    @wparam :
    @lparam :
*/
#define ATP_MSG_REQ_START_POWER_ON_UPDATE  MKSRV_MSG(MKSRV_ID_AUTOP, 12)
/**
*   @brief  : 通知AutoP更新失败告警信息
    @wparam : atp_alert_e
    @lparam :
*/
#define ATP_MSG_NOTIFY_UPGRADE_ALERTS      MKSRV_MSG(MKSRV_ID_AUTOP, 13)

/*
* usage   :notify app_vpPhone exit auth interface
*
* wparam  :none
* lparam  :none
* data_ptr:none
* data_len:none
*
*/
#define ATP_MSG_NOTIFY_EXIT_AUTH           MKSRV_MSG(MKSRV_ID_AUTOP, 14)
/************************************************************************/
/* common define                                                        */
/************************************************************************/
#define ATP_NAMAGE_NAME       "autoServer"

#define ATP_INFO_AES_LEN      32
#define ATP_INFO_FIELD_LEN    128
#define ATP_MAX_URL_LEN       512
#define ATP_ROM_INFO_LEN      512

typedef struct __atp_info
{
    char username[ATP_INFO_FIELD_LEN];        /* 指定认证名，如果存在，将会取代URL中认证信息 */
    char password[ATP_INFO_FIELD_LEN];        /* 指定认证密码 */
    char url[ATP_MAX_URL_LEN];   /* 更新文件的URL，可以带有认证信息，也可以不带鉴权
                                                   信息，没有带认证信息则使用username和password作为
                                                   鉴权信息 */
    char AES[ATP_INFO_AES_LEN];               /* 指定com.cfg更新文件的AES 解密key*/
    char MAC_AES[ATP_INFO_AES_LEN];           /* 指定mac.cfg更新文件的MAC_AES 解密key*/
    int force_reboot;                           /* 强制更新完成后重启 */
} atp_info_t;

typedef enum __atp_status
{
    AUTOP_UNKNOWN = -1, /* 未知状态，有可能正在更新 */
    AUTOP_IDLE = 0,     /* 空闲状态 */
    AUTOP_UPDATING,     /* 正在更新 */
} atp_status_t;

typedef struct __atp_ex_rom_info
{
    char url[ATP_MAX_URL_LEN];                /* 固件地址 */
    char useruri[ATP_ROM_INFO_LEN];           /*额外数据,lync需需传入dl_uc_user_info_t*/
    char authuri[ATP_ROM_INFO_LEN];
    char passwd[ATP_ROM_INFO_LEN];
    int lid;
} atp_ex_rom_info;

typedef enum __atp_msg
{
    ATP_MSG_BEGIN           = 0,
    ATP_MSG_LIBDL_TIMEOUT   = 1,              /* 下载库超时时间 */
    ATP_MSG_END,
} atp_msg_t;

typedef enum
{
    ATP_DOWNLOAD_UNKNOWN         = -1,
    ATP_DOWNLOAD_SUC             = 0,
    ATP_DOWNLOAD_CONN_ERR,
    ATP_DOWNLOAD_AUTH_ERR,
    ATP_DOWNLOAD_SERVER_CERT_ERR,
    ATP_DOWNLOAD_CLIENT_CERT_ERR,
    ATP_DOWNLOAD_RET_UNDEFINED
} atp_download_res_e;

typedef enum __autop_alert
{
    ATP_ALERT_BEGIN           = 0,
    ATP_ALERT_UPGRADE_CONFIG   = 1,
    ATP_ALERT_UPGRADE_ROM = 2,
    ATP_ALERT_END
} atp_alert_e;


#define ATP_MAX_FILE_NAME     256
#define ATP_INFO_AUTH_LEN     128
#define ATP_MAX_INFO_URL_LEN  512


/* 使用autop_upgrade_cfg_by_extra_info接口时，option需要用以下参数
 * 如果需要使用url更新且不需要比较MD5则option设置为(ATP_BY_URL|ATP_NO_CMP_MD5)
 * 如果需要使用atp_info_t更新且不需要比较MD5则option设置为(ATP_BY_URL_INFO|ATP_NO_CMP_MD5)
 * 其他类推
 */
#define ATP_BY_URL                 0x00
#define ATP_BY_URL_INFO            (1 << 0)
#define ATP_REBOOT                 (1 << 4)
#define ATP_NO_CMP_MD5             (1 << 5)


typedef struct __atp_ex_url_info
{
    char username[ATP_INFO_AUTH_LEN]; /*指定认证名，如果存在，将会取代URL中认证信息*/
    char password[ATP_INFO_AUTH_LEN]; /*指定认密码，如果存在，将会取代URL中认证信息*/
    char url[ATP_MAX_INFO_URL_LEN];   /*上传和下载的url，可以带鉴权信息也可以不带*/
    char src_path[ATP_MAX_FILE_NAME]; /*上传文件的路径*/
    char dst_path[ATP_MAX_FILE_NAME]; /*下载文件存放路径*/
    char remote_name[ATP_MAX_FILE_NAME]; /*上传后目标文件名称，可选，如果设置等同本地文件名称*/
    int encrpty;  /*是否需要加密上传*/
    int decrpty;  /*是否需要AutoP去解密，可选，没有设置AutoP只去下载文件*/

} atp_ex_url_info;

/**
* @brief  : 触发autop使用默认方式更新，URL由autop的策略获取
* @param  : caller -调用者的handle, timeout - 等待返回结果的超时时长
* @warning:
* @return : 成功返回0
*
*/
int atp_start_by_default(mkit_handle_t caller, int timeout);

/**
* @brief  : 使用调用者指定的URL更新
* @param  : caller -调用者的handle, timeout - 等待返回结果的超时时长，URL - 指定的URL
* @warning:
* @return : 成功返回0
*/
int atp_start_by_url(mkit_handle_t caller, int timeout, const char* url);


/**
* @brief  : 使用调用都指定的atp_info_t结构内的URL等信息更新
* @param  : caller -调用者的handle, timeout - 等待返回结果的超时时长，更新源的信息
* @warning:
* @return : 成功返回0
*/
int atp_start_by_info(mkit_handle_t caller, int timeout, atp_info_t* atp_info);


/**
* @brief  : 使用指定的URL更新rom
* @param  : caller -调用者的handle, timeout - 等待返回结果的超时时长，更新源的信息
* @warning:
* @return : 成功返回0
*/
int atp_upgrade_rom(mkit_handle_t caller, int timeout, const char* url);


/**
* @brief  : 使用指定的URL与账号信息更新rom
* @param  : caller -调用者的handle, timeout - 等待返回结果的超时时长，url - 固件地址
* @param  : dl_uc_user_info_t - lync账号信息
* @warning:
* @return : 成功返回0
*/
int atp_uc_upgrade_rom_by_user_info(mkit_handle_t caller, atp_ex_rom_info* rom_info,
                                    int timeout);

/**
* @brief  : 获取autop上电更新更新状态
* @param  : caller -调用者的handle, timeout - 等待返回结果的超时时长
* @warning:
* @return : 返回0，表示上电更新未完成，1表示上电更新完成
*/
int atp_get_power_on_status(mkit_handle_t caller, int timeout);


/**
* @brief  : 获取autop是更新状态
* @param  : caller -调用者的handle, timeout - 等待返回结果的超时时长
* @warning:
* @return : 返回autop目前的更新状态，空闲返回AUTOP_IDLE
*/
atp_status_t atp_get_status(mkit_handle_t caller, int timeout);


/**
* @brief  : 通知autop定时更新的配置已更改，autop会从配置文件中重新设置定时更新策略
* @param  : caller -调用者的handle, timeout - 等待返回结果的超时时长
* @warning:
* @return : 成功返回0
*/
int atp_policy_cfg_changed(mkit_handle_t caller, int timeout);


/**
* @brief  : 通知autop配置修改，针对autop内部使用的配置
* @param  : caller -调用者的handle, timeout - 等待返回结果的超时时长，msg_type - 配置类型
* @warning:
* @return : 成功返回0
*/
int atp_inter_cfg_changed(mkit_handle_t caller, int timeout, atp_msg_t msg_type);


/**
* @brief  : 通知autop更新用户导入的配置
* @param  : caller - 调用者的handle
            timeout - 等待返回结果的超时时长
            type - 0:用户配置，1:所有修改的配置
            filepath - 配置文件路径
* @warning:
* @return : 成功返回0
*/
int atp_upgrade_usercfg(mkit_handle_t caller, int timeout, int type, const char* filepath);


/**
* @brief  : 通知autop上传用户配置
* @param  : caller - 调用者的handle
            timeout - 等待返回结果的超时时长
            type - 0:上传用户配置, 1:用户配置需要被清空，上传配置后触发AutoP更新
* @warning:
* @return : 成功返回0
*/
int atp_upload_usercfg(mkit_handle_t caller, int timeout, int type);


/**
* @brief  : 通知autop导出用户配置
* @param  : caller -调用者的handle
            timeout - 等待返回结果的超时时长
            type - 0:用户配置，1:所有修改的配置
            path - 导出的配置文件路径
* @warning:
* @return : 成功返回0
*/
int atp_export_usercfg(mkit_handle_t caller, int timeout, int type, const char* filepath);


/**
* @brief  : 通知autop DSSKey配置修改
* @param  : caller -调用者的handle
            timeout - 等待返回结果的超时时长
            type - 0:linekey, 1:memorykey，2:exp, 3:programkey
            ext_index - 扩展台的index，index从0开始
            keys - 修改的XML配置节点index列表,index从1开始
* @warning:
* @return : 成功返回0
*/
int atp_upgrade_dsskey(mkit_handle_t caller, int timeout, int type, int ext_index, const char* keys);

/**
* @brief  : 通知autop 上传文件
* @param  : caller -调用者的handle
            timeout - 等待返回结果的超时时长
            url_info -上传文件信息，需要设置上传url、本地上传文件路径src_path，上传后目标文件名称
            remote_name这个可选，如果不需要重新命名可以不设置，encrpty表示是否加密上传
* @warning:
* @return : 成功返回0
*/
int atp_upload_file(mkit_handle_t caller, atp_ex_url_info* url_info, int timeout);

/**
* @brief  : 通知autop下载文件
* @param  : caller -调用者的handle
            timeout - 等待返回结果的超时时长
            url_info - 下载文件所需要的信息，需要设置url、dst_path，需要AutoP去解密的话要设置decrpty
* @warning:
* @return : 成功返回0
*/
int atp_download_file(mkit_handle_t caller, atp_ex_url_info* url_info, int timeout);
/**
* @brief  : 通知autop更新文件
* @param  : caller -调用者的handle
            timeout - 等待返回结果的超时时长
            option - 如果需要使用url更新且不需要比较MD5则option设置为(ATP_BY_URL|ATP_NO_CMP_MD5)
                     如果需要使用atp_info_t更新且不需要比较MD5则option设置为(ATP_BY_URL_INFO|ATP_NO_CMP_MD5)
            url_info - 下载文件所需要的信息，使用url更新需要设置为url，如果使用url_info更新需要设置为atp_info_t
                       结构体
* @warning:
* @return : 成功返回0
*/
int atp_upgrade_cfg_by_extra_info(mkit_handle_t caller, void* url_info, int option, int timeout);

#ifdef __cplusplus
}
#endif

#endif /* __SRV_AUTOP_H__ */
