#ifndef _EXP_DEFINE_H__
#define _EXP_DEFINE_H__

#include "exp/include/explogicdefine.h"

#include "dsklog/log.h"
#include "dsklog/dsklog.h"

#include "ylstl/ylvector.h"
#include "ylstl/ylstring.h"

//#define _USE_SKIA

#ifdef _USE_SKIA
#include <android/log.h>

#define EXP_INFO(...) __android_log_print(ANDROID_LOG_DEBUG , "expcolor", __VA_ARGS__)
#define EXP_ERR(...) __android_log_print(ANDROID_LOG_DEBUG , "expcolor", __VA_ARGS__)
#define EXP_WARN(...) __android_log_print(ANDROID_LOG_DEBUG , "expcolor", __VA_ARGS__)
#else
#define EXP_INFO    /*printf*/DSSKEY_INFO
#define EXP_ERR /*printf*/DSSKEY_ERR
#define EXP_WARN    /*printf*/DSSKEY_WARN
#endif

//线程名称
#define VP_EXP_COLOR_THREAD_NAME "exp_drawer_color"

//是否需要绘制时旋转画布
#define EXP_PAINT_ROTATE

//测试绘制开关
// #define EXP_PAINT_TEST

typedef YLVector<ExpDisplayItemInfo> vecExpItemInfo;

#define EXP_REFRESH_RETRY_TIME      3

//旋转角度的定义（并不能任意更改，需要对应调整位置）
#define EXP_ROTATE_ANGLE                90
//屏幕尺寸
#define EXP_SCREEN_WIDTH                272
#define EXP_SCREEN_HEIGHT               480
//最多可接的扩展台个数
#define EXP_MAX_COUNT                   5
//每个扩展台的页数
#define EXP_PAGE_COUNT                  3
//按钮个数
#define EXP_BTN_ROW_COUNT           10
#define EXP_BTN_COL_COUNT               2
//分割线数量
#define EXP_CUT_LINE_COUNT          9

//默认按钮高度
#define EXP_BTN_WIDTH                       131
#define EXP_BTN_HEIGHT                      46
//两侧按钮背景的水平起始坐标
#define EXP_BTN_LEFTBG_LEFT             0
#define EXP_BTN_RIGHTBG_LEFT            141
//竖直微调距离
#define EXP_BTN_BG_SHADOW_MARGIN        0
//按钮背景的竖直起始坐标
#define EXP_BTN_BG_TOP_MARGIN       3
//按钮背景的总尺寸
#define EXP_BTN_BG_WIDTH                131
#define EXP_BTN_BG_HEIGHT               471

//分割线
//两侧分割线的水平起始坐标
#define EXP_BTN_LEFTBG_CUTLINE_LEFT     0
#define EXP_BTN_RIGHTBG_CUTLINE_LEFT        147
//分割线尺寸
#define EXP_CUTLINE_WIDTH               125
#define EXP_CUTLINE_HEIGHT              1
//分割线颜色
#define EXP_CUTLINE_COLOR_ALPHA 36
#define EXP_CUTLINE_COLOR_WHITE 255
//不同位置分割线的竖直坐标
#define EXP_BTN_BG_TOP                      4
static int s_DsskeyCutLineTop[EXP_CUT_LINE_COUNT] =
{
    EXP_BTN_BG_TOP + EXP_BTN_BG_SHADOW_MARGIN + EXP_BTN_HEIGHT * 1 + EXP_CUTLINE_HEIGHT * 0,
    EXP_BTN_BG_TOP + EXP_BTN_BG_SHADOW_MARGIN + EXP_BTN_HEIGHT * 2 + EXP_CUTLINE_HEIGHT * 1,
    EXP_BTN_BG_TOP + EXP_BTN_BG_SHADOW_MARGIN + EXP_BTN_HEIGHT * 3 + EXP_CUTLINE_HEIGHT * 2,
    EXP_BTN_BG_TOP + EXP_BTN_BG_SHADOW_MARGIN + EXP_BTN_HEIGHT * 4 + EXP_CUTLINE_HEIGHT * 3,
    EXP_BTN_BG_TOP + EXP_BTN_BG_SHADOW_MARGIN + EXP_BTN_HEIGHT * 5 + EXP_CUTLINE_HEIGHT * 4,
    EXP_BTN_BG_TOP + EXP_BTN_BG_SHADOW_MARGIN + EXP_BTN_HEIGHT * 6 + EXP_CUTLINE_HEIGHT * 5,
    EXP_BTN_BG_TOP + EXP_BTN_BG_SHADOW_MARGIN + EXP_BTN_HEIGHT * 7 + EXP_CUTLINE_HEIGHT * 6,
    EXP_BTN_BG_TOP + EXP_BTN_BG_SHADOW_MARGIN + EXP_BTN_HEIGHT * 8 + EXP_CUTLINE_HEIGHT * 7,
    EXP_BTN_BG_TOP + EXP_BTN_BG_SHADOW_MARGIN + EXP_BTN_HEIGHT * 9 + EXP_CUTLINE_HEIGHT * 8,
};

