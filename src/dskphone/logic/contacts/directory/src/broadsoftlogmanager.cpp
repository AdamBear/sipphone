#include "broadsoftlogmanager.h"

#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "basemethod.h"
#include "taskaction/modtaskaction.h"
#include "threadtask/include/networkfileaction.h"
#include "threadtask/include/modthreadtask.h"
#include "directorymessagedefine.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "locallogmanager.h"
#include "dircommonfunc.h"
#include "contacts/directory/include/moddirinterface.h"

// BroadSoft文件下载到本地的保存名.
#define kszFileSavedName  "BroadSoftCallLog_%d.xml"

#define kszPlaced  "placed"
#define kszMissed  "missed"
#define kszReceived "received"
#define kszCallLogs "CallLogs"

CBroadSoftLogManager * CBroadSoftLogManager::GetInstance()
{
    static CBroadSoftLogManager instance;
    return &instance;
}

CBroadSoftLogManager::CBroadSoftLogManager()
{
    m_eDirType = BSFT_LOG;
    m_nLastReqGroupID = -1;
    MyInit();
}

CBroadSoftLogManager::~CBroadSoftLogManager()
{

}

bool CBroadSoftLogManager::MyInit()
{
    ReSetId(kszLogBeginId);
    return MyCreatGroupsFromConfig();
}

void CBroadSoftLogManager::ReloadConfig()
{
    MyFinal();
    MyInit();
}

void CBroadSoftLogManager::LoadAllCalllog()
{
    YLVector<int>::iterator it = m_vGroupId.begin();
    for (; it != m_vGroupId.end(); ++it)
    {
        BSFTLogGroupRecord * pGroup = Base2BSFTLogGroup(GetDataById(*it));

        if (pGroup == NULL)
        {
            continue;
        }

        pGroup->m_eStatus = FILE_DOWNLOAD_READY;
        MyExecDownload(*it);
    }
}

bool CBroadSoftLogManager::MyCreatGroupsFromConfig()
{
    if (configParser_GetConfigInt(kszBsftEnableCallLog) == 0
            || configParser_GetConfigInt(kszBroadsoftActive) != 1
            || configParser_GetConfigInt(kszXSISwitch) == 0)
    {
        return true;
    }

#if IF_FEATURE_MUTI_CALLLOG
    if (BsftLog_IsMutilLogEnable())
    {
        for (int i = 0; i < acc_AccountNum(); ++i)
        {
            yl::string strServer = GetServerUrlByAccountID(i);
            // 创建组
            int nRootGroupID = GetAccGroupIDByAccountID(i);
            CreatConfigGroup(nRootGroupID, strServer, acc_GetAccountShowText(i), i, false);
            CreatChildGroup(nRootGroupID);
        }
        return true;
    }
    else
#endif
    {
        bool bIsAllSipAuth = (configParser_GetConfigInt(kszAllowSipAuth) == 1);

        yl::string strXSIUserId = configParser_GetCfgItemStringValue(kszAccountXSIUserID, knAccountId);
        if (bIsAllSipAuth)
        {
            //userId为sip UserName  passwordc存的是userID：sip passward
            m_strUser = configParser_GetCfgItemStringValue(kszAccountRegisterName, knAccountId);
            m_strPassword = strXSIUserId;
            m_strPassword.append(":").append(configParser_GetCfgItemStringValue(kszAccountPassword,
                                             knAccountId).c_str());
        }
        else
        {
            m_strUser = strXSIUserId;
            m_strPassword = configParser_GetCfgItemStringValue(kszAccountXSIPassword, knAccountId);
        }

        // 读取XSI信息
        yl::string strServer = configParser_GetCfgItemStringValue(kszAccountXSIHostAddr, knAccountId);

        m_strPort = configParser_GetCfgItemStringValue(kszAccountXSIPort, knAccountId);
        yl::string strServerType = configParser_GetCfgItemStringValue(kszAccountXSIServerType, knAccountId);
        yl::string strVirsion = configParser_GetConfigString(kszBsftVersion);
        if (strVirsion.empty())
        {
            strVirsion = "v2.0";
        }

        yl::string strCustomURL = configParser_GetCfgItemStringValue(kszAccountXSICustomURL, knAccountId);

        if (strCustomURL.empty())
        {
            strCustomURL = BRROADWORKS_DEFAULT_URL;
        }

        if (!strServer.empty()
                && !strXSIUserId.empty()
                && !strServerType.empty()
                && (!bIsAllSipAuth || !m_strUser.empty()))
        {

            //strServer = commonAPI_FormatString(BRROADWORKS_CONTACT_CMD_PREFEX, strServerType.c_str(), strServer.c_str(),m_strPort.c_str(),strVirsion.c_str(),strXSIUserId.c_str());
            strServer = commonAPI_FormatString(BRROADWORKS_CONTACT_NEW_CMD_PREFEX, strServerType.c_str(),
                                               strServer.c_str(), m_strPort.c_str(), strCustomURL.c_str(), strVirsion.c_str(),
                                               strXSIUserId.c_str());

            strServer.append("CallLogs/");
            // 创建组
            CreatConfigGroup(kszAllContactId, strServer, TRID_ALL_CALLLOG);
            CreatConfigGroup(kszPlacedLogId, strServer + "Placed", TRID_PLACED_CALL_LOG);
            CreatConfigGroup(kszMissLogId, strServer + "Missed", TRID_MISSED_CALLLOG);
            CreatConfigGroup(kszReceivedLogId, strServer + "Received", TRID_RECEIVED_CALL_LOG);

            return true;
        }
    }


    return false;
}

