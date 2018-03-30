/* ******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2015-01-29
*
* for export APIs header file , config serice
*
****************************************************************** */
#ifndef __SERVICE_CFG_H__
#define __SERVICE_CFG_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/
#include <msgkit/mkmessage.h>
#include <msgkit/mkservice.h>

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*
*----------------------------------------------------------------------
* micrp&limits
*----------------------------------------------------------------------
*/
#ifndef CFG_BIT
#define CFG_BIT(x)                  (1 << (x))
#endif

#define CFG_CALL_IDENTITY_NORMAL    0x0             /* normal identity caller */
#define CFG_CALL_IDENTITY_USER      0x1             /* user(custom)identity caller */
#define CFG_CALL_IDENTITY_AUTOP     0x2             /* autop  identity caller */
#define CFG_CALL_IDENTITY_TR069     0x3             /* tr069  identity caller */
#define CFG_CALL_IDENTITY_SNMP      0x4             /* snmp   identity caller */
#define CFG_CALL_IDENTITY_DSK       0x5             /* LCD-UI/desktop identity caller */
#define CFG_CALL_IDENTITY_WUI       0x6             /* WEB-UI/CGI identity caller */

#define CFG_ATTRTYPE_THREE_LEVEL    0x0             /* three level string attribute */
/*
*----------------------------------------------------------------------
* type of config item
*----------------------------------------------------------------------
*/
typedef struct cfg_item
{
    char      *     key;                        /* 配置名称 */
    int             type;                       /* 配置数据类型 */

    /* value type */
#define CFG_VALUE_TYPE_STRING       0x0             /* string */
#define CFG_VALUE_TYPE_INT          0x1             /* interger 32bit */
#define CFG_VALUE_TYPE_RAW          0x2             /* raw */

    int             attr;                       /* 配置属性 */
    /* attribute */
#define CFG_ATTR_UMASK              0x00FFFFFF      /* user attribute mask */
#define CFG_ATTR_USHIFT             0               /* user attribute shift */
#define CFG_ATTR_KMASK              0xFF000000      /* server attribute mask */
#define CFG_ATTR_KSHIFT             24              /* server attribute shift */
#define CFG_ATTR_TYPE_STRING        (CFG_VALUE_TYPE_STRING  << CFG_ATTR_KSHIFT)     /* string */
#define CFG_ATTR_TYPE_INT           (CFG_VALUE_TYPE_INT     << CFG_ATTR_KSHIFT)     /* interger */
#define CFG_ATTR_TYPE_RAW           (CFG_VALUE_TYPE_RAW     << CFG_ATTR_KSHIFT)     /* raw */

#define CFG_ATTR_REBOOT             CFG_BIT(0)      /* 配置修改是否需要重启生效 */
#define CFG_ATTR_ENCRYPT            CFG_BIT(1)      /* 配置是否加密配置 */
#define CFG_ATTR_ACTION             CFG_BIT(2)      /* 配置修改是否需要执行相应的Action，目前Action的执行尚没有加入. */
#define CFG_ATTR_FACTORY            CFG_BIT(3)      /* 配置值是否仍是出厂值 */
#define CFG_ATTR_SYNC_CLEAN         CFG_BIT(4)      /* 配置是否升级后恢复成出厂，即原先factory ini中不带有'?'的配置 */

#define CFG_ATTR_ADMIN_WRITE        CFG_BIT(5)      /* 配置权限之ADMIN写权限 */
#define CFG_ATTR_ADMIN_READ         CFG_BIT(6)      /* 配置权限之ADMIN读权限 */
#define CFG_ATTR_USER_WRITE         CFG_BIT(7)      /* 配置权限之USER写权限 */
#define CFG_ATTR_USER_READ          CFG_BIT(8)      /* 配置权限之USER读权限 */
#define CFG_ATTR_VAR_WRITE          CFG_BIT(9)      /* 配置权限之VAR写权限 */
#define CFG_ATTR_VAR_READ           CFG_BIT(10)     /* 配置权限之VAR读权限 */

    void      *      value;                     /* 配置数据 */
    size_t           value_size;                /* value size */
    void      *      reserve;                   /* reserved */
    size_t           reserve_size;              /* reserved size */
    struct cfg_item * next;
} cfg_item_t;

/*
*----------------------------------------------------------------------
* types of callback
*----------------------------------------------------------------------
*/
typedef int (*cfg_enum_cb)(const char * key, int type, void * reserve, size_t reserve_size,
                           int attr, const void * value, size_t value_size, void * private_data);
typedef void (*cfg_debug_cb)(int module, int log_level, const char * fmt, ...);


