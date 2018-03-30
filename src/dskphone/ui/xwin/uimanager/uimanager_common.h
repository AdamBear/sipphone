#ifndef __UI_MANAGER_COMMON_H__
#define __UI_MANAGER_COMMON_H__
#include "settingui/include/settinguidefine.h"
#include "settinguilogic/include/settingdefine.h"
// softkey的个数
#define SOFTKEY_NUM  4

enum EWindowType
{
    eWT_Unknown = 0,

    eWT_Idle,

    eWT_Dialing,
    eWT_Talk,

    eWT_Search,
    eWT_Directory,
    eWT_CallLog,

    eWT_Message,

    eWT_Menu,
    eWT_PageMenu,
    eWT_XMLBrower,
    eWT_ZeroSPTouch,
    eWT_TestMode,
    eWT_PickupMenu,

    eWT_FullScreen,

    eWT_TileDialog,
    eWT_TileSubDialog,
};

#define DLG_IdleScreen              "CIdleScreenUI"

#define DLG_CDlgDirectory           "CDlgDirectory"
#define DLG_CDlgDirSearch           "CDlgDirSearch"
#define DLG_CDlgPreciseSearch       "CDlgPreciseSearch"
#ifdef PHOTO_SHOW
#define DLG_CDlgCustomPhoto      "CDlgCustomPhoto"
#endif

#define DLG_TalkUIFrame             "CDlgTalkUIFrame"
#define DLG_TalkingUI                   "CDlgTalkingUI"

#define DLG_DialingUI                   "CDlgDialingUI"
#define DLG_RingUI                      "CDlgRingUI"

#define DLG_ConnectUI               "CDlgConnectUI"
#define DLG_FinishUI                        "CDlgFinishUI"
#define DLG_AutoRedialUI            "CDlgAutoRedialUI"

#define DLG_ConfUI                      "CDlgConfUI"
#define DLG_ConfManagerUI       "CDlgConfManagerUI"

#define DLG_NetConfUI                "CDlgNetConfUI"
#define DLG_NetConfMgrUI         "CDlgNetConfMgrUI"

#define DLG_RTPStatusUI             "CDlgRTPStatusUI"
#define DLG_ACDCallInfoUI           "CDlgACDCallInfoUI"

#define DLG_DlgPickupMenu       "CDlgPickupMenu"
#define DLG_DlgSCAMenu          "CDlgSCAMenu"

#define DLG_ParkNotifyUI                "CDlgParkNotifyUI"
#define DLG_SelectLineUI                "CDlgSelectLineUI"
#define DLG_TalkPool                    "CDlgTalkPool"

#define DLG_PINGANDTRACEOUT "CDlgPingAndTraceOut"
#define DLG_ManualSetting   "CDlgManualSetting"
#define DLG_AdvancedSetting "CDlgAdvancedSetting"
#if IF_SETTING_MAINMENU_IS_TABLEVIEW
#define DLG_MainMenu        "CDlgMainMenu"
#endif
#if IF_SUPPORT_WALLPAPER
#define DLG_Wallpaper        "CDlgWallpaper"
#endif
#if IF_SUPPORT_USB_PHOTO
#define DLG_USB_PHOTO        "CDlgUsbPhoto"
#endif

#ifdef IF_SETTING_WIZARD
#define DLG_DlgBoot "CDlgBoot"
#endif
#ifdef IF_USB_SUPPORT
#define DLG_USB_AUDIO_PLAYER            "CDlgUSBAudioPlayer"
#endif
#ifdef IF_USB_RECORD
#define DLG_USB_AUDIO_RECORD            "CDlgUSBAudioRecord"
#endif
#define DLG_NEW_MESSAGE                 "CDlgNewMessage"
#define DLG_VIEW_MESSAGE                "CDlgViewMessage"
#define DLG_VOCIE_MAIL_LIST             "CDlgVoiceMailList"
#ifdef IF_SUPPORT_BLUETOOTH
#define DLG_BT_CHANNEL_CONTROLL         "CDlgBTChannelControll"
#endif
#define DLG_CDlgZeroTouch   "CDlgZeroTouch"

#define DLG_CDlgTestMode    "CDlgTestMode"
#define DLG_KB_TRIGGER      "CDlgKeyTrigger"

// 窗口类型
#define DLG_SETTINGUI   "CDlgSettingUI"
#define DLG_CDlgXMLBrowser      "CDlgXMLBrowser"

// 窗口类型
enum UI_TYPE
{
    // 该UI显示时是否同时要显示DsskeyUI
    UI_HAVE_DSSKEYUI = 0x01,
    // 该UI显示时是否同时要显示Softkey
    UI_HAVE_SOFTKEY = 0x01 << 1,
    // 子窗口:该UI是否可层叠在另外一个UI上(比如提示框,叠在另外一个窗口上)
    UI_CHILD = 0x01 << 2,
    // 窗口始终在parent类型的窗口前
    UI_TOP_PARENT = 0x01 << 3,
};

//////////////////////////////////Idle 数据结构定义////////////////////////////////////////

// Idle的显示区域更新
enum IDLE_RANGE_TYPE
{
    IRT_NONE        = 0x00,
    IRT_STATE_ICONS = 0x01,
    IRT_NOTIFY      = 0x02,
    IRT_SOFTKEY     = 0x04,
    IRT_CUS_NOTIFY  = 0x08,
    IRT_ALL         = 0xFF,
};

//////////////////////////////////Idle 通道图标枚举////////////////////////////////////////
// T42/T41使用
enum LCD_ICON_TYPE
{
    LCD_ICON_HS,
    LCD_ICON_HF,
    LCD_ICON_HEADS,
    LCD_ICON_VPN,
    LCD_ICON_FWD,
    LCD_ICON_LOCK,
    LCD_ICON_AA,
    LCD_ICON_MUTE,
    LCD_ICON_NET1,
    LCD_ICON_DND,
    LCD_ICON_VOICE,
    LCD_ICON_MISS,
    LCD_ICON_LAST
};

#endif // __UI_MANAGER_COMMON_H__
