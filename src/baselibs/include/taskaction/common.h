#ifndef __TASKACTION_COMMON_H__
#define __TASKACTION_COMMON_H__

#include <ylstl/ylstring.h>
#include <ETLLib/ETLLib.hpp>

#   ifdef taskaction_EXPORTS
#       define TASKACTION_DLL_DECLARRE      DECLARE_EXPORT
#   else
#       define TASKACTION_DLL_DECLARRE      DECLARE_IMPORT
#   endif

//TODO. Fix Me
#ifdef _MSGKIT
#include <dsklog/log.h>
#include "dskthread/msgobject.h"
#include <dskthread/moddskthread.h>
#include "dskthread/mkthread.h"

#define  chMessageHandler CMKProcessor
#define  chThreadLocal CMKThread
#define  chThreadTimer       mkThreadTimer
#else
typedef  TASK_ID DSK_TARGET_T;
#define TA_INFO(...)   etlTrace(TRACE_LEVEL_DEBUG, __VA_ARGS__)
#define TA_DEBUG(...)  etlTrace(TRACE_LEVEL_DEBUG, __VA_ARGS__)
#define TA_ERR(...)    etlTrace(TRACE_LEVEL_DEBUG, __VA_ARGS__)
#define TA_NOTICE(...) etlTrace(TRACE_LEVEL_DEBUG, __VA_ARGS__)
#define TA_WARN(...)   etlTrace(TRACE_LEVEL_DEBUG, __VA_ARGS__)
#endif



// 一些公共定义
namespace NS_TA
{

typedef bool (*typeMsgCallBackFun)(msgObject& msgObj);
typedef void (*typeThreadMethod)(void* pData);

// TaskAction请求类型, 注意内部存在分段
enum TaskActionRequestType
{
    TA_REQUEST_NONE = 0,                        // 无意义

    /* 通用线程请求类型 */
    TA_REQUEST_COMMON_START = 1,                // 普通请求类型起始值
    TA_REQUEST_LOCAL_FILE = 2,                  // 请求本地文件
    TA_REQUEST_NETWORK_FILE = 3,                // 请求网络文件
    TA_REQUEST_RING_FILE = 4,                   // 请求铃声文件
    TA_REQUEST_SUPER_SEARCH_CB = 5,             // 超级搜索的回调
    TA_REQUEST_REMOTE_CONTACT_SEARCH = 6,       // 远程联系人搜索
    TA_REQUEST_XML_BROWSER = 7,                 // XML BROWSER
    TA_REQUEST_SAVE_LOCAL_CONTACT = 8,          // 保存或读取本地联系人配置文件
    TA_REQUEST_SIP_PIC = 9,                     // 请求下载sip附带ulr头像图片
    TA_REQUEST_REMOTE_NETWORK_FILE = 10,
    TA_PRESENCE_REQUSET = 11,
    TA_REQUEST_SIMPLEACTION = 12,
    TA_REQUEST_USBHEADSET_CTRL = 13,
    TA_REQUEST_BT = 14,
    TA_REQUEST_WIFI = 15,
    TA_REQUEST_COMMON_END = 399,

    /* 专用线程请求类型 */
    TA_REQUEST_SPECIAL_START = 400,
    TA_REQUEST_LDAP = 401,                      // 请求LDAP
    TA_REQUEST_LOCAL_CONTACT_SEARCH = 402,      // 请求搜索本地联系人
    TA_REQUEST_SUPER_SEARCH = 403,              // 超级搜索
    TA_REQUEST_STORAGE_HELP = 404,              // 存储设备辅助线程
    TA_REQUEST_EXP_UPDATE_LIST = 405,           // exp drawer update list
    TA_REQUEST_EXP_DRAWER = 406,                // exp drawer
    // lync action type
    TA_REQUEST_LYNC_CONTACT_HELPER = 600,
    TA_REQUEST_LYNC_CONTACT_HELPER_SEARCH = 601,// lync 超级搜索
    TA_REQUEST_SPECIAL_END = 799,

    TA_REQUEST_MAX = 800
};

// TaskAction的错误类型.
enum TaskActionErrorType
{
    TA_NO_ERROR = 0,  // 无错误.

    // Network layer errors.
    TA_CONNECTION_REFUSED_ERROR = 1,
    TA_REMOTE_HOST_CLOSED_ERROR = 2,
    TA_HOST_NOT_FOUND_ERROR = 3,
    TA_TIMEOUT_ERROR = 4,
    TA_OPERATION_CANCELED_ERROR = 5,
    TA_SSL_HAND_SHAKE_FAILED_ERROR = 6,
    TA_UNKNOW_NETWORK_ERROR = 7,

    // Proxy errors.
    TA_PROXY_CONNECTION_REFUSED_ERROR = 8,
    TA_PROXY_CONNECTION_CLOSED_ERROR = 9,
    TA_PROXY_NOT_FOUND_ERROR = 10,
    TA_PROXY_TIMEOUT_ERROR = 11,
    TA_PROXY_AUTHENTICATION_REQUIRED_ERROR = 12,
    TA_PROXY_UNKONWN_PROXY_ERROR = 13,

