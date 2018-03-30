#ifndef _DEVICE_LIB_STRUCT_DEF_HEADER_
#define  _DEVICE_LIB_STRUCT_DEF_HEADER_

#include "exp/src/expdrive/expdriveheader/exp_drv.h"
#include "exp/src/expdrive/expdriveheader/expusb_api.h"
typedef int ExpID;
typedef int ExpHandle;
typedef int ExpType;  // this ExpType was defined by exp_drv.h


//驱动宏定义
#define EXT_LED_CTRL            26

#define LCD_DEV_NAME        "/dev/keypad0"

#define EXP_DEV_NAME        "/dev/exp0"
#define EXP_USB_DEV_NAME    "/dev/expusb0"

#define EXP_USB_APP_FILE        BOOT_PATH "bin/expapp.bin"
#define EXP_KEYNUM_PAGE 20

#define EXP_KEY_PAGE1 20
#define EXP_KEY_PAGE2 21
#define EXP_KEY_PAGE3 22
#define EXP_INVALID_KEY_ID   -1
#define EXP_KEY_NONE        -1

#define EXP_INSERT_DELAY_RESPONSE_TIME 500
#define EXP_KEY_LONG_PRESS_TIME 1500

//扩展台支持类型
enum DSSKEY_EXT_TYPE
{
	DT_ET_NONE = 0,        //不支持
	DT_ET_BLACK = 1 << 0,   //黑白屏扩展台
	DT_ET_COLOR = 1 << 1,   //彩屏扩展台
	DT_ET_EHS = 1 << 2    //EHS
};

enum EXP_DRIVE_TYPE
{
    EXP_DRIVE_TYPE_INVALID = 0,
    EXP_DRIVE_TYPE_RJ,
    EXP_DRIVE_TYPE_USB,
};

enum EXP_DISPLAY_POSITION
{
    EDP_NORMAL = 0x00, // 奇数行左对齐，偶数行右对齐
    EDP_CENTER = 0x01, // 居中显示
};

// 扩展台类型
// 不能用1、2、3表示，否则和普通按键键值冲突
enum EXP_TYPE
{
    ET_NONE = 0,
    ET_EXP20 = 20,
    ET_EXP38 = 38,
    ET_EXP39 = 39,
    ET_EXP40 = 40,
    ET_EXP50 = 50,
    ET_EHS = 41,
    ET_EXP_CM69 = 69,
};

enum DEVICE_TYPE_DEF
{

    DT_INVALID = -1,
    DT_ALL = 0,
    DT_EXP_20 = 20,
    DT_EXP_38 = 38,
    DT_EXP_39 = 39,
    DT_EXP_40 = 40,
    DT_EXP_50 = 50,
    DT_EXP_EHS = 41,
    DT_EXP_CM68 = 68,
    DT_EXP_CM69 = 69
};
#ifdef _EXPCOLOR
#define MAX_EXP_NUM     5
#else
#define MAX_EXP_NUM     6
#endif

enum DEV_MESSAGE_CODE
{
    DEV_MSG_MESSAGE_FIRST = 0x60001,
    DEV_MSG_KEY_RELEASED,           // wParam:0/1(0:键盘,1:扩展台)     lParam:0/KEY_CODE,此处的KEY_CODE为按键弹起的扩展台序号
    DEV_MSG_EXP_INSERT,             // wparam:hop index                 lParam:exp type
    DEV_MSG_EXP_REMOVE,             // wparam:hop index                 lParam:exp type
    DEV_MSG_EHS_INSERT,             // wparam:hop index                 lParam:0
    DEV_MSG_EHS_REMOVE,             // wparam:hop index                 lParam:0

    //T42
    DEV_MSG_DIGITKEY_EVENT,         // wParam:0                         lParam:KEY_CODE
    DEV_MSG_FUNKEY_EVENT,           // wParam:0/1/0x100(0:键盘,1:扩展台,0x100:耳麦)      lParam:KEY_CODE

    DEV_MSG_EXP_KEY,      //1c wParam:按键组合值       lParam:0/1(按下/弹起)
    DEV_MSG_EHS_KEY,       //1c wParam:按键组合值      lParam:0/1(按下/弹起) //fixed me 此消息待定义
    DEV_MSG_ELP_KEY,
    DEV_MSG_EXP_KEYDOWN,
    DEV_MSG_EXP_KEYUP,
    DEV_EXP_INSERT,
    DEV_EHS_INSERT,
    DEV_MSG_EXP_PAGE_KEY,   //翻页键按下
    DEV_MSG_EXP_KEY_TEST, //Exp随机按键测试 wParam:按键值 lParam:0/1 (弹起/按下)
    DEV_MSG_EXP_KEY_AFTER_LONG_PRESS_RELEASE, // 长按之后弹起的消息单独发送
    DEV_MSG_MESSAGE_LAST
};