void CBroadSoftLogManager::CreatConfigGroup(int nGroupId, const yl::string & strServer,
        yl::string strName,
        int nAccountID/* = knAccountId*/, bool bAddToShowList /*= true*/)
{
    BSFTLogGroupRecord * pGroup = new BSFTLogGroupRecord();

    if (pGroup != NULL)
    {
        pGroup->m_strServerUrl = strServer;

        pGroup->m_strDisplayName = strName;

#if IF_FEATURE_MUTI_CALLLOG
        pGroup->m_nAccountID = nAccountID;
#endif

        // 初始化创建 非账号的组，类型根据组名判断
        if (nAccountID == knAccountId)
        {
            pGroup->m_eLogType = (CALL_TYPE)nGroupId;
        }


        DIR_INFO("Add Group: %d %s %s", nGroupId, pGroup->m_strDisplayName.c_str(),
                 pGroup->m_strServerUrl.c_str());

        InsertMap(pGroup, nGroupId);

        if (bAddToShowList)
        {
            AddIdToMenuShowList(nGroupId);
        }
    }
}

bool CBroadSoftLogManager::ReqLoadCallLog(int nGroupId)
{
    DIR_INFO("CBroadSoftLogManager::ReqLoadCallLog groupId[%d]", nGroupId);
    BSFTLogGroupRecord * pGroup = Base2BSFTLogGroup(GetDataById(nGroupId));

    if (pGroup == NULL)
    {
        return false;
    }

    if (pGroup->m_eStatus == FILE_DOWNLOADING)
    {
        return true;
    }

#if IF_FEATURE_MUTI_CALLLOG
    if (BsftLog_IsMutilLogEnable())
    {
        // 组切换时候需求清除上次的历史记录，不然切换组之后一直存在
        if (m_nLastReqGroupID > 0)
        {
            ClearBsftGroupLog(m_nLastReqGroupID);
        }
        m_nLastReqGroupID = nGroupId;
        ClearBsftGroupLog(nGroupId);
    }
    else
#endif
    {
        ClearAllContactFromMap();
        ReSetId(kszLogBeginId);
    }

    pGroup->m_eStatus = FILE_DOWNLOAD_READY;

    return MyExecDownload(nGroupId) == FILE_DOWNLOADING;
}


CALL_TYPE CBroadSoftLogManager::GetCallTypeByGroupID(int nGroupID)
{
    BSFTLogGroupRecord * pGroup = Base2BSFTLogGroup(GetDataById(nGroupID));
    if (pGroup != NULL)
    {
        return pGroup->m_eLogType;
    }

    return CT_UNKNOW;
}

