#ifndef __SETTINGUI_SETTING_TYPE_DELEGATE_H__
#define __SETTINGUI_SETTING_TYPE_DELEGATE_H__

#include "basesettinguidelegate.h"
#include "messagebox/messageboxcallback.h"

class CSettingUISettingTypeDelegate : public CSettingUIDelegateBase, public CMsgBoxCallBackAction
{
public:
    CSettingUISettingTypeDelegate();
    virtual ~CSettingUISettingTypeDelegate();

    static CSettingUIDelegateBase * CreateSettingTypeDelegate();
    //弹框回调处理
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

public:
    virtual void OnEnterPage(const yl::string & strPage, int iDataIndex);
private:
    void PopUpPasswordCheckMsgBox();
};

typedef CSettingUISettingTypeDelegate * CSettingUISettingTypeDelegatePtr;

#endif // __SETTINGUI_SETTING_TYPE_DELEGATE_H__
