#ifndef __SETTINGUI_BALANCE_DELEGATE_H__
#define __SETTINGUI_BALANCE_DELEGATE_H__

#ifdef IF_FEATURE_GETBALANCE
#include "basesettinguidelegate.h"

class CSettingUIBalanceDelegate : public CSettingUIDelegateBase
{
public:
	CSettingUIBalanceDelegate();
	virtual ~CSettingUIBalanceDelegate();

	static CSettingUIDelegateBase* CreateBalanceDelegate();

	virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);

private:
	void SetValue(const yl::string& strItemId, const yl::string& strValue);
};

typedef CSettingUIBalanceDelegate* CSettingUIBalanceDelegatePtr;
#endif

#endif // __SETTINGUI_BALANCE_DELEGATE_H__