bool CBroadSoftLogManager::MyExecDelete(int nId, BSFT_DELETE_TYPE eType)
{
    if (!BsftLog_LogDeleteEnable())
    {
        return false;
    }

    int nGroupId;

    BSFTLogGroupRecord * pGroup;

    yl::string strServerUrl;

    yl::string strLocalFile = "xxx";//不为空即可

    VectorId vecId;

    vecId.clear();

    if (eType == BSFT_DELETE_SINGLE)
    {
        BSFTCallLogRecord * pCallLog = Base2BSFTLog(GetDataById(nId));

        if (pCallLog == NULL)
        {
            DIR_INFO("CBroadSoftLogManager::MyExecDelete,nId = %d is not exist", nId);
            return false;
        }

        nGroupId = pCallLog->m_nParentId;

        pGroup = Base2BSFTLogGroup(GetDataById(nGroupId));

        if (pGroup == NULL)
        {
            return false;
        }

        strServerUrl = pGroup->m_strServerUrl;

        yl::string strCallLogId = pCallLog->m_strCallLogId;

        if (strCallLogId.empty())
        {
            return false;
        }

        DIR_INFO("CBroadSoftLogManager::MyExecDelete,m_strDisplayName = -%s- is not exist",
                 pGroup->m_strDisplayName.c_str());

        if (pGroup->m_strDisplayName.compare("All Calllog") == 0)
        {
            switch (pCallLog->m_eLogType)
            {
            case CT_OUTGOING:
                strServerUrl.append("Placed");
                break;
            case CT_INCOMING:
                strServerUrl.append("Received");
                break;
            case CT_MISSCALL:
                strServerUrl.append("Missed");
                break;
            default:
                return false;
            }
        }
        strServerUrl.append("/").append(strCallLogId);

        vecId.push_back(nId);

    }
    else if (eType == BSFT_DELETE_GROUP)
    {
        nGroupId = nId;

        pGroup = Base2BSFTLogGroup(GetDataById(nGroupId));

        if (pGroup == NULL)
        {
            DIR_INFO("CBroadSoftLogManager::MyExecDelete,nGroupId = %d is not exist", nGroupId);
            return false;
        }

        strServerUrl = pGroup->m_strServerUrl;
        vecId = pGroup->m_vChildrenId;
    }
    else
    {
        return false;
    }

    DIR_INFO("CBroadSoftLogManager::MyExecDelete,url = %s", strServerUrl.c_str());

    CNetworkFileAction * pAction = TaskAction_CreateNetworkFileAction(strLocalFile.c_str(),
                                   strServerUrl.c_str());

    if (pAction != NULL)
    {
        pAction->SetExtraParam(nGroupId);
        pAction->SetRead(false);        // post文件.
        pAction->SetDelete(true);       // Delete方法
        pAction->SetSync(false);        // 异步.

#if IF_FEATURE_MUTI_CALLLOG
        if (BsftLog_IsMutilLogEnable())
        {
            yl::string strUser;
            yl::string strPassword;
            GetUserNameAndPasswordByAccountID(strUser, strPassword, pGroup->m_nAccountID);
            pAction->SetUsername(strUser);
            pAction->SetPassword(strPassword);
        }
        else
#endif
        {
            pAction->SetUsername(m_strUser);
            pAction->SetPassword(m_strPassword);
        }
        pAction->SetCallback(this);  // 回调函数.
        bool bIsAllSipAuth = (configParser_GetConfigInt(kszAllowSipAuth) == 1);

        if (bIsAllSipAuth)
        {
            pAction->SetDownLoadMode(MODE_SIP_XSI_AUTH);
        }
        else if (CBaseHelper::IsXsiAvailable(pGroup->m_nAccountID < 0 ? 0 : pGroup->m_nAccountID))
        {
            pAction->SetDownLoadMode(MODE_BASIC_XSI_AUTH);
        }

        if (TaskAction_ExecAction(pAction) == true)
        {
            RemoveRecord(vecId, nGroupId);
            return true;
        }
    }
    return false;
}
DIRECTORY_RET_CODE CBroadSoftLogManager::RemoveGroupLog(int nGroupId)
{
    //先判断网络是否可用
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    return MyExecDelete(nGroupId, BSFT_DELETE_GROUP) ? DIR_RET_SUCC : DIR_RET_FAIL;
}

