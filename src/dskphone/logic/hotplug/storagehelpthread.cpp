#ifdef IF_USB_SUPPORT
#include <unistd.h>
#include <dirent.h>
#include <ETLLib.hpp>
#include <sys/statfs.h>
#include <sys/mount.h>
#include <linux/fs.h>
#include "dsklog/log.h"
#include "commondefine.h"
#include <hpdev/hpdev_api.h>
#include "extended/usbmessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "commonapi/systemhelperapi.h"
#include "storagehelpthread.h"
#include "storagemanager.h"
#include "record/include/recordinterface.h"
#include <commonapi/systemhelperapi.h>


#define DEV_NODE                "/dev/myusb%d"
#define DEV_MOUNT_DIR           "/tmp/udisk%d"

IMPLEMENT_GETINSTANCE(CStorageHelpThread)
CStorageHelpThread::CStorageHelpThread()
    : m_bCheckVolumeEnable(false)
{
}

CStorageHelpThread::~CStorageHelpThread()
{
}

bool CStorageHelpThread::OnMessage(msgObject& objMsg)
{
    if (objMsg.message != HOTPLUG_STORAGE_TASK)
    {
        return false;
    }

    COMMONUNITS_INFO("Receive storage msg. task type[%d]", objMsg.wParam);
    if (objMsg.GetExtraSize() <= 0
            || objMsg.GetExtraData() == NULL)
    {
        COMMONUNITS_INFO("extra data is null or size is 0");
        return false;
    }

    switch (objMsg.wParam)
    {
    case STORAGE_TASK_USB_VOLUME_GET_REQ:
        _StorageHelp.GetVolumeInfoList(objMsg.GetExtraData());
        break;
    case STORAGE_TASK_USB_FILE_LOAD_REQ:
        _StorageHelp.LoadFileInfoList(objMsg.lParam, objMsg.GetExtraData());
        break;
    case STORAGE_TASK_USB_MOUNT_REQ:
        _StorageHelp.MountUsbDevice(objMsg.GetExtraData());
        break;
    case STORAGE_TASK_USB_UMOUNT_REQ:
        _StorageHelp.UmountUsbDevice(objMsg.GetExtraData());
        break;
    default:
        break;
    }

    return true;
}

void CStorageHelpThread::GetVolumeInfoList(LPCVOID pData)
{
    listVolumeInfo* pListVolume = *(listVolumeInfo * *)pData;
    if (!pListVolume)
    {
        return;
    }

    for (listVolumeInfo::iterator iter = pListVolume->begin();
            iter != pListVolume->end(); ++ iter)
    {
        if (!m_bCheckVolumeEnable)
        {
            COMMONUNITS_INFO("Volume check switch is disable.");
            return;
        }

        GetVolumeInfo(*iter);
    }

    ReturnTaskResult(STORAGE_TASK_USB_VOLUME_GET_REQ,
                     STORAGE_INVALID, sizeof(pListVolume), &pListVolume);
}

void CStorageHelpThread::GetVolumeInfo(StorageVolumeInfo& sVolumeInfo,
                                       bool bReturnResult /*= false*/)
{
    if (sVolumeInfo.iId == STORAGE_INVALID
            && sVolumeInfo.strStorageDir.empty())
    {
        return;
    }

    GetStorageSize(sVolumeInfo.strStorageDir,
                   sVolumeInfo.ulFreeSize, sVolumeInfo.ulTotalSize);
    sVolumeInfo.ulUsedSize = sVolumeInfo.ulTotalSize - sVolumeInfo.ulFreeSize;
    sVolumeInfo.dUsedPercent = (double)((sVolumeInfo.ulUsedSize) * 100.0 / sVolumeInfo.ulTotalSize);

    COMMONUNITS_DEBUG("id[%d] dir[%s] free[%llu] total[%llu] used present[%f]",
                      sVolumeInfo.iId,
                      sVolumeInfo.strStorageDir.c_str(),
                      sVolumeInfo.ulFreeSize,
                      sVolumeInfo.ulTotalSize,
                      sVolumeInfo.dUsedPercent);

    if (bReturnResult)
    {
        StorageVolumeInfo* pVolunmeInfo = new StorageVolumeInfo;
        if (pVolunmeInfo)
        {
            COMMONUNITS_INFO("New [StorageVolumeInfo] memery point[%p]", pVolunmeInfo);
            *pVolunmeInfo = sVolumeInfo;
            ReturnTaskResult(STORAGE_TASK_USB_VOLUME_GET_REQ,
                             sVolumeInfo.iId, sizeof(pVolunmeInfo), &pVolunmeInfo);
        }
    }
}

