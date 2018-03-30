#include "menumodel.h"
#include "setting/include/modmenu.h"
#include <assert.h>

#include "commonapi/systemhelperapi.h"
#include "commonapi/stringhelperapi.h"
#include "path_define.h"
#include "ETLLib.hpp"

#include "menutitlemanager.h"
#include "xmlparser/xmlparser.hpp"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "setting_page_define.h"
#include "feature/include/modfeature.h"
#include "resource/modresource.h"
#include "commonunits/t2xpath.h"

namespace NS_MENU
{
//////////////////////////////////////////////////////////////////////////
// Class CMenuModel
CMenuModel::CMenuModel()
{
    MyInit();
}

CMenuModel::~CMenuModel()
{
    MyFinal();
}

MenuItemList * CMenuModel::GetMenuListByXmlFileName(
    const yl::string & strXmlFileName) const
{
    if (strXmlFileName.empty())
    {
        return NULL;
    }

    // Return NULL if the list not exists.
    MenuListMapCIter citer = m_mapMenuList.find(strXmlFileName);
    if (citer == m_mapMenuList.end())
    {
        return NULL;
    }

    // It is the caller's responsibility to check if the pointer is NULL.
    return citer->second;
}

const yl::string CMenuModel::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    return g_pMenuTitleManager != NULL
           ? g_pMenuTitleManager->GetTitleByClickAction(strClickAction) : "";
}

yl::string CMenuModel::GetPriviledgeByClickAction(
    const yl::string & strClickAction) const
{
    // 根据Click Action获取页面权限.
    PageActionPriviledge::const_iterator citer = m_mapPagePrivi.find(strClickAction);
    return citer != m_mapPagePrivi.end() ? citer->second : "";
}

int CMenuModel::GetIndexByClickAction(const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return -1;
    }

    // 遍历查找某个click action对应的下标.
    for (MenuListMapCIter citerMap = m_mapMenuList.begin();
            citerMap != m_mapMenuList.end();
            ++citerMap)
    {
        if (citerMap->second != NULL)
        {
            int i = -1;
            for (MenuItemListIter citerList = citerMap->second->begin();
                    citerList != citerMap->second->end();
                    ++citerList)
            {
                const MenuItemData * pMenuItemData = *citerList;
                if (pMenuItemData != NULL)
                {
                    ++i;
                    if (pMenuItemData->m_strClickAction == strClickAction)
                    {
                        return i;
                    }
                }
            }  // for.
        }
    }  // for.

    return -1;
}

int CMenuModel::GetIndexByPageEnum(int eEnum)
{
    CAEnumMap::const_iterator citer = m_mapCAEnum.find(eEnum);
    if (citer != m_mapCAEnum.end())
    {
        return GetIndexByClickAction(citer->second);
    }
    return -1;
}

bool CMenuModel::MyInit()
{
    // 初始化枚举map.
    MyInitEnumMap();

    // Finds all XML files in the menu folder.
    MyLoadConfigure();

    return true;
}

bool CMenuModel::MyFinal()
{
    // Clear and free all list.
    MenuItemListIter iterList;
    MenuListMapIter iterMap = m_mapMenuList.begin();
    for (; iterMap != m_mapMenuList.end(); ++iterMap)
    {
        if (iterMap->second == NULL)
        {
            continue;
        }

        iterList = iterMap->second->begin();
        while (iterList != iterMap->second->end())
        {
            if (*iterList != NULL)
            {
                // Free the memory.
                delete *iterList;
                *iterList = NULL;
            }

            // Remove from the list.
            iterList = iterMap->second->erase(iterList);
        }  // inner for
    }  // outer for

    return true;
}

bool CMenuModel::MyInitEnumMap()
{
    m_mapCAEnum[MENU_SUB_PAGE_CALLLOG_ALL] = "OpenSubPage(call_log_1)";
    m_mapCAEnum[MENU_SUB_PAGE_CALLLOG_MISSED] = "OpenSubPage(call_log_2)";
    m_mapCAEnum[MENU_SUB_PAGE_CALLLOG_DIALED] = "OpenSubPage(call_log_3)";
    m_mapCAEnum[MENU_SUB_PAGE_CALLLOG_RECIEVED] = "OpenSubPage(call_log_4)";
    m_mapCAEnum[MENU_SUB_PAGE_CALLLOG_FORWARD] = "OpenSubPage(call_log_5)";

    return true;
}