//按钮整体（用于按下刷新）
//两侧按钮的水平起始坐标
#define EXP_FULL_LEFT_BTN_LEFT                          0
#define EXP_FULL_RIGHT_BTN_LEFT                         142
//按钮占用尺寸
#define EXP_FULL_BTN_WIDTH                                  130
#define EXP_FULL_BTN_HEIGHT                             46

#define EXP_FULL_BTN_FIRST_MARGIN                   46
#define EXP_FULL_BTN_CUTLINE_MARGIN             1

//不同位置按钮的竖直坐标
static int s_DsskeyFullBtnTop[EXP_BTN_ROW_COUNT] =
{
    s_DsskeyCutLineTop[0] - EXP_FULL_BTN_FIRST_MARGIN,
    s_DsskeyCutLineTop[0] + EXP_FULL_BTN_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[1] + EXP_FULL_BTN_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[2] + EXP_FULL_BTN_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[3] + EXP_FULL_BTN_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[4] + EXP_FULL_BTN_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[5] + EXP_FULL_BTN_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[6] + EXP_FULL_BTN_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[7] + EXP_FULL_BTN_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[8] + EXP_FULL_BTN_CUTLINE_MARGIN,
};

//按钮整体（用于局部刷新）
//两侧按钮的水平起始坐标
#define EXP_LEFT_BTN_LEFT                               8
#define EXP_RIGHT_BTN_LEFT                          149
//按钮占用尺寸
#define EXP_BTN_FLUSH_WIDTH                     120
#define EXP_BTN_FLUSH_HEIGHT                        32

#define EXP_BTN_FLUSH_FIRST_MARGIN      38
#define EXP_BTN_FLUSH_CUTLINE_MARGIN    8

//不同位置按钮的竖直坐标
static int s_DsskeyBtnTop[EXP_BTN_ROW_COUNT] =
{
    s_DsskeyCutLineTop[0] - EXP_BTN_FLUSH_FIRST_MARGIN,
    s_DsskeyCutLineTop[0] + EXP_BTN_FLUSH_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[1] + EXP_BTN_FLUSH_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[2] + EXP_BTN_FLUSH_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[3] + EXP_BTN_FLUSH_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[4] + EXP_BTN_FLUSH_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[5] + EXP_BTN_FLUSH_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[6] + EXP_BTN_FLUSH_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[7] + EXP_BTN_FLUSH_CUTLINE_MARGIN,
    s_DsskeyCutLineTop[8] + EXP_BTN_FLUSH_CUTLINE_MARGIN,
};

