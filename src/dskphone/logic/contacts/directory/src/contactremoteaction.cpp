#include "contactremoteaction.h"
#include "threadtask/include/common.h"
#include "remotecontactmanager.h"
#include "basemethod.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "commonunits/modcommonunits.h"

// 远程菜单联系人XML入口的根节点.
#define kszRMXmlMenuRootName  "IPPhoneMenu"
// 远程具体联系人XML入口的根节点.
#define kszRMXmlUnitRootName  "IPPhoneDirectory"
// 远程混合联系人XML入口的根节点.
#define kszRMXmlMixedRootName  "IPPhoneBook"
// 远程Text联系人XML人口的根节点
#define kszRMXmlTextRootName "IPPhoneText"
//远程本地联系人格式xml入口的根节点
#define kszRMXmlLocalNodeCRoot  "root_contact"

#define kxzRMXmlLcalNodeGRoot "root_group"

#define kszNodePageKey "MenuItem"

#define kszV50NodeRoot  "vp_contact"
#define kszV50NodeGRoot  "root_group"  // 组根节点.
#define kszV50NodeG  "group"  // 组节点.
#define kszV50NodeCRoot  "root_contact"  // 成员根节点.
#define kszV50NodeC  "contact"  // 成员节点.
#define kszV50AttrGName  "display_name"  // 组显示名.
#define kszV50AttrGRing  "ring"  // 组铃声.
#define kszV50AttrCName  "display_name"  // 成员显示名.
#define kszV50AttrCOfficeNumb  "office_number"  // 办公室电话.
#define kszV50AttrCMobileNumb  "mobile_number"  // 移动电话.
#define kszV50AttrCOtherNumb  "other_number"  // 其他号码.
#define kszV50AttrCLine  "line"  // 帐号.
#define kszV50AttrCRing  "ring"  // 铃声.
#define kszV50AttrCGroup  "group_id_name"  // 组id名.
#define kszV50AttrCDefPhoto  "default_photo"  // 默认图片.
#define kszV50AttrCSelPhoto  "selected_photo"  // 选择的图片.
#define kszV50AttrCFavorate  "is_favorite"  // 是否为搜藏夹联系人.

#define kszT2XMaxFileSize (512+256)*1024
#define kszT4XMaxFileSize (1024+512)*1024

#if (20 != PHONE_TYPE)

yl::string QueryAttribute(xml_node TmpNode, const char * szAttribute, const char * szDefault = "")
{
    yl::string strRet(szDefault);
    if (!TmpNode.empty())
    {
        xml_attribute pAttrName = TmpNode.attribute(szAttribute);
        if (!pAttrName.empty())
        {
            strRet.assign(pAttrName.value(), strlen(pAttrName.value()));
        }
    }
    return strRet;
}

bool RemoteComp(BaseDirDataRecord * lpData, BaseDirDataRecord * rpData)
{
    if (lpData == NULL || rpData == NULL)
    {
        return false;
    }
    if (lpData->m_eInheritType != rpData->m_eInheritType)
    {
        return lpData->m_eInheritType < rpData->m_eInheritType;
    }

    int nLeft = GetFirstCharSortValue(lpData->m_strDisplayName);
    int nRight = GetFirstCharSortValue(rpData->m_strDisplayName);

    if (nLeft == nRight)
    {
        return strcasecmp(lpData->m_strDisplayName.c_str(), rpData->m_strDisplayName.c_str()) < 0;
    }
    else
    {
        return nLeft < nRight;
    }
}

bool RemoteCompNotSort(BaseDirDataRecord * lpData, BaseDirDataRecord * rpData)
{
    if (lpData == NULL || rpData == NULL)
    {
        return false;
    }
    if (lpData->m_eInheritType != rpData->m_eInheritType)
    {
        return lpData->m_eInheritType < rpData->m_eInheritType;
    }


    return true;
}

//////////////////////////////////////////////////////////////////////////
// Class CContactRemoteAction.
CContactRemoteAction::CContactRemoteAction(const yl::string & strLocalFilePath,
        const yl::string & strRemoteFilePath)
    : CNetworkFileAction(strLocalFilePath, strRemoteFilePath)
{
    m_nGroupId = knInvalidRId;
    m_strTitle.clear();

    m_bNeedReleaseData = true;

    m_pNextMenu = NULL;
}

