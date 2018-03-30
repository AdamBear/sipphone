#ifndef __PATH_DEFINE_H__
#define __PATH_DEFINE_H__

#ifdef WIN32
#define ROOT_PATH   "./"
#else
#define ROOT_PATH   "/"
#endif

//启动分区路径
#define BOOT_PATH               ROOT_PATH "boot/"

//临时文件存放路径
#define TEMP_PATH               ROOT_PATH "tmp/"

#define SYNC_LOG_PATH           TEMP_PATH "sync_data/"

#define SHARE_MEMORY_FILE_NAME  TEMP_PATH "sharememory"
#define ACTIVE_FLAGE_FILE_NAME  TEMP_PATH "activeflag"
#define LED_STATUS_FILE_NAME    TEMP_PATH "ledstatus"

//根配置目录路径定义
#define CONFIG_PATH             ROOT_PATH "config/"

//主运行分区路径
#define PHONE_PATH              ROOT_PATH "phone/"

//系统资源路径定义
#define RESOURCE_PATH           PHONE_PATH "resource/"

// #ifdef _T4X
//字体文件存放路径
#define FONTS_FILE_PATH         RESOURCE_PATH "system/fonts/"

//语言文件存放路径
#define LANGUAGE_FILE_PATH      RESOURCE_PATH "system/language/"
// 固定铃声文件夹
#define RING_FILE_PATH          RESOURCE_PATH "system/sound/"
// BSFT特制铃声文件夹
#define BSFT_RING_FILE_PATH     RESOURCE_PATH "system/ring/"
// menuconfig
#define PHONE_RES_MENUCONFIG_PATH   RESOURCE_PATH "menuconfig/"
// edkconfig
#define PHONE_RES_EDKCONFIG_PATH    RESOURCE_PATH "edkconfig/"
//
//
// #else
// //字体文件存放路径
// #define FONTS_FILE_PATH          RESOURCE_PATH "Fonts/"
//
// //语言文件存放路径
// #define LANGUAGE_FILE_PATH       RESOURCE_PATH "Lang/"
//
// // 固定铃声文件夹
// #define RING_FILE_PATH       RESOURCE_PATH "Waves/"
//
// // BSFT特制铃声文件夹
// #define BSFT_RING_FILE_PATH      RESOURCE_PATH "Rings/"
// #endif

// media file的配置路径.
#define MEDIA_CONFIG_FILE_PATH  RESOURCE_PATH "mediafile/"

// 系统资源文件所在目录.
#define SYSTEM_RESOURCE_PATH    RESOURCE_PATH "system/sys-res/"

// 系统图片文件所在目录
#define SYSTEM_IMAGE_PATH       RESOURCE_PATH "system/theme/images/"

// 系统铃声文件所在目录
#define SYSTEM_SOUND_PATH       RESOURCE_PATH "system/sound/"

#define RESOURCE_DEVICE_PATH    RESOURCE_PATH   "device-res/"

//按键文件目录
#define SYSTEM_KEYMAP_PATH      RESOURCE_PATH "system/keymap/"

//界面布局文件
#define PHONE_LAYOUT_PATH       RESOURCE_PATH "system/layout/"

//屏幕信息文件
#define PHONE_SCREEN_PATH       RESOURCE_PATH "system/screen/"

// #ifdef _T4X
//主题路径定义
#define PHONE_THEME_PATH        RESOURCE_PATH "system/"



//默认主题路径!!!
#define THEME_DEFAULT_PATH      PHONE_THEME_PATH "theme/"


//主题图片存放路径
#define THEME_IMAGE_PATH        THEME_DEFAULT_PATH "images/"

//Menu图片存放路径
#define MENU_ICON_PATH          PHONE_THEME_PATH "images/"

// #else
// #define THEME_IMAGE_PATH     RESOURCE_PATH   "/Pics"
// #endif


//出厂配置文件路径
#define FACTORY_PATH            PHONE_PATH "factory/"

#define FACTORY_SYSTEM_INI_FILE_PATH    FACTORY_PATH "system/system.ini"
#define FACTORY_USER_INI_FILE_PATH      FACTORY_PATH "user/user.ini"
#define FACTORY_DATA_PATH               FACTORY_PATH "data/"

//phone脚本路径
#define PHONE_SCRIPTS_PATH      PHONE_PATH "scripts/"