//图标
//两侧图标的水平起始位置
#define ICON_LEFT_PART_LEFT         8
#define ICON_RIGHT_PART_LEFT        149
#define ICON_RIGHT_PART_RIGHT       240
//图标尺寸
#define ICON_WIDTH                      24
#define ICON_HEIGHT                     24
//图标与上方相邻的分割线的距离
#define CUTLINE_ICON_MARGIN     12
//第一个图标顶端坐标与下方分割线的距离
#define CUTLINE_ICON_FIRST_MARGIN       (11 + ICON_HEIGHT)
//不同位置图标的竖直坐标
static int s_DsskeyIconTop[EXP_BTN_ROW_COUNT] =
{
    s_DsskeyCutLineTop[0] - CUTLINE_ICON_FIRST_MARGIN,
    s_DsskeyCutLineTop[0] + CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[1] + CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[2] + CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[3] + CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[4] + CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[5] + CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[6] + CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[7] + CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[8] + CUTLINE_ICON_MARGIN,
};

//状态图标
//两侧图标的水平起始位置
#define STATUS_ICON_LEFT_PART_LEFT          25
#define STATUS_ICON_RIGHT_PART_LEFT     166
#define STATUS_ICON_RIGHT_PART_RIGHT    235

#define STATUS_NEW_ICON_LEFT_PART_LEFT      20
#define STATUS_NEW_ICON_RIGHT_PART_LEFT     161
#define STATUS_NEW_ICON_RIGHT_PART_RIGHT    240
//图标尺寸
#define STATUS_ICON_WIDTH           12
#define STATUS_ICON_HEIGHT          12
//图标与上方相邻的分割线的距离
#define STATUS_CUTLINE_ICON_MARGIN      28
#define STATUS_CUTLINE_NEW_ICON_MARGIN  24

//第一个图标顶端坐标与下方分割线的距离
#define STATUS_CUTLINE_ICON_FIRST_MARGIN        (6 + STATUS_ICON_HEIGHT)

#define STATUS_CUTLINE_NEW_ICON_FIRST_MARGIN        (11 + STATUS_ICON_HEIGHT)

//不同位置图标的竖直坐标
static int s_DsskeyStatusTop[EXP_BTN_ROW_COUNT] =
{
    s_DsskeyCutLineTop[0] - STATUS_CUTLINE_ICON_FIRST_MARGIN,
    s_DsskeyCutLineTop[0] + STATUS_CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[1] + STATUS_CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[2] + STATUS_CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[3] + STATUS_CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[4] + STATUS_CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[5] + STATUS_CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[6] + STATUS_CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[7] + STATUS_CUTLINE_ICON_MARGIN,
    s_DsskeyCutLineTop[8] + STATUS_CUTLINE_ICON_MARGIN,
};

static int s_DsskeyNewStatusTop[EXP_BTN_ROW_COUNT] =
{
    s_DsskeyCutLineTop[0] - STATUS_CUTLINE_NEW_ICON_FIRST_MARGIN,
    s_DsskeyCutLineTop[0] + STATUS_CUTLINE_NEW_ICON_MARGIN,
    s_DsskeyCutLineTop[1] + STATUS_CUTLINE_NEW_ICON_MARGIN,
    s_DsskeyCutLineTop[2] + STATUS_CUTLINE_NEW_ICON_MARGIN,
    s_DsskeyCutLineTop[3] + STATUS_CUTLINE_NEW_ICON_MARGIN,
    s_DsskeyCutLineTop[4] + STATUS_CUTLINE_NEW_ICON_MARGIN,
    s_DsskeyCutLineTop[5] + STATUS_CUTLINE_NEW_ICON_MARGIN,
    s_DsskeyCutLineTop[6] + STATUS_CUTLINE_NEW_ICON_MARGIN,
    s_DsskeyCutLineTop[7] + STATUS_CUTLINE_NEW_ICON_MARGIN,
    s_DsskeyCutLineTop[8] + STATUS_CUTLINE_NEW_ICON_MARGIN,
};

#define STATUS_ICON_OFFSET_H                5
#define STATUS_ICON_OFFSET_V                4

