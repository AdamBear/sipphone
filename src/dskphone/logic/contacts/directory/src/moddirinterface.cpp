#include "moddirinterface.h"
#include "basecontactmanager.h"
#include "localcontactmanager.h"
#include "locallogmanager.h"
#include "broadsoftcontactmanager.h"
#include "broadsoftlogmanager.h"
#include "remotecontactmanager.h"
#include "bluetoothcontactmanager.h"
#include "cloudcontactmanager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "contactmsghandler.h"
#include "interfacedefine.h"
#include "directoryshortcut.h"

#include "configiddefine.h"
#include "modconfigparser.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "basemethod.h"
#include "voip/modvoip.h"
#include "presence/include/modpresence.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "extended/bluetoothmessagedefine.h"
#include "plcmcontactmanager.h"
#if IF_FEATURE_GBBOOK
#include "genbandcontactmanager.h"
#endif
#include "service_system.h"

#include "dirlogicsync.h"

#if IF_FEATURE_GBBOOK
#include "service_sip_interface.h"
#endif

#if IF_FEATURE_METASWITCH
#include "metaswitch/mtswcontrol/include/mtsw_messagedefine.h"
#endif

#if IF_FEATURE_METASWITCH_CALLLOG
#include "mtswlogmanager.h"
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
#include "metaswitchcontactmanager.h"
#endif

#include "contacts/ldaplogic/src/ldapmanager.h"
#include "taskaction/modtaskaction.h"

// 配置文件路径, 由外部指定
yl::string g_strConfigPath;

// 临时文件路径
yl::string g_strTempPath;

// 定时器
LRESULT Dir_OnTimer(msgObject & refObj)
{
    bool bInTalk = talklogic_SessionExist();

    bool bRet = g_pLocalLogManager->OnTimer(refObj.wParam);

    //bRet |= g_pLocalDirManager->OnTimer(refObj.wParam);

    bRet |= g_pRemoteDirManager->OnTimeout(refObj.wParam, bInTalk);

    bRet |= g_pBroadSoftDirManager->OnTimeout(refObj.wParam, bInTalk);

#if IF_BUG_PLCMDIR
    bRet |= g_pPlcmDirManager->OnTimeout(refObj.wParam, bInTalk);
#endif

#ifdef IF_FEATURE_CLOUD
    bRet |= g_pCloudDirManager->OnTimeout(refObj.wParam, bInTalk);
#endif //#ifdef IF_FEATURE_CLOUD

#if IF_FEATURE_GBBOOK
    bRet |= g_pGenBandDirManager->OnTimeout(refObj.wParam, bInTalk);
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
    bRet |= g_pMetaSwitchDirManager->OnTimeout(refObj.wParam, bInTalk);
#endif

    return bRet;
}

bool Dir_Init(const yl::string & strConfigFile, const yl::string & strTmpFile)
{
#ifndef WIN32
    //http://10.2.1.199/Bug.php?BugID=87368
    signal(SIGPIPE, SIG_IGN);
#endif

    if (strConfigFile.empty() || strTmpFile.empty())
    {
        return false;
    }

    g_strConfigPath = strConfigFile;

    g_strTempPath = strTmpFile;

    //打印版本号，有提交版本需要更改版本号
    //DIR_INFO("Direcotry Version: %s", DIRECTORY_VERSION);

    // 注册网页消息.
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          NS_DIRECTORY::OnDirectoryMessage);

    etl_RegisterMsgHandle(DSK_MSG_SAVE_CALLLOG, DSK_MSG_SAVE_CALLLOG, NS_DIRECTORY::OnDirectoryMessage);
    //查询是否有历史记录
    etl_RegisterMsgHandle(DSK_MSG_GET_CONTACTCOUNT, DSK_MSG_GET_CONTACTCOUNT,
                          NS_DIRECTORY::OnDirectoryMessage);
    //删除历史记录
    etl_RegisterMsgHandle(DSK_MSG_DEl_LOCALLOG, DSK_MSG_DEl_LOCALLOG, NS_DIRECTORY::OnDirectoryMessage);

    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, Dir_OnTimer);

#ifdef IF_BT_SUPPORT_PHONE
    etl_RegisterMsgHandle(BLUETOOTH_MSG_CONNECT_STATE, BLUETOOTH_MSG_CONNECT_STATE,
                          NS_DIRECTORY::OnDirectoryMessage);
    etl_RegisterMsgHandle(BLUETOOTH_MSG_DIR_DONE, BLUETOOTH_MSG_DIR_DONE,
                          NS_DIRECTORY::OnDirectoryMessage);
#endif //#ifdef IF_BT_SUPPORT_PHONE


    etl_RegisterMsgHandle(DSK_MSG_GET_CONTACT_UPDATESTATS, DSK_MSG_GET_CONTACT_UPDATESTATS,
                          NS_DIRECTORY::OnDirectoryMessage);

    //监视网络状态改变消息
    etl_RegisterMsgHandle(SYS_MSG_NOTIFY_PHY_STATUS, SYS_MSG_NOTIFY_NETWORK_STATUS,
                          NS_DIRECTORY::OnDirectoryMessage);

    etl_RegisterMsgHandle(ATP_MSG_NOTIFY_AUTOP_END, ATP_MSG_NOTIFY_AUTOP_END,
                          NS_DIRECTORY::OnDirectoryMessage);

