#ifndef _SETTINGUI_STATUS_PHONE_DELEGATE_H_
#define _SETTINGUI_STATUS_PHONE_DELEGATE_H_

#include "settinguidatadelegatebase.h"

class CSettingUIStatusPhoneDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettingUIStatusPhoneDelegate();
    virtual ~CSettingUIStatusPhoneDelegate();

    static CSettingUIDelegateBase * CreateStatusPhoneDelegate();

    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);

private:
    bool GetDeviceCertStatus();
};

typedef CSettingUIStatusPhoneDelegate * CSettingUIStatusPhoneDelegatePtr;

#endif //_SETTINGUI_STATUS_PHONE_DELEGATE_H_