void CStorageHelpThread::LoadFileInfoList(int iId, LPCVOID pData)
{
    StorageReqFileInfo* pReqFileInfo = *((StorageReqFileInfo * *)pData);
    if (!pReqFileInfo)
    {
        return;
    }

    COMMONUNITS_INFO("Load file info list begin, id[%d], req type[%d], point[%p].",
                     iId, pReqFileInfo->iReqType, pReqFileInfo);

    YLList<yl::string> listReqFile = pReqFileInfo->listNewFile;
    if (pReqFileInfo->listNewFile.size() <= 0)
    {
        // 扫描路径下的文件
        for (YLList<yl::string>::iterator iter = pReqFileInfo->listDir.begin();
                iter != pReqFileInfo->listDir.end(); ++ iter)
        {
            commonAPI_getFilesInDirectoryByExt(&listReqFile, *iter,
                                               pReqFileInfo->strExtName, true);
        }
    }

    ReadFilesInfo(iId, pReqFileInfo->iReqType, pReqFileInfo->listRecordFile, listReqFile);

    // 按修改时间的顺序排序
    Sort(pReqFileInfo->listRecordFile, RecordFileInfo::CompareByModifyTime);

    COMMONUNITS_INFO("Load file info list finish, size[%d].",
                     pReqFileInfo->listRecordFile.size());

    ReturnTaskResult(STORAGE_TASK_USB_FILE_LOAD_REQ, iId,
                     sizeof(pReqFileInfo), &pReqFileInfo);
}

bool CStorageHelpThread::MountUsbDevice(LPCVOID pData)
{
    hpd_act* pHpdAction = (hpd_act*)pData;
    if (!pHpdAction || HPD_ACT_READY != pHpdAction->action)
    {
        return false;
    }

    bool bResultSuccess = true;

    char strDevNode[PATH_MAX] = "";
    sprintf(strDevNode, DEV_NODE, pHpdAction->minor);
    if (!CreateDevNode(strDevNode, pHpdAction->major, pHpdAction->minor))
    {
        COMMONUNITS_INFO("Create dev node[%s] failed", strDevNode);
        bResultSuccess = false;
    }

    char strMountPoint[PATH_MAX] = "";
    sprintf(strMountPoint, DEV_MOUNT_DIR, pHpdAction->minor);
    mkdir(strMountPoint, S_IRWXU | S_IRWXG | S_IRWXO);
    COMMONUNITS_INFO("Create dev dir[%s]", strMountPoint);

    if (IsMounted(strMountPoint))
    {
        // 卸载
        COMMONUNITS_INFO("Dev dir[%s] created, try to umount firstly", strMountPoint);
        Unmount(strMountPoint);
        // 删除目录
        rmdir(strMountPoint);
    }

    char strData[] = "utf8";
    if (!bResultSuccess
            || (!Mount(strDevNode, strMountPoint, "vfat", 0, strData)
                && !Mount(strDevNode, strMountPoint, "ext3", 0, strData)))
    {
        rmdir(strMountPoint);
        bResultSuccess = false;
    }

    COMMONUNITS_INFO("Mount [%s], form [%s] to [%s].",
                     bResultSuccess ? "succ" : "error", strDevNode, strMountPoint);

    StorageDevInfo* pStorageDevInfo = new StorageDevInfo;
    if (bResultSuccess && pStorageDevInfo)
    {
        COMMONUNITS_INFO("New [StorageDevInfo] memery point[%p]", pStorageDevInfo);
        pStorageDevInfo->iDevId = pHpdAction->devid;
        pStorageDevInfo->iIndex = _StorageManager.GenerateId();
        pStorageDevInfo->uType = USB_TYPE_LOCAL;
        pStorageDevInfo->uStatus = USB_STATUS_USABLE;
        pStorageDevInfo->strMountPoint = strMountPoint;
        pStorageDevInfo->strDevNode = strDevNode;
        //pStorageDevInfo->strDevName =

        // mount成功, 加载容量信息
        StorageVolumeInfo sVolumeInfo;
        sVolumeInfo.iId = pStorageDevInfo->iIndex;
        sVolumeInfo.strStorageDir = strMountPoint;
        GetVolumeInfo(sVolumeInfo, true);
    }

    ReturnTaskResult(STORAGE_TASK_USB_MOUNT_REQ, bResultSuccess,
                     sizeof(pStorageDevInfo), &pStorageDevInfo);

    return true;
}