/************************************************************************/
/* basic message define                                                 */
/************************************************************************/
/***********************************************************************/
/* notification define id from 0x10000 to 0x100ff (sent by cfgserver)  */
/*                                                                     */
/* warning: cause event 0 and 1 has been used befroe,                  */
/*          CONFIG_MSG_BCAST_CHANGED=   MKSRV_MSG(MKSRV_ID_CONFIG,0)   */
/*          CONFIG_MSG_BCAST_RESET  =   MKSRV_MSG(MKSRV_ID_CONFIG,1)   */
/*          so cfgserver notify event begin with 11                    */
/***********************************************************************/

/*
* usage   : to notify that user's monitor config has changed
* id      : MKSRV_MSG(MKSRV_ID_CONFIG,11)
* target  : cfg server
* wparam  : 0(reserve)
* lparam  : exp_id, used to identiy monitory regular expression from cfg_request_monitor
* data_ptr: NULL(reserve)
* data_len: 0(reserve)
*/
#define    CFG_MSG_NOTIFY_MONITOR           MKSRV_MSG(MKSRV_ID_CONFIG,11)

/*
* usage   : to notify that server flush to DB file status
* id      : MKSRV_MSG(MKSRV_ID_CONFIG,12)
* target  : cfg server
* wparam  : 0(reserve)
* lparam  : 0/1, flush to DB file begin/flush to DB file end
* data_ptr: NULL(reserve)
* data_len: 0(reserve)
*/
#define    CFG_MSG_NOTIFY_SERVER_FLUSH      MKSRV_MSG(MKSRV_ID_CONFIG,12)

/*
* usage   : to notify that server reset config to factory status
* id      : MKSRV_MSG(MKSRV_ID_CONFIG,13)
* target  : cfg server
* wparam  : 0(reserve)
* lparam  : 0/1, reset config to factory begin/reset config to factory end
* data_ptr: NULL(reserve)
* data_len: 0(reserve)
*/
#define    CFG_MSG_NOTIFY_SERVER_RESET      MKSRV_MSG(MKSRV_ID_CONFIG,13)

/*
* usage   : to notify that server do defragmentation on share memory status
* id      : MKSRV_MSG(MKSRV_ID_CONFIG,14)
* target  : cfg server
* wparam  : 0(reserve)
* lparam  : 0/1, do defragmentation on share memory begin/do defragmentation on share memory end
* data_ptr: NULL(reserve)
* data_len: 0(reserve)
*/
#define    CFG_MSG_NOTIFY_SERVER_DEFRAG     MKSRV_MSG(MKSRV_ID_CONFIG,14)

/*
* usage   : to notify that server all config rehash status
* id      : MKSRV_MSG(MKSRV_ID_CONFIG,15)
* target  : cfg server
* wparam  : 0(reserve)
* lparam  : 0/1, all config rehash begin/all config rehash end
* data_ptr: NULL(reserve)
* data_len: 0(reserve)
*/
#define    CFG_MSG_NOTIFY_SERVER_REHASH     MKSRV_MSG(MKSRV_ID_CONFIG,15)

/************************************************************************/
/* APIs for c                                                           */
/************************************************************************/
/*
*-----------------------------------------------------------
*   debug/init APIs
*-----------------------------------------------------------
*/
/*for version*/
const char * cfg_version(void);
const char * cfg_build(void);

/**
* @brief  : use for user to init basic data of cfgserver service
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_init(void);

/**
* @brief  : use for user cleanup service data of cfgserver service
*
* @param  : none
*
* @warning:
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_exit(void);

/**
* @brief  : setup global syslog hook,if NULL or default,will NOT printf any info!
*
* @param  : module,user defined syslog module id
*           cb,syslog callback
*/
void cfg_debug(int module, cfg_debug_cb cb);

/*
*-----------------------------------------------------------
*    public APIs
*-----------------------------------------------------------
*/

