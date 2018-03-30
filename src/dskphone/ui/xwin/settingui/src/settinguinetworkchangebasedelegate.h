#ifndef _SETTINGUI_NETWORK_CHANGE_BASE_DELEGATE_H_
#define _SETTINGUI_NETWORK_CHANGE_BASE_DELEGATE_H_


#include "settingui/include/settinguidefine.h"
#if IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
#include "settinguidatadelegatebase.h"

class CSettingUINetworkChangeBaseDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettingUINetworkChangeBaseDelegate();
    virtual ~CSettingUINetworkChangeBaseDelegate();

    static CSettingUIDelegateBase* CreatNetworkChangeBaseDelegate();
public:
    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void* pExtraData = NULL);
};

#endif

#endif //_SETTINGUI_NETWORK_CHANGE_BASE_DELEGATE_H_
