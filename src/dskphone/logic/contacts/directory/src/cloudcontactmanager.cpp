#ifdef IF_FEATURE_CLOUD

#include "cloudcontactmanager.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "basemethod.h"
#include "cloudcontactaction.h"
#include "bsftdirectoryaction.h"
#include "taskaction/modtaskaction.h"
#include "directorystructure.h"
#include "directorymessagedefine.h"
#include "dircommonfunc.h"
#include "ylalgorithm.h"
#include "account/include/modaccount.h"
#include "commonunits/modcommonunits.h"

#define kszFileSavedName  "CloudDirectory_%d.xml"

#define knAccountId 0

#define MAX_FILE_ID 20

#define kszDownLoadDirList "phonebook/xml"
#define kszDownLoadDirDetial "phonebook/detial"
#define kszDownLoadDirMd5 "phonebook/md5"

#define kszStartIndexx "1"
#define kszDownloadConnt "1000"

#include "ETLLib/aestable.h"
#include "ETLLib/base64.h"

CCloudContactManager * CCloudContactManager::GetInstance()
{
    static CCloudContactManager instance;
    return &instance;
}

CCloudContactManager::CCloudContactManager()
{
    m_eDirType = CLOUD_DIR;
    m_nCloudAccountID = -1;
}

CCloudContactManager::~CCloudContactManager()
{

}

bool CCloudContactManager::PreLoad()
{
    return Init(m_nCloudAccountID);
}

bool CCloudContactManager::Init(int nAccountID)
{
    if (nAccountID < 0)
    {
        return false;
    }
    UnInit();
    m_nCloudAccountID = nAccountID;
    return MyInit();
}

void CCloudContactManager::UnInit(bool bRefresh/* = false*/)
{
    MyFinal();
    timerKillThreadTimer((UINT)&m_iTimerID);
    m_nCloudAccountID = -1;
    m_strMd5 = "";

    if (bRefresh)
    {
        etl_NotifyApp(false, CLOUD_PHONEBOOK_RELOAD, 0, 1);
    }
}

bool CCloudContactManager::MyInit()
{
    DIR_INFO("CCloudContactManager::MyInit Cloud AccountId %d", m_nCloudAccountID);
    CBaseContactManger::MyInit();

    if (MyCreatGroupsFromConfig())
    {
        SetAutoUpdateTimer();
        ReqMd5File();
        return true;
    }
    return false;
}

void CCloudContactManager::SetAutoUpdateTimer(bool bEnable/* = true*/)
{
    if (!bEnable)
    {
        timerKillThreadTimer((UINT)&m_iTimerID);
        return;
    }

    if (IsDirAvailable())
    {
        // 读取定时器时间.
        int iUpdateInterval = yl::max(configParser_GetConfigInt(kszCloudDirUpdateInterVal), 1);

        if (iUpdateInterval > 43200 || iUpdateInterval < 0)
        {
            iUpdateInterval = 1;
        }

        DIR_INFO("Set Auto Update Interval %d", iUpdateInterval);

        m_iTimerID = iUpdateInterval * 60 * 1000;
        timerSetThreadTimer((UINT)&m_iTimerID, m_iTimerID);
    }
    return;
}


