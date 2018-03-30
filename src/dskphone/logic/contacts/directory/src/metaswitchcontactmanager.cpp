#if IF_FEATURE_METASWITCH_DIRECTORY

#include "metaswitchcontactmanager.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "basemethod.h"
#include "taskaction/modtaskaction.h"
#include "directorystructure.h"
#include "cbasehelper.h"
#include "directorymessagedefine.h"
#include "dircommonfunc.h"
#include "ylalgorithm.h"

#include "commonunits/modcommonunits.h"

#include "metaswitch/mtswcontrol/include/modmtsw.h"


#define kszGroupUrl "/data?data=SubscriberContactGroups"
#define kszContactListUrl "/data?data=Meta_Subscriber_UC9000_Contacts"
#define kszExtentionUrl "/data?data=Meta_BusinessGroup_ChildrenList_Subscriber"
#define kszMLHGUrl "/data?data=Meta_Subscriber_GroupMembershipsList"
#define kszMLHGGroupUrl "/data?&data=Meta_MLHG_ChildrenList_MLHGPilotDN?mlhg="

#define kszContactAndGroupUrl "/data?data=Meta_Subscriber_UC9000_Contacts,SubscriberContactGroups"

#define kszAddData "\r\nContent-Disposition: form-data; name=\"Meta_Subscriber_UC9000_Contacts?vCard.GROUP.UID=\"\r\n\r\n"
#define kszEditData "\r\nContent-Disposition: form-data; name=\"Meta_Subscriber_UC9000_Contacts?vCard.GROUP.UID="
#define kszDelData "\r\nContent-Disposition: form-data; name=\"Meta_Subscriber_UC9000_Contacts?vCard.GROUP.UID=*\"\r\n\r\n"


bool ContactListCompare(int nLeftId, int nRightId)
{
    return g_pMetaSwitchDirManager->CompareMtswContact(nLeftId, nRightId);
}

bool GroupCompare(int nLeftId, int nRightId)
{
    if (nLeftId == kszMtswAllContact)
    {
        return true;
    }

    if (nRightId == kszMtswAllContact)
    {
        return false;
    }

    return strcasecmp(g_pMetaSwitchDirManager->GetSortKeyById(nLeftId).c_str(),
                      g_pMetaSwitchDirManager->GetSortKeyById(nRightId).c_str()) < 0;
}

CMetaSwitchContactManager * CMetaSwitchContactManager::GetInstance()
{
    static CMetaSwitchContactManager instance;
    return &instance;
}

CMetaSwitchContactManager::CMetaSwitchContactManager()
{
    m_eDirType = MTSW_DIR;

    m_eSortType = SORT_BY_LASTNAME;
}

CMetaSwitchContactManager::~CMetaSwitchContactManager()
{
}

//请求下载联系人
bool CMetaSwitchContactManager::ReqLoadContact(int nGroupId)
{
    int iGroupId = nGroupId;

    if (nGroupId != kszExtention
            && nGroupId != kszMLHG)
    {
        iGroupId = kszContactList;
    }
    ContactMetaSwitchMenu * pGroup = Base2MtswMenu(GetMenuDataById(iGroupId));

    if (pGroup == NULL)
    {
        return false;
    }

    if (pGroup->m_eStatus == FILE_DOWNLOADING)
    {
        DIR_INFO("-=-=--== %d Group Downing -=-=-=-=--=", iGroupId);
        return true;
    }

    if (nGroupId != kszExtention
            && nGroupId != kszMLHG)
    {
        //如果该分组下面是组，需要让里面的All组在最前面
        //这样才可以删除map中所有联系人。
        ClearContactByGroupID(kszContactList);
    }
    else
    {
        ClearContactByGroupID(nGroupId);
    }

    bool bLoadSuc = false;

    if (nGroupId == kszExtention)
    {
        bLoadSuc = MTSW_ExecDownLoad(this, kszExtentionUrl, ACTION_EXTENTION, "", nGroupId);
    }
    else if (nGroupId == kszMLHG)
    {
        bLoadSuc = MTSW_ExecDownLoad(this, kszMLHGUrl, ACTION_MLHG, "", nGroupId);
    }
    else
    {
        bLoadSuc = MTSW_ExecDownLoad(this, kszContactAndGroupUrl, ACTION_CONTACTANDGROUP, "", iGroupId);
    }

    if (bLoadSuc)
    {
        pGroup->m_eStatus = FILE_DOWNLOADING;
    }

    return bLoadSuc;
}


//预加载函数，初始化,nGroup可以指定预加载组
void CMetaSwitchContactManager::PreLoad()
{
    MyFinal();

    if (!MyInit())
    {
        return;
    }

    if (MTSW_IsLogined())
    {
        LoadAllContact();
    }

}

