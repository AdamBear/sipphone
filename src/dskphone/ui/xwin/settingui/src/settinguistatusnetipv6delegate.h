#ifndef __SETTINGUI_STATUS_NETWORK_IPV6_DELEGATE_H__
#define __SETTINGUI_STATUS_NETWORK_IPV6_DELEGATE_H__

#include "basesettinguidelegate.h"

class CSettingUIStatusNetIPv6Delegate : public CSettingUIDelegateBase
{
public:
    CSettingUIStatusNetIPv6Delegate();
    virtual ~CSettingUIStatusNetIPv6Delegate();

    static CSettingUIDelegateBase * CreateStatusNetIPv6Delegate();

    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);

private:
    void SetValue(const yl::string & strItemId, const yl::string & strValue);
};

typedef CSettingUIStatusNetIPv6Delegate * CSettingUIStatusNetIPv6DelegatePtr;

#endif // __SETTINGUI_STATUS_NETWORK_IPV6_DELEGATE_H__
