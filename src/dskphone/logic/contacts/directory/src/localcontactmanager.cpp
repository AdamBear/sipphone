#include "localcontactmanager.h"
#include "directorystructure.h"
#include "basemethod.h"
#include "commonunits/modcommonunits.h"
#include "xportmanager.h"
#include "stable_sort.hpp"
#include "etlmsghandler/modetlmsghandler.h"
#include "configparser/modconfigparser.h"
#include "directorymessagedefine.h"
#include "devicelib/networkserver.h"
#include "configiddefine.h"

#if IF_FEATURE_BACKUP_DIRECTORY
#include "taskaction/modtaskaction.h"
#include "taskaction/uploadfileaction.h"

#define MAX_FILE_ID 20
#endif

#define knIntervalIndexNum 10

bool Compare(int nLeftId, int nRightId)
{
    if (nLeftId == kszAllContactId)
    {
        return true;
    }

    if (nRightId == kszAllContactId)
    {
        return false;
    }

    return g_pLocalDirManager->GetCompResult(nLeftId, nRightId) < 0;

}

CLocalContactManager * CLocalContactManager::GetInstance()
{
    static CLocalContactManager instance;
    return &instance;
}

CLocalContactManager::CLocalContactManager()
{
    m_eDirType = LOCAL_DIR;
#if IF_FEATURE_BACKUP_DIRECTORY
    bNetConNeedUpload = false;
    m_nFileIndex = 0;
#endif
    m_strSearchkey.clear();
    m_iContactUpdateMark = 0;
}

CLocalContactManager::~CLocalContactManager()
{
}

void CLocalContactManager::Init()
{
    MyInit();
}

void CLocalContactManager::InitBeforeLoadData()
{
    //必须先初始化基类初始化函数
    CBaseContactManger::MyInit();

    m_MapNumToName.clear();

    //创建黑名单组
    DirGroupRecord * pBlackList = new DirGroupRecord;
    if (pBlackList != NULL)
    {
        pBlackList->m_strDisplayName = kszGroupNameBlackList;

        InsertMap(pBlackList, kszBlackListGroupId);

        m_MapIdChange.put(kszGroupNameBlackList, kszBlackListGroupId);
    }

#if IF_FEATURE_FAVORITE_DIR
    DirGroupRecord * pFavoriteList = new DirGroupRecord;
    if (pFavoriteList != NULL)
    {
        pFavoriteList->m_strDisplayName = TRID_FAVOURITE;

        InsertMap(pFavoriteList, kszFavoriteGroupId);

        m_MapIdChange.put(TRID_FAVOURITE, kszFavoriteGroupId);
    }
#endif
}

bool CLocalContactManager::MyInit()
{
    InitBeforeLoadData();

    yl::string strPath = g_strConfigPath;
    strPath.append(kszXportXmlFile);

    yl::string strMacDirPath = g_strConfigPath;
    char szTemp[64] = {0};
    sprintf(szTemp, "%s-contact.xml", (LPCSTR)netGetMACText('\0'));
    strMacDirPath.append(szTemp);

    if (pathFileExist(strMacDirPath.c_str()) && 1 == configParser_GetConfigInt(kszLocalDirSyncEnable))
    {
        XPortFactory::ImportByPath(
            strMacDirPath.c_str(),
            false  // 覆盖
        );
    }
    //http://10.2.1.199/Bug.php?BugID=51710
    else if (pathFileExist(strPath.c_str()))
    {
        XPortFactory::ImportByPath(
            NULL,  // 默认路径.
            false  // 覆盖
        );
    }
    else
    {
        ParseGroup(szContactGroupFile);

        ParseContact(szContactsFile);

        SortAllData();

        // 清空组名和ID的映射关系
        m_MapIdChange.clear();

        //保存组信息到临时文件，供网页使用
        SaveGroupInfoToTempFile("LocalDirGroupInfo.xml", "LocalDirGroup");
    }

#if IF_FEATURE_FAVORITE_DIR
    NotifyFavoriteChange();
#endif

    return true;
}

void CLocalContactManager::ReloadContact()
{
    MyFinal();
    MyInit();
}

bool CLocalContactManager::ParseGroup(const yl::string & strFile)
{
    if (!pathFileExist(strFile.c_str()))//组文件不存在时，需要创建组文件
    {
        return CreateContactGroupList(strFile);
    }
    xml_document doc;
    doc.load_file(strFile.c_str());

    xml_node nodeRootNode = doc.child(kszGXmlRoot);
    if (!nodeRootNode)
    {
        return false;
    }

    for (xml_node node = nodeRootNode.child(kszGXmlNodeGroup); //添加组
            node;
            node = node.next_sibling(kszGXmlNodeGroup))
    {
        // 组上限
        int groupSize = m_vGroupId.size();
        if (groupSize >= DIRECTORY_MAX_GROUP_SIZE)
        {
            break;
        }
        // 组名重复
        yl::string strName = node.attribute(kszGXmlAttrDisplayName).value();
        // ALL 组和黑名单在初始化中进行
        if (!IsGroupNameUsable(strName, knInvalidRId))
        {
            continue;
        }

        LocalDirGroupRecord * pNewGroup = new LocalDirGroupRecord();

        if (pNewGroup != NULL)
        {
            pNewGroup->ParseFromXMLNode(node);

            int nId = InsertMap(pNewGroup);

            m_MapIdChange.put(strName, nId);

            AddIdToMenuShowList(nId);
        }
    }

    return true;
}

bool CLocalContactManager::ParseContact(const yl::string & strFile)
{
    if (!pathFileExist(strFile.c_str()))
    {
        return false;
    }

    xml_document doc;
    doc.load_file(strFile.c_str());

    xml_node rootNode = doc.child(kszCXmlRoot);
    if (!rootNode)
    {
        return false;
    }
    // 遍历各个联系人结点.
    for (xml_node node = rootNode.child(kszCXmlNodeContact);
            node;
            node = node.next_sibling(kszCXmlNodeContact))
    {
        int nGroupID = kszAllContactId;//组ID默认为0
        xml_attribute attrTmp = node.attribute(kszCXmlAttrGroupIdName);
        if (attrTmp)
        {
            // 读取group_id_name属性.
            yl::string strGroupID = attrTmp.value();
            nGroupID = GetGroupIdByGroupName(strGroupID);

            if (nGroupID == knInvalidRId)
            {
                nGroupID = kszAllContactId;
            }

            //网页有确保，可考虑去除该判断
            if (!EnableToInsert(nGroupID))
            {
                continue;
            }
        }

        LocalDirContactRecord * pNew = new LocalDirContactRecord();

        if (pNew == NULL)
        {
            DIR_INFO("Error new LocalDirContactRecord");
            return false;
        }
        //赋值父ID
        pNew->m_nParentId = nGroupID;

        pNew->ParseFromXMLNode(node);

        int nId = AddContact(pNew, nGroupID, false);

#if IF_FEATURE_FAVORITE_DIR
        if (nId > 0 && pNew->m_nFavoriteIndex > 0)
        {
            AddIdToGroup(nId, kszFavoriteGroupId, true);
        }
#endif

#ifdef IF_FEATURE_PSTN
        if (nId > 0 && pNew->m_bIsFavorite)
        {
            AddIdToGroup(nId, kszFavoriteGroupId, true);
        }
#endif

    }  // for.
    return true;
}