//配置分区路径定义
#define CONFIG_PATH             ROOT_PATH "config/"

//系统配置目录路径定义
#define CONFIG_SYSTEM_PATH          CONFIG_PATH "system/"
#define CONFIG_SYSTEM_INI_FILE_PATH CONFIG_SYSTEM_PATH "system.ini"

//用户配置目录路径定义
#define CONFIG_USER_PATH            CONFIG_PATH "user/"
#define CONFIG_USER_INI_FILE_PATH   CONFIG_USER_PATH "user.ini"
//voip配置路径定义
#define CONFIG_VOIP_PATH        CONFIG_USER_PATH "voip/"
//SIP帐号配置文件名
#define SIP_ACCOUNT_FILE_NAME   "sipAccount"
//SIP帐号配置文件路径
#define SIP_ACCOUNT_FILE_PATH   CONFIG_VOIP_PATH SIP_ACCOUNT_FILE_NAME
//PSTN帐号配置文件名
#define PSTN_ACCOUNT_FILE_NAME  "pstnAccount.cfg"
//PSTN帐号配置文件路径
#define PSTN_ACCOUNT_FILE_PATH  CONFIG_VOIP_PATH PSTN_ACCOUNT_FILE_NAME

//menu列表数据目录路径定义
#define PHONE_MENU_PATH                     "menu/"
#define PHONE_MENUCONFIG_PATH        "menuconfig/"

//用户数据目录路径定义
#define CONFIG_DATA_PATH        CONFIG_PATH "data/"

//用户资源分区路径定义
#define DATA_PATH               ROOT_PATH "data/"

/*#ifdef _T4X*/
//用户资源目录定义
#define USER_DATA_PATH          DATA_PATH "userdata/"

//用户自定义上传铃声目录定义
#define USER_RING_FILE_PATH     USER_DATA_PATH "sound/"

// 自定义翻译文件的存放路径
#define USER_LANG_FILE_PATH     USER_DATA_PATH "language/"
// #else
// //用户资源目录定义
// #define USER_DATA_PATH           CONFIG_PATH
//
// //用户自定义上传铃声目录定义
// #define USER_RING_FILE_PATH      USER_DATA_PATH "ringtone/"
//
// // 自定义翻译文件的存放路径
// #define USER_LANG_FILE_PATH      USER_DATA_PATH "UserLang/"
// #endif

#define szUserLangXmlFile       USER_LANG_FILE_PATH "Lang.xml"
#define SYSTELM_LOGO_FILE       "yealink.dob"
//#define szInitPics                THEME_IMAGE_PATH "init.dob"
#define szResEmergencyPath      RESOURCE_PATH "Emergency/"
//用户自定义Logo路径定义
#define szPicLogoFile           USER_DATA_PATH "theme/images/logo.dob"
// 自定义SoftKey文件的存放路径
#define szSoftKeyFile           CONFIG_PATH "softkey"
// 自定义SoftKey默认文件的存放路径
#define szSoftKeyDefaultFile    FACTORY_PATH "softkey"

#define szDHCPAdd               TEMP_PATH "yealink_dhcp_options"
#define szInitFile              TEMP_PATH "InitSave"
#define szXmlBrowser            TEMP_PATH "XmlBrowserFile.xml"
#define szPlayWaveFile          TEMP_PATH "PlayWaveFile.wav"
#define szXmlURLRecord          TEMP_PATH "XmlURLRecordFile.xml"
#define szVPNStatusFile         TEMP_PATH "openvpn_status"
#define szAutopStatusFile       TEMP_PATH "UpdateStatus"
#define szXmlBalance            TEMP_PATH "XmlBalanceFile.xml"
#define szAccountNum            FACTORY_PATH "voip/AccountNum.cfg"
#define szFactorySystemIni      FACTORY_PATH "system/system.ini"
#define szUserSpecific          FACTORY_PATH "user/user.ini"
#define szFactoryAutoDST        FACTORY_PATH "data/AutoDST.xml"
#define SETTING_PATH            CONFIG_DATA_PATH
#define CALLLOG_PATH            CONFIG_DATA_PATH
#define CONTACT_PATH            CONFIG_DATA_PATH
#define szCalllistFile          CONFIG_DATA_PATH "call_data.xml"
#define szContactsFile          CONFIG_DATA_PATH "contact_list.xml"
#define szContactGroupFile      CONFIG_DATA_PATH "contact_group_list.xml"

