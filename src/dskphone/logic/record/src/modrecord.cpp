#include "modrecord.h"

#ifdef IF_USB_SUPPORT
#include "recordmanager.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

#ifdef _RECORD_TEST_
#include "test.h"
#endif

// 初始化录音模块
void modRecord_Init()
{
#ifdef _RECORD_TEST_
    _RecordTest;
#endif

    _RecordManager;
}

// 设备是否连接上
bool modRecord_IsStorageConnect()
{
    return (STATE_CONNECT == _RecordManager.GetStorageState());
}

// 设备容量是否满
bool modRecord_IsStorageFull(RECORD_TYPE eRecordType/* = RECORD_NONE*/,
                             int iId /*= STORAGE_DEFAULT*/)
{
    return _RecordManager.IsStorageFull(eRecordType, iId);
}

// 是否处于空闲状态
bool modRecord_IsRecordIdle(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return _RecordManager.IsInRecordState(STATE_IDLE, eRecordType);
}

// 是否处于正在录制状态
bool modRecord_IsRecording(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return _RecordManager.IsInRecordState(STATE_RECORDING, eRecordType);
}

// 是否处于正在暂停录制
bool modRecord_IsRecordPause(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return (_RecordManager.IsInRecordState(STATE_RECORD_PAUSE, eRecordType)
            || _RecordManager.IsInRecordState(STATE_RECORD_AUTO_PAUSE, eRecordType));
}

// 是否处于播放空闲状态
bool modRecord_IsPlayIdle(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return _RecordManager.IsInRecordState(STATE_PLAY_IDLE, eRecordType);
}

// 是否处于正在播放状态
bool modRecord_IsPlaying(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return _RecordManager.IsInRecordState(STATE_PLAYING, eRecordType);
}

// 是否处于暂停播放状态
bool modRecord_IsPlayPause(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return _RecordManager.IsInRecordState(STATE_PLAY_PAUSE, eRecordType);
}

bool modRecord_IsTalkRecording(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return _RecordManager.IsInRecordScene(RECORD_SCENE_TALK, eRecordType);
}

bool modRecord_IsIdleRecording(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return _RecordManager.IsInRecordScene(RECORD_SCENE_IDLE, eRecordType);
}

void modRecord_GetRecordTime(unsigned int & uHour, unsigned int & uMinute,
                             unsigned int & uSecond, RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    _RecordManager.GetRecordTime(eRecordType, uHour, uMinute, uSecond);
}

// 获取资源信息列表
bool modRecord_GetResourceList(listRecordFileInfo& lstFileInfo, RECORD_TYPE eRecordType,
                               int iStartPos /*= 0*/, int iNum /*= -1*/, int iId /*= STORAGE_DEFAULT*/)
{
    return _RecordManager.GetRecordFileList(lstFileInfo, eRecordType, iStartPos, iNum, iId);
}

// 通过下标获取资源信息
bool modRecord_GetResourceDetail(RecordFileInfo & fileInfo, const char * pFile,
                                 RECORD_TYPE eRecordType/* = RECORD_NONE*/,
                                 int iId /*= STORAGE_DEFAULT*/)
{
    return _RecordManager.GetRecordDetailInfo(fileInfo, pFile, eRecordType, iId);
}

// 通过下标获取资源信息
bool modRecord_GetResourceByIndex(RecordFileInfo & fileInfo, int iIndex,
                                  RECORD_TYPE eRecordType, int iId /*= STORAGE_DEFAULT*/)
{
    return _RecordManager.GetRecordInfoByIndex(fileInfo, iIndex, eRecordType, iId);
}

// 通过文件名获取资源信息
int modRecord_GetResourceByFile(RecordFileInfo & fileInfo, const char * pFile,
                                RECORD_TYPE eRecordType, int iId /*= STORAGE_DEFAULT*/)
{
    return _RecordManager.GetRecordInfoByFile(fileInfo, pFile, eRecordType, iId);
}

// 获取设备的存储信息
void modRecord_GetStorageInfo(StorageInfo & storageInfo, int iId /*= STORAGE_DEFAULT*/)
{
    _RecordManager.GetRecordVolumeInfo(storageInfo, iId);
}

// 删除某文件
bool modRecord_DeleteResoucrce(RECORD_TYPE eRecordType,
                               const char * pFile, int iId /*= STORAGE_DEFAULT*/)
{
    return _RecordManager.DoOperation(OPERATION_DELETE, eRecordType, pFile, NULL, iId);
}

// 删除全部文件
bool modRecord_DeleteAll(RECORD_TYPE eRecordType, int iId /*= STORAGE_DEFAULT*/)
{
    return _RecordManager.DoOperation(OPERATION_DELETE_ALL, eRecordType, NULL, NULL, iId);
}
// 设置为壁纸
bool modRecord_SetDefaultWallPaper(const char * pFile, int iId /*= STORAGE_DEFAULT*/)
{
    return _RecordManager.DoOperation(OPERATION_SET_DEFAULT, RECORD_PICTURE, pFile, NULL, iId);
}

