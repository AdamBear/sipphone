#ifndef __STORAGE_HELP_THREAD_H__
#define __STORAGE_HELP_THREAD_H__

#ifdef IF_USB_SUPPORT
#include "singletonclassdefine.h"
#include "ETLLib.hpp"

class CStorageHelpThread
{
    DECLARE_SINGLETON_CLASS(CStorageHelpThread)

public:
    // 获取容量
    void GetVolumeInfoList(LPCVOID pData);
    // 获取容量信息
    void GetVolumeInfo(StorageVolumeInfo& sVolumeInfo, bool bReturnResult = false);
    // 加载文件列表信息
    void LoadFileInfoList(int iId, LPCVOID pData);
    // mount设备
    bool MountUsbDevice(LPCVOID pData);
    // umount设备
    void UmountUsbDevice(LPCVOID pData);
    // 设置容量查询开关
    void SetVolumeCheckEnable(bool bEnable)
    {
        m_bCheckVolumeEnable = bEnable;
    }

    //消息处理函数
    static bool OnMessage(msgObject& objMessage);

private:
    // 创建设备节点
    bool CreateDevNode(const char* pPath, int major, int minor);
    // 是否已经mount
    bool IsMounted(const char* pMountPoint);
    // 执行mount操作
    bool Mount(const char *pDev, const char *pMountPoint,
               const char *pFSType, unsigned long uMountFlag, void *pData);
    // 执行umount操作
    bool Unmount(const char* pMountPoint);
    // 指定存储设备路径，获取存储设备容量信息
    bool GetStorageSize(const yl::string& strDirName,
                        unsigned long long& freeSize, unsigned long long& totalSize);
    // 获取文件详情
    bool ReadFilesInfo(int iId, int iReqType, listRecordFileInfo& listFileInfo,
                       YLList<yl::string>& lstFileName);
    // 更新文件信息
    bool UpdateFileInfo(int iReqType, RecordFileInfo& fileInfo);
    // 返回线程任务结果
    void ReturnTaskResult(UINT uTaskType, UINT uId,
                          UINT uTaskDataSize, LPCVOID pTaskData);

private:
    bool            m_bCheckVolumeEnable;
};

#define _StorageHelp GET_SINGLETON_INSTANCE(CStorageHelpThread)

#endif // __STORAGE_HELP_THREAD_H__
#endif // IF_USB_SUPPORT
