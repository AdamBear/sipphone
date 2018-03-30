#ifndef _MEDIA_COMMONDATA_H_
#define _MEDIA_COMMONDATA_H_

#include "innermsgdefine.h"

/************************************************************************/
/*
    Media 模块内部定义
*/
/************************************************************************/
#ifdef IF_SUPPORT_VIDEO

// 摄像头状态变更
enum MediaMessageDefine
{
    MEDIA_MSG_NOTIFY_BEGIN = INNER_MSG_MEDIA_MESSAGE_BEGIN,

    // 内部使用，延迟更新摄像头模式及灯状态
    MEDIA_MSG_CAMERA_UPDATE,

    MEDIA_MSG_NOTIFY_END = INNER_MSG_MEDIA_MESSAGE_END,
};

enum MediaEventType
{
    MET_NONE,
    MET_IPVP_START,         // IPVP启动完成
    MET_INCOMING,
    MET_TALK_HIDE,
    MET_TALK_START,
    MET_TALK_STOP,
    MET_TALK_RESET,
    MET_TALK_HOLD,
    MET_TALK_MUTE,
    MET_CONF_START,
    MET_CONF_STOP,
    MET_TALK_VIDEO_MUTE,
    MET_LAYOUT_CHANGE,
    MET_PREVIEW_VIDEO,
    MET_TALK_VIDEO_KEYFRAME_REQ,
    MET_TALK_VIDEO_KEYFRAME_SEND,
    MET_STATS_INFO,
    MET_CAMERA_CHANGE,      // 摄像头插拔状态改变
    MET_CONFIG_CHANGE,
    MET_CAMERA_UPDATE,      // 摄像头开关、灯状态更新
    MET_ADJUST_POWER,       // 调整能力
    MET_TALK_DESTORY,       // 通话退出
    MET_STATS_UPDATE,       // 更新数据
    MET_TALK_VIDEO_FCTRL_REQ,   // 请求比特率
    MET_TALK_AUDIO_SRTP_REQ,    // 请求比特率

    MET_BITRATE_RESET,      // 带宽重置

    MET_SHARE_OPEN,         // 打开辅流通道
    MET_SHARE_START,        // 开启辅流传输
    MET_SHARE_STOP,         // 停止辅流

    MET_TALK_VIDEO_RECV_RESIZED,    // 接收的视频分辨率更改
    MET_TALK_STOP_VIDEO,
};

struct MediaEventData
{
    MediaEventType eType;
    int wEventPara;
    int lEventPara;
    void * pExt;

    MediaEventData()
    {
        eType = MET_NONE;
        wEventPara = 0;
        lEventPara = 0;
        pExt = NULL;
    }
};

class CBaseMedia
{
public:
    CBaseMedia() {}

    virtual ~CBaseMedia() {};
public:
    virtual bool OnEvent(MediaEventData * pData)
    {
        return false;
    }
    virtual bool GetStats(int iCallId, MediaStatsInfo & statsInfo)
    {
        return false;
    }
};
#endif //#ifdef IF_SUPPORT_VIDEO
#endif // _MEDIA_COMMONDATA_H_