DIRECTORY_RET_CODE CCloudContactManager::Update(int nGroupId /* = kszAllContactId */)
{
    DIR_INFO("CCloudContactManager Update (nGroupId = %d)", nGroupId);
    if (!IsNetworkConnected())
    {
        DIR_INFO("network is unuseable!");
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    if (nGroupId != kszAllContactId)
    {
        ReqLoadContact(nGroupId);
    }
    else
    {
        if (!IsDirAvailable())
        {
            DIR_INFO("Dir Not Config!");

            return DIR_RET_GROUP_UNEXISTED;
        }

        bool isPreLoadNotOver = false;

        for (int i = 0; i < m_vGroupId.size(); i++)
        {
            ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(m_vGroupId[i]));

            //已在下载中不在进行下载
            if (m_vGroupId[i] == kszAllContactId || pMenu == NULL)
            {
                continue;
            }

            if (pMenu->m_eStatus == FILE_DOWNLOADING || pMenu->m_bReqNext)
            {
                isPreLoadNotOver = true;
                continue;
            }

            ClearContactByGroupID(m_vGroupId[i]);

            pMenu->m_mapNameToId.clear();
            pMenu->m_eStatus = FILE_DOWNLOAD_READY;

            MyExecDownload(m_vGroupId[i], CLOUD_GET_CONTACTLIST, kszStartIndexx, kszDownloadConnt);
        }

        if (!isPreLoadNotOver)
        {
            //容错排序错乱问题。
            ClearChildIDListByGroupID(kszAllContactId)
        }
    }
    return DIR_RET_SUCC;
}

bool CCloudContactManager::MyCreatGroupsFromConfig()
{
    m_strUser = configParser_GetCfgItemStringValue(kszAccountUserName, m_nCloudAccountID);
    yl::string strPsd = configParser_GetCfgItemStringValue(kszAccountPassword, m_nCloudAccountID);
    m_strPassword = commonUnits_PwAesDataEncrypt(strPsd, "1105_VCCloud_Key");
    m_strServer = configParser_GetConfigString(kszCloudHttpServer);
    if (!m_strServer.empty() && m_strServer.substr(m_strServer.length() - 1, 1) != "/")
    {
        m_strServer.append("/");
    }

    CreatConfigGroup(kszEnterPriseId, TRID_ENTERPRISE, m_strServer);

    return !m_strServer.empty();
}

void CCloudContactManager::CreatConfigGroup(int nGroupId, yl::string strName,
        const yl::string & strServer)
{
    ContactBroadSoftMenu * pNew = new ContactBroadSoftMenu();
    pNew->m_strFullUrl = strServer + kszDownLoadDirList;
    pNew->m_strDisplayName = strName;

    DIR_INFO("CCloudContactManager::CreatConfigGroup: %d %s %s",
             nGroupId, pNew->m_strDisplayName.c_str(), pNew->m_strFullUrl.c_str());

    InsertMap(pNew, nGroupId);

}

bool CCloudContactManager::IsNeedShowAllGroup()
{
    return true;
}

bool CCloudContactManager::IsDirAvailable()
{
    if (!acc_IsCloudEnable())
    {
        // 云功能关闭，隐藏联系人
        return false;
    }
    return m_vGroupId.size() > 0;
}

