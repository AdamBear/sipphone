#ifndef __PSTN_ADAPTER_MESSAGE_DEFINE_H__
#define __PSTN_ADAPTER_MESSAGE_DEFINE_H__

#ifdef IF_FEATURE_PSTN

#if WIN32
#define INNER_MSG_PSTN_MESSAGE_BEGIN    395257
#define INNER_MSG_PSTN_MESSAGE_END  395266
#endif // WIN32

#include "talk/talklogic/include/talklogic_common.h"
#include "include/innermsgdefine.h"

#define PSTN_MGR_THREAD_NAME    "PSTNAdapterManager"
#define PSTN_MGR_THREAD_HANDLE  (msgGetThreadByName(PSTN_MGR_THREAD_NAME))

enum PSTN_MSG
{
    PSTN_MSG_BEGIN = INNER_MSG_PSTN_MESSAGE_BEGIN,

    /**
    * usage    : 设置定时器
    * target   : PSTN_MGR_THREAD_HANDLE
    * wparam   : TIMER_ID
    * lparam   : TIMEOUT
    * data_ptr : none
    * data_len : none
    */
    PSTN_MSG_SET_TIMER,

    /**
    * usage    : 停止定时器
    * target   : PSTN_MGR_THREAD_HANDLE
    * wparam   : TIMER_ID
    * lparam   : none
    * data_ptr : none
    * data_len : none
    */
    PSTN_MSG_KILL_TIMER,

    /**
    * usage    : PSTN线路状态
    * target   : none
    * wparam   : iDeviceId
    * lparam   : PSTN_LINE_CONN_STATUS(PSTN_LINE_DISCONNECTED=线路断开, PSTN_LINE_CONNECTED=线路连接)
    * data_ptr : none
    * data_len : none
    */
    PSTN_MSG_LINE_CHANGED,

    /**
    * usage    : 来电
    * target   : none
    * wparam   : iDeviceId
    * lparam   : iCallId
    * data_ptr : none
    * data_len : none
    */
    PSTN_MSG_INCOMING_CALL,

    /**
    * usage    : 来电显示
    * target   : none
    * wparam   : iDeviceId
    * lparam   : iCallId
    * data_ptr : 来电显示号码
    * data_len : sizeof(data_ptr)
    */
    PSTN_MSG_INCOMING_CID,

    /**
    * usage    : 来电中断
    * target   : none
    * wparam   : iDeviceId
    * lparam   : iCallId
    * data_ptr : none
    * data_len : none
    */
    PSTN_MSG_INCOMING_CANCEL,

    /**
    * usage    : 外呼结果
    * target   : none
    * wparam   : iDeviceId
    * lparam   : iCallId
    * data_ptr : bool*  true=外呼成功，false=外呼失败
    * data_len : sizeof(bool)
    */
    PSTN_MSG_DIAL_RESULT,

    /**
    * usage    : ÅÄ²æ»É½á¹û
    * target   : none
    * wparam   : iDeviceId
    * lparam   : iCallId
    * data_ptr : bool* true=³É¹¦,false=Ê§°Ü
    * data_len : sizeof(bool)
    */
    PSTN_MSG_FLASH_RESULT,

    PSTN_MSG_END = INNER_MSG_PSTN_MESSAGE_END
};

#endif // IF_FEATURE_PSTN

#endif // !__PSTN_ADAPTER_MESSAGE_DEFINE_H__