#if IF_FEATURE_GBBOOK
    etl_RegisterMsgHandle(SIP_SUB_ADDRESS_BOOK_NOTIFY, SIP_SUB_ADDRESS_BOOK_NOTIFY,
                          NS_DIRECTORY::OnDirectoryMessage);
#endif

#if IF_FEATURE_METASWITCH
    etl_RegisterMsgHandle(MTSW_MESSAGE_LOGIN, MTSW_MESSAGE_AUTH_ERR, NS_DIRECTORY::OnDirectoryMessage);
#endif

    // 初始化本地联系人搜索线程
    TaskAction_CreateSpecialThread(TA_REQUEST_LOCAL_CONTACT_SEARCH,
                                   TA_THREAD_LOCAL_SEARCH, NULL);

    g_pLocalDirManager->Init();

    g_pLocalLogManager->Init();

    g_pDirLogicSync->Init();

    // 初始化 BSFT 联系人 需要默认下载一次
    g_pBroadSoftDirManager->PreLoad();

    g_pBroadSoftLogManager;

    g_pRemoteDirManager->Init();

    g_pDirectoryShortcut->Init();

#if IF_BUG_PLCMDIR
    g_pPlcmDirManager->Init();
#endif //IF_BUG_PLCMDIR

#ifdef IF_FEATURE_CLOUD
    g_pCloudDirManager->Init();
#endif //#ifdef IF_FEATURE_CLOUD

#if IF_FEATURE_GBBOOK
    g_pGenBandDirManager->Init();
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
    g_pMetaSwitchDirManager->PreLoad();
#endif

#if IF_FEATURE_METASWITCH_CALLLOG
    g_pMTSWLogManager;
#endif
    return true;
}

bool Dir_GetShortcutList(DirShortcutListData * pData)
{
    return g_pDirectoryShortcut->GetShortcutList(pData);
}

CBaseContactManger * GetInstaceByType(DIR_TYPE eType)
{
    switch (eType)
    {
    case LOCAL_DIR:
        return g_pLocalDirManager;
    case LOCAL_LOG:
        return g_pLocalLogManager;
    case  REMOTE_DIR:
        return g_pRemoteDirManager;
    case BSFT_DIR:
        return g_pBroadSoftDirManager;
    case BSFT_LOG:
        return g_pBroadSoftLogManager;
#ifdef IF_BT_SUPPORT_PHONE
    case BLUETOOTH_DIR:
        return g_pBlueToothDirManager;
#endif
#if IF_BUG_PLCMDIR
    case PLCM_DIR:
        return g_pPlcmDirManager;
#endif // IF_BUG_PLCMDIR

#ifdef IF_FEATURE_CLOUD
    case CLOUD_DIR:
        return g_pCloudDirManager;
#endif //#ifdef IF_FEATURE_CLOUD
#if IF_FEATURE_GBBOOK
    case GB_DIR:
        return g_pGenBandDirManager;
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    case MTSW_LOG:
        return g_pMTSWLogManager;
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    case MTSW_DIR:
        return g_pMetaSwitchDirManager;
#endif
    case LDAP_DIR:
        return &g_refLdapManager;
    default:
        break;
    }

    return NULL;
}

bool Dir_GetContactListByGroupId(ContactListData * pData, const int nGroupId, DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->GetContactListByGroupId(pData, nGroupId);
    }

    return false;
}

#if IF_FEATURE_MUTI_CALLLOG
bool Dir_GetChildGroupByGroupID(YLVector<int> & output, const int nGroupId, DIR_TYPE eType)
{
    DIR_INFO("Dir_GetChildGroupByGroupID nGroupId [%d]", nGroupId);
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst == NULL)
    {
        DIR_INFO("Inst is Null");
        return false;
    }

    if (pInst->GetFirstChildTypeByGroupId(nGroupId) != DIR_INHERIT_GROUP)
    {
        DIR_INFO("Dir_GetChildGroupByGroupID Is Not Group");
        return false;
    }

    ContactListData sData;

    bool bRet = pInst->GetContactListByGroupId(&sData, nGroupId);

    if (sData.m_listId.size() > 0)
    {
        output = sData.m_listId;
    }

    return bRet;
}
#endif

bool Dir_LocalSearch(const yl::string & strSearchKey, const int nGroupId, DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        // 按手柄进行搜索
        return pInst->LocalSearch(strSearchKey, nGroupId);
    }

    return false;
}

bool Dir_GetSearchResultList(const yl::string & strSearchKey, ContactListData * pData,
                             DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->GetSearchResultList(strSearchKey, pData);
    }

    return false;
}

SmartPtr Dir_GetDetailById(int nId, DIR_TYPE eType)
{
#if DEBUG_CONTACT
    DIR_WARN("dir manager[%d]", eType);
#endif // DEBUG_CONTACT

    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->GetDetailById(nId);
    }
#if DEBUG_CONTACT
    DIR_WARN("No find dir manager[%d]", eType);
#endif // DEBUG_CONTACT

    return SmartPtr();
}