// dect 联系人文件名
#define szDectContactsFile      CONFIG_DATA_PATH "contact_handset%d_list.xml"
// dect 组文件名
#define szDectContactGroupFile  CONFIG_DATA_PATH "contact_handset%d_group_list.xml"

#define szForwardFile           CONFIG_USER_INI_FILE_PATH
#define szPhoneFile             CONFIG_USER_INI_FILE_PATH
#define szPasswordFile          CONFIG_DATA_PATH "htpasswd"
#define szXmlMessageFile        CONFIG_DATA_PATH "Message.xml"
#define szDialNowFile           CONFIG_DATA_PATH "DialNow.xml"
#define szDialPlanFile          CONFIG_DATA_PATH "DialPlan.xml"
#define szBlockOutFile          CONFIG_USER_INI_FILE_PATH
#define szAdvSettingFile        CONFIG_USER_INI_FILE_PATH
#define szAutoDSTFile           CONFIG_DATA_PATH "AutoDST.xml"
#define szSuperSearchXML        CONFIG_DATA_PATH "super_search.xml"
#define szSettingFile           CONFIG_USER_INI_FILE_PATH
#define szDsskeyFile            CONFIG_USER_PATH "vpPhone.ini"
#define szIMEFile               FACTORY_PATH "ime/ime.txt"
#define szCustomIMEFile         CONFIG_PATH "ime/custom_ime.txt"
#define szDeviceCertFile        CONFIG_PATH "certs/server/WebServer.pem"
// http://10.2.1.199/Bug.php?BugID=42001
#define szDeviceCertCustomFile  CONFIG_PATH "certs/phonedev/WebServer.pem"
#define szDeviceCertDefaultFile CONFIG_PATH "certs/phonedev/WebServer.pem"
#define szTrustCertPath         CONFIG_PATH "certs/phoneca/"
#define szLevelFile             ((PT_T49 == devicelib_GetPhoneType() ? RESOURCE_PATH "LevelTranslate.xml" : CONFIG_USER_PATH "WebItemsLevel.cfg"))
#define szVPNFile               CONFIG_PATH "openvpn/vpn.cnf"
// 组播Paging通话专属配置文件
#define szMulticastFile         CONFIG_USER_PATH "MulticastPage.cfg"
#define szSessionReportFile     TEMP_PATH "VQSessionReport.xml"
#define szRTPConfigureFile      CONFIG_DATA_PATH "rtp.xml"

// 黑白屏用户自定义logo路径
#define szCustomLogoPath        CONFIG_PATH "Pics/"
#define szCustomLogoFile        szCustomLogoPath "logo.dob"

//定义Dsskey配置文件保存路径，用于上传给服务器
#define szUpLoadDssKeyFile          CONFIG_DATA_PATH "DssKey.cfg"

// AutopCode的配置文件
#define szAutopCode             CONFIG_SYSTEM_PATH "autop_code.cfg"


//Dial
#define DIALPLAN_PATH           CONFIG_DATA_PATH "DialPlan.xml"
#define DIALNOW_PATH            CONFIG_DATA_PATH "DialNow.xml"
#define DIALBLOCK_PATH          CONFIG_DATA_PATH "DialBlock.xml"

#define szXmlFacIMEFile         FACTORY_PATH "ime/ime.xml"
#define szXmlCfgIMEFile         CONFIG_PATH "ime/custom_ime.xml"

// t49 sp2
#define szCloudAccountRequestFile           CONFIG_DATA_PATH "account-login-list.xml"
#define szCloudAccountHistoryFile           CONFIG_DATA_PATH "account-history-list.xml"
#define szCloudAccountDefaultFile           CONFIG_DATA_PATH "account-default-list.xml"

///////////////////////////////////技术支持/////////////////////////////////////////
#define szDsskeyHideInfoFile            CONFIG_PATH "data/dsskey_hide.xml"
#define CUSTOM_FACTORY_PATH   RESOURCE_PATH "/customfactory.xml"
#define RESOURCE_PATHHIDER_FILE RESOURCE_PATH "resPathHiderMapper.xml"
#define szDsskeyFactoryHideInfoFile         FACTORY_DATA_PATH "dsskey_hide.xml"

#endif //__PATH_DEFINE_H__