int CCloudContactManager::MyExecDownload(int GroupId,
        CLOUD_OPERATION_TYPE eType /*= CLOUD_GET_CONTACTLIST*/,
        const yl::string & strStartIndex /* =  */,
        const yl::string & strCountPerPage /* = */)
{
    yl::string strUrl;
    // 生成本地保存路径.
    yl::string strFullPath(g_strTempPath);

    if (++m_nFileIndex == MAX_FILE_ID)
    {
        m_nFileIndex = 0;
    }

    //同时下载多个，一个文件会出现覆盖写的问题。
    char szTemp[128];
    memset(szTemp, 0, sizeof(szTemp));
    sprintf(szTemp, kszFileSavedName, m_nFileIndex);
    strFullPath.append(szTemp);

    ContactBroadSoftMenu * pGroup = Base2BsftMenu(GetDataById(GroupId));
    if (eType == CLOUD_GET_CONTACTLIST)
    {
        if (pGroup == NULL)
        {
            return FILE_DOWNLOAD_FAIL;
        }
        else if (pGroup->m_eStatus == FILE_DOWNLOADING)
        {
            return FILE_DOWNLOADING;
        }

        strUrl = pGroup->m_strFullUrl;

        if (!m_strUser.empty())
        {
            AddUrlParameter(strUrl, "username", m_strUser, false);

        }

        if (!m_strPassword.empty())
        {
            AddUrlParameter(strUrl, "password", m_strPassword, false);
        }

        if (!strStartIndex.empty() && !strCountPerPage.empty())
        {
            AddUrlParameter(strUrl, "start", strStartIndex, false);
            AddUrlParameter(strUrl, "count", strCountPerPage, false);
        }

    }
    else if (eType == CLOUD_GET_MD5)
    {
        strUrl = m_strServer + kszDownLoadDirMd5;

        if (!m_strUser.empty())
        {
            AddUrlParameter(strUrl, "username", m_strUser, false);

        }

        if (!m_strPassword.empty())
        {
            AddUrlParameter(strUrl, "password", m_strPassword, false);
        }
    }

    DIR_INFO("Cloud Contact download url[%s][%s]", m_strUser.c_str(), m_strPassword.c_str());

    if (!strUrl.empty())
    {
        CCloudContactAction * pAction = new CCloudContactAction(strFullPath.c_str(), strUrl.c_str(), eType);
        if (pAction != NULL)
        {
            pAction->SetExtraParam(GroupId);//保存Id
            if (pGroup != NULL)
            {
                pGroup->m_eStatus = FILE_DOWNLOADING;
            }

            pAction->SetRead(true);  // 读取文件.
            pAction->SetUsername(m_strUser);
            pAction->SetPassword(m_strPassword);
            pAction->SetCallback(this);  // 回调函数.
            pAction->SetDownLoadMode(MODE_BASIC_AUTH);

            pAction->SetSync(false);  // 异步.
            return TaskAction_ExecAction(pAction)
                   ? FILE_DOWNLOADING : FILE_DOWNLOAD_FAIL;
        }

    }
    // 其他情况, 则下载失败.
    return FILE_DOWNLOAD_FAIL;
}

void CCloudContactManager::AddUrlParameter(yl::string & strUrl, const yl::string & strName,
        const yl::string & strValue, bool isPreMatch, bool isCaseSensitive/* = true*/)
{
    if (strUrl.empty())
    {
        return;
    }
    if (strUrl.find("?") == yl::string::npos)
    {
        strUrl.append("?");
    }
    else
    {
        strUrl.append("&");
    }

    strUrl.append(strName);
    strUrl.append("=");
    if (isPreMatch)
    {
        strUrl.append("*");
    }
    strUrl.append(strValue);
    if (isPreMatch)
    {
        strUrl.append("*");
    }

    if (!isCaseSensitive)
    {
        //不区分大小写
        strUrl.append("/i");
    }
}

bool CCloudContactManager::ReqMd5File()
{
    DIR_INFO("Cloud req md5");
    return MyExecDownload(knInvalidRId, CLOUD_GET_MD5);
}

bool CCloudContactManager::ReqLoadContact(int nGroupId)
{
    DIR_INFO("CCloudContactManager::ReqLoadContact [%d]", nGroupId);
    ContactBroadSoftMenu * pGroup = Base2BsftMenu(GetMenuDataById(nGroupId));

    if (pGroup == NULL)
    {
        return false;
    }

    if (pGroup->m_eStatus == FILE_DOWNLOADING || pGroup->m_bReqNext)
    {
        return true;
    }

    pGroup->m_mapNameToId.clear();
    ClearContactByGroupID(nGroupId);
    pGroup->m_eStatus = FILE_DOWNLOAD_READY;

    return MyExecDownload(nGroupId, CLOUD_GET_CONTACTLIST, kszStartIndexx,
                          kszDownloadConnt) == FILE_DOWNLOADING;
}

