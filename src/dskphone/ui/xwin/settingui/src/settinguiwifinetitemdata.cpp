#include "settinguiwifinetitemdata.h"

#ifdef IF_SUPPORT_WIFI
CSettingUIWifiNetItemData::CSettingUIWifiNetItemData()
    : CSettingUIItemDataBase(SETTING_ITEM_TYPE_BASE)
{
    m_eItemType |= SETTING_ITEM_WIFI_NET;
}

CSettingUIWifiNetItemData::~CSettingUIWifiNetItemData()
{
}

int CSettingUIWifiNetItemData::GetWifiNetDataItemType()
{
    return (SETTING_ITEM_WIFI_NET | SETTING_ITEM_TYPE_BASE);
}

CSettingUIWifiNetItemData * CSettingUIWifiNetItemData::GetWifiNetDataItem(
    CSettingUIItemDataBase * pDataItem)
{
    if (NULL == pDataItem
            || CSettingUIWifiNetItemData::GetWifiNetDataItemType() != pDataItem->GetItemType())
    {
        return NULL;
    }

    return (CSettingUIWifiNetItemDataPtr)pDataItem;
}

CSettingUIItemDataBase * CSettingUIWifiNetItemData::CreateWifiNetDataItem()
{
    CSettingUIWifiNetItemDataPtr pBluetoothDevInfo = new CSettingUIWifiNetItemData();

    return pBluetoothDevInfo;
}

const yl::string & CSettingUIWifiNetItemData::GetSignalStrengthIcon()
{
    return m_strSignalStrengthIcon;
}

void CSettingUIWifiNetItemData::SetSignalStrengthIcon(const yl::string & strDevIcon)
{
    m_strSignalStrengthIcon = strDevIcon;
}

const yl::string & CSettingUIWifiNetItemData::GetConnectStatusIcon()
{
    return m_strConnectStatusIcon;
}

void CSettingUIWifiNetItemData::SetConnectStatusIcon(const yl::string & strConnectStatusIcon)
{
    m_strConnectStatusIcon = strConnectStatusIcon;
}

const yl::string & CSettingUIWifiNetItemData::GetEncryptIcon()
{
    return m_strEncryptIcon;
}

void CSettingUIWifiNetItemData::SetEncryptIcon(const yl::string & strEncryptIcon)
{
    m_strEncryptIcon = strEncryptIcon;
}
#endif
