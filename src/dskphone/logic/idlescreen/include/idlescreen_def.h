#ifndef __IDLE_SCREEN_DEF_H__
#define __IDLE_SCREEN_DEF_H__

//TODO. 后续T48机型要统一使用一套接口
#if !IS_COLOR
#define IF_NOTIFICATION_CENTER_ON_IDLE
#endif

////////////////////////////Message//////////////////////////////////
#include "innermsgdefine.h"
#include "idle/idle_data_define.h"

enum IDLE_MSG_DEFINE
{
    IDLE_MSG_BEGIN = INNER_MSG_IDLE_MESSAGE_BEGIN,

    // Idle窗口重新获取焦点
    IDLE_FOCUS_CHANGED, // wParam:0/失去焦点 1/获取焦点 lParam:0  extra: null

    // Idle界面更新
    IDLE_UPDATE_UI,     // wParam:Ranges                lParam:0  extra: null

    // todo
    IDLE_MSG_NUMBER_EMPTY,

    //退出屏保到idle
    IDLE_MSG_EXIT_SCREENSAVER,

    IDLE_MSG_END = INNER_MSG_IDLE_MESSAGE_END,
};

////////////////////////////Action//////////////////////////////////
enum IDLE_ACTION_TYPE
{
    IDLE_ACTION_FUNC_KEY = 0x1001,  // idle界面的按键逻辑处理; wParam: KeyType; lParam: 0; ExtraData: NULL
    IDLE_ACTION_SOFT_KEY = 0x1002,  // idle界面的Softkey逻辑处理; wParam: Softkey; lParam: 0; ExtraData: NULL
};

////////////////////////////PopupBoxType//////////////////////////////////
enum IDLE_POPUPBOX_TYPE
{
    IPT_NONE = 0x00,
    IPT_CALL_COMPLETION,
    IPT_AUTOP_CODE,
    IPT_PICKUP_MENU,
    IPT_RESET_TO_FACTORY,
    IPT_REBOOT_CONFIRM,
    IPT_CTR_REQUEST,    // 请求ActionURI和呼出界面
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    IPT_USB_RECORD_SAVE,  // 通话结束后,自动结束录音的提示
#endif
    IPT_GET_BALANCE,    // 提示正在下载余额信息
    IPT_CALL_COMPLETION_CALLBACK,   //NFON CallBack
    IPT_HOTELING_START,
    IPT_DOWNLOAD_CONFIG,
    IPT_HOTELING_GUESTOUT,
    IPT_PSTN_ACCOUNT,
    IPT_USB_INSEART,
    IPT_BLUETOOOTH_ADAPTER_ADD,
    IPT_BLUETOOOTH_ADAPTER_REMOVE,
    IPT_WIFI_ADAPTER_ADD,
    IPT_WIFI_ADAPTER_REMOVE,
    IPT_WIFI_CONNECT_OVER_TIME,
    IPT_AUTOPRIVISION,
#if IF_FEATURE_XMLBROWSER_HOTELING
    IPT_XMLBROWSER_HOTELING_LOGOUT,
#endif
    IPT_USBHEADSET_PLUG_IN,
    IPT_USBHEADSET_PLUG_OUT,
    IPT_AUTOPRIVISION_IDLE,
    IPT_E911_ERROR,
    IPT_MTSW_LOGOUT,
    IPT_MAX,
};

// 弹框出错类型
enum IDLE_POPUPBOX_ERROR
{
    IPE_OK          =  0,
    IPE_FAILED      = -1,
    IPE_NOT_ALLOW   = -2,
};
//////////////////////////////////////////////////////////////////////////
#include <ylstring.h>
#include <yllist.h>
// 自定提示
struct CustomNotify
{
    CustomNotify()
    {
        iAlign = 0;
        iSize = 0;
        iColor = -1;
        strIcon.clear();
        strText.clear();
        strAccount.clear();
    }
    int iAlign; // 对齐方式
    int iSize;  // 大小 X_FONT_TYPE
    int iColor; // 颜色 TEXT_FONT_COLOR
    yl::string strIcon; // 提示的图标
    yl::string strText; // 提示的文本
    yl::string strAccount;  // 账号
};
typedef YLList<CustomNotify> typeCusNotifyList;

// 话机状态数据结构.
struct IdleStatusItem
{
    int m_nId;  // 状态Id.
    int m_nPopupType;  // 弹窗状态, 为PopupWindowType枚举.
    yl::string m_strHint;  // 提示语句.
    VecNotifyTypePriority m_vecTypePriority; //类型-优先级

    IdleStatusItem()
    {
        Reset();
    }

    void Reset()
    {
        m_nId = 0;
        m_nPopupType = 0;
        m_strHint.clear();
        m_vecTypePriority.clear();
    }
};

//状态执行动作
enum STATUS_ACTION
{
    STATUS_ACTION_DELETE = 0,
    STATUS_ACTION_ADD = 1,
    STATUS_ACTION_UPDATE = 2,
};

#if IF_FEATURE_WALLPAPER_MODE
enum WALLPAPAER_MODE
{
    WALLPAPAER_MODE_DEFAULT = 0,
    WALLPAPAER_MODE_CENTER = 1,
    WALLPAPAER_MODE_TILE = 2,
    WALLPAPAER_MODE_FILL = 3,
    WALLPAPAER_MODE_FIT = 4,
};
#endif

#endif

