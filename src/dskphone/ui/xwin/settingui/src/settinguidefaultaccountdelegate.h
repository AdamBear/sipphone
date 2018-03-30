#ifndef _SETTINGUI_DEFAULT_ACCOUNT_DELEGATE_H_
#define _SETTINGUI_DEFAULT_ACCOUNT_DELEGATE_H_

#include "settinguidefine.h"
#if IF_SUPPORT_DEFAULT_ACCOUNT_DIALOG
#include "settinguidatadelegatebase.h"

class CSettingUIComboBoxItemData;

class CSettinguiDefaultAccountListDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettinguiDefaultAccountListDelegate();
    virtual ~CSettinguiDefaultAccountListDelegate();

    static CSettingUIDelegateBase* CreateDefaultAccountListDelegate();
public:
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase*  pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
private:
    void LoadAccountList();
    void AddComboboxItemValue(CSettingUIComboBoxItemData* pComboBoxItem,
                              const yl::string& strDisplayName, const int& nValue);
};

#endif

#endif //_SETTINGUI_DEFAULT_ACCOUNT_DELEGATE_H_