bool CLocalContactManager::CreateContactGroupList(const yl::string & path)
{
    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child(kszGXmlRoot);

    static const char * s_defGList[] =
    {
        kszGroupNameAll,
        kszGroupNameBlackList,
//      kszGroupNameCompany,
//      kszGroupNameFamily,
//      kszGroupNameFriend,
    };

    // 生成各个默认组.
    for (size_t i = 0; i < sizeof(s_defGList) / sizeof(s_defGList[0]); ++i)
    {
        xml_node node = nodeRootNode.append_child(kszGXmlNodeGroup);
        node.append_attribute(kszGXmlAttrDisplayName, s_defGList[i]);
        node.append_attribute(kszGXmlAttrRing, "");

        //All组已加入
        if (i > 1)
        {
            LocalDirGroupRecord * pNew = new LocalDirGroupRecord();
            pNew->m_strDisplayName = s_defGList[i];
            int nId = InsertMap(pNew, i);
            AddIdToMenuShowList(nId);
        }
    }  // for.

    doc.save_file(path.c_str());

    return true;
}

bool CLocalContactManager::IsGroupNameUsable(const yl::string & strGroupName,
        int nExceptId /* = knInvalidRId */)
{
    if (strcasecmp(strGroupName.c_str(), kszGroupNameAll) == 0
            || strcasecmp(strGroupName.c_str(), kszGroupNameBlackList) == 0
#if IF_FEATURE_FAVORITE_DIR
            || strcasecmp(strGroupName.c_str(), "Favorites") == 0
#endif
#ifdef IF_FEATURE_PSTN
            || strcasecmp(strGroupName.c_str(), TRID_FAVORITE_LIST) == 0
            || strcasecmp(strGroupName.c_str(), "Favorite") == 0
#endif
            || strGroupName.empty())
    {
        return false;
    }

    for (int i = 0; i < m_vGroupId.size(); i++)
    {
        if (nExceptId != knInvalidRId && m_vGroupId[i] == nExceptId)
        {
            continue;
        }

        DirGroupRecord * pGroup = GetMenuDataById(m_vGroupId[i]);

        if (pGroup != NULL
                && strcasecmp(strGroupName.c_str(), pGroup->m_strDisplayName.c_str()) == 0)
        {
            return false;
        }
    }
    return true;
}

bool CLocalContactManager::IsNeedShowAllGroup()
{
    return true;
}

bool CLocalContactManager::ExportContactAndGroup(yl::string strPath /*= ""*/)
{
    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    // 导出联系人组.
    xml_node nodeRootGroup = doc.append_child(kszGXmlRoot);
    CreatRootGroupNode(nodeRootGroup);

    //导出联系人
    xml_node nodeRootContact = doc.append_child(kszCXmlRoot);
    CreatRootContactNode(nodeRootContact);

    yl::string strFilePath;
    if (strPath.empty())
    {
        strFilePath.append(g_strConfigPath);
        strFilePath.append(kszXportXmlFile);
    }
    else
    {
        strFilePath = strPath;
    }

    doc.save_file(strFilePath.c_str());
    return true;
}

void CLocalContactManager::CreatRootGroupNode(xml_node & rootGroup)
{
    //All 组
    xml_node nodeAll = rootGroup.append_child(kszGXmlNodeGroup);

    nodeAll.append_attribute(kszGXmlAttrDisplayName, kszGroupNameAll);

    //黑名单组
    xml_node nodeBlackList = rootGroup.append_child(kszGXmlNodeGroup);

    nodeBlackList.append_attribute(kszGXmlAttrDisplayName, kszGroupNameBlackList);

    //All 上面单独插入
    for (int i = 1; i < m_vGroupId.size(); i ++)
    {
        // todo
        //if (IsAllContactGroup(m_vGroupId[i]))
        //{
        //  continue;
        //}

        DirGroupRecord * pGroup = GetMenuDataById(m_vGroupId[i]);
        // 且需要排除 All 组，因为 All 组在上面已单独导出
        if (pGroup != NULL)
        {
            xml_node nodeGroup = rootGroup.append_child(kszGXmlNodeGroup);
            pGroup->ToXMLNode(nodeGroup);
        }
    }
}

void CLocalContactManager::CreatRootContactNode(xml_node & rootContact)
{
    DirGroupRecord * pAll = GetMenuDataById(kszAllContactId);

    if (pAll != NULL)
    {
        for (int i = 0; i < pAll->m_vChildrenId.size(); i ++)
        {
            BaseDirDataRecord * pContact = GetDataById(pAll->m_vChildrenId[i]);

            if (pContact != NULL && pContact->GetType() == DIR_TYPE_LOCAL_CONTACT)
            {
                xml_node nodeContact = rootContact.append_child(kszCXmlNodeContact);
                pContact->ToXMLNode(nodeContact);
            }

        }
    }

    //导出黑名单
    DirGroupRecord * pBlackList = GetMenuDataById(kszBlackListGroupId);

    if (pBlackList != NULL)
    {
        for (int i = 0; i < pBlackList->m_vChildrenId.size(); i ++)
        {
            BaseDirDataRecord * pContact = GetDataById(pBlackList->m_vChildrenId[i]);

            if (pContact != NULL && pContact->GetType() == DIR_TYPE_LOCAL_CONTACT)
            {
                xml_node nodeContact = rootContact.append_child(kszCXmlNodeContact);
                pContact->ToXMLNode(nodeContact);
            }
        }
    }
}

void CLocalContactManager::InsertToNumMap(const yl::string & strNumber, const yl::string & strName)
{
    if (strNumber.empty())
    {
        return;
    }
    m_MapNumToName.put(strNumber, strName);
}

bool CLocalContactManager::GetContactNameByNumber(const yl::string & strNumber,
        yl::string & strName, bool bSync /* = true*/)
{
    //MapNumberToName::iterator iter = m_MapNumToName.find(strNumber);

    //if (iter != m_MapNumToName.end())
    //{
    //  strName = iter->second;
    //  return true;
    //}

    return CBaseContactManger::GetContactNameByNumber(strNumber, strName, bSync);
}

