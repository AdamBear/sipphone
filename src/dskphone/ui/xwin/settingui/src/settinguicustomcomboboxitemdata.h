#ifndef __SETTINGUI_CUSTOM_COMBOBOX_ITEM_DATA_H__
#define __SETTINGUI_CUSTOM_COMBOBOX_ITEM_DATA_H__

#include "ylstl/ylstring.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"

#if IF_SUPPORT_CUSTOM_COMBO_BOX
enum
{
    // 第3层子类，使用5、6位
    SETTING_ITEM_TYPE_COMBOBOX_CUSTOM       = 0x00000f00,
};

class CSettingUICustomComboBoxItemData : public CSettingUIComboBoxItemData
{
public:
    explicit CSettingUICustomComboBoxItemData();
    virtual ~CSettingUICustomComboBoxItemData();

    static int GetCustomComboBoxDataItemType();
    static CSettingUICustomComboBoxItemData * GetCustomComboBoxDataItem(CSettingUIItemDataBase *
            pDataItem);

    static CSettingUIItemDataBase * CreateCustomComboBoxDataItem();

    // 解析Item数据
    static bool ParserCustomComboBoxItemData(const xml_node & node, CSettingUIItemDataBase * pItemInfo);

public:
    const yl::string & GetClickAction() const;
    void SetClickAction(const yl::string & strFocusAction);

    virtual const yl::string & GetItemAction();

private:
    yl::string      m_strClickAction;           // Focus事件
};

typedef CSettingUICustomComboBoxItemData    *   CSettingUICustomComboBoxItemDataPtr;
#endif

#endif // __SETTINGUI_CUSTOM_COMBOBOX_ITEM_DATA_H__