void CStorageHelpThread::UmountUsbDevice(LPCVOID pData)
{
    hpd_act* pHpdAction = (hpd_act*)pData;
    if (!pHpdAction || HPD_ACT_DELETE != pHpdAction->action)
    {
        return;
    }

    // 删除分区节点
    char strDevNode[PATH_MAX] = "";
    sprintf(strDevNode, DEV_NODE, pHpdAction->minor);
    remove(strDevNode);

    char strMountPoint[PATH_MAX] = "";
    sprintf(strMountPoint, DEV_MOUNT_DIR, pHpdAction->minor);
    if (!IsMounted(strMountPoint))
    {
        COMMONUNITS_INFO("[%s] is not mounted!", strMountPoint);
        return;
    }

    // 卸载
    Unmount(strMountPoint);
    rmdir(strMountPoint);

    COMMONUNITS_INFO("Storage device remove[%s]!", strMountPoint);

    ReturnTaskResult(STORAGE_TASK_USB_UMOUNT_REQ, pHpdAction->devid, 0, NULL);
}

bool CStorageHelpThread::CreateDevNode(const char* pPath, int iMajor, int iMinor)
{
    if (!pPath || strlen(pPath) <= 0)
    {
        return false;
    }

    mode_t mode = 0660 | S_IFBLK;
    dev_t dev = (iMajor << 8) | iMinor;

    if (mknod(pPath, mode, dev) < 0)
    {
        if (errno != EEXIST)
        {
            return false;
        }
    }

    COMMONUNITS_INFO("Create dev node[%s]", pPath);
    return true;
}

bool CStorageHelpThread::IsMounted(const char* pMountPoint)
{
    if (!pMountPoint || strlen(pMountPoint) <= 0)
    {
        return false;
    }

    FILE *fp;
    if (!(fp = fopen("/proc/mounts", "r")))
    {
        return false;
    }

    char strDevice[PATH_MAX];
    char strMountPath[PATH_MAX];
    char strRest[PATH_MAX];
    char strLine[1024];

    while (fgets(strLine, sizeof(strLine), fp))
    {
        strLine[strlen(strLine) - 1] = '\0';
        sscanf(strLine, "%255s %255s %255s\n", strDevice, strMountPath, strRest);
        if (strcmp(strMountPath, pMountPoint) == 0)
        {
            fclose(fp);
            return true;
        }
    }

    fclose(fp);
    return false;
}

