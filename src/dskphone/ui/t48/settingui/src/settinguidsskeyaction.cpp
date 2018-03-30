#include "settinguidsskeyaction.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "idle/idle_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "settingui/include/modsettingui.h"
#include "setting/include/modsetting.h"
#include "setting/include/modmenu.h"
#include "setting/include/modzero.h"
#include "messageui/include/modmessageui.h"
#include "baseui/modbaseui.h"
#include "setting_page_define.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "dsskey/include/dsskeystructure.h"
#include "uicommon/uimanager/uimanager_common.h"

#if IF_FEATURE_METASWITCH
#include "uicommon/uimanager/moduimanager.h"
#include "metaswitch/mtswvoicemail/include/modmtswvoicemail.h"
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#include "uicommon/messagebox/modmessagebox.h"
#include "zerosptouchui/include/modzerosptouch.h"
#include "uicommon/uikernel/languagehelper.h"

#define CBT_MTSW_LOGOUT 1
#endif

#define Pageaction_Opensubpage_MultiPaginglist "multicast_paging_list"

bool CMenuAciton::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData)
{
    Menu_DoAction("OpenMenu(menu_list.xml)");
    return true;
}

bool CStatusAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                               void * pExtraData)
{
    SettingUI_OpenStatus();
    return true;
}

bool CDssKeyZero::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData)
{
    // 如果存在通话
    if (talklogic_SessionExist())
    {
        return false;
    }
    zero_Enter();
    return true;
}

bool CDssKeySMS::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData)
{
    // 存在session，不响应
    if (talklogic_SessionExist())
    {
        return false;
    }

    SETTINGUI_INFO("UnReadMessage %d", Setting_GetTotalUnReadMessage());
    // 是否有未读msg
    if (Setting_GetTotalUnReadMessage() > 0)
    {
        //清空弹出标志
        idleScreen_ClearPopupFlag(PS_STATE_TEXTMESSAGE);

        //用户点击View按钮情况
        //单个账号信息单个新消息提示情况
        if (1 == Setting_GetTotalUnReadMessage())
        {
            MessageUI_GotoInboxMessageDlg(true);
        }
        else
        {
            //多个账号有信息提示，转向页面InBox 目录页面
            MessageUI_GotoInboxMessageDlg(false);
        }
    }
    else
    {
        // 无，进到Textmsg
        if (Menu_EnterPageByType(MENU_SETTING_TEXTMESSAGE, 0, false))
        {
            return true;
        }
    }

    return false;
}

bool CDssKeyNewSMS::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                               void * pExtraData)
{
    MessageUI_GotoNewMessageDlg();
    return true;
}

/***********************************Multicast Paging List***************************************/
// 按键按下的处理
bool CDssKeyPagingList::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                   void * pExtraData)
{
    if (!IsTopWndPaginglist())
    {
        SubMenuData objData(Pageaction_Opensubpage_MultiPaginglist, 0);
        bool bShowPassword = false;
        objData.pBeforeSetSubpageData = (void *)&bShowPassword;
        return SettingUI_OpenPage(objData);
    }

    return true;
}

bool CDssKeyPagingList::IsTopWndPaginglist()
{
    return SettingUI_IsDlgOnTop(DLG_CDlgMulticastPaingList)
           || SettingUI_IsDlgOnTop(DLG_CDlgMulticastPagingGroupEdit);
}

#if IF_FEATURE_METASWITCH_VOICEMAIL
bool CDssKeyMessageList::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                    void * pExtraData)
{
    if (!MTSWVM_IsMessagesListEnable())
    {
        return false;
    }

    if (UIManager_IsInIdle())
    {
        SettingUI_OpenPage(SubMenuData("OpenMenu(message_list.xml)", 1), true);
    }
    return true;
}
#endif

#if IF_FEATURE_METASWITCH
bool CDssKeyMTSWLogIn::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                  void * pExtraData)
{
    if (configParser_GetConfigInt(kszMTSWLoginMode) == 1)
    {
        ZeroSPTouch_EnterAutopDirect();
    }
    return true;
}


void MsgBoxMTSWCallBackConfirmReboot(CMessageBoxBase * pMsgBox)
{
    if (pMsgBox == NULL)
    {
        return;
    }
    int iMsgID = pMsgBox->GetID();
    MESSAGEBOX_RESULT eResault = pMsgBox->GetResult();

    if (iMsgID == CBT_MTSW_LOGOUT
            && MessageBox_OK == eResault)
    {
        MTSW_Commprotal_Logout();
        // 设备重启
        Setting_Reboot();
    }
}

bool CDssKeyMTSWLogout::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                   void * pExtraData)
{
    if (!UIManager_IsInIdle())
    {
        return false;
    }
    if (MTSW_IsCommportalActive()
            && configParser_GetConfigInt(kszMTSWLogoutEnable) == 1)
    {
        CMessageBoxBase * pMsgBox = MessageBox_GetTopMessageBox();
        if (pMsgBox != NULL)
        {
            if (pMsgBox->GetID() == CBT_MTSW_LOGOUT)
            {
                return false;
            }
        }
        MessageBox_ShowCommonMsgBox(WRAPPER_MSGBOX_CBFUN(MsgBoxMTSWCallBackConfirmReboot),
                                    LANG_TRANSLATE(TRID_MTSW_LOGOUT), MESSAGEBOX_OkCancel,  -1, "", "", CBT_MTSW_LOGOUT);
    }
    return true;
}
#endif

