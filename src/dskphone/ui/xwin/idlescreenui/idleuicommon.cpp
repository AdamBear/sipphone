#include "idleuicommon.h"
#include "baseui/include/t2xpicpath.h"
#include "xwindow/xWindow.h"
#include "settingui/include/modsettingui.h"
#include "settingui/include/pageiddefine.h"
#include "directoryui/include/moddirectoryui.h"
#include "settinguilogic/include/common.h"
#include "talkui/include/modtalkui.h"
#include <idleuilogic/include/idledefine.h>

int GetAlignByDTAlign(int nDTAlign)
{
    int iIdleAlign = 0;
    if (0 == nDTAlign)
    {
        iIdleAlign = align_left | align_v_center;
    }
    else
    {
        if (nDTAlign & DT_LEFT)
        {
            iIdleAlign = iIdleAlign | align_left;
        }
        else if (nDTAlign & DT_CENTER)
        {
            iIdleAlign = iIdleAlign | align_h_center;
        }
        else if (nDTAlign & DT_RIGHT)
        {
            iIdleAlign = iIdleAlign | align_right;
        }

        if (nDTAlign & DT_TOP)
        {
            iIdleAlign = iIdleAlign | align_top;
        }
        else if (nDTAlign & DT_VCENTER)
        {
            iIdleAlign = iIdleAlign | align_v_center;
        }
        else if (nDTAlign & DT_BOTTOM)
        {
            iIdleAlign = iIdleAlign | align_bottom;
        }
    }

    return iIdleAlign;
}

