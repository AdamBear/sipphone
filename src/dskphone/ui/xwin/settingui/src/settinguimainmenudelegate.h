#ifndef __SETTINGUI_MAIN_MENU_DELEGATE_H__
#define __SETTINGUI_MAIN_MENU_DELEGATE_H__

#include "basesettinguidelegate.h"
#include "messagebox/messageboxcallback.h"

class CSettingUIMainMenuDelegate : public CSettingUIDelegateBase, public CMsgBoxCallBackAction
{
public:
    CSettingUIMainMenuDelegate();
    virtual ~CSettingUIMainMenuDelegate();

    static CSettingUIDelegateBase * CreateMainMenuDelegate();

public:
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual bool IsResetFocusWhileReload()
    {
        return true;
    }
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey);
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
public:
    virtual void OnEnterPage(const yl::string & strPage, int iDataIndex);
    void PopUpPasswordCheckMsgBox();
};

#endif
