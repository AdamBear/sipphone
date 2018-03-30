#ifndef MENU_MENUMODEL_H_
#define MENU_MENUMODEL_H_

#include <list>
#include <map>
#include <ylstring.h>

#include "setting/include/common.h"
#include "commondefine.h"

namespace NS_MENU
{
// Class CMenuModel, this class is used to manager the menu items.
class CMenuModel
{
public:
    // Menu item map and the iterator.
    // Map key is the XML file name, value is the menu item list.
    typedef std::map<yl::string, MenuItemList *> MenuListMap;
    typedef MenuListMap::iterator MenuListMapIter;
    typedef MenuListMap::const_iterator MenuListMapCIter;

    // Page priviledge map.
    // Key: click action, value: page priviledge.
    typedef std::map<yl::string, yl::string> PageActionPriviledge;

    // 枚举和Click Action的map.
    // Key: 为页面枚举; Value: Click Action.
    typedef std::map<int, yl::string> CAEnumMap;

    CMenuModel();
    ~CMenuModel();

    // Get menu list by XML file name(the key).
    // | strXmlFileName | is the XML file.
    // Return the pointer point to the found list if successfully, otherwise return NULL.
    MenuItemList * GetMenuListByXmlFileName(const yl::string & strXmlFileName) const;

    // Get title by click action.
    // | strClickAction | is the click action.
    // Return the title if Retrieved successfully, otherwise return empty.
    const yl::string GetTitleByClickAction(const yl::string & strClickAction) const;

    // 根据Click Action获取页面权限.
    // | strClickAction | 是传入的click action.
    // 如果获取成功则返回权限, 否则返回空.
    yl::string GetPriviledgeByClickAction(const yl::string & strClickAction) const;

    // 根据ClickAction获取在菜单中的下标.
    // | strClickAction | 是传入的click action.
    // 如果获取成功则返回下标, 否则返回-1.
    int GetIndexByClickAction(const yl::string & strClickAction) const;

    // 根据枚举获取菜单的下标.
    // | eEnum | 为MenuPageType枚举.
    // 如果返回成功则返回下标, 否则返回-1.
    int GetIndexByPageEnum(int eEnum);

private:
    // Initialize. It loads all menu item from XML file.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyInit();
    // Finalize. It clears all menu items and frees the memory.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyFinal();

    // 初始化枚举map.
    bool MyInitEnumMap();
    // 加载配置文件.
    bool MyLoadConfigure();

    // Load XML file.
    // | strXmlFileName | is the XML file.
    bool MyLoadXmlFile(const yl::string & strXmlFileName);

    // Find All XML files in the specified folder.
    // | strFullPath | is the folder to find in, it must be a full path.
    // Returns the XML file list if found successfully, otherwise returns
    // empty list.
    std::list<yl::string> MyFindAllXmlFileInFolder(const yl::string & strFullPath);

    // Insert a menu item into the list.
    // | strXmlFileName | is the XML file name, it is the key also.
    // | mid | is the menu item to insert.
    // Return true if inserted successfully, otherwise returns false.
    bool MyInsert(const yl::string & strXmlFileName, MenuItemData mid);
    // Remove a menu item from the list.
    // | strXmlFileName | is the XML file name, it is the key also.
    // | strIdName | is the Id name of menu item.
    // Return true if removed successfully, otherwise return false.
    bool MyRemove(const yl::string & strXmlFileName, const yl::string & strIdName);
    // Whether or not the menu item exists.
    // | strXmlFileName | is the XML file name, it is the key also.
    // | strIdName | is the Id name of the menu item.
    // Return true if the menu item exists, otherwise return false.
    bool MyExist(const yl::string & strXmlFileName, const yl::string & strIdName);

private:
    MenuListMap m_mapMenuList;  // The menu list map.
    CAEnumMap m_mapCAEnum;  // 枚举对应的ClickAction的map.
    PageActionPriviledge m_mapPagePrivi;  // The page priviledge map.
};

}  // namespace NS_MENU

#endif  // MENU_MENUMODEL_H_
