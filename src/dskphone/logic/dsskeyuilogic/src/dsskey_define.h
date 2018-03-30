#pragma once

#include <config.h>

#define MAX_MODE_LENGTH  4 // 三级权限2位加PSTN 1位

// 第一位是三级权限隐藏属性
#define MODE_LEVEL_BROWSE_AUTH_INDEX    0
// 第二位是三级权限只读属性
#define MODE_LEVEL_WRITE_AUTH_INDEX     1
// 第三位扩展为PSTN隐藏属性
#define PSTN_BROWSE_AUTH_INDEX          2
// 第四位扩展为使用时是否需要密码
#define MODE_LEVEL_PASSWORD_AUTH_INDEX  3

#define DSS_TEMP_REGEXP_FILE  TEMP_PATH "dsskey/" "regexp.xml"
#define DSS_TEMP_FRAM_FILE  TEMP_PATH "dsskey/" "dsskey.xml"

// dsskeyuilogic 定义的模块自测宏，开启之后添加很多Log
//#define IF_DEBUG

#if 0 //def IF_DEBUG
#define DSSKEY_FACTORY_PATH  "/mnt/"
#define DSSKEY_CUSTOM_PATH   DSSKEY_FACTORY_PATH
#else
#define DSSKEY_FACTORY_PATH  RESOURCE_PATH "dsskey/"
#define DSSKEY_CUSTOM_PATH   CONFIG_DATA_PATH
#endif

#define XML_FILE_DSSKEY         DSSKEY_FACTORY_PATH "dsskey.xml"
#define XML_FILE_DSSKEY_CUSTOM  DSSKEY_CUSTOM_PATH  "dsskey_custom.xml"

#define XML_ROOT_NODE_NAME      "YealinkDsskeyConfig"
#define XML_NODE_FORMATE        "formate"
#define XML_NODE_FUNCTION       "function"
#define XML_NODE_AUTHORITY      "authority"
#define XML_NODE_DEFAULT        "default"
#define XML_NODE_ENUMERATION    "enumeration"
#define XML_NODE_SEQUENCE       "sequence"
#define XML_NODE_TIPS           "prompt"
#define XML_SUB_NODE_ITEM       "item"

#define XML_ATTR_ID             "id"
#define XML_ATTR_TYPE           "type"
#define XML_ATTR_VALUE          "value"
#define XML_ATTR_TIPS           XML_NODE_TIPS
#define XML_ATTR_MSG_ID         "msg"

#define XML_ATTR_CLASS          "class"
#define XML_ATTR_URI            "uri"
#define XML_ATTR_REDIRECT       "redirect"
#define XML_ATTR_DISPLAY_NAME   "display_name"
#define XML_ATTR_FUNCTION       XML_NODE_FUNCTION
#define XML_ATTR_VISIBLED       "visibled"
#define XML_ATTR_READONLY       "readonly"
#define XML_ATTR_IME            "ime"
#define XML_ATTR_NAME           "name"
#define XML_ATTR_PASSWD         "passwd"
#define XML_ATTR_FORMATE        XML_NODE_FORMATE
#define XML_ATTR_DEFAULT        "default"
#define XML_ATTR_RANG           "range"
#define XML_ATTR_IP             "ip"
#define XML_ATTR_BUTTON_TYPE          "button_type"

#define XML_ATTR_FUNC_LOAD      "load"
#define XML_ATTR_FUNC_SAVE      "save"
#define XML_ATTR_FUNC_STATIC    "static"
#define XML_ATTR_FUNC_DYNAMIC   "dynamic"
#define XML_ATTR_FUNC_REGEXP    "regexp"

#define XML_ITEM_KEY_LIST       "key_list"
#define XML_ITEM_KEY_EVENT      "key_event"

#define DSSKEY_MOUDLE_PROGRAMKEY    "programkey"
#define DSSKEY_MOUDLE_LINEKEY       "linekey"
#define DSSKEY_MOUDLE_EXPKEY        "expkey"

#define XML_NODE_NAME_LABEL     "label"
#define XML_NODE_NAME_VALUE     "value"
#define XML_NODE_NAME_LINE      "line"
#define XML_NODE_NAME_EXTENSION "extension"
#define XML_NODE_NAME_INDEX     "index"
#define XML_NODE_NAME_LINEVALUE "linevalue"