const yl::string & CLocalContactManager::GetContactNameByNumber(const yl::string & strNumber)
{
    MapNumberToName::iterator iter = m_MapNumToName.find(strNumber);

    if (iter != m_MapNumToName.end())
    {
        return iter->second;
    }

    return kszEmptyString;
}

const yl::string & CLocalContactManager::GetContactImageByNumber(const yl::string & strNumber,
        const yl::string & strName/* = ""*/)
{

    if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
    {

        SetContactLock();
        const LocalDirContactRecord * pContact = Base2LocalContact(GetBaseContactByNumber(strNumber, strName));
        SetContactUnLock();

        if (pContact != NULL)
        {
            return pContact->m_strDefaultPhoto;
        }

    }

    return kszEmptyString;
}

const yl::string & CLocalContactManager::GetRingByNumber(const yl::string & strNum)
{
    LocalDirContactRecord * pContact = Base2LocalContact(GetBaseContactByNumber(strNum));

    if (pContact == NULL)
    {
        return kszEmptyString;
    }

    if (!pContact->m_strRing.empty() && pathFileExist(pContact->m_strRing.c_str()))
    {
        DIR_INFO("The ring path in contact is: %s", pContact->m_strRing.c_str());
        return pContact->m_strRing;
    }
    else
    {
        // 如果未找到则返回组铃声.
        DirGroupRecord * pGroup = GetMenuDataById(pContact->m_nParentId);

        if (pGroup != NULL)
        {
            DIR_INFO("The ring path in group is: %s", pGroup->m_strRing.c_str());
            return pGroup->m_strRing;
        }
    }

    return kszEmptyString;
}

bool CLocalContactManager::EnableToInsert(int groupID, int nAdd/* = 1*/)
{
    DirGroupRecord * pGroup = GetMenuDataById(groupID == kszBlackListGroupId ? groupID :
                              kszAllContactId);
    if (pGroup == NULL)
    {
        return false;
    }

    return groupID == kszBlackListGroupId ? pGroup->m_vChildrenId.size() + nAdd <= kszBlackListMaxNum :
           pGroup->m_vChildrenId.size() + nAdd <= kszLocalMaxContactNum ;
}

void CLocalContactManager::SaveGroupToFile(bool bNeedUpload /*= true*/)
{
    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootGroup = doc.append_child(kszGXmlRoot);
    CreatRootGroupNode(nodeRootGroup);

    doc.save_file(szContactGroupFile);

    //保存组信息到临时文件，供网页使用
    SaveGroupInfoToTempFile("LocalDirGroupInfo.xml", "LocalDirGroup");

    SetContactUpdateMark(2);

    etl_NotifyApp(false, LOCAL_DIRECTORY_UPLOAD, 0, 0);

}

void CLocalContactManager::SaveContactToFile(bool bNeedUpload /*= true*/)
{
    //写文件前先重置下搜索关键字
    ResetSearchKey();
    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    //导出联系人
    xml_node nodeRootContact = doc.append_child(kszCXmlRoot);
    CreatRootContactNode(nodeRootContact);

    doc.save_file(szContactsFile);

    SetContactUpdateMark(1);
#if IF_FEATURE_BACKUP_DIRECTORY
    if (bNeedUpload)
    {
        UploadBackupDir();
    }
#endif

    etl_NotifyApp(false, LOCAL_DIRECTORY_UPLOAD, 0, 0);
}

int CLocalContactManager::AddGroup(const ContactGroupData & groupData, bool bImPort/* = false*/)
{
    if (groupData.m_strDisplayName.empty())
    {
        return DIR_RET_GROUP_NAME_EMPTY;
    }

    if (!IsGroupNameUsable(groupData.m_strDisplayName))
    {
        return DIR_RET_GROUP_DUPLICATE;
    }

    if (m_vGroupId.size() >= DIRECTORY_MAX_GROUP_SIZE)
    {
        return DIR_RET_GROUP_SIZE_UNEXPECTED;
    }

    LocalDirGroupRecord * pNew = new LocalDirGroupRecord();

    if (pNew == NULL)
    {
        return DIR_RET_FAIL;
    }

    pNew->m_strDisplayName = groupData.m_strDisplayName;
    pNew->m_strRing = groupData.m_strRing;

    int nId = InsertMap(pNew);

    AddIdToMenuShowList(nId);

    if (bImPort)
    {
        m_MapIdChange.put(pNew->m_strDisplayName, nId);
    }
    else
    {
        yl::stable_sort(m_vGroupId.begin(), m_vGroupId.end(), Compare);
        //save to file
        SaveGroupToFile();

        // 通知本地群组添加
        // http://10.3.5.199/Bug.php?BugID=72343
        etl_NotifyApp(FALSE, LOCAL_GROUP_UPDATE, 0, 0);
    }

    return nId;
}

int CLocalContactManager::UpdateGroup(int groupID, const ContactGroupData & groupData)
{
    if (groupData.m_strDisplayName.empty())
    {
        return DIR_RET_GROUP_NAME_EMPTY;
    }

    if (!IsGroupNameUsable(groupData.m_strDisplayName, groupID))
    {
        return DIR_RET_GROUP_DUPLICATE;
    }

    DirGroupRecord * pGroup = GetMenuDataById(groupID);

    if (pGroup == NULL)
    {
        return DIR_RET_GROUP_UNEXISTED;
    }

    bool isSort = pGroup->m_strDisplayName.compare(groupData.m_strDisplayName) != 0;

    pGroup->m_strDisplayName = groupData.m_strDisplayName;
    pGroup->m_strRing = groupData.m_strRing;

    if (isSort)
    {
        //Sort
        yl::stable_sort(m_vGroupId.begin(), m_vGroupId.end(), Compare);
        SaveContactToFile();

        //组名有改变，通知UI，by yuyt.
        etl_NotifyApp(FALSE, LOCAL_GROUP_UPDATE, 0, 0);
    }

    SaveGroupToFile(!isSort);

    return DIR_RET_SUCC;
}

