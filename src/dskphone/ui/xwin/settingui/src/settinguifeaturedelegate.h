#ifndef _SETTINGUI_FEATURE_DELEGATE_H_
#define _SETTINGUI_FEATURE_DELEGATE_H_

#include "basesettinguidelegate.h"

class CSettingUIFeatureDelegate : public CSettingUIDelegateBase
{
public:
    CSettingUIFeatureDelegate();
    virtual ~CSettingUIFeatureDelegate();

    static CSettingUIDelegateBase * CreateFeatureDelegate();
public:
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual void OnEnterPage(const yl::string & strPage, int iDataIndex);
private:
    int GetAvailableLineID();
};

#endif //_SETTINGUI_FEATURE_DELEGATE_H_

