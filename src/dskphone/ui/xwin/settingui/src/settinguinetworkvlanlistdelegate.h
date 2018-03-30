#ifndef __SETTINGUI_NETWORK_VLAN_LIST_DELEGATE_H__
#define __SETTINGUI_NETWORK_VLAN_LIST_DELEGATE_H__

#include "basesettinguidelegate.h"

#ifdef _CP
class CSettingUINetworkVlanListDelegate : public CSettingUIDelegateBase
{
public:
    CSettingUINetworkVlanListDelegate();
    virtual ~CSettingUINetworkVlanListDelegate();

    static CSettingUIDelegateBase * CreateNetworkVlanListDelegate();

public:
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
};
#endif

#endif