bool Dir_GetSuperDataById(const SuperDirData & refInIterm, DirectorySearchData & refData,
                          DIR_TYPE eType,
                          bool bGetPhoto/* = true */)
{
#if FEATURE_BROADTOUCH_UC
    if (eType == UC_DIR)
    {
        return Presence_GetSuperDataById(refInIterm, refData);
    }
#endif

    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->GetSuperDataById(refInIterm, refData, bGetPhoto);
    }

    return false;

}

bool Dir_IsSearchHit(int nId, const yl::string & strSearchKey, SearchMethod searchMethod,
                     DIR_TYPE eType,
                     StringMatchType eMatchType)
{
#if FEATURE_BROADTOUCH_UC
    if (eType == UC_DIR)
    {
        return Presence_IsSearchHit(nId, strSearchKey);
    }
#endif

    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->IsSearchHit(nId, strSearchKey, searchMethod, eMatchType);
    }

    return false;
}

int Dir_GetParentIdById(int nId, DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->GetParentIdById(nId);
    }
    return -1;
}


bool Dir_GetConfigMenuNameList(YLList<yl::string> & refData, DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->GetConfigMenuNameList(refData);
    }

    return false;
}

bool Dir_GetConfigMenuIdList(YLVector<int> & output, DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->GetConfigMenuIdList(output);
    }

    return false;
}

bool Dir_GetCotactNameByNumber(const yl::string & strNumber, yl::string & strName, bool bSync /* = true*/)
{
    if (strNumber.empty())
    {
        return false;
    }

    strName.clear();

    Dir_GetCotactNameByNumber(strNumber, strName, BLUETOOTH_DIR, bSync);

    // 到本地地址簿查找
    if (strName.empty())
    {
        Dir_GetCotactNameByNumber(strNumber, strName, LOCAL_DIR, bSync);
    }


#ifdef IF_FEATURE_CLOUD
    if (strName.empty()
            && acc_IsCloudEnable())
    {
        Dir_GetCotactNameByNumber(strNumber, strName, CLOUD_DIR);
    }
#endif //#ifdef IF_FEATURE_CLOUD


#if IF_FEATURE_METASWITCH_DIRECTORY
    if (strName.empty()
            && configParser_GetConfigInt(kszMTSWDirEnable) != 0
            && configParser_GetConfigInt(kszMetaSwitchActive) != 0)
    {
        Dir_GetCotactNameByNumber(strNumber, strName, MTSW_DIR, bSync);
    }
#endif

    // 到远程地址簿查找
    if (strName.empty()
            && configParser_GetConfigInt(kszSearchNameFromRemoteSwitch) != 0)
    {
        Dir_GetCotactNameByNumber(strNumber, strName, REMOTE_DIR, bSync); // 联系人模块提供接口
    }

    if (strName.empty()
            && BsftDir_IsEnable())
    {
        //从BroadSoft联系人中匹配
        Dir_GetCotactNameByNumber(strNumber, strName, BSFT_DIR, bSync);
    }

#if FEATURE_BROADTOUCH_UC
    if (strName.empty()
            && configParser_GetConfigInt(kszEnableUC) == 1
            && configParser_GetConfigInt(kszBroadsoftActive) == 1)
    {
        Presence_GetNameByNumber(strName, strNumber, PRESENCE_UC, bSync);
    }
#endif

#if IF_BUG_PLCMDIR
    if (strName.empty()
            && configParser_GetConfigInt(kszPlcmPhoneEnable) != 0)
    {
        Dir_GetCotactNameByNumber(strNumber, strName, PLCM_DIR, bSync);
    }
#endif

#if IF_FEATURE_GBBOOK
    if (strName.empty()
            && configParser_GetConfigInt(kszGbPabBookEnable) != 0
            && configParser_GetConfigInt(kszGbSoupEnable) == 1
            && configParser_GetConfigInt(kszGbPabMatchCalling) == 1)
    {
        Dir_GetCotactNameByNumber(strNumber, strName, GB_DIR, bSync);
    }
#endif

    return !strName.empty();
}


bool Dir_GetCotactNameByNumber(const yl::string & strNumber, yl::string & strName, DIR_TYPE eType, bool bSync /* = true*/)
{
    if (strNumber.empty())
    {
        return false;
    }

    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->GetContactNameByNumber(strNumber, strName, bSync);
    }

    return false;
}

//Added By Yuyt! 2014/3/19
int Dir_GetContactIdByIndex(int index, int nGroup, DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->GetContactIdByIndex(index, nGroup);
    }
    return -1;
}

int Dir_GetContactIndexById(int nId, int nGroup, DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);
    if (pInst != NULL)
    {
        return pInst->GetContactIndexById(nId, nGroup);
    }
    return -1;
}


bool Dir_IsDirConfiged(DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->IsDirAvailable();
    }

    return false;
}

const yl::string & Dir_GetDisplayNameById(int nId, DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->GetDisplayNameById(nId);
    }

    return kszEmptyString;
}

int Dir_GetContactCount(int nGroupId, DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->GetContactCount(nGroupId);
    }

    return -1;
}