CContactRemoteAction::~CContactRemoteAction()
{
    DIR_INFO("CContactRemoteAction::~CContactRemoteAction [%d]", m_bNeedReleaseData);
    if (m_bNeedReleaseData)
    {
        ReleaseData();
    }
    remove(GetFilePath().c_str());
}

bool CContactRemoteAction::Exec()
{
    DIR_INFO("CContactRemoteAction::Exec()");
    // 调用基类进行下载.
    CNetworkFileAction::Exec();

    bool isSucc = false;
    // 如果下载成功, 则解析.
    if (IsRead() && IsOperationSucceed())
    {
        isSucc = ParseDownloadedFile(GetFilePath());
        SetTaskActionErrorType(isSucc ? NS_TA::TA_NO_ERROR : NS_TA::TA_OTHER_ERROR);
    }

    //SetTaskActionErrorType(isSucc ? NS_TA::TA_NO_ERROR : NS_TA::TA_OTHER_ERROR);
    return isSucc;
}

void CContactRemoteAction::SetGroupId(int nId)
{
    m_nGroupId = nId;
}

void CContactRemoteAction::ReleaseData()
{
    for (ListData::iterator iter = m_listData.begin(); iter != m_listData.end(); iter++)
    {
        BaseDirDataRecord * pData = *iter;

        if (pData != NULL)
        {
            delete pData;
        }
    }

    if (m_pNextMenu != NULL)
    {
        delete m_pNextMenu;
    }
}

bool CContactRemoteAction::ParseDownloadedFile(const yl::string & strFileLocal)
{
    DIR_INFO("CContactRemoteAction::ParseDownloadedFile");
    // 解析XML文件第一个根节点, 以决定采用何种解析方式.
    bool isSucc = false;

    // xml文件大小限制
    int iFileSize = pathFileSize(strFileLocal.c_str());

    int nMaxFileSize = GetDirMaxDownloadSize();

    DIR_INFO("CContactRemoteAction::ParseDownloadedFile nFileSize [%d] MaxSize [%d]", iFileSize,
             nMaxFileSize);

    if (iFileSize > nMaxFileSize)
    {
        return false;
    }

    xml_document docParse;
    if (!docParse.load_file(strFileLocal.c_str()))
    {
        DIR_INFO("CContactRemoteManager::MyLoadDataFromFile Unsupported format[%s]!",
                 strFileLocal.c_str());
        return false;
    }

#if IF_BUG_PLCMDIR
    if (PrasePlcmDir(docParse))
    {
        Sort(m_listData, RemoteComp);

        return true;
    }
#endif

    // 循环查找节点.
    yl::string strNodeName;
    xml_node node = docParse.first_child();

    // 当网页重新请求更新时，中断上一次更新操作
    while (node)
    {
        strNodeName = node.name();

        size_t nMenuMaskLen = strlen(kszRMXmlMenuRootName);
        if (strNodeName.length() >= nMenuMaskLen
                && strNodeName.substr(strNodeName.length() - nMenuMaskLen) == kszRMXmlMenuRootName)
        {
            // 远程菜单联系人.
            isSucc = MenuParse(node);
            break;
        }

        size_t nUnitMaskLen = strlen(kszRMXmlUnitRootName);
        if (strNodeName.length() >= nUnitMaskLen
                && strNodeName.substr(strNodeName.length() - nUnitMaskLen) == kszRMXmlUnitRootName)
        {
            // 远程具体联系人.
            isSucc = UnitParse(node);
            break;
        }

        size_t nMixedMaskLen = strlen(kszRMXmlMixedRootName);
        if (strNodeName.length() >= nMixedMaskLen
                && strNodeName.substr(strNodeName.length() - nMixedMaskLen) == kszRMXmlMixedRootName)
        {
            // 混合联系人.
            isSucc = MixParse(node);
            break;
        }

        size_t nTextMaskLen = strlen(kszRMXmlTextRootName);
        if (strNodeName.length() >= nTextMaskLen
                && strNodeName.substr(strNodeName.length() - nTextMaskLen) == kszRMXmlTextRootName)
        {
            // Text文本.
            isSucc = true;
            break;
        }

        if (kszV50NodeRoot == strNodeName
                || kszV50NodeCRoot == strNodeName
                || kszV50NodeGRoot == strNodeName)
        {
            //暂不开放本地解析的功能
            //isSucc = LocalParse(docParse);
            break;
        }

        node = node.next_sibling();
    }  // while

    if (isSucc)
    {
        if (configParser_GetConfigInt(kszNotLocalDirSortEnable) == 1)
        {
            Sort(m_listData, RemoteComp);
        }
        else
        {
            Sort(m_listData, RemoteCompNotSort);
        }
    }

    return isSucc;
}

