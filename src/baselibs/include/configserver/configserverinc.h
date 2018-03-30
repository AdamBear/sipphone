#ifndef __CONFIG_SERVER_INC_H__
#define __CONFIG_SERVER_INC_H__

#include "ylstl/ylstring.h"
#include "ylstl/yllist.h"
#include "service_dsk_define.h"

#define CONFIG_SERVER_THREAD_NAME   "configServer"
#define CONFIG_SERVER_EMPTYSTR      "%EMPTY%"
#define CONFIG_SERVER_NULLSTR       "%NULL%"
#define _CONFIGSERVER_SUPPORT_WEB 0

#ifdef IF_DEBUG_LOG
#define _CONFIGSERVER_DEBUG      1
#else
#define _CONFIGSERVER_DEBUG      0
#endif


typedef struct
{
    yl::string  name;
    yl::string  val;
    int         attr;
    int         level;
} CfgInfo;

typedef YLList<CfgInfo *> typeExportConfigList;

typedef typeExportConfigList::iterator typeExportConfigListIter;

static char scszM7Version[] = "#!version:1.0.0.1\n";

#define USER_M7_DIR_PATH    CONFIG_PATH
#define CFG_TRANSLATOR_PATH BOOT_PATH "/bin/autopTranslator.xml"


#define FILE_SYS_INI_PATH       CONFIG_PATH"system/system.ini"
#define EXPORT_TMP_DIR                  "/tmp/config"
#define FAVORITE_SETTING_FACTORY_TMP    "/tmp/config/data/favorite_setting.xml"
#define SUPERSEARCH_FACTORY_TMP         "/tmp/config/data/super_search.xml"

#define PROGRAMKEY "programablekey."


#define NEED_ENCRYPT_OR_REBOOT(attr) ((((attr) & ATTR_BASE_MASK) & ATTR_ENCRYPT) != 0 || (((attr) & ATTR_BASE_MASK) & ATTR_REBOOT) != 0)
#define NEED_ENCRYPT(attr) ((((attr) & ATTR_BASE_MASK) & ATTR_ENCRYPT) != 0)

#define IS_DEVICE_ATTR(attr) ((((attr) & ATTR_BASE_MASK) & ATTR_DEVICE) != 0)

#define IS_LINE_VALUE(attr) ((((attr) & ATTR_VAL_MASK) & ATTR_VAL_LINE) != 0)
#define IS_LINE_NEGATION(attr) ((((attr) & ATTR_VAL_MASK) & ATTR_VAL_NEGATION) != 0)
#define IS_CFG_FILE(attr)  ((((attr) & ATTR_FILE_MASK) & ATTR_FILE_CFG) != 0)
#define IS_XML_FILE(attr)  ((((attr) & ATTR_FILE_MASK) & ATTR_FILE_XML) != 0)

// 配置更改类型,和网页定义好的
#define LINEKEY_MODULE 0
#define MEMORY_MODULE 1
#define EXP_MODULE 2
#define PROGRAMKEY_MODULE 3

// 更新配置状态
#define CSV_UPDATE_CUSTOMPROTECT (1 << 0)
#define CSV_UPDATE_CUSTOMSYNC    (1 << 1)

enum CONFIG_SERVER_MSG
{
    CSM_MSG_BEGIN = 0xd0000, // 851968
    CSM_BACKUP_USERCFG,      // wparam:0, lparam:0, extraData:
    CSM_ADD_USERCFG,         // wparam:0, lparam:0, extraData: UserCfgData
    CSM_RELOAD_USERCFG,      // wparam:config level, lparam:1 autop reload extraData:
    CSM_CLEAR_USERCFG,       // wparam:0:phone,1:web,2:autop,3:device,4:non device,5,all, lparam:1:autop reload, extraData:
    CSM_EXPORT_CFGID,        // wparam:0, lparam:0, extraData:
    CSM_EXPORT_USERCFG,      // wparam:0, lparam:0 导出mac-local.cfg, 1:导出mac-all.cfg, extraData: export path
    CSM_UPLOAD_USERCFG,      // wparam:0, lparam:0, 通知autop上传用户配置
    CSM_DSSKEY_CHANGE,       // wparam:0:linekey,1:memorykey, 2:EXP, 3:programkey lparam:EXPindex, extraData: dsskey列表
    CSM_UPDATE_STATE,        // wparam:1 overwrite mode lparam :1:enable 0:disable, lparam: shift to get status extra:to be extend
    CSM_DSSKEY_CHANGEEX,     // wparam:0:linekey,1:memorykey, 2:EXP, 3:programkey lparam:EXPindex, extraData: dsskey列表 UI 使用
    CSM_CLEAR_USERDATA,      // clear user data
    CSM_UPDATE_CFG,          // wparam:config level, lparam: extra:key change info
    CSM_DSSKEY_CHANGE_CHECK,
    CSM_FLUSH_CFG_TOFILE,
    CSM_MSG_LAST,
};

#endif //__CONFIG_SERVER_INC_H__
