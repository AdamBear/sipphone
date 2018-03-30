/* ******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2012-10-23
*
* for export APIs header file , message layer
*
****************************************************************** */
#ifndef _SETTING_TYPE_DEFINE_H_
#define _SETTING_TYPE_DEFINE_H_

/************************************************************************/
/*                                                                      */
/************************************************************************/
#include <msgkit/mkmessage.h>
#include <msgkit/mkservice.h>

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/************************************************************************/
/* limit define                                                         */
/************************************************************************/


/************************************************************************/
/* common define                                                        */
/************************************************************************/
enum SETTING_TYPE
{
    ST_MESSAGE_FIRST            = 0,
    ST_FORWARD                  = 1, // FWD配置更改
    ST_VPPHONE                  = 2, // dsskey配置更改
    ST_SIPLINE                  = 3, // 账号配置更改
    ST_DIALPLAN                 = 4, // 拨号规则更改
    ST_DIALNOW                  = 6, // 立即呼出规则更改
    ST_CONTACTS                 = 8, // 联系人改变
    ST_NETWORK                  = 9, // LAN & WAN(*)
    ST_BLF                      = 10, // BLF配置更改
    ST_CALLWAIT                 = 11, // Call Waiting配置更改
    ST_TONE                     = 12, // 信号音更改
    ST_AUTOANSWER               = 13, // 自动应答配置更改
    ST_LANG                     = 16, // 液晶语言更改
    ST_VPM                      = 17, // VPM配置更改
    ST_AREA                     = 18, // 地区号码消息
    ST_BLOCKOUT                 = 19, // 限制呼出消息
    ST_LOCK                     = 20, // 键盘锁更改
    ST_LOGO                     = 21, // idle logo更改
    ST_TR069                    = 23, // TR069配置更改
    ST_SMS                      = 24, // 短消息更改
    ST_EXT                      = 25, // 扩展台配置更改
    ST_AUTOREDIAL               = 26, // 自动回拨配置更改
    ST_SETTIME                  = 27, // 时间配置更改
    ST_ADV_SETTING              = 28, // 兼容T2X中AdvSetting的消息
    ST_DND                      = 30, // DND 配置
    ST_PROGRAMABLE_KEY          = 31, // 可编程程按键配置更改
    ST_CALLCOMPLETION           = 32, // Call Completion配置发生改变.
    ST_DESKLINE                 = 33, // hotdesking账号配置更改
    ST_DESKKEY                  = 34, // hotdesking softkey更改
    ST_LDAP                     = 35, // Ldap配置更改.
    ST_FEATURE_SYNC             = 37, // Broadsoft Sync
    ST_ACTIONURL                = 38, // actionurl配置更改
    ST_ACD                      = 39, // ACD配置更改
    ST_WATCHDOG                 = 41, // 看门狗配置更改
    ST_MULTICAST                = 42, // 组播Paging功能配置改变
    ST_DSSKEY_FORWARD           = 43, //
    ST_DNS_STATIC_CACHE         = 44, // 静态dns配置更改
    ST_XPORT_CONTACT            = 46, // 从网页导出或导入本地联系人, Lparam-0导入 1导出 Extra-导出或导入的路径.
    ST_REMOTE_PHONEBOOK         = 47, // 远程地址薄 配置更新
    ST_BS_CALLLOG               = 48, // Broadsoft CallLog 配置更新
    ST_BS_PHONEBOOK             = 49, // Broadsoft PHONEBOOK 配置更新
    ST_BLACKLIST                = 50, // 黑名单开关.
    ST_CALLLOG_CLEAN            = 51, // 清空历史记录.
    ST_SUPERSEARCH              = 52, // 超级搜索配置文件发生改变.
    ST_RING                     = 53, // 铃声更新, lParam: 0-切换，1-上传/删除
    ST_XSI_CONFIGE              = 54, // XSI的配置修改
    ST_CUSTOM_COFIG             = 55, //
    ST_FAVORITE_CONFIG          = 56, // favorite的配置修改
    ST_DISTINCT_RING            = 57, // 区别振铃配置有修改
    ST_DOORPHONE                = 83, // DoorPhone
    ST_BACK_LIGHT               = 85, // 背光灯设置
    ST_WALLPAPER                = 86, // 背景图更新, lParam: 0-切换，1-上传/删除
    ST_XLOAD_CONTACT_IMAGE      = 88, // 上传联系人头像. LPARAM-0上传, 1-删除. Extra-上传或者删除的图片(全路径)
    ST_POWERLED                 = 90, // 电源灯配置更改
    ST_PAGE_TIP                 = 91, // 翻页功能page状态提示功能开关
    ST_AUTO_BLFLIST             = 92, // autoblflist相关功能开关