// 远程菜单联系人的Xml定义为如下形式(其中XXX可为任意字符, 也可没有):
// <XXXIPPhoneMenu>
//     <Title>Menu</Title>
//     <MenuItem>
//         <Name>Group-0</Name>
//         <URL>http://10.1.0.87:280/RemoteDirGroup.xml</URL>
//     </MenuItem>
// </XXXIPPhoneMenu>
bool CContactRemoteAction::MenuParse(xml_node & NodeRoot)
{
    // MenuItem结点.
#define kszNodeNameMenu "MenuItem"
    // SoftKey节点.
#define kszNodeNameSoftKey "SoftKeyItem"
    // Title节点.
#define kszNodeNameTitle "Title"

    // 查找MenuItem结点.
    yl::string strNodeName;  // 节点名.
    for (xml_node NodeChild = NodeRoot.first_child();
            !NodeChild.empty();
            NodeChild = NodeChild.next_sibling())
    {
        strNodeName.assign(NodeChild.name(), strlen(NodeChild.name()));
        if (strNodeName == kszNodeNameMenu)
        {
            // 解析MenuItem结点.

            ContactRemoteMenu * pMenu = new ContactRemoteMenu();
            if (pMenu != NULL)
            {
                pMenu->m_nParentId = m_nGroupId;
                pMenu->ParseFromXMLNode(NodeChild);//解析联系人

                m_listData.push_back(pMenu);
            }
        }
        else if (strNodeName == kszNodeNameSoftKey)
        {
            // 解析SoftKey节点.
            ContactRemoteHotKey * pHotKey = new ContactRemoteHotKey();
            if (pHotKey != NULL)
            {
                pHotKey->m_nParentId = m_nGroupId;
                pHotKey->ParseFromXMLNode(NodeChild);//解析联系人
                m_listData.push_back(pHotKey);
            }
        }
        else if (strNodeName == kszNodeNameTitle)
        {
            m_strTitle.assign(NodeChild.child_value(), strlen(NodeChild.child_value()));
        }
    }  // for.
    return true;
}
// 远程具体联系人的Xml定义为如下形式(其中XXX可为任意字符, 也可没有):
// <XXXIPPhoneDirectory>
//     <Title>Yealink</Title>
//     <DirectoryEntry>
//         <Name>Armando</Name>
//         <Telephone>312</Telephone>
//         <Telephone>8845279</Telephone>
//         <Telephone>13498264126</Telephone>
//     </DirectoryEntry>
// </XXXIPPhoneDirectory>
bool CContactRemoteAction::UnitParse(xml_node & NodeRoot)
{
    // DirectoryEntry结点.
#define kszNodeNameUnit "DirectoryEntry"
    // SoftKey节点.
#define kszNodeNameSoftKey "SoftKeyItem"
    // Title节点.
#define kszNodeNameTitle "Title"

#define kszRemoteContactNodeItem "Item"
#define kszNodeNameUrl "URL"

    // 查找XML中各个具体联系人节点.
    yl::string strNodeName;  // 节点名.
    for (xml_node NodeChild = NodeRoot.first_child();
            !NodeChild.empty();
            NodeChild = NodeChild.next_sibling())
    {
        strNodeName.assign(NodeChild.name(), strlen(NodeChild.name()));

        if (strNodeName == kszNodeNameUnit)
        {
            ContactRemoteUnit * pUnit = new ContactRemoteUnit();
            if (pUnit != NULL)
            {
                pUnit->m_nParentId = m_nGroupId;
                pUnit->ParseFromXMLNode(NodeChild);//解析联系人
                m_listData.push_back(pUnit);
            }
        }
        else if (strNodeName == kszNodeNameSoftKey)
        {
            // 解析HotKey节点.
            // 解析SoftKey节点.
            ContactRemoteHotKey * pHotKey = new ContactRemoteHotKey();

            if (pHotKey != NULL)
            {
                pHotKey->m_nParentId = m_nGroupId;
                pHotKey->ParseFromXMLNode(NodeChild);//解析联系人
                m_listData.push_back(pHotKey);
            }
        }
        else if (strNodeName == kszNodePageKey)
        {
            yl::string strDisplayName;
            yl::string strUrl;

            xml_node NodeName = NodeChild.child(kszRemoteContactNodeItem);
            if (!NodeName.empty())
            {
                strDisplayName = NodeName.child_value();

                if (strDisplayName.compare("next-page") != 0)
                {
                    continue;
                }
            }

            // 读取URL地址.
            xml_node NodeUrl = NodeChild.child(kszRemoteNodeUrl);
            if (!NodeUrl.empty())
            {
                strUrl.assign(NodeUrl.child_value(), strlen(NodeUrl.child_value()));
            }

            if (m_pNextMenu == NULL && !strUrl.empty())
            {
                m_pNextMenu = new ContactRemoteMenu();

                m_pNextMenu->m_strDisplayName = strDisplayName;
                m_pNextMenu->m_strURL = strUrl;
                m_pNextMenu->m_nprevPageId = m_nGroupId;
            }
        }
        else if (strNodeName == kszNodeNameTitle)
        {
            m_strTitle.assign(NodeChild.child_value(), strlen(NodeChild.child_value()));
        }
    }  // for.
    return true;
}
// 其格式如下(其中XXX可为任意字符, 也可没有):
// <XXXIPPhoneBook>
//     <Title>Yealink</Title>
//     <Menu Name="Group-0">
//         <Unit Name="Roland" Phone1="12345" Phone2="000022" Phone3="230000" />
//         <Unit Name="Qt" Phone1="10000" Phone2="13000001222" Phone3="8820123" />
//     </Menu>
// </XXXIPPhoneBook>
bool CContactRemoteAction::MixParse(xml_node & NodeRoot)
{
#define kszNodeMenu "Menu"  // 菜单节点名.
#define kszNodeUnit "Unit"  // 具体联系人节点名.

    ContactRemoteMenu CRMData;  // 远程菜单联系人数据.
    ContactRemoteUnit CRUData;  // 远程具体联系人数据.
    // 首先查找菜单联系人.
    for (xml_node NodeMenu = NodeRoot.child(kszNodeMenu);
            !NodeMenu.empty();
            NodeMenu = NodeMenu.next_sibling(kszNodeMenu))
    {
        ContactRemoteMenu * pMenu = new ContactRemoteMenu();
        if (pMenu != NULL)
        {
            pMenu->m_nParentId = m_nGroupId;
            pMenu->m_nId = g_pRemoteDirManager->GetId();
            pMenu->ParseFromXMLNode(NodeMenu);//解析联系人
            m_listData.push_back(pMenu);
        }

        xml_node NodeChild;
        for (NodeChild = NodeMenu.child(kszNodeUnit);
                !NodeChild.empty();
                NodeChild = NodeChild.next_sibling(kszNodeUnit))
        {
            ContactRemoteUnit * pUnit = new ContactRemoteUnit();
            if (pUnit != NULL)
            {
                pUnit->m_nParentId = pMenu->m_nId;

                pUnit->ParseFromXMLNode(NodeChild);//解析联系人
                m_listData.push_back(pUnit);
            }
        }  // for.

        pMenu->m_eStatus = FILE_DOWNLOADED_OK;
    }  // for.

    return true;
}