// 设置Exp壁纸
bool modRecord_SetExpWallPaper(const char * pFile, int iId /*= STORAGE_DEFAULT*/)
{
    return _RecordManager.DoOperation(OPERATION_SET_EXP_WALLPAPER, RECORD_PICTURE, pFile);
}

// 设置屏保图标
bool modRecord_SetScreenSaver(const char * pFile, int iId /*= STORAGE_DEFAULT*/)
{
    return _RecordManager.DoOperation(OPERATION_SET_SCREENSAVER, RECORD_PICTURE, pFile, NULL, iId);
}

// 设置dsskey展开壁纸(T48)
bool modRecord_SetDsskeyWallPaper(const char * pFile, int iId /*= STORAGE_DEFAULT*/)
{
    return _RecordManager.DoOperation(OPERATION_SET_DSSEKY_DEFAULT, RECORD_PICTURE, pFile, NULL, iId);
}

// 视频截图
bool modRecord_Screenshot(const char * pExtName/* = NULL*/)
{
    return _RecordManager.DoOperation(OPERATION_SCREENSHOT, RECORD_VIDEO, pExtName);
}

// 开始录制
bool modRecord_RecordStart(RECORD_TYPE eRecordType, const char * pExtName/* = NULL*/,
                           RECORD_SCENE eScene/* = RECORD_SCENE_TALK*/)
{
    bool bSuccess = _RecordManager.DoOperation(OPERATION_RECORD_START, eRecordType, pExtName);
    if (bSuccess)
    {
        _RecordManager.SetRecordScene(eScene, eRecordType);
    }
    return bSuccess;
}

// 停止录制
bool modRecord_RecordStop(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return _RecordManager.DoOperation(OPERATION_RECORD_STOP, eRecordType);
}

// 暂停录制
bool modRecord_RecordPause(RECORD_TYPE eRecordType/* = RECORD_NONE*/, bool bAuto/* = false*/)
{
    return _RecordManager.DoOperation(OPERATION_RECORD_PAUSE, eRecordType, NULL, (void *)&bAuto);
}

// 恢复录制
bool modRecord_RecordResume(RECORD_TYPE eRecordType/* = RECORD_NONE*/, bool bAuto/* = false*/)
{
    return _RecordManager.DoOperation(OPERATION_RECORD_RESUME, eRecordType, NULL, (void *)&bAuto);
}

// 开始播放
bool modRecord_PlayStart(RECORD_TYPE eRecordType, const char * pFile)
{
    return _RecordManager.DoOperation(OPERATION_PLAY_START, eRecordType, pFile);
}

// 播放暂停
bool modRecord_PlayPause(RECORD_TYPE eRecordType, const char * pFile, unsigned int nSecondPos)
{
    return _RecordManager.DoOperation(OPERATION_PLAY_PAUSE, eRecordType, pFile, (void *)&nSecondPos);
}

// 播放恢复
bool modRecord_PlayResume(RECORD_TYPE eRecordType, const char * pFile,
                          RESUME_TYPE eResumeType/* = RESUME_PLAY_SEEK*/)
{
    return _RecordManager.DoOperation(OPERATION_PLAY_RESUME, eRecordType, pFile, (void *)&eResumeType);
}

// 结束播放
bool modRecord_PlayStop(RECORD_TYPE eRecordType, const char * pFile, unsigned int nSecondPos)
{
    return _RecordManager.DoOperation(OPERATION_PLAY_STOP, eRecordType, pFile, (void *)&nSecondPos);
}

// 快进快退
bool modRecord_PlaySeek(RECORD_TYPE eRecordType, const char * pFile, unsigned int nSecondPos)
{
    return _RecordManager.DoOperation(OPERATION_PLAY_POSITION, eRecordType, pFile, (void *)&nSecondPos);
}

// 是否可录制
bool modRecord_IsRecordEnable(RECORD_TYPE eRecordType)
{
    // U盘连接&容量充足&开启配置
    return (modRecord_IsStorageConnect()
            && !modRecord_IsStorageFull(eRecordType, STORAGE_DEFAULT)
            && 0 != configParser_GetConfigInt(kszUSBRecordEnable));
}

// 是否可截图
bool modRecord_IsScreenshotEnable()
{
    // U盘连接&容量充足&开启配置
    return (modRecord_IsStorageConnect()
            && !modRecord_IsStorageFull(RECORD_PICTURE, STORAGE_DEFAULT)
            && 0 != configParser_GetConfigInt(kszUSBScreenshotEnable));
}

// 文件个数
int modRecord_GetRecordFileNum(RECORD_TYPE eRecordType, int iId /*= STORAGE_DEFAULT*/)
{
    return _RecordManager.GetRecordFileNum(eRecordType, iId);
}

// USB检测是否失败
bool modRecord_DetectFailed()
{
    return _RecordManager.IsDetectFailed();
}

// 获取可录音剩余时间
yl::string modRecord_GetAvailableRecordingTime(int iId /*= STORAGE_DEFAULT*/)
{
    return _RecordManager.GetAvailableRecordingTime(iId);
}

