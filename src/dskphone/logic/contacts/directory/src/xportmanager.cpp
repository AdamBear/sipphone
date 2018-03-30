#include "xportmanager.h"
#include "ETLLib.hpp"
#include "interfacedefine.h"

#include "localcontactmanager.h"
#include "directorystructure.h"
#include "commonunits/modcommonunits.h"
#include "dircommonfunc.h"
#include "ylstringalgorithm.h"


// 以下为XML文件定义.
#define kszT3XNodeRoot  "contactData"  // 根节点.
#define kszT3XNodeGList  "groupinfo"  // 组列表的节点.
#define kszT3XNodeG  "group"  // 组的节点.
#define kszT3XNodeCList  "group"  // 一般联系人的节点列表.
#define kszT3XNodeC  "contact"  // 一般联系人的节点.
#define kszT3XNodeBList  "blacklist"  // 黑名单联系人的节点列表.
#define kszT3XNodeB  "contact"  // 黑名单联系人的节点.
#define kszT3XAttrCName  "sDisplayName"  // 联系人的显示名.
#define kszT3XAttrCOfficeNumb  "sOfficeNumber"  // 联系人的办公室电话.
#define kszT3XAttrCMoileNumb  "sMobilNumber"  // 联系人的移动电话.
#define kszT3XAttrCOtherNumb  "sOtherNumber"  // 联系人的其他电话.
#define kszT3XAttrCRing  "sRing"  // 联系人的铃声.
#define kszT3XAttrCGroup  "group"  // 联系人的组名.
#define kszT3XAttrCDefPhoto  "photoDefault"  // 联系人的默认图片.
#define kszT3XAttrCSelPhoto  "photoSelect"  // 联系人选中的图片.
#define kszT3XAttrGName  "name"  // 组的显示名.
#define kszT3XAttrGRing  "Ring"  // 组的铃声.
#define kszT3XAttrCLine  "sLine"  // 联系人账号

#define kszV40NodeRoot  "contactData"  // 根节点.
#define kszV40NodeG  "group"  // 组节点.
#define kszV40NodeC  "contact"  // 联系人节点.
#define kszV40AttrCName  "sLocalRename"  // 联系人显示名.
#define kszV40AttrCOfficeNumb  "sOfficeNumber"  // 办公室电话.
#define kszV40AttrCMobileNumb  "sMobilNumber"  // 移动电话.
#define kszV40AttrCOtherNumb  "sOtherNumber"  // 其他号码.
#define kszV40AttrCRing  "sRingFile"  // 联系人铃声.
#define kszV40AttrCImage  "sImagePath"  // 联系人头像.
#define kszV40AttrGName  "sGroupName"  // 组显示名.
#define kszV40AttrGRing  "sGroupRing"  // 组铃声.

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
#define kszV50AttrCFavoriteIndex "priority"
#define kszV50AttrDivert   "auto_divert"

#define kszBlackListRoot    "root_blacklist"   //导入的黑名单根节点
#define kszBlackListItem    "item"
#define kszBlackListNumber  "number"

#define knMaxBufferLen  260

// 联系人导出的Xml文件名.
#define kszXportXmlFile  "contact_xported.xml"

// 第一级根节点, 从配置文件转换工具转换过来的V50配置文件会存在一个
// 第一级根节点vp_contact, 在此需要兼容此种格式.
#define kszV50NodeRoot  "vp_contact"

static yl::string QueryAttribute(xml_node TmpNode,
                                 const char * szAttribute,
                                 const char * szDefault = "")
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

// 获取默认的导出或导入路径.
static void GetDefaultPath(yl::string & strPath)
{
    strPath = g_strConfigPath;
    strPath.append(kszXportXmlFile);
}

//////////////////////////////////////////////////////////////////////////
// Class CXPortBase.
CXPortBase::CXPortBase()
{

}

CXPortBase::~CXPortBase()
{

}

int CXPortBase::Import(const char * szPath, bool isAppend)
{
    yl::string strPath = "";
    if (NULL == szPath || szPath == 0 || szPath[0] == '\0')
    {
        GetDefaultPath(strPath);
    }
    else
    {
        strPath = szPath;
    }

    int nCheckImported = 0;

    // 导入.
    BeforeImport(isAppend);
    nCheckImported = this->Import(strPath.c_str());
    AfterImport(true);


    return nCheckImported;
}