bool CContactRemoteAction::LocalParse(xml_node & docParse)
{
    xml_node NodeRootFirst = docParse.child(kszV50NodeRoot);
    xml_node NodeG = docParse.child(kszV50NodeGRoot);
    if (NodeG.empty() && !NodeRootFirst.empty())
    {
        // 从转换工具生成的格式.
        NodeG = NodeRootFirst.child(kszV50NodeGRoot);
    }
    YLVector<ContactGroupInfo> vecGroup;
    bool nParseResult = ParseLocalGroup(NodeG, vecGroup);
//  if (nParseResult)
//  {
    xml_node TmpNodeC = docParse.child(kszV50NodeCRoot);
    if (TmpNodeC.empty() && !NodeRootFirst.empty())
    {
        // 从转换工具生成的格式.
        TmpNodeC = NodeRootFirst.child(kszV50NodeCRoot);
    }
    nParseResult = ParseLocalContact(TmpNodeC, vecGroup);
//  }
    return nParseResult;
}

bool CContactRemoteAction::ParseLocalGroup(xml_node GRoot, YLVector<ContactGroupInfo> & vecGroup)
{
    if (GRoot.empty())
    {
        return true;
    }

    vecGroup.clear();
    for (xml_node NodeG = GRoot.child(kszV50NodeG);
            !NodeG.empty();
            NodeG = NodeG.next_sibling(kszV50NodeG))
    {
        LocalDirGroupRecord * pGroup = new LocalDirGroupRecord();
        if (NULL != pGroup)
        {
            // 读取各个组属性.
            pGroup->m_nParentId = m_nGroupId;
            pGroup->m_strDisplayName = QueryAttribute(NodeG, kszV50AttrGName);
            pGroup->m_strRing = QueryAttribute(NodeG, kszV50AttrGRing);
            pGroup->m_nId = g_pRemoteDirManager->GetId();
            pGroup->m_eType = DIR_TYPE_GROUP;
            pGroup->m_eStatus = FILE_DOWNLOADED_OK;
            commonUnits_Hide2Real(pGroup->m_strRing, PHFILE_TYPE_RING_FILE);

            //vecGroup 在解析具体联系人时用来判断组是否已存在
            ContactGroupInfo groupInfo;
            groupInfo.m_strDisplayName = pGroup->m_strDisplayName;
            groupInfo.m_ID = pGroup->m_nId;
            vecGroup.push_back(groupInfo);

            m_listData.push_back(pGroup);

        }
    }  // for.

    return true;
}