void CCloudContactManager::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    CBaseContactManger::OnTaskActionCallback(pTaskAction);

    DIR_INFO("CCloudContactManager::OnTaskActionCallback begin");
    if (pTaskAction == NULL)
    {
        return;
    }

    if (pTaskAction->GetReqType() != NS_TA::TA_REQUEST_NETWORK_FILE)
    {
        return;
    }
    // 返回结果的类型为请求网络文件.
    CCloudContactAction * pNetworkAction =
        dynamic_cast<CCloudContactAction *>(pTaskAction);
    if (pNetworkAction == NULL)
    {
        return;
    }

    //Post 不处理
    if (!pNetworkAction->IsRead())
    {
        return;
    }

    int nGroupId = pNetworkAction->GetExtraParam();

    CLOUD_OPERATION_TYPE eType = pNetworkAction->GetOptType();

    switch (eType)
    {
    case CLOUD_GET_MD5:
        {
            yl::string strMd5;
            MyLoadMD5FromFile(pNetworkAction->GetFilePath(), strMd5);

            DIR_INFO("Current MD5 [%s] New MD5 [%s]", m_strMd5.c_str(), strMd5.c_str());


            //Md5不同
            if (!strMd5.empty() && strMd5 != m_strMd5)
            {
                m_strMd5 = strMd5;
                ReSetId();
                Update(kszEnterPriseId);
            }

        }
        break;
    case CLOUD_GET_CONTACTLIST:
        {
            // 是否存在下载项.
            ContactBroadSoftMenu * pContactGroup = Base2BsftMenu(GetDataById(nGroupId));

            if (pContactGroup == NULL)
            {
                DIR_INFO("pContactGroup = NULL!");
                return;
            }

            int messg = CLOUD_PHONEBOOK_RELOAD;
            // 只有状态为下载中, 才可以复制数据. 因为用户
            // 可能会在下载过程中取消下载.

            if ((pContactGroup->m_eStatus != FILE_DOWNLOADING
                    && pContactGroup->m_nServerTotalCount <= pContactGroup->m_vChildrenId.size())
                    || pContactGroup->m_eStatus == FILE_DOWNLOAD_CANCELED)
            {
                return;
            }

            if (!pNetworkAction->IsOperationSucceed())
            {
                int result = 0;

                if (pNetworkAction->GetTaskActionErrorType() == NS_TA::TA_HTTP_DOWNLOAD_UNAUTHORIZED)
                {
                    result = 401;
                }
                // 发送消息.
                DIR_INFO("Send Message to UI! result = %d", result);
                etl_NotifyApp(false, messg, nGroupId, result);
                pContactGroup->m_eStatus = FILE_DOWNLOAD_FAIL;
                pContactGroup->m_bReqNext = false;
            }
            else
            {
                bool suc = false;
                bool bReqNext = false;
                suc = MyLoadDataFromFile(nGroupId, pNetworkAction->GetFilePath(), bReqNext);
                pContactGroup->m_eStatus = suc ? FILE_DOWNLOADED_OK : FILE_DOWNLOAD_FAIL;
                DIR_INFO("Send Message to UI! suc = %d GroupId[%d],bReqNext[%d]", suc, nGroupId, bReqNext);
                etl_NotifyApp(false, messg, nGroupId, suc);
            }

            if (pContactGroup->m_eStatus == FILE_DOWNLOAD_FAIL)
            {
                m_strMd5 = "";
            }
        }
        break;
    default:
        break;
    }

    remove(pNetworkAction->GetFilePath().c_str());
}

bool CCloudContactManager::MyLoadDataFromFile(int GroupId, const yl::string & strFilePath,
        bool & bReqNest)
{
    DIR_INFO("CCloudContactManager::MyLoadDataFromFile [%s]", strFilePath.c_str());

#define kszNodeEnterPrise "Enterprise"
#define kszNodeTotalCount "totalAvailableRecords"
#define kszNodeCountPerPage "numberOfRecords"
#define kszNodestartIndex "startIndex"

    if (strFilePath.empty())
    {
        return false;
    }

    // 打开Xml文件.
    if (!pathFileExist(strFilePath.c_str()))
    {
        return false;
    }

    bool succ = false;

    xml_document doc;
    OpenBSFTXML(strFilePath.c_str(), doc);

    yl::string strNodeName;

    xml_node node = doc.first_child();

    for (; node; node = node.next_sibling())
    {
        strNodeName = node.name();
        if (strNodeName.compare(kszNodeEnterPrise) == 0)
        {
            succ = MyLoadEnterpriseXml(GroupId, &node);
            break;
        }

    }

#if 0//本需求不支持分页请求。
    //分页请求
    bReqNest = ReqNextPage(GroupId, node);
#endif
    return succ;
}

