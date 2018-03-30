#ifndef __VOICE_COMMON_H__
#define __VOICE_COMMON_H__

#include "innermsgdefine.h"

// 通道模式分类
enum CHANNEL_MODE
{
    CHANNEL_AUTO,
    CHANNEL_IDLE,
    CHANNEL_RING,
    CHANNEL_HANDSET,
    CHANNEL_HANDFREE,
    CHANNEL_HEADSET,
    CHANNEL_GROUP,
    CHANNEL_SELECT,
    CHANNEL_2HEADSET,     // 呼叫中心双耳麦模式
    CHANNEL_SPEAKER_ONLY, // 相当于mute,只有喇叭出声,禁用mic
    CHANNEL_MIC_ONLY,     // 只启用免提的mic,禁用喇叭
    CHANNEL_BTHEADSET,    // 蓝牙通道
    CHANNEL_TONE_IDLE,
    CHANNEL_NULL,
};

// 音量类型
enum VOLUME_TYPE
{
    VT_AUTO,
    VT_RING,
    //VT_COLORRINGBACK,
    VT_TALK_HANDFREE,
    VT_TALK_HANDSET,
    VT_TALK_HEADSET,
    VT_TONE_HANDFREE,
    VT_TONE_HANDSET,
    VT_TONE_HEADSET,
    VT_GROUP,
    VT_TALK_PAGING,
};

// 真实物理通道
enum REAL_CHANNEL
{
    RC_INIT,
    RC_HANDSET,
    RC_HEADSET,
    RC_HANDFREE,
    RC_GROUP_HEADSET,
    RC_GROUP_HANDSET,
    RC_2HEADSET,
    RC_MIC_ONLY,    // 只启用mic,禁用喇叭
    RC_SPEAKER_ONLY,// 相当于mute,只有喇叭出声,禁用mic
};

// 信号音类型
enum TONE_TYPE
{
    TONE_NONE = -1,
    TONE_DIAL,        // 拨号
    TONE_RINGBACK,    // 回铃
    TONE_AUTOANSWER,  // 自动应答
    TONE_BUSY,        // 忙
    TONE_CALLWAIT1,   // 呼叫等待1,// 南非电信 http://192.168.1.99/Bug.php?BugID=19816, 这个bug原先加的是TONE_CALLWAIT_ALERT_INFO,现在改为callwaiting1,和vpm一致,原来的alert info改为callwaiting,即默认的
    TONE_CALLWAIT2,   // 呼叫等待2
    TONE_CALLWAIT3,   // 呼叫等待3
    TONE_CALLWAIT4,   // 呼叫等待4
    TONE_STUTTER,     // 嘟嘟提示音
    TONE_VOICEMAIL,   // 语音信息提示
    TONE_TEXT_MSG,    // 短信提示
    TONE_TRAN_FAILED, // tran失败   (t3x:同TONE_STUTTER)
    TONE_HOLD_ALERT,  // hold提示音 (t3x:同TONE_STUTTER)
    TONE_DIAL2,       // 拨号音2
    TONE_CALLER_WAIT,   // 呼叫等待音 http://192.168.1.99/Bug.php?BugID=12701
    TONE_CALLWAIT,
    TONE_DIAL3,       // 拨号音3  http://192.168.1.99/Bug.php?BugID=23825
    TONE_CONF,        // 会议中去电方接起后响提示音
    TONE_CONGESTION,  // congestion tone  http://192.168.1.99/Bug.php?BugID=38550
    TONE_STUTTER_DIAL,  //拨号嘟嘟提示音
};

//通话中DTMF音播放的方向
enum DTMF_DIR
{
    DTMF_DIR_NULL,     //两边都不播放
    DTMF_DIR_TOTELE,   //本地播放
    DTMF_DIR_TONET,    //对方播放
    DTMF_DIR_TOBOTH,   //两边都播放
};

enum RING_DEVICE
{
    RD_DEFAULT,
    RD_HEADSET,
    RD_HEADSET_GROUP
};

// 耳机类型
enum HEADSET_TYPE
{
    HEADSET_NORMAL = 0,
    HEADSET_BT,
    HEADSET_USB
};

#define TSG_TONE_DTMF_0         0x40
#define TSG_TONE_DTMF_1         0x41
#define TSG_TONE_DTMF_2         0x42
#define TSG_TONE_DTMF_3         0x43
#define TSG_TONE_DTMF_4         0x44
#define TSG_TONE_DTMF_5         0x45
#define TSG_TONE_DTMF_6         0x46
#define TSG_TONE_DTMF_7         0x47
#define TSG_TONE_DTMF_8         0x48
#define TSG_TONE_DTMF_9         0x49
#define TSG_TONE_DTMF_A         0x4A
#define TSG_TONE_DTMF_B         0x4B
#define TSG_TONE_DTMF_C         0x4C
#define TSG_TONE_DTMF_D         0x4D
#define TSG_TONE_DTMF_STAR      0x4E
#define TSG_TONE_DTMF_POUND     0x4F

#define DEFALUT_VOL      8
#define DEFALUT_MID_VOL  18

#define MIN_LOGIC_VOLUME 0
#define MAX_LOGIC_VOLUME 15
#define ENHANCE_VOLUME   15
#define INVALID_PAGING_VOLUME 0
#define MIN_REAL_VOLUME  8
#define MAX_REAL_VOLUME  27

#define LOCALDTMF_ONTIME_DEF   100
#define LOCALDTMF_OFFTIME_DEF  50

// 蓝牙通话模式
#define BT_IPVP_TALK_MODE   0x01

// 回调函数, 询问注册回调的模块, 切换的声道是否需要纠正
typedef void(*FunUpdateChannelInfo)(CHANNEL_MODE & eMode);

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if (p != NULL) {delete p; p=NULL;}
#endif

#define  VOICE_INFO_DEBUG 0
#if _DEBUG
#define VOICE_DBG VOICE_INFO
#else
#define VOICE_DBG
#endif
// 消息
enum VoiceMessageDefine
{
    VOICE_MESSAGE_BEGIN = INNER_MSG_VOICE_MESSAGE_BEGIN,

    // vpm start
    VOICE_VPM_START = VOICE_MESSAGE_BEGIN + 1,

    // change channel, wparam:cur channel  lparam:old channel
    VOICE_CHANGE_CHANNEL,

    // channel forbidden wparam:forbidden channel
    VOICE_CHANNEL_FORBDDEN,

    VOICE_MESSAGE_END = INNER_MSG_VOICE_MESSAGE_END
};

#endif // __VOICE_COMMON_H__
