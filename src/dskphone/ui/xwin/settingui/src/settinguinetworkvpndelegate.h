#ifndef _SETTINGUI_NETWORK_VPN_DELEGATE_H_
#define _SETTINGUI_NETWORK_VPN_DELEGATE_H_

#include "settingui/include/settinguidefine.h"

#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
#include "settinguinetworkchangebasedelegate.h"
#else
#include "basesettinguidelegate.h"
#endif

class CSettingUINetworkVPNDelegate
#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
    : public CSettingUINetworkChangeBaseDelegate
#else
    : public CSettingUIDelegateBase
#endif
{
public:
    CSettingUINetworkVPNDelegate();
    virtual ~CSettingUINetworkVPNDelegate();

    static CSettingUIDelegateBase * CreatNetworkVPNDelegate();

public:
    bool PageDataValid(CSettingUIPageDataBase * pPagedata, CSettingUIDataValidInfo & validInfo);
};

#endif //_SETTINGUI_NETWORK_VPN_DELEGATE_H_
