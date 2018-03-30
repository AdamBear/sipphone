#ifndef _SETTINGUI_NETWORK_VLAN_PORT_DELEGATE_H_
#define _SETTINGUI_NETWORK_VLAN_PORT_DELEGATE_H_

#include "settingui/include/settinguidefine.h"

#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
#include "settinguinetworkchangebasedelegate.h"
#else
#include "settinguidatadelegatebase.h"
#endif

class CSettinguiNetworkVlanDelegate
#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
    : public CSettingUINetworkChangeBaseDelegate
#else
    : public CSettinguiDataDelegateBase
#endif
{
public:
    CSettinguiNetworkVlanDelegate();
    virtual ~CSettinguiNetworkVlanDelegate();

    static CSettingUIDelegateBase * CreatNetworkVlanDelegate();

public:
    virtual bool PageDataValid(CSettingUIPageDataBase * pPagedata, CSettingUIDataValidInfo & validInfo);

protected:
    bool IsDHCPOptionValid(const yl::string & strOption, CSettingUIDataValidInfo & validInfo);
    bool SetInValidData(const yl::string & strOption, int nIndex, CSettingUIDataValidInfo & validInfo);
};

#endif //_SETTINGUI_NETWORK_VLAN_PORT_DELEGATE_H_
