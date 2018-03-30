#ifndef MENU_MENUCONTROLLER_H_
#define MENU_MENUCONTROLLER_H_

#include "ETLLib/ETLLib.hpp"

#include "path_define.h"

#include "commondefine.h"
#include "menumodel.h"

class CMenuCallback;

namespace NS_MENU
{

class CMenuModel;

// Class CMenuController, it is a lazy singleton.
class CMenuController
{
public:
    // Get the singleton instance.
    static CMenuController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // Do action, it opens a new menu or some page.
    // | strClickAction | is the click action string.
    // Returns true if entered successfully, otherwise return false.
    bool DoAction(const yl::string & strClickAction);

    // Do long action, it opens a new menu or some page.
    // | strLongClickAction | is the long click action string.
    // Returns true if entered successfully, otherwise return false.
    bool DoLongAction(const yl::string & strLongClickAction) const;

    // Get menu list by menu type, it calls GetMenuListByXmlFileName in model in internal.
    // | strClickAction | is the click action string.
    // Return the found struct if successfully, otherwise return NULL.
    MenuItemListData GetMenuListByClickAction(const yl::string & strClickAction) const;

    // Get title by click action.
    // | strClickAction | is the click action.
    // Return the title if Retrieved successfully, otherwise return empty.
    const yl::string GetTitleByClickAction(const yl::string & strClickAction) const;

    // 根据Click Action获取页面权限.
    // | strClickAction | 是传入的click action.
    // 如果获取成功则返回权限, 否则返回空.
    yl::string GetPriviledgeByClickAction(const yl::string & strClickAction) const;

    // 根据枚举获取菜单的下标.
    // | eEnum | 为MenuPageType枚举.
    // 如果返回成功则返回下标, 否则返回-1.
    int GetIndexByPageEnum(int eEnum);

    // 判断某个页面是否可用
    // nPageType 是页面类型，为MenuPageType的枚举值
    bool IsPageValid(int nPageType);

    // 进入某个页面.
    // | nPageType | 是页面类型, 为MenuPageType枚举值.
    // | nIndex | 为子页面的下标.
    // bEnter为true表示直接进入，否则表示选中但不进入
    // 如果进入成功则返回true, 否则返回false.
    bool EnterPageByType(int nPageType, int nIndex = 0, bool bEnter = true);

    // 从Dsskey设置界面进入联系人模块的
    // | pCallbackFun | 联系人模块的回调指针
    bool EnterDirFormDsskey(CMenuCallback * pMenuCallback);

    // 从Dsskey设置界面退出联系人模块
    bool ExitDirFormDsskey();

    //根据Action进入最后一层界面，Index为默认选中的控件序号
    bool EnterPageByAction(const yl::string & strClickAction, int nSelectIndex = 0);

    //根据菜单的aciton获取对应的三级权限文件中的key
    yl::string GetWebItemKeyByClickAction(const yl::string strClickAction) const;

    MenuItemList * GetFixMenuListInFileByClickAction(const yl::string & strClickAction) const;

    bool IsSpecialDlgToShowSuccessAccount(yl::string strSubPageAction)
    const;       //V81Beta需求 Anonymous Auto answer只显示已注册成功的账号

private:
    // Initialize.
    // Return true if initialized successfully, otherwise return false.
    bool MyInit();
    // Finalize.
    // Return true if finalized successfully, otherwise return false.
    bool MyFinal();

    // Following methods are used to enter some menu,such as main-menu, feature-menu, etc.
    // There is no difference with entering main-menu or entering feature-menu.
    // In internal we calls UI interface.
    // | strMenuName | is the click action, such as "OpenXml(setting_list.xml)".
    // | bTopWindow | is menu top the window
    // Returns true if entered successfully, otherwise return false.
    // Enter main-menu.
    bool MyEnterMenuByString(const yl::string & strClickAction, bool bTopWindow = false) const;

    bool MyEnterSubPageByString(const yl::string & strClickAction) const;

    //bool MyEnterSubMenuByString(const yl::string& strClickAction, bool bTopWindow = false) const;

    bool MyEnterDirectoryPageByString(const yl::string & strClickAction) const;

    bool MyEnterCallLogPageByString(const yl::string & strClickAction) const;

    bool MyEnterDialByString(const yl::string & strClickAction) const;


    CMenuController();
    ~CMenuController();

    // 清空Dsskey的标志位
    void ClearDsskeyFlag();

    // 注册消息.
    void MyRegisterMessage();

    // 取消消息注册.
    void MyUnRegisterMessage();

    // 消息处理函数.
    static LRESULT MyOnLdapMessage(msgObject & msg);

private:
    CMenuModel * m_pMenuModel; // The menu model.

    static CMenuController * m_pMenuController; // The controller singleton instance.

    bool            m_bDsskeyJumpDirFalg;
    CMenuCallback * m_pMenuCallback;
    MenuItemList  * m_pMenuItemList;    // 用来缓存三级权限过滤后的链表
};
}  // namespace NS_MENU

#define g_pMenuController (NS_MENU::CMenuController::GetInstance())

#endif  // MENU_MENUCONTROLLER_H_
