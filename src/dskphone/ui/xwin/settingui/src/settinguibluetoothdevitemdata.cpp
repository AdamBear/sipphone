#include "settinguibluetoothdevitemdata.h"

#ifdef IF_SUPPORT_BLUETOOTH
CSettingUIBluetoothDevItemData::CSettingUIBluetoothDevItemData()
    : CSettingUIItemDataBase(SETTING_ITEM_TYPE_BASE)
{
    m_eItemType |= SETTING_ITEM_BLUETOOTH_DEV;
}

CSettingUIBluetoothDevItemData::~CSettingUIBluetoothDevItemData()
{
}

int CSettingUIBluetoothDevItemData::GetBluetoothDevDataItemType()
{
    return (SETTING_ITEM_BLUETOOTH_DEV | SETTING_ITEM_TYPE_BASE);
}

CSettingUIBluetoothDevItemData * CSettingUIBluetoothDevItemData::GetBluetoothDevDataItem(
    CSettingUIItemDataBase * pDataItem)
{
    if (NULL == pDataItem
            || CSettingUIBluetoothDevItemData::GetBluetoothDevDataItemType() != pDataItem->GetItemType())
    {
        return NULL;
    }

    return (CSettingUIBluetoothDevItemDataPtr)pDataItem;
}

CSettingUIItemDataBase * CSettingUIBluetoothDevItemData::CreateBluetoothDevDataItem()
{
    CSettingUIBluetoothDevItemDataPtr pBluetoothDevInfo = new CSettingUIBluetoothDevItemData();

    return pBluetoothDevInfo;
}

const yl::string & CSettingUIBluetoothDevItemData::GetDevIcon()
{
    return m_strDevIcon;
}

void CSettingUIBluetoothDevItemData::SetDevIcon(const yl::string & strDevIcon)
{
    m_strDevIcon = strDevIcon;
}

const yl::string & CSettingUIBluetoothDevItemData::GetConnectStatusIcon()
{
    return m_strConnectStatusIcon;
}

void CSettingUIBluetoothDevItemData::SetConnectStatusIcon(const yl::string & strConnectStatusIcon)
{
    m_strConnectStatusIcon = strConnectStatusIcon;
}

const yl::string & CSettingUIBluetoothDevItemData::GetMac()
{
    return m_strMac;
}

void CSettingUIBluetoothDevItemData::SetMac(const yl::string & strMac)
{
    m_strMac = strMac;
}
#endif
