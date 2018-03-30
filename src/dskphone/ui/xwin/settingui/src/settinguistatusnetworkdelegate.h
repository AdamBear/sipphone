#ifndef _SETTINGUI_STATUS_NETWORK_DELEGATE_H_
#define _SETTINGUI_STATUS_NETWORK_DELEGATE_H_

#include "basesettinguidelegate.h"

class CSettingUIStatusNetworkDelegate : public CSettingUIDelegateBase
{
public:
    CSettingUIStatusNetworkDelegate();
    virtual ~CSettingUIStatusNetworkDelegate();

    static CSettingUIDelegateBase * CreateStatusNetworkDelegate();

    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);

private:
    void SetValue(const yl::string & strItemId, const yl::string & strValue);
protected:
    int m_nIPMode;
};

typedef CSettingUIStatusNetworkDelegate * CSettingUIStatusNetworkDelegatePtr;

#endif //_SETTINGUI_STATUS_NETWORK_DELEGATE_H_
