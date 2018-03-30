#include "settinguimenudetailitemdata.h"


CSettingUIDetailMenuItemData::CSettingUIDetailMenuItemData()
    : CSettingUIMenuItemData()
{
    m_eItemType |= SETTING_ITEM_TYPE_DETAIL_MENU;
}

CSettingUIDetailMenuItemData::~CSettingUIDetailMenuItemData()
{
}

int CSettingUIDetailMenuItemData::GetDetailMenuDataItemType()
{
    return (SETTING_ITEM_TYPE_MENU | SETTING_ITEM_TYPE_DETAIL_MENU | SETTING_ITEM_TYPE_BASE);
}

CSettingUIDetailMenuItemData * CSettingUIDetailMenuItemData::GetDetailMenuListDataItem(
    CSettingUIItemDataBase * pDataItem)
{
    if (NULL == pDataItem
            || CSettingUIDetailMenuItemData::GetDetailMenuDataItemType() != pDataItem->GetItemType())
    {
        return NULL;
    }

    return (CSettingUIDetailMenuItemDataPtr)pDataItem;
}

CSettingUIItemDataBase * CSettingUIDetailMenuItemData::CreateDetailMenuListDataItem()
{
    CSettingUIDetailMenuItemDataPtr pDetailMenuDataItem = new CSettingUIDetailMenuItemData();

    return pDetailMenuDataItem;
}

bool CSettingUIDetailMenuItemData::ParserDetailMenuItemData(const xml_node & node,
        CSettingUIItemDataBase * pItemInfo)
{
    CSettingUIDetailMenuItemData * pDetailMenuInfo =
        CSettingUIDetailMenuItemData::GetDetailMenuListDataItem(pItemInfo);
    if (NULL != pDetailMenuInfo && !node.empty())
    {
        pDetailMenuInfo->SetClickAction(node.attribute("action").as_string());
        pDetailMenuInfo->SetFocusAction(node.attribute("focusaction").as_string());
    }

    return CSettingUIMenuItemData::ParserMenuItemData(node, pItemInfo);
}
