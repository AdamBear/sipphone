#ifndef __SETTINGUI_ACCOUNT_BASE_DELEGATE_H__
#define __SETTINGUI_ACCOUNT_BASE_DELEGATE_H__

#include "voicemailplaybasedelegate.h"

class CSettingUIAccountDelegateBase : public CVoiceMailPlayBaseDelegate
{
public:
    CSettingUIAccountDelegateBase();
    virtual ~CSettingUIAccountDelegateBase();

    // 获取Input Delegate对像
    static CSettingUIDelegateBase * CreateAccountDelegateBase();

public:
    // 加载配置数据
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    // 打开界面附加操作
    virtual void InitExtraData(void * pData);

    void ShowOnlyBackSoftkey(CArraySoftKeyBarData & vecSoftKey);
};


typedef CSettingUIAccountDelegateBase * CSettingUIAccountDelegateBasePtr;

#endif // __SETTINGUI_ACCOUNT_BASE_DELEGATE_H__