#if IF_BUG_PLCMDIR
bool CContactRemoteAction::PrasePlcmDir(xml_document & docParse)
{
#define kszplcmDirRoot "directory"
#define kszItemListNode "item_list"
#define kszItmePlcmNode "item"
#define kszPlcmDirLastName "ln"
#define kszPlcmDirFirstName "fn"
#define kszPlcmDirNumber "ct"


    // 循环查找节点.
    xml_node nodeList = docParse.child(kszplcmDirRoot).child(kszItemListNode);

    if (nodeList.empty())
    {
        DIR_WARN("Item List Node Not Found!");
        return false;
    }

    bool suc = false;

    for (xml_node node = nodeList.child(kszItmePlcmNode); !node.empty();
            node = node.next_sibling(kszItmePlcmNode))
    {
        ContactRemoteUnit * pUnit = new ContactRemoteUnit();

        pUnit->m_nParentId = m_nGroupId;

        if (strcmp(node.name(), "item") == 0)
        {
            xml_node firstNameNode = node.child(kszPlcmDirFirstName);

            if (!firstNameNode.empty())
            {
                pUnit->m_strDisplayName = firstNameNode.child_value();
            }

            xml_node lastNameNode = node.child(kszPlcmDirLastName);
            if (!lastNameNode.empty())
            {
                if (!pUnit->m_strDisplayName.empty())
                {
                    pUnit->m_strDisplayName.append(" ");
                }
                pUnit->m_strDisplayName.append(lastNameNode.child_value());
            }

            xml_node numberNode = node.child(kszPlcmDirNumber);

            if (!numberNode.empty())
            {
                stNumber stNum;
                stNum.strName = "Telephone";
                stNum.strValue.assign(numberNode.child_value(), strlen(numberNode.child_value()));

                pUnit->m_listNumber.push_back(stNum);
            }
        }

        if (!pUnit->m_strDisplayName.empty())
        {
            pUnit->m_strNameIndex = commonAPI_transT9String(pUnit->m_strDisplayName);

            m_listData.push_back(pUnit);

            suc = true;
        }
        else
        {
            delete pUnit;
        }
    }

    return suc;
}
#endif

