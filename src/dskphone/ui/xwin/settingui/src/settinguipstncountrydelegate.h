#ifndef _SETTINGUI_PSTN_COUNTRY_DELEGATE_H_
#define _SETTINGUI_PSTN_COUNTRY_DELEGATE_H_

#ifdef IF_FEATURE_PSTN

#include "basesettinguidelegate.h"

class CSetttinguiPSTNCountryDelegate : public CSettingUIDelegateBase
{
public:
    CSetttinguiPSTNCountryDelegate();
    virtual ~CSetttinguiPSTNCountryDelegate();

    static CSettingUIDelegateBase * CreatePSTNCountryDelegate();
public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
public:
    void LoadListCountryItem();
    void AddRadioListItem(const yl::string & strID, const yl::string & strName);
};

#endif //IF_FEATURE_PSTN
#endif //_SETTINGUI_PSTN_COUNTRY_DELEGATE_H_