DIRECTORY_RET_CODE CBroadSoftLogManager::RemoveSingleLog(int nId)
{
    //先判断网络是否可用
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    return MyExecDelete(nId, BSFT_DELETE_SINGLE) ? DIR_RET_SUCC : DIR_RET_FAIL;
}
int CBroadSoftLogManager::MyExecDownload(int nGroupID)
{
    BSFTLogGroupRecord * pGroup = Base2BSFTLogGroup(GetDataById(nGroupID));

    // 只有状态为已准备, 才可以下载.
    if (pGroup != NULL
            && pGroup->m_eStatus == FILE_DOWNLOAD_READY)
    {
        // 生成本地保存路径.
        yl::string strFullPath(g_strTempPath);
        char buf[64] = {0};
        sprintf(buf, kszFileSavedName, nGroupID);
        strFullPath.append(buf);

        DIR_INFO("CBroadSoftLogManager::MyExecDownload strUrl %s", pGroup->m_strServerUrl.c_str());

        // 开始下载.
        CNetworkFileAction * pAction =
            TaskAction_CreateNetworkFileAction(strFullPath.c_str(), pGroup->m_strServerUrl.c_str());
        if (pAction != NULL)
        {
            // 设置组的下载状态为下载中
            pGroup->m_eStatus = FILE_DOWNLOADING;

            pAction->SetRead(true);  // 读取文件.
#if IF_FEATURE_MUTI_CALLLOG
            if (BsftLog_IsMutilLogEnable())
            {
                yl::string strUser;
                yl::string strPassword;
                GetUserNameAndPasswordByAccountID(strUser, strPassword, pGroup->m_nAccountID);
                pAction->SetUsername(strUser);
                pAction->SetPassword(strPassword);
            }
            else
#endif
            {
                pAction->SetUsername(m_strUser);
                pAction->SetPassword(m_strPassword);
            }
            pAction->SetExtraParam(nGroupID); // 保存GroupID，用于回调时找到对应的组
            pAction->SetCallback(this);  // 回调函数.
            bool bIsAllSipAuth = (configParser_GetConfigInt(kszAllowSipAuth) == 1);
            if (bIsAllSipAuth)
            {
                pAction->SetDownLoadMode(MODE_SIP_XSI_AUTH);
            }
            else if (CBaseHelper::IsXsiAvailable(pGroup->m_nAccountID < 0 ? 0 : pGroup->m_nAccountID))
            {
                pAction->SetDownLoadMode(MODE_BASIC_XSI_AUTH);
            }

            pAction->SetSync(false);  // 异步.
            return TaskAction_ExecAction(pAction)
                   ? FILE_DOWNLOADING : FILE_DOWNLOAD_FAIL;
        }
    }

    // 其他情况, 则下载失败.
    return FILE_DOWNLOAD_FAIL;
}

void CBroadSoftLogManager::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    if (pTaskAction == NULL)
    {
        return;
    }

    if (pTaskAction->GetReqType() != NS_TA::TA_REQUEST_NETWORK_FILE)
    {
        CBaseContactManger::OnTaskActionCallback(pTaskAction);
        return;
    }

    // 返回结果的类型为请求网络文件.
    CNetworkFileAction * pNetworkAction =
        dynamic_cast<CNetworkFileAction *>(pTaskAction);

    // 获取组节点
    int nGroupID = pNetworkAction->GetExtraParam();
    BSFTLogGroupRecord * pGroup = Base2BSFTLogGroup(GetDataById(nGroupID));


    if (pGroup == NULL || pGroup->m_eStatus != FILE_DOWNLOADING)
    {

        //删除内存中的文件
        remove(pNetworkAction->GetFilePath().c_str());
        return;
    }

    int result = 0;
    if (pNetworkAction->IsOperationSucceed()
            && ParseDownloadedFile(pNetworkAction->GetFilePath(), nGroupID))
    {
        result = 1;
    }
    else if (pNetworkAction->GetTaskActionErrorType() == NS_TA::TA_HTTP_DOWNLOAD_UNAUTHORIZED)
    {
        // 鉴权失败
        result = 401;
    }

