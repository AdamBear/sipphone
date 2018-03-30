#ifndef MENU_MENUGENERATOR_H_
#define MENU_MENUGENERATOR_H_

#include <map>
#include <ylstring.h>

#include "setting/include/common.h"

#include "commondefine.h"

struct MenuItemListData;

namespace NS_MENU
{
// Class CMenuGenerator. 联系人菜单生成器.
// 因为联系人组会动态改变, 所以需要专门的菜单生成器.
class CMenuGenerator
{
public:
    // 动态菜单的数据.
    struct DynamicMenuData
    {
        bool m_isDynamic;  // 是否为动态菜单的标记.

        DynamicMenuData()
        {
            Reset();
        }

        void Reset()
        {
            m_isDynamic = false;
        }
    };

    // 动态菜单的map, key为click action, value为true则click action为
    // 动态菜单.
    typedef std::map<yl::string, DynamicMenuData *> DynamicMenuMap;
    typedef DynamicMenuMap::iterator DynamicMenuMapIter;
    typedef DynamicMenuMap::const_iterator DynamicMenuMapCIter;

    // 获取单键实例.
    static CMenuGenerator * GetInstance();
    // 销毁单键实例.
    static void ReleaseInstance();

    // 根据click action获取菜单列表.
    // | pData | [out] 是buffer.
    // | strClickAction | 是click action.
    // 获取成功则返回true, 否则返回false.
    bool GetMenuListByClickAction(MenuItemListData * pData,
                                  const yl::string & strClickAction);

    // 判断某个Click Action是否需要动态创建.
    // | strClickAction | 为click action.
    // 如果Click Action需要动态创建则返回true, 否则返回false.
    bool MayDynamicCreate(const yl::string & strClickAction) const;

private:
    // 构造函数和析构函数.
    CMenuGenerator();
    ~CMenuGenerator();

    // 初始化.
    // 如果初始化成功则返回ture, 否则返回false.
    bool MyInit();
    // 清除.
    // 如果清除成功则返回true, 否则返回false.
    bool MyFinal();

    // 初始化动态菜单map
    // 初始化成功则返回true, 否则返回false.
    bool MyInitDynamicMap();
    // 清空动态菜单map.
    // 清除成功则返回ture, 否则返回false.
    bool MyFinalDynamicMap();
    // 增加一个动态菜单.
    // | strClickAction | 为该菜单的click action.
    // 增加成功则返回true, 否则返回false.
    bool MyAddDynamicMenu(const yl::string & strClickAction);
    // 删除一个动态菜单.
    // | strClickAction | 为该菜单的click action.
    // 删除成功则返回true, 否则返回false.
    bool MyRemoveDynamicMenu(const yl::string & strClickAction);
    // 查找该动态菜单的值.
    // | strClickAction | 为该菜单的click action.
    // 查找成功则返回该菜单的数据指针, 否则返回NULL.
    DynamicMenuData * MyFindDynamicMenu(const yl::string & strClickAction);
    // 重载.
    const DynamicMenuData * MyFindDynamicMenu(
        const yl::string & strClickAction) const;
    //从菜单文件当中先加载固定的菜单
    //strClickAction是菜单的action，包含菜单文件的文件名
    void LoadFixMenuItemlistInFile(const yl::string & strClickAction);

    /************************************************************************/
    /* Call Feature                                                         */
    /************************************************************************/
    // 以下生成Call Feature菜单的成员函数.
    // 生成总Call Feature菜单.
    MenuItemList * MyGenCallFeatureMenuItemList();
    // 生成Call Control菜单项.
    void MyGenCFMenuItemCallControl();
    // 生成Call Forward菜单项.
    void MyGenCFMenuItemCallForward();
    // 生成Call Waiting菜单项.
    void MyGenCFMenuItemCallWaiting();
    // 生成Auto Answer菜单项.
    void MyGenCFMenuItemAntoAnswer();
    // 生成DSS Keys菜单项.
    void MyGenCFMenuItemDssKey();
    // 生成DND菜单项.
    void MyGenCFMenuItemDnd();
    // 生成Intercom菜单项.
    void MyGenCFMenuItemIntercom();
    // 生成Anonymous Call菜单项.
    void MyGenCFMenuItemAnonymousCall();
    // 生成Advanced菜单项.
    void MyGenCFMenuItemAdvanced();
    // 生成Broadsoft Directory菜单项.
    void MyGenCFMenuItemBroadsoftDirectory();
    // 生成Broadsoft Calllog菜单项.
    void MyGenCFMenuItemBroadsoftCalllog();

    /******************Basic Setting*****************************/
    MenuItemList * MyGenBasicSettingMenuItemList();
    void MyGenCFMenuItemBasicSettingMyStatus();
    void MyGenBasicSettingMenuItemResetLocal();

    /*****************Status********************************/
    MenuItemList * MyGenStatusMenuItemList();
    void MyGenCFMenuItemStatusRTP();

    /****************Advance Setting****************************/
    MenuItemList * MyGenAdvanceSettingMenuItemList();
    void MyGenAdvanceSettingMenuItemResetDefault();

    /****************Advance Setting****************************/
    MenuItemList * MyGenMessageMenuList();
    void MyGenMessageMenuItemSMS();
    void MyGenMessageMenuItemVoiceMail();

#if IF_FEATURE_METASWITCH_VOICEMAIL
    void MyGenMessageMenuItemMTSWVoiceMail();
#endif

    /****************Diagnostics****************************/
    // 生成 Diagnostics菜单项.
    void MyGenCFMenuItemDiagnostics();

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    /****************Menu*****************************/
    MenuItemList * MyGenerateMenuItemList();
    void MyGenerateUSBMenu();
    void MyGenerateUSBIdleReocrd();

    MenuItemList * MyGenerateUSBItemList();
#endif

    /****************Network List****************************/
    MenuItemList * MyGenNetworkList();

    void RemoveItemByIdName(const yl::string & strIdName);

    // 清空菜单列表.
    // 清空成功则返回true, 否则返回false.
    bool MyClearMenuList();

#ifndef _T48
    // 以下为数据转换函数.
    // 将本地组数据转换为菜单数据.
    // | pData | [out] 是buffer.
    // | refGroupData | 是本地组数据.
    // 转换成功则返回true, 否则返回false.
    bool MyLocalGroup2Menu(MenuItemData * pData,
                           const ContactGroupSimpleData & refGroupData) const;
#endif

private:
    MenuItemList * m_pMenuList; // 菜单列表.
    DynamicMenuMap m_mapDynamicMenu;  // 动态菜单map.

    static CMenuGenerator * m_pInst; // 单键.

    DISALLOW_COPY_AND_ASSIGN(CMenuGenerator);
};

}  // namespace NS_DIRECTORY

#define g_pMenuGen (NS_MENU::CMenuGenerator::GetInstance())

#endif  // MENU_MENUGENERATOR_H_
