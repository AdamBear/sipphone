#include "settinguicomboboxitemdata.h"
#include "commonapi/stringhelperapi.h"

/////////////////////////CSettingUIComboBoxOption///////////////////////
CSettingUIComboBoxOption::CSettingUIComboBoxOption()
    : m_strDisplayName("")
    , m_strVisiableID("")
    , m_strItemVale("")
    , m_listLayoutMembers()
{
}

CSettingUIComboBoxOption::~CSettingUIComboBoxOption()
{
}


/////////////////////////CSettingUIComboBoxItemData///////////////////////
CSettingUIComboBoxItemData::CSettingUIComboBoxItemData()
    : CSettingUIItemDataBase(SETTING_ITEM_TYPE_BASE)
    , m_strDataChangedAction("")
    , m_listOptions()
    , m_bSwitchSoftKey(false)
    , m_bSetupSoftkeyByItem(true)
{
    m_eItemType |= SETTING_ITEM_TYPE_COMBOBOX;
}

CSettingUIComboBoxItemData::~CSettingUIComboBoxItemData()
{
    ClearOptionItems();
}

int CSettingUIComboBoxItemData::GetComboBoxDataItemType()
{
    return (SETTING_ITEM_TYPE_COMBOBOX | SETTING_ITEM_TYPE_BASE);
}

CSettingUIComboBoxItemData * CSettingUIComboBoxItemData::GetComboBoxDataItem(
    CSettingUIItemDataBase * pDataItem)
{
    if (NULL == pDataItem
            || CSettingUIComboBoxItemData::GetComboBoxDataItemType() != pDataItem->GetItemType())
    {
        return NULL;
    }

    return (CSettingUIComboBoxItemDataPtr)pDataItem;
}

CSettingUIItemDataBase * CSettingUIComboBoxItemData::CreateComboBoxDataItem()
{
    CSettingUIComboBoxItemDataPtr pComboBoxDataItem = new CSettingUIComboBoxItemData();

    return pComboBoxDataItem;
}

bool CSettingUIComboBoxItemData::ParserComboBoxItemData(const xml_node & node,
        CSettingUIItemDataBase * pItemInfo)
{
    CSettingUIComboBoxItemData * pCmbInfo = CSettingUIComboBoxItemData::GetComboBoxDataItem(pItemInfo);
    if (NULL != pCmbInfo && !node.empty())
    {
        pCmbInfo->SetDataChangedAction(node.attribute("datachangeaction").as_string());
        pCmbInfo->SetSwitchSoftKey(node.attribute("switchsoftkey").as_bool());

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

    return CSettingUIItemDataBase::ParserItemData(node, pItemInfo);
}

bool CSettingUIComboBoxItemData::SetItemData(const CSettingUIItemDataBase & itemDatas)
{
    return true;
}

bool CSettingUIComboBoxItemData::GetItemData(CSettingUIItemDataBase & itemDatas)
{
    return true;
}

bool CSettingUIComboBoxItemData::LoadItemData(CSettingUIItemDataBase & itemDatas)
{
    return true;
}

bool CSettingUIComboBoxItemData::SaveItemData(const CSettingUIItemDataBase & itemDatas)
{
    return true;
}

const yl::string & CSettingUIComboBoxItemData::GetDataChangedAction()
{
    return m_strDataChangedAction;
}

void CSettingUIComboBoxItemData::SetDataChangedAction(const yl::string & strDataChangedAction)
{
    m_strDataChangedAction = strDataChangedAction;
}

bool CSettingUIComboBoxItemData::AddOptionItem(CSettingUIComboBoxOption * pOptionData)
{
    if (!IsOptionExist(pOptionData))
    {
        m_listOptions.push_back(pOptionData);
        return true;
    }

    return false;
}

bool CSettingUIComboBoxItemData::DelOptionItem(CSettingUIComboBoxOption * pOptionData)
{
    if (NULL == pOptionData)
    {
        return false;
    }

    ListComboBoxOptionData::iterator iter = m_listOptions.begin();
    for (; iter != m_listOptions.end(); ++iter)
    {
        if (0 == pOptionData->m_strDisplayName.compare((*iter)->m_strDisplayName))
        {
            m_listOptions.erase(iter);
            delete pOptionData;
            pOptionData = NULL;
            return true;
        }
    }

    return false;
}

void CSettingUIComboBoxItemData::ClearOptionItems()
{
    ListComboBoxOptionData::iterator iter = m_listOptions.begin();
    for (; iter != m_listOptions.end();)
    {
        CSettingUIComboBoxOption * pOptionItemData = *iter;
        if (NULL != *iter)
        {
            iter = m_listOptions.erase(iter);
            if (NULL != pOptionItemData)
            {
                delete pOptionItemData;
            }
        }
        else
        {
            ++iter;
        }
    }

    m_listOptions.clear();
}

ListComboBoxOptionData CSettingUIComboBoxItemData::GetComboBoxOptionData()
{
    return m_listOptions;
}

bool CSettingUIComboBoxItemData::IsSwitchSoftKey()
{
    return m_bSwitchSoftKey;
}

void CSettingUIComboBoxItemData::SetSwitchSoftKey(bool bSwitch)
{
    m_bSwitchSoftKey = bSwitch;
}

bool CSettingUIComboBoxItemData::IsOptionExist(CSettingUIComboBoxOption * pOptionData)
{
    ListComboBoxOptionData::iterator iter = m_listOptions.begin();
    for (; iter != m_listOptions.end(); ++iter)
    {
        if (0 == pOptionData->m_strDisplayName.compare((*iter)->m_strDisplayName))
        {
            return true;
        }
    }

    return false;
}

bool CSettingUIComboBoxItemData::IsSetupSoftkeyByItem()
{
    return m_bSetupSoftkeyByItem;
}

void CSettingUIComboBoxItemData::SetIsSetupSoftkeyByItem(bool bSetupSoftkeyByItem)
{
    m_bSetupSoftkeyByItem = bSetupSoftkeyByItem;
}
