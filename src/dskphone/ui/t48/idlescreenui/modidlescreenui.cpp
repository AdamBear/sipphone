#include "modidlescreenui.h"

#include "dlgidlescreen.h"
#include "setting/include/common.h"
#include "idlenotemanager.h"
#include "extended/xmlbrowser/include/logicanduicommon.h"
#include "idlescreen/include/modidlescreen.h"
#include "screensavers/include/modscreensavers.h"
#include "baseidlepopboxinfo.h"
#include "uimanager/moduimanager.h"
#include "talk/talklogic/include/modtalklogic.h"

void idleScreenUI_PopWinStatusChange(STATUS_ACTION eAction);

#define REGISTER_POPUPBOX_TYPE_HANDLER(type, className) \
    REGISTER_POPUPBOX_PROXY(type, className);\
    idleScreen_SetStatusChangeCallback(type, idleScreenUI_PopWinStatusChange);


// 注册模块的窗口
void idleScreenUI_RegisterDlgClass()
{
    REGISTER_DLG(CDlgIdleScreen);
    // 注册弹出框的优先级
    int iPriority = PS_STATE_MAX;
    idleScreen_RegisterStatus(PS_STATE_NOTE_HOTDESK, POPUP_WINDOW_POPUP, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_AUTO_PROVISION, POPUP_WINDOW_POPUP, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_CTR_REQUEST, POPUP_WINDOW_POPUP, iPriority--);//远程控制
    idleScreen_RegisterStatus(PS_STATE_CALL_COMPLETION, POPUP_WINDOW_POPUP, iPriority--);
#if IF_BUG_23548
    idleScreen_RegisterStatus(PS_STATE_CALL_COMPLETION_CALLBACK, POPUP_WINDOW_POPUP, iPriority--);
#endif
    idleScreen_RegisterStatus(PS_STATE_ADAPTER_ADD, POPUP_WINDOW_POPUP, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_ADAPTER_REMORE, POPUP_WINDOW_POPUP, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_WIFI_ADAPTER_ADD, POPUP_WINDOW_POPUP, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_WIFI_ADAPTER_REMOVE, POPUP_WINDOW_POPUP, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_WIFI_CONNECTOVERTIME, POPUP_WINDOW_POPUP, iPriority--);

    idleScreen_RegisterStatus(PS_STATE_AUTO_PROVISION_ERROR, POPUP_WINDOW_POPUP, 0);

    if (g_pIdleScreenUI != NULL)
    {
        g_pIdleScreenUI->UpdateTextMsgNotify();
        g_pIdleScreenUI->UpdateCallForwardNotify();
        g_pIdleScreenUI->UpdateMissCallNotify();
        g_pIdleScreenUI->UpdateVoiceMailNotify();
    }

    idleScreen_RegisterStatus(PS_STATE_NETWORK_FAILED, POPUP_WINDOW_POPUP, 0);
    //解决http://10.3.5.199/Bug.php?BugID=63336
    idleScreen_RegisterStatus(PS_STATE_NETWORK_OK_BUT_CONFLICT, POPUP_WINDOW_POPUP, 0);

#if IF_FEATURE_EMERGENCY_HELD
    idleScreen_RegisterStatus(PS_STATE_E911_ERROR, POPUP_WINDOW_POPUP, 0);
#endif

    idleScreen_RegisterStatus(PS_STATE_PICKUP_MENU, POPUP_WINDOW_POPUP, 0);
#if IF_FEATURE_XMLBROWSER_HOTELING
    idleScreen_RegisterStatus(PS_STATE_HOTELING_LOGOUT_PROMPT, POPUP_WINDOW_POPUP, 0);
#endif
    //注册没有弹出框的状态
    idleScreen_RegisterStatus(PS_STATE_DND, POPUP_WINDOW_NO_POPUP, 0);
    idleScreen_RegisterStatus(PS_STATE_AA, POPUP_WINDOW_NO_POPUP, 0);
#ifndef IF_FEATURE_FORWARD_OPT
    idleScreen_RegisterStatus(PS_STATE_FWD, POPUP_WINDOW_NO_POPUP, 0);
#endif
    idleScreen_RegisterStatus(PS_STATE_LOCK, POPUP_WINDOW_NO_POPUP, 0);
    idleScreen_RegisterStatus(PS_STATE_VPN, POPUP_WINDOW_NO_POPUP, 0);
    idleScreen_RegisterStatus(PS_STATE_BLUETOOTH, POPUP_WINDOW_NO_POPUP, 0);
    idleScreen_RegisterStatus(PS_STATE_BLUETOOTH_CONNECT, POPUP_WINDOW_NO_POPUP, 0);
    idleScreen_RegisterStatus(PS_STATE_BLUETOOTH_MOBILECONNECT, POPUP_WINDOW_NO_POPUP, 0);
    idleScreen_RegisterStatus(PS_STATE_WIFI, POPUP_WINDOW_NO_POPUP, 0);
    idleScreen_RegisterStatus(PS_STATE_WIFI_CONNECT, POPUP_WINDOW_NO_POPUP, 0);

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    idleScreen_RegisterStatus(PS_STATE_USBDEVICE, POPUP_WINDOW_NO_POPUP, 0);
    idleScreen_RegisterStatus(PS_STATE_USBDEVICE_DETECTING, POPUP_WINDOW_NO_POPUP, 0);
    idleScreen_RegisterStatus(PS_STATE_USBDEVICE_DETECTFAILED, POPUP_WINDOW_NO_POPUP, 0);
    idleScreen_RegisterStatus(PS_STATE_SAVE_RECORD_FILE, POPUP_WINDOW_POPUP, 0);
#endif

    //lcm
    idleScreen_RegisterStatus(PS_STATE_RING_MUTE, POPUP_WINDOW_NO_POPUP, 0);//音量为0
    idleScreen_RegisterStatus(PS_STATE_HEADSET, POPUP_WINDOW_NO_POPUP, 0);//使用耳麦

    idleScreen_RegisterStatus(PS_STATE_TALK_MUTE, POPUP_WINDOW_NO_POPUP, 0);

    idleScreen_RegisterStatus(PS_STATE_AUTO_PROVISION_UPDATING, POPUP_WINDOW_POPUP, 0);
    idleScreen_RegisterStatus(PS_STATE_AUTO_PROVISION_UPDATE_SUCCESS, POPUP_WINDOW_POPUP, 0);
    idleScreen_RegisterStatus(PS_STATE_AUTO_PROVISION_UPDATE_FAILED, POPUP_WINDOW_POPUP, 0);

    // 注册弹出框的类型和创建函数映射
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_CALL_COMPLETION, CCallCompletionPopboxProxy);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_AUTO_PROVISION, CAutoPPopboxProxy);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_CALLFORWARD, CCallForwardPopboxProxy);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_MISSCALL, CMissCallPopboxProxy);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_VOICEMAIL, CVoiceMailPopboxProxy);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_TEXTMESSAGE, CTextMessagePopboxProxy);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_NETWORK_FAILED, CNetworkWorkFailedPopboxProxy);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_NETWORK_OK_BUT_CONFLICT, CNetworkWorkFailedPopboxProxy);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_REMOTE_CONTACT_DOWNLOAD_FAILED,
                                   CRemoteContactDownloadFailedPopboxProxy);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_CTR_REQUEST, CCRTRequestPopboxProxy);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_NOTE_HOTDESK, CHotdesktPopboxProxy);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_ADAPTER_ADD, CAdapterAddPopboxProxy);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_ADAPTER_REMORE, CAdapterRemovePopboxProxy);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_AUTO_PROVISION_ERROR, CAutopErrorPopboxProxy);
