/************************************************************************
 * FileName  : modMenu.cpp (implementation file)
 * Purpose   :
 * Date      : 2012/06/19 16:32:38
 ************************************************************************/

#include "setting/include/modmenu.h"
#include "menu/menucontroller.h"
#include <map>

typedef std::map<EMenuDefaultIconType, yl::string> MenuDefaultIconMap;

static MenuDefaultIconMap lg_deftIconMap;

//设置Menu中用到的默认图标
void Menu_SetDefaultIcon(EMenuDefaultIconType eIconType, const yl::string & strIconFile)
{
    lg_deftIconMap[eIconType] = strIconFile;
}

//获取Menu中用到的默认图标
yl::string Menu_GetDefaultIcon(EMenuDefaultIconType eIconType)
{
    MenuDefaultIconMap::const_iterator iter = lg_deftIconMap.find(eIconType);

    if (iter != lg_deftIconMap.end())
    {
        return iter->second;
    }
    else
    {
        return "";
    }
}

//相应对应的点击操作
bool Menu_DoAction(const yl::string & strClickAction)
{
    return g_pMenuController->DoAction(strClickAction);
}

//通过操作名获取Menu的Title
const yl::string Menu_GetTitleByClickAction(const yl::string & strClickAction)
{
    return g_pMenuController->GetTitleByClickAction(strClickAction);
}

//通过操作名获取对应的MenuList内容
MenuItemListData Menu_GetMenuListByClickAction(const yl::string & strClickAction)
{
    return g_pMenuController->GetMenuListByClickAction(strClickAction);
}

//根据Page类型进入对应菜单页面
bool Menu_EnterPageByType(int nPageType, int nIndex /* = 0 */, bool bEnter /* = true */)
{
    return g_pMenuController->EnterPageByType(nPageType, nIndex, bEnter);
}

//根据菜单的aciton获取对应的三级权限文件中的key
yl::string Menu_GetWebItemKeyByClickAction(const yl::string strClickAction)
{
    return g_pMenuController->GetWebItemKeyByClickAction(strClickAction);
}
