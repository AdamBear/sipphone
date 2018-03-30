#ifndef __SETTINGUI_STATUS_NETWORK_IPV4_DELEGATE_H__
#define __SETTINGUI_STATUS_NETWORK_IPV4_DELEGATE_H__

#include "basesettinguidelegate.h"

class CSettingUIStatusNetIPv4Delegate : public CSettingUIDelegateBase
{
public:
    CSettingUIStatusNetIPv4Delegate();
    virtual ~CSettingUIStatusNetIPv4Delegate();

    static CSettingUIDelegateBase * CreateStatusNetIPv4Delegate();

    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);

private:
    void SetValue(const yl::string & strItemId, const yl::string & strValue);
};

typedef CSettingUIStatusNetIPv4Delegate * CSettingUIStatusNetIPv4DelegatePtr;

#endif // __SETTINGUI_STATUS_NETWORK_IPV4_DELEGATE_H__