    // uc
    ST_PRESENCE                 = 93, // Presence配置改变
    ST_UCACTIVE                 = 94, // UC

    ST_AUTO_FAVORITE            = 95, // 自动配置favorite到dsskey
    ST_DSSKEY_LENGHT            = 96, // Dsskey的长度 0为普通 1为加长
    ST_VOICEMAIL_POPUP          = 97, // 是否弹出VoiceMail提示框
    ST_VQ_REPORT_OPTION         = 98, // RTP显示项配置改变
    ST_VQ_REPORT_ACTIVE         = 99, // RTP显示开关改变
    ST_VQ_INTERVAL_PERIOD       = 100, // RTP Interval Report发送周期改变
    ST_CSTA                     = 101, // CSTA配置更改
    ST_SYSLOG                   = 102, // syslog配置更改
    ST_HOTDESKING               = 103, // hotdesking配置更改
    ST_IME                      = 104, // 输入法改变
    ST_CALLCENTER_UNREASON      = 105, // autop CallCenter UnAvailbleReason change
    ST_SIP                      = 106, // SIP配置改变
    ST_BT_CONFIG                = 107, //蓝牙开关配置更改
    ST_AUTOSET_LINEKEY          = 108, // 账号自动设置开关

    ST_XSI_TYPE                 = 110, //和V72保持统一
    ST_EMERGENCY                = 111, // FWD/DND紧急号码更改, lparam：1-FWD 2-DND
    ST_RETRY_DELAY              = 112, // Autop的超时时间（Web/Autop使用）

    ST_CAMERA                   = 113, // 摄像头配置更改

    ST_VIDEO_BANDWIDTH          = 114, // 视频带宽配置更改
    ST_WIFI_PROFILE             = 115, // 与统一中性保持一致的Wi-Fi配置项预留
    ST_DIRECTORY_SORT_ALPHABETICALLY = 116, //非本地联系人是否按字母排序

    ST_SCREEN_SAVER_TIME    = 117,//进入屏保时间改变
    ST_SCREEN_SAVER_MODE    = 118,//使用屏保图片模式（系统、自定义）改变
    ST_SCREEN_SAVER_PIC = 119,//屏保自定义图片改变
    ST_SCREEN_SAVER_BG_CHANGE_TIME  = 120,//屏保更换背景时间改变
    ST_SCREEN_SAVER_CLOCK_MOVE_TIME = 121,//屏保时钟移动时间改变

    ST_HDMI_CONFIG = 122, //HDMI 配置变更

    ST_SCREENSAVER_SHOW_CLOCK   = 123,//屏保是否显示时钟配置改变

    ST_CHANNEL_CONFIG = 124,      //通道配置变化

    ST_CONFIG_LEVEL   = 125,      //Autop 不覆盖相关配置

    ST_IDLE_CLOCK = 126, // Idle Clock是否显示改变

    // h323账号 todo
    ST_H323LINE = 127, // H323

    ST_CLOUD_CONFIG = 128, //Cloud 配置

    // 默认账号改变
    ST_DEFAULT_ACCOUNT = 129,

    ST_EDK_SWITCH               = 130, // EDK 功能开关:    0/1  功能总开关/提示模式外部开关
    ST_EDK_CONFIG               = 131, // edk配置文件改变
    ST_USER_MODE                = 132, // User Mode
    ST_CALLRECORD_MODE          = 133, // Call record Mode
    ST_MUTE_MODE                = 134, // Mute Mode 0:keep mute; 1:all mute
    ST_BLF_CUSTOM_DATA          = 135, // BLF Custom Data
    ST_POWERSAVING              = 136, //节电模式配置更改
    ST_PRESENCE_CHANGE          = 137, //是否允许更改presence状态
    ST_TRANSPARENTCY            = 138, //主题透明度
    ST_PSTN_LINE                = 139, // PSTN账号配置更改
    ST_COUNTRY                  = 140, // 国家设置
    ST_DSSKEY                   = 141, // Dsskey类型改变
    ST_DSSKEY_SWITCH            = 142, // Dsskey相关的开关改变
    ST_SHORT_LINEKEY_LABEL      = 143, // Linekey文字缩小显示
    ST_EXP_WALLPAPER            = 144, // exp壁纸更新

