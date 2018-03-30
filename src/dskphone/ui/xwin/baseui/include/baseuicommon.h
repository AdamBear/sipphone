#ifndef __BASE_UI_COMMON_H__
#define __BASE_UI_COMMON_H__

#include "ylstring.h"
#include "ylvector.h"
#include <ETLLib/ETLLib.hpp>
#include "xwindow/xwindowcommon.h"
#include "xwindow/xGraphic.h"
#include "settinguilogic/include/settingdefine.h"
#include "imagecache/include/imagecachedefine.h"

typedef struct
{
    int m_nAction;
    yl::string m_strTitle;
} ST_OPTION_ITEM_DATA;

typedef YLVector<ST_OPTION_ITEM_DATA> VEC_OPTION_ITEM_DATA;

#ifndef WIN32
#define _snprintf snprintf
#endif

extern const char * BASE_UI_LAYOUT_FILE_OPTION_MENU;
extern const char * BASE_UI_LAYOUT_FILE_DETAIL_LIST_ITEM;
extern const char * BASE_UI_LAYOUT_FILE_EDIT_LIST_ITEM;
extern const char* BASE_UI_LAYOUT_FILE_TWO_LINE_EDIT_LIST_ITEM;
extern const char * BASE_UI_LAYOUT_FILE_COMBOBOX_LIST_ITEM;
extern const char * BASE_UI_LAYOUT_FILE_MENU_LIST_ITEM;
extern const char * BASE_UI_LAYOUT_FILE_RADIO_LIST_ITEM;
extern const char * BASE_UI_LAYOUT_FILE_SELECT_LIST_ITEM;
extern const char * BASE_UI_LAYOUT_FILE_TEXTSTATUS_LIST_ITEM;

extern const char * BASE_UI_ID_TEXT_TITLE;
extern const char * BASE_UI_ID_TEXT_VALUE;
extern const char * BASE_UI_ID_EDIT_VALUE;
extern const char * BASE_UI_ID_COMBO_VALUE;
extern const char * BASE_UI_ID_TEXT_MENU;
extern const char * BASE_UI_ID_ICON_RADIO;
extern const char * BASE_UI_ID_ICON_CHECK;
extern const char * BASE_UI_ID_LIST_VIEW;
extern const char* BASE_UI_ID_BTN_RADIO;

extern const char * BASE_UI_ATTR_TIME_SIZE;
extern const char * BASE_UI_ATTR_DATE_SIZE;
extern const char * BASE_UI_ATTR_LINE_SPACING;
extern const char * BASE_UI_ATTR_DATE_TIME_FORMAT;  // 时间日期显示格式
extern const char * BASE_UI_ATTR_DATE_FORMAT;       // 日期显示格式
extern const char * BASE_UI_ATTR_TIME_FORMAT;       // 时间日期显示格式
extern const char * BASE_UI_ATTR_FORMAT;

// 时间日期显示格式
extern const char * BASE_UI_ENUM_DATE_TIME_FORMAT_TIME_DATE;            // Time Date
extern const char *
BASE_UI_ENUM_DATE_TIME_FORMAT_TWO_LINE;             // 两行，第一行时间，第二行日期，字体不一样
extern const char *
BASE_UI_ENUM_DATE_TIME_FORMAT_LEFT_DATE_MID_TIME;   // Date      Time       (right)
extern const char* BASE_UI_ENUM_DATE_TIME_FORMAT_TIME;     // Time
// 日期显示格式
extern const char * BASE_UI_ENUM_DATE_FORMAT_WWW_MMM_DD;    // WWW MMM DD
extern const char * BASE_UI_ENUM_DATE_FORMAT_DD_MMM_YY;     // DD-MM-YY
extern const char * BASE_UI_ENUM_DATE_FORMAT_YYYY_MM_DD;    // YYYY-MM-DD
extern const char * BASE_UI_ENUM_DATE_FORMAT_DD_MM_YYYY;    // DD/MM/YYYY
extern const char * BASE_UI_ENUM_DATE_FORMAT_MM_DD_YY;      // MM/DD/YY
extern const char * BASE_UI_ENUM_DATE_FORMAT_DD_MMM_YYYY;   // DD MM YYYY
extern const char * BASE_UI_ENUM_DATE_FORMAT_WWW_DD_MMM;    // WWW DD MMM
extern const char * BASE_UI_ENUM_DATE_FORMAT_T20_DD_MM_YY;  //显示格式： DD/MM/YY
extern const char * BASE_UI_ENUM_DATE_FORMAT_T20_YY_MM_DD;  //显示格式： YY/MM/DD
// 时间显示格式
extern const char * BASE_UI_ENUM_TIME_FORMAT_HH_MM_APM;         // HH:MM A/PM
extern const char * BASE_UI_ENUM_TIME_FORMAT_HH_MM;             // HH:MM;
extern const char * BASE_UI_ENUM_TIME_FORMAT_HH_MM_SS;          // HH:MM:SS
extern const char * BASE_UI_ENUM_TIME_FORMAT_HH_MM_SMALL_SS;    // HH:MM ss