#if IF_FEATURE_MUTI_CALLLOG
    if (BsftLog_IsMutilLogEnable())
    {
        //进入network log也需清空miss notif
        if (result == 1 && (pGroup->m_eLogType == CT_CALLALL || pGroup->m_eLogType == CT_MISSCALL))
        {
            g_pLocalLogManager->ClearLogs(kszMissNotifId);
        }
    }
    else
#endif
    {
        //进入network log也需清空miss notif
        if (result == 1 && (nGroupID == kszAllContactId || nGroupID == kszMissLogId))
        {
            g_pLocalLogManager->ClearLogs(kszMissNotifId);
        }
    }


    pGroup->m_eStatus = result == 1 ? FILE_DOWNLOADED_OK : FILE_DOWNLOAD_FAIL;

    int nAcountID = 0;
#if IF_FEATURE_MUTI_CALLLOG
    nAcountID = GetAccountIDByGroupID(nGroupID);
#endif
    CBaseHelper::ProcessCallback(pTaskAction, nAcountID, XSI_CALLLOD_SEND);

    // 发送消息.
    etl_NotifyApp(false, BROADSOFT_CALLLOG_DOWNLOADED_RESULT, nGroupID, result);

    //删除内存中的文件
    remove(pNetworkAction->GetFilePath().c_str());
}

/**
 * 解析下载的文件
*/
bool CBroadSoftLogManager::ParseDownloadedFile(const yl::string & strFilePath,
        int nParentID/* = 0*/)
{
    // 打开Xml文件.
    if (!pathFileExist(strFilePath.c_str()))
    {
        return false;
    }

    xml_document doc;
    OpenBSFTXML(strFilePath.c_str(), doc);

    xml_node nodeXML = doc.first_child();
    xml_node nodeRootNode = nodeXML.next_sibling();

    yl::string strNodeName = nodeRootNode.name();

    bool bRet = true;
#if IF_FEATURE_MUTI_CALLLOG
    if (BsftLog_IsMutilLogEnable())
    {
        CALL_TYPE eType = GetLogTypeByNodeName(strNodeName);
        int nGroupID = 0;
        if (eType == CT_CALLALL)
        {
            for (nodeRootNode = nodeRootNode.first_child(); nodeRootNode;
                    nodeRootNode = nodeRootNode.next_sibling())
            {
                strNodeName = nodeRootNode.name();

                eType = GetLogTypeByNodeName(strNodeName);
                if (eType != CT_CALLALL
                        && eType != CT_UNKNOW)
                {
                    nGroupID = GetGroupIdByLogType(CT_CALLALL, nParentID);

                    // 解析具体类型的记录,all组需要本地排序
                    bRet &= ParseXmlData(nodeRootNode, nParentID, eType, true);
                }
            }
        }
        else if (eType != CT_UNKNOW)
        {
            nGroupID = GetGroupIdByLogType(eType, nParentID);
            // 解析具体类型的记录
            bRet = ParseXmlData(nodeRootNode, nParentID, eType);
        }
        else
        {
            return false;
        }
    }
    else
#endif
    {
        // 判断是哪类型的记录
        int nGroupId = GetGroupIdByNodeName(strNodeName);
        if (nGroupId == kszAllContactId)
        {
            //是所有类型的记录
            for (nodeRootNode = nodeRootNode.first_child(); nodeRootNode;
                    nodeRootNode = nodeRootNode.next_sibling())
            {
                strNodeName = nodeRootNode.name();

                nGroupId = GetGroupIdByNodeName(strNodeName);
                if (nGroupId != kszAllContactId
                        && nGroupId != -1)
                {
                    CALL_TYPE eType = GetLogTypeByGroupId(nGroupId);

                    // 解析具体类型的记录
                    bRet &= ParseXmlData(nodeRootNode, kszAllContactId, eType);
                }
            }
        }
        else if (nGroupId != -1)
        {
            CALL_TYPE eType = GetLogTypeByGroupId(nGroupId);

            // 解析具体类型的记录
            bRet = ParseXmlData(nodeRootNode, nGroupId, eType);
        }
        else
        {
            //解析错误
            return false;
        }
    }

    return bRet;
}