int CXPortBase::Export()
{
    return g_pLocalDirManager->ExportContactAndGroup()
           ? IMPORT_NO_ERROR : IMPORT_INVALID_FILE;
}

bool CXPortBase::BeforeImport(bool isAppend)
{
    // 如果不是追加, 则清空以前的记录.

    return g_pLocalDirManager->BeforeImport(isAppend);
}

bool CXPortBase::AfterImport(bool bNeedUpload /*= true*/)
{
    // 保存组和联系人.
    return g_pLocalDirManager->AfterImport(bNeedUpload);
}

//////////////////////////////////////////////////////////////////////////
// Class CXPortT3X
CXPortT3X::CXPortT3X()
{

}

CXPortT3X::~CXPortT3X()
{

}

int CXPortT3X::Import(const char * szPath)
{
    // 首先加载组.
#ifndef IF_NO_EXCEPTION
    try
#endif // IF_NO_EXCEPTION
    {
        // 加载内容到内存.

        xml_document fdoc;
        fdoc.load_file(szPath);

        // 读取根节点信息.
        xml_node pNodeRoot = fdoc.child(kszT3XNodeRoot);
        if (pNodeRoot.empty())
        {
            return IMPORT_INVALID_FILE;
        }

        // 导入组.
        MyImportGroup(pNodeRoot);
        // 导入成员.
        MyImportContact(pNodeRoot);

        return IMPORT_NO_ERROR;
    }
#ifndef IF_NO_EXCEPTION
    catch (...)
    {
        DIR_INFO("CXPortT3X::Import, invalid file (%s)", szPath);
        return IMPORT_INVALID_FILE;
    }
#endif // IF_NO_EXCEPTION

    return IMPORT_INVALID_FILE;
}

int CXPortT3X::MyImportGroup(xml_node Root)
{
    if (Root.empty())
    {
        return IMPORT_INVALID_FILE;
    }

    // 获取组列表节点.
    xml_node pNodeGList = Root.child(kszT3XNodeGList);
    if (!pNodeGList.empty())
    {
        // 导入各个组.
        for (xml_node NodeG = pNodeGList.child(kszT3XNodeG);
                !NodeG.empty();
                NodeG = NodeG.next_sibling(kszT3XNodeG))
        {
            ContactGroupData stGD;

            // 获取组名.
            stGD.m_strDisplayName = QueryAttribute(NodeG, kszT3XAttrGName);
            // 获取组的铃声.
            stGD.m_strRing = QueryAttribute(NodeG, kszT3XAttrGRing);

            commonUnits_Hide2Real(stGD.m_strRing, PHFILE_TYPE_RING_FILE);

            g_pLocalDirManager->AddGroup(stGD, true);
        }  // for.
    }

    return IMPORT_NO_ERROR;
}

int CXPortT3X::PraseContact(xml_node node, ContactGeneralItemData & stID,
                            int nGroupId/* = knInvalidRId*/)
{
    stID.m_strDisplayName = QueryAttribute(node, kszT3XAttrCName);
    if (stID.m_strDisplayName.empty())
    {
        return knInvalidRId;
    }

    stID.m_strOfficePhoneNumber = QueryAttribute(node, kszT3XAttrCOfficeNumb);
    stID.m_strMobilePhoneNumber = QueryAttribute(node, kszT3XAttrCMoileNumb);
    stID.m_strOtherPhoneNumber = QueryAttribute(node, kszT3XAttrCOtherNumb);

    // 解析铃声.
    stID.m_strRing = QueryAttribute(node, kszT3XAttrCRing);
    yl::trim_both(stID.m_strRing, " \t");

    //TODO铃声转化
    commonUnits_Hide2Real(stID.m_strRing, PHFILE_TYPE_RING_FILE);

    // 解析账号
    yl::string strLine = QueryAttribute(node, kszT3XAttrCLine, "0");
    yl::trim_both(strLine, " \t");
    stID.m_nLine = atoi(strLine.c_str());

    // 获取组信息.
    stID.m_strGroupName = QueryAttribute(node, kszT3XAttrCGroup);

    if (nGroupId == knInvalidRId)
    {
        if (stID.m_strGroupName.empty())
        {
            stID.m_strGroupName = kszGroupNameAll;
            nGroupId = kszAllContactId;
        }
        else
        {
            nGroupId = g_pLocalDirManager->GetGroupIdByGroupName(stID.m_strGroupName);
            if (nGroupId == knInvalidRId)
            {
                ContactGroupData stGD;
                // 组不存在, 则创建组.
                stGD.m_strDisplayName = stID.m_strGroupName;
                stGD.m_strRing = "";
                nGroupId = g_pLocalDirManager->AddGroup(stGD, true);
            }
        }
    }

    return nGroupId;
}

