#ifndef _SETTINGUI_WANPORT_IP_SET_DELEGATE_H_
#define _SETTINGUI_WANPORT_IP_SET_DELEGATE_H_


#include "settingui/include/settinguidefine.h"
#if IF_SUPPORT_IP_SET_SINGLE_MENU

#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
#include "settinguinetworkchangebasedelegate.h"
#else
#include "settinguidatadelegatebase.h"
#endif

class CSettingUIWanPortIPSetDelegate
#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
    : public CSettingUINetworkChangeBaseDelegate
#else
    : public CSettinguiDataDelegateBase
#endif
{
public:
    CSettingUIWanPortIPSetDelegate();
    virtual ~CSettingUIWanPortIPSetDelegate();

    static CSettingUIDelegateBase* CreatWanPortIPSetDelegate();
public:
    virtual bool PageDataFilter(CSettingUIPageDataBase* pPagedata, bool bReload = false);
    virtual void OnComboBoxValueChange(int nIndex);
private:
    void HideIpV4StaticIPItem(CSettingUIPageDataBase* pPagedata);
    void HideIpV6StaticIPItem(CSettingUIPageDataBase* pPagedata);
    void HideStaticDnsItem(CSettingUIPageDataBase* pPagedata);
};

#endif

#endif //_SETTINGUI_WANPORT_IP_SET_DELEGATE_H_
