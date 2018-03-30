#include "locallogmanager.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "directorymessagedefine.h"
#include "basemethod.h"
#include "directorystructure.h"
#include "path_define.h"
#include "localcontactmanager.h"
#include "extended/event/include/modevent.h"
#include "extended/actionuri_url/include/modactionuri_url.h"

#include "account/include/modaccount.h"
#include "wireless/bluetoothNew/include/modbluetooth.h"

#include "bluetoothcontactmanager.h"

#include "contacts/directory/include/moddirinterface.h"
#include "contacts/directory/include/commondata.h"
#include "talk/digitmap/include/moddigitmap.h"

// 定时保存的时间间隔.
#define knSaveTimeInterval          500  // (5s).

// 每个通话记录最大的保存数目.
#ifdef _DECT
#define knMaxPerRecordCount  20
#else
#define knMaxPerRecordCount  100
#endif

// 组显示名
//#define kszLocalLogAllCalls           "All Calls"
#define kszNameMissedCalls      TRID_MISSED_CALLS

#define kszNamePlacedCalls      TRID_PLACED_CALLS
#define kszNameReceivedCalls    TRID_RECEIVED_CALLS

#define kszNameForwardedCalls   TRID_FORWARDED_CALLS

#define kszNameMissNotifGroup    "Missed Notif"
#define kszNameForwardNotifGroup "ForWard Notif"

// 搜索最近记录时的最大数.
#define knMaxRecentCount    10

//每条历史记录，最多数量
#define kszMaxLogCount      20

// 用于统计次数的结构体.
struct stTemp
{
    yl::string m_strName;  // 名字.
    yl::string m_strNumber;  // 号码.
    int m_nTimes;  // 次数.
    yl::string m_strPic;

    stTemp()
    {
        Reset();
    }

    void Reset()
    {
        m_strName.clear();
        m_strNumber.clear();
        m_nTimes = 0;
        m_strPic.clear();
    }

    // Override operator <.
    static bool CompareCal(const stTemp & refLeft, const stTemp & refRight)
    {
        if (refLeft.m_nTimes > refRight.m_nTimes)
        {
            return true;
        }
        return false;
    }
};


CLocalLogManager * CLocalLogManager::GetInstance()
{
    static CLocalLogManager instance;
    return &instance;
}

CLocalLogManager::CLocalLogManager()
{
    m_eDirType = LOCAL_LOG;
    m_iTimerID = knSaveTimeInterval;
}

CLocalLogManager::~CLocalLogManager()
{
}

bool CLocalLogManager::Init()
{
    MyInit();
    InitGroup();

    yl::string strMacDirPath = CONFIG_PATH "sync_data/";
    char szTemp[64] = {0};
    sprintf(szTemp, "%s-calllog.xml", (LPCSTR)netGetMACText('\0'));
    strMacDirPath.append(szTemp);

    bool suc = false;
    if (1 == configParser_GetConfigInt(kszLocalCallLogSyncEnable)
            && pathFileExist(strMacDirPath.c_str()))
    {
        suc = ParseDataFromFile(strMacDirPath);
        SaveToFile();
        remove(strMacDirPath.c_str());
    }
    else
    {
        suc = ParseDataFromFile();
    }

    return suc;
}

void CLocalLogManager::InitGroup()
{
    CreatConfigGroup(kszMissLogId, kszNameMissedCalls, true);
    CreatConfigGroup(kszPlacedLogId, kszNamePlacedCalls, true);
    CreatConfigGroup(kszReceivedLogId, kszNameReceivedCalls, true);
    CreatConfigGroup(kszMissNotifId, kszNameMissNotifGroup, false);

#if IF_FEATURE_FORBID_FORWARD_CALLLOG
    if (configParser_GetConfigInt(kszForwardCallLog) != 0)
#endif
    {
        CreatConfigGroup(kszForWardedLogId, kszNameForwardedCalls, true);
        CreatConfigGroup(kszForwardNotifId, kszNameForwardNotifGroup, false);
    }
}

