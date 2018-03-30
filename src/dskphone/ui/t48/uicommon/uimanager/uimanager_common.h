#ifndef __UI_MANAGER_COMMON__
#define __UI_MANAGER_COMMON__

// softkey的个数
#define SOFTKEY_NUM  4
// linekey的个数
#define LINEKEY_NUM  4
// 标题栏上图标的个数
#define ICON_NUM     5

// 窗口类型
enum DIALOG_FLAG
{
    // 该窗体是否为顶层窗体，即原来的Template
    DIALOG_TOPWND = 0x01,
    // 该窗体是否跳过Return To Idle，主要针对通话窗口
    DIALOG_REJECT_RETURN_TO_IDLE = 0x01 << 1,
    // Dsskey显示是否充满屏幕
    DIALOG_DSSKEY_FULL = 0x01 << 2,
};


enum TITLE_AREA_MODE
{
    TITLE_MODE_NONE = -1,               // 不显示任何信息
    TITLE_MODE_ACCOUNT_INFO = 0,        // 显示默认账号信息
    TITLE_MODE_CALL_MANAGER,            // 显示预拨号信息（有声道图标）
    TITLE_MODE_TALK_INFO,               // 显示通话信息（有声道图标,并且账号区域无法点击）
    TITLE_MODE_STATUS_BAR,              // 显示状态栏
    TITLE_MODE_TITLE,                   // 显示Title
    TITLE_MODE_TIME,                    // 显示时间
    TITLE_MODE_PAGE_INFO,               // 页数信息
    TITLE_MODE_RECORDING,               // 显示录音图标
    TITLE_MODE_USB_RECORDING,           // 显示USB录音
};

struct TitleLayoutInfo
{
    TitleLayoutInfo()
    {
        Reset();
    }

    ~TitleLayoutInfo() {}

    void Reset()
    {
        eLeftType = TITLE_MODE_ACCOUNT_INFO;
        eMidType  = TITLE_MODE_TITLE;
        eRightType = TITLE_MODE_TIME;
    }

    // 重载操作符
    TitleLayoutInfo & operator=(const TitleLayoutInfo & other)
    {
        // 证同测试
        if (this == &other)
        {
            return *this;
        }

        this->eLeftType = other.eLeftType;
        this->eMidType = other.eMidType;
        this->eRightType = other.eRightType;

        return *this;
    }

    bool operator==(const TitleLayoutInfo & other)
    {
        return (this->eLeftType == other.eLeftType
                && this->eMidType == other.eMidType
                && this->eRightType == other.eRightType);
    }

    bool operator!=(const TitleLayoutInfo & other)
    {
        return !(*this == other);
    }

    TITLE_AREA_MODE     eLeftType;
    TITLE_AREA_MODE     eMidType;
    TITLE_AREA_MODE     eRightType;
};

enum EWindowType
{
    eWT_Unknown     = 0,
    eWT_Idle,
    eWT_SubPage,
    eWT_Dialing,
    eWT_Talk,
    eWT_Talk_Dial,
    eWT_Search_Directory,
    eWT_Directory,
    eWT_CallLog,
    eWT_Message,
    eWT_Menu,
    eWT_SubMenu,
    eWT_XMLBrower,
    eWT_ZeroSPTouch,
    eWT_TestMode,
    eWT_FullScreen,
    eWT_Dsskey,
    eWT_CallManager,
};

// 小窗口名称（对小窗口的标识）
#define DLG_Calllist       "CDlgCalllist"
#define DLG_IdleScreen     "CDlgIdleScreen"


// setting
#define DLG_CDlgDefaultAccount  "CDlgDefaultAccount"
#define DLG_CDlgMenu            "CDlgMenu"
#define DLG_CDlgPageMenu        "CDlgPageMenu"
#define DLG_CDlgSubMenu         "CDlgSubMenu"
#define DLG_CDlgAnonymousCall   "CDlgAnonymousCall"
#define DLG_CDlgAutoRedial      "CDlgAutoRedial"
#define DLG_CDlgAlwaysForward   "CDlgAlwaysForward"
#define DLG_CDlgBusyForward     "CDlgBusyForward"
#define DLG_CDlgNOAnswerForward "CDlgNoAnswerForward"
#define DLG_CDlgAutoAnswer      "CDlgAutoAnswer"
#define DLG_CDlgCallWaiting     "CDlgCallWaiting"
#define DLG_CDlgDoNotDisturb    "CDlgDoNotDisturb"
#define DLG_CDlgIntercom        "CDlgIntercom"
#define DLG_CDlgLanguage        "CDlgLanguage"
#define DLG_CDlgAuthority       "CDlgAuthority"
#define DLG_CDlgDateTimeFormat  "CDlgDateTimeFormat"
#define DLG_CDlgDHCPTime        "CDlgDHCPTime"
#define DLG_CDlgDateTimeGeneral "CDlgDateTimeGeneral"
#define DLG_CDlgDssKey          "CDlgDssKey"
#define DLG_CDlgIPModeStatus    "CDlgIPModeStatus"
#define DLG_CDlgCustomForward   "CDlgCustomForward"
#define DLG_CDlgWanPortOption   "CDlgWanPortOption"
#define DLG_CDlgDNDCustom       "CDlgDNDCustom"
#define DLG_CDlgCustomForwardOption "CDlgCustomForwardOption"