// EXP模块需要处理的消息
enum EXP_MSG_CODE
{
    EXP_MSG_FIRST = 0x6000,   // 24576
    EXP_MSG_CTRL,             // 01 wParam:io ID    lParam:0    extra: ctrl data
    EXP_MSG_EXP_READY,        // 扩展台执行文件下载完成   wParam:expindex
    EXP_MSG_EXP_KEY_EVENT,    // wParam:keyindex        lParam:istatus
    EXP_MSG_EXP_POWER_LACK,   // 扩展台供电不足的提示信息
    EXP_MSG_EXP_REGNIZE_FAIL, // 扩展台识别错误的提示信息
    EXP_MSG_LAST
};

enum EXP50_KEY_INDEX
{
    EXP50_KEY_1 = 0,
    EXP50_KEY_2 = 1,
    EXP50_KEY_3 = 2,
    EXP50_KEY_4 = 3,
    EXP50_KEY_5 = 4,
    EXP50_KEY_6 = 5,
    EXP50_KEY_7 = 6,
    EXP50_KEY_8 = 7,
    EXP50_KEY_9 = 8,
    EXP50_KEY_10 = 9,
    EXP50_KEY_11 = 10,
    EXP50_KEY_12 = 11,
    EXP50_KEY_13 = 12,
    EXP50_KEY_14 = 13,
    EXP50_KEY_15 = 14,
    EXP50_KEY_16 = 15,
    EXP50_KEY_17 = 16,
    EXP50_KEY_18 = 17,
    EXP50_KEY_19 = 18,
    EXP50_KEY_20 = 19,
    EXP50_KEY_PAGE1 = 20,
    EXP50_KEY_PAGE2 = 21,
    EXP50_KEY_PAGE3 = 22,
};

//!!!!!
enum EXP40_KEY_INDEX
{
    EXP40_KEY_1 = (0 << 8) + 0,
    EXP40_KEY_2 = (0 << 8) + 1,
    EXP40_KEY_3 = (1 << 8) + 0,
    EXP40_KEY_4 = (1 << 8) + 1,
    EXP40_KEY_5 = (2 << 8) + 0,
    EXP40_KEY_6 = (2 << 8) + 1,
    EXP40_KEY_7 = (3 << 8) + 0,
    EXP40_KEY_8 = (3 << 8) + 1,
    EXP40_KEY_9 = (4 << 8) + 0,
    EXP40_KEY_10 = (4 << 8) + 1,
    EXP40_KEY_11 = (5 << 8) + 0,
    EXP40_KEY_12 = (5 << 8) + 1,
    EXP40_KEY_13 = (6 << 8) + 0,
    EXP40_KEY_14 = (6 << 8) + 1,
    EXP40_KEY_15 = (7 << 8) + 0,
    EXP40_KEY_16 = (7 << 8) + 1,
    EXP40_KEY_17 = (8 << 8) + 0,
    EXP40_KEY_18 = (8 << 8) + 1,
    EXP40_KEY_19 = (9 << 8) + 0,
    EXP40_KEY_20 = (9 << 8) + 1,
    EXP40_KEY_PAGE1 = (10 << 8) + 0,
    EXP40_KEY_PAGE2 = (10 << 8) + 1,
};

enum EXP38_KEY_INDEX
{
    EXP_INSERT = 0x30,
    EXP_REMOVE = 0x31,
    EXP_KEY_RELEASE = 0x3f,
    EXP_EHS = 0x33,
    EXP38_KEY_1 = (0 << 8) + 0,
    EXP38_KEY_2 = (0 << 8) + 1,
    EXP38_KEY_3 = (1 << 8) + 0,
    EXP38_KEY_4 = (1 << 8) + 1,
    EXP38_KEY_5 = (2 << 8) + 0,
    EXP38_KEY_6 = (2 << 8) + 1,
    EXP38_KEY_7 = (3 << 8) + 0,
    EXP38_KEY_8 = (3 << 8) + 1,
    EXP38_KEY_9 = (4 << 8) + 0,
    EXP38_KEY_10 = (4 << 8) + 1,
    EXP38_KEY_11 = (5 << 8) + 0,
    EXP38_KEY_12 = (5 << 8) + 1,
    EXP38_KEY_13 = (6 << 8) + 0,
    EXP38_KEY_14 = (6 << 8) + 1,
    EXP38_KEY_15 = (7 << 8) + 0,
    EXP38_KEY_16 = (7 << 8) + 1,
    EXP38_KEY_17 = (8 << 8) + 0,
    EXP38_KEY_18 = (8 << 8) + 1,
    EXP38_KEY_19 = (9 << 8) + 0,
    EXP38_KEY_20 = (0 << 8) + 2,
    EXP38_KEY_21 = (0 << 8) + 3,
    EXP38_KEY_22 = (1 << 8) + 2,
    EXP38_KEY_23 = (1 << 8) + 3,
    EXP38_KEY_24 = (2 << 8) + 2,
    EXP38_KEY_25 = (2 << 8) + 3,
    EXP38_KEY_26 = (3 << 8) + 2,
    EXP38_KEY_27 = (3 << 8) + 3,
    EXP38_KEY_28 = (4 << 8) + 2,
    EXP38_KEY_29 = (4 << 8) + 3,
    EXP38_KEY_30 = (5 << 8) + 2,
    EXP38_KEY_31 = (5 << 8) + 3,
    EXP38_KEY_32 = (6 << 8) + 2,
    EXP38_KEY_33 = (6 << 8) + 3,
    EXP38_KEY_34 = (7 << 8) + 2,
    EXP38_KEY_35 = (7 << 8) + 3,
    EXP38_KEY_36 = (8 << 8) + 2,
    EXP38_KEY_37 = (8 << 8) + 3,
    EXP38_KEY_38 = (9 << 8) + 2,
};

