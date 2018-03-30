#ifndef _SETTINGUI_BT_CHANNEL_CONTROLL_ITEMDATA_H_
#define _SETTINGUI_BT_CHANNEL_CONTROLL_ITEMDATA_H_

#ifdef IF_SUPPORT_BLUETOOTH
#include "settinguilogic/src/settinguicomboboxitemdata.h"

enum
{
    // 第3层子类，使用5、6位
    SETTING_ITEM_BT_CHANNEL_CONTROLL = 0x00002200,
};

class CSettingUIBTChannelControllItemData : public CSettingUIComboBoxItemData
{
public:
    explicit CSettingUIBTChannelControllItemData();
    virtual ~CSettingUIBTChannelControllItemData();

    static int GetBTChannelControllDataItemType();
    static CSettingUIBTChannelControllItemData * GetBTChannelControllDataItem(
        CSettingUIItemDataBase * pDataItem);
    static CSettingUIItemDataBase * CreateBTChannelControllDataItem();
public:
    virtual bool IsNeedSaveData()
    {
        return true;
    }
    void SetExtraDispname(const yl::string & strDiap);
    yl::string GetExtraDispname();
    void SetExtraValue(const yl::string & strValue);
    yl::string GetExtraValue();
protected:
    yl::string m_strExtraDiapname;
    yl::string m_strExtraValue;

};

typedef CSettingUIBTChannelControllItemData * CSettingUIBTChannelControllItemDataPtr;
#endif

#endif //_SETTINGUI_BT_CHANNEL_CONTROLL_ITEMDATA_H_
