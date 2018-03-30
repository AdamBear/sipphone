#include "settinguibaseaccountitemdata.h"

#include "datafactory.h"
#include "configfiltermanager.h"
#include "itemtypemanager.h"

CSettingUIAccountItemDataBase::CSettingUIAccountItemDataBase(int eItemType)
    : CSettingUIItemDataBase(SETTING_ITEM_TYPE_BASE)
    , m_iLineID(-1)
{
    m_eItemType |= eItemType;
}

CSettingUIAccountItemDataBase::~CSettingUIAccountItemDataBase()
{
}

void CSettingUIAccountItemDataBase::Reset()
{
    CSettingUIItemDataBase::Reset();
    m_iLineID = -1;
}

int CSettingUIAccountItemDataBase::GetBaseAccountDataItemType()
{
    return (SETTING_ITEM_TYPE_ACCOUNT_BASE | SETTING_ITEM_TYPE_BASE);
}

CSettingUIAccountItemDataBase * CSettingUIAccountItemDataBase::GetBaseAccountDataItem(
    CSettingUIItemDataBase * pDataItem)
{
    if (NULL == pDataItem
            || CSettingUIAccountItemDataBase::GetBaseAccountDataItemType() != pDataItem->GetItemType())
    {
        return NULL;
    }

    return (CSettingUIAccountItemDataBasePtr)pDataItem;
}

CSettingUIItemDataBase * CSettingUIAccountItemDataBase::CreateBaseAccountDataItem()
{
    CSettingUIAccountItemDataBasePtr pBaseAccountDataItem = new CSettingUIAccountItemDataBase(
        SETTING_ITEM_TYPE_ACCOUNT_BASE);

    return pBaseAccountDataItem;
}

bool CSettingUIAccountItemDataBase::ParserAccountItemData(const xml_node & node,
        CSettingUIAccountItemDataBase * pItemInfo)
{
    if (NULL == pItemInfo || node.empty())
    {
        return false;
    }

    //return CSettingUIItemDataBase::ParserItemData(node, pItemInfo);
    return true;
}

int CSettingUIAccountItemDataBase::GetLineID()
{
    return m_iLineID;
}

void CSettingUIAccountItemDataBase::SetLineID(int iLineID)
{
    m_iLineID = iLineID;
}