//新建联系人，加入到Perosnal联系人中
//|refData|是加入到联系人的信息
//返回新增联系人ID,失败返回错误类型
//nId 不为-1则指定id
int CMetaSwitchContactManager::AddContact2ContactList(ContactMetaSwitchContactList & refData)
{
    //先判断网络是否可用
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    if ((refData.m_strFirstName.empty() && refData.m_strLastName.empty()))
    {
        return DIR_RET_EMPTY;
    }

    ContactMetaSwitchMenu * pGroupContactList = Base2MtswMenu(GetDataById(kszMtswAllContact));
    if (pGroupContactList == NULL)
    {
        pGroupContactList = Base2MtswMenu(GetDataById(kszContactList));
    }

    if (pGroupContactList == NULL)
    {
        return DIR_RET_GROUP_UNEXISTED;
    }

    if (GetContactCount(pGroupContactList->m_nId) >= kszLocalMaxContactNum)
    {
        return DIR_RET_GROUP_SIZE_UNEXPECTED;
    }

    int Index = DIR_RET_FAIL;

    ContactMetaSwitchContactList * pNew = new ContactMetaSwitchContactList();

    *pNew = refData;

    Index = AddContact(pNew, pGroupContactList->m_nId);

    if (Index > 0)
    {
        char * szTmp = pNew->GetJsonString();
        yl::string strData = kszAddData;
        strData += szTmp;
        strData += "\r\n";
        strData += "\r\n";
        MTSW_ExecPost(this, kszContactListUrl, strData, ACTION_CONTACTLIST_ADD);
        if (szTmp != NULL)
        {
            free(szTmp);
        }

        ResetSearchKey();
    }

    return Index;
}

// 删除联系人
DIRECTORY_RET_CODE CMetaSwitchContactManager::DeleteContactListContact(VectorId & vecRemoveId)
{
    //先判断网络是否可用
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    DIR_INFO("CMetaSwitchContactManager::DeleteContactListContact [%d]", vecRemoveId.size());

    ContactMetaSwitchMenu * pContactList = Base2MtswMenu(GetDataById(kszMtswAllContact));
    if (pContactList == NULL)
    {
        DIR_INFO("Metaswithc Is Null, Get Contact List Group");
        pContactList = Base2MtswMenu(GetDataById(kszContactList));
    }

    if (pContactList == NULL)
    {
        return DIR_RET_GROUP_UNEXISTED;
    }

    char * szTmp;
    szTmp = GetDeleteJsonString(vecRemoveId);

    yl::string strData = kszDelData;
    strData += szTmp;
    strData += "\r\n";
    MTSW_ExecPost(this, kszContactListUrl, strData, ACTION_CONTACTLIST_DELETE);
    if (szTmp != NULL)
    {
        free(szTmp);
    }

    RemoveRecord(vecRemoveId, pContactList->m_nId);

    RemoveIdFromGroup(vecRemoveId, kszSearchGroupId);

    ResetSearchKey();
    return DIR_RET_SUCC;
}

char * CMetaSwitchContactManager::GetDeleteJsonString(VectorId & vecId)
{
    cJSON * root = cJSON_CreateObject();

    cJSON * vCardArray = cJSON_CreateArray();

    cJSON_AddItemToObject(root, "vCard", vCardArray);

    for (int i = 0; i < vecId.size(); i++)
    {
        ContactMetaSwitchContactList * pContact = Base2MtswContact(GetDataById(vecId[i]));

        if (pContact == NULL)
        {
            continue;
        }

        cJSON * contact = cJSON_CreateObject();

        cJSON_AddItemToArray(vCardArray, contact);

        cJSON * GroupArray = cJSON_CreateArray();

        cJSON_AddItemToObject(contact, "GROUP", GroupArray);

        cJSON * GroupContact = cJSON_CreateObject();

        cJSON_AddItemToArray(GroupArray, GroupContact);

        cJSON * UIDArray = cJSON_CreateArray();

        cJSON_AddItemToObject(GroupContact, "UID", UIDArray);


        cJSON * UID = cJSON_CreateObject();

        cJSON_AddItemToObject(UID, "_", cJSON_CreateString(pContact->m_strUID.c_str()));

        cJSON_AddItemToArray(UIDArray, UID);
    }


    cJSON_AddItemToObject(root, "_Action", cJSON_CreateString("delete"));

    char * out = cJSON_Print(root);
    cJSON_Delete(root);

    return out;
}