void CLocalLogManager::CreatConfigGroup(int nGroupId, yl::string strName, bool isShow)
{
    DirGroupRecord * pGroup = new DirGroupRecord();

    pGroup->m_strDisplayName = strName;

    InsertMap(pGroup, nGroupId);

    if (isShow)
    {
        AddIdToMenuShowList(nGroupId);
    }
}

bool CLocalLogManager::GetContactListByGroupId(ContactListData* pData, const int nGroupId)
{
    ClearNotifyListByID(nGroupId);

#ifdef _DECT
    return MyCopyList(pData, nGroupId, eReqID);
#else
    return CBaseContactManger::MyCopyList(pData, nGroupId);
#endif
}

void CLocalLogManager::ClearNotifyListByID(const int nGroupId, REQUEST_ID eReqID /*= DEFAULT_ID*/)
{
    if (nGroupId == kszAllContactId)
    {
        ClearNotifyList(kszForwardNotifId, eReqID);
        ClearNotifyList(kszMissNotifId, eReqID);
    }
    else if (nGroupId == kszMissLogId)
    {
        ClearNotifyList(kszMissNotifId, eReqID);
    }
    else if (nGroupId == kszForWardedLogId)
    {
        ClearNotifyList(kszForwardNotifId, eReqID);
    }
}

// 读取并解析历史记录配置文件
bool CLocalLogManager::ParseDataFromFile(yl::string strPath /*= ""*/)
{
    // 获取配置文件全路径.
    yl::string strFullPath(szCalllistFile);

    if (!strPath.empty())
    {
        strFullPath = strPath;
    }

    if (!pathFileExist(strFullPath.c_str()))
    {
        CALLLOG_WARN("file [%s] not exsit", strFullPath.c_str());
        return false;
    }

    xml_document doc;
    doc.load_file(strFullPath.c_str());

    xml_node rootNode = doc.child(kszCallLogNodeRoot);
    if (!rootNode)
    {
        return false;
    }

    for (xml_node node = rootNode.child(kszNodeItem);
            node;
            node = node.next_sibling(kszNodeItem))
    {
        LocalLogItemRecord * pLog = new LocalLogItemRecord();

        if (pLog != NULL)
        {
            //解析联系人
            pLog->ParseFromXMLNode(node);

            DirGroupRecord * pGroup = GetMenuDataById(pLog->m_nParentId);

            if (pGroup == NULL)
            {
                delete pLog;
                continue;
            }

            AddContact(pLog, pLog->m_nParentId, false);
        }
    }

    return true;
}

// 保存历史记录到配置文件中
bool CLocalLogManager::SaveToFile(yl::string strPath /*= ""*/)
{
    DirGroupRecord * pAll = GetMenuDataById(kszAllContactId);

    if (pAll == NULL)
    {
        return false;
    }
    yl::string strFullPath(szCalllistFile);

    if (!strPath.empty())
    {
        strFullPath = strPath;
    }


    if (!strFullPath.empty())
    {
        // 设置Xml文件头
        xml_document docSave;
        xml_node nodeDecl = docSave.prepend_child(node_declaration);
        nodeDecl.append_attribute("version", "1.0");
        nodeDecl.append_attribute("encoding", "utf-8");

        xml_node nodeRootNode = docSave.append_child(kszCallLogNodeRoot);
        //保存数据
        for (int i = 0; i < pAll->m_vChildrenId.size(); i ++)
        {
            BaseDirDataRecord * pRecord = GetDataById(pAll->m_vChildrenId[i]);
            if (pRecord != NULL)
            {
                xml_node nodeItem = nodeRootNode.append_child(kszNodeItem);
                pRecord->ToXMLNode(nodeItem);
            }
        }
        return docSave.save_file(strFullPath.c_str());
    }
    return false;
}

