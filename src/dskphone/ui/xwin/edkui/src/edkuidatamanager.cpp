#include "edkuidatamanager.h"
#include "xmlparser/xmlparser.hpp"
#include "path_define.h"
#include "../include/edkui_def.h"
#include "edk/include/modedk.h"
#include "commonunits/t2xpath.h"
#include "commonapi/stringhelperapi.h"

#if IF_FEATURE_EDK
IMPLEMENT_GETINSTANCE(CEdkUIDataManager)

CEdkUIDataManager::CEdkUIDataManager()
{
    m_mapMenuIDPage.clear();
    m_mapMenuIDToDirType.clear();
    m_mapSoftkeyIDToType.clear();
}

CEdkUIDataManager::~CEdkUIDataManager()
{
}

bool CEdkUIDataManager::LoadMapConfig()
{
    if (!Edk_IsEnable())
    {
        Clear();
        return false;
    }

    Clear();
    EDK_INFO("load edk ui map config.");

    xml_document doc;
    if (!doc.load_file(res_getPathByName(RESOURCE_PATH_EDKCONFIG).c_str()))
    {
        EDK_INFO("load file error");
        return false;
    }

    // edk 节点
    xml_node rootNode = doc.child(ROOT_NODE);
    if (!rootNode)
    {
        return false;
    }

    //------------------------ softkey 节点 ------------------
    xml_node softkeyNode = rootNode.child(KEY_NODE);
    if (!softkeyNode)
    {
        return false;
    }

    xml_node childNode = softkeyNode.child(ATTRI_ITEM);
    while (childNode)
    {
        xml_attribute attrSoftkey_Type = childNode.attribute(ATTRI_SOFTKEY_TYPE);
        xml_attribute attrProgramkey_Type = childNode.attribute(ATTRI_PROGRAMKEY_TYPE);
        if (!attrSoftkey_Type
                && 0 == strcmp(attrSoftkey_Type.value(), "")
                && !attrProgramkey_Type
                && 0 == strcmp(attrProgramkey_Type.value(), ""))
        {
            childNode = childNode.next_sibling(ATTRI_ITEM);
            continue;
        }

        xml_attribute attrKeyID = childNode.attribute(ATTRI_KEY_ID);
        if (attrKeyID)
        {
            KeyType keyType;

            keyType.eSoftkeyType = (SoftKey_TYPE)atoi(attrSoftkey_Type.value());
            keyType.eDsskeyType = (DssKey_Type)atoi(attrProgramkey_Type.value());
            m_mapSoftkeyIDToType.insert(attrKeyID.value(), keyType);
        }

        childNode = childNode.next_sibling(ATTRI_ITEM);
    }

#if 0 //打印加载进来的数据
    for (Iter_SoftkeyID_Type_Map iter = m_mapSoftkeyIDToType.begin();
            iter != m_mapSoftkeyIDToType.end();
            iter ++)
    {
        KeyType & keyType = iter->second;

        EDK_INFO("SoftkeyIDToTypeMap id[%s], softkeyType[%d], dsskeyType[%d]", iter->first.c_str(),
                 keyType.eSoftkeyType, keyType.eDsskeyType);
    }
#endif

    //------------------------ setting ui 节点 ------------------
    xml_node settingNode = rootNode.child(SETTING_NODE);
    if (!settingNode)
    {
        return false;
    }

    childNode = settingNode.child(ATTRI_ITEM);
    while (childNode)
    {
        xml_attribute attrType = childNode.attribute(ATTRI_PAGE);
        if (!attrType || 0 == strcmp(attrType.value(), ""))
        {
            childNode = childNode.next_sibling(ATTRI_PAGE);
            continue;
        }

        xml_attribute attrMenuID = childNode.attribute(ATTRI_MENU_ID);
        xml_attribute attrDev = childNode.attribute(ATTRI_DEV);
        if (!attrMenuID.empty() && (attrDev.empty() || IsPhoneSupport(attrDev.value())))
        {
            m_mapMenuIDPage.insert(attrMenuID.value(), attrType.value());
        }

        childNode = childNode.next_sibling(ATTRI_ITEM);
    }

#if 0 //打印加载进来的数据
    for (Iter_MenuID_Page_Map iter = m_mapMenuIDPage.begin();
            iter != m_mapMenuIDPage.end();
            iter ++)
    {
        EDK_INFO("MenuIDPageMap id[%s], page[%s]", iter->first.c_str(), iter->second.c_str());
    }
#endif

    //------------------------ directory ui 节点 ------------------
    xml_node dirNode = rootNode.child(DIRECTORY_NODE);
    if (!dirNode)
    {
        return false;
    }

    childNode = dirNode.child(ATTRI_ITEM);
    while (childNode)
    {
        xml_attribute attrType = childNode.attribute(ATTRI_TYPE);
        if (!attrType || 0 == strcmp(attrType.value(), ""))
        {
            childNode = childNode.next_sibling(ATTRI_TYPE);
            continue;
        }

        xml_attribute attrMenuID = childNode.attribute(ATTRI_MENU_ID);
        if (attrMenuID)
        {
            m_mapMenuIDToDirType.insert(attrMenuID.value(), atoi(attrType.value()));
        }

        childNode = childNode.next_sibling(ATTRI_ITEM);
    }
#if 0 //打印加载进来的数据
    for (Iter_MenuID_DirType_Map iter = m_mapMenuIDToDirType.begin();
            iter != m_mapMenuIDToDirType.end();
            iter++)
    {
        EDK_INFO("MenuIDToDirTypeMap id[%s], type[%d]", iter->first.c_str(), iter->second);
    }
#endif

    return true;
}