bool CLocalContactManager::RemoveGroup(int groupID, bool isOutClear/* = false*/)
{
    //防止外部错误传入All和黑名单组ID
    if (groupID == kszAllContactId || groupID == kszBlackListGroupId)
    {
        return false;
    }

    MapContactInfo::iterator iter = m_mapAllInfo.find(groupID);

    if (iter == m_mapAllInfo.end())
    {
        return false;
    }

    DirGroupRecord * pGroup = Base2DirGroup(iter->second);

    if (pGroup == NULL)
    {
        return false;
    }

    bool bSaveContact = false;

    //重置组中联系人父ID
    for (int i = 0; i < pGroup->m_vChildrenId.size(); i++)
    {
        BaseDirDataRecord * pData = GetDataById(pGroup->m_vChildrenId[i]);

        if (pData != NULL)
        {
            pData->m_nParentId = kszAllContactId;
        }
        bSaveContact = true;
    }

    delete iter->second;
    iter->second = NULL;

    m_mapAllInfo.erase(iter);

    //可通过外部统一删除，加快删除速度
    if (!isOutClear)
    {
        for (int i = 0; i < m_vGroupId.size(); i++)
        {
            if (m_vGroupId[i] == groupID)
            {
                m_vGroupId.removeAt(i);
                break;
            }
        }

        SaveGroupToFile();

        if (bSaveContact)
        {
            SaveContactToFile();
        }

        // 通知本地群组被删除，针对内部删除
        // http://10.3.5.199/Bug.php?BugID=72116
        etl_NotifyApp(FALSE, LOCAL_GROUP_UPDATE, 0, 0);
    }

    return true;
}

void CLocalContactManager::ClearGroup()
{
    //跳过All组
    for (int i = 1; i < m_vGroupId.size(); i++)
    {
        RemoveGroup(m_vGroupId[i], true);
    }

    m_vGroupId.clear();

    AddIdToMenuShowList(kszAllContactId);

    SaveGroupToFile(false);
    //联系人组名有变化
    SaveContactToFile();
}

int CLocalContactManager::AddLocalContactList(YLList<ContactGeneralItemData> & refList,
        int nGroupId)
{
    int nAddCount = 0;
    bool bUpdate = false;

    if (!EnableToInsert(nGroupId, refList.size()))
    {
        return DIR_RET_GROUP_SIZE_UNEXPECTED;
    }

#if IF_FEATURE_FAVORITE_DIR
    bool bIsAutoNeedSet = false;
#endif

    for (YLList<ContactGeneralItemData>::iterator iter = refList.begin(); iter != refList.end(); iter++)
    {
        ContactGeneralItemData & refData = *iter;

#if IF_FEATURE_FAVORITE_DIR
        if (refData.m_nFavoriteIndex > 0)
        {
            bIsAutoNeedSet = true;
        }
#endif

        int nContactID = IsContactNameExist(refData.m_strDisplayName, nGroupId);
        if (nContactID > 0)
        {
            // http://10.2.1.199/Bug.php?BugID=92604 更新不需要每次都保存联系人文件
            UpdateContact(refData, nGroupId, nContactID, false);
            bUpdate = true;
        }
        else
        {
            if (AddLocalContact(refData, nGroupId, false) > 0)
            {
                nAddCount++;
            }
        }
    }

    if (nAddCount > 0 || bUpdate)
    {
        //保存联系人到文件
        SaveContactToFile();

#if IF_FEATURE_FAVORITE_DIR
        if (bIsAutoNeedSet)
        {
            NotifyFavoriteChange();
        }
#endif
    }

    return nAddCount;
}


int CLocalContactManager::AddLocalContact(const ContactGeneralItemData & refContactData,
        int nGroupId, bool bSave /* = true */)
{
    if (!EnableToInsert(nGroupId))
    {
        return DIR_RET_GROUP_SIZE_UNEXPECTED;
    }

    if (IsContactNameExist(refContactData.m_strDisplayName, nGroupId) > 0)
    {
        return DIR_RET_CONTACT_EXIST;
    }

    if (kszBlackListGroupId == nGroupId)
    {
        if (GetContactCount(nGroupId) >= DIRECTORY_MAX_BLACKLIST_SIZE)
        {
            return DIR_RET_GROUP_SIZE_UNEXPECTED;
        }
    }
    else if (GetContactCount(nGroupId) >= DIRECTORY_MAX_CONTACT_SIZE)
    {
        return DIR_RET_GROUP_SIZE_UNEXPECTED;
    }

#if IF_FEATURE_FAVORITE_DIR
    if (refContactData.m_nFavoriteIndex > LOCAL_FAVORITE_INDEX_MAX)
    {
        return DIR_RET_FAVORITE_INDEX_MAX;
    }

    if (IsFavoriteIndexExist(refContactData.m_nFavoriteIndex))
    {
        return DIR_RET_FAVORITE_INDEX_EXIST;
    }
#endif

    LocalDirContactRecord * pNew = new LocalDirContactRecord();

    if (pNew == NULL)
    {
        return DIR_RET_FAIL;
    }

    pNew->m_nLine = refContactData.m_nLine;//账号
    pNew->m_strDisplayName = commonAPI_TrimString(refContactData.m_strDisplayName,
                             " "); //去掉两边空格

    //号码
//  if (!refContactData.m_strOfficePhoneNumber.empty())
//  {
    stNumber stNum;
    stNum.strName = kszOfficeNumber;
    stNum.strValue = refContactData.m_strOfficePhoneNumber;
    pNew->m_listNumber.push_back(stNum);
    InsertToNumMap(stNum.strValue, pNew->m_strDisplayName);
    //}

//  if (!refContactData.m_strMobilePhoneNumber.empty())
//  {
    stNum.strName = kszMobileNumber;
    stNum.strValue = refContactData.m_strMobilePhoneNumber;
    pNew->m_listNumber.push_back(stNum);
    InsertToNumMap(stNum.strValue, pNew->m_strDisplayName);
    //}

//  if (!refContactData.m_strOtherPhoneNumber.empty())
//  {
    stNum.strName = kszOtherNumber;
    stNum.strValue = refContactData.m_strOtherPhoneNumber;
    pNew->m_listNumber.push_back(stNum);
    InsertToNumMap(stNum.strValue, pNew->m_strDisplayName);
    //}

    pNew->m_strRing = refContactData.m_strRing;
    pNew->m_nParentId = nGroupId;//组id

    pNew->m_strDivert = refContactData.m_strAutoDivert;

    if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
    {
        pNew->m_strDefaultPhoto = refContactData.m_strDefaultPhoto;
        pNew->m_nSelectedPhoto = refContactData.m_nSelectedPhoto;
    }


#if IF_FEATURE_FAVORITE_DIR
    pNew->m_nFavoriteIndex = refContactData.m_nFavoriteIndex;
#endif

#ifdef IF_FEATURE_PSTN
    pNew->m_bIsFavorite = refContactData.m_bIsFavorite;
#endif


    int nRec = AddContact(pNew, pNew->m_nParentId);

#if IF_FEATURE_FAVORITE_DIR
    if (nRec > 0
            && pNew->m_nFavoriteIndex > 0)
    {
        AddIdToGroup(nRec, kszFavoriteGroupId);
    }
#endif

    if (nRec > 0
            && bSave)
    {
        //保存联系人到文件
        SaveContactToFile();

        if (pNew->m_nParentId == m_nSearchGroup || (pNew->m_nParentId != kszBlackListGroupId
                && m_nSearchGroup == kszAllContactId))
        {
            if (pNew->SearchHit(m_strSearchkey) != HIT_TYPE_NONE)
            {
                AddIdToGroup(nRec, kszSearchGroupId);
            }
        }

#if IF_FEATURE_FAVORITE_DIR
        if (nRec > 0
                && pNew->m_nFavoriteIndex > 0)
        {
            NotifyFavoriteChange();
        }
#endif

#ifdef IF_FEATURE_PSTN
        if (nRec > 0
                && pNew->m_bIsFavorite)
        {
            AddIdToGroup(nRec, kszFavoriteGroupId);
        }
#endif
    }

    return nRec;
}

