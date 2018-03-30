#ifndef _SETTINGUI_ADV_PSTN_ACCOUNT_SETTING_DELEGATE_H_
#define _SETTINGUI_ADV_PSTN_ACCOUNT_SETTING_DELEGATE_H_

#ifdef IF_FEATURE_PSTN

#include "settinguiaccountbasedelegate.h"

class CAdvPSTNAccountSettingDelegate : public CSettingUIAccountDelegateBase
{
public:
    CAdvPSTNAccountSettingDelegate();
    ~CAdvPSTNAccountSettingDelegate();

    static CSettingUIDelegateBase * CreateAdvPSTNAccountSettingDelegate();
public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
};

#endif //IF_FEATURE_PSTN
#endif //_SETTINGUI_ADV_PSTN_ACCOUNT_SETTING_DELEGATE_H_
