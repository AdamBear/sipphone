#ifndef __SETTINGUI_RADIO_ITEM_DATA_H__
#define __SETTINGUI_RADIO_ITEM_DATA_H__

#include "settinguilogic/src/settinguimenuitemdata.h"
#include "ylstl/ylstring.h"

enum
{
    // 第2层子类，使用5、6位
    SETTING_ITEM_TYPE_RADIO     = 0x00001000,
};

class CSettingUIRadioItemData : public CSettingUIMenuItemData
{
public:
    explicit CSettingUIRadioItemData();
    virtual ~CSettingUIRadioItemData();

    static int GetRadioDataItemType();
    static CSettingUIRadioItemData * GetRadioListDataItem(CSettingUIItemDataBase * pDataItem);

    static CSettingUIItemDataBase * CreateRadioListDataItem();

    // 解析Item数据
    static bool ParserRadioItemData(const xml_node & node, CSettingUIItemDataBase * pItemInfo);

    bool IsCheckByParserItemData(const yl::string & strCheck, CSettingUIRadioItemData * pRadioInfo);

public:
    void SetCheck(bool bCheck);
    bool IsCheck();
    virtual void Reset();
protected:
    bool m_bCheck;

};

typedef     CSettingUIRadioItemData    *    CSettingUIRadioItemDataPtr;

#endif //__SETTINGUI_RADIO_ITEM_DATA_H__

