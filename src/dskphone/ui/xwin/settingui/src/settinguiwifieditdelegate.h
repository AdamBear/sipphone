#ifndef _SETTINGUI_WIFI_EDIT_DELEGATE_H_
#define _SETTINGUI_WIFI_EDIT_DELEGATE_H_

#ifdef IF_SUPPORT_WIFI

#include "settinguibtwifidelegate.h"

class CSettingUIWifiEditDelegate : public CSettingUIBtWifiDelegate
{
public:
    CSettingUIWifiEditDelegate();
    virtual ~CSettingUIWifiEditDelegate();

    static CSettingUIDelegateBase * CreateWifiEditDelegate();
public:
    virtual void OnComboBoxValueChange(int nIndex);
};

#endif
#endif //_SETTINGUI_WIFI_EDIT_DELEGATE_H_