    // Content errors.
    TA_CONTENT_ACCESS_DENIED = 14,
    TA_CONTENT_OPERATION_NOT_PERMITTED_ERROR = 15,
    TA_CONTENT_NOT_FOUND_ERROR = 16,
    TA_AUTHENTICATION_REQUIRED_ERROR = 17,
    TA_UNKOWN_CONTENT_ERROR = 18,

    // Protocol errors.
    TA_PROTOCOL_UNKOWN_ERROR = 19,
    TA_PROTOCOL_INVALID_OPERATION_ERROR = 20,
    TA_PROTOCOL_FAILURE = 21,
    TA_PROTOCOL_NOT_SUPPORTED = 22,  // 不支持的协议.

    TA_MEMORY_ERROR = 40,  // 内存错误.
    TA_HAS_NO_NETWORK_REPLAY = 41,  // 无网络返回.

    // File errors.
    TA_FILE_INVALID_PATH = 42,  // 不正确的文件路径.
    TA_FILE_FAILED_TO_OPEN = 43,  // 打开文件失败.
    TA_FILE_NO_DATA_SPECIFIED_WHEN_WRITE = 44,  // 写文件时, 未指定内容.
    TA_FILE_NO_DATA_WHEN_READ = 45,  // 读取文件时, 未获取到任何内容.
    TA_FILE_OPERATION_NOT_SUPPORTED = 46,  // 读取或写入文件的操作, 未提供支持.
    TA_FILE_OPERATION_FALIED = 47,  // 读取或写入文件过程中出现错误.

    // Ldap errors.
    TA_LDAP_FAILED_TO_BIND_SERVER = 55,  // 绑定LDAP服务器失败.
    TA_LDAP_SEARCH_STRING_IS_EMPTY = 56,  // LDAP搜索字符串为空.
    TA_LDAP_SEARCH_STRING_TO_LONG = 57,  // LDAP搜索字符串过长.
    TA_LDAP_FAILED_TO_SEARCH = 58,  // 调用LDAP搜索函数出错.
    TA_LDAP_FAILED_TO_GET_REAULTS = 59,  // 调用yealink_ldap_result获取结果时出错.
    TA_LDAP_NOT_WRITABLE = 60,  // LDAP是不可写的.

    // 本地联系人搜索.
    TA_LOCAL_CONTACT_SEARCH_FAILED = 70,  // 本地联系人搜素出错.

    // 超级搜索错误.
    TA_SUPER_SEARCH_NO_KEY = 71,  // 未指定搜索关键字.
    TA_SUPER_SEARCH_INVALID_INDEX = 72,  // 不正确的下标.

    // URL错误.
    TA_URL_ERROR = 73,
    TA_URL_US_NOKTEXT = 74,
    TA_URL_BLANK_NRECOGNIZED = 75,
    TA_URL_TIMEOUT = 76,

    TA_OTHER_ERROR = 80,

    //MTSW sessionid 过期
    TA_SESSION_EXPIRED = 81,
    // 文件大小超出
    TA_FILE_EXCEED_MAX_SIZE = 82,

    TA_URL_REDIRECT = 302,
    TA_HTTP_DOWNLOAD_UNAUTHORIZED = 401,//鉴权错误

    TA_HTTP_ERR400 = 400, /* 无效查询 */
    TA_HTTP_ERR403 = 403, /* 被禁止 */
    TA_HTTP_ERR408 = 408, /* 请求超时 */
    TA_HTTP_ERR500 = 500, /* 服务器出错 */

    TA_ERROR_MAX
};

enum TaskActionPriority
{
    TA_PRIORITY_NORMAL = 0, // 优先级普通 非用户感知业务, 后台操作
    TA_PRIORITY_MEDIUM,     // 优先级中等 非用户感知业务, 但有时间要求 or 用户感知业务，但不重要
    TA_PRIORITY_HIGH,       // 优先级高   用户可感知的重要业务
};

enum TaskActionDumpType
{
    TA_DUMP_THREAD_INFO = 0,
    TA_DUMP_TASK_HISTORY
};

// 本地搜索和超级搜索复用
#define TASKACTION_THREAD_NAME_LOCALSEARCH  "thread_localsearch"
#define TASKACTION_THREAD_NAME_LDAP         "thread_LDAP"

// 删除对象
#define SAFE_DELETE(p)  \
{  \
    if ((p) != NULL)  \
    {  \
        delete (p);  \
        (p) = NULL;  \
    }  \
}

// 删除数组
#define DELETE_ARRAY(p)  \
{  \
    if ((p) != NULL)  \
    {  \
        delete [] (p);  \
        (p) = NULL;  \
    }  \
}

}  // namespace NS_TA

#endif  // __TASKACTION_COMMON_H__
