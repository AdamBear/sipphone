#ifndef __SETTINGUI_GUIDE_MANAGER_H__
#define __SETTINGUI_GUIDE_MANAGER_H__

#include "features-define.h"

#ifdef IF_SETTING_WIZARD

#include "ylstl/ylstring.h"
#include "messagebox/messageboxcallback.h"

class CSettingUIWizardManager : public CMsgBoxCallBackAction
{
private:
    CSettingUIWizardManager();
    virtual ~CSettingUIWizardManager();

public:
    static CSettingUIWizardManager * GetInstance();
    static void ReleaseInstance();
public:
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    void EnterWizard();
    void QuitWizard();
    void EnterNextWizardPage(const yl::string & strCurrentID);
    void BackToPreviousInterface();
    void FinishWizard();
    int GetCurrentIndex(const yl::string & strCurrentID);
    bool IsInWizard();
    bool IsWizardDEnable();
    int GetTotalGuidePageCount();
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);
private:
    static CSettingUIWizardManager   *  s_SettingUIGuideMgr;
    bool m_bInWizard;
};

#define g_pSettingUIWizardManager (CSettingUIWizardManager::GetInstance())

#endif // IF_SETTING_WIZARD

#endif // __SETTINGUI_GUIDE_MANAGER_H__