//文字
//字体大小
#define TEXT_FONT_SIZE                  16
#define TEXT_FONT_TESTMODE_SIZE 26
//文字最大长度
#define TEXT_MAX_LENGTH             12
//两侧文字的水平起始坐标
#define TEXT_LEFT_PART_LEFT         36
#define TEXT_RIGHT_PART_LEFT        177
#define TEXT_RIGHT_PART_RIGHT       144
//文字绘制区域尺寸
#define TEXT_WIDTH                          91
#define TEXT_HEIGHT                     30
//文字与上方相邻分割线的距离
#define CUTLINE_TEXT_MARGIN     8
//第一行文字顶端坐标与下方分割线的距离
#define CUTLINE_TEXT_FIRST_MARGIN       (CUTLINE_TEXT_MARGIN + TEXT_HEIGHT)
//不同位置文字绘制区域的竖直坐标
static int s_DsskeyTextTop[EXP_BTN_ROW_COUNT] =
{
    s_DsskeyCutLineTop[0] - CUTLINE_TEXT_FIRST_MARGIN,
    s_DsskeyCutLineTop[0] + CUTLINE_TEXT_MARGIN,
    s_DsskeyCutLineTop[1] + CUTLINE_TEXT_MARGIN,
    s_DsskeyCutLineTop[2] + CUTLINE_TEXT_MARGIN,
    s_DsskeyCutLineTop[3] + CUTLINE_TEXT_MARGIN,
    s_DsskeyCutLineTop[4] + CUTLINE_TEXT_MARGIN,
    s_DsskeyCutLineTop[5] + CUTLINE_TEXT_MARGIN,
    s_DsskeyCutLineTop[6] + CUTLINE_TEXT_MARGIN,
    s_DsskeyCutLineTop[7] + CUTLINE_TEXT_MARGIN,
    s_DsskeyCutLineTop[8] + CUTLINE_TEXT_MARGIN,
};

#ifdef _USE_SKIA
#define TEXT_ALIGN_VERTICAL_MARGIN      22
#endif

#if defined(WIN32) && defined(_EXP_COLOR_SUPPORT)
#include "include/picpath.h"
#include "include/uicommon.h"
#include "baseui/t4xpicpath.h"
#include "dsskeyui/include/dsskeyuicommondefine.h"
#include "lamp/ledlamp/include/modledlamp.h"
#else
//ui图标定义
enum Dsskey_Icon_Type
{
    DK_ICON_None = 0,

    DK_ICON_Default,

    // common
    DK_ICON_UnRegistered,
    DK_ICON_Registering,
    DK_ICON_Registered,
    DK_ICON_Using,
//#ifdef _T42
    DK_ICON_Callout,
//#endif
    DK_ICON_Speeddial,
    DK_ICON_Remote_Offline,
    DK_ICON_Remote_Talking,
    DK_ICON_Remote_Online,
    DK_ICON_Remote_Ringing,

    // linekey
    DK_ICON_RemoteDir,

    // EXP39
    DK_ICON_DND,
    DK_ICON_Forward,
    DK_ICON_HOLD,
    DK_ICON_SMS,
    DK_ICON_Triangle,
    DK_ICON_Warning,

    //TO DO:
    // Call Center
    //DK_ICON_ACD,
    //DK_ICON_HOTELING,
    //DK_ICON_ACD_TRACE,
    //DK_ICON_DISPCODE,
    //DK_ICON_ESCALATE,
#ifdef _T42
    DK_ICON_ACD_IDLE,
    DK_ICON_ACD_AVAILABLE,
    DK_ICON_ACD_UNAVAILABLE,
    DK_ICON_ACD_WRAPUP,
    DK_ICON_RINGING,
    DK_ICON_BLA,
#endif

