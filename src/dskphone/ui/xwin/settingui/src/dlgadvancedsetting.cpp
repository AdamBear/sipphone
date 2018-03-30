#include "dlgadvancedsetting.h"
#include "settinguilogic/src/networksettingmanager.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "modsettingui.h"
#include "languagehelper/modlangtranslate.h"
#include "translateiddefine.h"
#include "settingui/include/settinguidefine.h"

namespace
{
#define MSG_BOX_ID_UPDATING_NETWORK 1002
}

CBaseDialog * CDlgAdvancedSetting::CreateInstance()
{
    CDlgAdvancedSetting * pDlg = new CDlgAdvancedSetting;

    if (NULL != pDlg)
    {
        pDlg->loadContent("setting/dlgsettingui.xml");
    }

    return pDlg;
}

CDlgAdvancedSetting::CDlgAdvancedSetting()
{

}

CDlgAdvancedSetting::~CDlgAdvancedSetting()
{

}

void CDlgAdvancedSetting::Init()
{
    SLOT_CONNECT(this, signalAppear, this, CDlgAdvancedSetting::OnAppearSlot);
}

void CDlgAdvancedSetting::Uninit()
{
    SLOT_CONNECT(this, signalAppear, this, CDlgAdvancedSetting::OnAppearSlot);

    CheckNetwork();
}

void CDlgAdvancedSetting::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    if (MSG_BOX_ID_UPDATING_NETWORK == pMessageBox->GetID())
    {
        SettingUI_Reboot(0);
    }
    else
    {
        CDlgSettingUI::MessageBoxCallBack(pMessageBox);
    }
}

void CDlgAdvancedSetting::OnAppearSlot(xSignal * sender, WPARAM wParam, LPARAM lParam,
                                       int nDataBytes, LPVOID pData)
{
#if !IF_NETWORK_CHANGE_REBOOT_SAVE_PROMPT
    if (1 == wParam)
    {
        CheckNetwork();
    }
#endif
}

void CDlgAdvancedSetting::CheckNetwork()
{
    if (_NetworkChangeFlagManager.GetNetworkChangeFlag())
    {
        MSGBOX_INFO stMsgInfo;

        stMsgInfo.eType = MESSAGEBOX_NOTE;
        stMsgInfo.strTitle = _LANG(TRID_NOTE);
        stMsgInfo.strNote = _LANG(TRID_UPDATING_NETWORK_PLEASE_WAIT);
        stMsgInfo.nMsTime = SHORT_TIME;
        stMsgInfo.pAction = this;
        stMsgInfo.iID = MSG_BOX_ID_UPDATING_NETWORK;

        MessageBox_ShowCommonMsgBox(stMsgInfo);

        _NetworkChangeFlagManager.ClearChangeFlag();
    }
}

