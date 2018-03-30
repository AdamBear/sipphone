#ifndef _VIEWMODEUI_DEFINE_H_
#define _VIEWMODEUI_DEFINE_H_

#include <ylstring.h>
#include <yllist.h>
#include <ylvector.h>
//#include <devicelib/phonedevice.h>

// UI 类型定义
enum VIEW_MODE_UI_TYPE
{
    VIEW_MODE_UI_TYPE_BEGIN = 0,
    // 蓝牙界面
    WIRELESS_UI_TYPE_BT_MENU = 1,
    WIRELESS_UI_TYPE_BT_SCAN = 2,
    WIRELESS_UI_TYPE_BT_PAIRED = 3,
    WIRELESS_UI_TYPE_BT_DEV_DETAIL = 4,
    WIRELESS_UI_TYPE_BT_DONGLE_INFO = 5,
    WIRELESS_UI_TYPE_BT_PIN_INPUT = 6,

    // WIFI界面
    WIRELESS_UI_TYPE_WIFI_MENU = 10,
    WIRELESS_UI_TYPE_WIFI_SCAN = 11,
    WIRELESS_UI_TYPE_WIFI_PROFILE = 12,
    WIRELESS_UI_TYPE_WIFI_NET_STATUS = 13,
    WIRELESS_UI_TYPE_WIFI_NET_DETAIL = 14,
    WIRELESS_UI_TYPE_WIFI_NET_EDIT = 15,
    WIRELESS_UI_TYPE_WIFI_NET_ADD = 16,
    WIRELESS_UI_TYPE_WIFI_PIN_INPUT = 17,
    WIRELESS_UI_TYPE_WIFI_NET_DETAIL_2_ADD = 18,
    WIRELESS_UI_TYPE_WIFI_AUTH_NETWORK = 19,

    VIEW_MODE_UI_TYPE_END
};

#define DATA_ID_INVALID     (-1)
#define DATA_ID_NONE        (-10086)


enum WifiItemDataID
{
    WIFI_ITEM_DATA_ID_MENU_SWITCH = VIEW_MODE_UI_TYPE_END + 1,
    // add/edit 页面item data id
    WIFI_ITEM_DATA_ID_EDIT_SECTURE,
    WIFI_ITEM_DATA_ID_EDIT_PROFILE_NAME,
    WIFI_ITEM_DATA_ID_EDIT_SSID,
    WIFI_ITEM_DATA_ID_EDIT_CIPHER,
    WIFI_ITEM_DATA_ID_EDIT_PSK,
    WIFI_ITEM_DATA_ID_EDIT_EAP_NAME,
};

// UI显示信息定义
struct ViewInfo
{
    int                     iDataID;                    // 数据标识
    yl::string              strDisplayName;             // 显示名称
    yl::string              strValue;                   // 显示值
    yl::string              strValue1;                  // 显示值
    bool
    bUseValue;                  // 标记使用value还是value1 ---- 黑白屏使用, 彩屏无用
    YLVector<yl::string>
    strVecExtraValue;           // 如: combox 显示值、icon名称(仅限第一个字段)等
    YLVector<yl::string>
    strVecCustomValue;          // 如: combox 显示值对应的值, icon label除第一个图标外的其他图标, 可自定义用法
    bool                    bShow;

    ViewInfo()
    {
        Reset();
    }

    void Reset()
    {
        iDataID = DATA_ID_INVALID;
        strDisplayName = "";
        strValue = "";
        strValue1 = "";
        bUseValue = true;
        strVecExtraValue.clear();
        strVecCustomValue.clear();
        bShow = true;
    }
};
typedef YLVector<ViewInfo>              VecViewInfo;

struct ItemDataID2Info
{
    int iDataID;
    bool bIsFocus;                      // 是否焦点
    yl::string strValue;                // 控件值
    yl::string strCustomValue;          // 控件自定义数据

    ItemDataID2Info()
    {
        Reset();
    }

    void Reset()
    {
        iDataID = DATA_ID_INVALID;
        bIsFocus = false;
        strValue = "";
        strCustomValue = "";
    }
};
typedef YLVector<ItemDataID2Info>       VecItemDataID2Info;


// MsgBox
enum MsgBoxResult
{
    MSGBOX_RESULT_NONE = -1,
    MSGBOX_RESULT_OK,
    MSGBOX_RESULT_CANCEL,
    MSGBOX_RESULT_TIMER_EXIT,
    MSGBOX_RESULT_EXTERNAL_EXIT
};

enum MsgBoxStyle
{
    MSGBOX_STYLE_NONE = -1,
    MSGBOX_STYLE_OK,
    MSGBOX_STYLE_CANCEL,
    MSGBOX_STYLE_OK_CANCEL
};

enum AnimationType
{
    ANIMATION_CYCLE_ROTATECALL,
};

#endif //_VIEWMODEUI_DEFINE_H_
