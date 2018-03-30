#include "settinguicheckboxitemdata.h"
#include "translateiddefine.h"

#if IF_SUPPORT_CHECK_BOX
CSettingUICheckBoxItemData::CSettingUICheckBoxItemData()
    : CSettingUIItemDataBase(SETTING_ITEM_TYPE_BASE)
    , m_strOnText(TRID_ON)
    , m_strOffText(TRID_OFF)
    , m_strValueChangedAction("")
{
    m_eItemType |= SETTING_ITEM_TYPE_CHECKBOX;
}

CSettingUICheckBoxItemData::~CSettingUICheckBoxItemData()
{
}

int CSettingUICheckBoxItemData::GetCheckBoxDataItemType()
{
    return (SETTING_ITEM_TYPE_CHECKBOX | SETTING_ITEM_TYPE_BASE);
}

CSettingUICheckBoxItemData * CSettingUICheckBoxItemData::GetCheckBoxDataItem(
    CSettingUIItemDataBase * pDataItem)
{
    if (NULL == pDataItem
            || CSettingUICheckBoxItemData::GetCheckBoxDataItemType() != pDataItem->GetItemType())
    {
        return NULL;
    }

    return (CSettingUICheckBoxItemDataPtr)pDataItem;
}

CSettingUIItemDataBase * CSettingUICheckBoxItemData::CreateCheckBoxDataItem()
{
    CSettingUICheckBoxItemDataPtr pCheckBoxDataItem = new CSettingUICheckBoxItemData();

    return pCheckBoxDataItem;
}

bool CSettingUICheckBoxItemData::ParserCheckBoxItemData(const xml_node & node,
        CSettingUIItemDataBase * pItemInfo)
{
    CSettingUICheckBoxItemData * pSwitchInfo = CSettingUICheckBoxItemData::GetCheckBoxDataItem(
                pItemInfo);
    if (NULL != pSwitchInfo && !node.empty())
    {
        pSwitchInfo->SetOnText(node.attribute("ontext").as_string());
        pSwitchInfo->SetOffText(node.attribute("offtext").as_string());
    }

    return CSettingUIItemDataBase::ParserItemData(node, pItemInfo);
}

bool CSettingUICheckBoxItemData::SetItemData(const CSettingUIItemDataBase & itemDatas)
{
    return true;
}

bool CSettingUICheckBoxItemData::GetItemData(CSettingUIItemDataBase & itemDatas)
{
    return true;
}

bool CSettingUICheckBoxItemData::LoadItemData(CSettingUIItemDataBase & itemDatas)
{
    return true;
}

bool CSettingUICheckBoxItemData::SaveItemData(const CSettingUIItemDataBase & itemDatas)
{
    return true;
}

const yl::string & CSettingUICheckBoxItemData::GetOnText()
{
    return m_strOnText;
}

void CSettingUICheckBoxItemData::SetOnText(const yl::string & strOnText)
{
    m_strOnText = strOnText;
}

const yl::string & CSettingUICheckBoxItemData::GetOffText()
{
    return m_strOffText;
}

void CSettingUICheckBoxItemData::SetOffText(const yl::string & strOffText)
{
    m_strOffText = strOffText;
}

const yl::string & CSettingUICheckBoxItemData::GetValueChangedAction()
{
    return m_strValueChangedAction;
}

void CSettingUICheckBoxItemData::SetValueChangedAction(const yl::string & strValueChangedAction)
{
    m_strValueChangedAction = strValueChangedAction;
}
#endif

