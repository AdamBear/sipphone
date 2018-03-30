#ifndef _SETTINGUI_WANPORT_DELEGATE_H_
#define _SETTINGUI_WANPORT_DELEGATE_H_

#include "settingui/include/settinguidefine.h"

#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
#include "settinguinetworkchangebasedelegate.h"
#else
#include "settinguidatadelegatebase.h"
#endif

class CSettingUIWanPortDelegate
#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
    : public CSettingUINetworkChangeBaseDelegate
#else
    : public CSettinguiDataDelegateBase
#endif
{
public:
    CSettingUIWanPortDelegate();
    virtual ~CSettingUIWanPortDelegate();

    static CSettingUIDelegateBase* CreateWanPortDelegate();
    virtual void OnComboBoxValueChange(int nIndex);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata,
                                bool bReload /*= false*/);
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad /*= false*/);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData,
                              bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/);
protected:
    int m_nIPModePreference;

};

#endif //_SETTINGUI_WANPORT_DELEGATE_H_
