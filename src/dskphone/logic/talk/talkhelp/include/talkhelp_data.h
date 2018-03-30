#ifndef __TALK_MOD_HELP_DATA_HEADER_H__
#define __TALK_MOD_HELP_DATA_HEADER_H__

#include "ylstring.h"

enum TalkErrorDefine
{
    TALK_ERROR_NONE = 0,                             // 没有错误
    TALK_ERROR_SIP_BEGIN = 1,                        // SIP产生的错误信息，从1开始
    TALK_ERROR_SIP_END = 0xFFFF,                   // SIP产生的错误信息，预留65535条
    TALK_ERROR_GUI_BEGIN,                   // GUI内部产生的错误信息
    TALK_ERROR_CALLOUT_SLU,                 // "Share line unavailable"     SCA占线失败
    TALK_ERROR_CALLOUT_MSE,                 // "MaxSessionError"            通话数量达到最大数量
    TALK_ERROR_CALLOUT_NU,                  // "Network Unavailable"        当前网络不可用；
    TALK_ERROR_CALLOUT_IA,                  // "Invalid Account"            呼出账号非法；
    TALK_ERROR_CALLOUT_BH,                  // "Busy Here"                  账号忙（账号状态不允许继续呼出）；
    TALK_ERROR_CALLOUT_FN,                  // "Forbidden Number"           黑名单（或者被拨号规则限制的）号码
    TALK_ERROR_CALLOUT_TO,                  // "Time Out"                  呼出超时
    TARK_ERROR_CALL_RELEASED,               // "1s"                        时间
    TALK_ERROR_CALLOUT_IN,                  // "Number is invalid!"
    TALK_ERROR_CALL_RELEASED,               // "1s"                        时间
    TALK_ERROR_CALLOUT_NOAUTH,              // "No OutGoing Auth"          没有呼出的权限
    TALK_ERROR_H323,                        // h323协议使用
    TALK_ERROR_CUSTOM_CONGESTION_TONE,      //  "自定义播放congestion tone音"
    TALK_ERROR_BT_CALLOUT_MSE,              // "MaxSessionError"            通话数量达到最大数量
    TALK_ERROR_DEFAULT = 0xFFFFFF,   // 默认规则
    TALK_ERROR_MAX,
};

struct TalkErrorInfo
{
    int         iReason;
    yl::string  strError;

    TalkErrorInfo() : iReason(TALK_ERROR_NONE), strError() {}
    TalkErrorInfo(int iRz) : iReason(iRz), strError() {}
    TalkErrorInfo(int iRz, const yl::string& strErr) : iReason(iRz), strError(strErr) {}
};



#endif
