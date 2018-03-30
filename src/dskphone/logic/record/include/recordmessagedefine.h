#ifndef __RECORD_MESSAGE_DEFINE_H__
#define __RECORD_MESSAGE_DEFINE_H__

#include "innermsgdefine.h"

enum RecordMessageDefine
{
    RECORD_MESSAGE_BEGIN = INNER_MSG_RECORD_MESSAGE_BEGIN,

    // wparam： 0=断开,1=连接上,2=正在检测,3=无法识别,4=bt或wifi插上
    RECORD_STORAGE_STATE_CHANGE,

    // waparm： 警告值（单位分钟）
    RECORD_STORAGE_VOLUME_WARNING,

    // 录制 直到 容量满
    RECORD_STORAGE_SPACE_NOTENOUGH,

    // waparm：0=开始录制,1=停止录制,2=截图,3=暂停录制,4=恢复录制  lparam：成功与否(0/1)
    RECORD_STORAGE_RECORD_RESULT,

    // 加载存储设备的录制文件 wParam: record type  lParam: 0=开始加载, 1=加载完成
    RECORD_STORAGE_LOAD_FILE_LIST,

    // wparam：0=成功, 1=源文件不存在, 2=源文件太大, 3=data分区容量不足
    RECORD_SET_DEFAULT_WALLPAPER,

    // wparam：0=成功, 1=源文件不存在, 2=源文件太大, 3=data分区容量不足
    RECORD_SET_EXP_WALLPAPER,

    // wparam：0=成功, 1=源文件不存在, 2=源文件太大, 3=data分区容量不足
    RECORD_SET_SCREENSAVER,

    RECORD_PLAYRECORD_END,

    /////////////////////逻辑层内部使用////////////////////////////

    RECORD_MESSAGE_END = INNER_MSG_RECORD_MESSAGE_END,
};

enum Pic_Set_Result
{
    PIC_SET_RST_ERROR = -1,
    PIC_SET_RST_SUCCESS,
    PIC_SET_RST_FILE_NOT_EXIST,
    PIC_SET_RST_FILE_TOO_LARGE,
    PIC_SET_RST_PARTITION_NOT_ENOUGH,
};

#ifdef _RECORD_TEST_
enum RecordTestMessageDefine
{
    RECORDTEST_MESSAGE_BEGIN = INNER_MSG_RECORDTEST_MESSAGE_BEGIN,

    // 模拟设备插上, wparam: 0=识别成功 非0=识别失败
    RECORDTEST_MESSAGE_DEVICE_PLUS,

    // 模拟设备拔出
    RECORDTEST_MESSAGE_DEVICE_REMOVE,

    // 模拟获取设备信息
    RECORDTEST_MESSAGE_DEVICE_GET_STORAGE_INFO,

    // 模拟设备的操作
    RECORDTEST_MESSAGE_DEVICE_GET_STORAGE_STATE,

    // 模拟设备的操作
    RECORDTEST_MESSAGE_DEVICE_GET_RECORD_STATE,

    // 获取资源列表 wparam：录制类型 0=none, 1=audio, 2=video, 3=picture
    RECORDTEST_MESSAGE_DEVICE_GET_RESOURCE_LIST,

    // 删除资源列表 wparam：录制类型 0=none, 1=audio, 2=video, 3=picture; ExtData:FileName
    RECORDTEST_MESSAGE_DEVICE_DELETE_RESOURCE,

    // 设置默认壁纸
    RECORDTEST_MESSAGE_DEVICE_SET_DEFAULT_WALLPAPER,

    // 视频截图 ExtData:ExtName
    RECORDTEST_MESSAGE_DEVICE_VIDEO_SCREEN_SHOT,

    // 录制操作 wparam：录制类型 0=none, 1=audio, 2=video, 3=picture; lparam:0=stat 1=stop  ExtData:FileName
    RECORDTEST_MESSAGE_DEVICE_RECORD,

    // 播放操作 wparam：录制类型 0=none, 1=audio, 2=video, 3=picture; lparam:0=stat 1=stop 2=pause 3=resume 4=seek  ExtData:xxx
    RECORDTEST_MESSAGE_DEVICE_PLAY,

    RECORDTEST_MESSAGE_END = INNER_MSG_RECORDTEST_MESSAGE_END,
};
#endif // _RECORD_TEST_

#endif  // __RECORD_MESSAGE_DEFINE_H__
