#ifndef __MENU_ICON_LIST_ITEM_DATA_H__
#define __MENU_ICON_LIST_ITEM_DATA_H__

#include "settinguilogic/include/settingdefine.h"
#if IF_SUPPORT_MENUICON_ITEM

#include "ylstl/ylstring.h"
#include "settinguibaseitemdata.h"

enum
{
    SETTING_ITEM_TYPE_MENU_ICON     = 0x02000000,
};

class CMenuIconListItemData
{
public:
    CMenuIconListItemData();
    virtual ~CMenuIconListItemData();

    virtual void Reset();

//  static int GetMenuIconDataItemType();
//  static CMenuIconListItemData* GetMenuIconListDataItem(CSettingUIItemDataBase* pDataItem);

    static CSettingUIItemDataBase * CreateMenuIconListItem();

public:
    const yl::string & GetItemID();
    void SetItemID(const yl::string & strItemID);
    const yl::string & GetDisplayName();
    void SetDisplayName(const yl::string & strDisplayName);
    const yl::string & GetAuthorityID();
    void SetAuthorityID(const yl::string & strAuthorityID);
    const yl::string & GetIconName();
    void SetIconName(const yl::string & strIconName);
    const yl::string & GetSelectedIconName();
    void SetSelectedIconName(const yl::string & strSelectedIconName);
    const yl::string & GetClickAction();
    void SetClickAction(const yl::string & strClickAction);
    const yl::string & GetLongClickAction();
    void SetLongClickAction(const yl::string & strLongClickAction);

    bool IsShow();
    void SetShow(bool bShow);

protected:
    yl::string              m_strIdName;            // Item Id
    yl::string              m_strDisplayName;       // Item显示名称
    yl::string              m_strAuthorityID;       // Item三级权限名称
    yl::string              m_strIconName;          // Item常态显示图标
    yl::string              m_strSelectedIconName;  // Item选中态显示图标
    yl::string              m_strClickAction;       // Item点击事件
    yl::string              m_strLongClickAction;   // Item长按事件
    bool                    m_bShow;
};

typedef CMenuIconListItemData   *   CMenuIconListItemDataPtr;
#endif

#endif

