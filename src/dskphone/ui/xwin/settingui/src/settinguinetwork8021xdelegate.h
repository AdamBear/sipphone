#ifndef _SETTINGUI_NETWORK_8021X_DELEGATE_H_
#define _SETTINGUI_NETWORK_8021X_DELEGATE_H_

#include "settingui/include/settinguidefine.h"

#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
#include "settinguinetworkchangebasedelegate.h"
#else
#include "settinguidatadelegatebase.h"
#endif

class CSettingUINetwork8021xDelegate
#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
    : public CSettingUINetworkChangeBaseDelegate
#else
    : public CSettinguiDataDelegateBase
#endif
{
public:
    CSettingUINetwork8021xDelegate();
    virtual ~CSettingUINetwork8021xDelegate();

    static CSettingUIDelegateBase * CreatNetwork8021xDelegate();
public:
    // 界面数据过滤
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
};

#endif //_SETTINGUI_NETWORK_8021X_DELEGATE_H_
