#include "settinguiwizardmanager.h"

#ifdef IF_SETTING_WIZARD

#include "keymap.h"
#include "settinguimanager.h"
#include "pageiddefine.h"
#include "uicallbackfun.h"
#include "settingmsgboxmgr.h"
#include "setting/setting_data_define.h"
#include "settinguilogic/src/networksettingmanager.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "messagebox/modmessagebox.h"
#include "uimanager/moduimanager.h"
#include "baseui/include/fullscreenmessagebox.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "languagehelper/modlangtranslate.h"
#include "translateiddefine.h"
#include "adapterbox/include/modadapterbox.h"

CSettingUIWizardManager * CSettingUIWizardManager::s_SettingUIGuideMgr = NULL;

namespace
{
#define WIZARD_PAGE_COUNT 4

#define TIME_FINISH_GUIDE 3000
#define MSGBOX_ID_COMPLETE_GUIDE 1006
#define INDEX_PAGE_SIPLINE 3

yl::string g_arrGuideInterface[WIZARD_PAGE_COUNT] =
{
    kszPageIDGuideLanguaue
    , kszPageIDGuideTimeDate
    , kszPageIDGuideNetworkWanPort
    , kszPageIDGuideCommonSipLine
};
}

CSettingUIWizardManager * CSettingUIWizardManager::GetInstance()
{
    if (NULL == s_SettingUIGuideMgr)
    {
        s_SettingUIGuideMgr = new CSettingUIWizardManager();
    }

    return s_SettingUIGuideMgr;
}

void CSettingUIWizardManager::ReleaseInstance()
{
    if (NULL != s_SettingUIGuideMgr)
    {
        delete s_SettingUIGuideMgr;
        s_SettingUIGuideMgr = NULL;
    }
}

CSettingUIWizardManager::CSettingUIWizardManager()
    : m_bInWizard(false)
{

}

CSettingUIWizardManager::~CSettingUIWizardManager()
{

}

void CSettingUIWizardManager::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL != pMessageBox && MSGBOX_ID_COMPLETE_GUIDE == pMessageBox->GetID())
    {
        QuitWizard();
    }
}

void CSettingUIWizardManager::EnterWizard()
{
    if (AdapterBox_IsTestMode())
    {
        return;
    }

    if (!IsInWizard())
    {
        m_bInWizard = true;
        PageOpenData opPageData(kszPageIDGuideLanguaue);
        g_SetUICallBackFun.EnterPage(opPageData);
    }
}

bool CSettingUIWizardManager::IsWizardDEnable()
{
    return 1 ==  configParser_GetConfigInt(kszIsEnableSetupWizard);
}

void CSettingUIWizardManager::QuitWizard()
{
    m_bInWizard = false;

    // 清空向导标志
    configParser_SetConfigInt(kszIsEnableSetupWizard, 0);
    // 通知外部设置向导结束
    etl_NotifyApp(false, SETTING_MESSAGE_WIZARD_EXIT, 0, 0);
    // 回到Idle界面
    g_SetUICallBackFun.BackToIdle();
}

void CSettingUIWizardManager::EnterNextWizardPage(const yl::string & strCurrentID)
{
    int nIndex = GetCurrentIndex(strCurrentID);
    ++nIndex;
    if (nIndex >= WIZARD_PAGE_COUNT || nIndex <= 0)
    {
        return;
    }

    yl::string strPageID = g_arrGuideInterface[nIndex];
    int nExtraData = -1;

    if (kszPageIDGuideCommonSipLine == strPageID)
    {
        nExtraData = 0;

#if IF_FEATURE_CLOUD_ACCOUNT
        if (1 == configParser_GetConfigInt(kszCloudActive))
        {
            strPageID = kszPageIDGuideCloudSipLine;
        }
#endif
    }

    PageOpenData opPageData(strPageID);
    if (-1 != nExtraData)
    {
        opPageData.pExtraData = (void *)&nExtraData;
    }

    g_SetUICallBackFun.EnterPage(opPageData);
}

void CSettingUIWizardManager::BackToPreviousInterface()
{
    g_SetUICallBackFun.FallBackToPrevousPage();
}

void CSettingUIWizardManager::FinishWizard()
{
    m_bInWizard = false;

    // 清空向导标志
    configParser_SetConfigInt(kszIsEnableSetupWizard, 0);

    //判断网络是否改变
    if (_NetworkChangeFlagManager.GetNetworkChangeFlag())
    {
        g_SettingMsgBoxMgr.ShowRebootMsgBox(0, _LANG(TRID_WIZARD_NETWORKCHANGED));
        _NetworkChangeFlagManager.ClearChangeFlag();
    }
    else
    {
        MSGBOX_INFO stMsgInfo;
        stMsgInfo.eType = (MESSAGEBOX_TYPE)MESSAGEBOX_FULLSCREEN;
        stMsgInfo.strNote = _LANG(TRID_WIZARD_CONFIGURE);
        stMsgInfo.nMsTime = TIME_FINISH_GUIDE;
        stMsgInfo.pAction = this;
        stMsgInfo.iID = MSGBOX_ID_COMPLETE_GUIDE;
        MessageBox_ShowCommonMsgBox(stMsgInfo);
    }
}

int CSettingUIWizardManager::GetCurrentIndex(const yl::string & strCurrentID)
{
#if IF_FEATURE_CLOUD_ACCOUNT
    if (kszPageIDGuideCloudSipLine == strCurrentID)
    {
        return INDEX_PAGE_SIPLINE;
    }
#endif

    for (int i = 0; i < WIZARD_PAGE_COUNT; ++i)
    {
        if (strCurrentID == g_arrGuideInterface[i])
        {
            return i;
        }
    }

    return -1;
}

bool CSettingUIWizardManager::IsInWizard()
{
    return m_bInWizard;
}

int CSettingUIWizardManager::GetTotalGuidePageCount()
{
    return WIZARD_PAGE_COUNT;
}

bool CSettingUIWizardManager::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (PHONE_KEY_HANDFREE == nKeyCode || PHONE_KEY_HANDSET_ON_HOOK == nKeyCode)
    {
        return true;
    }

    return false;
}

#endif