#define XML_BOOL_TRUE           "true"
#define XML_BOOL_FALSE          "false"

#define XML_FUNC_GET_CFG_INT     "getintvalue"
#define XML_FUNC_GET_CFG_STRING  "getstringvalue"
#define XML_FUNC_GET_PHONETYPE   "phonetype"
#define XML_FUNC_GET_ACCOUNT_NUM "accountnumber"
#define XML_FUNC_LOGIC_AND       "and"
#define XML_FUNC_LOGIC_OR        "or"
#define XML_FUNC_LOGIC_NOT       "not"

#define XML_ATTR_LOCAL_LOG      "local_log"
#define XML_ATTR_BSFT_LOG       "bsft_log"
#define XML_ATTR_REMOTE_DIR     "remote_dir"
#define XML_ATTR_LOCAL_DIR      "local_dir"
#define XML_ATTR_BSFT_DIR       "bsft_dir"
#define XML_ATTR_BLUETOOTH_DIR  "bluetooth_dir"
#define XML_ATTR_PLCM_DIR       "plcm_dir"
#define XML_ATTR_LDAP_DIR       "ldap_dir"
#define XML_ATTR_UC_DIR         "uc_dir"
#define XML_ATTR_CLOUD_DIR      "cloud_dir"
#define XML_ATTR_GB_DIR         "gb_dir"
#define XML_ATTR_MTSW_DIR       "mtsw_dir"
#define XML_ATTR_MTSW_LOG       "mtsw_log"

#define XML_FUN_SEPARATOR       ","

#include "contacts/directory/include/moddirinterface.h"


extern void DM_GetGroupDisplayName(DIR_TYPE eDirType, int nGroupId, yl::string & strDisplayName);

extern void DM_GetDsskeyBsftGroupList(YLVector<int> & vecGroupId);

// 定义比较函数
#define STR_CMP_PLUS(lValue, rValue) (strcmp(lValue, rValue) == 0)
//#define STR_CMP_PLUS (strcasecmp(lValue, rValue) == 0)


#ifdef IF_TimeTrace
#undef IF_TimeTrace
#endif

#ifdef IF_DEBUG

#include "dsklog/log.h"

#define IF_LOG_TRACE(fmt,...)  APP_INFO("fun=[%s] " fmt, __FUNCTION__, ##__VA_ARGS__)
#define IF_LINE_TRACE  APP_INFO("file=[%s] fun=[%s] line=[%d]", __FILE__, __FUNCTION__, __LINE__);

struct TimeLapse
{
    TimeLapse(const yl::string & strTips) : strTraceID(strTips)
    {
        gettimeofday(&dwStart, NULL);
    }

    void ReStart(LPCSTR chStr)
    {
        if (chStr)
        {
            struct timeval  dwEnd;
            gettimeofday(&dwEnd, NULL);
            unsigned long dwTime = 1000 * (dwEnd.tv_sec - dwStart.tv_sec) + (dwEnd.tv_usec - dwStart.tv_usec) /
                                   1000;
            gettimeofday(&dwStart, NULL);
            fprintf(stderr, "%s Time Take [%ld]ms \n", chStr, dwTime);
        }
    }

    ~TimeLapse()
    {
        struct timeval  dwEnd;
        gettimeofday(&dwEnd, NULL);
        unsigned long dwTime = 1000 * (dwEnd.tv_sec - dwStart.tv_sec) + (dwEnd.tv_usec - dwStart.tv_usec) /
                               1000;
        fprintf(stderr, "%s Time Take [%ld]ms \n", strTraceID.c_str(), dwTime);
    }

private:
    struct timeval  dwStart;
    yl::string      strTraceID;
};

#define IF_TimeTrace \
    TimeLapse ttrace(__FUNCTION__);

#endif // IF_DEBUG

#ifndef IF_TimeTrace
#define IF_TimeTrace void(0)
#endif
#ifndef IF_LOG_TRACE
#define IF_LOG_TRACE(fmt,...) void(0)
#endif
#ifndef IF_LINE_TRACE
#define IF_LINE_TRACE(fmt,...) void(0)
#endif

//#define IF_EXTERN_FUNCTION