void Dir_CancelLoad(int nGroupId, DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        pInst->CancelLoad(nGroupId);
    }
}

bool Dir_SuperSearch(const yl::string & strKey, YLVector<SuperDirData> & output, DIR_TYPE eType,
                     StringMatchType eMatchType)
{
#if FEATURE_BROADTOUCH_UC
    if (eType == UC_DIR)
    {
        return Presence_SuperSearch(strKey, output);
    }
#endif

    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->SuperSearch(strKey, output, eMatchType);
    }

    return false;
}

StringMatchType Dir_GetSearchMatchType()
{
    if (1 == configParser_GetConfigInt(kszDirSearchMatchtype))
    {
        return LEAD_MATCH;
    }
    return CONTAIN_MATCH;
}

FileDownloadStatus Dir_GetDownLoadStatus(int nGroupId, DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->GetDownLoadStatus(nGroupId);
    }

    return FILE_DOWNLOAD_STATUS_NONE;
}

int Dir_IsContactNameExist(const yl::string & strName, DIR_TYPE eType)
{
    if (eType == LOCAL_DIR)
    {
        return g_pLocalDirManager->IsContactNameExist(strName, kszAllContactId);
    }
    else if (eType == BSFT_DIR)
    {
        ContactBroadSoftMenu * pMenu = Base2BsftMenu(g_pBroadSoftDirManager->GetMenuDataById(
                                           kszPersonalId));

        if (pMenu != NULL)
        {
            return pMenu->IsContactNameExist(strName);
        }
    }
#if IF_FEATURE_GBBOOK
    else if (eType == GB_DIR)
    {
        return g_pGenBandDirManager->IsContactNameExist(strName);
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (eType == MTSW_DIR)
    {
        return g_pMetaSwitchDirManager->IsContactNameExist(strName, kszAllContactId);
    }
#endif

    return -1;
}

bool Dir_IsAllLoadFinish(int nGroupId, DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->IsAllLoadFinish(nGroupId);
    }

    return true;
}

bool Dir_ClearContactData(int nGroupId, DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->ClearContactByGroupID(nGroupId, true);
    }

    return false;
}

bool Dir_GetFastIndexListByGroupId(int nGroupId, FastIndexList & refList, DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->GetFastIndexListByGroupId(nGroupId, refList);
    }

    return true;
}

DIR_INHERIT_TYPE Dir_GetFirstChildTypeByGroupId(int nGroupId, DIR_TYPE eType)
{
    CBaseContactManger * pInst = GetInstaceByType(eType);

    if (pInst != NULL)
    {
        return pInst->GetFirstChildTypeByGroupId(nGroupId);
    }

    return DIR_INHERIT_BASE;
}

int LocalLog_AddLog(const LocalLogItemRecord & refData)
{
    return g_pLocalLogManager->AddLog(refData);
}

bool LocalLog_RemoveLog(VectorId & vecId, int nGroupId)
{
    return g_pLocalLogManager->RemoveLog(vecId, nGroupId);
}

bool LocalLog_ClearLogs(int nGroupId, int nAccountID/* = -1*/)
{
    return g_pLocalLogManager->ClearLogs(nGroupId, nAccountID);
}

bool LocalLog_GetLastOutCallNumAndAccountID(yl::string & strNumber, int & nAccountId)
{
    return g_pLocalLogManager->GetLastOutCallNumAndAccountID(strNumber, nAccountId);
}

bool LocalLog_SearchCallLogRecentOutCall(SuperSearchItemListData * pData)
{
    return g_pLocalLogManager->SearchCallLogRecentOutCall(pData);
}

void LocalLog_Reload()
{
    return g_pLocalLogManager->Reload();
}


bool LocalLog_DetailedEnable()
{
    return configParser_GetConfigInt(kszDetailInfoEnable) == 1;
}

bool LocalLog_MergeLogEnable()
{
    return configParser_GetConfigInt(kszCalllogMergeEnable) == 1;
}

int LocalDir_AddGroup(const ContactGroupData & groupData)
{
    return g_pLocalDirManager->AddGroup(groupData);
}

int LocalDir_UpdateGroup(int groupID, const ContactGroupData & groupData)
{
    return g_pLocalDirManager->UpdateGroup(groupID, groupData);
}

bool LocalDir_RemoveGroup(int groupID)
{
    return g_pLocalDirManager->RemoveGroup(groupID);
}

void LocalDir_ClearGroup()
{
    g_pLocalDirManager->ClearGroup();
}

int LocalDir_AddLocalContact(const ContactGeneralItemData & refContactData, int nGroupId)
{
    return g_pLocalDirManager->AddLocalContact(refContactData, nGroupId);
}

int LocalDir_AddLocalContactList(YLList<ContactGeneralItemData> & refList, int nGroupId)
{
    return g_pLocalDirManager->AddLocalContactList(refList, nGroupId);
}

bool LocalDir_RemoveContacts(const VectorId & vecRemoveId, int nParentId)
{
    return g_pLocalDirManager->RemoveContacts(vecRemoveId, nParentId);
}

int LocalDir_UpdateContact(const ContactGeneralItemData & refContactData, int nGroupId, int nId)
{
    return g_pLocalDirManager->UpdateContact(refContactData, nGroupId, nId);
}