#define MAX_LEN_URL             511 //url，最大长度512
#define MAX_LEN_DISPLAY_NAME    99  // 用户名、显示名最大长度
#define MAX_LEN_DISPLAY_NAME_SHORT      32  // 显示名
#define MAX_LEN_SERVER_NAME     99 //server name,as time.windows.com
#define MAX_LEN_PASSWORD        99  // 密码相关最大长度
#define MAX_LEN_UNLOCKPIN       15  // 键盘锁相关最大长度
#define MAX_LEN_PHONE_NUMBER    99  //存储的号码长度
#define MAX_LEN_PHONE_NUMBER_LONG   99  //存储的号码长度
#define MAX_LEN_IP              99  // IP相关最大长度，port除外（增加了IPv6，参照T2X,设置为99）
#define MAX_LEN_PORT            5   // 端口最大长度
#define MAX_LEN_DIAL            32  // Idle拨号最大长度
#define MAX_LEN_MAIL            50  // email地址
#define MAX_LEN_NUMBER          3   //输入整数，一般为时间设置
#define MAX_LEN_KEYPAD_TIME_OUT 4   //输入整数，键盘锁上锁时间长度
#define MAX_LEN_NUMBER_SHORT    2   //两位整数，如hotlinedelay
#define MAX_LEN_AES             16  //AES界面的输入框限制16位
#define MAX_LEN_DTMF            32  //DTMF类型使用
#define MAX_LEN_PROXY           255 //OutboundProxy
#define MAX_LEN_BSFT_NAME       50  //BSFT Personal名字最大长度
#define MAX_LEN_BSFT_NUMBER     23  //BSFT Number最大长度
#define MAX_LEN_DSSKEY_VALUE    99  //DssKey Value最大长度

#define IMPLEMENT_MULTI_WORKSTATION_INSTANCE(className) \
className& className::GetInstance() \
{ \
    MULTI_KERNEL_INSTANCE(className) \
}

#define IMPLEMENT_WORKSTATION_INSTANCE(className) \
className& className::GetInstance() \
{ \
    SINGLE_KERNEL_INSTANCE(className) \
}

//打印时间内容
void dskPhonePrintTime(const char * lpszFmt, ...);

/* 函数名  ：Timet2String()                                       */
/* 功能  ： time_t格式的时间转换成字符串,并且翻译（因此不放在逻辑层）,转换后的时间不包括年，
/* 参数说明 ：tTime [in] 是要装换的时间，strSep [in]是分隔符，
bFullWeekDay [in] 是否显示星期完整名，不显示完整名时显示缩写，如完整名为Monday，缩写为Mon,同时
如果是今天或昨天日期会转换为today或者yesterday，不显示完整名时不会显示today和yesterday
bShowMask [in] 分和秒之间是否要冒号*/
/* 返回值 ：普通的时间，字符串                 */
/************************************************************************/
yl::string Timet2String(time_t tTime, const yl::string & strSep = "", bool bFullWeekDay = true,
                        bool bShowMask = false, bool bShowComma = true);

// 将time_t类型的时间转换为字符串, 字符串格式由当前的日期和时间类型决定,转换后的时间包括年
// | tTime | 是整数型的时间.
// | strSep | 是日期和时间之间的分隔符, 默认为一个空格.
// 如果转换成功则返回转换后的字符串, 否则返回空.
yl::string Timet2DetailString(time_t tTime, const yl::string & strSep = " ", bool bShowComma = true);

// 时间转换成字符
// 转换成 hh:mm:ss或hh:mm ss或hh mm或hh:mm
yl::string ClockTime2String(yl::string & strHour, yl::string & strMin, yl::string & strSec);
// 日期转换成字符
yl::string Date2String();

// 获取日期格式类别
int GetDateFormatType();

bool RTL_GetRealString(const yl::string& strText, yl::string& strShowText);

#define T2X_SYSTEM_COLOR_BLACK xColor(0, 0, 0)
#define T2X_ROUND_FRAME_PIXEL 5

void PaintRoundFrame(xPainter & p, const chRect & rcFrame, const xColor & clrBG);

enum RESPATHTYPE
{
    RESPATHTYPE_SCREEN,
    RESPATHTYPE_LAYOUT,
    RESPATHTYPE_KEYMAP,
    RESPATHTYPE_PICS,
    RESPATHTYPE_FONT,
    RESPATHTYPE_MENU,
};

enum
{
    IMG_CLASSIFY_WALLPAPER = IMG_CLASSIFY_CUSTOM,
    IMG_CLASSIFY_USB_PHOTO,
    IMG_CLASSIFY_CUSTOM_PHOTO,
};

yl::string getRealPathByPhoneType(yl::string strRootPath, yl::string strFileName,
                                  RESPATHTYPE eType);

#endif
