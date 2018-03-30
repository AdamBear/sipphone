/************************************************************************
 * FileName  : TestMode_common.h (header file)
 * Purpose   :
 * Date      : 2012/11/13 14:39:15
 ************************************************************************/

#ifndef _TESTMODE_COMMON_H_
#define _TESTMODE_COMMON_H_

enum T48_LIGHT_INDEX
{
    T48_LIGHT_SPK   = 0x01 << 3,       //HANDFREE
    T48_LIGHT_MUTE       = 0x01 << 2,       //MUTE
    T48_LIGHT_HEADSET    = 0x01 << 1,       //MESSAGE
    T48_LIGHT_POWER      = 0x01 << 0,       //POWER_LIGHT
};
enum TEST_STATE
{
    TS_NONE = 0x00,
    TS_INIT,
    TS_LCD,
    TS_RANDNUM,//彩屏添加随机数
    TS_KEY,
    TS_EXP_KEY,
    TS_USB,
    TS_LED,
    TS_ECHO,
    TS_EXIT,
    TS_EXP_LIGHT,
    TS_EXP_REAL_KEY,
    TS_EXP_LCD,
    TS_EXP_EXIT,
    TS_EXP_ABORT,
    TS_EASY_TOUCH,


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
//  EM_SPEEKER = 0x00,
    EM_ECHO_START = 0x00,
    EM_SPEAKER_RECORD,
    EM_SPEAKER_PLAY,
    EM_HEADSET,
    EM_HANDSET,
};

enum LIGHT_STEP
{
    LS_GREEN = 0x00,
    LS_RED = 0x01,
    LS_ORANGE = 0x02
};

enum LIGHT_CROSS_STEP
{
    LIGHT_CROSS_STEP_1 = 0x0,
    LIGHT_CROSS_STEP_2,
};

enum LCD_COLOR_STEP
{
    LCD_WHITE = 0x00,
    LCD_BLACK = 0x01,
    LCD_RED = 0x02,
    LCD_GREEN = 0x03,
    LCD_BLUE = 0x04,
    LCD_GRID = 0x05,
    LCD_GRAY80 = 0x06,
    LCD_GRAY50 = 0x07,
    LCD_GRAY30 = 0x08,
    LCD_GRADIENT = 0x09,
    LCD_WHITESCREEN = 0x0a,
    LCD_END = 0x0b
};

enum USB_STATE
{
    USB_NONE = 0x00,
    USB_INSERT = 0x01,
    USB_REMOVE = 0x02,
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
//#define EXP_INSERT        0x00ff0000
//#define EXP_REMOVE        0x00ff0ff0

// EXP 实际按键数
#define DEV_EXP_KEY_NUM_40      20
#define DEV_EXP_KEY_NUM_39      20
#define DEV_EXP_KEY_NUM_38      38
//#define EXPIndex(i)      ((i >> 16) & 0x00ff)
//#define EXPKeyIndex(i)   ((i) & 0x0000ffff)

//#define EXPKeyCol(i)   ((i) & 0x000000ff)
//#define EXPKeyRow(i)   ((i >> 8) & 0x0000ff)

#define MAX_EXP_NUM     6
// 获取Exp按键对应的EXP类型
#define GetEXPModuleType(i) (((i) >> 24))
//获取Exp38对应的index  (bit0~bit5表示第几个按键，bit6~bit8表示第几个扩展台)
#define GetEXPIndex(i)      (((i) >> 6) & 0xf)
//获取Exp38Key对应的key index
#define GetEXPKeyIndex(i)   ((i) & 0x003f)

#endif