enum EXP39_KEY_INDEX
{
    EXP39_KEY_1 = (0 << 8) + 0,
    EXP39_KEY_2 = (0 << 8) + 1,
    EXP39_KEY_3 = (1 << 8) + 0,
    EXP39_KEY_4 = (1 << 8) + 1,
    EXP39_KEY_5 = (2 << 8) + 0,
    EXP39_KEY_6 = (2 << 8) + 1,
    EXP39_KEY_7 = (3 << 8) + 0,
    EXP39_KEY_8 = (3 << 8) + 1,
    EXP39_KEY_9 = (4 << 8) + 0,
    EXP39_KEY_10 = (4 << 8) + 1,
    EXP39_KEY_11 = (5 << 8) + 0,
    EXP39_KEY_12 = (5 << 8) + 1,
    EXP39_KEY_13 = (6 << 8) + 0,
    EXP39_KEY_14 = (6 << 8) + 1,
    EXP39_KEY_15 = (7 << 8) + 0,
    EXP39_KEY_16 = (7 << 8) + 1,
    EXP39_KEY_17 = (8 << 8) + 0,
    EXP39_KEY_18 = (8 << 8) + 1,
    EXP39_KEY_19 = (9 << 8) + 0,
    EXP39_KEY_20 = (9 << 8) + 1,
};

enum EXP_ICON_TYPE
{
    EIT_NONE = 0x00,

    EIT_REMOTE_OFFLINE = 0x01,
    EIT_REMOTE_TALKING = 0x02,
    EIT_REMOTE_ONLINE = 0x03,
    EIT_REMOTE_RINGING = 0x04,

    EIT_DND = 0x05,
    EIT_HOLD = 0x06,

    EIT_REGISTERED = 0x07,
    EIT_UNREGISTERED = 0x08,
    EIT_USING = 0x09,
    EIT_REGISTERING = 0x0a,

    EIT_SMS = 0x0b,
    EIT_SPEEDDIAL = 0x0c,

    EIT_TRIANGLE = 0x0d,
    EIT_WARNING = 0x0e,
    EIT_PAGE_1 = 0x0f,
    EIT_PAGE_2 = 0x10,
    EIT_PAGE_NONE = 0x11,
    // 13-12-20新增BLF状态图标
    EIT_BLF_PARK = 0x12,
    EIT_BLF_CALLOUT = 0x13,
    EIT_BLF_IDLE = 0x14,
    EIT_BLF_UNKNOWN = 0x15,
    EIT_BLF_TALK = 0x16,
    EIT_BLF_RINGING = 0x17,
    EIT_BLF_HOLD = 0x18,
};

#define EXT_LCD_DISPLAY         41
#define EXT_CONTRAST_VALUE      43
#define EXT_PICTURE_DISPLAY     44
#define EXT_LCD_TEST            45
#define EXT_SCREEN_SWITCH       46
#define EXT_BACKLIGHT_VALUE     47
#define EXT_STATE_ICON          48

//EXp39显示时传给驱动的数据
struct EXP_Display_Data
{
    unsigned char * pData;
    unsigned int  uLength;
};

// 显示的灰度模式
enum EXP_GRAY_MODE
{
    EGM_NORMAL = 0x00,  // 普通模式
    EGM_GRAY0  = 0x01,  // 灰度0
    EGM_GRAY1  = 0x02,  // 灰度1
    EGM_INVERT = 0x03,  // 反选
};
#endif