void LocalDir_ClearContact(int nGroupId)
{
    return g_pLocalDirManager->ClearContact(nGroupId);
}

bool LocalDir_GetDivertNumberByNumber(const yl::string & strNum, yl::string & strDivertNum)
{
    return g_pLocalDirManager->GetDivertNumberByNumber(strNum, strDivertNum);
}

bool LocalDir_IsBlackContactNumber(const yl::string & strNum)
{
    return g_pLocalDirManager->IsBlackContactNumber(strNum);
}

const yl::string & LocalDir_GetRingByNumber(const yl::string & strNum)
{
    return g_pLocalDirManager->GetRingByNumber(strNum);
}

DIRECTORY_RET_CODE LocalDir_MoveToGroup(YLVector<int> & vecContactID, int nGroupID,
                                        int nGroupFromId)
{
    return g_pLocalDirManager->MoveToGroup(vecContactID, nGroupID, nGroupFromId);
}

const yl::string & LocalDir_GetContactImageByNumber(const yl::string & strNumber,
        const yl::string & strName/* = ""*/)
{
    return g_pLocalDirManager->GetContactImageByNumber(strNumber, strName);
}

const yl::string & BTDir_GetContactImageByNumber(const yl::string & strNumber,
        const yl::string & strName/* = ""*/)
{
#ifdef IF_BT_SUPPORT_PHONE
    return g_pBlueToothDirManager->GetContactImageByNumber(strNumber, strName);
#else
    return kszEmptyString;
#endif
}

#if IF_FEATURE_FAVORITE_DIR
bool LocalDir_IsFavoriteEnable()
{
    return g_pLocalDirManager->IsFavoriteEnable();
}

bool LocalDir_IsContactInFavorite(const int & nContactId)
{
    return g_pLocalDirManager->IsContactInFavorite(nContactId);
}

bool LocalDir_IsFavoriteIndexExist(const int & nIndex, const int & nContactID)
{
    return g_pLocalDirManager->IsFavoriteIndexExist(nIndex, nContactID);
}

int LocalDir_CopyContactToFavorite(int nContactId, int nFavoriteIndex /*= 0*/)
{
    return g_pLocalDirManager->CopyContactToFavorite(nContactId, nFavoriteIndex);
}

bool LocalDir_RemoveContactsIdFromFavorite(const VectorId& vecRemoveId, int nParentID)
{
    return g_pLocalDirManager->RemoveContactsIdFromFavorite(vecRemoveId, nParentID);
}

int LocalDir_GetFavoriteIndex()
{
    return g_pLocalDirManager->CreatFavoriteIndex();
}
#endif //IF_FEATURE_FAVORITE_DIR

bool LocalDir_IsContactInFavorite(int nContactId)
{
    return g_pLocalDirManager->IsContactInFavorite(nContactId);
}

int LocalDir_CopyContactToFavorite(int nContactId)
{
    return g_pLocalDirManager->CopyContactToFavorite(nContactId);
}

#ifdef IF_FEATURE_PSTN
//bool LocalDir_RemoveContactsIdFromFavorite(const VectorId& vecRemoveId, int nParentID)
//{
//    return g_pLocalDirManager->RemoveContactsIdFromFavorite(vecRemoveId, nParentID);
//}
#endif

DIRECTORY_RET_CODE RemoteDir_ReqLoadContact(int nGroupId, const yl::string & strKey/* = ""*/)
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    return g_pRemoteDirManager->ReqLoadContact(nGroupId, strKey) ? DIR_RET_SUCC : DIR_RET_FAIL;
}

int RemoteDir_GetRootGroupId(int nId)
{
    return g_pRemoteDirManager->GetRootGroupId(nId);
}

bool RemoteDir_IsSpecialSearchGroup(int nGroupId)
{
    int nId = g_pRemoteDirManager->GetRootGroupId(nGroupId);

    SmartPtr pDetail = Dir_GetDetailById(nId, REMOTE_DIR);
    if (pDetail.IsEmpty() || DIR_TYPE_REMOTE_MENU != pDetail->GetType())
    {
        return false;
    }

    ContactRemoteMenu * pRemoteMenu = (ContactRemoteMenu *)pDetail.Get();

    return (NULL == pRemoteMenu ? false : pRemoteMenu->m_bSearchRootGroup);
}

FileDownloadStatus RemoteDir_GetHotKeyStatus(const yl::string & strKey, int nGroupId,
        int & nReHotkeyId)
{
    return g_pRemoteDirManager->GetHotKeyStatus(strKey, nGroupId, nReHotkeyId);
}


DIRECTORY_RET_CODE RemoteDir_Update(int nGroupId /* = -1 */)
{
    return g_pRemoteDirManager->Update(nGroupId);
}

const yl::string & RemoteDir_GetRemoteDisplayName()
{
    return g_pRemoteDirManager->GetRemoteDisplayName();
}

DIRECTORY_RET_CODE BsftDir_ReqLoadContact(int nGroupId)
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    return g_pBroadSoftDirManager->ReqLoadContact(nGroupId) ? DIR_RET_SUCC : DIR_RET_FAIL;
}

