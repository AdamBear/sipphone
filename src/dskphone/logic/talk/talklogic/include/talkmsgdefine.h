#ifndef __TALK_MSG_DEFINE_H__
#define __TALK_MSG_DEFINE_H__
#include <innermsgdefine.h>

/************************************************************************/
/*
    通话消息定义层次
1、Session回话：
2、Routine状态：
3、Media事件（audio&video）：
4、Audio事件：
5、Video事件：
*/
/************************************************************************/


enum TalkMessageDefine
{
    TALK_MESSAGE_BEGIN = INNER_MSG_TALK_MESSAGE_BEGIN,

    TALK_MSG_TALK_ENTER,            // 进入通话Routine，未必开启声音
    // dect 机制与中性不完全一致，因此DECT使用自己一套消息定义
    TALK_MSG_RINGING_MEDIA,         // 来电对外通知铃声参数
    TALK_MSG_RINGBACK_MEDIA,        // wParam:session id        lParam:bColorRing       extra:"VOICE PARAM" 回铃带声音参数
    TALK_MSG_TALK_START_MEDIA,      // wParam:session id        lParam:bVoiceParamChanged       extra:"VOICE PARAM"
    TALK_MSG_CALL_FINISHED,         // wParam:session id
    TALK_MSG_CALL_DELETE,           // wParam:session id        lParam:reason       extra:
    TALK_MSG_CALL_FAIL,             // wParam:session id        lParam:reason       extra:
    TALK_MSG_CALL_REPLACE,          // wParam: old session id, lParam: new call id, extra: sip user info
    TALK_MSG_BE_TRANSFERED,
    TALK_MSG_BE_FORWARDED,          // wParam old session id, lParam: new callid
    TALK_MSG_UPDATE_USERINFO,       // wParam:session id        lParam:0        extra:
    TALK_MSG_AUTO_ANSWER,           // wParam:session id        lParam:0        extra:
    TALK_MSG_RINGBACK_CHANGE_SEC183TO180,       // wParam:session id        lParam:0        extra:

    TALK_MSG_ID_CHANGE,

    // 音视频相关消息
    // 通话隐藏事件
    TALK_MSG_TALK_HIDE,

    // 通话声音开启
    TALK_MSG_TALK_START,                    // wParam:(int sessionid) lParam:(int account id)
    // 通话声音停止
    TALK_MSG_TALK_STOP,                     // wParam:(int sessionid) lParam:(int account id)
    // 通话声音重置
    TALK_MSG_TALK_RESET,                    // wParam:(int sessionid) lParam:(int account id)
    // 开启会议
    TALK_MSG_CONF_START,                    // wParam: extra:
    // 停止会议
    TALK_MSG_CONF_STOP,                     // wParam:

    // 通话声音hold
    TALK_MSG_TALK_HOLD,                     // wParam:(int callid) lParam:1-hold 0-unhold

    // 视频Mute
    TALK_MSG_VIDEO_MUTE,                    // wParam:(int callid)

    TALK_MSG_STOP_VIDEO,                    // wParam:(int callid)

    // osd 图层
    TALK_MSG_VIDEO_OSD_CHANGE,                  // extra:

    // 请求发送视频关键帧
    TALK_MSG_VIDEO_KEY_FRAME_REQUEST,               // wParam:(int callid) lParam:1-video 0-share

    // 通话焦点改变
    TALK_MSG_SESSION_FOUCS_CHANGE,             //wParam:(int session id)

    // 此路会话结束
    TALK_MSG_SESSION_DESTORY,               // wParam:(int session id) lParam:(int account id)

    // 焦点切换
    TALK_MSG_UPDATESTATS,                    //wParam:(int current talking session id), lParam(1:lost 0:good)

    TALK_MSG_BITRATE_RESET,                 // wParam:(int callid) lParam:IPVP_VIDEO_ID_SPEECH/IPVP_VIDEO_ID_SHARED 0/1 extdata:bitrate

    TALK_MSG_SHARE_OPEN,                    // wParam:(int callid) lParam:(int account id)
    TALK_MSG_SHARE_START,                   // wParam:(int callid) lParam:(int account id)
    TALK_MSG_SHARE_STOP,                        // wParam:(int callid) lParam:(int account id)

    TALK_MSG_INTERNAL_ASYNC_SIGNAL,

    TALK_MESSAGE_END = INNER_MSG_TALK_MESSAGE_END
};

#endif // __TALK_MSG_DEFINE_H__