//Edit联系人，加入到联系人中
//|refData|是加入到联系人的信息
//返回失败信息，成功返回0
DIRECTORY_RET_CODE CMetaSwitchContactManager::EditContactListContact(int nId,
        ContactMetaSwitchContactList & refData)
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    ContactMetaSwitchMenu * pMenu = Base2MtswMenu(GetDataById(kszMtswAllContact));
    if (pMenu == NULL)
    {
        pMenu = Base2MtswMenu(GetDataById(kszContactList));
    }

    if (pMenu == NULL)
    {
        return DIR_RET_GROUP_UNEXISTED;
    }

    ContactMetaSwitchContactList * pContact = Base2MtswContact(GetDataById(nId));

    if (pContact == NULL)
    {
        return DIR_RET_FAIL;
    }

    if (refData.m_strFirstName.empty() && refData.m_strLastName.empty())
    {
        return DIR_RET_EMPTY;
    }

    //名字有改变，需要排序
    bool bSort = strcasecmp(pContact->m_strFirstName.c_str(), refData.m_strFirstName.c_str()) != 0
                 || strcasecmp(pContact->m_strLastName.c_str(), refData.m_strLastName.c_str()) != 0;

    //编辑联系人不去下载，display名字需要组装
    refData.m_strDisplayName = refData.m_strFirstName;
    if (!refData.m_strDisplayName.empty() && !refData.m_strLastName.empty())
    {
        refData.m_strDisplayName.append(",");
    }
    refData.m_strDisplayName.append(refData.m_strLastName);

    refData.m_strNameIndex = commonAPI_transT9String(refData.m_strDisplayName);

    refData.m_nParentId = pMenu->m_nId;

    refData.m_nId = nId;

    refData.m_strUID = pContact->m_strUID;

    *pContact = refData;

    etl_NotifyApp(false, METASWITCH_PHONEBOOK_POST_RESULT, nId, 1);

    char * szTmp = pContact->GetJsonString(false);
    yl::string strData = kszEditData;
    strData += refData.m_strUID;
    strData += "\"\r\n\r\n";
    strData += szTmp;
    strData += "\r\n";
    if (szTmp != NULL)
    {
        free(szTmp);
    }

    if (bSort)
    {
        SortContactByCurrentSortType();
        ResetSearchKey();
    }

    return MTSW_ExecPost(this, kszContactListUrl, strData,
                         ACTION_CONTACTLIST_EDIT) ? DIR_RET_SUCC : DIR_RET_FAIL;
}

//定时更新，UI点击更新
//nGroupId未指定更新某一地址簿，默认值更新所有
DIRECTORY_RET_CODE CMetaSwitchContactManager::Update(int nGroupId /*= kszAllContactId*/)
{
    DIR_INFO("CMetaSwitchContactManager::Update (nGroupId = %d)", nGroupId);
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    if (nGroupId != kszAllContactId)
    {
        if (ReqLoadContact(nGroupId))
        {
            return DIR_RET_SUCC;
        }
    }
    else
    {

        return LoadAllContact() == FILE_DOWNLOADING ? DIR_RET_SUCC : DIR_RET_FAIL;
    }

    return DIR_RET_FAIL;
}

// 处理定时器消息.
// | refMessage | 是要处理的消息.
// 如果处理成功则返回TRUE, 否则返回FALSE.
bool CMetaSwitchContactManager::OnTimeout(unsigned int uTimerID, bool bInTalk) DECLARE_ATTR
{
    if (bInTalk)
    {
        // 通话中不去更新
        return false;
    }

    if (uTimerID == (UINT)&m_iTimerID)
    {
        ReSetId();
        Update();
        return true;
    }

    return false;
}

//设置自动更新定时器
void CMetaSwitchContactManager::SetAutoUpdateTimer()
{
    if (IsDirAvailable())
    {
        // 读取定时器时间.
        using namespace yl;
        int iUpdateInterval = max(configParser_GetConfigInt(kszMTSWDirUpdateInterVal), 60);

        if (iUpdateInterval > 43200 || iUpdateInterval < 0)
        {
            iUpdateInterval = 60;
        }

        m_iTimerID = iUpdateInterval * 60 * 1000;

        timerSetThreadTimer((UINT)&m_iTimerID, m_iTimerID);
    }
}

void CMetaSwitchContactManager::KillUpdateTimer()
{
    timerKillThreadTimer((UINT)&m_iTimerID);
}

// 初始化.
// 如果初始化成功则返回true, 否则返回false.
bool CMetaSwitchContactManager::MyInit()
{
    DIR_INFO("CMetaSwitchContactManager::MyInit");
    if (configParser_GetConfigInt(kszMTSWDirEnable) == 0
            || configParser_GetConfigInt(kszMetaSwitchActive) == 0)
    {
        return false;
    }

    CBaseContactManger::MyInit();

    ContactMetaSwitchMenu * pGroup = Base2MtswMenu(GetDataById(kszTempContact));
    if (pGroup == NULL)
    {
        pGroup = new ContactMetaSwitchMenu();
        if (pGroup != NULL)
        {
            InsertMap(pGroup, kszTempContact);
        }
    }

    if (MyCreatGroupsFromConfig())
    {
        SetAutoUpdateTimer();

        return true;
    }
    return false;
}

