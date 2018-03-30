#include "settinguimenuitemdata.h"


CSettingUIMenuItemData::CSettingUIMenuItemData()
    : CSettingUIItemDataBase(SETTING_ITEM_TYPE_BASE)
    , m_strClickAction("")
    , m_strFocusAction("")
{
    m_eItemType |= SETTING_ITEM_TYPE_MENU;
}

CSettingUIMenuItemData::~CSettingUIMenuItemData()
{
}

int CSettingUIMenuItemData::GetMenuDataItemType()
{
    return (SETTING_ITEM_TYPE_MENU | SETTING_ITEM_TYPE_BASE);
}

CSettingUIMenuItemData * CSettingUIMenuItemData::GetMenuListDataItem(CSettingUIItemDataBase *
        pDataItem)
{
    if (NULL == pDataItem
            || CSettingUIMenuItemData::GetMenuDataItemType() != pDataItem->GetItemType())
    {
        return NULL;
    }

    return (CSettingUIMenuItemDataPtr)pDataItem;
}

CSettingUIItemDataBase * CSettingUIMenuItemData::CreateMenuListDataItem()
{
    CSettingUIMenuItemDataPtr pMenuDataItem = new CSettingUIMenuItemData();

    return pMenuDataItem;
}

bool CSettingUIMenuItemData::ParserMenuItemData(const xml_node & node,
        CSettingUIItemDataBase * pItemInfo)
{
    CSettingUIMenuItemData * pMenuInfo = CSettingUIMenuItemData::GetMenuListDataItem(pItemInfo);
    if (NULL != pMenuInfo && !node.empty())
    {
        pMenuInfo->SetClickAction(node.attribute("action").as_string());
        pMenuInfo->SetFocusAction(node.attribute("focusaction").as_string());
    }

    return CSettingUIItemDataBase::ParserItemData(node, pItemInfo);
}

void CSettingUIMenuItemData::Reset()
{
    CSettingUIItemDataBase::Reset();
    m_strClickAction = "";
}

bool CSettingUIMenuItemData::SetItemData(const CSettingUIItemDataBase & itemDatas)
{
    return true;
}

bool CSettingUIMenuItemData::GetItemData(CSettingUIItemDataBase & itemDatas)
{
    return true;
}

bool CSettingUIMenuItemData::LoadItemData(CSettingUIItemDataBase & itemDatas)
{
    return true;
}

bool CSettingUIMenuItemData::SaveItemData(const CSettingUIItemDataBase & itemDatas)
{
    return true;
}

const yl::string & CSettingUIMenuItemData::GetItemAction()
{
    return GetClickAction();
}

const yl::string & CSettingUIMenuItemData::GetItemFocusAction()
{
    return GetFocusAction();
}

const yl::string & CSettingUIMenuItemData::GetClickAction() const
{
    return m_strClickAction;
}

void CSettingUIMenuItemData::SetClickAction(const yl::string & strClickAction)
{
    m_strClickAction = strClickAction;
}

const yl::string & CSettingUIMenuItemData::GetFocusAction() const
{
    return m_strFocusAction;
}

void CSettingUIMenuItemData::SetFocusAction(const yl::string & strFocusAction)
{
    m_strFocusAction = strFocusAction;
}