void CLocalContactManager::RemoveFromNumMapById(int nId)
{
    BaseContactRecord * pContact = Base2BaseContact(GetDataById(nId));
    if (pContact != NULL)
    {
        for (ListNumber::iterator iter = pContact->m_listNumber.begin();
                iter != pContact->m_listNumber.end(); iter++)
        {
            stNumber & stNum = *iter;

            m_MapNumToName.erase(stNum.strValue);
        }
    }
}

bool CLocalContactManager::RemoveContacts(const VectorId & vecRemoveId, int nParentId)
{
    if (vecRemoveId.size() == 0)
    {
        return false;
    }

#if IF_FEATURE_FAVORITE_DIR
    bool bIsNeedAutoSet = false;
#endif

    //先删除号码表中内容
    for (int i = 0; i < vecRemoveId.size(); i++)
    {
#if IF_FEATURE_FAVORITE_DIR
        if (!bIsNeedAutoSet)
        {
            LocalDirContactRecord * pContact = Base2LocalContact(GetDataById(vecRemoveId[i]));
            if (pContact != NULL && pContact->m_nFavoriteIndex > 0)
            {
                bIsNeedAutoSet = true;
            }

        }
#endif

        RemoveFromNumMapById(vecRemoveId[i]);
    }

    RemoveRecord(vecRemoveId, nParentId);

    RemoveIdFromGroup(vecRemoveId, kszSearchGroupId);

#if IF_FEATURE_FAVORITE_DIR
    RemoveIdFromGroup(vecRemoveId, kszFavoriteGroupId);

    if (bIsNeedAutoSet)
    {
        NotifyFavoriteChange();
    }
#endif

#ifdef IF_FEATURE_PSTN
    RemoveIdFromGroup(vecRemoveId, kszFavoriteGroupId);
#endif

    SaveContactToFile();

    return true;
}

void CLocalContactManager::ClearContact(int nGroupId)
{
    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);

    if (pGroup == NULL)
    {
        return;
    }

#if IF_FEATURE_PSTN
    VectorId vecRemoveId;
#endif

#if IF_FEATURE_FAVORITE_DIR
    bool bIsNeedAutoSet = false;
#endif

    //先删除号码表中内容
    for (int i = 0; i < pGroup->m_vChildrenId.size(); i++)
    {
        int nID = pGroup->m_vChildrenId[i];

#if IF_FEATURE_FAVORITE_DIR
        if (!bIsNeedAutoSet)
        {
            LocalDirContactRecord * pContact = Base2LocalContact(GetDataById(nID));
            if (pContact != NULL && pContact->m_nFavoriteIndex > 0)
            {
                bIsNeedAutoSet = true;
            }

        }
#endif

        RemoveFromNumMapById(nID);

#if IF_FEATURE_PSTN
        if (IsContactInFavorite(nID))
        {
            vecRemoveId.push_back(nID);
        }
#endif
    }

#if IF_FEATURE_PSTN
    if (vecRemoveId.size() > 0)
    {
        //清空联系人时候，从favorite组中删除
        RemoveIdFromGroup(vecRemoveId, kszFavoriteGroupId);
    }
#endif

    if (nGroupId == kszAllContactId)
    {
        ClearAllContactFromMap(kszBlackListGroupId, kszFavoriteGroupId);
    }
    else
    {
        ClearContactByGroupID(nGroupId);
    }

    SaveContactToFile();

#if IF_FEATURE_FAVORITE_DIR
    if (bIsNeedAutoSet)
    {
        NotifyFavoriteChange();
    }
#endif
}

