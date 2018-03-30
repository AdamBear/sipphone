#ifndef _SETTINGUI_WIFI_NET_DELEGATE_H_
#define _SETTINGUI_WIFI_NET_DELEGATE_H_

#ifdef IF_SUPPORT_WIFI
#include "settinguibtwifidelegate.h"

class CSettingUIWifiNetDelegate : public CSettingUIBtWifiDelegate
{
public:
    CSettingUIWifiNetDelegate();
    virtual ~CSettingUIWifiNetDelegate();

    static CSettingUIDelegateBase * CreateWifiNetDelegate();

public:
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
#if IS_COLOR
    yl::string SetSignalStrengthIcon(const yl::string& strSignalStrength, const yl::string& strConnectState, const yl::string& strLock);
#endif

protected:
    virtual CSettingUIItemDataBase * CreateDefaultItemData(const ViewInfo & viewInfo);
    virtual bool InitListItemData(CSettingUIItemDataBase * pItemData, const ViewInfo & viewInfo);
};

#endif
#endif //_SETTINGUI_WIFI_NET_DELEGATE_H_