bool IconType2PicPath(ICON_TYPE eIconType, yl::string& strIcon)
{
    bool bRet = true;
    switch (eIconType)
    {
    // account
    case II_ACCOUNT_LS_DISABLE:
        {
            strIcon = PIC_ACCOUNT_DISABLE;
        }
        break;
    case II_ACCOUNT_LS_LOGING:
        {
            strIcon = PIC_ACCOUNT_LOGING;
        }
        break;
    case II_ACCOUNT_LS_FORWARD:
        {
            strIcon = PIC_ACCOUNT_FORWARD;
        }
        break;
    case II_ACCOUNT_LS_DND:
        {
            strIcon = PIC_ACCOUNT_DND;
        }
        break;
    case II_ACCOUNT_LS_ACD_LOGOUT:
        {
            strIcon = PIC_ACCOUNT_ACD_LOGOUT;
        }
        break;
    case II_ACCOUNT_LS_ACD_LOGIN:
        {
            strIcon = PIC_ACCOUNT_ACD_LOGIN;
        }
        break;
    case II_ACCOUNT_LS_ACD_UNAVAILABLE:
        {
            strIcon = PIC_ACCOUNT_ACD_UNAVAILABLE;
        }
        break;
    case II_ACCOUNT_LS_ACD_WRAPUP:
        {
            strIcon = PIC_ACCOUNT_ACD_WRAPUP;
        }
        break;
    case II_ACCOUNT_LS_HEADSET:
        {
            strIcon = PIC_ACCOUNT_HEADSET;
        }
        break;
    case II_ACCOUNT_LS_SCA_REGISTED:
        {
            strIcon = PIC_ACCOUNT_SCA_REGISTED;
        }
        break;
    case II_ACCOUNT_LS_REGISTED:
        {
            strIcon = PIC_ACCOUNT_REGISTED;
        }
        break;
#warning II_ACCOUNT_LS_CLOUD_REGISTED
    //case II_ACCOUNT_LS_CLOUD_REGISTED:
    //    {
    //        strIcon = PIC_ACCOUNT_CLOUD_REGISTED;
    //    }
    //    break;
    case II_ACCOUNT_LS_REGISTEDFAIL:
        {
            strIcon = PIC_ACCOUNT_REGISTEDFAIL;
        }
        break;
    case II_ACCOUNT_LS_UNREGISTER:
        {
            strIcon = PIC_ACCOUNT_UNREGISTER;
        }
        break;
    // iconlist
    case IT_STATE_DND:
        {
            strIcon = PIC_IDLE_DND;
        }
        break;
    case IT_STATE_AA:
        {
            strIcon = PIC_IDLE_AA;
        }
        break;
    case IT_STATE_FWD:
        {
            strIcon = PIC_IDLE_FWD;
        }
        break;
    case IT_STATE_LOCK:
        {
            strIcon = PIC_IDLE_LOCK;
        }
        break;
    case IT_STATE_RING_MUTE:
        {
            strIcon = PIC_IDLE_RING_MUTE;
        }
        break;
    case IT_STATE_VOICEMAIL:
        {
            strIcon = PIC_IDLE_VOICEMAIL;
        }
        break;
    case IT_STATE_TEXTMESSAGE:
        {
            strIcon = PIC_IDLE_TEXTMESSAGE;
        }
        break;
    case IT_STATE_MISSCALL:
        {
            strIcon = PIC_IDLE_MISSCALL;
        }
        break;
    case IT_STATE_VPN:
        {
            strIcon = PIC_IDLE_VPN;
        }
        break;
    case IT_STATE_INTERNET:
        {
            strIcon = PIC_IDLE_INTERNET;
        }
        break;
    case IT_STATE_TALK_MUTE:
        {
            strIcon = PIC_IDLE_TALK_MUTE;
        }
        break;
    case IT_USB_DEVICE:
        {
            strIcon = PIC_IDLE_USB_DEVICE;
        }
        break;
    case IT_USB_DEVICE_DETECTING:
        {
            strIcon = PIC_IDLE_USB_DEVICE_DETECTING;
        }
        break;
#ifdef IF_USB_RECORD
    case IT_USB_IDLE_RECORD:
        {
            strIcon = PIC_IDLE_USB_RECORD;
        }
        break;
    case IT_USB_IDLE_RECORD_PAUSE:
        {
            strIcon = PIC_IDLE_USB_RECORD_PAUSE;
        }
        break;
#endif
#ifdef IF_FEATURE_PSTN
    case IT_STATE_PSTN:
        {
            strIcon = PIC_IDLE_PSTN;
        }
        break;
#endif
#ifdef IF_SUPPORT_BLUETOOTH
    case IT_STATE_BLUETOOTH:
        {
            strIcon = PIC_IDLE_BLUETOOTH;
        }
        break;
    case IT_STATE_BLUETOOTH_CONNECTED:
        {
            strIcon = PIC_IDLE_BLUETOOTH_CONNECTED;
        }
        break;
    case IT_STATE_BLUETOOTH_PHONE_CONNECTED:
        {
            strIcon = PIC_IDLE_BLUETOOTH_MOBILE_CONNECTED;
        }
        break;
#endif
#ifdef IF_SUPPORT_WIFI
    case IT_STATE_WIFI:
        {
            strIcon = PIC_IDLE_WIFI;
        }
        break;
    case IT_STATE_WIFI_CONNECTED:
        {
            strIcon = PIC_IDLE_WIFI_CONNECTED;
        }
        break;
#endif
#if IF_SUPPORT_TEXT_NOTIFY
#warning IT_STATE_CABLE_OFF
        //case IT_STATE_CABLE_OFF:
        //    {
        //        strIcon = PIC_IDLE_CABLE_OFF;
        //    }
        //    break;
#warning IT_STATE_CABLE_ON
        //case IT_STATE_CABLE_ON:
        //    {
        //        strIcon = PIC_IDLE_CABLE_ON;
        //    }
        //    break;
#endif
    case IT_UNKNOWN:
        {
            strIcon = "";
        }
        break;
    default:
        {
            strIcon = "";
            bRet = false;
        }
        break;
    }
    return bRet;
}

