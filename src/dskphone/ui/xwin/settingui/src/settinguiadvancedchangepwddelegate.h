#ifndef __SETTINGUI_ADVANCED_CHANGE_PWD_DELEGATE_H__
#define __SETTINGUI_ADVANCED_CHANGE_PWD_DELEGATE_H__

#include "settinguidatadelegatebase.h"

class CSettingUIAdvancedChangePwdDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettingUIAdvancedChangePwdDelegate();
    virtual ~CSettingUIAdvancedChangePwdDelegate();

    static CSettingUIDelegateBase * CreateChangePwdDelegate();
public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool OnAction(int eActionType, int iDataIndex);

    virtual bool ProcessMsgCallBack(int nResult, int nID, void * pExtraData = NULL);

private:
    void PageSetPwdSave();
    void SavePwd();
    void SetValidData(const yl::string & strNote, int nIndex);
    void ShowOKCancleMsgBox(const yl::string & strNote);
};

typedef CSettingUIAdvancedChangePwdDelegate * CSettingUIAdvancedChangePwdDelegatePtr;

#endif // __SETTINGUI_ADVANCED_CHANGE_PWD_DELEGATE_H__
