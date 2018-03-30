#ifdef IF_USB_SUPPORT
#include "etlmsghandler/modetlmsghandler.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "dsklog/log.h"
#include "storagemanager.h"
#include "record/include/modrecord.h"
#include "storagehelpthread.h"
#include "taskaction/modtaskaction.h"
#include "commonunits/commonunits_def.h"

#define SAFE_DELETE(x) if(NULL != x)\
                       {\
                           COMMONUNITS_INFO("Safe delete point[%p]", x);\
                           delete x;\
                           x = NULL;\
                       }\

static LRESULT OnMessage(msgObject & objMsg)
{
    bool bHandle = true;
    switch (objMsg.message)
    {
    case HOTPLUG_STORAGE_TASK_RESULT:
        _StorageManager.ProcessTaskResult(objMsg);
        break;
    case TM_TIMER:
        _StorageManager.ProcessTimer(objMsg.wParam);
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

IMPLEMENT_GETINSTANCE(CStorageManager)
CStorageManager::CStorageManager()
    : m_iCheckVolumeTimer(0)
{
}

CStorageManager::~CStorageManager()
{
}

// 初始化
bool CStorageManager::Init()
{
    // usb口禁用则不初始化
    if (configParser_GetConfigInt(kszUsbPortEnable) == 0)
    {
        COMMONUNITS_WARN("Usb prot disabled, init fail.");
        return false;
    }

    ResetId();
    m_listStorageDevInfo.clear();
    m_listVolumeInfo.clear();
    m_mapStorageId2ReqFile.clear();

    TaskAction_CreateSpecialThread(NS_TA::TA_REQUEST_STORAGE_HELP,
                                   TA_THREAD_STORAGE_HELP,
                                   CStorageHelpThread::OnMessage);

    SingleMsgReg(TM_TIMER, OnMessage);
    SingleMsgReg(HOTPLUG_STORAGE_TASK_RESULT, OnMessage);
    return true;
}

// 获取当前活跃的存储设备
int CStorageManager::GetActiveStorageIndex()
{
    for (listStorageDevInfo::iterator iter = m_listStorageDevInfo.begin();
            iter != m_listStorageDevInfo.end(); ++ iter)
    {
        StorageDevInfo& sStorageDevInfo = *iter;
        if (sStorageDevInfo.iIndex != STORAGE_INVALID
                && sStorageDevInfo.uStatus == USB_STATUS_ACTIVE)
        {
            return sStorageDevInfo.iIndex;
        }
    }

    return STORAGE_INVALID;
}

// 获取存储设备数量
int CStorageManager::GetStorageDevNum()
{
    return m_listStorageDevInfo.size();
}

// 获取存储设备容量信息
bool CStorageManager::GetStorageVolume(int iId, StorageVolumeInfo& sVolumeInfo)
{
    if (iId == STORAGE_INVALID)
    {
        return false;
    }

    for (listVolumeInfo::iterator iter = m_listVolumeInfo.begin();
            iter != m_listVolumeInfo.end();
            ++ iter)
    {
        StorageVolumeInfo& sVolumeTemp = *iter;
        if (sVolumeTemp.iId == iId)
        {
            sVolumeInfo = sVolumeTemp;
            return true;
        }
    }

    return false;
}

// 获取存储设备已加载的文件列表
bool CStorageManager::GetStorageLoadFileList(int iId, int iReqType,
        listRecordFileInfo& listRecordFile)
{
    mapStorageId2ReqFile::iterator iter = m_mapStorageId2ReqFile.find(iId);
    if (iter == m_mapStorageId2ReqFile.end())
    {
        return false;
    }

    listStorageReqFile& listReqFile = iter->second;
    for (listStorageReqFile::iterator iterList = listReqFile.begin();
            iterList != listReqFile.end(); ++ iterList)
    {
        StorageReqFileInfo& sReqFile = *iterList;
        if (sReqFile.iReqType == iReqType)
        {
            // 文件列表信息被获取则删除缓存
            listRecordFile = sReqFile.listRecordFile;
            iterList = listReqFile.erase(iterList);

            COMMONUNITS_INFO("Got storage file, delete cache. id[%d], req type[%d]", iId, iReqType);
            return true;
        }
    }

    return false;
}

// usb存储设备挂载/卸载事件
bool CStorageManager::LoadUsbStorageMountEvent(hpd_act& sHdpAction)
{
    if (HPD_ACT_READY == sHdpAction.action)
    {
        ProcessUsbMountEvent(sHdpAction);
    }
    else if (HPD_ACT_DELETE == sHdpAction.action)
    {
        ProcessUsbUmountEvent(sHdpAction);
    }

    return true;
}

// 处理usb mount
bool CStorageManager::ProcessUsbMountEvent(hpd_act& sHdpAction)
{
    PushTask2Thread(STORAGE_TASK_USB_MOUNT_REQ,
                    0, sizeof(sHdpAction), &sHdpAction);
    return true;
}

// 处理usb umount
bool CStorageManager::ProcessUsbUmountEvent(hpd_act& sHdpAction)
{
    StorageDevInfo sDevInfo;
    GetStorageInfoByDevId(sHdpAction.devid, sDevInfo);
    SetUsbStatus(sDevInfo.iIndex, USB_STATUS_WILL_UNMOUNT);

    PushTask2Thread(STORAGE_TASK_USB_UMOUNT_REQ,
                    0, sizeof(sHdpAction), &sHdpAction);
    return true;
}

// 文件列表信息请求
bool CStorageManager::RequestLoadFileList(int iReqType, YLList<yl::string>& listDir,
        yl::string& strExtName,
        YLList<yl::string>& listFile, int iId)
{
    // 申请内存让异步线程操作, 异步线程返回主线程后需delete
    StorageReqFileInfo* pReqFileInfo = new StorageReqFileInfo;
    if (!pReqFileInfo)
    {
        return false;
    }

    COMMONUNITS_INFO("New [StorageReqFileInfo] memery point[%p]", pReqFileInfo);
    pReqFileInfo->iReqType = iReqType;
    pReqFileInfo->listDir = listDir;
    pReqFileInfo->strExtName = strExtName;
    pReqFileInfo->listNewFile = listFile;

    // 记录即将请求的信息
    listStorageReqFile listReqFile;
    mapStorageId2ReqFile::iterator iter = m_mapStorageId2ReqFile.find(iId);
    if (iter == m_mapStorageId2ReqFile.end())
    {
        listReqFile.push_back(*pReqFileInfo);
    }
    else
    {
        listReqFile = iter->second;
        listReqFile.push_back(*pReqFileInfo);
    }
    m_mapStorageId2ReqFile.insert(iId, listReqFile);

    PushTask2Thread(STORAGE_TASK_USB_FILE_LOAD_REQ,
                    iId, sizeof(pReqFileInfo), &pReqFileInfo);
    return true;
}

// 处理线程任务返回结果
// 与其他线程的数据交互方式为指针, 每个任务结果返回都需要SAFE_DELETE(p)！！！
bool CStorageManager::ProcessTaskResult(msgObject & objMessage)
{
    COMMONUNITS_INFO("Receive storage msg. task result type[%d]", objMessage.wParam);

    bool bHandle = true;
    switch (objMessage.wParam)
    {
    case STORAGE_TASK_USB_VOLUME_GET_REQ:
        OnVolumeCheckResult(objMessage.lParam, objMessage.GetExtraData());
        break;
    case STORAGE_TASK_USB_MOUNT_REQ:
        OnUsbMountEventResult(objMessage.lParam, objMessage.GetExtraData());
        break;
    case STORAGE_TASK_USB_UMOUNT_REQ:
        OnUsbUnmountEventResult(objMessage.lParam);
        break;
    case STORAGE_TASK_USB_FILE_LOAD_REQ:
        OnLoadFileListResult(objMessage.lParam, objMessage.GetExtraData());
        break;
    default:
        COMMONUNITS_INFO("The task type[%d] not defined", objMessage.wParam);
        bHandle = false;
        break;
    }

    return bHandle;
}

// 处理定时器
bool CStorageManager::ProcessTimer(int iTimerId)
{
    if (iTimerId != (UINT)&m_iCheckVolumeTimer)
    {
        return false;
    }

    timerKillThreadTimer((UINT)&m_iCheckVolumeTimer);

    // 存在会产生空间大小变化的事件, 则继续查询
    if (!modRecord_IsRecordIdle(RECORD_AUDIO)
#ifdef IF_SUPPORT_VIDEO
            || !modRecord_IsIdle(RECORD_VIDEO)
#endif
       )
    {
        StartVolumeCheck(true, true);
    }

    return true;
}

// 获取存储设备信息(通过底层标识)
bool CStorageManager::GetStorageInfoByDevId(int iDevId, StorageDevInfo& sStorageDevInfo)
{
    for (listStorageDevInfo::iterator iter = m_listStorageDevInfo.begin();
            iter != m_listStorageDevInfo.end();
            ++ iter)
    {
        StorageDevInfo& sTempInfo = *iter;
        if (iDevId == sTempInfo.iDevId)
        {
            sStorageDevInfo = sTempInfo;
            return true;
        }
    }

    return false;
}

// 获取存储设备信息(通过上层标识)
bool CStorageManager::GetStorageInfoByIndex(int iIndex, StorageDevInfo& sStorageDevInfo)
{
    for (listStorageDevInfo::iterator iter = m_listStorageDevInfo.begin();
            iter != m_listStorageDevInfo.end(); ++ iter)
    {
        StorageDevInfo& sTempInfo = *iter;
        if (iIndex == sTempInfo.iIndex)
        {
            sStorageDevInfo = sTempInfo;
            return true;
        }
    }

    return false;
}

// 获取存储设备信息(通过上层标识)
int CStorageManager::GetStorageIndexByPriority(UINT uPriority)
{
    for (listStorageDevInfo::iterator iter = m_listStorageDevInfo.begin();
            iter != m_listStorageDevInfo.end(); ++ iter)
    {
        StorageDevInfo& sTempInfo = *iter;
        if (uPriority == sTempInfo.uPriority)
        {
            return sTempInfo.iIndex;
        }
    }

    return STORAGE_INVALID;
}

// 获取存储设备路径
yl::string CStorageManager::GetStoragePathByIndex(int iId)
{
    StorageDevInfo sStorageDevInfo;
    if (!GetStorageInfoByIndex(iId, sStorageDevInfo))
    {
        return "";
    }

    return sStorageDevInfo.strMountPoint;
}

// 开始检测容量信息
void CStorageManager::StartVolumeCheck(bool bEnable, bool bCheckNow /*= false*/)
{
    _StorageHelp.SetVolumeCheckEnable(bEnable);

    if (bEnable)
    {
        if (bCheckNow)
        {
            listVolumeInfo* pVolumeInfo = new listVolumeInfo;
            if (!pVolumeInfo)
            {
                return;
            }

            COMMONUNITS_INFO("New [listVolumeInfo] memery point[%p]", pVolumeInfo);
            *pVolumeInfo = m_listVolumeInfo;
            PushTask2Thread(STORAGE_TASK_USB_VOLUME_GET_REQ,
                            STORAGE_INVALID, sizeof(pVolumeInfo), &pVolumeInfo);
        }

        // 设置定时器定时查询
        m_iCheckVolumeTimer = 5000;
        timerSetThreadTimer((UINT)&m_iCheckVolumeTimer, m_iCheckVolumeTimer);
    }
    else
    {
        timerKillThreadTimer((UINT)&m_iCheckVolumeTimer);
        COMMONUNITS_INFO("Disable volume check timer[%d].", (UINT)&m_iCheckVolumeTimer);
    }
}

// 改变当前活跃的存储设备
void CStorageManager::ChangeActiveStorage(int iId, bool bActive)
{
    if (bActive)
    {
        for (listStorageDevInfo::iterator iter = m_listStorageDevInfo.begin();
                iter != m_listStorageDevInfo.end(); ++ iter)
        {
            StorageDevInfo& sTempInfo = *iter;
            // 设置为活跃, 其他置为可用状态
            if (iId == sTempInfo.iIndex)
            {
                COMMONUNITS_INFO("Change storage [%d] to active", iId);
                sTempInfo.uStatus = USB_STATUS_ACTIVE;
            }
            else
            {
                sTempInfo.uStatus = USB_STATUS_USABLE;
            }
        }
    }
    else
    {
        // 当前是活跃设备才重置
        StorageDevInfo sStorageInfo;
        if (GetStorageInfoByIndex(iId, sStorageInfo)
                && sStorageInfo.uStatus == USB_STATUS_ACTIVE)
        {
            ChangeActiveStorage(GetStorageIndexByPriority(GetMaxPriority(true)), true);
        }
    }
}

// 获取usb状态
UINT CStorageManager::GetUsbStatus(int iId)
{
    StorageDevInfo sStorageInfo;
    if (!GetStorageInfoByIndex(iId, sStorageInfo))
    {
        return USB_STATUS_UNUSABLE;
    }

    return sStorageInfo.uStatus;
}

// 容量信息检测结果处理
void CStorageManager::OnVolumeCheckResult(int iId, LPCVOID pData)
{
    // 更新指定存储设备的容量信息
    if (STORAGE_INVALID != iId)
    {
        StorageVolumeInfo* pVolumeInfo = *(StorageVolumeInfo * *)pData;
        if (!pVolumeInfo)
        {
            return;
        }

        listVolumeInfo::iterator iter;
        if (GetVolumeInfoIter(iId, iter))
        {
            *iter = *pVolumeInfo;
        }
        else
        {
            m_listVolumeInfo.push_back(*pVolumeInfo);
        }

        COMMONUNITS_INFO("Update storage[%d] volume info, point[%p]",
                         pVolumeInfo->iId, pVolumeInfo);
        SAFE_DELETE(pVolumeInfo);
        return;
    }

    // 更新整个存储设备列表的容量信息
    listVolumeInfo* pListVolumeInfo = *(listVolumeInfo * *)pData;
    if (!pListVolumeInfo)
    {
        return;
    }

    // 不可整个结构覆盖, 避免本地缓存不在上一次更新的范围内
    for (listVolumeInfo::iterator iterCheck = pListVolumeInfo->begin();
            iterCheck != pListVolumeInfo->end(); ++ iterCheck)
    {
        StorageVolumeInfo& sVolumeCheck = *iterCheck;
        for (listVolumeInfo::iterator iterCache = m_listVolumeInfo.begin();
                iterCache != m_listVolumeInfo.end(); ++ iterCache)
        {
            StorageVolumeInfo& sVolumeCache = *iterCache;
            if (sVolumeCache.iId != sVolumeCheck.iId
                    || sVolumeCache.strStorageDir != sVolumeCheck.strStorageDir)
            {
                continue;
            }

            sVolumeCache = sVolumeCheck;
            COMMONUNITS_INFO("Update storage[%d] volume info", sVolumeCache.iId);
            etl_NotifyApp(false, HOTPLUG_USB_STORAGE_VOLUME_CHANGE, sVolumeCache.iId, 0);
            break;
        }
    }

    SAFE_DELETE(pListVolumeInfo);
}

// 文件列表信息加载结果处理
void CStorageManager::OnLoadFileListResult(int iId, LPCVOID pData)
{
    StorageReqFileInfo* pReqFileInfo = *((StorageReqFileInfo * *)pData);
    if (!pReqFileInfo || STORAGE_INVALID == iId)
    {
        etl_NotifyApp(false, HOTPLUG_STORAGE_FILE_LOAD_RESULE, -1, 0);
        SAFE_DELETE(pReqFileInfo);
        return;
    }

    mapStorageId2ReqFile::iterator iterMap = m_mapStorageId2ReqFile.find(iId);
    if (iterMap == m_mapStorageId2ReqFile.end())
    {
        etl_NotifyApp(false, HOTPLUG_STORAGE_FILE_LOAD_RESULE, -1, 0);
        SAFE_DELETE(pReqFileInfo);
        return;
    }

    listStorageReqFile& listReqFile = iterMap->second;
    for (listStorageReqFile::iterator iterList = listReqFile.begin();
            iterList != listReqFile.end(); ++ iterList)
    {
        StorageReqFileInfo& sReqFile = *iterList;
        if (pReqFileInfo->iReqType == sReqFile.iReqType
                && pReqFileInfo->strExtName == sReqFile.strExtName)
        {
            COMMONUNITS_INFO("Fill requested file, req type[%d], num[%d]",
                             sReqFile.iReqType, pReqFileInfo->listRecordFile.size());

            sReqFile.listRecordFile = pReqFileInfo->listRecordFile;
            etl_NotifyApp(false, HOTPLUG_STORAGE_FILE_LOAD_RESULE, sReqFile.iReqType, iId);

            break;
        }

    }

    SAFE_DELETE(pReqFileInfo);
}

// usb mount结果处理
bool CStorageManager::OnUsbMountEventResult(int iSucc, LPCVOID pData)
{
    StorageDevInfo* pStorageInfo = *(StorageDevInfo * *)pData;
    if (!pStorageInfo || !iSucc)
    {
        SAFE_DELETE(pStorageInfo);
        return false;
    }

    // v84需求: U盘禁用, 测试模式依然可检测
    if (configParser_GetConfigInt(kszUSBDiskFunctionEnable) == 0)
    {
        etl_NotifyAppEx(false, HOTPLUG_USB_STORAGE_STATE_CHANGE_UNINIT,
                        1, 0, pStorageInfo->strMountPoint.length() + 1,
                        pStorageInfo->strMountPoint.c_str());
    }
    else
    {
        //pStorageInfo->iIndex = GenerateId();
        pStorageInfo->uStatus = USB_STATUS_ACTIVE;
        pStorageInfo->uPriority = GetMaxPriority() + 1;
        m_listStorageDevInfo.push_back(*pStorageInfo);

        // 改变当前活跃设备 PS: 后识别者生效
        ChangeActiveStorage(pStorageInfo->iIndex, true);

        etl_NotifyAppEx(false, HOTPLUG_USB_STORAGE_STATE_CHANGING,
                        1, pStorageInfo->iIndex,
                        pStorageInfo->strMountPoint.length() + 1,
                        pStorageInfo->strMountPoint.c_str());

        AddVolumeCheck(pStorageInfo->iIndex, pStorageInfo->strMountPoint);
    }

    SAFE_DELETE(pStorageInfo);
    return true;
}

// usb umount结果处理
bool CStorageManager::OnUsbUnmountEventResult(int iDevId)
{
    if (configParser_GetConfigInt(kszUSBDiskFunctionEnable) == 0)
    {
        etl_NotifyApp(false, HOTPLUG_USB_STORAGE_STATE_CHANGE_UNINIT, 0, 0);
    }
    else
    {
        StorageDevInfo sDevInfo;
        if (!GetStorageInfoByDevId(iDevId, sDevInfo))
        {
            return false;
        }

        // 取消活跃设备状态
        ChangeActiveStorage(sDevInfo.iIndex, false);

        // 存储设备移除, 删除缓存
        DeleteVolumeCheck(sDevInfo.iIndex);
        DeleteStorageDevInfo(sDevInfo.iIndex);
        DeleteRequestedFileList(sDevInfo.iIndex);

        etl_NotifyAppEx(false, HOTPLUG_USB_STORAGE_STATE_CHANGING, 0, 0,
                        sDevInfo.strMountPoint.length() + 1,
                        sDevInfo.strMountPoint.c_str());
    }

    return true;
}

// 添加存储设备容量信息检测
void CStorageManager::AddVolumeCheck(int iId, const yl::string& strDir)
{
    if (STORAGE_INVALID == iId || strDir.empty())
    {
        return;
    }

    COMMONUNITS_INFO("Add volume check, id[%d] dir[%s]", iId, strDir.c_str());

    // 没有请求缓存, 则装入请求信息
    StorageVolumeInfo sVolumeInfo;
    if (!GetStorageVolume(iId, sVolumeInfo))
    {
        sVolumeInfo.iId = iId;
        sVolumeInfo.strStorageDir = strDir;
        m_listVolumeInfo.push_back(sVolumeInfo);
    }

    StartVolumeCheck(true, true);
}

// 删除存储设备容量信息检测
void CStorageManager::DeleteVolumeCheck(int iId)
{
    for (listVolumeInfo::iterator iter = m_listVolumeInfo.begin();
            iter != m_listVolumeInfo.end();)
    {
        StorageVolumeInfo& sVolumeInfo = *iter;
        if (iId == sVolumeInfo.iId)
        {
            iter = m_listVolumeInfo.erase(iter);
            COMMONUNITS_INFO("Delete storage[%d] volume info", iId);
        }
        else
        {
            ++ iter;
        }
    }

    if (m_listVolumeInfo.size() <= 0)
    {
        StartVolumeCheck(false);
    }
}

// 删除存储设备信息
void CStorageManager::DeleteStorageDevInfo(int iId)
{
    for (listStorageDevInfo::iterator iter = m_listStorageDevInfo.begin();
            iter != m_listStorageDevInfo.end(); ++ iter)
    {
        StorageDevInfo& sTemoInfo = *iter;
        if (iId == sTemoInfo.iIndex)
        {
            iter = m_listStorageDevInfo.erase(iter);
            COMMONUNITS_INFO("Delete storage[%d] device info", iId);
            break;
        }
    }
}

// 删除存储设备文件列表信息请求
void CStorageManager::DeleteRequestedFileList(int iId)
{
    mapStorageId2ReqFile::iterator iter = m_mapStorageId2ReqFile.find(iId);
    if (iter != m_mapStorageId2ReqFile.end())
    {
        iter == m_mapStorageId2ReqFile.erase(iter);
    }
}

// 获取容量列表的元素
bool CStorageManager::GetVolumeInfoIter(int iId, listVolumeInfo::iterator& iter)
{
    for (listVolumeInfo::iterator iterList = m_listVolumeInfo.begin();
            iterList != m_listVolumeInfo.end(); ++ iterList)
    {
        StorageVolumeInfo& sVolumeInfo = *iterList;
        if (sVolumeInfo.iId == iId)
        {
            iter = iterList;
            return true;
        }
    }

    return false;
}

// 推送任务到异步线程处理
void CStorageManager::PushTask2Thread(UINT uTaskType, UINT uId,
                                      UINT uTaskDataSize, LPCVOID pTaskData)
{
    msgPostMsgToThreadEx(msgGetThreadByName(TA_THREAD_STORAGE_HELP),
                         HOTPLUG_STORAGE_TASK, uTaskType, uId, uTaskDataSize, pTaskData);
}

// 获取最大优先级
int CStorageManager::GetMaxPriority(bool bExceptActive /*= false*/)
{
    if (!bExceptActive
            && m_listStorageDevInfo.size() > 0)
    {
        return m_listStorageDevInfo.back().uPriority;
    }

    UINT uMaxPriority = 0;
    for (listStorageDevInfo::iterator iter = m_listStorageDevInfo.begin();
            iter != m_listStorageDevInfo.end(); ++ iter)
    {
        StorageDevInfo& sTemoInfo = *iter;
        if (bExceptActive
                && sTemoInfo.uStatus == USB_STATUS_ACTIVE)
        {
            continue;
        }

        uMaxPriority = sTemoInfo.uPriority > uMaxPriority ?
                       sTemoInfo.uPriority : uMaxPriority;
    }

    return uMaxPriority;
}

// 设置usb状态
void CStorageManager::SetUsbStatus(int iId, UINT uStatus)
{
    for (listStorageDevInfo::iterator iter = m_listStorageDevInfo.begin();
            iter != m_listStorageDevInfo.end(); ++iter)
    {
        StorageDevInfo& sTemoInfo = *iter;
        if (sTemoInfo.iIndex == iId)
        {
            sTemoInfo.uStatus = uStatus;
            break;
        }
    }
}

int CStorageManager::GenerateId()
{
    /**
     * 规则:当只有一个设备的时候, id设置为STORAGE_DEFAULT
     *      多个设备的时候, 则往上叠加, id在设备插入期间不改变
     */
    if (m_listStorageDevInfo.size() <= 0)
    {
        m_iId = STORAGE_DEFAULT;
    }
    else
    {
        ++ m_iId;
    }

    COMMONUNITS_INFO("Generate Storage id[%d]", m_iId);

    return m_iId;
}

#endif // IF_USB_SUPPORT
