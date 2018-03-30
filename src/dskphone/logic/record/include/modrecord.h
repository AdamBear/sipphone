#ifndef __MOD_RECORD_H__
#define __MOD_RECORD_H__

#include <yllist.h>
#include "../include/common.h"
#include "hotplug/commondefine.h"

// 初始化录制模块
void modRecord_Init();

// 设备是否连接上
bool modRecord_IsStorageConnect();

// 设备容量是否满
bool modRecord_IsStorageFull(RECORD_TYPE eRecordType = RECORD_NONE, int iId = STORAGE_DEFAULT);

// 是否处于空闲状态
bool modRecord_IsRecordIdle(RECORD_TYPE eRecordType = RECORD_NONE);

// 是否处于正在录制状态
// 录制类型为RECORD_NONE, 则三种类型任一个处于录制状态返回true
bool modRecord_IsRecording(RECORD_TYPE eRecordType = RECORD_NONE);

// 是否处于正在暂停录制
bool modRecord_IsRecordPause(RECORD_TYPE eRecordType = RECORD_NONE);

// 是否处于播放空闲状态
bool modRecord_IsPlayIdle(RECORD_TYPE eRecordType = RECORD_NONE);

// 是否处于正在播放状态
// 录制类型为RECORD_NONE, 则三种类型任一个处于播放状态返回true
bool modRecord_IsPlaying(RECORD_TYPE eRecordType = RECORD_NONE);

// 是否处于暂停播放状态
// 录制类型为RECORD_NONE, 则三种类型任一个处于暂停状态返回true
bool modRecord_IsPlayPause(RECORD_TYPE eRecordType = RECORD_NONE);

// 是否处于通话录制状态
bool modRecord_IsTalkRecording(RECORD_TYPE eRecordType = RECORD_NONE);

// 是否处于Idle录制状态
bool modRecord_IsIdleRecording(RECORD_TYPE eRecordType = RECORD_NONE);

// 获取录制时长
void modRecord_GetRecordTime(unsigned int & uHour, unsigned int & uMinute, unsigned int & uSecond,
                             RECORD_TYPE eRecordType = RECORD_NONE);

// 获取资源信息列表
bool modRecord_GetResourceList(listRecordFileInfo& lstFileInfo, RECORD_TYPE eRecordType,
                               int iStartPos = 0, int iNum = -1, int iId = STORAGE_DEFAULT);

// 通过下标获取资源的详细信息
bool modRecord_GetResourceDetail(RecordFileInfo & fileInfo, const char * pFile,
                                 RECORD_TYPE eRecordType = RECORD_NONE, int iId = STORAGE_DEFAULT);

// 通过下标获取资源信息
bool modRecord_GetResourceByIndex(RecordFileInfo & fileInfo, int iIndex,
                                  RECORD_TYPE eRecordType, int iId = STORAGE_DEFAULT);

// 通过文件名获取资源信息
int modRecord_GetResourceByFile(RecordFileInfo & fileInfo, const char * pFile,
                                RECORD_TYPE eRecordType, int iId = STORAGE_DEFAULT);

// 获取设备的存储信息
void modRecord_GetStorageInfo(StorageInfo & storageInfo, int iId = STORAGE_DEFAULT);

// 删除某文件
bool modRecord_DeleteResoucrce(RECORD_TYPE eRecordType,
                               const char * pFile, int iId = STORAGE_DEFAULT);

// 删除全部文件
bool modRecord_DeleteAll(RECORD_TYPE eRecordType, int iId = STORAGE_DEFAULT);

// 设置为壁纸
bool modRecord_SetDefaultWallPaper(const char * pFile, int iId = STORAGE_DEFAULT);

// 设置Exp壁纸
bool modRecord_SetExpWallPaper(const char * pFile, int iId = STORAGE_DEFAULT);

// 设置屏保图标
bool modRecord_SetScreenSaver(const char * pFile, int iId = STORAGE_DEFAULT);

// 设置dsskey展开壁纸(T48)
bool modRecord_SetDsskeyWallPaper(const char * pFile, int iId = STORAGE_DEFAULT);

// 视频截图
bool modRecord_Screenshot(const char * pExtName = NULL);

// 开始录制
bool modRecord_RecordStart(RECORD_TYPE eRecordType, const char * pExtName = NULL,
                           RECORD_SCENE eScene = RECORD_SCENE_TALK);

// 停止录制
// 目前录制针对全局, RECORD_NONE表示停掉所有录制
bool modRecord_RecordStop(RECORD_TYPE eRecordType = RECORD_NONE);

// 暂停录制
bool modRecord_RecordPause(RECORD_TYPE eRecordType = RECORD_NONE, bool bAuto = false);

// 恢复录制
bool modRecord_RecordResume(RECORD_TYPE eRecordType = RECORD_NONE, bool bAuto = false);

// 开始播放
bool modRecord_PlayStart(RECORD_TYPE eRecordType, const char * pFile);

// 播放暂停
bool modRecord_PlayPause(RECORD_TYPE eRecordType, const char * pFile, unsigned int nSecondPos);

// 播放恢复
bool modRecord_PlayResume(RECORD_TYPE eRecordType, const char * pFile,
                          RESUME_TYPE eResumeType = RESUME_PLAY_SEEK);

// 结束播放
bool modRecord_PlayStop(RECORD_TYPE eRecordType, const char * pFile, unsigned int nSecondPos = 0);

// 快进快退
bool modRecord_PlaySeek(RECORD_TYPE eRecordType, const char * pFile, unsigned int nSecondPos);

// 是否可录制
bool modRecord_IsRecordEnable(RECORD_TYPE eRecordType);

// 是否可截图
bool modRecord_IsScreenshotEnable();

// 文件个数
int modRecord_GetRecordFileNum(RECORD_TYPE eRecordType, int iId = STORAGE_DEFAULT);

// USB检测是否失败
bool modRecord_DetectFailed();

// 获取可录音剩余时间
yl::string modRecord_GetAvailableRecordingTime(int iId = STORAGE_DEFAULT);

// 将文件大小转化为字符串
yl::string modRecord_TransSizeToString(const unsigned long long & ullSize);

// 将时长转化为字符串
yl::string modRecord_TransTimeToString(const unsigned long & lTime);

bool modRecord_IsLoadingFile(RECORD_TYPE eRecordType, int iId = STORAGE_DEFAULT);

#endif // __MOD_RECORD_H__