    // 页面编号
    DK_ICON_Page_1,
    DK_ICON_Page_2,
    DK_ICON_BLF_PARK,
    DK_ICON_BLF_CALLOUT,
    DK_ICON_BLF_IDLE,
    DK_ICON_BLF_UNKNOWN,
    DK_ICON_BLF_TALK,
    DK_ICON_BLF_RINGING,
    DK_ICON_BLF_HOLD,

    DK_ICON_BLA_PARK,
    DK_ICON_BLA_UNKNOWN,
    DK_ICON_BLA_SEIZED,
    DK_ICON_BLA_PROGRESSING,
    DK_ICON_BLA_ACTIVE,
    DK_ICON_BLA_HELD,
    DK_ICON_BLA_HELD_PRIVATE,
    DK_ICON_BLA_BRIDGE_ACTIVE,
    DK_ICON_BLA_BRIDGE_HELD,
    DK_ICON_BLA_IDLE,
    DK_ICON_BLA_ALERTING,

    DK_ICON_INTERCOM,
    DK_ICON_INTERCOM_TALKING,
    DK_ICON_INTERCOM_HOLD,
    DK_ICON_INTERCOM_RINGING,

    DK_ICON_LINE_FAIL,
    DK_ICON_LINE_FORWARD,
    DK_ICON_LINE_DND,

    DK_ICON_VOICE_MAIL,

    DK_ICON_PICK_UP,
    DK_ICON_GROUP_PICK_UP,
    DK_ICON_CALLPARK,
    DK_ICON_CALLPARK_FAIL,
    DK_ICON_CALLPARK_RINGING,
    DK_ICON_CALLPARK_AUTO_TALK,
    DK_ICON_CALLPARK_MUNUAL_TALK,

    DK_ICON_DTMF,
    DK_ICON_PREFIX_ICON,

    DK_ICON_LOCAL_GROUP,
    DK_ICON_DIRECTORY,
    DK_ICON_XMLBROWSER,

    DK_ICON_LDAP,
    DK_ICON_FAVORITES,

    DK_ICON_CONFERENCE,
    DK_ICON_TRANSFER,
    DK_ICON_REDIAL,

    DK_ICON_TALK,
    DK_ICON_CALLRETURN,

    DK_ICON_RECORD,
    DK_ICON_RECORDING,

    DK_ICON_PAGING,
    DK_ICON_GROUP_LISTENING,

    DK_ICON_COLOR_ACD_AVAILABLE,
    DK_ICON_COLOR_ACD_UNAVAILABLE,
    DK_ICON_COLOR_ACD_WRAPUP,
    DK_ICON_COLOR_ACD_TRACE,
    DK_ICON_ACD_LOGOUT,
    DK_ICON_ACD_DEFAULT,

    DK_ICON_ZERO_TOUCH,
    DK_ICON_MEET_ME,
    DK_ICON_HOTELING_LOGININ,
    DK_ICON_HOTELING_LOGINOUT,

    DK_ICON_DISPCODE,
    DK_ICON_ESCALATE,

    DK_ICON_RETRIEVE_PARK,
    DK_ICON_HOTDESKING,
    DK_ICON_BUDDIES,
    DK_ICON_STATUS,

    DK_ICON_EDK,
    DK_ICON_KEYPAD_LOCK,
    DK_ICON_GET_BALANCE,

    DK_ICON_UC_ONLINE,
    DK_ICON_UC_AWAY,
    DK_ICON_UC_EXTENDED_AWAY,
    DK_ICON_UC_BUSY,
    DK_ICON_UC_OFFLINE,

    DK_ICON_URL_GREEN,
    DK_ICON_URL_RED,
    DK_ICON_URL,

    DK_ICON_MOBILE_BLUE_GREEN,
    DK_ICON_MOBILE_BLUE_CONNETING,
    DK_ICON_MOBILE_BLUE_RED,

    DK_ICON_EDITING,

    DK_ICON_METASWITCH_MESSAGE_LIST,
    DK_ICON_METASWITCH_LOGOUT,

