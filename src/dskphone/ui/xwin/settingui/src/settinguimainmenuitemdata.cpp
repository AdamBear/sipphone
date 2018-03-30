#include "settinguidefine.h"
#if IF_SETTING_MAINMENU_IS_TABLEVIEW
#include "settinguimainmenuitemdata.h"

CSettingUIMainMenuItemData::CSettingUIMainMenuItemData()
{
    m_eItemType |= SETTING_ITEM_TYPE_MAIN_MENU;
}

CSettingUIMainMenuItemData::~CSettingUIMainMenuItemData()
{

}

int CSettingUIMainMenuItemData::GetMainMenuDataItemType()
{
    return (SETTING_ITEM_TYPE_MAIN_MENU | SETTING_ITEM_TYPE_BASE | SETTING_ITEM_TYPE_MENU);
}

CSettingUIMainMenuItemData* CSettingUIMainMenuItemData::GetMainMenuDataItem(
    CSettingUIItemDataBase* pDataItem)
{
    if (NULL == pDataItem
            || CSettingUIMainMenuItemData::GetMainMenuDataItemType() != pDataItem->GetItemType())
    {
        return NULL;
    }

    return (CSettingUIMainMenuItemDataPtr)pDataItem;
}

CSettingUIItemDataBase* CSettingUIMainMenuItemData::CreateMainMenuDataItem()
{
    CSettingUIMainMenuItemDataPtr pTextMessageItemData = new CSettingUIMainMenuItemData();

    return pTextMessageItemData;
}

bool  CSettingUIMainMenuItemData::ParserMainMenuItemData(const xml_node& node,
        CSettingUIItemDataBase* pItemInfo)
{
    CSettingUIMainMenuItemDataPtr pItem = GetMainMenuDataItem(pItemInfo);
    if (NULL != pItem && !node.empty())
    {
        pItem->SetItemType(GetMainMenuDataItemType());
        pItem->SetPicPath(node.attribute("icon").as_string());
        pItem->SetClickAction(node.attribute("action").as_string());
    }

    return CSettingUIMenuItemData::ParserMenuItemData(node, pItemInfo);
}

const yl::string& CSettingUIMainMenuItemData::GetPicPath() const
{
    return m_strPicPath;
}

void CSettingUIMainMenuItemData::SetPicPath(const yl::string& strPicPath)
{
    m_strPicPath = strPicPath;
}
#endif