int CXPortT3X::MyImportContact(xml_node Root)
{
    if (Root.empty())
    {
        return IMPORT_INVALID_FILE;
    }

    // 获取成员列表节点.
    xml_node nodeGroup = Root.child(kszT3XNodeCList);
    if (!nodeGroup.empty())
    {
        ContactGeneralItemData stID;
        for (xml_node TmpNodeC = nodeGroup.child(kszT3XNodeC);
                !TmpNodeC.empty();
                TmpNodeC = TmpNodeC.next_sibling(kszT3XNodeC))
        {
            stID.Reset();
            // 获取联系人信息.
            int nGroupId = PraseContact(TmpNodeC, stID);

            if (nGroupId  >= 0)
            {
                g_pLocalDirManager->AddLocalContact(stID, nGroupId, false);
            }
        }  // for.
    }

    nodeGroup = Root.child(kszT3XNodeBList);
    if (!nodeGroup.empty())
    {
        ContactGeneralItemData stID;
        for (xml_node TmpNodeC = nodeGroup.child(kszT3XNodeC);
                !TmpNodeC.empty();
                TmpNodeC = TmpNodeC.next_sibling(kszT3XNodeC))
        {
            stID.Reset();
            // 获取联系人信息.
            int nGroupId = PraseContact(TmpNodeC, stID, kszBlackListGroupId);

            if (nGroupId  >= 0)
            {
                g_pLocalDirManager->AddLocalContact(stID, nGroupId, false);
            }
        }  // for.
    }

    return IMPORT_NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////
// Class CXPortV40.
CXPortV40::CXPortV40()
{

}

CXPortV40::~CXPortV40()
{

}

int CXPortV40::Import(const char * szPath)
{
    if (szPath == NULL || szPath[0] == '\0')
    {
        return IMPORT_INVALID_FILE;
    }
    xml_document docImport;
    docImport.load_file(szPath);

    xml_node NodeRoot = docImport.child(kszV40NodeRoot);
    if (NodeRoot.empty())
    {
        return IMPORT_INVALID_FILE;
    }

    ContactGroupData stGD;
    for (xml_node TmpNodeG = NodeRoot.child(kszV40NodeG);
            !TmpNodeG.empty();
            TmpNodeG = TmpNodeG.next_sibling(kszV40NodeG))
    {
        // 获取组名.
        stGD.m_strDisplayName = QueryAttribute(TmpNodeG, kszV40AttrGName);
        if (stGD.m_strDisplayName == "Anonymous")
        {
            // 导入组下面的成员.
            MyImportContact(TmpNodeG, kszAllContactId, kszGroupNameAll);
        }
        else if (stGD.m_strDisplayName == "Black List")
        {
            // 导入组下面的成员.
            MyImportContact(TmpNodeG, kszBlackListGroupId, kszGroupNameBlackList);
        }
        else
        {
            stGD.m_strRing = QueryAttribute(TmpNodeG, kszV40AttrGRing);

            yl::string strGroupName = stGD.m_strDisplayName;
            commonUnits_Hide2Real(stGD.m_strRing, PHFILE_TYPE_RING_FILE);

            int nGroupId;

            if (strGroupName.empty())
            {
                nGroupId = kszAllContactId;
            }
            else
            {
                nGroupId = g_pLocalDirManager->GetGroupIdByGroupName(strGroupName);
                if (nGroupId == knInvalidRId)
                {
                    ContactGroupData stNewGD;
                    // 组不存在, 则创建组.
                    stNewGD.m_strDisplayName = strGroupName;
                    stNewGD.m_strRing = "";
                    nGroupId = g_pLocalDirManager->AddGroup(stNewGD, true);
                }
            }


            if (nGroupId < 0)
            {
                nGroupId = kszAllContactId;
            }

            // 导入组下面的成员.
            MyImportContact(TmpNodeG, nGroupId, strGroupName);
        }  // for.
    }

    return IMPORT_NO_ERROR;
}

int CXPortV40::MyImportContact(xml_node GRoot, int nGroupId, const yl::string & strGroupName)
{
    if (GRoot.empty())
    {
        return IMPORT_INVALID_FILE;
    }

    for (xml_node TmpNodeC = GRoot.child(kszV40NodeC);
            !TmpNodeC.empty();
            TmpNodeC = TmpNodeC.next_sibling(kszV40NodeC))
    {
        // 遍历成员节点.
        ContactGeneralItemData stID;
        stID.m_strDisplayName = QueryAttribute(TmpNodeC, kszV40AttrCName);
        if (stID.m_strDisplayName.empty())
        {
            continue;
        }

        // 获取成员各个属性.
        stID.m_strOfficePhoneNumber = QueryAttribute(TmpNodeC, kszV40AttrCOfficeNumb);
        stID.m_strMobilePhoneNumber = QueryAttribute(TmpNodeC, kszV40AttrCMobileNumb);
        stID.m_strOtherPhoneNumber = QueryAttribute(TmpNodeC, kszV40AttrCOtherNumb);
        stID.m_strRing = QueryAttribute(TmpNodeC, kszV40AttrCRing);
        commonUnits_Hide2Real(stID.m_strRing, PHFILE_TYPE_RING_FILE);
        stID.m_strGroupName = strGroupName;
        //int nContactID = -1; //用于接收增加的联系人ID
        g_pLocalDirManager->AddLocalContact(stID, nGroupId, false);
    }  // for.

    return IMPORT_NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////
// Class CXPortV50.
CXPortV50::CXPortV50()
{

}

CXPortV50::~CXPortV50()
{

}

int CXPortV50::Import(const char * szPath)
{
    if (szPath == NULL || szPath[0] == '\0')
    {
        return IMPORT_INVALID_PATH;
    }

#ifndef IF_NO_EXCEPTION
    try
#endif // IF_NO_EXCEPTION
    {
        xml_document docImport;
        docImport.load_file(szPath);

        xml_node NodeRootFirst = docImport.child(kszV50NodeRoot);

        xml_node NodeG = docImport.child(kszV50NodeGRoot);
        if (NodeG.empty() && !NodeRootFirst.empty())
        {
            // 从转换工具生成的格式.
            NodeG = NodeRootFirst.child(kszV50NodeGRoot);
        }
        int nImportedResult = MyImportGroup(NodeG);
        if (nImportedResult == IMPORT_NO_ERROR)
        {
            xml_node TmpNodeC = docImport.child(kszV50NodeCRoot);
            if (TmpNodeC.empty() && !NodeRootFirst.empty())
            {
                // 从转换工具生成的格式.
                TmpNodeC = NodeRootFirst.child(kszV50NodeCRoot);
            }
            nImportedResult = MyImportContact(TmpNodeC);
        }
        return nImportedResult;
    }

#ifndef IF_NO_EXCEPTION
    catch (...)
    {
        DIR_INFO("CXPortV50::Import, [%s] invalid!", szPath);
        return IMPORT_INVALID_FILE;
    }
#endif // IF_NO_EXCEPTION

    return IMPORT_INVALID_FILE;
}

int CXPortV50::MyImportGroup(xml_node GRoot)
{
    if (GRoot.empty())
    {
        return IMPORT_NO_ERROR;
    }

    ContactGroupData stGD;
    for (xml_node NodeG = GRoot.child(kszV50NodeG);
            !NodeG.empty();
            NodeG = NodeG.next_sibling(kszV50NodeG))
    {
        // 读取各个属性.
        stGD.m_strDisplayName = QueryAttribute(NodeG, kszV50AttrGName);
        stGD.m_strRing = QueryAttribute(NodeG, kszV50AttrGRing);

        commonUnits_Hide2Real(stGD.m_strRing, PHFILE_TYPE_RING_FILE);
        g_pLocalDirManager->AddGroup(stGD, true);
    }  // for.

    return IMPORT_NO_ERROR;
}

int CXPortV50::MyImportContact(xml_node CRoot)
{
    if (CRoot.empty())
    {
        return IMPORT_NO_ERROR;
    }

    ContactGeneralItemData stID;
    for (xml_node NodeC = CRoot.child(kszV50NodeC);
            !NodeC.empty();
            NodeC = NodeC.next_sibling(kszV50NodeC))
    {
        stID.Reset();
        stID.m_strDisplayName = QueryAttribute(NodeC, kszV50AttrCName);
        if (stID.m_strDisplayName.empty())
        {
            continue;
        }

        stID.m_strOfficePhoneNumber = QueryAttribute(NodeC, kszV50AttrCOfficeNumb);
        stID.m_strMobilePhoneNumber = QueryAttribute(NodeC, kszV50AttrCMobileNumb);
        stID.m_strOtherPhoneNumber = QueryAttribute(NodeC, kszV50AttrCOtherNumb);
        stID.m_strRing = QueryAttribute(NodeC, kszV50AttrCRing);
        commonUnits_Hide2Real(stID.m_strRing, PHFILE_TYPE_RING_FILE);
        // 添加line属性
        yl::string strTempLine = QueryAttribute(NodeC, kszV50AttrCLine);
        if (!strTempLine.empty()
                && atoi(strTempLine.c_str()) >= 0)
        {
            stID.m_nLine = atoi(strTempLine.c_str());
        }
        else
        {
            stID.m_nLine = -1;
        }
        if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
        {
            stID.m_strDefaultPhoto = QueryAttribute(NodeC, kszV50AttrCDefPhoto);
        }

#ifdef IF_FEATURE_PSTN
        yl::string strTemp = QueryAttribute(NodeC, kszV50AttrCFavorate);
        stID.m_bIsFavorite = strTemp == "1" ? true : false;
#endif

        // 导入成员的组信息, 如果组不存在, 则创建.
        // 获取组信息.
        stID.m_strGroupName = QueryAttribute(NodeC, kszV50AttrCGroup);

        int nGroupId;

        if (stID.m_strGroupName.empty())
        {
            stID.m_strGroupName = kszGroupNameAll;
            nGroupId = kszAllContactId;
        }
        else
        {
            nGroupId = g_pLocalDirManager->GetGroupIdByGroupName(stID.m_strGroupName);
            if (nGroupId == knInvalidRId)
            {
                // 组不存在, 则创建组.
                ContactGroupData stGD;
                stGD.m_strDisplayName = stID.m_strGroupName;
                stGD.m_strRing = "";
                nGroupId = g_pLocalDirManager->AddGroup(stGD, true);
                //创建组不成功，则统一加入all组
                if (nGroupId < 0)
                {
                    nGroupId = kszAllContactId;
                }
            }
        }

#if IF_FEATURE_FAVORITE_DIR
        yl::string strTempIndex = QueryAttribute(NodeC, kszV50AttrCFavoriteIndex);
        if (!strTempIndex.empty()
                && atoi(strTempIndex.c_str()) > 0)
        {
            stID.m_nFavoriteIndex = atoi(strTempIndex.c_str());
        }
        else
        {
            stID.m_nFavoriteIndex = -1;
        }
#endif //IF_FEATURE_FAVORITE_DIR

        stID.m_strAutoDivert = QueryAttribute(NodeC, kszCxmlAttrDivert);

        g_pLocalDirManager->AddLocalContact(stID, nGroupId, false);

    }  // for.

    return IMPORT_NO_ERROR;
}

// Class CXPortBlackList
CXPortBlackList::CXPortBlackList()
{

}

CXPortBlackList::~CXPortBlackList()
{

}

int CXPortBlackList::Import(const char * szPath)
{
#ifndef IF_NO_EXCEPTION
    try
#endif // IF_NO_EXCEPTION
    {
        // 加载内容到内存.
        xml_document fdoc;
        fdoc.load_file(szPath);

        // 读取根节点信息.
        xml_node pNodeRoot = fdoc.child(kszBlackListRoot);
        if (pNodeRoot.empty())
        {
            return IMPORT_INVALID_FILE;
        }

        // 导入成员.
        MyImportContact(pNodeRoot);

        return IMPORT_NO_ERROR;
    }

#ifndef IF_NO_EXCEPTION
    catch (...)
    {
        DIR_INFO("CXPortBlackList::Import, invalid file (%s)", szPath);
        return IMPORT_INVALID_FILE;
    }
#endif // IF_NO_EXCEPTION

    return IMPORT_INVALID_FILE;
}

int CXPortBlackList::MyImportContact(xml_node CRoot)
{
    if (CRoot.empty())
    {
        return IMPORT_NO_ERROR;
    }
    for (xml_node node = CRoot.child(kszBlackListItem);
            node;
            node = node.next_sibling(kszBlackListItem))
    {
        xml_attribute attrTmp = node.attribute(kszBlackListNumber);

        if (attrTmp)
        {
            ContactGeneralItemData data;
            data.m_strDisplayName       = attrTmp.value();  // Contact display name.
            data.m_strOfficePhoneNumber = attrTmp.value();  // Office phone number.
            data.m_strMobilePhoneNumber = attrTmp.value();  // Mobile phone number.
            data.m_strOtherPhoneNumber  = attrTmp.value();  // Other phone number.
            //data.m_strGroupName;    // Group name.
            //yl::string m_strRing;   // Ring name(no path).
            //int m_nLine;            // Line.
            //strNumber = attrTmp.value();
            DIR_INFO("Blacklist Number [%s]", attrTmp.value());
            g_pLocalDirManager->AddLocalContact(data, kszBlackListGroupId, false);
        }
    }  // for.

    return IMPORT_NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////
// namespace XPortFactory.
namespace XPortFactory
{
int ImportByPath(const char * szPath,
                 bool isAppend)
{
    yl::string strPath = "";

    if (NULL == szPath || szPath[0] == '\0')
    {
        GetDefaultPath(strPath);
    }
    else
    {
        strPath = szPath;
    }

    DIR_INFO("Directory: Import file path = %s", strPath.c_str());

    CXPortBase * pXPort = NULL;

    int nFileType = ParseContactFileType(strPath.c_str());
    DIR_INFO("Directory: Import file type = %d", nFileType);
    switch (nFileType)
    {
    case CONTACT_FILE_T3X:
        pXPort = new CXPortT3X();
        break;
    case CONTACT_FILE_V40:
        pXPort = new CXPortV40();
        break;
    case CONTACT_FILE_V50:
        pXPort = new CXPortV50();
        break;
    case CONTACT_FILE_BLACKLIST:
        pXPort = new CXPortBlackList();
        break;
    default:
        pXPort = NULL;
        break;
    }
    int nRet = IMPORT_INVALID_FILE;
    if (pXPort != NULL)
    {
        nRet = pXPort->Import(strPath.c_str(), isAppend);

        delete pXPort;
    }

    remove(strPath.c_str());
    return nRet;
}

int ExportToPath()
{
    int nRet = IMPORT_INVALID_PATH;
    CXPortV50 * pXPort = new CXPortV50();
    if (pXPort != NULL)
    {
        nRet = pXPort->Export();

        delete pXPort;
    }
    return nRet;
}

int ParseContactFileType(const char * szPath)
{
    // 加载内容到内存.
    xml_document fdoc;
    fdoc.load_file(szPath);

    // 读取根节点信息.
    xml_node pNodeRoot = fdoc.child(kszT3XNodeRoot);
    if (!pNodeRoot.empty())
    {
        // http://10.3.5.199/Bug.php?BugID=70006
        xml_node pNodeGInfo = pNodeRoot.child(kszT3XNodeGList);
        xml_node pNodeBList = pNodeRoot.child(kszT3XNodeBList);
        xml_node pNodeG = pNodeRoot.child(kszT3XNodeG);
        xml_attribute AttrG = pNodeG.attribute(kszV40AttrGName);
        if (!pNodeGInfo.empty() || !pNodeBList.empty()
                || (!pNodeG.empty() && AttrG.empty()))
        {
            return CONTACT_FILE_T3X;
        }
        else
        {
            return CONTACT_FILE_V40;
        }
    }

    pNodeRoot = fdoc.child(kszV50NodeRoot);
    if (!pNodeRoot.empty())
    {
        return CONTACT_FILE_V50;
    }

    pNodeRoot = fdoc.child(kszV50NodeCRoot);
    if (!pNodeRoot.empty())
    {
        return CONTACT_FILE_V50;
    }

    pNodeRoot = fdoc.child(kszV50NodeGRoot);
    if (!pNodeRoot.empty())
    {
        return CONTACT_FILE_V50;
    }
    pNodeRoot = fdoc.child(kszBlackListRoot);
    if (!pNodeRoot.empty())
    {
        return CONTACT_FILE_BLACKLIST;
    }

    return CONTACT_FILE_TYPE_UNKOWN;
}

};  // namespace XPortFactory.