//加载成功则返回true, 否则返回false.
bool CMetaSwitchContactManager::MyCreatGroupsFromConfig()
{
    if (configParser_GetConfigInt(kszMTSWContactLists) == 1)
    {
        CreatConfigGroup(kszContactList, kszMTSWContactName);
    }

    if (configParser_GetConfigInt(kszMTSWExtentions) == 1)
    {
        CreatConfigGroup(kszExtention, kszMTSWExtentionName);
    }

    if (configParser_GetConfigInt(kszMTSWMLHGS) == 1)
    {
        CreatConfigGroup(kszMLHG, kszMTSWMLHGName);

    }

    return true;
}

//创建配置组节点
void CMetaSwitchContactManager::CreatConfigGroup(int nGroupId, const char * lpszConfigName)
{
    ContactMetaSwitchMenu * pNew = new ContactMetaSwitchMenu();

    pNew->m_strDisplayName = configParser_GetConfigString(lpszConfigName);

    DIR_INFO("mtsw Add Group: %d %s %s", nGroupId, pNew->m_strDisplayName.c_str(),
             pNew->m_strFullUrl.c_str());

    InsertMap(pNew, nGroupId);

    AddIdToMenuShowList(nGroupId);
}

bool CMetaSwitchContactManager::IsNeedShowAllGroup()
{
    return false;
}

// 启动下载.
// | strUrl | 是文件在服务器的地址, 也是map的key.
// | isSync | 同步还是异步.
// 如果下载成功则返回FILE_DOWNLOAD_OK, 否则返回错误码
int CMetaSwitchContactManager::LoadAllContact()
{
    DIR_INFO("CMetaSwitchContactManager::MyExecDownload");

    bool suc = false;

    for (int i = 0; i < m_vGroupId.size(); i++)
    {
        int iGroupId = m_vGroupId[i];

        DirGroupRecord * pGroup = GetMenuDataById(iGroupId);

        if (pGroup == NULL || pGroup->m_eStatus == FILE_DOWNLOADING)
        {
            continue;
        }

        DIR_INFO("CMetaSwitchContactManager::LoadAllContact GroupId:[%d]", iGroupId);
        ClearContactByGroupID(iGroupId);

        bool bLoadSuc = false;

        if (iGroupId == kszContactList)
        {
            bLoadSuc = MTSW_ExecDownLoad(this, kszContactAndGroupUrl, ACTION_CONTACTANDGROUP, "", iGroupId);
        }
        else if (iGroupId == kszExtention)
        {
            bLoadSuc = MTSW_ExecDownLoad(this, kszExtentionUrl, ACTION_EXTENTION, "", iGroupId);
        }
        else if (iGroupId == kszMLHG)
        {
            bLoadSuc = MTSW_ExecDownLoad(this, kszMLHGUrl, ACTION_MLHG, "", iGroupId);
        }

        if (bLoadSuc)
        {
            pGroup->m_eStatus = FILE_DOWNLOADING;
            suc = true;
        }
    }

    return suc ? FILE_DOWNLOADING : FILE_DOWNLOAD_FAIL;
}

DIRECTORY_RET_CODE CMetaSwitchContactManager::ClearContact(int nGroupId)
{
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);

    if (pGroup == NULL)
    {
        return DIR_RET_GROUP_UNEXISTED;
    }

    char * szTmp = GetDeleteJsonString(pGroup->m_vChildrenId);

    yl::string strData = kszDelData;
    strData += szTmp;
    strData += "\r\n";

    if (szTmp != NULL)
    {
        free(szTmp);
    }

    DIRECTORY_RET_CODE rec = MTSW_ExecPost(this, kszContactListUrl, strData,
                                           ACTION_CONTACTLIST_DELETE) ? DIR_RET_SUCC : DIR_RET_FAIL;

    if (rec == DIR_RET_SUCC)
    {
        ClearContactByGroupID(nGroupId);

        ResetSearchKey();
    }

    return rec;
}


void CMetaSwitchContactManager::SortContactList()
{
    DIR_INFO("CMetaSwitchContactManager::SortContactList Sort type [%d]", m_eSortType);
    if (m_eSortType == SORT_BY_LASTNAME)
    {
        m_eSortType = SORT_BY_FIRSTNAME;
    }
    else
    {
        m_eSortType = SORT_BY_LASTNAME;
    }

    DirGroupRecord * pGroup = GetMenuDataById(kszMtswAllContact);

    if (pGroup == NULL)
    {
        pGroup = GetMenuDataById(kszContactList);
    }

    if (pGroup != NULL)
    {
        yl::stable_sort(pGroup->m_vChildrenId.begin(), pGroup->m_vChildrenId.end(), ContactListCompare);
    }

    DirGroupRecord * pAllGroup = GetMenuDataById(kszAllContactId);

    if (pAllGroup != NULL)
    {
        yl::stable_sort(pAllGroup->m_vChildrenId.begin(), pAllGroup->m_vChildrenId.end(),
                        ContactListCompare);

    }
}

