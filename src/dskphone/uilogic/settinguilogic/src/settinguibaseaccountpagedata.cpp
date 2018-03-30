#include "settinguibaseaccountpagedata.h"
#include "settinguibaseaccountitemdata.h"
#include "dsklog/log.h"

CSettingUIAccountPageDataBase::CSettingUIAccountPageDataBase()
    : CSettingUIPageDataBase(SETTING_PAGE_DATA_TYPE_BASE)
    , m_iLineID(-1)
{
    m_ePageDataType |= SETTING_PAGE_DATA_TYPE_ACCOUNT;
}

CSettingUIAccountPageDataBase::~CSettingUIAccountPageDataBase()
{
}

void CSettingUIAccountPageDataBase::Reset()
{
    CSettingUIPageDataBase::Reset();
    m_iLineID = -1;
}

int CSettingUIAccountPageDataBase::GetAccountPageDataType()
{
    return (SETTING_PAGE_DATA_TYPE_ACCOUNT | SETTING_PAGE_DATA_TYPE_BASE);
}

CSettingUIAccountPageDataBase * CSettingUIAccountPageDataBase::GetAccountPageData(
    CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData
            || CSettingUIAccountPageDataBase::GetAccountPageDataType() != pPageData->GetPageDataType())
    {
        return NULL;
    }

    return (CSettingUIAccountPageDataBasePtr)pPageData;
}

CSettingUIPageDataBase * CSettingUIAccountPageDataBase::CreateAccountPageData()
{
    CSettingUIAccountPageDataBasePtr pAccountPageData = new CSettingUIAccountPageDataBase();

    return pAccountPageData;
}

int CSettingUIAccountPageDataBase::GetItemLineIDByDataIndex(int nDataIndex)
{
    if (nDataIndex < 0)
    {
        return -1;
    }

    CSettingUIItemDataBasePtr pItemData = GetItemByDataIndex(nDataIndex);
    CSettingUIAccountItemDataBasePtr pAccontBase =
        CSettingUIAccountItemDataBase::GetBaseAccountDataItem(pItemData);

    if (NULL == pAccontBase)
    {
        return -1;
    }

    return pAccontBase->GetLineID();
}

int CSettingUIAccountPageDataBase::GetLineID()
{
    return m_iLineID;
}

void CSettingUIAccountPageDataBase::SetLineID(int iLineID)
{
#if IF_SETTING_DISPLAY_DEBUG
    SETTINGUI_INFO("------------- [CSettingUIAccountPageDataBase::SetLineID] lineID[%d] ============"
                   , iLineID);
#endif
    m_iLineID = iLineID;
}

