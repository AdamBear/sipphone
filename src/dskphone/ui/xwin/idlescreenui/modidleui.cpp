#include "modidleui.h"

#include <idleuilogic/include/idledefine.h>
#include "idlescreenui/idlescreenui.h"
#include "idlescreen/include/modidlescreen.h"

#include "idleuilogic/src/accountidlehandle.h"
#include "idleuilogic/src/acdidlehandle.h"
#include "idleuilogic/src/autopidlehandle.h"
#include "idleuilogic/src/bluetoothidlehandle.h"

#include "idleuilogic/src/callcompletionidlehandle.h"
#include "idleuilogic/src/callforwardidlehandle.h"
#include "idleuilogic/src/crtrequestidlehandle.h"
#include "idleuilogic/src/hotdeskidlehandle.h"
#include "idleuilogic/src/idlehandle.h"

#include "idleuilogic/src/keypadlockidlehandle.h"
#include "idleuilogic/src/misscallidlehandle.h"
#include "idleuilogic/src/muteidlehandle.h"
#include "idleuilogic/src/networkidlehandle.h"
#include "idleuilogic/src/pickupmenuidlehandle.h"

#include "idleuilogic/src/textmessageidlehandle.h"
#include "idleuilogic/src/usbidlehandle.h"
#include "idleuilogic/src/usbidlerecordidlehandle.h"
#include "idleuilogic/src/voicemailidlehandle.h"
#include "idleuilogic/src/wifiidlehandle.h"
#include "idleuilogic/src/pstnidlehandle.h"
#include "idleuilogic/src/balanceidlehandle.h"
#include "idleuilogic/src/voiceforbiddenidlehandle.h"
#include "idleuilogic/src/dndidlehandle.h"
#include "screensavers/include/modscreensavers.h"
#if IF_FEATURE_GENBAND_E911
#include "idleuilogic/src/e911locationidlehandle.h"
#endif

#include "idleuilogic/include/modidleul.h"

void idleScreenUI_PopWinStatusChange(STATUS_ACTION eAction);

