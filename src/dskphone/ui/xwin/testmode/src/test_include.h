#ifndef __TEST_INCLUDE_H__
#define __TEST_INCLUDE_H__

#include "xwindow/xWindow.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#include "service_ipvp.h"
#include "commonunits/commonunits_def.h"
#include "keymap.h"

#define kszStatusReset "resetflag"

enum TEST_STATE
{
    TS_NONE = 0x00,
    TS_INIT,
    TS_LCD,
    TS_KEY,
    TS_AUTO_TEST_KEY,
    TS_ECHO,
    TS_RANDNUM,
    TS_EXIT,
    TS_EXP_LIGHT,
    TS_EXP_KEY,
    TS_EXP_LCD,
    TS_EXP_EXIT,
    TS_LED,
#if JOYCE_TEL_BUGFREE_19348
    TS_SN,
#endif
    TS_USB,
    TS_WIFI,
    TS_FINISH,
    TS_IP,
#if IS_COLOR
    TS_COLOR,
    TS_COLOR_LCD,
    TS_BLUETOOTH,
#endif
#ifdef _CP920
    TS_BACK_LIGHT,
#endif
};

enum LED_STATE
{
    LED_STATE_NONE = 0,
    LED_STATE_OFF,      // 全灭
    LED_STATE_GREEN,
    LED_STATE_RED,
    LED_STATE_FLASH,    // 交替亮
};

enum ECHO_STATE
{
    ES_NONE = 0,
    ES_LOOP,    // 连续回音测试
    ES_RECORD,      // 录音中
    ES_PLAY,        // 播放中
};

enum LCD_STEP
{
    LS_STEP1 = 0x00,
    LS_STEP2,
    LS_STEP3,
    LS_STEP4,
    LS_LAST
};

enum ECHO_MODE
{
    EM_SPEEKER = 0x00,
    EM_ECHO_START = 0x01,
    EM_EHS,
    EM_SPEAKER_RECORD,
    EM_SPEAKER_PLAY,
    EM_HEADSET,
    EM_HANDSET,
    EM_AUDIO_HANDFREE_MIC,
    EM_AUDIO_HANDFREE_SPEAKER,
    EM_LED,
    EM_ECHO_MIC,
    EM_ECHO_MUTE,           // mute功能测试
    EM_ECHO_LINEDEVICE,     // 3.5mm通道
};

enum RANDNUM_MODE
{
    RN_WHITESCREEN = 0x00,
    RN_NUM = 0x01,
};

enum AUDIO_MODE
{
    AD_HANDFREE_MIC = 0x00,
    AD_HANDFREE_SPEAKER = 0x01,
};

enum TEXT_TYPE
{
    TT_TITLE = 0,
    TT_CONTENT_NORMAL,
    TT_CONTENT_CENTER,
    TT_CONTENT_SECOND_HALF,
    TT_CONTENT_FULL_SCREEN,
    TT_MAX,
};

enum AREA_TYPE
{
    AT_ALL = 0,
    AT_FIRST_OF_FOUR,
    AT_SECOND_OF_FOUR,
    AT_THIRD_OF_FOUR,
    AT_FOURTH_OF_FOUR,
    AT_MAX
};

enum LIGHT_STEP
{
    LS_GREEN        = 0x00,
    LS_RED          = 0x01,
    LS_ORANGEONE    = 0x02,
    LS_ORANGETWO    = 0x03,
    LS_OFF          = 0x04,
};

#if IS_COLOR
enum LCD_COLOR_STEP
{
    LCD_RED         = 0x00,
    LCD_GREEN       = 0x01,
    LCD_BLUE        = 0x02,
    LCD_GRID        = 0x03,
    LCD_GRAY80      = 0x04,
    LCD_GRAY50      = 0x05,
    LCD_GRAY30      = 0x06,
    LCD_FLICK       = 0x07,
    LCD_GRADIENT    = 0x08,
    LCD_WHITESCREEN = 0x09,
    LCD_END         = 0x0a,
};

#define TESEMODE_RING_PATH      "ring.wav"
#define PIC_TESTMODE_GRID       "grid.png"
#define PIC_TESTMODE_GRAY80     "gray80.png"
#define PIC_TESTMODE_GRAY50     "gray50.png"
#define PIC_TESTMODE_GRAY30     "gray30.png"
#define PIC_TESTMODE_FLICK      "flick.bmp"
#define PIC_TESTMODE_GRADIENT   "gradient.png"

/* 声音路径 */
#define DEFAULT_RING_FILE       RESOURCE_PATH"/testmode/ring.wav"
#endif //IS_COLOR

#ifdef _EXP_COLOR_SUPPORT
#define PIC_TESTMODE_EXP        RESOURCE_PATH "testmode/exp/"
#define PIC_TESTMODE_EXPGRAY30  PIC_TESTMODE_EXP "Exp_gray30.png"
#define PIC_TESTMODE_EXPGRAY50  PIC_TESTMODE_EXP "Exp_gray50.png"
#define PIC_TESTMODE_EXPGRAY80  PIC_TESTMODE_EXP "Exp_gray80.png"
#define PIC_TESTMODE_EXPGRID    PIC_TESTMODE_EXP "Exp_grid.png"
#define PIC_TESTMODE_EXPFLICK   PIC_TESTMODE_EXP "Exp_flick.bmp"
#endif

#define  TEST_FONT_20                   20
#ifdef _CP920
#define  TEST_FONT_10                   9
#define  TEST_FONT_12                   16
#else
#define  TEST_FONT_10                   10
#if IS_COLOR
#define  TEST_FONT_12                   16
#define  TEST_FONT_24                   24
#define  TEST_FONT_30                   30
#define  TEST_FONT_50                   50
#else
#define  TEST_FONT_12                   10
#endif
#endif

#ifdef WIN32
#define IF_TEST_ALL_PHONE
#endif

#define IS_SUPPORT_USB(x)   (PT_T27S == x || PT_T43S == x || PT_CP860 == x || PT_T43P == x || PT_CP920 == x)

#include <ETLLib.hpp>
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "etlmsghandler/modetlmsghandler.h"

#include "devicelib/phonedevice.h"
#include "lamp/ledlamp/include/led_data_define.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "dsskey/src/dsskey_common.h"
#include "dsskey/include/moddsskey.h"
#include "extended/testmodemessagedefine.h"

#include "testphone.h"
#include "teststate.h"
#include "testexp.h"

#endif