// 增加一条通话记录.
int CLocalLogManager::AddLog(const LocalLogItemRecord & refData)
{
    LocalLogItemRecord & refLogItem = const_cast<LocalLogItemRecord &>(refData);
    DIR_INFO("AddLog id=[%d][%d] line=[%d] [%s][%s]", refData.m_nParentId, refData.m_nId,
             refData.m_nLine, refLogItem.GetDisplayName().c_str(), refLogItem.GetNumber().c_str());

#if IF_FEATURE_METASWITCH_CALLLOG
    if (!LocalLog_IsEnable())
    {
        return knInvalidRId;
    }
#endif

    // 判断是否开启了保存历史记录的开关.
    bool bSaveCallLog = configParser_GetConfigInt(kszSaveCallLogSwitch) == 1;

    if (!bSaveCallLog && refData.m_nParentId == kszPlacedLogId)
    {
        m_stNoSavePlacedLog = refData;
    }

    if (!bSaveCallLog
#if IF_FEATURE_BSFT_MISSCALL
            && refData.m_nParentId != kszMissLogId
            && refData.m_nParentId != kszForWardedLogId
#endif
       )
    {
        DIR_INFO("Directory: kszSaveCallLogSwitch = 0");
        return knInvalidRId;
    }

    if (refData.m_nParentId <= kszAllContactId || refData.m_nParentId > kszForWardedLogId)
    {
        return knInvalidRId;
    }

    // 判断如果未开启保存missed call, 则不保存.
    if (refData.m_nParentId == kszMissLogId)
    {
        bool bMissedCallLog = acc_IsAccountSaveMissCallLog(refData.m_nLine);
        if (!bMissedCallLog)
        {
            DIR_INFO("Directory: bMissedCallLog = false");
            return knInvalidRId;
        }
    }

#if IF_FEATURE_FORBID_FORWARD_CALLLOG
    // 判断如果未开启forward, 则不保存.
    if (refData.m_nParentId == kszForWardedLogId)
    {
        if (configParser_GetConfigInt(kszForwardCallLog) == 0)
        {
            DIR_INFO("CLocalLogManager::AddLog forbid forward call log!");
            return knInvalidRId;
        }
    }
#endif

    int nContactID = ProcessMergeLog(refData);
    if (nContactID != knInvalidRId)
    {
        etl_NotifyApp(false, LOCAL_CALLLOG_UPLOAD, 0, 0);
        //开启定时保存计时器
        SetDirty(true);
        return nContactID;
    }

    LocalLogItemRecord * pNew = new LocalLogItemRecord();

    *pNew = refData;

    //做类型判断，对来电类型的号码做规则转换进行保存。
    if (pNew->GetLogType() == CT_INCOMING
            || pNew->GetLogType() == CT_MISSCALL
            || pNew->GetLogType() == CT_FORWARDCALL)
    {
        //pNew->GetNumber();
        yl::string formatNumber = pNew->GetNumber();
        bool ret = DigitMap_GetFmtIncomingNumber(formatNumber);
        if (ret)
        {
            APP_INFO(" ## DigitMap_GetFmtIncomingNumber formatNumber == %s ", formatNumber.c_str());
            pNew->SetNumber(formatNumber);
            pNew->m_strDisplayName = formatNumber;
        }
    }

    //本地联系人匹配到名字和传入历史记录名字一致，则清除历史记录本身名字
    if (!pNew->m_strDisplayName.empty()
            && g_pLocalDirManager->GetContactNameByNumber(pNew->GetNumber()).compare(
                pNew->m_strDisplayName) == 0
#ifdef IF_BT_SUPPORT_PHONE
            || g_pBlueToothDirManager->GetContactNameByNumber(pNew->GetNumber()).compare(
                pNew->m_strDisplayName) == 0
#endif
            && !pNew->m_strDisplayName.empty())
    {
        pNew->m_strDisplayName.clear();
    }

    int nId = pNew->m_nId;

    nId = bSaveCallLog ? AddContact(pNew, pNew->m_nParentId, true, true, nId) : InsertMap(pNew);

    DIR_INFO("AddLog id [%d]?=[%d] save=[%d]", refData.m_nId, nId, bSaveCallLog);

    if (nId > 0)
    {
        if (pNew->m_nParentId == kszMissLogId)
        {
            AddIdToGroup(nId, kszMissNotifId, true, true);
            if (!bSaveCallLog)
            {
                m_listNoSaveMissId.push_back(nId);
            }
        }
        else if (pNew->m_nParentId == kszForWardedLogId)
        {
            AddIdToGroup(nId, kszForwardNotifId, true, true);
            if (!bSaveCallLog)
            {
                m_listNoSaveFWDId.push_back(nId);
            }
        }

        if (bSaveCallLog)
        {
            etl_NotifyApp(false, LOCAL_CALLLOG_UPLOAD, 0, 0);
            //开启定时保存计时器
            SetDirty(true);
        }
    }
    return nId;
}