#define DLG_CDlgNetworkVlanCDP  "CDlgNetworkVlanCDP"

#define DLG_CDlgBroadsoftCalllog    "CDlgBroadsoftCalllog"
#define DLG_CDlgBroadsoftDirectoty  "CDlgBroadsoftDirectory"
#define DLG_CDlgCallCompletion      "CDlgCallCompletion"

#define DLG_CDlgGeneral             "CDlgGeneral"
#define DLG_CDlgHotline             "CDlgHotline"

#define DLG_CDlgTestModeStatus      "CDlgTestModeStatus"
#define DLG_CDlgStatusGeneral       "CDlgStatusGeneral"
#define DLG_CDlgStatusAccounts      "CDlgStatusAccounts"
#define DLG_CDlgStatusNetwork       "CDlgStatusNetwork"
#define DLG_CDlgStatusPhone         "CDlgStatusPhone"
#define DLG_CDlgStatusRTP           "CDlgStatusRTP"

#define DLG_CDlgAccount         "CDlgAccount"
#define DLG_CDlgSetPassword     "CDlgSetPassword"
#define DLG_CDlgNetwork_VLAN        "CDlgNetwork_VLAN"
#define DLG_CDlgNetwork_VPN     "CDlgNetwork_VPN"
#define DLG_CDlgNetworkWebServer    "CDlgNetworkWebServer"
#define DLG_CDlgNetworkX_Mode       "CDlgNetworkX_Mode"
#define DLG_CDlgNetworkWANPort      "CDlgNetworkWANPort"
#define DLG_CDlgNetworkRouter       "CDlgNetworkRouter"
//#define DLG_CDlgNetworkPCPort     "CDlgNetworkPCPort"
#define DLG_CDlgNetworkDHCPServer   "CDlgNetworkDHCPServer"

#define DLG_CDlgSetAESKey       "CDlgSetAESKey"
#define DLG_CDlgAutoProvision       "CDlgAutoProvision"
#define DLG_CDlgFWDInternational    "CDlgFWDInternational"

#define DLG_CDlgIpCamera            "CDlgIpCamera"
#define DLG_CDlgDoorPhone           "CDlgDoorPhone"

#define DLG_CDlgDisplayGeneral                      "CDlgDisplayGeneral"
// #define DLG_CDlgDisplayPhoto                     "CDlgDisplayPhoto"
#define DLG_CDlgDisplayScreenSaver                  "CDlgDisplayScreenSaver"
// #define DLG_CDlgDisplayTheme                     "CDlgDisplayTheme"
#define DLG_CDlgDisplayWallpaper                    "CDlgDisplayWallpaper"
#define DLG_CDlgRingtone                            "CDlgRingtone"
#define DLG_CDlgSetAccountRingtone                  "CDlgSetAccountRingTone"
#define DLG_CDlgSoundGeneral                        "CDlgSoundGeneral"
#define DLG_CDlgVideoSetting                        "CDlgVideoSetting"

#define DLG_CDlgResetToFactory                      "CDlgResetToFactory"
#define DLG_CDlgRebootPage                          "CDlgRebootPage"
#define DLG_CDlgResetLocalConfig                       "CDlgResetLocalConfig"
#define DLG_CDlgUpdateConfigurationPage             "CDlgUpdateConfigurationPage"
// DssKey
#define DLG_CDlgDsskeyFrame                 "CDlgDsskeyFrame"

// talk
#define DLG_TalkUIFrame             "CDlgTalkUIFrame"
#define DLG_BeTransferred           "CDlgBeTransferredUI"
#define DLG_Connect                 "CDlgConnectUI"
#define DLG_Dial                    "CDlgDialingUI"
#define DLG_Finished                "CDlgFinishedUI"
#define DLG_NetworkConf             "CDlgNetworkConf"
#define DLG_Ringing                 "CDlgRingingUI"
#define DLG_Talking                 "CDlgTalkingUI"
#define DLG_Conference              "CDlgConferenceUI"
#define DLG_RingInConf              "CDlgRingInConfUI"
#define DLG_SelectPage              "CDlgSelectPageUI"
#define DLG_TalkPool                "CDlgTalkPoolUI"
#define DLG_ConfManagerUI       "CDlgConfManagerUI"
#define DLG_ParkNotifyUI            "CDlgParkNotifyUI"
#define DLG_RTPStatusUI         "CDlgRTPStatusUI"

