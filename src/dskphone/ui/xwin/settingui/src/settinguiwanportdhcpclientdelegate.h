#ifndef _SETTINGUI_WANPORT_DHCP_CLIENT_DELEGATE_H_
#define _SETTINGUI_WANPORT_DHCP_CLIENT_DELEGATE_H_

#include "settingui/include/settinguidefine.h"
#if !IF_SUPPORT_IP_SET_SINGLE_MENU
#include "settinguidatadelegatebase.h"

class CSettingUIWanPortDhcpClientDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettingUIWanPortDhcpClientDelegate();
    virtual ~CSettingUIWanPortDhcpClientDelegate();

    static CSettingUIDelegateBase * CreatWanPortDhcpClientDelegate();
public:
    virtual void SaveExtraData();
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual void OnComboBoxValueChange(int nIndex);
};
#endif

#endif //_SETTINGUI_WANPORT_DHCP_CLIENT_DELEGATE_H_
