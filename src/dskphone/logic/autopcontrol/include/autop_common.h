#ifndef _AUTOPCOMMON_H_
#define _AUTOPCOMMON_H_
#include "autop_msg_mk.h"
#include "ylstl/ylstring.h"
#include "dsklog/log.h"

enum AUTOP_UPDATE_TYPE
{
    AUT_NONE,
    AUT_ZERO,  //ZERO TOUCH
    AUT_RPS,   //RPS
    AUT_CODE,  //AUTOP CODE
    AUT_NORMAL,   //AUTOP URL 更新

#if IF_BUG_30885
    AUT_TELSTRA_OPTION66,   //
    AUT_TELSTRA_DMS,
#endif

#if IF_FEATURE_METASWITCH
    AUT_MTSW_HTTP_DIGEST
#endif // IF_FEATURE_METASWITCH
};

// 上电更新的状态
enum POWER_ON_STATUS
{
    AUTOP_POWER_ON_UPDATING,// 正在进行上电更新
    AUTOP_POWER_ON_END,     // 上电更新完成
    AUTOP_POWER_ON_WAIT,    // 等待上电完成
    AUTOP_POWER_ON_CONNECT, // 启动完成后连接
};

//更新状态
enum AUTOP_UPDATE_STATUS
{
    AUT_STATUS_NONE = -1,
    AUT_STATUS_UPDATING,
    AUT_STATUS_SUCCESS,
    AUT_STATUS_FAILED,
    AUT_STATUS_AUTH
};

enum ZeroSpDlgType
{
    ZERO_DLG_MAIN = 0,
    ZERO_DLG_STATUS,
    ZERO_DLG_NETWORK,
    ZERO_DLG_AUTOPCONFIG,
    ZERO_DLG_RESULT,
    ZERO_DLG_UNAUTHORIZED,
    ZERO_DLG_UPDATEING,
    ZERO_DLG_UPDATEFAIL,
    ZERO_DLG_UPDATEFINISH,
    ZERO_DLG_REBOOTING,
    ZERO_DLG_RPS,
    ZERO_DLG_AUTHENTICATIONAGAIN,
    ZERO_DLG_READY_REDIRECT,
    ZERO_DLG_REDIRECTOR_AUTHENTICATION,
    ZERO_DLG_ZERO_AUTHENTICATION,
    ZERO_DLG_AUTOP_AUTHENTICATION,
    ZERO_DLG_UPDATE_TIME_OUT,
    ZERO_DLG_CANCLE_AUTOP,
    ZERO_DLG_EXIT,

#if IF_FEATURE_METASWITCH
    ZERO_DLG_AUTHFAIL_PAGE,
#endif // IF_FEATURE_METASWITCH
    ZERO_DLG_CHECK_NETWORK,
    ZERO_DLG_NETWORK_FAIL,

};

// AutopCode结构体
typedef struct  AUTOP_PARAM
{
    int         iIdx;       // 索引
    yl::string  strCode;    // 呼出码
    yl::string  name; //name
    atp_info_t * lpAutoPData; // 与Autop交互的结构体

    AUTOP_PARAM()
    {
        iIdx = -1;
        strCode = "";
        name = "";
        lpAutoPData = new atp_info_t();
    }

    ~AUTOP_PARAM()
    {
        if (NULL != lpAutoPData)
        {
            delete lpAutoPData;
            lpAutoPData = NULL;
        }
    }

} * LPAUTOP_PARAM;

#endif
