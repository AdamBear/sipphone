#ifndef __STORAGE_MANAGER_H__
#define __STORAGE_MANAGER_H__

#ifdef IF_USB_SUPPORT
#include "singletonclassdefine.h"
#include "ETLLib.hpp"
#include <hpdev/hpdev_api.h>
#include "hotplug/commondefine.h"

class CStorageManager
{
    DECLARE_SINGLETON_CLASS(CStorageManager)
    friend class CStorageHelpThread;
public:
    // 初始化
    bool Init();
    // 获取当前活跃的存储设备
    int GetActiveStorageIndex();
    // 获取存储设备数量
    int GetStorageDevNum();
    // 获取存储设备容量信息
    bool GetStorageVolume(int iId, StorageVolumeInfo& szVolume);
    // 获取存储设备已加载的文件列表
    bool GetStorageLoadFileList(int iId, int iReqType, listRecordFileInfo& listRecordFile);
    // usb存储设备挂载/卸载事件
    bool LoadUsbStorageMountEvent(hpd_act& sHdpAction);
    // 处理usb mount
    bool ProcessUsbMountEvent(hpd_act& sHdpAction);
    // 处理usb umount
    bool ProcessUsbUmountEvent(hpd_act& sHdpAction);
    // 文件列表信息请求
    bool RequestLoadFileList(int iReqType, YLList<yl::string>& listDir,
                             yl::string& strExtName, YLList<yl::string>& listFile, int iId);
    // 处理线程任务返回结果
    bool ProcessTaskResult(msgObject & objMessage);
    // 处理定时器
    bool ProcessTimer(int iTimerId);
    // 获取存储设备信息
    bool GetStorageInfoByDevId(int iDevId, StorageDevInfo& sStorageDevInfo);
    bool GetStorageInfoByIndex(int iId, StorageDevInfo& sStorageDevInfo);
    // 通过优先级获取标识
    int GetStorageIndexByPriority(UINT uPriority);
    // 获取存储设备路径
    yl::string GetStoragePathByIndex(int iId);
    // 开始检测容量信息
    void StartVolumeCheck(bool bEnable, bool bCheckNow = false);
    // 改变当前活跃的存储设备
    void ChangeActiveStorage(int iId, bool bActive);
    // 获取usb状态
    UINT GetUsbStatus(int iId);

private:
    // 容量信息检测结果处理
    void OnVolumeCheckResult(int iId, LPCVOID pData);
    // 文件列表信息加载结果处理
    void OnLoadFileListResult(int iId, LPCVOID pData);
    // usb mount结果处理
    bool OnUsbMountEventResult(int iSucc, LPCVOID pData);
    // usb umount结果处理
    bool OnUsbUnmountEventResult(int iDevId);
    // 添加存储设备容量信息检测
    void AddVolumeCheck(int iId, const yl::string& strDir);
    // 删除存储设备容量信息检测
    void DeleteVolumeCheck(int iId);
    // 删除存储设备信息
    void DeleteStorageDevInfo(int iId);
    // 删除存储设备文件列表信息请求
    void DeleteRequestedFileList(int iId);
    // 获取容量列表的元素
    bool GetVolumeInfoIter(int iId, listVolumeInfo::iterator& iter);
    // 推送任务到异步线程处理
    void PushTask2Thread(UINT uTaskType, UINT uId,
                         UINT uTaskDataSize, LPCVOID pTaskData);
    // 获取最大优先级
    int GetMaxPriority(bool bExceptActive = false);
    // 设置usb状态
    void SetUsbStatus(int iId, UINT uStatus);
    // 生成存储设备标识id
    int GenerateId();
    // 重置存储设备标识id
    void ResetId()
    {
        m_iId = -1;
    }
private:
    int                         m_iId;                  // 存储设备标识
    listStorageDevInfo          m_listStorageDevInfo;   // 存储设备信息
    listVolumeInfo              m_listVolumeInfo;       // 存储设备容量信息
    mapStorageId2ReqFile        m_mapStorageId2ReqFile; // 存储设备请求的文件列表信息
    int                         m_iCheckVolumeTimer;    // 容量信息检测定时器
};

#define _StorageManager GET_SINGLETON_INSTANCE(CStorageManager)

#endif // __STORAGE_MANAGER_H__
#endif // IF_USB_SUPPORT