#define DLG_DlgPickupMenu       "CDlgPickupMenu"
#define DLG_DlgSCAMenu          "CDlgSCAMenu"
#define DLG_DlgBLFMenu          "CDlgBLFMenu"

// 联系人
#define DLG_CDlgDirDetail                       "CDlgDirDetail"
#define DLG_CDlgDirectory                       "CDlgDirectory"
#define DLG_CDlgDirEdit                         "CDlgDirEdit"
#define DLG_CDlgDirNew                          "CDlgDirNew"
#define DLG_CDlgDirSearch                       "CDlgDirSearch"
#define DLG_CDlgDirSetting                      "CDlgDirSetting"

//语音留言
#define DLG_CDlgSetVoiceMail                        "CDlgSetVoiceMail"
#define DLG_CDlgViewVoiceMail                       "CDlgViewVoiceMail"

//消息留言
#define DLG_CDlgMessageEdit                         "CDlgMessageEdit"
#define DLG_CDlgMessageFrame                    "CDlgMessageFrame"
#define DLG_CDlgMessageView                 "CDlgMessageView"

//CallLogUI
#define DLG_CDlgCallLog                         "CDlgCallLog"
#define DLG_CDlgCallLogSetting                  "CDlgCallLogSetting"

//CallControl
#define DLG_CDlgCallControl                         "CDlgCallControl"
#define DLG_CDlgSimultaneousRingNew                 "CDlgSimultaneousRingNew"
#define DLG_CDlgRemoteOfficeNew                     "CDlgRemoteOfficeNew"
#define DLG_CDlgGroupNightFWD                       "CDlgGroupNightFWD"
#define DLG_CDlgMOBILITY                            "CDlgMobility"
#define DLG_CDlgSimultaneousRingNumbers             "CDlgSimultaneousRingNumbers"
#define DLG_CDlgAnyWhereNew                         "CDlgAnyWhereLocationNew"
#define DLG_CDlgAnyWhereEdit                        "CDlgAnyWhereLocationEdit"
#define DLG_CDlgAnyWhereList                        "CDlgAnyWhereLocationList"
#define DLG_CDlgSecurityClassification              "CDlgSecurityClassification"
#define DLG_CDlgSilentAlerting                      "CDlgSilentAlerting"

// 触摸屏校准
#define DLG_CDlgCalibration                         "CCalibrationUI"

// 屏保
#define DLG_CDlgScreenSavers                        "CDlgScreenSavers"
#define DLG_CDlgSetScreenSaver                      "CDlgSetScreenSaver"

// 透明度设置界面
#define DLG_CDlgSetTransparency                     "CDlgSetTransparency"

// 壁纸预览
#define DLG_CDlgPreviewPictureFullScreen        "CDlgPreviewPictureFullScreen"

// 电子相册
#define DLG_CDlgDisplayPhoto                        "CDlgDisplayPhoto"

// 主题设置
#define DLG_CDlgDisplayTheme                        "CDlgDisplayTheme"

// XMLBrowser
#define DLG_CDlgXMLBrowser                          "CDlgXMLBrowser"

//帮助向导
#define DLG_CDlgImageView                           "CDlgImageView"

// zeroSpTouch
#define DLG_CDlgZeroSPTouch                         "CDlgZeroSPTouch"

// Test Mode
#define DLG_CDlgTestMode                            "CDlgTestMode"

// DssKey
#define DLG_FullDssKey                                  "CDlgFullDsskeyUI"

// Call Center
#define DLG_CDlgACDCallInfo                     "CDlgACDCallInfo"
#define DLG_CDlgACDEscalateTo                   "CDlgACDEscalateTo"
#define DLG_CDlgACDDispositionCode              "CDlgACDDispositionCode"
#define DLG_CDlgACDInitialState                 "CDlgACDInitialState"
#define DLG_CDlgACDUnavailableReason            "CDlgACDUnavailableReason"
#define DLG_CDlgHotelingLogin                   "CDlgHotelingLogin"
#define DLG_CDlgACDStatusSelect                 "CDlgACDStatusSelect"

// 键盘锁界面
#define DLG_CDlgUnlockPIN                           "CDlgUnlockPIN"
#define DLG_CDlgKeyGuardLock                        "CDlgKeyGuardLock"
#define DLG_CDlgCheckKeyGuardLock                   "CDlgCheckKeyGuardLock"
//扩展台对比度
#define DLG_CDlgContrast    "CDlgContrast"