int CLocalContactManager::IsContactNameExist(const yl::string & strName, int nGroupId,
        int nExceptId/* = knInvalidRId*/)
{
    //需求原因改为所有联系人不能重名-syl
    DirGroupRecord * pGroup = GetMenuDataById(kszAllContactId);
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

    pGroup = GetMenuDataById(kszBlackListGroupId);
    if (pGroup == NULL)
    {
        return -1;
    }

    j = 0;
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

DIRECTORY_RET_CODE CLocalContactManager::MoveToGroup(YLVector<int> & vecContactID, int nGroupID,
        int nGroupFromId)
{
    // 如果把联系人从本地移到黑名单，先判断黑名单是否可以容纳下，如果容纳不下则返回
    if (nGroupID == kszBlackListGroupId || nGroupFromId == kszBlackListGroupId)
    {
        if (!EnableToInsert(nGroupID, vecContactID.size()))
        {
            return DIR_RET_GROUP_SIZE_UNEXPECTED;
        }
    }

    if (nGroupFromId != kszAllContactId)
    {
        RemoveIdFromGroup(vecContactID, nGroupFromId);
    }

    //黑名单需同时删除all组
    if (nGroupID == kszBlackListGroupId)
    {
        RemoveIdFromGroup(vecContactID, kszAllContactId);
#if IF_FEATURE_FAVORITE_DIR
        RemoveIdFromGroup(vecContactID, kszFavoriteGroupId);
#endif
    }

#if IF_FEATURE_FAVORITE_DIR
    bool bIsNeedAutoSet = false;
#endif

    for (int i = 0; i < vecContactID.size(); i++)
    {
        int nId = vecContactID[i];

        LocalDirContactRecord * pContact = Base2LocalContact(GetDataById(nId));

        if (pContact == NULL)
        {
            continue;
        }
        //移动组
        if (pContact->m_nParentId != nGroupID)
        {
            if (nGroupFromId == kszAllContactId)
            {
                VectorId revId;
                revId.push_back(nId);
                //删除非all组
                if (pContact->m_nParentId != kszAllContactId)
                {
                    RemoveIdFromGroup(revId, pContact->m_nParentId);
                }
            }

            if (nGroupID != kszAllContactId)
            {
                AddIdToGroup(nId, nGroupID, true);
            }

#if IF_FEATURE_FAVORITE_DIR
            if (nGroupID == kszBlackListGroupId)
            {
                pContact->m_nFavoriteIndex = -1;
                bIsNeedAutoSet = true;
            }
#endif
            //黑名单移过来，同时需加入到all组
            if (pContact->m_nParentId == kszBlackListGroupId)
            {
                AddIdToGroup(nId, kszAllContactId, true);
            }

            pContact->m_nParentId = nGroupID;
        }
    }

    SaveContactToFile();

#if IF_FEATURE_FAVORITE_DIR
    if (bIsNeedAutoSet)
    {
        NotifyFavoriteChange();
    }
#endif
    return DIR_RET_SUCC;
}

int CLocalContactManager::UpdateContact(const ContactGeneralItemData & refContactData, int nGroupId,
                                        int nId, bool bSaveToFile /* = true*/)
{
    LocalDirContactRecord * pContact = Base2LocalContact(GetDataById(nId));

    if (pContact == NULL)
    {
        return DIR_RET_FAIL;
    }

#if IF_FEATURE_FAVORITE_DIR
    if (refContactData.m_nFavoriteIndex > LOCAL_FAVORITE_INDEX_MAX)
    {
        return DIR_RET_FAVORITE_INDEX_MAX;
    }
#endif

#if IF_FEATURE_FAVORITE_DIR
    bool bCurrContactInFavorite = IsContactInFavorite(nId);
    int nOldIndex = pContact->m_nFavoriteIndex;
    bool bIsNeetAutoSet = IsUpdateContactNeedSetLineKey(pContact, refContactData);
#endif

    bool bNameChang = strcasecmp(pContact->m_strDisplayName.c_str(),
                                 refContactData.m_strDisplayName.c_str()) != 0;

    bool bSort = bNameChang;

    //检测重名
    if (bNameChang /*&& nGroupId != kszAllContactId*/)
    {
        if (IsContactNameExist(refContactData.m_strDisplayName, nGroupId, nId) > 0)
        {
            return DIR_RET_CONTACT_EXIST;
        }
    }

#if IF_FEATURE_FAVORITE_DIR
    //index不同
    if (refContactData.m_nFavoriteIndex != nOldIndex)
    {
        if (IsFavoriteIndexExist(refContactData.m_nFavoriteIndex))
        {
            return DIR_RET_FAVORITE_INDEX_EXIST;
        }
    }
#endif
    //移动组
    if (pContact->m_nParentId != nGroupId)
    {
        if (pContact->m_nParentId == kszBlackListGroupId || nGroupId == kszBlackListGroupId)
        {
            if (!EnableToInsert(nGroupId))
            {
                return DIR_RET_GROUP_SIZE_UNEXPECTED;
            }
        }

        VectorId revId;
        revId.push_back(nId);
        //删除非all组
        if (pContact->m_nParentId != kszAllContactId)
        {
            RemoveIdFromGroup(revId, pContact->m_nParentId);
        }

        //黑名单需同时删除all组
        if (nGroupId == kszBlackListGroupId)
        {
            RemoveIdFromGroup(revId, kszAllContactId);
        }

        if (nGroupId != kszAllContactId)
        {
            AddIdToGroup(nId, nGroupId, true);
        }

        //黑名单移过来，同时需加入到all组
        if (pContact->m_nParentId == kszBlackListGroupId)
        {
            AddIdToGroup(nId, kszAllContactId, true);
        }

        pContact->m_nParentId = nGroupId;

        bSort = false;
    }

    pContact->m_strDisplayName = refContactData.m_strDisplayName;

    if (bNameChang)
    {
        pContact->m_strNameIndex = commonAPI_transT9String(pContact->GetDisplayName());
    }

    pContact->m_nLine = refContactData.m_nLine;//账号

    pContact->m_strRing = refContactData.m_strRing;

    pContact->m_strDivert = refContactData.m_strAutoDivert;

    if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
    {
        pContact->m_strDefaultPhoto = refContactData.m_strDefaultPhoto;
        pContact->m_nSelectedPhoto = refContactData.m_nSelectedPhoto;
    }

    RemoveFromNumMapById(nId);
    pContact->m_listNumber.clear();

    //号码
//  if (!refContactData.m_strOfficePhoneNumber.empty())
//  {
    stNumber stNum;
    stNum.strName = kszOfficeNumber;
    stNum.strValue = refContactData.m_strOfficePhoneNumber;
    pContact->m_listNumber.push_back(stNum);
    InsertToNumMap(stNum.strValue, pContact->m_strDisplayName);
    //}

//  if (!refContactData.m_strMobilePhoneNumber.empty())
//  {
    stNum.strName = kszMobileNumber;
    stNum.strValue = refContactData.m_strMobilePhoneNumber;
    pContact->m_listNumber.push_back(stNum);
    InsertToNumMap(stNum.strValue, pContact->m_strDisplayName);
    //}

//  if (!refContactData.m_strOtherPhoneNumber.empty())
//  {
    stNum.strName = kszOtherNumber;
    stNum.strValue = refContactData.m_strOtherPhoneNumber;
    pContact->m_listNumber.push_back(stNum);
    InsertToNumMap(stNum.strValue, pContact->m_strDisplayName);
    //}

#if IF_FEATURE_FAVORITE_DIR
    pContact->m_nFavoriteIndex = refContactData.m_nFavoriteIndex;
#endif

    if (bSort)
    {
        MySort(nGroupId);

        //http://bugfree.yealink.com/Bug.php?BugID=100765
        if (nGroupId != kszAllContactId)
        {
            MySort(kszAllContactId);
        }
    }

    VectorId vecId;

    vecId.push_back(nId);
    RemoveIdFromGroup(vecId, kszSearchGroupId);

    if (pContact->m_nParentId == m_nSearchGroup || (pContact->m_nParentId != kszBlackListGroupId
            && m_nSearchGroup == kszAllContactId))
    {
        if (pContact->SearchHit(m_strSearchkey) != HIT_TYPE_NONE)
        {
            AddIdToGroup(nId, kszSearchGroupId);
        }
    }

    // http://10.2.1.199/Bug.php?BugID=92604 保存文件耗时太长了，如果是外部联系人覆盖，不需要这里保存，由外部更新完保存
    if (bSaveToFile)
    {
        SaveContactToFile();
    }

#if IF_FEATURE_FAVORITE_DIR
    if (nOldIndex > 0)
    {
        RemoveIdFromGroup(vecId, kszFavoriteGroupId);
    }
    if (pContact->m_nFavoriteIndex > 0)
    {
        AddIdToGroup(nId, kszFavoriteGroupId);
    }
    if (bIsNeetAutoSet)
    {
        NotifyFavoriteChange();
    }
#endif

    return nId;
}

void CLocalContactManager::MySort(int nGroupId)
{
    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);

    if (pGroup != NULL)
    {
        yl::stable_sort(pGroup->m_vChildrenId.begin(), pGroup->m_vChildrenId.end(), Compare);
    }
}