bool CCloudContactManager::MyLoadMD5FromFile(const yl::string & strFilePath, yl::string & strMd5)
{
    DIR_INFO("Parser cloud contact md5[%s]", strFilePath.c_str());

#define kszNodeMD5 "Md5"

    if (strFilePath.empty())
    {
        return false;
    }

    // 打开Xml文件.
    if (!pathFileExist(strFilePath.c_str()))
    {
        return false;
    }

    bool succ = false;

    xml_document doc;
    OpenBSFTXML(strFilePath.c_str(), doc);

    yl::string strNodeName;

    xml_node node = doc.first_child();

    for (; node; node = node.next_sibling())
    {
        strNodeName = node.name();
        if (strNodeName.compare(kszNodeMD5) == 0)
        {
            strMd5 = node.child_value();
            succ = true;
            break;
        }

        if (strNodeName == "Result")
        {
            // 打印错误代码
            xml_node nodeTmp1 = node.child("Code");
            xml_node nodeTmp2 = node.child("Message");
            DIR_INFO("Md5 code[%s] msg[%s]", nodeTmp1.child_value(), nodeTmp2.child_value());
        }
    }

    return succ;
}

bool CCloudContactManager::ReqNextPage(int GroupId, const xml_node & node)
{
    DIR_INFO("CCloudContactManager::ReqNextPage");
    if (node.empty())
    {
        return false;
    }

    ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(GroupId));

    if (pMenu == NULL)
    {
        return false;
    }

    yl::string strTotalCount;
    yl::string strCountPerPage;

    int nStartIndex = 0;

    xml_node NodeTotalCount = node.child(kszNodeTotalCount);

    xml_node NodeCountPerPage = node.child(kszNodeCountPerPage);

    xml_node NodeStartIndex = node.child(kszNodestartIndex);

    if (NodeTotalCount.empty() || NodeCountPerPage.empty()/* || NodeStartIndex.empty()*/)
    {
        pMenu->m_bReqNext = false;
        return false;
    }

    strCountPerPage = NodeCountPerPage.child_value();
    pMenu->m_nCountPerPage = atoi(strCountPerPage.c_str());

    strTotalCount = NodeTotalCount.child_value();
    pMenu->m_nServerTotalCount = atoi(strTotalCount.c_str());

    if (NodeStartIndex.empty())
    {
        nStartIndex = pMenu->m_vChildrenId.size() - pMenu->m_nCountPerPage + 1;
    }
    else
    {
        nStartIndex = atoi(NodeStartIndex.child_value());
    }

    DIR_INFO("pData->m_nCountPerPage = %d,pData->m_nServerTotalCount = %d,now count[%d]",
             pMenu->m_nCountPerPage, pMenu->m_nServerTotalCount, pMenu->m_vChildrenId.size());

    if (nStartIndex + pMenu->m_nCountPerPage <= pMenu->m_nServerTotalCount
            && !strCountPerPage.empty()
            && !strTotalCount.empty())
    {
        char szBuf[32] = {0};
        sprintf(szBuf, "%d", nStartIndex + pMenu->m_nCountPerPage);
        yl::string strStartIndex = szBuf;

        DIR_INFO("CCloudContactManager::ReqNextPage MyExecDownload");
        pMenu->m_eStatus = FILE_DOWNLOAD_READY;
        MyExecDownload(GroupId, CLOUD_GET_CONTACTLIST, strStartIndex, strCountPerPage);

        pMenu->m_bReqNext = true;

        return true;
    }

    pMenu->m_bReqNext = false;

    return false;
}