DIRECTORY_RET_CODE CLocalLogManager::AddIdToGroup(int nId, int nGroupId, bool IsSort /* = true */,
        bool isOrder /* = true */)
{
    DIRECTORY_RET_CODE eRet = DIR_RET_FAIL;

    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);
    if (pGroup == NULL)
    {
        return DIR_RET_FAIL;
    }

    if (nGroupId != kszAllContactId && pGroup->m_vChildrenId.size() >= knMaxPerRecordCount)
    {
        VectorId remID;
        remID.push_back(pGroup->m_vChildrenId[pGroup->m_vChildrenId.size() - 1]);
        RemoveRecord(remID, nGroupId);
    }

    //按发起时间逆向排序
    eRet = CBaseContactManger::AddIdToGroup(nId, nGroupId, IsSort, false);

    if (nGroupId == kszForwardNotifId || nGroupId == kszMissNotifId)
    {
        NotifyStatusChange(nGroupId);
    }

    return eRet;
}

// 设置改动标识
void CLocalLogManager::SetDirty(bool bDirty)
{
    if (bDirty)
    {
        ResetSearchKey();
    }

    if (m_bIsDirty != bDirty)
    {
        m_bIsDirty = bDirty;

        // 如果有改动则启动计时器.
        if (m_bIsDirty)
        {
            CALLLOG_INFO("begin to set timer to save call log file");
            timerSetThreadTimer((unsigned int)&m_iTimerID, m_iTimerID);
        }
    }
}


// 定时器处理
bool CLocalLogManager::OnTimer(unsigned int uTimerID)
{
    // 如果是保存计时器, 则保存配置文件.
    if (uTimerID != (unsigned int)&m_iTimerID)
    {
        return false;
    }


    if (m_bIsDirty)
    {
        CALLLOG_INFO("calllog timer out, ready fo save file");
        SaveToFile();
        //重新置为False
        SetDirty(false);
    }

    timerKillThreadTimer(uTimerID);

    return true;// 返回保存文件是否被成功处理
}

void CLocalLogManager::NotifyStatusChange(int nGroupId)
{
    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);

    if (pGroup != NULL)
    {
        if (nGroupId == kszForwardNotifId)
        {
            etl_NotifyApp(false, LOCAL_CALLLOG_MESSAGE_FORWARD, 0, pGroup->m_vChildrenId.size());
        }
        else if (nGroupId == kszMissNotifId)
        {

            etl_NotifyApp(false, LOCAL_CALLLOG_MESSAGE_MISSED, 0, pGroup->m_vChildrenId.size());
        }
    }
}

void CLocalLogManager::ClearNotifyList(int nGroupId, int nAccountID /*= -1*/)
{
    if (nGroupId != kszMissNotifId && nGroupId != kszForwardNotifId)
    {
        return;
    }
    VectorId tmpVecId;
    RemoveNoSaveNotifyRecord(tmpVecId, nGroupId);

    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);

    if (NULL == pGroup
#if IF_BUG_36591
            || 0 == pGroup->m_vChildrenId.size()
#endif
       )
    {
        return;
    }

