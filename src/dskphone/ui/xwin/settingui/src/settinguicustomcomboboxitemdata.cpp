#include "settinguicustomcomboboxitemdata.h"
#include "commonapi/stringhelperapi.h"
#include "dsklog/log.h"

#if IF_SUPPORT_CUSTOM_COMBO_BOX
CSettingUICustomComboBoxItemData::CSettingUICustomComboBoxItemData()
    : CSettingUIComboBoxItemData()
    , m_strClickAction("")
{
    m_eItemType |= SETTING_ITEM_TYPE_COMBOBOX_CUSTOM;
}

CSettingUICustomComboBoxItemData::~CSettingUICustomComboBoxItemData()
{
}

int CSettingUICustomComboBoxItemData::GetCustomComboBoxDataItemType()
{
    return (SETTING_ITEM_TYPE_COMBOBOX_CUSTOM | SETTING_ITEM_TYPE_COMBOBOX | SETTING_ITEM_TYPE_BASE);
}

CSettingUICustomComboBoxItemData * CSettingUICustomComboBoxItemData::GetCustomComboBoxDataItem(
    CSettingUIItemDataBase * pDataItem)
{
    if (NULL == pDataItem
            || CSettingUICustomComboBoxItemData::GetCustomComboBoxDataItemType() != pDataItem->GetItemType())
    {
        return NULL;
    }

    return (CSettingUICustomComboBoxItemDataPtr)pDataItem;
}

CSettingUIItemDataBase * CSettingUICustomComboBoxItemData::CreateCustomComboBoxDataItem()
{
    CSettingUICustomComboBoxItemDataPtr pCustomComboBoxDataItem = new
    CSettingUICustomComboBoxItemData();

    return pCustomComboBoxDataItem;
}

bool CSettingUICustomComboBoxItemData::ParserCustomComboBoxItemData(const xml_node & node,
        CSettingUIItemDataBase * pItemInfo)
{
    CSettingUICustomComboBoxItemData * pCmbInfo =
        CSettingUICustomComboBoxItemData::GetCustomComboBoxDataItem(pItemInfo);
    if (NULL != pCmbInfo && !node.empty())
    {
        pCmbInfo->SetDataChangedAction(node.attribute("datachangeaction").as_string());
        pCmbInfo->SetClickAction(node.attribute("action").as_string());

        xml_node itemNode = node.first_child();
        while (itemNode)
        {
            if (itemNode.empty())
            {
                itemNode = itemNode.next_sibling();
                continue;
            }

            CSettingUIComboBoxOption * pCmbOption = new CSettingUIComboBoxOption();
            if (NULL != pCmbOption)
            {
                pCmbOption->m_strDisplayName = itemNode.attribute("displayname").as_string();
                pCmbOption->m_strItemVale = itemNode.attribute("value").as_string();
                pCmbOption->m_strVisiableID = itemNode.attribute("visibleid").as_string();

                commonAPI_splitStringBySeparator(itemNode.attribute("layoutmember").as_string(), ',',
                                                 pCmbOption->m_listLayoutMembers);

                pCmbInfo->AddOptionItem(pCmbOption);
            }

            itemNode = itemNode.next_sibling();
        }
    }

    return CSettingUIComboBoxItemData::ParserComboBoxItemData(node, pItemInfo);
}

const yl::string & CSettingUICustomComboBoxItemData::GetClickAction() const
{
    return m_strClickAction;
}

void CSettingUICustomComboBoxItemData::SetClickAction(const yl::string & strClickAction)
{
    m_strClickAction = strClickAction;
}

const yl::string & CSettingUICustomComboBoxItemData::GetItemAction()
{
    return GetClickAction();
}
#endif