// 通过softkey type获取softkey id
bool CEdkUIDataManager::GetSoftkeyIDByType(SoftKey_TYPE eType, yl::string & strSoftkeyID)
{
    EDK_INFO("CEdkUIDataManager::GetSoftkeyIDByType --- softkey type[%d]", eType);

    for (Iter_SoftkeyID_Type_Map iter = m_mapSoftkeyIDToType.begin();
            iter != m_mapSoftkeyIDToType.end();
            iter ++)
    {
        KeyType & keyType = iter->second;
        if (keyType.eSoftkeyType != ST_EMPTY
                && keyType.eSoftkeyType == eType)
        {
            EDK_INFO(" -------- softkey id[%s]", iter->first.c_str());

            strSoftkeyID = iter->first;
            return true;
        }
    }

    return false;
}

// 通过softkey id获取softkey type
SoftKey_TYPE CEdkUIDataManager::GetSoftkeyTypeByID(const yl::string & strSoftkeyID)
{
    Iter_SoftkeyID_Type_Map iter = m_mapSoftkeyIDToType.find(strSoftkeyID);

    if (iter != m_mapSoftkeyIDToType.end())
    {
        KeyType & keyType = iter->second;
        return keyType.eSoftkeyType;
    }

    return ST_EMPTY;
}

// 通过menu id获取page name
bool CEdkUIDataManager::GetPageNameByMenuID(const yl::string & strMenuID, yl::string & strPageName)
{
    Iter_MenuID_Page_Map iter = m_mapMenuIDPage.find(strMenuID);

    if (iter != m_mapMenuIDPage.end())
    {
        strPageName = iter->second;
        return true;
    }

    return false;
}

// 通过page name获取menu id
bool CEdkUIDataManager::GetMenuIDByPageName(const yl::string & strPageName, yl::string & strMenuID)
{
    for (Iter_MenuID_Page_Map iter = m_mapMenuIDPage.begin();
            iter != m_mapMenuIDPage.end();
            iter ++)
    {
        if (iter->second == strPageName)
        {
            strMenuID = iter->first;
            return true;
        }
    }

    return false;
}

// 通过menu id获取联系人页面类型
int CEdkUIDataManager::GetDirTypeByMenuID(const yl::string & strMenuID)
{
    Iter_MenuID_DirType_Map iter = m_mapMenuIDToDirType.find(strMenuID);

    if (iter != m_mapMenuIDToDirType.end())
    {
        return iter->second;
    }

    return 0;
}