#if IF_BUG_36591
    if (nGroupId == kszMissNotifId)
    {
#if IF_FEATURE_EVENT_MODULE
        event_SendEvent(EVENT_VIEW_MISSCALL);
#else
        actURL_SendActionURL(ACTION_URL_VIEW_MISSCALL);
#endif
    }
    else if (nGroupId == kszForwardNotifId)
    {
#if IF_FEATURE_EVENT_MODULE
        event_SendEvent(EVENT_VIEW_FORWARD);
#else
        actURL_SendActionURL(ACTION_URL_VIEW_FORWARD);
#endif
    }
#endif

    bool bSucc = true;
    if (nAccountID != -1 && kszMissNotifId == nGroupId)
    {
        for (int i = 0; i < pGroup->m_vChildrenId.size(); ++i)
        {
            SmartPtr pSmart = GetDetailById(pGroup->m_vChildrenId[i]);
            if (pSmart.IsEmpty())
            {
                continue;
            }

            LocalLogItemRecord * pLocalLog = (LocalLogItemRecord *)pSmart.Get();

            if (pLocalLog != NULL && pLocalLog->m_nLine == nAccountID)
            {
                bSucc = true;
                pGroup->m_vChildrenId.removeAt(i);
                --i;
            }
        }
    }
    else
    {
        pGroup->m_vChildrenId.clear();
    }

    if (bSucc)
    {
        NotifyStatusChange(nGroupId);
    }
}

// 删除一条或多条通话记录.
bool CLocalLogManager::RemoveLog(VectorId & vecId, int nGroupId)
{
    DIR_INFO("CLocalLogProcessor::RemoveLog");

    if (nGroupId == kszMissNotifId || nGroupId == kszForwardNotifId)
    {
        RemoveNoSaveNotifyRecord(vecId, nGroupId);

        RemoveIdFromGroup(vecId, nGroupId);

        NotifyStatusChange(nGroupId);

        return true;
    }

    RemoveRecord(vecId, nGroupId);

    SetDirty(true);
    etl_NotifyApp(false, LOCAL_CALLLOG_UPLOAD, 0, 0);

    return true;
}

bool CLocalLogManager::ClearLogs(int nGroupId, int nAccountID/* = -1*/)
{
    DIR_INFO("CLocalLogManager::ClearLogs group[%d]", nGroupId);

    if (nGroupId == kszMissNotifId || nGroupId == kszForwardNotifId)
    {
        ClearNotifyList(nGroupId, nAccountID);

        return true;
    }
    else if (nGroupId == kszAllContactId)
    {
        ClearNotifyList(kszMissNotifId);
        ClearNotifyList(kszForwardNotifId);
    }
    else if (nGroupId == kszMissLogId)
    {
        ClearNotifyList(kszMissNotifId);
    }
    else if (nGroupId == kszForWardedLogId)
    {
        ClearNotifyList(kszForwardNotifId);
    }

    bool bRec = ClearContactByGroupID(nGroupId);


    if (bRec)
    {
        SetDirty(true);
        etl_NotifyApp(false, LOCAL_CALLLOG_UPLOAD, 0, 0);
    }

    return bRec;
}

