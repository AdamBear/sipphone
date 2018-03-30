#include "settinguitextmessageitemdata.h"


CSettingUITextMessageItemData::CSettingUITextMessageItemData()
    : CSettingUIItemDataBase(SETTING_ITEM_TYPE_BASE)
    , m_bUrgent(false)
{
    m_eItemType |= SETTING_ITEM_TYPE_MESSAGE;
}

CSettingUITextMessageItemData::~CSettingUITextMessageItemData()
{
}

int CSettingUITextMessageItemData::GetTextMessageDataItemType()
{
    return (SETTING_ITEM_TYPE_MESSAGE | SETTING_ITEM_TYPE_BASE);
}

CSettingUITextMessageItemData * CSettingUITextMessageItemData::GetTextMessageDataItem(
    CSettingUIItemDataBase * pDataItem)
{
    if (NULL == pDataItem
            || CSettingUITextMessageItemData::GetTextMessageDataItemType() != pDataItem->GetItemType())
    {
        return NULL;
    }

    return (CSettingUITextMessageItemDataPtr)pDataItem;
}

CSettingUIItemDataBase * CSettingUITextMessageItemData::CreateTextMessageDataItem()
{
    CSettingUITextMessageItemDataPtr pTextMessageItemData = new CSettingUITextMessageItemData();

    return pTextMessageItemData;
}

const yl::string & CSettingUITextMessageItemData::GetStatusIcon()
{
    return m_strIcon;
}

void CSettingUITextMessageItemData::SetStatusIcon(const yl::string & strIcon)
{
    m_strIcon = strIcon;
}

const yl::string & CSettingUITextMessageItemData::GetDateString()
{
    return m_strDate;
}

void CSettingUITextMessageItemData::SetDateString(const yl::string & strDate)
{
    m_strDate = strDate;
}

bool CSettingUITextMessageItemData::GetUrgent()
{
    return m_bUrgent;
}

void CSettingUITextMessageItemData::SetUrgent(bool bUrgent)
{
    m_bUrgent = bUrgent;
}