    //自定义图片类型
    DK_ICON_CUSTOM,
};

//ui 状态定义
enum PRESENCE_STATUS
{
    PRESENCE_STATUS_NONE = 0,

    PRESENCE_STATUS_ONLINE,
    PRESENCE_STATUS_CHATTY,
    PRESENCE_STATUS_AWAY,
    PRESENCE_STATUS_EXTENDEDAWAY,
    PRESENCE_STATUS_BUSY,
    PRESENCE_STATUS_OFFLINE,
    PRESENCE_STATUS_UNKNOWN,

    PRESENCE_STATUS_MAX,
};

//液晶控制类型
enum LIGHT_Type
{
    LT_OFF = 0,
    LT_GREEN_ON,
    LT_RED_ON,
    LT_RED_GREEN_ON,
    LT_RED_FLASH_1,
    LT_RED_FLASH_2,
    LT_RED_FLASH_3,
    LT_RED_FLASH_4,
    LT_GREEN_FLASH_1,//快
    LT_GREEN_FLASH_2,
    LT_GREEN_FLASH_3,
    LT_GREEN_FLASH_4,//慢
    LT_RED_GREEN_FLASH_1,
    LT_RED_GREEN_FLASH_2,
    LT_RED_GREEN_FLASH_3,
    LT_RED_GREEN_FLASH_4,

    LT_NONE = 0xff,
};
#endif

//按钮按下效果的图片路径
#ifdef _USE_SKIA
#define PIC_DSSKEY_BTN_DOWN             SYSTEM_IMAGE_PATH "expcolor/Theme1/Theme#Theme1@expkey_press.png"
#define PIC_DSSKEY_BTN_HEIGHT           SYSTEM_IMAGE_PATH "expcolor/Theme1/Theme#Theme1@expkey_height.png"
#define PIC_DSSKEY_BTN_USE              SYSTEM_IMAGE_PATH "expcolor/Theme1/Theme#Theme1@expkey_use.png"

#define PIC_DEFAULT_EXP_BACKGROUND  SYSTEM_RESOURCE_PATH "image/Default-exp50.jpg"
#define PIC_DEFAULT_EXP_SCREENSAVER SYSTEM_RESOURCE_PATH "image/Default-exp50-ScreenSaver.jpg"

#define PIC_ROOT_PATH               SYSTEM_IMAGE_PATH "expcolor/"

#define FONT_PATH                       "/system/fonts/WenQuanYiMicroHei.ttf"

#else
#define PIC_DSSKEY_BTN_DOWN             THEME_DEFAULT_PATH "t54/Theme1/Theme#Theme1@expkey_press.png"
#define PIC_DSSKEY_BTN_HEIGHT           THEME_DEFAULT_PATH "t54/Theme1/Theme#Theme1@expkey_height.png"
#define PIC_DSSKEY_BTN_USE              THEME_DEFAULT_PATH "t54/Theme1/Theme#Theme1@expkey_use.png"

#define PIC_DEFAULT_EXP_BACKGROUND  SYSTEM_RESOURCE_PATH "exp50/Default-exp50.jpg"
#define PIC_DEFAULT_EXP_SCREENSAVER SYSTEM_RESOURCE_PATH "exp50/Default-Screensaver-exp50.jpg"

#define PIC_ROOT_PATH               THEME_DEFAULT_PATH "t54/"
#endif

#define EXPUSB_FB                                   "/dev/expusb0"

#define EXP_IS_USE_IMAGE_CACHE_IN_EXPCOLOR 0

#define MYPAINTER_COLOR_BLACK 0, 0, 0, 255
#define MYPAINTER_COLOR_WHITE 255, 255, 255, 255

enum DSSKEY_AlIGN_TYPE
{
    DSSKEY_ALIGN_NORMOL = 1,
    DSSKEY_AlIGN_RIGHT,
};
#endif //_EXP_DEFINE_H