void EnterPage(IDLE_ENTER_PAGE_TYPE_E eEnterPageType)
{
    yl::string strPageName = "";
    void* pExtraData = NULL;
    bool bExtra = false;
    int nExtra = 0;
    switch (eEnterPageType)
    {
    case IEPT_BROADSOFT_CALLLOG_PLACEED:
        {
            DM_EnterDirectoryUI(CALL_LOG_TYPE_BROADSOFT, kszPlacedLogId);
        }
        break;
    case IEPT_BROADSOFT_CALLLOG_MISSED:
        {
            DM_EnterDirectoryUI(CALL_LOG_TYPE_BROADSOFT, kszPlacedLogId);
        }
        break;
    case IEPT_LOCAL_CALLLOG_PLACEED:
        {
            DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, kszPlacedLogId);
        }
        break;
    case IEPT_LOCAL_CALLLOG_CALL_FORWARD:
        {
            DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, kszForWardedLogId);
        }
        break;
    case IEPT_LOCAL_CALLLOG_MISSED:
        {
            DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, kszMissLogId);
        }
        break;
    case IEPT_PICKUP_MENU_EXIT:
        {
            talkui_ExitPickupMenu();
        }
        break;
    case IEPT_PICKUP_MENU_UPDATE:
        {
            talkui_UpdatePickupMenu();
        }
        break;
    case IEPT_UNLOCK_CHECK:
        {
            SettingUI_EnterUnlockCheck();
        }
        break;
    case IEPT_HOT_DESK:
        {
            strPageName = kszPageIDHotDeskBase;
        }
        break;
    case IEPT_HOT_DESK_BASE_DATA:
        {
            strPageName = kszPageIDHotDeskBase;
            nExtra = HDT_BASE_WITH_DATA;
            pExtraData = (void*)(&nExtra);
        }
        break;
    case IEPT_MAIN_MENU:
        {
            strPageName = kszPageIDMainMenu;
        }
        break;
    case IEPT_HOTELING_LOGIN:
        {
            strPageName = kszPageIDHotelingLogin;
        }
        break;
    case IEPT_HOTELING_LOGIN_ACD_OFF:
        {
            strPageName = kszPageIDHotelingLogin;
            bExtra = true;
            pExtraData = (void*)(&bExtra);
        }
        break;
    case IEPT_ACD_UNAVAILABLE_REASON_CODE:
        {
            strPageName = kszPageIDACDUnavailableReasonCode;
        }
        break;
    case IEPT_ACD_DISPCODE:
        {
            strPageName = kszPageIDACDDispositionCode;
        }
        break;
    case IEPT_GUEST_UNLOCK_CHECK:
        {
            strPageName = kszPageIDGuestUnlockCheck;
        }
        break;
    case IEPT_BALANCE:
        {
            strPageName = kszPageIDBalance;
        }
        break;
#ifdef IF_FEATURE_PSTN
    case IEPT_PSTN_ACCOUNT_LIST:
        {
            strPageName = kszPageIDPSTNAccountList;
        }
        break;
#endif
    case IEPT_TEXTMESSAGE_VIEW_INMSG:
        {
            strPageName = kszPageIDTextMsgView;
            nExtra = TEXT_MSG_GROUP_IN;
            pExtraData = (void*)(&nExtra);
        }
        break;
    case IEPT_TEXTMESSAGE_INBOX:
        {
            strPageName = kszPageIDTextMsgInbox;
        }
        break;
#ifdef IF_SUPPORT_VIDEO
    case IEPT_USB_VIEW_CAMERA:
        {
            strPageName = kszPageIDUSBViewCamera;
        }
        break;
#endif
#ifdef IF_SUPPORT_BLUETOOTH
    case IEPT_BLUE_TOOTH_MENU:
        {
            strPageName = kszPageIDBluetoothMenu;
        }
        break;
#endif
    case IEPT_VIEW_VOICE_MAIL:
        {
            strPageName = kszPageIDViewVoiceMail;
        }
        break;
#ifdef IF_SUPPORT_WIFI
    case IEPT_WIFI_MENU:
        {
            strPageName = kszPageIDWifiMenu;
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_VOICEMAIL
    case IEPT_MTSW_VOICE_MAIL_LOGIN:
        {
            strPageName = kszPageMTSWCommportal;
        }
        break;
    case IEPT_MTSW_VOICE_MAIL_LIST:
        {
            strPageName = kszPageMTSWVoiceMailList;
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_ACD
    case IEPT_MTSW_ACD_MYSTATUS:
        {
            // 进入状态切换界面
            strPageName = kszPageMTSWAcdMyStatus;
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    case IEPT_MTSW_CALLLOG_PLACEED:
        {
            //  TBD-liuqy 进入metaswitch的呼出历史记录
            // DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, kszForWardedLogId);
        }
        break;
#endif
#if IF_FEATURE_GENBAND_E911
    case IEPT_E911_SET_LOCATION:
        {
            strPageName = kszPageIDSetE911Location;
        }
        break;
#endif
#if IF_SUPPORT_BLUETOOTH
    case IEPT_BLUE_TOOTH:
        {
            strPageName = kszPageIDBluetoothMenu;
        }
        break;
#endif
#if IF_SUPPORT_WIFI
    case IEPT_WIFI:
        {
            strPageName = kszPageIDWifiMenu;
        }
        break;
#endif
    default:
        break;
    }

    if (!strPageName.empty())
    {
        SettingUI_EnterPage(strPageName, pExtraData);
    }
}
