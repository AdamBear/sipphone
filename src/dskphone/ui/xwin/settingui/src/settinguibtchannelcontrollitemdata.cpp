#include "settinguibtchannelcontrollitemdata.h"

#ifdef IF_SUPPORT_BLUETOOTH
CSettingUIBTChannelControllItemData::CSettingUIBTChannelControllItemData()
{
    m_eItemType |= SETTING_ITEM_BT_CHANNEL_CONTROLL;
}

CSettingUIBTChannelControllItemData::~CSettingUIBTChannelControllItemData()
{

}

int CSettingUIBTChannelControllItemData::GetBTChannelControllDataItemType()
{
    return (SETTING_ITEM_TYPE_COMBOBOX | SETTING_ITEM_TYPE_BASE | SETTING_ITEM_BT_CHANNEL_CONTROLL);
}

CSettingUIBTChannelControllItemData *
CSettingUIBTChannelControllItemData::GetBTChannelControllDataItem(CSettingUIItemDataBase *
        pDataItem)
{
    if (NULL == pDataItem
            || CSettingUIBTChannelControllItemData::GetBTChannelControllDataItemType() !=
            pDataItem->GetItemType())
    {
        return NULL;
    }

    return (CSettingUIBTChannelControllItemDataPtr)pDataItem;
}

CSettingUIItemDataBase * CSettingUIBTChannelControllItemData::CreateBTChannelControllDataItem()
{
    CSettingUIBTChannelControllItemDataPtr pComboBoxDataItem = new
    CSettingUIBTChannelControllItemData();

    return pComboBoxDataItem;
}

void CSettingUIBTChannelControllItemData::SetExtraDispname(const yl::string & strDiap)
{
    m_strExtraDiapname = strDiap;
}

yl::string CSettingUIBTChannelControllItemData::GetExtraDispname()
{
    return m_strExtraDiapname;
}

void CSettingUIBTChannelControllItemData::SetExtraValue(const yl::string & strValue)
{
    m_strExtraValue = strValue;
}

yl::string CSettingUIBTChannelControllItemData::GetExtraValue()
{
    return m_strExtraValue;
}
#endif