    ST_EXP_PAGE_BLF_NOTIFY      = 145, //lparam:1 switch change;    lparam:2 rule change
    ST_EXP_POWER_SAVING         = 146,
    ST_FLASH_DURATION           = 147, // PSTN拍叉簧时间配置改变
    ST_KEYBOARD                 = 148, // keybord配置变更
    ST_DSSKEY_LENGTH_SHORTEN    = 149,     // Dssskey缩略显示
    ST_FORCEVOICE_SWITCH        = 150,
    ST_MTSW_CONFIG              = 152, //Commprotal配置更改
    ST_MTSW_COMMPORTAL          = 153,// Commportal开关，
    ST_MTSW_KEEPALIVE           = 154,// KeepAlive配置，
    ST_MTSW_LOG_CONFIG          = 155,// 历史记录配置改变，
    ST_MTSW_ACD_CONFIG          = 156,// ACD配置改变，
    ST_MTSW_DIR_CONFIG          = 157,// 联系人配置改变，
    ST_SWITCH_CHANGE            = 158,
    ST_MAX_DOWNLOAD_SIZE        = 159, // 下载的文件最大值
    ST_FEATURE_SOFTKEY          = 160,   //idle界面功能softkey显示
    ST_CALLCENTER_CONFIG        = 161, //callcenter配置变更 wparam：1. ACD queue status配置改变,lparam：account ID
    ST_WIFI_AP_PROFILE          = 162, // 无线热点配置变更
    ST_USER_INFO_CHANGED        = 163, // 话机用户信息变更 lParam：0/1 --> 用户名或用户密码变更/usermode切换
    ST_WARNING_DISPLARY_MODE    = 164, // 警告显示模式切换
    ST_BLF_LED_CONFIG           = 165,  // 网页通知blf led 模式变更
    ST_BLF_LIST_PARK_LIST_CONFIG = 166, //blflist一键park配置变更
    ST_PICKUP_BLF_VISUAL_LIST_CONFIG = 167, //features.pickup.blf_visual.list配置变更

    /*0-199中性项目*/


    // lync
    ST_LYNC_CONFIG              = 200, // lync相关配置更改
    ST_USER_AGENT               = 201, // 隐藏页面修改user agent
    ST_BTOE_AS_AUDIO_DEV        = 202, /* 话机是否仅作为音频设备 */
    ST_LOCATION                 = 203, /*位置信息更新*/
    ST_LYNC_ACCOUNT             = 204, /*autop 配置账号*/
    ST_SWITCH_PAGE              = 205,
    ST_CALENDAR_SWITCH          = 206, // 日历开关
    ST_MUTE_STRATEGY_MODE       = 207, // custom.features.mute_strategy.mode  2:永久mute
    ST_LYNC_EXP_CONFIG          = 208,  //Lync Exp设置更改
    ST_DIAG_REPORT              = 209,  //诊断上报开关
    ST_BTOE_SWITCH              = 210,  // BTOE开关
    ST_BTOE_PAIR_MODE_SWITCH    = 211, /*BTOE配对模式切换*/
    ST_LYNC_LOGIN_TYPE_CHANGE   = 212, // 登录方式开关切换
    ST_LYNC_INDICATOR_LED       = 213,  //lync的指示灯配置变更
    ST_REG_AUTH_METHOD_SWITCH   = 214, // 鉴权方式切换
    ST_AUTO_DISCOVER_SWITCH     = 215,  // auto discover开关
    ST_SEARCH_FILTER            = 216,  // 搜索过滤号码配置变更
    ST_CALENDAR_REMINDER_SWITCH = 217,  // 日历会议弹窗开关
    ST_HOT_DESKING_SWITCH       = 218,  // Hot-Desking的开关配置
    ST_MOH_MODE                 = 219,  // Moh模式切换
    ST_MOH_RING_UPLOAD          = 220,  // Moh铃声上传
    ST_MOH_RING_CHANGE          = 221,  // Autop配置Moh
    ST_AUTODIAL                 = 222,  // 自动呼出开关、时间
    ST_MOH_RING_DELETE          = 223,  // Autop删除Moh
    ST_BA_POWER_LED             = 224,  // BossAdmin电源灯设置
    ST_SET_AS_CAP_MODE          = 225,  // 设置为cap模式
    ST_EXCHANGE_CONNECT_SWITCH  = 226,  // exchange连接开关
    ST_CAP_PRESENCE_SHOW        = 227,  // cap账号presence状态是否显示
    ST_VOICEMAIL_LIMIT          = 228,  // vm功能限制
    ST_INTERCOM                 = 229,
    ST_WEB_SIGN_IN              = 230,  // Web Sign in配置变更
    ST_RTP_PORT_UPDATE          = 231, /* RTP端口范围更新 */ // todo hwz 原来的值为205有冲突，外部也需要统一
    ST_CODEC_CONFIG             = 232, // CODEC配置变化     // todo hwz 原来的值为207有冲突，外部也需要统一