SORT_TYPE CMetaSwitchContactManager::GetCurrentSortType()
{
    return m_eSortType;
}

bool CMetaSwitchContactManager::GetContactNameByNumber(const yl::string & strNumber,
        yl::string & strName, bool bSync /*= true*/)
{
    ContactMetaSwitchMenu * pMenu = Base2MtswMenu(GetDataById(kszExtention));

    if (pMenu != NULL)
    {
        for (int i = 0; i < pMenu->m_vChildrenId.size(); i++)
        {
            ContactMetaSwitchExtention * pContact = static_cast<ContactMetaSwitchExtention *>(GetDataById(
                    pMenu->m_vChildrenId[i]));

            if (pContact != NULL)
            {
                for (ListNumber::iterator it = pContact->m_listNumber.begin(); it != pContact->m_listNumber.end();
                        it++)
                {
                    stNumber & stNum = *it;
                    if (strNumber.compare(stNum.strValue) == 0)
                    {
                        strName = pContact->GetDisplayName();
                        return true;
                    }
                }
            }
        }
    }
    return CBaseContactManger::GetContactNameByNumber(strNumber, strName);
}

bool CMetaSwitchContactManager::CompareMtswContact(int leftId, int rightId)
{
    BaseDirDataRecord * pBaseLeft = GetDataById(leftId);
    BaseDirDataRecord * pBaseRight = GetDataById(rightId);
    if (pBaseLeft != NULL && pBaseRight != NULL)
    {
        ContactMetaSwitchContactList * pRecordLeft =  Base2MtswContact(pBaseLeft);
        ContactMetaSwitchContactList * pRecordRight =  Base2MtswContact(pBaseRight);

        if (pRecordLeft != NULL && pRecordRight != NULL)
        {
            if (SORT_BY_LASTNAME == m_eSortType)
            {
                int iCmp = strcasecmp(pRecordLeft->m_strLastName.c_str(), pRecordRight->m_strLastName.c_str());
                if (iCmp < 0)
                {
                    return true;
                }
                else if (iCmp > 0)
                {
                    return false;
                }
                else
                {
                    return strcasecmp(pRecordLeft->m_strFirstName.c_str(), pRecordRight->m_strFirstName.c_str()) < 0;
                }
            }
            else if (SORT_BY_FIRSTNAME == m_eSortType)
            {
                int iCmp = strcasecmp(pRecordLeft->m_strFirstName.c_str(), pRecordRight->m_strFirstName.c_str());
                if (iCmp < 0)
                {
                    return true;
                }
                else if (iCmp > 0)
                {
                    return false;
                }
                else
                {
                    return strcasecmp(pRecordLeft->m_strLastName.c_str(), pRecordRight->m_strLastName.c_str()) < 0;
                }
            }
        }
        else if (pRecordLeft != NULL)
        {
            int iCmp = 0;
            if (SORT_BY_LASTNAME == m_eSortType)
            {
                iCmp = strcasecmp(pRecordLeft->m_strLastName.c_str(), pBaseRight->m_strDisplayName.c_str());
            }
            else
            {
                iCmp = strcasecmp(pRecordLeft->m_strFirstName.c_str(), pBaseRight->m_strDisplayName.c_str());
            }
            if (iCmp < 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (pRecordRight != NULL)
        {
            int iCmp = 0;
            if (SORT_BY_LASTNAME == m_eSortType)
            {
                iCmp = strcasecmp(pBaseLeft->m_strDisplayName.c_str(), pRecordRight->m_strLastName.c_str());
            }
            else
            {
                iCmp = strcasecmp(pBaseLeft->m_strDisplayName.c_str(), pRecordRight->m_strFirstName.c_str());
            }
            if (iCmp > 0)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return strcasecmp(pBaseLeft->m_strDisplayName.c_str(), pBaseRight->m_strDisplayName.c_str()) < 0;
        }
    }
    return true;
}

bool CMetaSwitchContactManager::PraseData(cJSON * pOjbParse, int nId)
{
#define kszdataType "dataType"
#define kszMetaGroup "SubscriberContactGroups"
#define kszMetaContactList "Meta_Subscriber_UC9000_Contacts"
#define kszMetaExtention "Meta_BusinessGroup_ChildrenList_Subscriber"
#define kszMetaMLHGName "Meta_Subscriber_GroupMembershipsList"
#define kszMetaMLHGGroupName "Meta_MLHG_ChildrenList_MLHGPilotDN"


    for (; pOjbParse; pOjbParse = pOjbParse->next)
    {
        cJSON * pDataType = cJSON_GetObjectItem(pOjbParse, kszdataType);

        if (pDataType == NULL || pDataType->valuestring == NULL)
        {
            continue;
        }

        yl::string strType = pDataType->valuestring;

        cJSON * pData = cJSON_GetObjectItem(pOjbParse, "data");

        if (pData == NULL)
        {
            continue;
        }

        if (strType.compare(kszMetaContactList) == 0)
        {
            //解析联系人
            ParseContact(pData);
        }
        else if (strType.compare(kszMetaGroup) == 0)
        {
            ParseGroup(pData);
        }
        else if (strType.compare(kszMetaExtention) == 0)
        {
            //解析企业联系人
            ParseExtention(pData);
        }
        else if (strType.compare(kszMetaMLHGName) == 0)
        {
            //解析MLHG联系人
            ParseMLHG(pData);
        }
        else if (strType.compare(kszMetaMLHGGroupName) == 0)
        {
            //解析MLHG联系人号码
            ParseMLHGGroup(pData, nId);
        }
    }

    return true;
}

void CMetaSwitchContactManager::ParseContact(cJSON * pOjbParse)
{
    DIR_INFO("CMetaSwitchContactManager::ParseContact");
    if (pOjbParse == NULL)
    {
        return;
    }

    m_mapUidToId.clear();

    cJSON * pVcard = cJSON_GetObjectItem(pOjbParse, "vCard");

    if (pVcard)
    {
        ContactMetaSwitchMenu * pContacListAllGroup = new ContactMetaSwitchMenu();
        pContacListAllGroup->m_strDisplayName = "All Contacts";
        pContacListAllGroup->m_nParentId = kszContactList;
        InsertMap(pContacListAllGroup, kszMtswAllContact);
        AddIdToGroup(kszMtswAllContact, kszContactList, false);

        for (cJSON * pJsonContact = pVcard->child; pJsonContact; pJsonContact = pJsonContact->next)
        {
            ContactMetaSwitchContactList * pNew = new ContactMetaSwitchContactList();

            if (pNew->PraseData(pJsonContact))
            {
                AddContact(pNew, kszMtswAllContact);

                m_mapUidToId.put(pNew->m_strUID, pNew->m_nId);
            }
            else
            {
                delete pNew;
            }
        }
    }
}

void CMetaSwitchContactManager::ParseGroup(cJSON * pOjbParse)
{
    DIR_INFO("CMetaSwitchContactManager::ParseGroup");
#define kszDescription "Description"
#define kszMembersUID "MembersUID"
#define kszUniqueID "UniqueID"
#define kszMemberListsUID "MemberListsUID"

    if (pOjbParse == NULL)
    {
        return;
    }
    cJSON * pGroup = cJSON_GetObjectItem(pOjbParse, "Group");

    //如果只是组，其下不需要再有All组
    if (pGroup == NULL || pGroup->child == NULL)
    {
        DirGroupRecord * pContactList = GetMenuDataById(kszContactList);

        DirGroupRecord * pContactListAll = GetMenuDataById(kszMtswAllContact);

        if (pContactList != NULL && pContactListAll != NULL)
        {
            pContactList->m_vChildrenId = pContactListAll->m_vChildrenId;

            RemoveDataFromMap(kszMtswAllContact);
        }

        return;
    }

    char * pValue = NULL;

    for (cJSON * pData = pGroup->child; pData; pData = pData->next)
    {
        pValue = cJSON_GetFistChildValueByObjectName(pData, kszDescription);

        if (pValue == NULL)
        {
            continue;
        }

        ContactMetaSwitchMenu * pGroup = new ContactMetaSwitchMenu();

        pGroup->m_strDisplayName = pValue;

        pGroup->m_nParentId = kszContactList;

        pGroup->m_eStatus = FILE_DOWNLOADED_OK;

        pValue = cJSON_GetFistChildValueByObjectName(pData, kszUniqueID);

        if (pValue != NULL)
        {
            pGroup->m_strUID = pValue;
        }

        int nGroupId = InsertMap(pGroup);

        AddIdToGroup(nGroupId, kszContactList);

        cJSON * pMebId = cJSON_GetObjectItem(pData, kszMembersUID);

        if (pMebId)
        {
            for (cJSON * pId = pMebId->child; pId; pId = pId->next)
            {
                cJSON * pIDData = cJSON_GetObjectItem(pId, "_");

                if (pIDData && pIDData->valuestring)
                {
                    YLHashMap<yl::string, int>::iterator iter = m_mapUidToId.find(pIDData->valuestring);

                    if (iter != m_mapUidToId.end())
                    {
                        AddIdToGroup(iter->second, nGroupId);
                    }
                }
            }
        }
    }
    SortGroup(kszContactList);

    m_mapUidToId.clear();
}

void CMetaSwitchContactManager::ParseExtention(cJSON * pOjbParse)
{
#define kszSubscriber "Subscriber"

    if (pOjbParse == NULL)
    {
        return;
    }


    cJSON * pExtObj = cJSON_GetObjectItem(pOjbParse, kszSubscriber);

    if (pExtObj == NULL || pExtObj->child == NULL)
    {
        return;
    }

    for (cJSON * pData = pExtObj->child; pData; pData = pData->next)
    {
        ContactMetaSwitchExtention * pNew = new ContactMetaSwitchExtention();

        if (pNew->PraseData(pData))
        {
            int nId = AddContact(pNew, kszExtention);
        }
        else
        {
            delete pNew;
        }
    }
}


void CMetaSwitchContactManager::ParseMLHGGroup(cJSON * pOjbParse, int nId)
{
    ContactMetaSwitchMLHG * pContact = Base2MtswhMLHGContact(GetDataById(nId));

    if (pContact == NULL || pOjbParse == NULL)
    {
        return;
    }

    cJSON * pMLHG = cJSON_GetObjectItem(pOjbParse, "MLHGPilotDN");

    if (pMLHG == NULL || pMLHG->child == NULL)
    {
        return;
    }

    cJSON * pData = pMLHG->child;

    char * pValue = NULL;

    if (pData)
    {
        pValue = cJSON_GetFistChildValueByObjectName(pData, "IntercomDialingCode");
    }

    if (pValue != NULL)
    {
        stNumber stMum;
        stMum.strName = kszMtswDirectoryNumber;
        stMum.strValue = pValue;
        pContact->m_listNumber.push_back(stMum);
    }
}

void CMetaSwitchContactManager::ParseMLHG(cJSON * pOjbParse)
{
    cJSON * pGroup = cJSON_GetObjectItem(pOjbParse, "Group");

    if (pGroup == NULL)
    {
        return;
    }

    if (pGroup == NULL || pGroup->child == NULL)
    {
        return;
    }

    for (cJSON * pData = pGroup->child; pData; pData = pData->next)
    {
        ContactMetaSwitchMLHG * pNew = new ContactMetaSwitchMLHG();

        if (pNew->PraseData(pData))
        {
            int nId = AddContact(pNew, kszMLHG);

            yl::string strUrl = kszMLHGGroupUrl;
            strUrl += pNew->m_strDisplayName;
            MTSW_ExecDownLoad(this, strUrl, ACTION_MLHG_GROUP, "", nId);
        }
        else
        {
            delete pNew;
        }
    }
}


void CMetaSwitchContactManager::OnRequestError(NS_TA::TaskActionErrorType eErrCode, int nType,
        int nId)
{
    DIR_INFO("-=-=-=- CMetaSwitchContactManager::OnRequestError: nType:%d =-=-=-=-", nType);
    if (nType > ACTION_CONTACTLIST && nType <= ACTION_CONTACTANDGROUP)
    {
        DirGroupRecord * pGroup = GetMenuDataById(nId);

        if (pGroup != NULL)
        {
            pGroup->m_eStatus = FILE_DOWNLOAD_FAIL;
        }
    }

    if (eErrCode != TA_SESSION_EXPIRED)
    {
        etl_NotifyApp(
            false,
            METASWITCH_PHONEBOOK_DOWNLOADED_RESULT,
            nId,
            eErrCode);
    }
}


bool CMetaSwitchContactManager::OnLoadSucProcess(CMTSWAction * pAction)
{
    int nType = pAction->GetMTSWActionType();

    DIR_INFO("-=-=-=- CMetaSwitchContactManager::OnMTSWActionResponse: nType:%d =-=-=-=-", nType);
    bool bResult = false;

    if (nType == ACTION_CONTACTLIST
            || nType == ACTION_EXTENTION
            || nType == ACTION_MLHG
            || nType == ACTION_GROUP
            || nType == ACTION_MLHG_GROUP
            || nType == ACTION_CONTACTANDGROUP)
    {
        DirGroupRecord * pGroup = GetMenuDataById(pAction->GetExtraParam());

        if (pAction->GetJsonObject() != NULL)
        {
            if (nType == ACTION_EXTENTION
                    || nType == ACTION_MLHG
                    || nType == ACTION_CONTACTANDGROUP)
            {
                if (pGroup == NULL)
                {
                    etl_NotifyApp(
                        false,
                        METASWITCH_PHONEBOOK_DOWNLOADED_RESULT,
                        pAction->GetExtraParam(),//id
                        0);
                    return false;
                }
                else
                {
                    if (pGroup->m_eStatus != FILE_DOWNLOADING)
                    {
                        return false;
                    }
                }
            }

            bResult = PraseData(pAction->GetJsonObject(), pAction->GetExtraParam());

        }

        if (pGroup != NULL)
        {
            pGroup->m_eStatus = bResult ? FILE_DOWNLOADED_OK : FILE_DOWNLOAD_FAIL;
        }

        DIR_INFO("Send Mesg bResult[%d]", bResult);

        etl_NotifyApp(
            false,
            METASWITCH_PHONEBOOK_DOWNLOADED_RESULT,
            pAction->GetExtraParam(),//id
            bResult);
    }
    else if (nType == ACTION_CONTACTLIST_ADD)
    {
        DirGroupRecord * pGroup = GetMenuDataById(kszContactList);
        if (pGroup != NULL)
        {
            pGroup->m_eStatus = FILE_DOWNLOADING;
        }
        ClearContactByGroupID(kszContactList);
        MTSW_ExecDownLoad(this, kszContactAndGroupUrl, ACTION_CONTACTANDGROUP, "", kszContactList);
    }

    return bResult;
}

DIRECTORY_RET_CODE CMetaSwitchContactManager::AddIdToGroup(int nId, int nGroupId,
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

        bool bCmp = CompareMtswContact(pGroup->m_vChildrenId[nMid], nId);
        if (bCmp)
        {
            nCmp = -1;
        }
        else
        {
            nCmp = 1;
        }

        if ((nCmp > 0 && isOrder) || (nCmp < 0 && !isOrder))
        {
            nRingt = nMid - 1;
        }
        else if (nCmp == 0 && nGroupId != kszAllContactId && nGroupId != kszServerSearchGroupId
                 && m_eDirType != BSFT_DIR
                 && m_eDirType != MTSW_DIR
                )
        {
            return DIR_RET_CONTACT_EXIST;
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

void CMetaSwitchContactManager::SortGroup(int nGroupId)
{
    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);

    if (pGroup != NULL)
    {
        yl::stable_sort(pGroup->m_vChildrenId.begin(), pGroup->m_vChildrenId.end(), GroupCompare);
    }
}

int CMetaSwitchContactManager::IsContactNameExist(const yl::string & strName, int nGroupId,
        int nExceptId/* = knInvalidRId*/)
{
    //需求原因改为所有联系人不能重名-syl
    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);
    if (pGroup == NULL)
    {
        return -1;
    }

    int j = 0;
    for (; j < pGroup->m_vChildrenId.size(); j++)
    {
        BaseDirDataRecord * pBase = GetDataById(pGroup->m_vChildrenId[j]);

        if (pBase == NULL || pBase->m_nId == nExceptId)
        {
            continue;
        }

        if (strcasecmp(pBase->m_strDisplayName.c_str(), strName.c_str()) == 0)
        {
            return pBase->m_nId;
        }
    }
    return -1;
}

bool CMetaSwitchContactManager::MySearch(int nGroupId, const yl::string & strKey, VectorId & output,
        SearchMethod m_eSearchType)
{
    int nSearchGroupID = nGroupId;
    if (nGroupId == kszContactList)
    {
        //Contact List 组才走下面，Contact组包含两种情况，一种是底下全部是联系人，一种是底下全部是组

        DirGroupRecord * pGroup = GetMenuDataById(kszMtswAllContact);
        if (pGroup != NULL)
        {
            //存在mtsw all 组，all组下面的联系人就是contactlist下面所有联系人
            nSearchGroupID = kszMtswAllContact;
        }
    }

    DIR_INFO("CMetaSwitchContactManager::MySearch nSearchGroupID [%d]", nSearchGroupID);

    return CBaseContactManger::MySearch(nSearchGroupID, strKey, output, m_eSearchType);
}

void CMetaSwitchContactManager::SortContactByCurrentSortType()
{
    DIR_INFO("CMetaSwitchContactManager::SortContactBySortType Sort type [%d]", m_eSortType);

    //如果只有一个组，则只有kszContactList组需要排序
    bool bSortChildGroup = true;
    DirGroupRecord * pGroup = GetMenuDataById(kszMtswAllContact);

    if (pGroup == NULL)
    {
        bSortChildGroup = false;
        pGroup = GetMenuDataById(kszContactList);
    }

    if (pGroup == NULL)
    {
        return;
    }

    if (bSortChildGroup)
    {
        DirGroupRecord * pGroupMenu = GetMenuDataById(kszContactList);

        if (pGroupMenu != NULL)
        {
            for (int i = 0; i < pGroupMenu->m_vChildrenId.size(); ++i)
            {
                int nGroupID = pGroupMenu->m_vChildrenId[i];
                DirGroupRecord * pChildGroup = GetMenuDataById(nGroupID);
                if (nGroupID != kszContactList && pChildGroup != NULL)
                {
                    yl::stable_sort(pChildGroup->m_vChildrenId.begin(), pChildGroup->m_vChildrenId.end(),
                                    ContactListCompare);
                }
            }
        }
    }
    else
    {
        yl::stable_sort(pGroup->m_vChildrenId.begin(), pGroup->m_vChildrenId.end(), ContactListCompare);
    }

    DirGroupRecord * pAllGroup = GetMenuDataById(kszAllContactId);

    if (pAllGroup != NULL)
    {
        yl::stable_sort(pAllGroup->m_vChildrenId.begin(), pAllGroup->m_vChildrenId.end(),
                        ContactListCompare);
    }
}

#endif //IF_FEATURE_METASWITCH_DIRECTORY