bool CMenuModel::MyLoadConfigure()
{
    // Finds all XML files in the menu folder.
    YLList<yl::string> listFileFound;
    if (!commonAPI_getFilesInDirectoryByExt(&listFileFound,  res_getPathByName(RESOURCE_PATH_MENU),
                                            "xml"))
    {
        return false;
    }

    // Not found, return.
    if (listFileFound.size() <= 0)
    {
        return false;
    }

    bool isSucceed = false;
    // Initialize all lists.
    YLList<yl::string>::const_iterator citer = listFileFound.begin();
    for (; citer != listFileFound.end(); ++citer)
    {
        const yl::string & refXmlFileName = *citer;
        if (refXmlFileName.empty())
        {
            continue;
        }

        // New list.
        MenuItemList * pMIL = new MenuItemList();
        if (pMIL != NULL)
        {
            bool isInserted = m_mapMenuList.insert(
                                  std::make_pair(refXmlFileName, pMIL)).second;
            if (isInserted)
            {
                // Load data from per list.
                isSucceed = MyLoadXmlFile(refXmlFileName);
            }
        }
    }  // for.

    return true;
}

bool CMenuModel::MyLoadXmlFile(const yl::string & strXmlFileName)
{
    const char * const kszRootMenu = "root_menu"; // The root node name.
    const char * const kszNodeNameItem = "item"; // The item node name.
    const char * const kszAttributeNameIdName = "id_name"; // The id-name attribute name.
    const char * const kszAttributeNameDisplayName = "display_name"; // The display-name attribute name.
    const char * const kszAttributeNameIconName =
        "normal_icon_name"; // The icon-name node attribute. //!!!!! VPX的配置表字段需要修改
    const char * const kszAttributeNameSelectedIconName =
        "selected_icon_name"; // The icon-name node attribute.
    const char * const kszAttributeNameClickAction = "click_action"; // The click-action attribute name.
    const char * const kszAttributeNameLongClickAction =
        "long_click_action"; // The long-click-action attribute name.
    const char * const kszAttributeNameUserLevel = "user_level"; // The user-level attribute name.
    const char * const kszAttributeNameVersion = "version"; // The version attribute name.

    // Get the full XML file path.
    yl::string strFullXmlFilePath = res_getPathByName(RESOURCE_PATH_MENU);
    strFullXmlFilePath += strXmlFileName;

    xml_document doc;
    if (!pathFileExist(strFullXmlFilePath.c_str())
            || !doc.load_file(strFullXmlFilePath.c_str()))
    {
        return false;
    }

    xml_node nodeRoot = doc.child(kszRootMenu);
    if (!nodeRoot)
    {
        chWARNING2(FALSE,
                   commonAPI_FormatString("%s has none <root_menu> node", strFullXmlFilePath.c_str()).c_str());
        return false;
    }

    yl::string strAttributeName;
    yl::string strClickAction;
    yl::string strTitle;
    MenuItemData mid;

    // Iterate to load child items.
    //xmlElement* pItemNode = nodeRoot.child(kszNodeNameItem);
    xml_node nodeItem = nodeRoot.child(kszNodeNameItem);
    while (nodeItem)
    {
        // Reset the struct.
        mid.Reset();
        strAttributeName = nodeItem.attribute(kszAttributeNameIdName).value();
        //三级权限关闭时不显示userMode界面
        if (strAttributeName == "userMode" && !feature_UserModeIsOpen())
        {
            nodeItem = nodeItem.next_sibling();
            continue;
        }

        if (!strAttributeName.empty())
        {
            mid.m_strIdName = strAttributeName;
        }

        // Read display-name attribute, the display name also is the title.
        strTitle = nodeItem.attribute(kszAttributeNameDisplayName).value();
        if (!strTitle.empty())
        {
            mid.m_strDisplayName = strTitle;
        }

        // Read normal icon-name attribute.
        strAttributeName = nodeItem.attribute(kszAttributeNameIconName).value();
        if (!strAttributeName.empty())
        {
            mid.m_strIconName = Menu_GetDefaultIcon(eMDIT_DeftMenuIconPath);
            mid.m_strIconName += strAttributeName;
        }

        // Read selected icon-name attribute.
        strAttributeName = nodeItem.attribute(kszAttributeNameSelectedIconName).value();
        if (!strAttributeName.empty())
        {
            mid.m_strSelectedIconName = Menu_GetDefaultIcon(eMDIT_DeftMenuIconPath);
            mid.m_strSelectedIconName += strAttributeName;
        }

        // Read selected-icon-name attribute.
        strAttributeName = nodeItem.attribute(kszAttributeNameSelectedIconName).value();
        if (!strAttributeName.empty())
        {
            mid.m_strSelectedIconName = Menu_GetDefaultIcon(eMDIT_DeftMenuIconPath);
            mid.m_strSelectedIconName += strAttributeName;
        }

        // Read click-action attribute.
        strClickAction = nodeItem.attribute(kszAttributeNameClickAction).value();
        if (!strClickAction.empty())
        {
            mid.m_strClickAction = strClickAction;
        }

        // Read long-click-action attribute.
        strAttributeName = nodeItem.attribute(kszAttributeNameLongClickAction).value();
        if (!strAttributeName.empty())
        {
            mid.m_strLongClickAction = strAttributeName;
        }

        // Read user-level attribute.
        strAttributeName = nodeItem.attribute(kszAttributeNameUserLevel).value();
        if (!strAttributeName.empty())
        {
            mid.m_strUserLevel = strAttributeName;
        }

        MyInsert(strXmlFileName, mid);

        // Set title.
        if (g_pMenuTitleManager != NULL)
        {
            g_pMenuTitleManager->SetTitle(strClickAction, strTitle);
        }

        // Set priviledge.
        m_mapPagePrivi[strClickAction] = mid.m_strUserLevel;
        nodeItem = nodeItem.next_sibling();
    }

    return true;
}