bool CStorageHelpThread::Mount(const char *pDev, const char *pMountPoint,
                               const char *pFSType, unsigned long uMountFlag, void *pData)
{
    if (!pDev || !pMountPoint || !pFSType || !pData)
    {
        return false;
    }

    if (mount(pDev, pMountPoint, pFSType, uMountFlag, pData) < 0)
    {
        COMMONUNITS_WARN("Mount err");
        return false;
    }

    return true;
}

bool CStorageHelpThread::Unmount(const char* pMountPoint)
{
    if (!pMountPoint || strlen(pMountPoint) <= 0)
    {
        return false;
    }

#define MNT_DETACH  0x00000002

    if (umount2(pMountPoint, MNT_DETACH) < 0)
    {
        COMMONUNITS_WARN("Umount err");
        return false;
    }

    return true;
}

bool CStorageHelpThread::GetStorageSize(const yl::string& strDirName,
                                        unsigned long long& freeSize,
                                        unsigned long long& totalSize)
{
    if (!strDirName.empty() && pathFileExist(strDirName.c_str()))
    {
        struct statfs diskInfo;
        statfs(strDirName.c_str(), &diskInfo);
        unsigned long long blocksize = diskInfo.f_bsize;
        freeSize = diskInfo.f_bfree * blocksize;
        totalSize = blocksize * diskInfo.f_blocks;
        return true;
    }

    return false;
}

bool CStorageHelpThread::ReadFilesInfo(int iId, int iReqType,
                                       listRecordFileInfo & listRecordFileInfo,
                                       YLList<yl::string>& listFileName)
{
    listRecordFileInfo.clear();
    RecordFileInfo fileInfo;

    for (YLList<yl::string>::iterator itr = listFileName.begin();
            itr != listFileName.end(); ++ itr)
    {
        yl::string::size_type nFound = (*itr).find_last_of('/');
        if (nFound == yl::string::npos)
        {
            continue ;
        }

        int fd = open((*itr).c_str(), O_RDONLY);
        if (fd < 0)
        {
            // http://bugfree.yealink.com/Bug.php?BugID=132591
            // U盘不可用或者即将unmount, 则停止文件读取
            if (_StorageManager.GetUsbStatus(iId) == USB_STATUS_UNUSABLE
                    || _StorageManager.GetUsbStatus(iId) == USB_STATUS_WILL_UNMOUNT)
            {
                COMMONUNITS_WARN("Usb will unmount, break get file info.");
                break;
            }
            else
            {
                continue;
            }
        }

        struct stat statbuf;
        fstat(fd, &statbuf);

        fileInfo.Reset();
        fileInfo.strName = (*itr).substr(nFound + 1);
        fileInfo.strPath = (*itr).substr(0, nFound + 1);
        fileInfo.lSize = statbuf.st_size;
        fileInfo.modifyTime = statbuf.st_mtime;
        close(fd);

        if (UpdateFileInfo(iReqType, fileInfo))
        {
            listRecordFileInfo.push_back(fileInfo);
        }
    }

    return true;
}

bool CStorageHelpThread::UpdateFileInfo(int iReqType, RecordFileInfo& fileInfo)
{
    if (RECORD_AUDIO == iReqType
            || RECORD_VIDEO == iReqType)
    {
        MediaInfo mediaInfo;
        yl::string strFile = fileInfo.strPath + fileInfo.strName;

        if (RecordInterface_GetInfo(mediaInfo, strFile.c_str()))
        {
            fileInfo.lTime = mediaInfo.nSecTotal;
            return true;
        }
        else
        {
            return false;
        }
    }

    // 不需要获取其他信息的类型, 默认为获取成功
    return true;
}

void CStorageHelpThread::ReturnTaskResult(UINT uTaskType, UINT uId,
        UINT uTaskDataSize, LPCVOID pTaskData)
{
    etl_NotifyAppEx(false, HOTPLUG_STORAGE_TASK_RESULT,
                    uTaskType, uId, uTaskDataSize, pTaskData);
}

#endif // IF_USB_SUPPORT
