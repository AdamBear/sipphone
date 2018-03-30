#include "record_inc.h"

#ifdef IF_USB_SUPPORT
#include <fcntl.h>
#include <winimp/winimp.h>
#include "hotplug/storagemanager.h"

// 辅助函数：补全路径后的斜杠
static void AddPathSlash(yl::string & strPath)
{
    if (strPath.empty())
    {
        return ;
    }

    if (strPath[strPath.length() - 1] != '/')
    {
        strPath.append("/");
    }
}

// 辅助函数：获取当前时间
static yl::string GetTime()
{
    time_t tTime = netGetLocalTime();
    tm * tmLocal = localtime(&tTime);

    if (NULL != tmLocal)
    {
        char szBuffer[200] = {0};
        sprintf(szBuffer, "%d%02d%02d-%02d%02d", tmLocal->tm_year + 1900, tmLocal->tm_mon + 1,
                tmLocal->tm_mday, tmLocal->tm_hour, tmLocal->tm_min);
        return szBuffer;
    }
    return "";
}

// 辅助函数：过滤特殊字符
static void ReplaceIllgal(char * pName)
{
    if (NULL == pName)
    {
        return ;
    }

    static char szIllgal[] = "\\/:*?\"<>|";
    int j = 0;
    while (szIllgal[j++] != '\0')
    {
        int i = 0;
        while (pName[i] != '\0')
        {
            if (pName[i] == szIllgal[j])
            {
                pName[i] = '\0';
                strcat(pName, &pName[i + 1]);
                continue;
            }

            ++i;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CBaseRecord::CBaseRecord()
    : m_eType(RECORD_NONE),
      m_eRecordState(STATE_IDLE),
      m_eRecordPlayState(STATE_PLAY_IDLE),
      m_eScene(RECORD_SCENE_NONE),
      m_iActiveId(STORAGE_INVALID)
{
    _RecordManager.AddRecordObject(this);
}

CBaseRecord::~CBaseRecord()
{
    m_mapId2FileInfo.clear();
}

// 操作执行
bool CBaseRecord::DoOperation(OPERATION_TYPE eOprType, const char * pFile/* = NULL*/,
                              void * pExtData/* = NULL*/, int iId /*= STORAGE_DEFAULT*/)
{
    bool bSuccess = false;

    switch (eOprType)
    {
    case OPERATION_DELETE:
        {
            bSuccess = Delete(iId, pFile);
        }
        break;
    case OPERATION_DELETE_ALL:
        {
            bSuccess = Delete(iId, NULL, true);
        }
        break;
    default:
        break;
    }

    return bSuccess;
}

bool CBaseRecord::ProcessMsg(msgObject & msg)
{
    bool bHandle = true;
    switch (msg.message)
    {
    case HOTPLUG_USB_STORAGE_STATE_CHANGING:
        {
            if (1 == msg.wParam)
            {
                m_iActiveId = _StorageManager.GetActiveStorageIndex();
                CreateFolder(GetFolder());

                // 装入需要请求信息
                StorageFileInfo sFileInfo;
                m_mapId2FileInfo.insert(m_iActiveId, sFileInfo);
            }
        }
        break;
    case HOTPLUG_STORAGE_FILE_LOAD_RESULE:
        {
            int iId = msg.lParam;
            USBDEVICE_INFO("Storage load file type[%d] id[%d]", msg.wParam, iId);
            if (m_eType != msg.wParam || STORAGE_INVALID == iId)
            {
                return false;
            }

            // 查找是否有发起请求
            mapId2FileInfo::iterator iter = m_mapId2FileInfo.find(iId);
            if (iter == m_mapId2FileInfo.end())
            {
                USBDEVICE_WARN("There is no file list info load request.");
                return false;
            }

            StorageFileInfo & sFileInfo = iter->second;
            if (sFileInfo.listFileInfo.size() == 0)
            {
                _StorageManager.GetStorageLoadFileList(iId, m_eType, sFileInfo.listFileInfo);
            }
            else
            {
                listRecordFileInfo listFileInfo;
                _StorageManager.GetStorageLoadFileList(iId, m_eType, listFileInfo);

                // 同步到缓存中
                for (listRecordFileInfo::iterator iterList = listFileInfo.begin();
                        iterList != listFileInfo.end(); ++ iterList)
                {
                    sFileInfo.listFileInfo.push_front(*iterList);
                }
            }

            // 按修改时间的顺序排序
            Sort(sFileInfo.listFileInfo, RecordFileInfo::CompareByModifyTime);

            m_eFileLoadState = FILE_LOAD_STATE_IDLE;
            etl_NotifyApp(false, RECORD_STORAGE_LOAD_FILE_LIST, m_eType, 1);
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

bool CBaseRecord::IsAvaliable(unsigned long long dSize)
{
    return (0 < GetAvailableTime(dSize));
}

bool CBaseRecord::GetFileList(listRecordFileInfo & listFile,
                              int iStartPos, int iNum, int iId /*= STORAGE_DEFAULT*/)
{
    if (iStartPos > GetRecordFileNum(iId))
    {
        return false;
    }

    mapId2FileInfo::iterator iter;
    if (!GetStorageFileInfoItem(iId, iter))
    {
        return false;
    }

    int iCount = 0;
    int iCurrPos = 0;
    for (listRecordFileInfo::iterator iterList = (iter->second).listFileInfo.begin();
            iterList != (iter->second).listFileInfo.end(); ++ iterList)
    {
        if (iCurrPos ++ < iStartPos)
        {
            continue;
        }

        if (iNum == -1 || iCount ++ < iNum)
        {
            listFile.push_back(*iterList);
        }

        if (iNum != -1 && iCount >= iNum)
        {
            break;
        }
    }

    return listFile.size();
}

bool CBaseRecord::GetFileInfo(RecordFileInfo & fileInfo,
                              int iFileIndex, int iId /*= STORAGE_DEFAULT*/)
{
    if (iFileIndex < 0)
    {
        return false;
    }

    listRecordFileInfo listFileInfo;
    if (!GetFileList(listFileInfo, 0, -1, iId))
    {
        return false;
    }

    int i = 0;
    for (listRecordFileInfo::iterator iter = listFileInfo.begin();
            iter != listFileInfo.end(); ++ iter, ++ i)
    {
        if (i == iFileIndex)
        {
            fileInfo = (*iter);
            return true;
        }
    }

    return false;
}

int CBaseRecord::GetFileInfo(RecordFileInfo & fileInfo,
                             const char * pFile, int iId /*= STORAGE_DEFAULT*/)
{
    if (NULL == pFile || strcmp(pFile, "") == 0)
    {
        return -1;
    }

    listRecordFileInfo listFileInfo;
    if (!GetFileList(listFileInfo, 0, -1, iId))
    {
        return false;
    }

    int i = 0;
    yl::string strFile;
    for (listRecordFileInfo::iterator iter = listFileInfo.begin();
            iter != listFileInfo.end(); ++ iter, ++ i)
    {
        RecordFileInfo & sFileInfo = (*iter);
        strFile = sFileInfo.strPath + sFileInfo.strName;
        if (0 == strcmp(strFile.c_str(), pFile))
        {
            fileInfo = sFileInfo;
            break;
        }
    }

    return i;
}

// 生成全路径
yl::string CBaseRecord::GenerateFullPath(int iId, const yl::string & strFolder,
        const yl::string & strSuffix,
        const char * pFileName/* = NULL*/)
{
    // 名字为:20130526-12:45-username,如果重复,则在后面加"(数字)"
    yl::string strFile = _StorageManager.GetStoragePathByIndex(iId);
    AddPathSlash(strFile);

    // 保存到特定文件夹
    if (!strFolder.empty())
    {
        strFile.append(strFolder);
        strFile.append("/");
    }

    // 加上当前时间
    strFile.append(GetTime());

    // 加上扩展名
    if (NULL != pFileName)
    {
        strFile.append("-");

        char szBuff[256 + 1] = {0};
        strncpy(szBuff, pFileName, chMIN(256, strlen(pFileName)));
        ReplaceIllgal(szBuff);
        strFile.append(szBuff);
    }

    int iIndex = 1;
    char bufIndex[128] = {0};
    yl::string strBak = strFile;
    strFile.append(strSuffix);

    // 查找可用的文件名
    while (pathFileExist(strFile.c_str()))
    {
        sprintf(bufIndex, "(%d)", iIndex++);
        strFile = strBak;
        strFile.append(bufIndex);
        strFile.append(strSuffix);
    }

    return strFile;
}

// 删除
bool CBaseRecord::Delete(int iId, const char * pFile, bool bDelAll /*= false*/)
{
    if (!bDelAll
            && (NULL == pFile
                || strcmp(pFile, "") == 0))
    {
        return false;
    }

    USBDEVICE_INFO("Delete storage[%d] record file [%s]", iId, !bDelAll ? pFile : "all");
    mapId2FileInfo::iterator iterMap = m_mapId2FileInfo.find(iId);
    if (iterMap == m_mapId2FileInfo.end())
    {
        return false;
    }

    yl::string strFile;
    int iOrigenalSize = (iterMap->second).listFileInfo.size();
    listRecordFileInfo & listFileInfo = (iterMap->second).listFileInfo;
    for (listRecordFileInfo::iterator iterList = listFileInfo.begin();
            iterList != listFileInfo.end();)
    {
        RecordFileInfo & sFileInfo = *iterList;
        strFile = sFileInfo.strPath + sFileInfo.strName;
        if (bDelAll
                || 0 == strcmp(pFile, strFile.c_str()))
        {
            remove(pFile);
            iterList = listFileInfo.erase(iterList);
        }
        else
        {
            ++ iterList;
        }
    }

    sync();

    // 删除文件, 存储容量改变, 重新请求
    if (iOrigenalSize != listFileInfo.size())
    {
        _StorageManager.StartVolumeCheck(true, true);
    }

    return true;
}

// 获取文件信息
RecordFileInfo * CBaseRecord::GetFileInfo(const char * pFile, int iId /*= STORAGE_DEFAULT*/)
{
    if (NULL == pFile || strcmp(pFile, "") == 0)
    {
        return NULL;
    }

    mapId2FileInfo::iterator iter;
    if (!GetStorageFileInfoItem(iId, iter))
    {
        return NULL;
    }

    listRecordFileInfo& listFileInfo = (iter->second).listFileInfo;
    yl::string strFile;
    for (listRecordFileInfo::iterator iter = listFileInfo.begin();
            iter != listFileInfo.end(); ++ iter)
    {
        RecordFileInfo & fileInfo = (*iter);
        strFile = fileInfo.strPath + fileInfo.strName;
        if (0 == strcmp(strFile.c_str(), pFile))
        {
            return &(*iter);
        }
    }

    return NULL;
}

void CBaseRecord::CreateFolder(const yl::string & strName)
{
    // 创建文件目录
    yl::string strDir = _StorageManager.GetStoragePathByIndex(m_iActiveId);
    AddPathSlash(strDir);
    strDir += strName;

    pathCreateDirectory(strDir.c_str());
}

int CBaseRecord::GetRecordFileNum(int iId)
{
    mapId2FileInfo::iterator iter;
    if (!GetStorageFileInfoItem(iId, iter))
    {
        return false;
    }

    return (iter->second).listFileInfo.size();
}

bool CBaseRecord::RequestLoadFileList(int iId)
{
    // 获取需扫描的路径
    YLList<yl::string> listReqDir;
    GetSearchDirs(iId, listReqDir);

    // 获取需要请求详细信息的文件
    YLList<yl::string> listNewFile;
    mapId2FileInfo::iterator iter = m_mapId2FileInfo.find(iId);
    if (iter != m_mapId2FileInfo.end())
    {
        StorageFileInfo & sFileInfo = iter->second;
        if (sFileInfo.listNewFile.size() > 0)
        {
            listNewFile = sFileInfo.listNewFile;
            sFileInfo.listNewFile.clear();
        }
    }

    // 请求文件扫描&信息获取
    yl::string strSupportFormat = GetSupportFormat();
    _StorageManager.RequestLoadFileList(m_eType, listReqDir,
                                        strSupportFormat, listNewFile, iId);

    // 设置文件加载中标识
    m_eFileLoadState = FILE_LOAD_STATE_LOADING;

    return true;
}

bool CBaseRecord::GetSearchDirs(int iId, YLList<yl::string> & listSearchDir)
{
    yl::string strStoragePath = _StorageManager.GetStoragePathByIndex(iId);
    if (strStoragePath.empty())
    {
        return false;
    }
    AddPathSlash(strStoragePath);

    if (!GetFolder().empty())
    {
        listSearchDir.push_back(strStoragePath + GetFolder() + "/");
    }

    if (!GetFolderEx().empty())
    {
        YLList<yl::string> listFloderEx;
        if (!commonAPI_splitStringBySeparator(GetFolderEx(), ',', listFloderEx)
                || listFloderEx.size() <= 0)
        {
            return listSearchDir.size() > 0;
        }

        for (YLList<yl::string>::iterator iter = listFloderEx.begin();
                iter != listFloderEx.end(); ++ iter)
        {
            listSearchDir.push_back(strStoragePath + *iter + "/");
        }
    }

    return listSearchDir.size() > 0;
}

bool CBaseRecord::AddNewFile(const yl::string & strFile)
{
    mapId2FileInfo::iterator iter;
    if (!GetStorageFileInfoItem(m_iActiveId, iter))
    {
        return false;
    }

    StorageFileInfo & sStorageFile = iter->second;
    sStorageFile.listNewFile.push_back(strFile);
    return true;
}

bool CBaseRecord::GetStorageFileInfoItem(int iId, mapId2FileInfo::iterator & iter)
{
    iter = m_mapId2FileInfo.find(iId);
    return iter != m_mapId2FileInfo.end();
}

#endif // IF_USB_RECORD