// 通过联系人页面类型获取menu id
bool CEdkUIDataManager::GetMenuIDByDirType(int iDirType, yl::string & strMenuID)
{
    // log优化
    //EDK_INFO("Menu dir type[%d]", iDirType);

    for (Iter_MenuID_DirType_Map iter = m_mapMenuIDToDirType.begin();
            iter != m_mapMenuIDToDirType.end();
            iter ++)
    {
        if (iter->second == iDirType)
        {
            EDK_INFO(" -------- menu id[%s]", iter->first.c_str());

            strMenuID = iter->first;
            return true;
        }
    }

    return false;
}

// 通过dsskey type 获取program key id
bool CEdkUIDataManager::GetProgramkeyIDByType(DssKey_Type eType, yl::string & strKeyID)
{
    EDK_INFO(" CEdkUIDataManager::GetProgramkeyIDByType --- dsskey type[%d]", eType);

    for (Iter_SoftkeyID_Type_Map iter = m_mapSoftkeyIDToType.begin();
            iter != m_mapSoftkeyIDToType.end();
            iter ++)
    {
        KeyType & keyType = iter->second;
        if (keyType.eDsskeyType != DT_NONE
                && keyType.eDsskeyType == eType)
        {
            strKeyID = iter->first;
            return true;
        }
    }

    return false;
}

// 通过program key id 获取dsskey type
DssKey_Type CEdkUIDataManager::GetProgramkeyTypeByID(const yl::string & strKeyID)
{
    Iter_SoftkeyID_Type_Map iter = m_mapSoftkeyIDToType.find(strKeyID);

    if (iter != m_mapSoftkeyIDToType.end())
    {
        KeyType & keyType = iter->second;
        return keyType.eDsskeyType;
    }

    return DT_NONE;
}

bool CEdkUIDataManager::FindSoftkeyItemByID(Iter_SoftkeyID_Type_Map & iterDest,
        const yl::string & strID)
{
    for (Iter_SoftkeyID_Type_Map iter = m_mapSoftkeyIDToType.begin();
            iter != m_mapSoftkeyIDToType.end();
            iter ++)
    {
        if (strID == iter->first)
        {
            iterDest = iter;
            return true;
        }
    }

    return false;
}

void CEdkUIDataManager::Clear()
{
    m_mapMenuIDPage.clear();
    m_mapMenuIDToDirType.clear();
    m_mapSoftkeyIDToType.clear();
}

// 设置menu id对应的条件判断函数指针
bool CEdkUIDataManager::SetAllowEnterUIFunc(FuncIsAllowEnterMenu pAllowFunc,
        const yl::string & strPageName/* = ""*/,
        int iDirType/* = 0*/)
{
    if (NULL == pAllowFunc)
    {
        return false;
    }

    yl::string strMenuID = "";
    if (strPageName != "" || iDirType != 0)
    {
        if (!GetMenuIDByPageName(strPageName, strMenuID))
        {
            GetMenuIDByDirType(iDirType, strMenuID);
        }
    }

    if (strMenuID != "")
    {
        m_mapAllowEnterUIFunc.insert(strMenuID, pAllowFunc);
        return true;
    }

    return false;
}

// 获取menu id对应的条件判断函数指针
bool CEdkUIDataManager::GetAllowEnterUIFunc(const yl::string & strMenuID,
        FuncIsAllowEnterMenu & pAllowFunc)
{
    Iter_MenuID_AllowFunc_Map iter = m_mapAllowEnterUIFunc.find(strMenuID);
    if (iter == m_mapAllowEnterUIFunc.end())
    {
        return false;
    }

    pAllowFunc = iter->second;
    return true;
}

bool CEdkUIDataManager::IsPhoneSupport(const yl::string& strDev)
{
    YLList<yl::string> lstOutPut;
    commonAPI_splitStringBySeparator(strDev, ' ', lstOutPut);

    YLList<yl::string>::iterator itr = lstOutPut.begin();
    for (; itr != lstOutPut.end(); ++itr)
    {
        if ((*itr) == devicelib_GetPhoneName())
        {
            return true;
        }
    }

    return false;
}

#endif
