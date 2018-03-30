#ifndef _SETTINGUI_NETWORK_NAT_DELEGATE_H_
#define _SETTINGUI_NETWORK_NAT_DELEGATE_H_

#include "settingui/include/settinguidefine.h"

#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
#include "settinguinetworkchangebasedelegate.h"
#else
#include "settinguidatadelegatebase.h"
#endif

class CSettingUINetworkNATDelegate
#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
    : public CSettingUINetworkChangeBaseDelegate
#else
    : public CSettinguiDataDelegateBase
#endif
{
public:
    CSettingUINetworkNATDelegate();
    virtual ~CSettingUINetworkNATDelegate();

    static CSettingUIDelegateBase * CreatNetworkNATDelegate();

public:
    bool PageDataValid(CSettingUIPageDataBase * pPagedata, CSettingUIDataValidInfo & validInfo);
};

#endif //_SETTINGUI_NETWORK_NAT_DELEGATE_H_
