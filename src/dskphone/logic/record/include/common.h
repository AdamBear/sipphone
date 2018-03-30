#ifndef _USB_COMMON_H_
#define _USB_COMMON_H_

#include <ylstring.h>

// 录制类型
enum RECORD_TYPE
{
    RECORD_NONE,
    RECORD_AUDIO,
    RECORD_VIDEO,
    RECORD_PICTURE,
    RECORD_PROPERTY,
};

// 恢复执行方式
enum RESUME_TYPE
{
    RESUME_GENERAL,
    RESUME_PLAY_SEEK,
};

enum RECORD_SCENE
{
    RECORD_SCENE_NONE,
    RECORD_SCENE_TALK,
    RECORD_SCENE_IDLE,
};

enum RECORD_FILE_LOAD_STATE
{
    FILE_LOAD_STATE_IDLE = 0,
    FILE_LOAD_STATE_LOADING,
};

// 操作类型
enum OPERATION_TYPE
{
    OPERATION_DELETE,           // 删除文件
    OPERATION_DELETE_ALL,       // 删除全部文件
    OPERATION_SET_DEFAULT,      // 设置为默认
    OPERATION_RECORD_START,     // 开始录音
    OPERATION_RECORD_STOP,      // 结束录音
    OPERATION_RECORD_PAUSE,     // 暂停录音
    OPERATION_RECORD_RESUME,    // 恢复录音
    OPERATION_SCREENSHOT,       // 视频截图
    OPERATION_PLAY_START,       // 开始播放
    OPERATION_PLAY_PAUSE,       // 暂停播放
    OPERATION_PLAY_RESUME,      // 恢复播放
    OPERATION_PLAY_STOP,        // 停止播放
    OPERATION_PLAY_POSITION,    // 定位播放
    OPERATION_SET_EXP_WALLPAPER,// 设置Exp壁纸
    OPERATION_SET_SCREENSAVER,  // 设置屏保图片
    OPERATION_SET_DSSEKY_DEFAULT// 设置dsskey展开壁纸
};

//// 设备的状态
//enum STORAGE_STATE
//{
//  STATE_NONE = -1,
//  STATE_DISCONNECT,   // 设备断开
//  STATE_CONNECT,      // 设置连接上
//  STATE_DETECTING,    // 设备识别中
//  STATE_UNRECOGNIZED, // 无法识别
//  STATE_OTHER_DEVICE, // 蓝牙或者wifi dongle或者PSTN Box
//};

// 设备存储信息
struct StorageInfo
{
    unsigned long long  m_ullTotal;
    unsigned long long  m_ullFree;
    unsigned long long  m_ullUsed;
    // 可录制时间，单位秒
    unsigned long       m_ulAudioRecordTime;
    unsigned long       m_ulVideoRecordTime;

    StorageInfo()
    {
        m_ullTotal = 0;
        m_ullFree = 0;
        m_ullUsed = 0;
        m_ulAudioRecordTime = 0;
        m_ulVideoRecordTime = 0;
    }
};

// 文件信息
struct RecordFileInfo
{
    yl::string strName; // 名称
    yl::string strPath; // 路径
    long lSize;         // 大小
    long lTime;         // 时长
    unsigned int lBreak;// 断点
    time_t modifyTime;  // 修改时间

    RecordFileInfo()
    {
        Reset();
    }

    void Reset()
    {
        strName = "";
        strPath = "";
        lSize = 0;
        lTime = 0;
        lBreak = 0;
        modifyTime = 0;
    }

    static bool CompareByModifyTime(const RecordFileInfo& lhs,
                                    const RecordFileInfo& rhs)
    {
        return (lhs.modifyTime > rhs.modifyTime);
    }
};
typedef YLList<RecordFileInfo>  listRecordFileInfo;

#endif // _USB_COMMON_H_