DIRECTORY_RET_CODE BsftDir_ReqNextPageContact(int nGroupId, bool bSearch/* = false*/)
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    return g_pBroadSoftDirManager->ReqNextPageContact(nGroupId, bSearch) ? DIR_RET_SUCC : DIR_RET_FAIL;
}

DIRECTORY_RET_CODE BsftDir_ReqSearchContact(int nGroupId, ContactBroadSoftItemData & refSearchData)
{
    return g_pBroadSoftDirManager->ReqSearchContact(nGroupId, refSearchData);
}

bool BsftDir_PreLoad(int nGroupId /* = knInvalidRId */, bool bLoad /* = true */)
{
    return g_pBroadSoftDirManager->PreLoad(nGroupId, bLoad);
}

int BsftDir_AddContact2Personal(ContactBroadSoftPersonal & refData)
{
    return g_pBroadSoftDirManager->AddContact2Personal(refData);
}

DIRECTORY_RET_CODE BsftDir_DeletePersonalContact(VectorId & vecRemoveId,
        BSFT_SEARCH_TYPE eType/* = SEARCH_NONE*/)
{
    return g_pBroadSoftDirManager->DeletePersonalContact(vecRemoveId, eType);
}

DIRECTORY_RET_CODE BsftDir_EditPersonalContact(int nId, ContactBroadSoftPersonal & refData,
        BSFT_SEARCH_TYPE eType/* = SEARCH_NONE*/)
{
    return g_pBroadSoftDirManager->EditPersonalContact(nId, refData, eType);
}

DIRECTORY_RET_CODE BsftDir_Update(int nGroupId /* = kszAllContactId */)
{
    return g_pBroadSoftDirManager->Update(nGroupId);
}

//处理XSI密码
DIRECTORY_RET_CODE BsftDir_OnSubmitXSIPassword(const yl::string & strPswd, int nGroupId)
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    bool bIsAllowSipAuth = (configParser_GetConfigInt(kszAllowSipAuth) == 1);
    if (bIsAllowSipAuth)
    {
        configParser_SetCfgItemStringValue(kszAccountPassword, 0, strPswd, CONFIG_LEVEL_PHONE);

        //发送消息，账号配置改变
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, 0);
    }
    else
    {
        configParser_SetCfgItemStringValue(kszAccountXSIPassword, 0, strPswd, CONFIG_LEVEL_PHONE);
    }

    CBaseHelper::SetXSINotif(true);
    configParser_SetConfigString(kszBsftCustomListUse, "", CONFIG_LEVEL_PHONE);

    // BroadSoft配置更新. // todo
    if (g_pBroadSoftDirManager->PreLoad(nGroupId, CONFIG_LEVEL_PHONE))
    {
        return DIR_RET_SUCC;
    }
    return DIR_RET_FAIL;
}

int BsftDir_AddContact2AllContact(ContactGeneralItemData & refData)
{
    return g_pBroadSoftDirManager->AddContact2AllContact(refData);
}

bool BsftDir_UpdateEnable()
{
    return configParser_GetConfigInt(kszBsftDirUpdateEnable) == 1;
}

void BsftDir_ClearContactInfo(int nGroupID)
{
    g_pBroadSoftDirManager->ClearContactInfo(nGroupID);
}

bool BsftDir_HasNextPage(int nGroupID)
{
    return g_pBroadSoftDirManager->HasNextPage(nGroupID);
}

bool BsftLog_LogDeleteEnable()
{
    return configParser_GetConfigInt(kszBsftLogDeleteEnable) == 1;
}


DIRECTORY_RET_CODE BsftLog_RemoveSingleLog(int nId)
{
    return g_pBroadSoftLogManager->RemoveSingleLog(nId);
}

DIRECTORY_RET_CODE BsftLog_RemoveGroupLog(int nGroupId)
{
    return g_pBroadSoftLogManager->RemoveGroupLog(nGroupId);
}

DIRECTORY_RET_CODE BsftLog_ReqLoadCallLog(int nGroupId)
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    return g_pBroadSoftLogManager->ReqLoadCallLog(nGroupId) ? DIR_RET_SUCC : DIR_RET_FAIL;
}

DIRECTORY_RET_CODE BsftLog_OnSubmitXSIPassword(const yl::string & strPswd, int nGroupId)
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    int nAccountID = 0;
#if IF_FEATURE_MUTI_CALLLOG
    if (BsftLog_IsMutilLogEnable())
    {
        nAccountID = BsftLog_GetAccountIDByGroupID(nGroupId);
    }
#endif

    bool bIsAllowSipAuth = (configParser_GetConfigInt(kszAllowSipAuth) == 1);
    if (bIsAllowSipAuth)
    {
        configParser_SetCfgItemStringValue(kszAccountPassword, nAccountID, strPswd, CONFIG_LEVEL_PHONE);

        //发送消息，账号配置改变
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, nAccountID);
    }
    else
    {
        configParser_SetCfgItemStringValue(kszAccountXSIPassword, nAccountID, strPswd, CONFIG_LEVEL_PHONE);
    }

    CBaseHelper::SetXSINotif(true);

    g_pBroadSoftLogManager->ReloadConfig();

    if (g_pBroadSoftLogManager->ReqLoadCallLog(nGroupId))
    {
        return DIR_RET_SUCC;
    }
    return DIR_RET_FAIL;
}

