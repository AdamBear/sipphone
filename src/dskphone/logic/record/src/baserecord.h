#ifndef __BASE_RECORD_H__
#define __BASE_RECORD_H__

#ifdef IF_USB_SUPPORT
#include <yllist.h>
#include <ETLLib.hpp>
#include "../include/common.h"
#include "hotplug/commondefine.h"
#include "timepiece.h"

// 录制状态
enum RECORD_STATE
{
    STATE_IDLE,                 // 空闲
    STATE_RECORDING,            // 正在录制
    STATE_RECORD_PAUSE,         // 暂停录制
    STATE_RECORD_AUTO_PAUSE,    // 自动暂停录制
    STATE_PLAY_IDLE,            // 播放空闲
    STATE_PLAYING,              // 正在播放
    STATE_PLAY_PAUSE,           // 暂停播放
};

struct StorageFileInfo
{
    listRecordFileInfo  listFileInfo;       // 已加载文件列表
    YLList<yl::string>  listNewFile;        // 新增文件
};
// storage id —— file info
typedef YLHashMap<int, StorageFileInfo> mapId2FileInfo;

#define KB(x) (x * 1024)
#define MB(x) (KB(x) * 1024)
#define GB(x) (MB(x) * 1024)

class CBaseRecord
{
public:
    CBaseRecord();
    virtual ~CBaseRecord();

    // 操作执行
    virtual bool DoOperation(OPERATION_TYPE eOprType, const char* pFile = NULL,
                             void* pExtData = NULL, int iId  = STORAGE_DEFAULT);
    // 消息处理
    virtual bool ProcessMsg(msgObject& msg);
    // 空间转换成时长
    virtual long GetAvailableTime(unsigned long long dSize) { return 0; }
    // 用来判断空间是否足够
    virtual bool IsAvaliable(unsigned long long dSize);
    // 获取文件信息
    bool GetFileList(listRecordFileInfo& listFile, int iStartPos, int iNum, int iId = STORAGE_DEFAULT);
    bool GetFileInfo(RecordFileInfo& fileInfo, int iFileIndex, int iId = STORAGE_DEFAULT);
    virtual RecordFileInfo* GetFileInfo(const char* pFile, int iId = STORAGE_DEFAULT);
    int GetFileInfo(RecordFileInfo& fileInfo, const char* pFile, int iId = STORAGE_DEFAULT);
    // 获取录制类型
    inline RECORD_TYPE GetRecordType() { return m_eType; }
    // 获取录制状态
    inline RECORD_STATE GetRecordState() { return m_eRecordState; }
    // 获取录音播放状态
    inline RECORD_STATE GetRecordPlayState() { return m_eRecordPlayState; }
    // 设置录制场景
    inline void SetRecordScene(RECORD_SCENE eScene) { m_eScene = eScene; }
    // 获取录制场景
    inline RECORD_SCENE  GetRecordScene() { return m_eScene; }
    // 获取录制时间
    inline void GetRecordTime(unsigned int& uHour,
                              unsigned int& uMinute, unsigned int& uSecond)
    {
        m_timepiece.GetTime(uHour, uMinute, uSecond);
    }
    // 获取record文件数量
    int GetRecordFileNum(int iId);
    // 加载文件请求
    bool RequestLoadFileList(int iId);
    // 获取文件搜索路径
    bool GetSearchDirs(int iId, YLList<yl::string>& listSearchDir);
    // 新增文件
    bool AddNewFile(const yl::string& strFile);
    // 获取文件加载状态
    RECORD_FILE_LOAD_STATE GetFileLoadState(int iId) { return m_eFileLoadState; };

protected:
    // 生成全路径
    yl::string GenerateFullPath(int iId, const yl::string& strFolder, 
                                const yl::string& strExtName,
                                const char* pFileName = NULL);
    // 创建文件目录
    void CreateFolder(const yl::string& strName);
    // 删除文件
    bool Delete(int iId, const char* pFile, bool bDelAll = false);

private:
    // 获取存储设备
    bool GetStorageFileInfoItem(int iId, mapId2FileInfo::iterator& iter);    
    // 文件夹名
    virtual yl::string GetFolder() { return ""; }
    // 额外的文件夹名
    virtual yl::string GetFolderEx() { return ""; }
    // 后缀名
    virtual yl::string GetSuffix() { return ""; }
    // 支持的格式
    virtual yl::string GetSupportFormat() { return ""; }

protected:
    RECORD_STATE            m_eRecordState;     // 录制状态
    RECORD_STATE            m_eRecordPlayState; // 音频播放状态
    RECORD_FILE_LOAD_STATE  m_eFileLoadState;   // 文件加载状态
    RECORD_TYPE             m_eType;            // 录制类型
    RECORD_SCENE            m_eScene;           // 录制场景
    TimePiece               m_timepiece;        // 录音计时器
    yl::string              m_strRecordFile;    // 当前正在录制的文件
    int                     m_iActiveId;        // 当前活跃存储设备
    mapId2FileInfo          m_mapId2FileInfo;   // 存储设备&关联文件
};

#endif // IF_USB_RECORD
#endif // __BASE_RECORD_H__