void CCloudContactManager::InsertOneContact(BaseContactRecord * pAdd, ContactBroadSoftMenu * pGroup)
{
    if (pAdd != NULL && pGroup != NULL)
    {
        if (pAdd->m_strNameIndex.empty())
        {
            pAdd->m_strNameIndex = commonAPI_transT9String(pAdd->GetDisplayName());
        }

        int nId = InsertMap(pAdd);
        pAdd->m_nParentId = pGroup->m_nId;
        pGroup->m_mapNameToId.put(pAdd->m_strDisplayName, pAdd->m_nId);

        AddIdToGroup(nId, pGroup->m_nId);

        AddIdToGroup(nId, kszAllContactId);

    }
}

bool CCloudContactManager::MyLoadEnterpriseXml(int GroupId,
        xml_node * pElem)
{
    if (GroupId != kszEnterPriseId || pElem == NULL)
    {
        return false;
    }

    ContactBroadSoftMenu * pGroup = Base2BsftMenu(GetDataById(GroupId));

    if (pGroup == NULL)
    {
        return false;
    }

#define kszNodeEnterPriseDirectory "enterpriseDirectory"
#define kszNodeDirectoryDetails "directoryDetails"

    // 读取各个节点的值.
    xml_node nodeDirectoryDetails = pElem->child(kszNodeEntry);
    bool isV1 = true;

    if (!nodeDirectoryDetails)
    {
        isV1 = false;
        xml_node nodeGroupDirectory = pElem->child(kszNodeEnterPriseDirectory);
        nodeDirectoryDetails = nodeGroupDirectory.child(kszNodeDirectoryDetails);
    }

    for (; nodeDirectoryDetails;
            nodeDirectoryDetails = nodeDirectoryDetails.next_sibling(isV1 ? kszNodeEntry :
                                   kszNodeDirectoryDetails))
    {
        ContactCloudEnterprise * pNew = new ContactCloudEnterprise();
        if (pNew != NULL)
        {
            pNew->ParseFromXMLNode(nodeDirectoryDetails);//解析联系人
            pNew->m_strDisplayName = pNew->m_strFirstName;
            InsertOneContact(pNew, pGroup);
        }
    }  // Outer for.

    return true;
}


bool CCloudContactManager::OnTimeout(unsigned int uTimerID, bool bInTalk)
{
    if (bInTalk)
    {
        // 通话中不去更新
        return false;
    }

    if (uTimerID == (UINT)&m_iTimerID)
    {
        ReqMd5File();
        return true;
    }

    return false;
}

DIRECTORY_RET_CODE CCloudContactManager::AddIdToGroup(int nId, int nGroupId,
        bool IsSort /* = true */, bool isOrder /* = true */)
{
    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);
    if (pGroup == NULL)
    {
        return DIR_RET_GROUP_UNEXISTED;
    }

    if (!IsSort)
    {
        pGroup->m_vChildrenId.push_back(nId);

        return DIR_RET_SUCC;
    }

    if (pGroup->m_vChildrenId.size() == 0)
    {
        pGroup->m_vChildrenId.push_back(nId);
        return DIR_RET_SUCC;
    }

    int nLeft = 0;

    int nRingt = pGroup->m_vChildrenId.size() - 1;



    while (nLeft <= nRingt)
    {
        int nMid = (nLeft + nRingt) / 2;

        int nCmp = 0;

        nCmp = GetCompResult(pGroup->m_vChildrenId[nMid], nId);

        if ((nCmp > 0 && isOrder) || (nCmp < 0 && !isOrder))
        {
            nRingt = nMid - 1;
        }
        else
        {
            nLeft = nMid + 1;
        }
    }

    int nIndex = nLeft > nRingt ? nLeft : nRingt;

    if (nIndex == pGroup->m_vChildrenId.size())
    {
        pGroup->m_vChildrenId.push_back(nId);
    }
    else
    {
        pGroup->m_vChildrenId.insertAt(nIndex, nId);
    }

    return DIR_RET_SUCC;
}
#endif //#ifdef IF_FEATURE_CLOUD