bool BsftLog_IsEnable()
{
    if (configParser_GetConfigInt(kszBroadsoftActive) != 0
            && configParser_GetConfigInt(kszBsftEnableCallLog) != 0
            && configParser_GetConfigInt(kszXSISwitch) != 0)
    {
        return true;
    }

    return false;
}


CALL_TYPE BsftLog_GetCallTypeByGroupID(int nGroupID)
{
    return g_pBroadSoftLogManager->GetCallTypeByGroupID(nGroupID);
}

#if IF_FEATURE_MUTI_CALLLOG
int BsftLog_ChangeToActualGroup(int nGroupID, CALL_TYPE eType /*= CT_CALLALL*/)
{
    return g_pBroadSoftLogManager->ChangeToActualGroup(nGroupID, eType);
}

int BsftLog_GetGroupIDByAccountID(int nAccountID, CALL_TYPE eType /*= CT_CALLALL*/)
{
    return g_pBroadSoftLogManager->GetGroupIDByAccountID(nAccountID, eType);
}

void BsftLog_ClearCallLog()
{
    g_pBroadSoftLogManager->ClearBsftCallLog();
}

bool BsftLog_IsMutilLogEnable()
{
    return configParser_GetConfigInt(kszMutilBroadSoftLog) == 1;
}

bool BsftLog_IsAccountGroup(int nGroupID)
{
    return g_pBroadSoftLogManager->IsAccountGroup(nGroupID);
}

int BsftLog_GetAccountIDByGroupID(int nGroupID)
{
    return g_pBroadSoftLogManager->GetAccountIDByGroupID(nGroupID);
}

int BsftLog_GetAccGroupIDByAccountID(int nAccountID)
{
    return g_pBroadSoftLogManager->GetAccGroupIDByAccountID(nAccountID);
}
#endif

bool BsftDir_IsEnable()
{
    if (configParser_GetConfigInt(kszBroadsoftActive) != 0
            && configParser_GetConfigInt(kszBsftEnableDirectory) != 0
            && configParser_GetConfigInt(kszXSISwitch) != 0)
    {
        return true;
    }

    return false;
}

#ifdef IF_BT_SUPPORT_PHONE
DIRECTORY_RET_CODE BlueToothDir_Update()
{
    return g_pBlueToothDirManager->Update();
}

void BlueToothDir_CancelUpdate()
{
    g_pBlueToothDirManager->CancelUpdate();
}

bool BlueToothDir_LoadContactPhoto()
{
    return g_pBlueToothDirManager->LoadContactPhoto();
}

FileDownloadStatus BlueToothDir_GetDownloadStatus()
{
    return g_pBlueToothDirManager->GetDownloadStatus();
}
#else
DIRECTORY_RET_CODE BlueToothDir_Update()
{
    return DIR_RET_FAIL;
}

void BlueToothDir_CancelUpdate()
{
}

bool BlueToothDir_LoadContactPhoto()
{
    return false;
}

FileDownloadStatus BlueToothDir_GetDownloadStatus()
{
    return FILE_DOWNLOAD_STATUS_NONE;
}
#endif

#if IF_BUG_PLCMDIR

DIRECTORY_RET_CODE PclmDir_ReqLoadContact(int nGroupId)
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    //URL 为空
    yl::string strUrl = configParser_GetConfigString(kszAutoProvisionUrl);
    if (strUrl.empty())
    {
        return GET_LIST_NONE_DOWNLOAD;
    }

    return g_pPlcmDirManager->ReqLoadContact(nGroupId) ? DIR_RET_SUCC : DIR_RET_FAIL;
}


DIRECTORY_RET_CODE PclmDir_Update()
{
    return g_pPlcmDirManager->Update();
}

#endif

#if IF_FEATURE_MUTI_CALLLOG
int LocalLog_GetLastChildMissAccountID()
{
    return g_pLocalLogManager->GetLastMissAccountID();
}
#endif

//清除MissCall等状态
void Dir_ClearNotifyList(int nGroupId, REQUEST_ID eReqID /*= DEFAULT_ID*/)
{
    g_pLocalLogManager->ClearNotifyListByID(nGroupId, eReqID);
}

#ifdef IF_FEATURE_CLOUD

DIRECTORY_RET_CODE CloudDir_ReqLoadContact(int nGroupId)
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    return g_pCloudDirManager->ReqLoadContact(nGroupId) ? DIR_RET_SUCC : DIR_RET_FAIL;
}

bool CloudDir_Init(int nAccountId)
{
    g_pCloudDirManager->Init(nAccountId);
    return true;
}

bool CloudDir_UnInit(bool bRefreshUI /*= true*/)
{
    g_pCloudDirManager->UnInit(bRefreshUI);
    return true;
}

bool CloudDir_AutoUpdate(bool bEnable)
{
    g_pCloudDirManager->SetAutoUpdateTimer(bEnable);
    return true;
}

bool CloudDir_IsAccountBindCloudDir(int nAccountId)
{
    return g_pCloudDirManager->GetCloudAccountId() == nAccountId;
}


