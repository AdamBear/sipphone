#ifndef __SETTINGUI_COMBOBOX_ITEM_DATA_H__
#define __SETTINGUI_COMBOBOX_ITEM_DATA_H__

#include "ylstl/ylstring.h"
#include "ylstl/yllist.h"
#include "settinguibaseitemdata.h"
#include "common.h"

enum
{
    // 第1层子类，使用3、4位
    SETTING_ITEM_TYPE_COMBOBOX      = 0x00020000,
};

class CSettingUIComboBoxOption
{
public:
    CSettingUIComboBoxOption();
    virtual ~CSettingUIComboBoxOption();

public:
    yl::string          m_strDisplayName;
    yl::string          m_strVisiableID;
    yl::string          m_strItemVale;
    LIST_STRING         m_listLayoutMembers;
};

typedef YLList<CSettingUIComboBoxOption *>       ListComboBoxOptionData;

class CSettingUIComboBoxItemData : public CSettingUIItemDataBase
{
public:
    explicit CSettingUIComboBoxItemData();
    virtual ~CSettingUIComboBoxItemData();

    static int GetComboBoxDataItemType();
    static CSettingUIComboBoxItemData * GetComboBoxDataItem(CSettingUIItemDataBase * pDataItem);

    static CSettingUIItemDataBase * CreateComboBoxDataItem();

    // 解析Item数据
    static bool ParserComboBoxItemData(const xml_node & node, CSettingUIItemDataBase * pItemInfo);

public:
    virtual bool SetItemData(const CSettingUIItemDataBase & itemDatas);
    virtual bool GetItemData(CSettingUIItemDataBase & itemDatas);

    virtual bool LoadItemData(CSettingUIItemDataBase & itemDatas);
    virtual bool SaveItemData(const CSettingUIItemDataBase & itemDatas);
    virtual bool IsNeedSaveData()
    {
        return true;
    }

public:
    const yl::string & GetDataChangedAction();
    void SetDataChangedAction(const yl::string & strDataChangedAction);

    bool AddOptionItem(CSettingUIComboBoxOption * pOptionData);
    bool DelOptionItem(CSettingUIComboBoxOption * pOptionData);
    // 清除ComboBox Item内容
    void ClearOptionItems();

    ListComboBoxOptionData GetComboBoxOptionData();

    bool IsSwitchSoftKey();
    void SetSwitchSoftKey(bool bSwitch);

    bool IsSetupSoftkeyByItem();
    void SetIsSetupSoftkeyByItem(bool bSetupSoftkeyByItem);

private:
    bool IsOptionExist(CSettingUIComboBoxOption * pOptionData);

protected:
    yl::string                          m_strDataChangedAction;     // 值改变触发事件
    ListComboBoxOptionData              m_listOptions;             // 下拉框列表项
    bool m_bSwitchSoftKey;
    bool m_bSetupSoftkeyByItem;
};

typedef CSettingUIComboBoxItemData   *  CSettingUIComboBoxItemDataPtr;
#endif

