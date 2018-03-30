#ifndef __SETTINGUI_CHECKBOX_ITEM_DATA_H__
#define __SETTINGUI_CHECKBOX_ITEM_DATA_H__

#include "settinguibaseitemdata.h"
#include "ylstl/ylstring.h"

#if IF_SUPPORT_CHECK_BOX
enum
{
    // 第1层子类，使用3、4位
    SETTING_ITEM_TYPE_CHECKBOX      = 0x00010000,
};

class CSettingUICheckBoxItemData : public CSettingUIItemDataBase
{
public:
    explicit CSettingUICheckBoxItemData();
    virtual ~CSettingUICheckBoxItemData();

    static int GetCheckBoxDataItemType();
    static CSettingUICheckBoxItemData * GetCheckBoxDataItem(CSettingUIItemDataBase * pDataItem);

    static CSettingUIItemDataBase * CreateCheckBoxDataItem();

    // 解析Item数据
    static bool ParserCheckBoxItemData(const xml_node & node, CSettingUIItemDataBase * pItemInfo);

public:
    virtual bool SetItemData(const CSettingUIItemDataBase & itemDatas);
    virtual bool GetItemData(CSettingUIItemDataBase & itemDatas);

    virtual bool LoadItemData(CSettingUIItemDataBase & itemDatas);
    virtual bool SaveItemData(const CSettingUIItemDataBase & itemDatas);

public:
    const yl::string & GetOnText();
    void SetOnText(const yl::string & strOnText);
    const yl::string & GetOffText();
    void SetOffText(const yl::string & strOffText);
    const yl::string & GetValueChangedAction();
    void SetValueChangedAction(const yl::string & strValueChangedAction);

protected:
    yl::string      m_strOnText;            // 开关On显示内容
    yl::string      m_strOffText;           // 开关Off显示内容
    yl::string      m_strValueChangedAction;// 值改变事件
};

typedef CSettingUICheckBoxItemData   *  CSettingUICheckBoxItemDataPtr;
#endif

#endif