bool CLocalLogManager::SearchCallLogRecentOutCall(SuperSearchItemListData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    YLList<stTemp> listCalculated;  // 统计链表.

    m_contactLock.Lock();

    ContactListData
    logList;    // http://10.2.1.199/Bug.php?BugID=85238 要获取Placed Call的全部记录，个数不能传10，默认传0可获取全部数据
//  logList.m_nIndexFrom = pData->m_nIndexFrom;
//  logList.m_nCountToGet = pData->m_nCountToGet;
    GetCallLogByReqID(&logList, kszPlacedLogId);

    // 遍历记录列表, 统计每个拨出号码的次数.
    for (int i = 0; i < logList.m_listId.size(); i++)
    {
        LocalLogItemRecord * pItemData = Base2LocalLog(GetDataById(logList.m_listId[i]));
        if (pItemData == NULL)
        {
            continue;
        }

        bool bFound = false;
        //  遍历统计链表.
        for (YLList<stTemp>::iterator iterCal = listCalculated.begin();
                iterCal != listCalculated.end();
                iterCal++)
        {
            stTemp & refCal = *iterCal;
            if (refCal.m_strNumber.compare(pItemData->GetNumber()) == 0)
            {
                refCal.m_nTimes++;
                bFound = true;
                break;
            }
        }

        // 如果没找到, 则插入一个到链表末尾.
        if (!bFound)
        {
            stTemp stTmp;
            stTmp.m_strName = pItemData->GetDisplayName();
            stTmp.m_strNumber = pItemData->GetNumber();

#ifdef IF_BT_SUPPORT_PHONE
            stTmp.m_strPic = g_pBlueToothDirManager->GetContactImageByNumber(stTmp.m_strNumber);
#endif
            if (stTmp.m_strPic.empty())
            {
                stTmp.m_strPic = g_pLocalDirManager->GetContactImageByNumber(stTmp.m_strNumber);
            }

            stTmp.m_nTimes = 1;
            listCalculated.push_back(stTmp);
        }
    }

    // 清空内存.
    m_contactLock.Unlock();

    // 对结果链表进行排序.
    Sort(listCalculated, stTemp::CompareCal);

    // 获取数据
    int nIndexFrom = pData->m_nIndexFrom;
    int nCountGet = pData->m_nCountToGet;
    // 如果nIndexFrom == 0 && nCountToGet == 0, 则获取该组所有联系人.
    if (nIndexFrom == 0 && nCountGet == 0)
    {
        nCountGet = listCalculated.size();
    }

    int nIndex = 0;
    DirectorySearchData stCLSD;
    for (YLList<stTemp>::const_iterator citerTemp = listCalculated.begin();
            citerTemp != listCalculated.end();
            citerTemp++)
    {
        if (nIndex >= nIndexFrom)
        {
            const stTemp & pTemp = *citerTemp;

            stCLSD.Reset();
            stCLSD.m_strName = pTemp.m_strName;
            if (stCLSD.m_strName.empty())
            {
                stCLSD.m_strName = pTemp.m_strNumber;
            }
            stCLSD.m_strNumber = pTemp.m_strNumber;
            stCLSD.m_strPic = pTemp.m_strPic;
            pData->m_listContact.push_back(stCLSD);

            if (--nCountGet == 0)
            {
                break;
            }
        }
        nIndex++;
    }

    pData->m_nCountToGet = listCalculated.size();

    return true;
}

void CLocalLogManager::Reload()
{
    MyFinal();
    Init();
}

const yl::string & CLocalLogManager::GetSortKeyById(int nId)
{
    LocalLogItemRecord * pLog = Base2LocalLog(GetDataById(nId));

    if (pLog != NULL)
    {
        return pLog->m_strBornTick;
    }

    return kszEmptyString;
}

