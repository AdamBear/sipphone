#ifndef _SETTINGUI_CONTRAST_DELEGATE_H_
#define _SETTINGUI_CONTRAST_DELEGATE_H_

#include "settinguidatadelegatebase.h"

class CSettinguiContrastDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettinguiContrastDelegate();
    virtual ~CSettinguiContrastDelegate();

    static CSettingUIDelegateBase * CreateContrastDelegate();
public:
    virtual void OnComboBoxValueChange(int nIndex);
protected:
    void OnLeavePage();
};

#endif //_SETTINGUI_CONTRAST_DELEGATE_H_