bool Dir_IsCloudCallLog(int nId)
{
    return g_pLocalLogManager->IsCloudCallLog(nId);
}
#endif //#ifdef IF_FEATURE_CLOUD

#if IF_FEATURE_GBBOOK

//GAB
DIRECTORY_RET_CODE GAB_ReqSearchGABByType(const yl::string & strkey, GBBOOK_OPERATION_TYPE eType)
{
    return g_pGenBandDirManager->ReqSearchGABByType(strkey, eType);
}

const yl::string & GAB_GetCurentGabKey()
{
    return g_pGenBandDirManager->GetCurentGabKey();
}

GBBOOK_OPERATION_TYPE GAB_GetCurrentGabOptType()
{
    return g_pGenBandDirManager->GetCurrentGabOptType();
}

void GAB_ClearSearchInfo()
{
    g_pGenBandDirManager->ClearGabSearchInfo();
}

//PAB
DIRECTORY_RET_CODE PAB_ReqAddGroup(const yl::string & strName)
{
    return g_pGenBandDirManager->ReqAddGroup(strName);
}

DIRECTORY_RET_CODE PAB_ReqRemoveGroup(int nGroupId)
{
    return g_pGenBandDirManager->ReqRemoveGroup(nGroupId);
}

DIRECTORY_RET_CODE PAB_ReqModifyGroup(int nGroupId, const yl::string & strName)
{
    return g_pGenBandDirManager->ReqModifyGroup(nGroupId, strName);
}

DIRECTORY_RET_CODE PAB_ReqAddContact(ContactGBBookUnit & refData)
{
    return g_pGenBandDirManager->ReqAddContact(refData);
}

DIRECTORY_RET_CODE PAB_ReqRemoveContact(int nId)
{
    return g_pGenBandDirManager->ReqRemoveContact(nId);
}

DIRECTORY_RET_CODE PAB_ReqModifyContact(int nId, ContactGBBookUnit & refData)
{
    return g_pGenBandDirManager->ReqModifyContact(nId, refData);
}

bool PAB_UpdatePabData()
{
    return g_pGenBandDirManager->UpdatePabData();
}


int PAB_IsContactNameExist(const yl::string & strName)
{
    return g_pGenBandDirManager->IsGroupNameExist(strName);
}
#endif

#if IF_FEATURE_METASWITCH_CALLLOG
bool MTSWLog_IsEnable()
{
    if (configParser_GetConfigInt(kszMTSWCallListEnable) == 1
            && configParser_GetConfigInt(kszMetaSwitchActive) == 1
            && configParser_GetConfigInt(kszMTSWCommportalEnable) == 1)
    {
        return true;
    }
    return false;
}

bool LocalLog_IsEnable()
{
    //在Metaswitch历史记录开启的情况下才允许关闭本地历史记录
    if (!MTSWLog_IsEnable())
    {
        return true;
    }

    if (configParser_GetConfigInt(kszReplaceLocalCallListEnable) == 1)
    {
        return false;
    }
    return true;
}

DIRECTORY_RET_CODE MTSWLog_ReqLoadCallLog()
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }
    return g_pMTSWLogManager->ReqLoadCallLog() ? DIR_RET_SUCC : DIR_RET_FAIL;
}

bool MTSWLog_IsDownLoading()
{
    return g_pMTSWLogManager->IsCallLogDownLoading();
}
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
int MtswDir_AddContact2ContactList(ContactMetaSwitchContactList & refData,
                                   bool bFouceAdd /*= false*/)
{
    return g_pMetaSwitchDirManager->AddContact2ContactList(refData);
}

DIRECTORY_RET_CODE MtswDir_EditContactList(int nId, ContactMetaSwitchContactList & refData,
        bool bFouceEdit/* = false*/)
{
    return g_pMetaSwitchDirManager->EditContactListContact(nId, refData);
}

DIRECTORY_RET_CODE MtswDir_DeleteContactList(VectorId & vecRemoveId)
{
    return g_pMetaSwitchDirManager->DeleteContactListContact(vecRemoveId);
}

DIRECTORY_RET_CODE MtswDir_ClearContact(int nGroupId)
{
    return g_pMetaSwitchDirManager->ClearContact(nGroupId);
}

DIRECTORY_RET_CODE MtswDir_Update(int nGroupId /* = kszAllContactId */)
{
    return g_pMetaSwitchDirManager->Update(nGroupId);
}

void MtswDir_SortContactList()
{
    return g_pMetaSwitchDirManager->SortContactList();
}

SORT_TYPE MtswDir_GetSortType()
{
    return g_pMetaSwitchDirManager->GetCurrentSortType();
}

bool MtswDir_IsEnable()
{
    if (configParser_GetConfigInt(kszMTSWDirEnable) == 1
            && configParser_GetConfigInt(kszMetaSwitchActive) == 1
            && configParser_GetConfigInt(kszMTSWCommportalEnable) == 1)
    {
        return true;
    }
    return false;
}

void MtswDir_CancelLoad()
{
    //g_pMetaSwitchDirManager->SetDownLoadStatus(FILE_DOWNLOAD_CANCELED);
}

#endif
