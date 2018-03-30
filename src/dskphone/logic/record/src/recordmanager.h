///////////////////////////////////////////////////////////
//  recordanager
//  Implementation of the Class RecordManager
//  Created on:      05-二月-2015 10:55:09
//  Original author: yuh
///////////////////////////////////////////////////////////

#ifndef __RECORD_MANAGER_H__
#define __RECORD_MANAGER_H__

#ifdef IF_USB_SUPPORT
#include <yllist.h>
#include "baserecord.h"
#include "../include/common.h"
#include "hotplug/commondefine.h"


class CRecordManager
{
public:
    static CRecordManager & GetInstance()
    {
        static CRecordManager sInc;
        return sInc;
    }

public:
    // 操作处理
    bool DoOperation(OPERATION_TYPE eOprType, RECORD_TYPE eRecordType,
                     const char * pFile = NULL, void * pExtData = NULL, int iId = STORAGE_DEFAULT);

    // 获取资源信息
    bool GetRecordFileList(listRecordFileInfo& lstFileInfo,
                           RECORD_TYPE eRecordType, int iStartPos, int iNum, int iId);
    bool GetRecordDetailInfo(RecordFileInfo & fileInfo, const char * pFile, RECORD_TYPE eRecordType, int iId);
    bool GetRecordInfoByIndex(RecordFileInfo & fileInfo, int iIndex, RECORD_TYPE eRecordType, int iId);
    int GetRecordInfoByFile(RecordFileInfo & fileInfo, const char * pFile, RECORD_TYPE eRecordType, int iId);

    // 获取存储设备的信息
    bool GetRecordVolumeInfo(StorageInfo& sStorageInfo, int iId);

    // 获取可录音剩余时间
    yl::string GetAvailableRecordingTime(int iId);

    // 将文件大小转化为字符串
    yl::string TransSizeToString(const unsigned long long & ullSize);

    // 将时长转化为字符串
    yl::string TransTimeToString(const unsigned long & lTime);

    bool IsLoadingFile(RECORD_TYPE eRecordType, int iId);

public:
    // 设备容量是否满
    bool IsStorageFull(RECORD_TYPE eRecordType, int iId);

    // 是否处于某种录制状态
    bool IsInRecordState(int iRecordState, RECORD_TYPE eRecordType);

    // 是否处于某种录制场景
    bool IsInRecordScene(RECORD_SCENE eRecordScene, RECORD_TYPE eRecordType);
    // 设置录制场景
    void SetRecordScene(RECORD_SCENE eRecordScene, RECORD_TYPE eRecordType);
    // 获取录制时长
    void GetRecordTime(RECORD_TYPE eRecordType, unsigned int & uHour,
                       unsigned int & uMinute, unsigned int & uSecond);

    // USB是否识别失败
    bool IsDetectFailed()
    {
        return m_bDetectFailed;
    };


public:
    // 处理设备消息
    bool ProcessDeviceMsg(msgObject & msg);

    // 发送消息给对象
    bool SendMessageToRecordObject(msgObject & msg, RECORD_TYPE eRecordType = RECORD_NONE);

    // 定时器处理
    bool OnTimer(unsigned int iTimerID);

    // 增加文件对象
    void AddRecordObject(CBaseRecord * pRecordObject);

    // 获取当前设备状态
    inline USB_STORAGE_STATE GetStorageState()
    {
        return m_eStorageState;
    }

    // 文件个数
    int GetRecordFileNum(RECORD_TYPE eRecordType, int iId);

    void SetDetectTimer(HotPlugUsbDeviceType eType);
    void CancelDetectTimer();

private:
    CRecordManager();
    ~CRecordManager();

    // 初始化
    void Init();

    // 通过类型找到对象
    CBaseRecord * GetRecordObject(RECORD_TYPE eRecordType);

    // 设置设备状态
    void ChangeStorageState(USB_STORAGE_STATE eStorangeState,
                            HotPlugUsbDeviceType eDeviceType,
                            HotPlugUsbDeviceState eUsbState);

private:
    // 设备状态
    USB_STORAGE_STATE m_eStorageState;

    // 检测设备超时定时器
    HotPlugUsbDeviceType m_eCurrentDevice;

    // 对象管理
    YLList<CBaseRecord *> m_lstRecordObject;

    // USB是否识别失败
    bool m_bDetectFailed;
};

#define _RecordManager CRecordManager::GetInstance()

#endif // IF_USB_RECORD
#endif // __RECORD_MANAGER_H__