#if IF_BUG_23548
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_CALL_COMPLETION_CALLBACK, CCompletionCallBackPopboxProxy);
#endif

#if IF_FEATURE_XMLBROWSER_HOTELING
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_HOTELING_LOGOUT_PROMPT, CHotelingLogoutPopboxProxy);
#endif
    // Wi-Fi
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_WIFI_ADAPTER_ADD, CWifiAdapterAddPopboxProxy);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_WIFI_ADAPTER_REMOVE, CWifiAdapterRemovePopboxProxy);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_WIFI_CONNECTOVERTIME, CWifiConnectOverTimePopboxProxy);

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_SAVE_RECORD_FILE, CRecordSavePopboxProxy);
#endif

    REGISTER_POPUPBOX_PROXY(PS_STATE_PICKUP_MENU, CPickupMenuPopboxProxy);

    //unlock
    idleScreen_RegisterStatus(PS_STATE_HOTELING_GUESTOUT_CHECK, POPUP_WINDOW_POPUP, iPriority--);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_HOTELING_GUESTOUT_CHECK, CHotelingUnlockPopboxProxy);

    //Guestout
    idleScreen_RegisterStatus(PS_STATE_HOTELING_GUESTOUT, POPUP_WINDOW_POPUP, iPriority--);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_HOTELING_GUESTOUT, CHotelingGuestOutPopboxProxy);

    idleScreen_RegisterStatus(PS_STATE_BLUETOOTH_MOBILE_ASK_SYNC_DIRECTORY, POPUP_WINDOW_POPUP,
                              iPriority--);
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_BLUETOOTH_MOBILE_ASK_SYNC_DIRECTORY,
                                   CBluetoothMobileAskSyncDirectory);

    // AutoP提示框，不需要退屏保和节电
    REGISTER_POPUPBOX_PROXY(PS_STATE_AUTO_PROVISION_UPDATING, CAtuoPUpdatingPopboxProxy);
    REGISTER_POPUPBOX_PROXY(PS_STATE_AUTO_PROVISION_UPDATE_SUCCESS, CAtuoPUpdateSuccessPopboxProxy);
    REGISTER_POPUPBOX_PROXY(PS_STATE_AUTO_PROVISION_UPDATE_FAILED, CAtuoPUpdateFailedPopboxProxy);

#if IF_FEATURE_EMERGENCY_HELD
    REGISTER_POPUPBOX_TYPE_HANDLER(PS_STATE_E911_ERROR, CE911CallBackPopboxProxy);
#endif
}
// 刷新XMLBrowser的数据
void idleScreenUI_UpdateXMLBrowser()
{
    YLList<CustomNotify> listNotifyData = *(idleScreen_GetCustomNotify());
    if (g_pIdleScreenUI != NULL)
    {
        g_pIdleScreenUI->SetCustomData(listNotifyData);
    }
}

// 显示dsskey Lock password
void idleScreenUI_ShowDsskeyLockPassword(int iDssKeyID)
{
    g_pIdleScreenUI->ShowDsskeyLockPassword(iDssKeyID);
}

// 状态改变退出屏保过度方案(V81 SP1)
void idleScreenUI_PopWinStatusChange(STATUS_ACTION eAction)
{
    switch (eAction)
    {
    case STATUS_ACTION_ADD:
    case STATUS_ACTION_UPDATE:
        {
            idleScreenUI_ResetScreensaver();
        }
        break;
    default:
        break;
    }
}
void idleScreenUI_ResetScreensaver()
{
    ScreenSavers_ExitScreensaver();

    if (UIManager_IsInIdle())
    {
        g_pIdleScreenUI->StartScreensaverTimer();
    }
}