int CBroadSoftLogManager::GetGroupIdByNodeName(const yl::string & strName)
{
    int nId = -1;
    if (strncmp(strName.c_str(), kszCallLogs, 8) == 0)
    {
        nId = kszAllContactId;
    }
    if (strncmp(strName.c_str(), kszPlaced, 6) == 0)
    {
        nId = kszPlacedLogId;
    }
    else if (strncmp(strName.c_str(), kszMissed, 6) == 0)
    {
        nId = kszMissLogId;
    }
    else if (strncmp(strName.c_str(), kszReceived, 8) == 0)
    {
        nId = kszReceivedLogId;
    }

    return nId;
}

// 解析组
bool CBroadSoftLogManager::ParseXmlData(const xml_node & refNode, int nGroupId, CALL_TYPE eLogType,
                                        bool bNeedLocalSort /*= false*/)
{
#if IF_FEATURE_MUTI_CALLLOG
    int nAccountID = GetAccountIDByGroupID(nGroupId);
#endif
    // 读取各个节点的值.
    yl::string strNodeName;
    yl::string strNodeContent;
    for (xml_node node = refNode.child(kszBSFTLogNodeEntry);
            node;
            node = node.next_sibling(kszBSFTLogNodeEntry))
    {
        BSFTCallLogRecord * pNew = new BSFTCallLogRecord();
        if (pNew != NULL)
        {
            //解析联系人
            pNew->ParseFromXMLNode(node);
            pNew->m_eLogType = eLogType;
            pNew->m_nParentId = nGroupId;
#if IF_FEATURE_MUTI_CALLLOG
            pNew->m_nLineID = nAccountID;
            //插入
            AddContact(pNew, nGroupId, nGroupId == kszAllContactId || bNeedLocalSort, false);
#else
            //插入
            AddContact(pNew, nGroupId, nGroupId == kszAllContactId, false);
#endif
        }
    }
    return true;
}

bool CBroadSoftLogManager::IsNeedShowAllGroup()
{
    return false;
}

const yl::string & CBroadSoftLogManager::GetSortKeyById(int nId)
{
    BSFTCallLogRecord * pLog = Base2BSFTLog(GetDataById(nId));

    if (pLog != NULL)
    {
        return pLog->m_strBornTick;
    }

    return kszEmptyString;
}

#if IF_FEATURE_MUTI_CALLLOG
bool CBroadSoftLogManager::CreatChildGroup(int nParentID)
{
    CreatGroupByParentID(nParentID, CT_CALLALL);
    CreatGroupByParentID(nParentID, CT_OUTGOING);
    CreatGroupByParentID(nParentID, CT_MISSCALL);
    CreatGroupByParentID(nParentID, CT_INCOMING);

    return true;
}

bool CBroadSoftLogManager::CreatGroupByParentID(int nParentID, CALL_TYPE eLogType)
{
    BSFTLogGroupRecord * pParentGroup = Base2BSFTLogGroup(GetDataById(nParentID));
    if (pParentGroup == NULL)
    {
        return false;
    }

    BSFTLogGroupRecord * pGroup = new BSFTLogGroupRecord();
    if (pGroup != NULL)
    {
        pGroup->m_nParentId = nParentID;
        pGroup->m_eLogType = eLogType;
        pGroup->m_nAccountID = pParentGroup->m_nAccountID;

        yl::string strServer = pParentGroup->m_strServerUrl;
        switch (eLogType)
        {
        case CT_CALLALL:
            {
                pGroup->m_strDisplayName = TRID_ALL_CALLLOG;
            }
            break;
        case CT_OUTGOING:
            {
                pGroup->m_strDisplayName = TRID_PLACED_CALL_LOG;
                strServer += "Placed";
            }
            break;
        case CT_MISSCALL:
            {
                pGroup->m_strDisplayName = TRID_MISSED_CALLLOG;
                strServer += "Missed";
            }
            break;
        case CT_INCOMING:
            {
                pGroup->m_strDisplayName = TRID_RECEIVED_CALL_LOG;
                strServer += "Received";
            }
            break;
        default:
            break;
        }
        pGroup->m_strServerUrl = strServer;

        //账号节点下的ID也预先创建好，避免被清空
        //
        int nChildID = (nParentID - 1) * 4 + (int)eLogType + (acc_AccountNum() + kszBsftLogRootGroupFirstId)
                       + 1;
        int nID = InsertMap(pGroup, nChildID);
        AddIdToGroup(nID, nParentID, false);
        return true;
    }
    return false;
}