/**
* @brief  : get config data, include value,attribute,type
*
* @param  : caller, user msgkit handle
*           key, config key
*           value, config value
*           value_size, value size
*           type, config value type
*           attr, config attribute
*
* @warning: param type,attr could be NULL,the other params must not be NULL.
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_get_data(mkit_handle_t caller, const char * key, void * value, size_t * value_size,
                 int * type, int * attr);

/**
* @brief  : get config string value
*
* @param  : caller, user msgkit handle
*           key, config key
*           str, to save config string value.
*           strlen, str's size
*           def_str, default value when config doesn't exist
*
* @warning: if config exist but is "", str will be is "";
            if config doesn't exist, str will set to def_str
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_get_string(mkit_handle_t caller, const char * key, char * str, size_t str_len,
                   const char * def_str);

/**
* @brief  : get config attribute
*
* @param  : caller, user msgkit handle
*           key, config key
*           attr, to save config attribute.
*           def_attr, default attribute when config doesn't exist
*
* @warning: if config doesn't exist, attr will set to def_attr
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_get_attr(mkit_handle_t caller, const char * key, int * attr, const int def_attr);

/**
* @brief  : get config interger value
*
* @param  : caller, user msgkit handle
*           key, config key
*           val, to save config interger value.
*           def_attr, default interger value when config doesn't exist
*
* @warning: if config doesn't exist or not interger, val will set to def_val
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_get_int(mkit_handle_t caller, const char * key, int * val, const int def_val);

/**
* @brief  : set config string value
*
* @param  : caller, user msgkit handle
*           key, config key
*           str, string value to set.
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_set_string(mkit_handle_t caller, const char * key, char * str);

/**
* @brief  : set config interger value
*
* @param  : caller, user msgkit handle
*           key, config key
*           val, val value to set.
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_set_int(mkit_handle_t caller, const char * key, int val);

/**
* @brief  : set config string value by identity user
*
* @param  : caller, user msgkit handle
*           identity, caller identity
*           key, config key
*           str, string value to set.
*
* @exp    : cfg_identity_set_string(caller, CFG_CALL_IDENTITY_USER,     "syslog.log_level", "5")
*           cfg_identity_set_string(caller, CFG_CALL_IDENTITY_NORMAL, "syslog.log_level", "4")
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_identity_set_string(mkit_handle_t caller, int identity, const char * key, char * str);

/**
* @brief  : set config interger value by identity user
*
* @param  : caller, user msgkit handle
*           identity, caller identity
*           key, config key
*           val, val value to set.
*
* @exp    : cfg_identity_set_int(caller, CFG_CALL_IDENTITY_USER,     "syslog.log_level", 5)
*           cfg_identity_set_int(caller, CFG_CALL_IDENTITY_NORMAL, "syslog.log_level", 4)
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_identity_set_int(mkit_handle_t caller, int identity, const char * key, int val);

/**
* @brief  : get config factory data, include value,attribute,type
*
* @param  : caller, user msgkit handle
*           key, config key
*           value, config factory value
*           value_size, value size
*           type, config value type
*           attr, config factory attribute
*
* @warning: 'type','attr' could be NULL,the other params must not be NULL.
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_get_factory_data(mkit_handle_t caller, const char * key, void * value, size_t * value_size,
                         int * type, int * attr);

/**
* @brief  : get config factory string value
*
* @param  : caller, user msgkit handle
*           key, config key
*           str, to save config factory string value.
*           strlen, str's size
*           def_str, default string value when config doesn't exist in factory
*
* @warning: if config exist in factory but is "", str will set to "";
            if config doesn't exist in factory, str will set to def_str
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_get_factory_string(mkit_handle_t caller, const char * key, char * str, size_t str_len,
                           const char * def_str);

/**
* @brief  : get config factory attribute
*
* @param  : caller, user msgkit handle
*           key, config key
*           attr, to save config factory attribute.
*           def_attr, default attribute when config doesn't exist in factory
*
* @warning: if config doesn't exist in factory, attr will set to def_attr
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_get_factory_attr(mkit_handle_t caller, const char * key, int * attr, const int def_attr);

/**
* @brief  : get config factory interger value
*
* @param  : caller, user msgkit handle
*           key, config key
*           val, to save config factory interger value.
*           def_attr, default interger value when config doesn't exist in factory
*
* @warning: if config doesn't exist or not interger in factory, val will set to def_val
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_get_factory_int(mkit_handle_t caller, const char * key, int * val, const int def_val);

/**
* @brief  : get/set/getfac config attribute by transform string, such as Three Level Permission
*
* @param  : caller, user msgkit handle
*           key, config key
*           attrtype, attribute type, such as CFG_ATTRTYPE_THREE_LEVEL
*           attrstr, attribute string to save/set.
*           attrstr_len, attrstr's size
*           def_attrstr, default attribute string
*
* @warning: if config doesn't exist, attr will set to def_attr.
*           MUST know that mulit string attribute mapping to the same one three level permission,
*           such as "11" and "12" means "WR R N", it will return the first matched string attribute when get.
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_get_attrstr(mkit_handle_t caller, const char * key, int attrtype, char * attrstr,
                    size_t attrstr_len, const char * def_attrstr);
int cfg_set_attrstr(mkit_handle_t caller, const char * key, int attrtype, char * attrstr);
int cfg_get_factory_attrstr(mkit_handle_t caller, const char * key, int attrtype, char * attrstr,
                            size_t attrstr_len, const char * def_attrstr);

/**
* @brief  : enum items by matching regular expression
*
* @param  : caller, user msgkit handle
*           exp, wildcard match or regular expression string @exp: "syslog.*", "^syslog\\."
*           cb, enum callback
*           private_data, private data for enum callback
*
* @warning: !!user MUST not call any other cfg service API in callback!!
*           Note that if exp only have number(0-9)/char(a-zA-Z)/'.'/'_'/'?'/'*', sucn as "syslog.*"
*           it will be explained as wildcard match, Otherwise it will be explained as
*           regular expression such as "^syslog\\."
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_enum_item(mkit_handle_t caller, const char * exp, cfg_enum_cb cb, void * private_data);

/**
* @brief  : find all items' data by matching regular expression
*
* @param  : caller, user msgkit handle
*           exp, wildcard match or regular expression string @exp: "syslog.*", "^syslog\\."
*           item, to save matched configs' data
*
* @warning: item list alloc in service library, need to be free when useless.
*           call cfg_free_item to free item.
*           Note that if exp only have number(0-9)/char(a-zA-Z)/'.'/'_'/'?'/'*', sucn as "syslog.*"
*           it will be explained as wildcard match, Otherwise it will be explained as
*           regular expression such as "^syslog\\."
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_find_item(mkit_handle_t caller, const char * exp, cfg_item_t ** item);

/**
* @brief  : free item list
*
* @param  : caller, user msgkit handle
*           item, item list head
*
* @warning:
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_free_item(mkit_handle_t caller, cfg_item_t ** item);

/**
* @brief  : request monitor configs by matching regular expression
*
* @param  : caller, user msgkit handle
*           exp, wildcard match or regular expression string @exp: "syslog.*", "^syslog\\."
*           exp_id, for user identity param 'exp'
*
* @warning: exp_id defined by user self.
*           Note that if exp only have number(0-9)/char(a-zA-Z)/'.'/'_'/'?'/'*', sucn as "syslog.*"
*           it will be explained as wildcard match, Otherwise it will be explained as
*           regular expression such as "^syslog\\."
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_request_monitor(mkit_handle_t caller, const char * exp, int exp_id);

/**
* @brief  : request monitor configs by matching regular expression
*
* @param  : caller, user msgkit handle
*           exp, wildcard match or regular expression string @exp: "syslog.*", "^syslog\\."
*
* @warning: if exp is NULL,means cancel all config's monitor.
*           Note that if exp only have number(0-9)/char(a-zA-Z)/'.'/'_'/'?'/'*', sucn as "syslog.*"
*           it will be explained as wildcard match, Otherwise it will be explained as
*           regular expression such as "^syslog\\."
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_cancel_monitor(mkit_handle_t caller, const char * exp);

/**
* @brief  : request/cancel cfgserver's notify event
*
* @param  : event, server status notify event, support
*               CFG_MSG_NOTIFY_SERVER_FLUSH,CFG_MSG_NOTIFY_SERVER_RESET,
*               CFG_MSG_NOTIFY_SERVER_DEFRAG,CFG_MSG_NOTIFY_SERVER_REHASH
*               so far.
*
* @exp    : cfg_request_notify(caller, CFG_MSG_NOTIFY_SERVER_FLUSH)
*           cfg_request_notify(caller, -1)
*           cfg_cancel_notify (caller, -1)
*
* @warning: if set event as -1, means requeset/cancel notify all status event
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_request_notify(mkit_handle_t caller, int event);
int cfg_cancel_notify(mkit_handle_t caller, int event);

/**
* @brief  : export/recover/cleanup identity config data
*
* @param  : caller, user msgkit handle
*           identity, caller identity
*           item, to save identity export/recover/cleanup data
*           clean, 0/1 if clean modify flag, for judging new modify since last export
*
* @exp    : cfg_export_identity_data (caller, CFG_CALL_IDENTITY_USER, &list_head, 1);
*           cfg_recover_identity_data(caller, CFG_CALL_IDENTITY_USER, &list_head);
*           cfg_cleanup_identity_data(caller, CFG_CALL_IDENTITY_USER, &list_head);
*
* @warning: item list alloc in service library, need to be free when useless.
*           config value changed to factory value when cleanup identity .
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_export_identity_data(mkit_handle_t caller, int identity, cfg_item_t ** item, int clean);
int cfg_recover_identity_data(mkit_handle_t caller, int identity, cfg_item_t ** item);
int cfg_cleanup_identity_data(mkit_handle_t caller, int identity, cfg_item_t ** item);

/**
* @brief  : save config data immediately
*
* @param  : caller, user msgkit handle
*
* @warning: MUST called before reboot.
*
* @return : 0 as success,otherwise failed.
*
*/
int cfg_flush_to_file(mkit_handle_t caller);
/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __SERVICE_CFG_H__ */

