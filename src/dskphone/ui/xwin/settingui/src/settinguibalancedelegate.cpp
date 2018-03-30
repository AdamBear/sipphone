#include "settinguibalancedelegate.h"
#include <settinguilogic/include/settingdefine.h>
#include "settinguilogic/include/pagedatafactory.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settingui/include/itemnamedefine.h"
#include "common/common_data_define.h"
#include "account/include/modaccount.h"
#include "settingui/src/uicallbackfun.h"

#ifdef IF_FEATURE_GETBALANCE

CSettingUIBalanceDelegate::CSettingUIBalanceDelegate()
: CSettingUIDelegateBase()
{
}

CSettingUIBalanceDelegate::~CSettingUIBalanceDelegate()
{
}

CSettingUIDelegateBase* CSettingUIBalanceDelegate::CreateBalanceDelegate()
{
	CSettingUIBalanceDelegate* pStatusDelegate = new CSettingUIBalanceDelegate;

	return (CSettingUIDelegateBasePtr)pStatusDelegate;
}

CSettingUIPageDataBase* CSettingUIBalanceDelegate::LoadPageData(bool bReLoad /*= false*/)
{
	CSettingUIDelegateBase::LoadPageData(bReLoad);

	if (NULL == m_pPageData || !bReLoad)
	{
		return NULL;
	}

	Balance_Info_t tBalanceInfo;
	acc_GetBalanceInfo(tBalanceInfo);

	SetValue(BALANCE_ACCOUNT, tBalanceInfo.szAccount);
	SetValue(BALANCE_VALUE, tBalanceInfo.szBalance);

	g_SetUICallBackFun.RefreshDlgTitle(tBalanceInfo.szName, m_pPageData->GetPageID());

	return m_pPageData;
}

void CSettingUIBalanceDelegate::SetValue(const yl::string& strItemId, const yl::string& strValue)
{
	if (NULL != m_pPageData)
	{
		CSettingUIItemDataBasePtr pItemInfo = m_pPageData->GetItemByItemId(strItemId);
		if (NULL != pItemInfo)
		{
			pItemInfo->SetValue(strValue);
		}
	}
}
#endif