bool CContactRemoteAction::ParseLocalContact(xml_node CRoot, YLVector<ContactGroupInfo> & vecGroup)
{
    if (CRoot.empty())
    {
        return true;
    }


    for (xml_node NodeC = CRoot.child(kszV50NodeC);
            !NodeC.empty();
            NodeC = NodeC.next_sibling(kszV50NodeC))
    {
        LocalDirContactRecord * pNew = new LocalDirContactRecord();
        if (NULL == pNew)
        {
            return 0;
        }

        pNew->m_strDisplayName = QueryAttribute(NodeC, kszV50AttrCName);
        if (pNew->m_strDisplayName.empty())
        {
            continue;
        }

        stNumber stNum;
        stNum.strName = kszOfficeNumber;
        stNum.strValue = QueryAttribute(NodeC, kszV50AttrCOfficeNumb);
        pNew->m_listNumber.push_back(stNum);

        stNum.strName = kszMobileNumber;
        stNum.strValue = QueryAttribute(NodeC, kszV50AttrCMobileNumb);
        pNew->m_listNumber.push_back(stNum);


        stNum.strName = kszOtherNumber;
        stNum.strValue = QueryAttribute(NodeC, kszV50AttrCOtherNumb);
        pNew->m_listNumber.push_back(stNum);


        pNew->m_strRing = QueryAttribute(NodeC, kszV50AttrCRing);
        commonUnits_Hide2Real(pNew->m_strRing, PHFILE_TYPE_RING_FILE);
        // 添加line属性
        yl::string strTempLine = QueryAttribute(NodeC, kszV50AttrCLine);
        if (!strTempLine.empty()
                && atoi(strTempLine.c_str()) >= 0)
        {
            pNew->m_nLine = atoi(strTempLine.c_str());
        }
        else
        {
            pNew->m_nLine = -1;
        }

        if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
        {
            pNew->m_strDefaultPhoto = QueryAttribute(NodeC, kszV50AttrCDefPhoto);
        }

        // 导入成员的组信息, 如果组不存在, 则创建.
        // 获取组信息.
        yl::string strGroupIDName = QueryAttribute(NodeC, kszV50AttrCGroup);

        int nGroupId;

        if (strGroupIDName.empty())
        {
            strGroupIDName = kszGroupNameAll;
            nGroupId = kszAllContactId;
        }
        else
        {

            nGroupId = FindLocalGroup(vecGroup, strGroupIDName);
            if (nGroupId == knInvalidRId)
            {
                // 组不存在, 则创建组.
                LocalDirGroupRecord * pGroup = new LocalDirGroupRecord();
                pGroup->m_nParentId = m_nGroupId;
                pGroup->m_strDisplayName = QueryAttribute(NodeC, kszV50AttrCGroup);
                pGroup->m_strRing = QueryAttribute(NodeC, kszV50AttrGRing);
                pGroup->m_nId = g_pRemoteDirManager->GetId();
                nGroupId = pGroup->m_nId;
                pGroup->m_eType = DIR_TYPE_GROUP;
                pGroup->m_eStatus = FILE_DOWNLOADED_OK;
                commonUnits_Hide2Real(pGroup->m_strRing, PHFILE_TYPE_RING_FILE);


                ContactGroupInfo groupInfo;
                groupInfo.m_strDisplayName = pGroup->m_strDisplayName;
                groupInfo.m_ID = pGroup->m_nId;
                vecGroup.push_back(groupInfo);
                m_listData.push_back(pGroup);

                //创建组不成功，则统一加入all组
                if (nGroupId < 0)
                {
                    nGroupId = kszAllContactId;
                }
            }
        }
        pNew->m_nParentId = nGroupId;
        pNew->m_nId = g_pRemoteDirManager->GetId();
        pNew->m_eType = DIR_TYPE_LOCAL_CONTACT;
        m_listData.push_back(pNew);
    }  // for.

    return true;
}

void CContactRemoteAction::SetNeedReleaseData(bool bRelease)
{
    m_bNeedReleaseData = bRelease;
}

int CContactRemoteAction::FindLocalGroup(YLVector<ContactGroupInfo> & vecGroup,
        yl::string & strGroupName)
{
    YLVector<ContactGroupInfo>::iterator iter;
    for (iter = vecGroup.begin(); iter != vecGroup.end(); iter++)
    {
        if (strGroupName == iter->m_strDisplayName)
        {
            return iter->m_ID;
            break;
        }
    }

    return knInvalidRId;
}


#endif // (20 != PHONE_TYPE)