    //dect
    ST_COUNTRY_TONE             = 300,
    ST_FEATURES                 = 301,  // 多个配置更新的消息，同ST_PHONE_SETTING 过渡之用
    ST_CALLPARK_CFG             = 302,  // callpark配置改变
    ST_FIRMWARE_URL_CHANGED     = 303,  //手柄固件URL更改

    ST_BASE_PIN_CODE_CHANGE     = 304,  //pincode改变
    ST_REGISTER_PIN_CODE_CHANGE = 305,  //注册PIN码改变的消息
    ST_PLCMDIR_CONFIG           = 306,  //PLCM phonebook service 0:URL配置变更；1：更新时间变更，目前暂未使用
    ST_HS_NUMBER_ASSIGNMENT     = 307,  //手柄账号配置改变
    ST_HS_CONFIG_CHANGE         = 308,  //手柄配置改变
    ST_HS_NAME_CHANGE           = 309,  //手柄名称改变
    ST_HS_REMOVE_REGIST_INFO    = 310,  //wParam:HS ID，注销手柄
    ST_XPORT_BLACKLIST          = 311,  //autop上传联系人黑名单
    ST_LINE_LABEL_TYPE          = 312,  //wParam:1:Label,2:DisplayName, 3:User Name
    ST_FEATURES2                = 313,  // 多个配置更新的消息，类似ST_FEATURES

    //技术支持新加消息从这里开始
    ST_LOCAL_CONTACT_SYNC       = 501,
    ST_DIGIT_MAP                = 502,
    ST_STATION_NAME             = 503,
    ST_LINEKEY_TIMER            = 504,
    ST_INACTIVE_TIME            = 505,
    ST_GCS_UPDATE               = 506, // GCS相关key配置更新
    ST_GB_BOOK                  = 507,
    ST_BLF_LIST_URI             = 508,
    ST_ERROR_INFO               = 509,
    ST_XSI_EVENT_CONFIG         = 510, //xsi-event配置更新
    ST_DND_SYNC                 = 511, //DND功能同步
    ST_AUTOSET_LOCAL_FAVORITE   = 512, // 账号自动设置开关
    ST_XML_SCREEN_SAVER_URL     = 513, //xmlScreensaver URL配置更新
    ST_XMLBROWSER_HOTELING      = 514, //xml browser hoteling
    ST_LOCAL_CALLLOG_SYNC       = 515,
    ST_BS_DIR_MODE              = 516,
    ST_CALLLOG_IMPORT           = 517,
    ST_FONALITY_LOGO            = 518,
    ST_POWER_SAVING_LIGHT_TIME  = 519,
    ST_POWER_SAVING_DARK_TIME   = 520,
    ST_LOOPBACK_CONFIG          = 521,
//#warning V82
    ST_IDLE_TEXT_COLOR          = 522, //ilde字体颜色 // 和测试部申请的是517，后续要修改

    ST_VIRXSI_LINE              = 523, // 虚拟XSI账号更改
    ST_VIRXSI_CONFIG            = 524, // 虚拟XSI的配置修改
    ST_VIRXSI_DND_CONFIG        = 525, // 虚拟XSI账号DND配置修改
    ST_LOCALCALLLOG_REPLACERULE = 526, // 来电记录替换规则配置修改
    ST_UPDATE_IDLE_SOFTKEY      = 528, // 刷新lide界面softkey

    //额外的调试段
    ST_DEBUG = 10000,
    ST_DEBUG_TIMER = 10001,
    ST_DEBUG_TASKACTION = 10002,
    ST_MESSAGE_LAST,
};

#ifdef __cplusplus
}
#endif

#endif /* _SETTING_TYPE_DEFINE_H_ */

