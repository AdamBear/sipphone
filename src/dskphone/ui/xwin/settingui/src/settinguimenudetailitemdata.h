#ifndef __SETTINGUI_DETAIL_MENU_ITEM_DATA_H__
#define __SETTINGUI_DETAIL_MENU_ITEM_DATA_H__

#include "settinguilogic/src/settinguimenuitemdata.h"

enum
{
    // 第2层子类，使用5、6位
    SETTING_ITEM_TYPE_DETAIL_MENU       = 0x00001100,
};

class CSettingUIDetailMenuItemData : public CSettingUIMenuItemData
{
public:
    explicit CSettingUIDetailMenuItemData();
    virtual ~CSettingUIDetailMenuItemData();

    static int GetDetailMenuDataItemType();
    static CSettingUIDetailMenuItemData * GetDetailMenuListDataItem(CSettingUIItemDataBase * pDataItem);
    static CSettingUIItemDataBase * CreateDetailMenuListDataItem();

    static bool ParserDetailMenuItemData(const xml_node & node, CSettingUIItemDataBase * pItemInfo);
};

typedef     CSettingUIDetailMenuItemData    *   CSettingUIDetailMenuItemDataPtr;

#endif //__SETTINGUI_DETAIL_MENU_ITEM_DATA_H__