void IdleUI_Init()
{
    REGISTER_DLG(CIdleScreenUI);
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();

    // 注册icon类型状态
    idleScreen_RegisterStatus(PS_STATE_DND, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_AA, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_FWD, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_LOCK, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_MUTE, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_TALK_MUTE, SNT_ICON, 1);
    idleScreen_RegisterStatus(PS_STATE_RING_MUTE, SNT_ICON, 1);
    if (PT_CP920 != ePhoneType)
    {
        idleScreen_RegisterStatus(PS_STATE_TEXTMESSAGE, SNT_ICON);
    }
    idleScreen_RegisterStatus(PS_STATE_MISSCALL, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_VPN, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_NETWORK_FAILED, SNT_ICON);

#ifdef IF_SUPPORT_WIFI
    idleScreen_RegisterStatus(PS_STATE_WIFI, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_WIFI_CONNECT, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_WIFI_CONNECTTING, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_WIFI_CONNECT_FAIL, SNT_ICON);
#endif

    idleScreen_RegisterStatus(PS_STATE_USB_CONNECT, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_USB_CONNECTING, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_HEADSET, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_VOICEMAIL, SNT_ICON);

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    idleScreen_RegisterStatus(PS_STATE_IDLE_RECORD, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_IDLE_RECORD_PAUSE, SNT_ICON);
#endif

#ifdef IF_SUPPORT_BLUETOOTH
    idleScreen_RegisterStatus(PS_STATE_BLUETOOTH, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_BLUETOOTH_CONNECT, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_BLUETOOTH_CONNECTING, SNT_ICON);
    idleScreen_RegisterStatus(PS_STATE_BLUETOOTH_MOBILECONNECT, SNT_ICON);
#endif

#ifdef IF_FEATURE_PSTN
    idleScreen_RegisterStatus(PS_STATE_PSTN_ACCOUNT_ICON, SNT_ICON);
#endif

    int iPriority = PS_STATE_MAX - 1;
    //注册softkey类型状态

    idleScreen_RegisterStatus(PS_STATE_ACD_CHANGE, SNT_SOFTKEY, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_HOTELING_CHANGE, SNT_SOFTKEY, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_IDLE, SNT_SOFTKEY, iPriority--);

    // 注册弹出框类型和对应优先级
    iPriority = PS_STATE_MAX;
    // 提示类型优先级较高
    idleScreen_RegisterStatus(PS_STATE_AUTO_PROVISION_ERROR, SNT_POPUP, iPriority);
    idleScreen_RegisterStatus(PS_STATE_LOCK, SNT_POPUP, iPriority);
#ifdef IF_USB_SUPPORT
    idleScreen_RegisterStatus(PS_STATE_USB_CONNECT, SNT_POPUP, iPriority);
    idleScreen_RegisterStatus(PS_STATE_USB_DISCONNECT, SNT_POPUP, iPriority);
    idleScreen_RegisterStatus(PS_STATE_USBDEVICE_DETECTFAILED, SNT_POPUP, iPriority);
#warning PS_STATE_USBDEVICE_NOT_SUPPORT
    //idleScreen_RegisterStatus(PS_STATE_USBDEVICE_NOT_SUPPORT, SNT_POPUP, iPriority);
#ifdef IF_USB_RECORD
    idleScreen_RegisterStatus(PS_STATE_USB_RECORD_SUCCESS, SNT_POPUP, iPriority);
#endif
#endif

#ifdef IF_USB_SUPPORT
#warning PS_STATE_USB_CURRENT_OVERLOAD
    //idleScreen_RegisterStatus(PS_STATE_USB_CURRENT_OVERLOAD, SNT_POPUP, iPriority--);
#warning PS_STATE_IDLE_RECORD_WARNING
    //idleScreen_RegisterStatus(PS_STATE_IDLE_RECORD_WARNING, SNT_POPUP, iPriority--);
#endif

    idleScreen_RegisterStatus(PS_STATE_FLEXIBLE_UPDATE, SNT_POPUP, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_FLEXIBLE_UPDATE_END, SNT_POPUP, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_NOTE_HOTDESK, SNT_POPUP, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_AUTO_PROVISION, SNT_POPUP, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_CTR_REQUEST, SNT_POPUP, iPriority--);//远程控制
    idleScreen_RegisterStatus(PS_STATE_CALL_COMPLETION, SNT_POPUP, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_HOTELING_GUESTOUT, SNT_POPUP, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_HOTELING_GUESTOUT_CHECK, SNT_POPUP, iPriority--);

#ifdef IF_FEATURE_GETBALANCE
    idleScreen_RegisterStatus(PS_STATE_BALANCE_LOADING, SNT_POPUP, iPriority);
    idleScreen_RegisterStatus(PS_STATE_BALANCE_RESULT, SNT_POPUP, iPriority);
    idleScreen_RegisterStatus(PS_STATE_BALANCE_LOAD_ERROR, SNT_POPUP, iPriority);
#endif
    idleScreen_RegisterStatus(PS_STATE_VOICE_FORBIDDEN, SNT_POPUP, iPriority);


    // 需要注册其中一种状态，以便后续变更提示配置，能正常切换
#if IF_SUPPORT_TEXT_NOTIFY
    idleScreen_RegisterStatus(PS_STATE_NETWORK_FAILED, SNT_TEXT_NOTIFY, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_CALLFORWARD, SNT_TEXT_NOTIFY, iPriority--);
#else
    idleScreen_RegisterStatus(PS_STATE_NETWORK_FAILED, SNT_POPUP, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_CALLFORWARD, SNT_POPUP, iPriority--);
#endif

#ifdef IF_FEATURE_PSTN
    idleScreen_RegisterStatus(PS_STATE_PSTN_ACCOUNT_NOTE, SNT_POPUP, iPriority--);
#endif

#if defined(IF_SUPPORT_BLUETOOTH) && IF_SUPPORT_BLUETOOTH_ADAPTER
    idleScreen_RegisterStatus(PS_STATE_ADAPTER_ADD, SNT_POPUP, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_ADAPTER_REMORE, SNT_POPUP, iPriority--);
#endif

#ifdef IF_SUPPORT_WIFI
    idleScreen_RegisterStatus(PS_STATE_WIFI_CONNECTOVERTIME, SNT_POPUP, iPriority--);
#if IF_SUPPORT_WIFI_ADAPTER
    idleScreen_RegisterStatus(PS_STATE_WIFI_ADAPTER_ADD, SNT_POPUP, iPriority--);
    idleScreen_RegisterStatus(PS_STATE_WIFI_ADAPTER_REMOVE, SNT_POPUP, iPriority--);
#endif
#endif

    idleScreen_RegisterStatus(PS_STATE_PICKUP_MENU, SNT_POPUP, iPriority--);

#ifdef IF_FEATURE_GENBAND_E911
    idleScreen_RegisterStatus(PS_STATE_E911_LOCATION, SNT_POPUP, iPriority--);
#endif

#warning SNT_ACCOUNT
    //// 注册账号类型
    idleScreen_RegisterStatus(PS_STATE_ACCOUNT, SNT_ACCOUNT, 0);

    ////
    IdleUL_RegisterHandle(PS_STATE_AA, new CBaseIdleHandle);
    IdleUL_RegisterHandle(PS_STATE_MUTE, new CBaseIdleHandle);
    IdleUL_RegisterHandle(PS_STATE_TALK_MUTE, new CBaseIdleHandle);
    IdleUL_RegisterHandle(PS_STATE_VPN, new CBaseIdleHandle);
    IdleUL_RegisterHandle(PS_STATE_FWD, new CBaseIdleHandle);
    IdleUL_RegisterHandle(PS_STATE_HEADSET, new CBaseIdleHandle);

    IdleUL_RegisterHandle(PS_STATE_DND, new CDNDHandle);
    IdleUL_RegisterHandle(PS_STATE_RING_MUTE, new CMuteHandle);
    IdleUL_RegisterHandle(PS_STATE_CALL_COMPLETION, new CCallCompletionHandle);
    IdleUL_RegisterHandle(PS_STATE_AUTO_PROVISION, new CAutoPHandle);
    IdleUL_RegisterHandle(PS_STATE_CALLFORWARD, new CCallForwardHandle);
    IdleUL_RegisterHandle(PS_STATE_MISSCALL, new CMissCallHandle);
    IdleUL_RegisterHandle(PS_STATE_VOICEMAIL, new CVoiceMailHandle);
    if (PT_CP920 != ePhoneType)
    {
        IdleUL_RegisterHandle(PS_STATE_TEXTMESSAGE, new CTextMessageHandle);
    }

    IdleUL_RegisterHandle(PS_STATE_NETWORK_FAILED, new CNetworkHandle);
    IdleUL_RegisterHandle(PS_STATE_CTR_REQUEST, new CCRTRequestHandle);
    IdleUL_RegisterHandle(PS_STATE_NOTE_HOTDESK, new CHotdesktHandle);
    IdleUL_RegisterHandle(PS_STATE_PICKUP_MENU, new CPickupMenuHandle);
    IdleUL_RegisterHandle(PS_STATE_AUTO_PROVISION_ERROR, new CAutopErrorHandle);
    IdleUL_RegisterHandle(PS_STATE_HOTELING_GUESTOUT, new CHotelingGuestOutHandle);
    IdleUL_RegisterHandle(PS_STATE_HOTELING_GUESTOUT_CHECK, new CHotelingGuestOutCheckHandle);
    IdleUL_RegisterHandle(PS_STATE_FLEXIBLE_UPDATE, new CFlexibleSeatingHandle);
    IdleUL_RegisterHandle(PS_STATE_FLEXIBLE_UPDATE_END, new CFlexibleSeatingHandle);

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    IdleUL_RegisterHandle(PS_STATE_ACD_CHANGE, new CAcdHandle);
    IdleUL_RegisterHandle(PS_STATE_HOTELING_CHANGE, new CHotelingHandle);
#endif

    IdleUL_RegisterHandle(PS_STATE_LOCK, new CKeypadLockHandle);
    IdleUL_RegisterHandle(PS_STATE_IDLE, new CIdleHandle);

#ifdef IF_FEATURE_PSTN
    IdleUL_RegisterHandle(PS_STATE_PSTN_ACCOUNT_ICON, new CPstnHandle);
    IdleUL_RegisterHandle(PS_STATE_PSTN_ACCOUNT_NOTE, new CPstnHandle);
#endif

#ifdef IF_SUPPORT_WIFI
    //wifi
    IdleUL_RegisterHandle(PS_STATE_WIFI_CONNECTOVERTIME, new CWifiHandle);
    IdleUL_RegisterHandle(PS_STATE_WIFI, new CWifiHandle);
    IdleUL_RegisterHandle(PS_STATE_WIFI_CONNECT, new CWifiHandle);
    IdleUL_RegisterHandle(PS_STATE_WIFI_CONNECTTING, new CWifiHandle);
    IdleUL_RegisterHandle(PS_STATE_WIFI_CONNECT_FAIL, new CWifiHandle);
#if IF_SUPPORT_WIFI_ADAPTER
    IdleUL_RegisterHandle(PS_STATE_WIFI_ADAPTER_ADD, new CWifiAdapterHandle);
    IdleUL_RegisterHandle(PS_STATE_WIFI_ADAPTER_REMOVE, new CWifiAdapterHandle);
#endif
#endif

#ifdef IF_USB_SUPPORT
    IdleUL_RegisterHandle(PS_STATE_USB_CONNECT, new CUSBHandle);
    IdleUL_RegisterHandle(PS_STATE_USB_CONNECTING, new CUSBHandle);
    IdleUL_RegisterHandle(PS_STATE_USB_DISCONNECT, new CUSBHandle);
#warning PS_STATE_USBDEVICE_NOT_SUPPORT
    //IdleUL_RegisterHandle(PS_STATE_USBDEVICE_NOT_SUPPORT, new CUSBHandle);
    IdleUL_RegisterHandle(PS_STATE_USBDEVICE_DETECTFAILED, new CUSBHandle);
#warning PS_STATE_USB_CURRENT_OVERLOAD
    //IdleUL_RegisterHandle(PS_STATE_USB_CURRENT_OVERLOAD, new CUSBHandle);
#endif

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#warning PS_STATE_IDLE_RECORD_WARNING
    //IdleUL_RegisterHandle(PS_STATE_IDLE_RECORD_WARNING, new CUSBIdleRecordHandle);
    IdleUL_RegisterHandle(PS_STATE_IDLE_RECORD, new CUSBIdleRecordHandle);
    IdleUL_RegisterHandle(PS_STATE_IDLE_RECORD_PAUSE, new CUSBIdleRecordHandle);
    IdleUL_RegisterHandle(PS_STATE_USB_RECORD_SUCCESS, new CUSBIdleRecordHandle);
#endif

#ifdef IF_SUPPORT_BLUETOOTH
    IdleUL_RegisterHandle(PS_STATE_BLUETOOTH, new CBlueToothHandle);
    IdleUL_RegisterHandle(PS_STATE_BLUETOOTH_CONNECT, new CBlueToothHandle);
    IdleUL_RegisterHandle(PS_STATE_BLUETOOTH_CONNECTING, new CBlueToothHandle);
    IdleUL_RegisterHandle(PS_STATE_BLUETOOTH_MOBILECONNECT, new CBlueToothHandle);
#if IF_SUPPORT_BLUETOOTH_ADAPTER
    IdleUL_RegisterHandle(PS_STATE_ADAPTER_ADD, new CBlueToothAdapterHandle);
    IdleUL_RegisterHandle(PS_STATE_ADAPTER_REMORE, new CBlueToothAdapterHandle);
#endif
#endif

#ifdef IF_FEATURE_GETBALANCE
    IdleUL_RegisterHandle(PS_STATE_BALANCE_LOADING, new CBalanceHandle);
    IdleUL_RegisterHandle(PS_STATE_BALANCE_RESULT, new CBalanceHandle);
    IdleUL_RegisterHandle(PS_STATE_BALANCE_LOAD_ERROR, new CBalanceHandle);
#endif
    IdleUL_RegisterHandle(PS_STATE_VOICE_FORBIDDEN, new CVoiceForbiddenHandle);

#ifdef IF_FEATURE_GENBAND_E911
    IdleUL_RegisterHandle(PS_STATE_E911_LOCATION, new CE911LocationHandle);
#endif

    IdleUL_RegisterHandle(PS_STATE_ACCOUNT, new CAccountIdleHandle);


///////////////////status<-->screensaver/////////////////
    idleScreen_SetStatusChangeCallback(PS_STATE_CALL_COMPLETION, idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_AUTO_PROVISION, idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_CALLFORWARD, idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_MISSCALL, idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_VOICEMAIL, idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_TEXTMESSAGE, idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_NETWORK_FAILED, idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_NETWORK_OK_BUT_CONFLICT, idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_REMOTE_CONTACT_DOWNLOAD_FAILED,
                                       idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_CTR_REQUEST, idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_NOTE_HOTDESK, idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_ADAPTER_ADD, idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_ADAPTER_REMORE, idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_PICKUP_MENU, idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_AUTO_PROVISION_ERROR, idleScreenUI_PopWinStatusChange);

#if IF_BUG_23548
    idleScreen_SetStatusChangeCallback(PS_STATE_CALL_COMPLETION_CALLBACK, idleScreenUI_PopWinStatusChange);
#endif
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    idleScreen_SetStatusChangeCallback(PS_STATE_SAVE_RECORD_FILE, idleScreenUI_PopWinStatusChange);
#endif

    idleScreen_SetStatusChangeCallback(PS_STATE_EXP_RECOGNITION, idleScreenUI_PopWinStatusChange);

#if IF_FEATURE_XMLBROWSER_HOTELING
    idleScreen_SetStatusChangeCallback(PS_STATE_HOTELING_LOGOUT_PROMPT, idleScreenUI_PopWinStatusChange);
#endif

    // Wi-Fi
    idleScreen_SetStatusChangeCallback(PS_STATE_WIFI_ADAPTER_ADD, idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_WIFI_ADAPTER_REMOVE, idleScreenUI_PopWinStatusChange);
    idleScreen_SetStatusChangeCallback(PS_STATE_WIFI_CONNECTOVERTIME, idleScreenUI_PopWinStatusChange);
    //unlock
    idleScreen_SetStatusChangeCallback(PS_STATE_HOTELING_GUESTOUT_CHECK, idleScreenUI_PopWinStatusChange);
    //Guestout
    idleScreen_SetStatusChangeCallback(PS_STATE_HOTELING_GUESTOUT, idleScreenUI_PopWinStatusChange);

    idleScreen_SetStatusChangeCallback(PS_STATE_BLUETOOTH_MOBILE_ASK_SYNC_DIRECTORY,
                                       idleScreenUI_PopWinStatusChange);

#if IF_FEATURE_KOIP
    idleScreen_SetStatusChangeCallback(PS_STATE_QUEUE, idleScreenUI_PopWinStatusChange);
#endif

#if IF_FEATURE_EMERGENCY_HELD
    idleScreen_SetStatusChangeCallback(PS_STATE_E911_ERROR, idleScreenUI_PopWinStatusChange);
#endif

}

// 状态改变退出屏保过度方案(V81 SP1)
void idleScreenUI_PopWinStatusChange(STATUS_ACTION eAction)
{
    switch (eAction)
    {
    case STATUS_ACTION_ADD:
    case STATUS_ACTION_UPDATE:
        {
            ScreenSavers_Exit();
        }
        break;
    default:
        break;
    }
}