void CLocalContactManager::SortAllData()
{
    //组排序
    yl::stable_sort(m_vGroupId.begin(), m_vGroupId.end(), Compare);

    //组内联系人排序
    for (int i = 0; i < m_vGroupId.size(); i++)
    {
        MySort(m_vGroupId[i]);
    }

    //黑名单排序
    MySort(kszBlackListGroupId);
}

// 设置联系人文件有修改的标志,1为修改联系人信息，2为修改组，3为二者都修改
void CLocalContactManager::SetContactUpdateMark(int iUpdateType)
{
    // 设置0是要清空标志，否则标志一直不为0，网页每次都重新载入文件 http://10.2.1.199/Bug.php?BugID=85228
    if (iUpdateType == 0)
    {
        m_iContactUpdateMark = iUpdateType;
        return;
    }

    // 先获取原来的值
    int iOldGroupValue = m_iContactUpdateMark;

    iOldGroupValue |= iUpdateType;

    m_iContactUpdateMark = iOldGroupValue;
}

bool CLocalContactManager::GetDivertNumberByNumber(const yl::string & strNum,
        yl::string & strDivertNum)
{
    BaseContactRecord * pBase = GetBaseContactByNumber(strNum);
    if (pBase == NULL)
    {
        DIR_INFO("GetDivertNumberByNumber empty");
        return false;
    }

    LocalDirContactRecord * pContact = Base2LocalContact(pBase);
    if (pContact != NULL && pContact->m_nParentId != kszBlackListGroupId)
    {
        strDivertNum = pContact->m_strDivert;
        return true;
    }

    return false;
}

bool CLocalContactManager::IsBlackContactNumber(const yl::string & strNum)
{
    DirGroupRecord * pBlackGroup = GetMenuDataById(kszBlackListGroupId);

    if (pBlackGroup == NULL)
    {
        DIR_ERR("BlackList Group Not Exist!");
        return false;
    }

    for (int i = 0; i < pBlackGroup->m_vChildrenId.size(); i++)
    {
        BaseContactRecord * pContact = Base2BaseContact(GetDataById(pBlackGroup->m_vChildrenId[i]));

        if (pContact == NULL)
        {
            continue;
        }

        for (ListNumber::iterator iter = pContact->m_listNumber.begin();
                iter != pContact->m_listNumber.end(); iter++)
        {
            stNumber & stNub = *iter;

            if (stNub.strValue.compare(strNum) == 0)
            {
                return true;
            }
        }
    }

    return false;
}

int CLocalContactManager::GetGroupIdByGroupName(const yl::string & strName)
{
    int nId = knInvalidRId;

    if (strName == "Blacklist")
    {
        return kszBlackListGroupId;
    }

    IdChangeMap::iterator iter = m_MapIdChange.find(strName);

    if (iter != m_MapIdChange.end())
    {
        nId = iter->second;
    }

    return nId;
}

bool CLocalContactManager::AfterImport(bool bNeedUpload /*= true*/)
{
    SortAllData();
    m_MapIdChange.clear();

    SaveGroupToFile(false);
    SaveContactToFile(bNeedUpload);

#if IF_FEATURE_FAVORITE_DIR
    NotifyFavoriteChange();
#endif
    return true;
}

bool CLocalContactManager::BeforeImport(bool isAppend)
{
    if (!isAppend)
    {
        MyFinal();
        InitBeforeLoadData();
    }
    else
    {
        for (int i = 0; i < m_vGroupId.size(); i++)
        {
            m_MapIdChange.put(GetDisplayNameById(m_vGroupId[i]), m_vGroupId[i]);
        }
    }
    m_MapIdChange.put(kszGroupNameBlackList, kszBlackListGroupId);

#if IF_FEATURE_FAVORITE_DIR
    m_MapIdChange.put(TRID_FAVOURITE, kszFavoriteGroupId);
#endif

    return true;
}

#if IF_FEATURE_FAVORITE_DIR

bool CLocalContactManager::IsFavoriteEnable()
{
    return configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1;
}

bool CLocalContactManager::IsContactInFavorite(const int & nContactID)
{
    LocalDirContactRecord * pContact = Base2LocalContact(GetDataById(nContactID));

    if (pContact == NULL)
    {
        return false;
    }

    return pContact->m_nFavoriteIndex > 0;
}