SmartPtr CLocalLogManager::GetDetailById(int nId)
{
    MapContactInfo::iterator iter = m_mapAllInfo.find(nId);

    if (iter == m_mapAllInfo.end())
    {
        return SmartPtr();
    }

    BaseDirDataRecord * pBaseRecord = (BaseDirDataRecord *) iter->second;
    SmartPtr pDetail = TransferToSmartPtr(pBaseRecord);

    if (NULL == pDetail.Get())
    {
        return SmartPtr();
    }

    // 列表界面详情不需要获取账号
    if (pBaseRecord->m_eType == DIR_TYPE_LOCALLOG_CALLLOG)
    {
        LocalLogItemRecord * pRecord = (LocalLogItemRecord *) pDetail.Get();

#ifdef IF_BT_SUPPORT_PHONE
        if (pRecord->m_nLine != Bluetooth_GetBtAccountID())
#endif
        {
            //转化为line id
            if (pRecord->m_strLocalSipName.empty())
            {
                pRecord->m_nLine = -1;
            }
            else
            {
                if (acc_GetRegisterName(acc_GetDirectIPAccountID()) == pRecord->m_strLocalSipName)
                {
                    pRecord->m_nLine = acc_GetDirectIPAccountID();
                }
                else
                {
                    pRecord->m_nLine = acc_GetAccountIDByName(pRecord->m_strLocalSipName.c_str(),
                                       pRecord->m_strLocalSipServer.c_str());
                }
            }
        }
    }

    return pDetail;
}

bool CLocalLogManager::GetLastOutCallNumAndAccountID(yl::string & strNumber, int & nAccountId)
{
    //保存历史记录没有打开
    if (configParser_GetConfigInt(kszSaveCallLogSwitch) == 0
            && m_stNoSavePlacedLog.m_nLine >= 0)
    {
        strNumber = m_stNoSavePlacedLog.GetNumber();
        nAccountId = m_stNoSavePlacedLog.m_nLine;
        return true;
    }

    DirGroupRecord * pGroup = GetMenuDataById(kszPlacedLogId);
    if (pGroup == NULL || pGroup->m_vChildrenId.size() == 0)
    {
        return false;
    }

    LocalLogItemRecord * pLog = Base2LocalLog(GetDataById(pGroup->m_vChildrenId[0]));

    if (pLog != NULL)
    {
        strNumber = pLog->GetNumber();
        nAccountId = pLog->m_nLine >= 0 ? pLog->m_nLine :
                     acc_GetAccountIDByName(pLog->m_strLocalSipName.c_str(), pLog->m_strLocalSipServer.c_str());
        return true;
    }

    return false;
}

bool CLocalLogManager::IsNeedShowAllGroup()
{
    return true;
}

bool CLocalLogManager::IsCloudCallLog(int nId)
{
    LocalLogItemRecord * pLog = Base2LocalLog(GetDataById(nId));
    if (pLog != NULL)
    {
        yl::string strLogLocalServer = configParser_GetConfigString(kszCloudSipServer);
        if (pLog->m_strLocalSipServer == strLogLocalServer)
        {
            return true;
        }
    }
    return false;
}

bool CLocalLogManager::GetCallLogByReqID(ContactListData * pData, const int nGroupId)
{
    if (!GetContactListByGroupId(pData, nGroupId))
    {
        return false;
    }

    return true;
}

bool CLocalLogManager::ExportContactToFile(yl::string strFile)
{
    return SaveToFile(strFile);
}

//删除一条或者多头不保存的MissCall或者FWD历史记录数据
bool CLocalLogManager::RemoveNoSaveNotifyRecord(VectorId & vecId, int nGroupId)
{
    if (nGroupId != kszMissNotifId && nGroupId != kszForwardNotifId)
    {
        return false;
    }

    if (nGroupId == kszMissNotifId
            && m_listNoSaveMissId.size() == 0)
    {
        return false;
    }

    if (nGroupId == kszForwardNotifId
            && m_listNoSaveFWDId.size() == 0)
    {
        return false;
    }

    YLList<int> & tmpList = (nGroupId == kszMissNotifId ? m_listNoSaveMissId : m_listNoSaveFWDId);
    YLList<int>::ListIterator it = tmpList.begin();
    // 删除整组数据
    if (vecId.size() == 0)
    {
        for (; it != tmpList.end(); it++)
        {
            int nId = (*it);
            VectorId vecTempId;
            vecTempId.push_back(nId);
            RemoveRecord(vecTempId);
        }

        tmpList.clear();

    }
    //删除一条或者多条
    else
    {
        for (int i = 0; i < vecId.size(); i++)
        {
            for (; it != tmpList.end(); it++)
            {
                int nId = (*it);
                if (vecId[i] == nId)
                {
                    VectorId vecTempId;
                    vecTempId.push_back(vecId[i]);
                    RemoveRecord(vecTempId);
                    tmpList.erase(it);
                    break;
                }
            }
        }
    }

    return true;
}