CALL_TYPE CBroadSoftLogManager::GetLogTypeByNodeName(const yl::string & strName)
{
    CALL_TYPE eType = CT_UNKNOW;
    if (strncmp(strName.c_str(), kszCallLogs, 8) == 0)
    {
        eType = CT_CALLALL;
    }
    else if (strncmp(strName.c_str(), kszPlaced, 6) == 0)
    {
        eType = CT_OUTGOING;
    }
    else if (strncmp(strName.c_str(), kszMissed, 6) == 0)
    {
        eType = CT_MISSCALL;
    }
    else if (strncmp(strName.c_str(), kszReceived, 8) == 0)
    {
        eType = CT_INCOMING;
    }

    return eType;
}

int CBroadSoftLogManager::GetGroupIdByLogType(CALL_TYPE eLogType, int nParentID)
{
    DIR_INFO("CBroadSoftLogManager::GetGroupIdByLogType eType [%d] nParentId [%d]", eLogType,
             nParentID);
    BSFTLogGroupRecord * pParentGroup = Base2BSFTLogGroup(GetDataById(nParentID));
    if (pParentGroup == NULL)
    {
        DIR_INFO("ParentId Is Null");
        return -1;
    }

    for (int i = 0; i < pParentGroup->m_vChildrenId.size(); ++i)
    {
        BSFTLogGroupRecord * pGroup = Base2BSFTLogGroup(GetDataById(pParentGroup->m_vChildrenId[i]));
        if (pGroup != NULL && pGroup->m_eLogType == eLogType)
        {
            return pParentGroup->m_vChildrenId[i];
        }
    }

    return -1;
}

int CBroadSoftLogManager::ChangeToActualGroup(int nGroupID, CALL_TYPE eType /*= CT_CALLALL*/)
{
    if (IsAccountGroup(nGroupID))
    {
        return GetGroupIdByLogType(eType, nGroupID);
    }
    return nGroupID;
}

int CBroadSoftLogManager::GetGroupIDByAccountID(int nAccountID, CALL_TYPE eType /*= CT_CALLALL*/)
{
    return GetGroupIdByLogType(eType, GetAccGroupIDByAccountID(nAccountID));
}

void CBroadSoftLogManager::ClearBsftGroupLog(int nGroupId)
{
    VectorId vecId;

    BSFTLogGroupRecord * pGroup;

    pGroup = Base2BSFTLogGroup(GetDataById(nGroupId));

    if (pGroup == NULL)
    {
        DIR_INFO("CBroadSoftLogManager::ClearBsftCallLog,nGroupId = %d is not exist", nGroupId);
        return ;
    }

    vecId = pGroup->m_vChildrenId;

    DIR_INFO("-----------------Remove Size [%d][%d]",
             vecId.size(), pGroup->m_vChildrenId.size());

    RemoveRecord(vecId, nGroupId);

    ReSetId(kszLogBeginId);

}

void CBroadSoftLogManager::ClearBsftCallLog()
{
    int AccountNum = acc_AccountNum();

    int nGroupId;

    for (int i = 0; i < AccountNum; i++)
    {
        for (int j = 0; j < 4; j++)//CT_INCOMING; CT_MISSCALL; CT_OUTGOING; CT_CALLALL四种
        {
            nGroupId = i * 4 + j + (AccountNum + kszBsftLogRootGroupFirstId) + 1;

            ClearBsftGroupLog(nGroupId);
        }

    }

}

// 查找手柄对应的显示组ID列表
bool CBroadSoftLogManager::GetConfigMenuIdList(VectorId & output)
{
    if (BsftLog_IsMutilLogEnable())
    {
        m_vGroupId.clear();
        for (int i = 0; i < acc_AccountNum(); ++i)
        {
            if (acc_IsAccountAvailable(i))
            {
                AddIdToMenuShowList(GetAccGroupIDByAccountID(i));
            }
        }
    }

    return CBaseContactManger::GetConfigMenuIdList(output);
}