//蓝牙
#define DLG_CDlgBluetoothDetail         "CDlgBluetoothDetail"
#define DLG_CDlgBluetoothBase           "CDlgBluetoothBase"

#define DLG_CDlgAskPassword                 "CDlgAskPassword"

// wifi
#define DLG_CDlgWifiAddEdit             "CDlgWifiAddEdit"
#define DLG_CDlgWifiBase                "CDlgWifiBase"
#define DLG_CDlgWifiDetail              "CDlgWifiDetail"
#define DLG_CDlgWifiStatus              "CDlgWifiStatus"
#define DLG_CDlgWifiStorage             "CDlgWifiStorage"

// HotDesk base
#define DLG_CDlgHotDeskBase                 "CDlgHotDeskBase"

// autopcode_authorized
#define DLG_CCDlgAutoPCodeAuthorized        "CDlgAutoPCodeAuthorized"

#define DLG_CDlgNetwork_LLDP        "CDlgNetwork_LLDP"
#define DLG_CDlgNetwork_VLANDHCP    "CDlgNetwork_VLANDHCP"

#define DLG_CDlgNetworkNAT      "CDlgNetworkNAT"
// Presence My Status
#define DLG_CDlgMyStatus                    "CDlgMyStatus"

#define DLG_CDlgCallManager                 "CDlgCallManager"

#define DLG_CSettingWindow                  "CSettingWindow"

#define DLG_CDlgAnonymousRejection          "CDlgAnonymousRejection"
#define DLG_CDlgHideNumber                  "CDlgHideNumber"

#define DLG_CDlgMulticastPaingList          "CDlgMulticastPaingList"
#define DLG_CDlgMulticastPagingGroupEdit    "CDlgMulticastPagingGroupEdit"

#define DLG_CDlgCallControlLineSelect       "CDlgCallControlLineSelect"

#define DLG_CDlgBSFTCallWaiting                      "CDlgBSFTCallWaiting"

// Account Balance Info
#define DLG_CDlgGetBalance                           "CDlgGetBalance"

#define DLG_CDlgMultiForward                         "CDlgMultiForward"

#define DLG_CDlgCallRecord                          "DLG_CDlgCallRecord"

#define DLG_CDlgExeAssisDetails                     "CDlgExeAssisDetails"
#define DLG_CDlgExeAssisCommon                      "CDlgExeAssisCommon"
#define DLG_CDlgExeAssisFilter                      "CDlgExeAssisFilter"
#define DLG_CDlgExeAssisScreen                      "CDlgExeAssisScreen"
#define DLG_CDlgExeAssisAlter                       "CDlgExeAssisAlter"

#define DLG_CDlgExeAssisListView                    "CDlgExeAssisListView"

#define DLG_CDlgUSBBrowseRecord                     "CDlgUSBBrowseRecord"
#define DLG_CDlgUSBBrowsePhoto                      "CDlgUSBBrowsePhoto"
#define DLG_CDlgUSBPhotoDetail                      "CDlgUSBPhotoDetail"
#define DLG_CDlgUSBBrowseStorage                    "CDlgUSBBrowseStorage"
#define DLG_CDlgUSBIdleRecord                       "CDlgUSBIdleRecord"

#define DLG_CDlgVisualVoiceMailList                 "CDlgVisualVoiceMailList"

//网络检测  故障处理 ping 和 traceroute
#define DLG_CDlgPingAndTracerouteUI                 "CDlgPingAndTracerouteUI"

#define DLG_CDlgForwardSelective                    "CDlgForwardSelective"
#define DLG_CDlgAutoAnswerTimer                     "CDlgAutoAnswerTimer"

#define DLG_CDlgFWDNotReach                         "CDlgFWDNotReach"

#define DLG_CDlgMTSWAcdList                 "CDlgMTSWAcdList"
#define DLG_CDlgMTSWAcdDetail               "CDlgMTSWAcdDetail"
#define DLG_CDlgMTSWAcdMembers              "CDlgMTSWAcdMembers"
#define DLG_CDlgMTSWAcdSelectDispCode       "CDlgMTSWAcdSelectDispCode"
#define DLG_CDlgMTSWAcdChangeState          "CDlgMTSWAcdChangeState"

#define DLG_CDlgMTSWVoicemail               "CDlgMTSWVoicemail"

#define DLG_CDlgE911Location                "CDlgE911Location"
#define DLG_CDlgE911SetLocation             "CDlgE911SetLocation"

#endif // __UI_MANAGER_COMMON__