#if IF_FEATURE_MUTI_CALLLOG
int CLocalLogManager::GetLastMissAccountID()
{
    DirGroupRecord * pGroup = GetMenuDataById(kszMissNotifId);
    if (pGroup == NULL)
    {
        DIR_INFO("CLocalLogManager::GetLastMissAccountID Group Null");
        return -1;
    }

    if (pGroup->m_vChildrenId.size() <= 0)
    {
        return -1;
    }

    int nLastContactID = pGroup->m_vChildrenId[0];
    LocalLogItemRecord * pLog = Base2LocalLog(GetDataById(nLastContactID));
    if (pLog != NULL)
    {
        DIR_INFO("Current Line ID [%d]", pLog->m_nLine);
        return pLog->m_nLine;
    }

    return -1;
}
#endif

bool CLocalLogManager::ImportCalllog(yl::string strFilePath /* = ""*/)
{
    bool suc = false;
    if (strFilePath.empty())
    {
        Reload();
    }
    else
    {
        DIR_INFO("CLocalLogManager::ImportCalllog [%s]", strFilePath.c_str());

        MyFinal();
        MyInit();
        InitGroup();
        suc = ParseDataFromFile(strFilePath);
        SaveToFile();
        remove(strFilePath.c_str());
    }

    return suc;
}


int CLocalLogManager::ProcessMergeLog(const LocalLogItemRecord & refData)
{
    if (!LocalLog_MergeLogEnable())
    {
        return knInvalidRId;
    }

    DirGroupRecord * pGroup = GetMenuDataById(kszAllContactId);
    if (pGroup == NULL || pGroup->m_vChildrenId.size() <= 0)
    {
        return knInvalidRId;
    }

    int nContactID = pGroup->m_vChildrenId[0];
    LocalLogItemRecord * pLastLogData = Base2LocalLog(GetDataById(nContactID));
    if (pLastLogData == NULL)
    {
        return knInvalidRId;
    }

    DIR_INFO("Last Log ID [%d] ParentID [%d] Display Name [%s]",
             nContactID, pLastLogData->m_nParentId, pLastLogData->m_strDisplayName.c_str());

    // 历史记录类型不同
    if (pLastLogData->m_nParentId != refData.m_nParentId)
    {
        return knInvalidRId;
    }

    // 本地账号不同
    if (pLastLogData->m_strLocalSipName != refData.m_strLocalSipName
            || pLastLogData->m_strLocalSipServer != refData.m_strLocalSipServer)
    {
        return knInvalidRId;
    }

    //不是misscall Forwardcall 且同时不是拒接等类型
    if (pLastLogData->m_eRelation == CR_NONE
            && refData.m_nParentId != kszMissLogId
            && refData.m_nParentId != kszForWardedLogId)
    {
        return knInvalidRId;
    }

    // Relation不同
    if (pLastLogData->m_strRelationNum != refData.m_strRelationNum
            || pLastLogData->m_eRelation != refData.m_eRelation)
    {
        return knInvalidRId;
    }

    // 号码不同
    if (pLastLogData->GetNumber() != refData.GetNumber()
            || pLastLogData->m_strRemoteSipServer != refData.m_strRemoteSipServer)
    {
        return knInvalidRId;
    }

    //相同历史记录
    pLastLogData->m_nNumOfTimes++;
    pLastLogData->m_strBornTick = refData.m_strBornTick;

    DIR_INFO("Number Of Times [%d] Number [%s]",
             pLastLogData->m_nNumOfTimes, pLastLogData->GetNumber().c_str());

    return nContactID;
}