yl::string CBroadSoftLogManager::GetServerUrlByAccountID(int nAccountID)
{
    bool bIsAllSipAuth = (configParser_GetConfigInt(kszAllowSipAuth) == 1);

    yl::string strXSIUserId = configParser_GetCfgItemStringValue(kszAccountXSIUserID, nAccountID);

    // 读取XSI信息
    yl::string strServer = configParser_GetCfgItemStringValue(kszAccountXSIHostAddr, nAccountID);

    yl::string strPort = configParser_GetCfgItemStringValue(kszAccountXSIPort, nAccountID);
    yl::string strServerType = configParser_GetCfgItemStringValue(kszAccountXSIServerType, nAccountID);
    yl::string strVirsion = configParser_GetConfigString(kszBsftVersion);
    if (strVirsion.empty())
    {
        strVirsion = "v2.0";
    }

    yl::string strCustomURL = configParser_GetCfgItemStringValue(kszAccountXSICustomURL, nAccountID);

    if (strCustomURL.empty())
    {
        strCustomURL = BRROADWORKS_DEFAULT_URL;
    }

    if (!strServer.empty()
            && !strXSIUserId.empty()
            && !strServerType.empty())
    {
        strServer = commonAPI_FormatString(BRROADWORKS_CONTACT_NEW_CMD_PREFEX, strServerType.c_str(),
                                           strServer.c_str(), strPort.c_str(), strCustomURL.c_str(), strVirsion.c_str(), strXSIUserId.c_str());
        strServer.append("CallLogs/");
        return strServer;
    }
    return "";
}

int CBroadSoftLogManager::GetAccGroupIDByAccountID(int nAccountID)
{
    return nAccountID + kszBsftLogRootGroupFirstId;
}

int CBroadSoftLogManager::GetAccountIDByGroupID(int nGroupID)
{
    if (!BsftLog_IsMutilLogEnable())
    {
        return knAccountId;
    }

    if (nGroupID > 0)
    {
        BSFTLogGroupRecord * pGroup = Base2BSFTLogGroup(GetDataById(nGroupID));
        if (pGroup != NULL)
        {
            return pGroup->m_nAccountID;
        }
    }

    return -1;
}

bool CBroadSoftLogManager::GetUserNameAndPasswordByAccountID(yl::string & strUserName,
        yl::string & strPassword, int nAccountID /*= knAccountId*/)
{
    bool bIsAllSipAuth = (configParser_GetConfigInt(kszAllowSipAuth) == 1);

    yl::string strXSIUserId = configParser_GetCfgItemStringValue(kszAccountXSIUserID, nAccountID);
    if (bIsAllSipAuth)
    {
        //userId为sip UserName  passwordc存的是userID：sip passward
        strUserName = configParser_GetCfgItemStringValue(kszAccountRegisterName, nAccountID);
        strPassword = strXSIUserId;
        strPassword.append(":").append(configParser_GetCfgItemStringValue(kszAccountPassword,
                                       nAccountID).c_str());
    }
    else
    {
        strUserName = strXSIUserId;
        strPassword = configParser_GetCfgItemStringValue(kszAccountXSIPassword, nAccountID);
    }

    return true;
}

bool CBroadSoftLogManager::IsAccountGroup(int nGroupID)
{
    if (nGroupID >= kszBsftLogRootGroupFirstId
            && nGroupID <= kszBsftLogRootGroupFirstId + acc_AccountNum())
    {
        return true;
    }
    return false;
}

/*int CBroadSoftLogManager::GetAllGroupIDByRootID(VectorId& vGroupID, int iRootID)
{
    if (BsftLog_IsMutilLogEnable())
    {
        m_vGroupId.clear();
        for (int i=0; i<acc_AccountNum(); ++i)
        {
            if (acc_IsAccountAvailable(i))
            {
                //AddIdToMenuShowList(GetIdbyAccountAndType(i, kszAllContactId));
                AddIdToMenuShowList(GetAccGroupIDByAccountID(i));
            }
        }
    }

    return CBaseContactManger::GetAllGroupIDByRootID(vGroupID, iRootID);
}*/

bool CBroadSoftLogManager::IsDirAvailable()
{
    if (BsftLog_IsMutilLogEnable())
    {
        return true;
    }
    return CBaseContactManger::IsDirAvailable();
}
#endif