// 将文件大小转化为字符串
yl::string modRecord_TransSizeToString(const unsigned long long & ullSize)
{
    return _RecordManager.TransSizeToString(ullSize);
}

// 将时长转化为字符串
yl::string modRecord_TransTimeToString(const unsigned long & lTime)
{
    return _RecordManager.TransTimeToString(lTime);
}

bool modRecord_IsLoadingFile(RECORD_TYPE eRecordType, int iId /*= STORAGE_DEFAULT*/)
{
    return _RecordManager.IsLoadingFile(eRecordType, iId);
}

#else // IF_USB_RECORD

void modRecord_Init()
{
}

bool modRecord_IsStorageConnect()
{
    return false;
}

bool modRecord_IsStorageFull(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return true;
}

bool modRecord_IsRecordIdle(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return false;
}

bool modRecord_IsRecording(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return false;
}

bool modRecord_IsRecordPause(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return false;
}

bool modRecord_IsPlaying(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return false;
}

bool modRecord_IsPlayPause(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return false;
}

bool modRecord_IsTalkRecording(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return false;
}

bool modRecord_IsIdleRecording(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return false;
}

void modRecord_GetRecordTime(unsigned int & uHour, unsigned int & uMinute, unsigned int & uSecond,
                             RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
}

bool modRecord_GetResourceList(listRecordFileInfo& lstFileInfo, RECORD_TYPE eRecordType,
                               int iStartPos /*= 0*/, int iNum /*= -1*/, int iId /*= STORAGE_DEFAULT*/)
{
    return false;
}

bool modRecord_GetResourceDetail(RecordFileInfo & fileInfo, const char * pFile,
                                 RECORD_TYPE eRecordType/* = RECORD_NONE*/
                                 int iId /*= STORAGE_DEFAULT*/)
{
    return false;
}

bool modRecord_GetResourceByIndex(RecordFileInfo & fileInfo, int iIndex,
                                  RECORD_TYPE eRecordType, int iId /*= STORAGE_DEFAULT*/)
{
    return false;
}

int modRecord_GetResourceByFile(RecordFileInfo & fileInfo, const char * pFile,
                                RECORD_TYPE eRecordType, int iId /*= STORAGE_DEFAULT*/)
{
    return -1;
}

void modRecord_GetStorageInfo(StorageInfo & storageInfo, int iId /*= STORAGE_DEFAULT*/)
{
}

bool modRecord_DeleteResoucrce(RECORD_TYPE eRecordType, const char * pFile
                               int iId /*= STORAGE_DEFAULT*/)
{
    return false;
}

bool modRecord_SetDefaultWallPaper(const char * pFile, int iId /*= STORAGE_DEFAULT*/)
{
    return false;
}

bool modRecord_SetDsskeyWallPaper(const char * pFile, int /*iId = STORAGE_DEFAULT*/)
{
    return false;
}

bool modRecord_Screenshot(const char * pExtName/* = NULL*/)
{
    return false;
}

bool modRecord_RecordStart(RECORD_TYPE eRecordType, const char * pExtName/* = NULL*/,
                           RECORD_SCENE eScene/* = RECORD_SCENE_TALK*/)
{
    return false;
}

bool modRecord_RecordStop(RECORD_TYPE eRecordType/* = RECORD_NONE*/)
{
    return false;
}

bool modRecord_RecordPause(RECORD_TYPE eRecordType/* = RECORD_NONE*/, bool bAuto/* = false*/)
{
    return false;
}

bool modRecord_RecordResume(RECORD_TYPE eRecordType/* = RECORD_NONE*/, bool bAuto/* = false*/)
{
    return false;
}

bool modRecord_PlayStart(RECORD_TYPE eRecordType, const char * pFile)
{
    return false;
}

bool modRecord_PlayPause(RECORD_TYPE eRecordType, const char * pFile, unsigned int nSecondPos)
{
    return false;
}

bool modRecord_PlayResume(RECORD_TYPE eRecordType, const char * pFile,
                          RESUME_TYPE eResumeType/* = RESUME_PLAY_SEEK*/)
{
    return false;
}

bool modRecord_PlayStop(RECORD_TYPE eRecordType, const char * pFile)
{
    return false;
}

bool modRecord_PlaySeek(RECORD_TYPE eRecordType, const char * pFile, unsigned int nSecondPos)
{
    return false;
}

bool modRecord_IsRecordEnable(RECORD_TYPE eRecordType)
{
    return false;
}

bool modRecord_IsScreenshotEnable()
{
    return false;
}

int modRecord_GetRecordFileNum(RECORD_TYPE eRecordType, int iId /*= STORAGE_DEFAULT*/)
{
    return 0;
}

bool modRecord_DetectFailed()
{
    return true;
}

yl::string modRecord_GetAvailableRecordingTime(int iId /*= STORAGE_DEFAULT*/)
{
    return "";
}

yl::string modRecord_TransSizeToString(const unsigned long long & ullSize)
{
    return "";
}

yl::string modRecord_TransTimeToString(const long & lTime)
{
    return "";
}

#endif // IF_USB_RECORD