bool CMenuModel::MyInsert(const yl::string & strXmlFileName, MenuItemData mid)
{
    // Check the params.
    if (strXmlFileName.empty() || mid.m_strIdName.empty())
    {
        return false;
    }

    // If the Id existed already, return false.
    if (MyExist(strXmlFileName, mid.m_strIdName))
    {
        return false;
    }

    // Return false if the list not existed.
    MenuListMapIter iter = m_mapMenuList.find(strXmlFileName);
    if (iter == m_mapMenuList.end() || iter->second == NULL)
    {
        return false;
    }

    MenuItemData * pMID = new MenuItemData();
    if (pMID == NULL)
    {
        return false;
    }

    *pMID = mid;
    iter->second->push_back(pMID);
    return true;
}

bool CMenuModel::MyRemove(const yl::string & strXmlFileName, const yl::string & strIdName)
{
    // Check the params.
    if (strXmlFileName.empty() || strIdName.empty())
    {
        return false;
    }

    // Return false if the list not existed.
    MenuListMapIter iterMap = m_mapMenuList.find(strXmlFileName);
    if (iterMap == m_mapMenuList.end() || iterMap->second == NULL)
    {
        return false;
    }

    MenuItemListIter iterList = iterMap->second->begin();
    for (; iterList != iterMap->second->end(); ++iterList)
    {
        // Remove the menu item if found.
        if (*iterList != NULL && (*iterList)->m_strIdName == strIdName)
        {
            // Free the memory.
            delete *iterList;
            *iterList = NULL;

            // Remove the menu item from list.
            iterMap->second->erase(iterList);
            break;
        }
    }  // for

    return true;
}

bool CMenuModel::MyExist(const yl::string & strXmlFileName, const yl::string & strIdName)
{
    // Check the params.
    if (strXmlFileName.empty() || strIdName.empty())
    {
        return false;
    }

    // Return false if the list not existed.
    MenuListMapCIter citerMap = m_mapMenuList.find(strXmlFileName);
    if (citerMap == m_mapMenuList.end() || citerMap->second == NULL)
    {
        return false;
    }

    // Iterate to find the menu item.
    MenuItemListIter citerList = citerMap->second->begin();
    for (; citerList != citerMap->second->end(); ++citerList)
    {
        if (*citerList != NULL && (*citerList)->m_strIdName == strIdName)
        {
            return true;
        }
    }  // for

    return false;
}

};  // name NS_MENU
