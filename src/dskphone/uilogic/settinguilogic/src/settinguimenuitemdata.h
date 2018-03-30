#ifndef __SETTINGUI_MENU_ITEM_DATA_H__
#define __SETTINGUI_MENU_ITEM_DATA_H__

#include "settinguibaseitemdata.h"
#include "ylstl/ylstring.h"

enum
{
    // 第1层子类，使用3、4位
    SETTING_ITEM_TYPE_MENU      = 0x00040000,
};

class CSettingUIMenuItemData : public CSettingUIItemDataBase
{
public:
    explicit CSettingUIMenuItemData();
    virtual ~CSettingUIMenuItemData();

    static int GetMenuDataItemType();
    static CSettingUIMenuItemData * GetMenuListDataItem(CSettingUIItemDataBase * pDataItem);

    static CSettingUIItemDataBase * CreateMenuListDataItem();

    // 解析Item数据
    static bool ParserMenuItemData(const xml_node & node, CSettingUIItemDataBase * pItemInfo);

public:
    virtual void Reset();

public:
    virtual bool SetItemData(const CSettingUIItemDataBase & itemDatas);
    virtual bool GetItemData(CSettingUIItemDataBase & itemDatas);

    virtual bool LoadItemData(CSettingUIItemDataBase & itemDatas);
    virtual bool SaveItemData(const CSettingUIItemDataBase & itemDatas);

    // 获取Item点击事件
    virtual const yl::string & GetItemAction();
    // 获取Item焦点事件
    virtual const yl::string & GetItemFocusAction();

public:
    const yl::string & GetClickAction() const;
    void SetClickAction(const yl::string & strClickAction);

    const yl::string & GetFocusAction() const;
    void SetFocusAction(const yl::string & strFocusAction);

protected:
    yl::string      m_strClickAction;           // 点击事件
    yl::string      m_strFocusAction;           // Focus事件
};

typedef     CSettingUIMenuItemData   *  CSettingUIMenuItemDataPtr;

#endif

