#include "settinguidsskeyaction.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "idlescreen/include/modidlescreen.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"


#if IF_FEATURE_METASWITCH
#include "adapterbox/include/modadapterbox.h"
#include "metaswitch/mtswvoicemail/include/modmtswvoicemail.h"
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#include "messagebox/modmessagebox.h"
#include "zerotouchui/include/modzerotouchui.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#endif
#if IF_FEATURE_EDK
#include "edk/include/modedk.h"
#endif

bool CMenuAciton::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData)
{
    SettingUI_EnterPage(kszPageIDMainMenu);
    return true;
}

bool CStatusAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                               void* pExtraData)
{
    SettingUI_EnterPage(kszPageIDStatus);
    return true;
}

bool CDssKeyZero::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData)
{
    // 如果存在通话
    if (talklogic_SessionExist())
    {
        return false;
    }
    SettingUILogic_EnterZero();
    return true;
}

bool CDssKeyXMLBrowser::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                   void* pExtraData)
{
    xmlbrowser_Enter(logicData.strValue.c_str());
    return true;
}

#if IF_FEATURE_PAGING
/***********************************Multicast Paging List***************************************/
// 按键按下的处理
bool CDssKeyPagingList::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                   void* pExtraData)
{
    if (!IsTopWndPaginglist())
    {
        return SettingUI_EnterPage(kszPageIDPagingList);
    }

    return true;
}

bool CDssKeyPagingList::IsTopWndPaginglist()
{
    return SettingUI_IsTopPage(kszPageIDPagingList)
           || SettingUI_IsTopPage(kszPageIDPagingListEdit);
}
#endif

bool CDssKeySMS::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData)
{
    // 存在session，不响应
    if (talklogic_SessionExist())
    {
        return false;
    }

    SETTINGUI_INFO("UnReadMessage %d", SettingUILogic_GetTotalUnReadMessage());
    // 是否有未读msg
    if (SettingUILogic_GetTotalUnReadMessage() > 0)
    {
        //清空弹出标志
        idleScreen_ClearNotifyTypeFlag(PS_STATE_TEXTMESSAGE, SNT_POPUP | SNT_TEXT_NOTIFY);
        //单个账号信息单个新消息提示情况
        //打开inbox detail message
        int nUnReadMsg = SettingUILogic_GetTotalUnReadMessage();
        if (1 == nUnReadMsg)
        {
            int nGType = TEXT_MSG_GROUP_IN;
            SettingUI_EnterPage(kszPageIDTextMsgView, (void*)(&nGType));
        }
        else
        {
            SettingUI_EnterPage(kszPageIDTextMsgInbox);
        }
    }
    else
    {
        // 无，进到Textmsg
        SettingUI_EnterPage(kszPageIDTextMsg);
    }

    return true;
}

bool CDssKeyNewSMS::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                               void* pExtraData)
{
    SettingUI_EnterPage(kszPageIDTextMsgNew);
    return true;
}

#if FEATURE_BROADTOUCH_UC
bool CDssKeyMyStatus::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                 void* pExtraData)
{
    if (1 == configParser_GetConfigInt(kszEnableChangePresenceStatus))
    {
        SettingUI_EnterPage(kszPageIDMyStatus);
    }
    return true;
}
#endif

#if IF_FEATURE_EDK
//////////////////////EDK/////////////////////////
// 按键按下的处理
bool CDssKeyEDK::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData)
{
    if (Edk_IsEnable())
    {
        EDK_DoAction(logicData.strValue);
    }
    return true;
}
#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL
bool CDssKeyMessageList::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                    void* pExtraData)
{
    if (!MTSWVM_IsMessagesListEnable())
    {
        return false;
    }

    if (AdapterBox_IsInIdle())
    {
        SettingUI_EnterPage(kszPageMTSWVoiceMailList);
    }
    return true;
}
#endif

#if IF_FEATURE_METASWITCH
void CDssKeyMTSWLogout::MessageBoxCallBack(CMessageBoxBase* pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }
    int iMsgID = pMessageBox->GetID();
    MESSAGEBOX_RESULT eResault = pMessageBox->GetResult();

    if (MSGBOX_ID_MTSW_LOGOUT == iMsgID
            && MessageBox_OK == eResault)
    {
        MTSW_Commprotal_Logout();
        AdapterBox_Reboot(0);
    }
}

bool CDssKeyMTSWLogout::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                   void* pExtraData)
{
    if (!AdapterBox_IsInIdle())
    {
        return false;
    }

    if (MTSW_IsCommportalActive()
            && 1 == configParser_GetConfigInt(kszMTSWLogoutEnable))
    {
        CMessageBoxBase* pMsgBox = MessageBox_GetTopMessageBox();
        if (NULL != pMsgBox && MSGBOX_ID_MTSW_LOGOUT == pMsgBox->GetID())
        {
            return false;
        }

        MSGBOX_INFO stMsgInfo;
        stMsgInfo.pAction = this;
        stMsgInfo.strTitle = _LANG(TRID_NOTE);
        stMsgInfo.strNote = _LANG(TRID_MTSW_LOGOUT);
        stMsgInfo.eType = MESSAGEBOX_OkCancel;
        stMsgInfo.nMsTime = -1;
        stMsgInfo.iID = MSGBOX_ID_MTSW_LOGOUT;
        MessageBox_ShowCommonMsgBox(stMsgInfo);
    }
    return true;
}

bool CDssKeyMTSWLogIn::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                  void* pExtraData)
{
    if (1 == configParser_GetConfigInt(kszMTSWLoginMode))
    {
        ZeroTouchUI_EnterAutopDirect();
    }
    return true;
}
#endif
