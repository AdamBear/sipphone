#include "settinguidefaultaccountdelegate.h"

#if IF_SUPPORT_DEFAULT_ACCOUNT_DIALOG
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"

#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "account/include/modaccount.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettinguiDefaultAccountListDelegate::CSettinguiDefaultAccountListDelegate()
{
}

CSettinguiDefaultAccountListDelegate::~CSettinguiDefaultAccountListDelegate()
{
}

CSettingUIDelegateBase* CSettinguiDefaultAccountListDelegate::CreateDefaultAccountListDelegate()
{
    CSettinguiDefaultAccountListDelegate* pDelegate = new CSettinguiDefaultAccountListDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}


CSettingUIPageDataBase* CSettinguiDefaultAccountListDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIPageDataBase* pPageData = CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL == pPageData)
    {
        return NULL;
    }

    if (bReLoad)
    {
        LoadAccountList();
    }

    return pPageData;
}

bool CSettinguiDefaultAccountListDelegate::SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    int nAccountId = GetComboboxValue(DEFAULT_ACCOUNT);
    acc_SetDefaultAccount(nAccountId, true);

    return CSettinguiDataDelegateBase::SavePageData(pPageData, bBackToPrevious, bPopHint);
}

void CSettinguiDefaultAccountListDelegate::LoadAccountList()
{
    if (NULL == m_pPageData)
    {
        return;
    }

    CSettingUIItemDataBasePtr pItemInfo = m_pPageData->GetItemByItemId(DEFAULT_ACCOUNT);
    CSettingUIComboBoxItemDataPtr pComboboxItem = CSettingUIComboBoxItemData::GetComboBoxDataItem(pItemInfo);
    if (NULL == pComboboxItem)
    {
        return;
    }

    //获取有效账号
    ListAccounts listAccounts = acc_GetAvailableAccountList();
    if (0 == listAccounts.size())
    {
        AddComboboxItemValue(pComboboxItem, _LANG(TRID_NO_ACCOUNT), INVALID_ACCOUNTID);
    }
    else
    {
        for (ListAccounts::iterator iter = listAccounts.begin(); iter != listAccounts.end(); ++iter)
        {
            int iAccount = *iter;
            yl::string strAccountName = acc_GetUsername(iAccount) + "@" + acc_GetServerName(iAccount);
            AddComboboxItemValue(pComboboxItem, strAccountName, iAccount);
        }
    }

    int nDefAccID = acc_GetDefaultAccount();
    if (acc_IsDirectIPAccount(nDefAccID))
    {
        nDefAccID = INVALID_ACCOUNTID;
    }

    SetComboboxValue(DEFAULT_ACCOUNT, nDefAccID);
}

void CSettinguiDefaultAccountListDelegate::AddComboboxItemValue(CSettingUIComboBoxItemData* pComboBoxItem,
        const yl::string& strDisplayName, const int& nValue)
{
    if (NULL == pComboBoxItem)
    {
        return;
    }

    CSettingUIComboBoxOption* pOption = new CSettingUIComboBoxOption;

    if (NULL == pOption)
    {
        return;
    }

    pOption->m_strDisplayName = strDisplayName;
    pOption->m_strItemVale = commonAPI_FormatString("%d", nValue);

    if (!pComboBoxItem->AddOptionItem(pOption))
    {
        delete pOption;
    }
}

#endif