bool CLocalContactManager::IsFavoriteIndexExist(const int & nIndex, int nContactID /*= 0*/)
{
    DirGroupRecord * pGroup = GetMenuDataById(kszFavoriteGroupId);
    if (pGroup == NULL)
    {
        return false;
    }

    int nMaxSize = pGroup->m_vChildrenId.size();

    if (nMaxSize <= 0)
    {
        return false;
    }

    LocalDirContactRecord * pMaxContact = Base2LocalContact(GetDataById(pGroup->m_vChildrenId[nMaxSize -
                                                   1]));
    if (pMaxContact != NULL)
    {
        if (nIndex > pMaxContact->m_nFavoriteIndex)
        {
            return false;
        }
    }

    for (int i = 0; i < nMaxSize; ++i)
    {
        int nCurContactID = pGroup->m_vChildrenId[i];
        // 遍历当前Favorite列表联系人
        LocalDirContactRecord * pContact = Base2LocalContact(GetDataById(nCurContactID));

        if (pContact == NULL)
        {
            continue;
        }

        if (pContact->m_nFavoriteIndex != nIndex)
        {
            continue;
        }

        // 当前排除的联系人ID和当前联系人ID一样
        // 表示是同一个联系人的index
        if (nContactID > 0 && nCurContactID == nContactID)
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    return false;
}

DIRECTORY_RET_CODE CLocalContactManager::CopyContactToFavorite(int nContactID,
        int nFavoriteIndex /*= 0*/)
{
    DIR_INFO("CLocalContactManager::CopyContactToGroup [%d] [%d]", nContactID, nFavoriteIndex);

    LocalDirContactRecord * pContact = Base2LocalContact(GetDataById(nContactID));

    if (pContact == NULL)
    {
        DIR_INFO("CLocalContactManager::CopyContactToGroup Contact Null");
        return DIR_RET_FAIL;
    }

    if (IsContactInFavorite(nContactID))
    {
        DIR_INFO("IsContactExistCurrGroup true");
        return DIR_RET_FAVORITE_INDEX_EXIST;
    }

    //外部传递进来的copy才需要判断是否index存在
    if (nFavoriteIndex > 0
            && IsFavoriteIndexExist(nFavoriteIndex))
    {
        DIR_INFO("IsFavoriteIndexExist true");
        return DIR_RET_FAVORITE_INDEX_EXIST;
    }

    int nAddFavoriteIndex = nFavoriteIndex;
    if (nFavoriteIndex <= 0)
    {
        nAddFavoriteIndex = CreatFavoriteIndex();
    }

    //自动生成的Index需要判断是不是等于最大值，因为自动生成如果超过最大值也返回最大值
    if (GetFavoriteMaxIndex() == LOCAL_FAVORITE_INDEX_MAX)
    {
        DIR_INFO("IsFavoriteIndexExist true");
        return DIR_RET_FAVORITE_INDEX_EXIST;
    }

    DIR_INFO("Copy Contact Index : %d", nAddFavoriteIndex);

    if (nAddFavoriteIndex > LOCAL_FAVORITE_INDEX_MAX)
    {
        DIR_INFO("Favorite Max Index [%d]", nAddFavoriteIndex);
        return DIR_RET_FAVORITE_INDEX_MAX;
    }

    // favorite不限制个数 http://10.2.1.199/Bug.php?BugID=123282
    /*DirGroupRecord* pGroup = GetMenuDataById(kszFavoriteGroupId);
    if (pGroup != NULL && pGroup->m_vChildrenId.size() >= 1000)
    {
        DIR_INFO("group size unexpected");
        return DIR_RET_GROUP_SIZE_UNEXPECTED;
    }*/

    pContact->m_nFavoriteIndex = nAddFavoriteIndex;

    AddIdToGroup(nContactID, kszFavoriteGroupId, true);

    SaveContactToFile();

    NotifyFavoriteChange();

    return DIR_RET_SUCC;
}

#if IF_FEATURE_FAVORITE_DIR
bool CLocalContactManager::RemoveContactsIdFromFavorite(const VectorId & vecRemoveId, int nParentID)
{
    for (int i = 0; i < vecRemoveId.size(); ++i)
    {
        int nContactID = vecRemoveId[i];
        LocalDirContactRecord * pContact = Base2LocalContact(GetDataById(nContactID));
        if (pContact != NULL)
        {
            pContact->m_nFavoriteIndex = -1;
        }
    }

    RemoveIdFromGroup(vecRemoveId, kszFavoriteGroupId);

    //当前组为Favorite组，要从搜索组中删除
    if (nParentID == kszFavoriteGroupId)
    {
        RemoveIdFromGroup(vecRemoveId, kszSearchGroupId);
    }

    SaveContactToFile();

    NotifyFavoriteChange();

    return true;
}
#endif // IF_FEATURE_PSTN

void CLocalContactManager::NotifyFavoriteChange()
{
    DIR_INFO("CLocalContactManager::NotifyFavoriteChange ");
    if (!IsFavoriteEnable())
    {
        DIR_INFO("CLocalContactManager::NotifyFavoriteChange Disable");
        return;
    }
    etl_NotifyApp(false, LOCAL_FAVORITE_CHANGED, 0, 0);
}

DIRECTORY_RET_CODE CLocalContactManager::AddIdToGroup(int nId, int nGroupId,
        bool IsSort /* = true */, bool isOrder /* = true */)
{
    if (m_eDirType == LOCAL_DIR && nGroupId == kszFavoriteGroupId)
    {
        DirGroupRecord * pGroup = GetMenuDataById(nGroupId);
        if (pGroup == NULL)
        {
            return DIR_RET_GROUP_UNEXISTED;
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

            LocalDirContactRecord * pRecordMid = Base2LocalContact(GetDataById(pGroup->m_vChildrenId[nMid]));
            LocalDirContactRecord * pRecord = Base2LocalContact(GetDataById(nId));
            int nMidIndex = 0;
            int nIndex = 0;
            if (pRecordMid != NULL)
            {
                nMidIndex = pRecordMid->m_nFavoriteIndex;
            }
            if (pRecord != NULL)
            {
                nIndex = pRecord->m_nFavoriteIndex;
            }

            nCmp = nMidIndex - nIndex;

            if ((nCmp > 0 && isOrder) || (nCmp < 0 && !isOrder))
            {
                nRingt = nMid - 1;
            }
            else if (nCmp == 0)
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
    }
    else
    {
        return CBaseContactManger::AddIdToGroup(nId, nGroupId, IsSort, isOrder);
    }

    return DIR_RET_SUCC;
}

int CLocalContactManager::CreatFavoriteIndex()
{
    // 创建Index 的规则为当前Favorite最大联系人Index加10
    // 超过最大值去最大值
    return yl::min(GetFavoriteMaxIndex() + knIntervalIndexNum,
                   LOCAL_FAVORITE_INDEX_MAX);
}

int CLocalContactManager::GetFavoriteMaxIndex()
{
    DirGroupRecord * pGroup = GetMenuDataById(kszFavoriteGroupId);
    if (pGroup == NULL)
    {
        return 0;
    }

    int nMaxSize = pGroup->m_vChildrenId.size();
    if (nMaxSize <= 0)
    {
        return 0;
    }
    LocalDirContactRecord * pRecord = Base2LocalContact(GetDataById(pGroup->m_vChildrenId[nMaxSize - 1]));
    if (pRecord == NULL)
    {
        return 0;
    }

    return pRecord->m_nFavoriteIndex;
}

bool CLocalContactManager::IsUpdateContactNeedSetLineKey(LocalDirContactRecord * pContact,
        const ContactGeneralItemData & refContactData)
{
    bool bIsNeetAutoSet = false;
    if (pContact == NULL)
    {
        return false;
    }

    if (pContact->m_nFavoriteIndex <= 0 && refContactData.m_nFavoriteIndex <= 0)
    {
        return false;
    }

    if (pContact->m_strDisplayName != refContactData.m_strDisplayName
            || pContact->m_nFavoriteIndex != refContactData.m_nFavoriteIndex
            || pContact->m_nLine != refContactData.m_nLine)
    {
        return true;
    }

    bool bIsFirstNumChange = false;

    yl::string strOffNum;
    pContact->GetNumberByName(kszOfficeNumber, strOffNum);
    if (strOffNum != refContactData.m_strOfficePhoneNumber)
    {
        return true;
    }
    else if (!strOffNum.empty())
    {
        return false;
    }

    yl::string strMobNum;
    pContact->GetNumberByName(kszMobileNumber, strMobNum);
    if (strMobNum != refContactData.m_strMobilePhoneNumber)
    {
        return true;
    }
    else if (!strOffNum.empty())
    {
        return false;
    }

    yl::string strOthNum;
    pContact->GetNumberByName(kszOtherNumber, strOthNum);
    if (strOthNum != refContactData.m_strOtherPhoneNumber)
    {
        return true;
    }
    else if (!strOthNum.empty())
    {
        return false;
    }

    return false;
}

#endif







