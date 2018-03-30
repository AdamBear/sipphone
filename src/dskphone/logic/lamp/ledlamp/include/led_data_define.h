#ifndef __LED_DATA_DEFINE_H__
#define __LED_DATA_DEFINE_H__

//按键类型
enum LEDKEY_TYPE
{
    LT_LINE,
    LT_MEMORY,
    LT_EXP,
    LT_SMS,
    LT_HEADSET,
    LT_BACKLIGHT,
    LT_HANDFREE,
    LT_POWER,
};

//指示灯颜色
enum LEDCOLOR_LTYE
{
    LT_RED,
    LT_GREEN,
    LT_REDGREEN,
};

//指示灯状态
enum LEDSTATUS_TYPE
{
    LST_NONE,
    LST_OFF,
    LST_ON,
    LST_SLOWFLASH,
    LST_FASTFLASH,
};

enum LIGHT_INDEX
{
    LIGHT_LINE1 = 0x01 << 3,
    LIGHT_LINE2 = 0x01,
    LIGHT_LINE3 = 0x01 << 12,
    LIGHT_LINE4 = 0x01 << 1,
    LIGHT_LINE5 = 0x01 << 13,
    LIGHT_LINE6 = 0x01 << 2,
    LIGHT_LINE7 = 0x00,         //虚拟灯
    LIGHT_MEMO1 = 0x01 << 10,
    LIGHT_MEMO2 = 0x01 << 9,
    LIGHT_MEMO3 = 0x01 << 8,
    LIGHT_MEMO4 = 0x01 << 18,
    LIGHT_MEMO5 = 0x01 << 17,
    LIGHT_MEMO6 = 0x01 << 16,
    LIGHT_MEMO7 = 0x01 << 6,
    LIGHT_MEMO8 = 0x01 << 5,
    LIGHT_MEMO9 = 0x01 << 4,
    LIGHT_MEMO10 = 0x01 << 15,
};

enum T19_LIGHT_INDEX
{
    T19_LIGHT_BACKLIGHT = 0x01 << 5,
    T19_LIGHT_POWER = 0x01,
};

enum T23_LIGHT_INDEX
{
    T23_LIGHT_MSG        = 0x01 << 7,       //MESSAGE
    T23_LIGHT_LK2_RED    = 0x01 << 6,       //LK2 RED
    T23_LIGHT_LK2_GREEN  = 0x01 << 5,       //LK2 GREEN
    T23_LIGHT_LK3_RED    = 0x01 << 4,       //LK3 RED
    T23_LIGHT_LK3_GREEN  = 0x01 << 3,       //LK3 GREEN
    T23_LIGHT_LK1_RED    = 0x01 << 2,       //LK1  RED
    T23_LIGHT_LK1_GREEN  = 0x01 << 1,       //LK1 GREEN
    T23_LIGHT_POWER      = 0x01 << 0,       //POWER_LIGHT
};

enum T66_LIGHT_INDEX
{
    T66_LIGHT_LK6_RED      = 0x01 << 20,      //LK6 RED
    T66_LIGHT_LK7_RED      = 0x01 << 19,      //LK7 RED
    T66_LIGHT_LK5_RED      = 0x01 << 18,      //LK5 RED
    T66_LIGHT_LK8_RED      = 0x01 << 17,      //LK8 RED
    T66_LIGHT_LK6_GREEN    = 0x01 << 16,      //LK6 GREEN
    T66_LIGHT_LK7_GREEN    = 0x01 << 15,      //LK7 GREEN
    T66_LIGHT_LK5_GREEN    = 0x01 << 14,      //LK5 GREEN
    T66_LIGHT_LK8_GREEN    = 0x01 << 13,      //LK8 GREEN
    T66_LIGHT_LK2_RED      = 0x01 << 12,      //LK2 RED
    T66_LIGHT_LK3_RED      = 0x01 << 11,      //LK3 RED
    T66_LIGHT_LK1_RED      = 0x01 << 10,      //LK1 RED
    T66_LIGHT_LK4_RED      = 0x01 << 9,       //LK4 RED
    T66_LIGHT_LK2_GREEN    = 0x01 << 8,       //LK2 GREEN
    T66_LIGHT_LK3_GREEN    = 0x01 << 7,       //LK3 GREEN
    T66_LIGHT_LK1_GREEN    = 0x01 << 6,       //LK1 GREEN
    T66_LIGHT_LK4_GREEN    = 0x01 << 5,       //LK4 GREEN
    T66_LIGHT_SPK          = 0x01 << 4,       //SPK_LIGHT
    T66_LIGHT_MUTE         = 0x01 << 3,       //MUTE_LIGHT
    T66_LIGHT_HEADSET      = 0x01 << 2,       //HEADSET_LIGHT
    T66_LIGHT_MSG          = 0x01 << 1,       //MSG_LIGHT
    T66_LIGHT_POWER        = 0x01,            //POWER_LIGHT

};

//broadcom T27灯号
enum T27_LIGHT_INDEX
{
    T27_LIGHT_LK8_RED      = 0x01 << 22,      //LK8 RED
    T27_LIGHT_LK7_RED      = 0x01 << 21,      //LK7 RED
    T27_LIGHT_LK6_RED      = 0x01 << 20,      //LK6 RED
    T27_LIGHT_LK5_RED      = 0x01 << 19,      //LK5 RED


    T27_LIGHT_LK8_GREEN    = 0x01 << 16,      //LK8 GREEN
    T27_LIGHT_LK7_GREEN    = 0x01 << 15,      //LK7 GREEN
    T27_LIGHT_LK6_GREEN    = 0x01 << 14,      //LK6 GREEN
    T27_LIGHT_LK5_GREEN    = 0x01 << 13,      //LK5 GREEN

    T27_LIGHT_MSG          = 0x01 << 11,      //MSG_LIGHT
    T27_LIGHT_LK4_RED      = 0x01 << 10,      //LK4 RED
    T27_LIGHT_LK3_RED      = 0x01 << 9,       //LK3 RED
    T27_LIGHT_LK2_RED      = 0x01 << 8,       //LK2 RED
    T27_LIGHT_LK1_RED      = 0x01 << 7,      //LK1 RED

    T27_LIGHT_HEADSET      = 0x01 << 5,       //HEADSET_LIGHT
    T27_LIGHT_LK4_GREEN    = 0x01 << 4,       //LK4 GREEN
    T27_LIGHT_LK3_GREEN    = 0x01 << 3,       //LK3 GREEN
    T27_LIGHT_LK2_GREEN    = 0x01 << 2,       //LK2 GREEN
    T27_LIGHT_LK1_GREEN    = 0x01 << 1,       //LK1 GREEN
    T27_LIGHT_POWER        = 0x01,            //POWER_LIGHT
};


enum T61_LIGHT_INDEX
{
    T61_LIGHT_LK1_RED      = 0x01 << 14,      //MK1_RED
    T61_LIGHT_LK1_GREEN    = 0x01 << 13,      //MK1 GREEN
    T61_LIGHT_LK2_RED      = 0x01 << 12,      //MK2 RED
    T61_LIGHT_LK2_GREEN    = 0x01 << 11,      //MK2_GREEN
    T61_LIGHT_LK3_GREEN    = 0x01 << 10,      //MK3_GREEN
    T61_LIGHT_LK3_RED      = 0x01 << 9,       //MK3 RED
    T61_LIGHT_LK4_RED      = 0x01 << 8,       //MK4_RED
    T61_LIGHT_LK4_GREEN    = 0x01 << 7,       //MK4 GREEN
    T61_LIGHT_LK5_RED      = 0x01 << 6,       //MK5_RED
    T61_LIGHT_LK5_GREEN    = 0x01 << 5,       //MK5 GREEN
    T61_LIGHT_LK6_GREEN    = 0x01 << 4,       //MK6_GREENLIGHT
    T61_LIGHT_LK6_RED      = 0x01 << 3,       //MK6_RED_LIGHT
    T61_LIGHT_HEADSET      = 0x01 << 2,       //HEADSET_LIGHT
    T61_LIGHT_MSG          = 0x01 << 1,       //MSG_LIGHT
    T61_LIGHT_POWER        = 0x01,            //POWER_LIGHT
};

enum T42_LIGHT_INDEX
{
    T42_LIGHT_LK6_RED   = 0x01  << 21,          //LK6GREEN
    T42_LIGHT_LK4_RED   = 0x01  << 20,          //LK4GREEN
    T42_LIGHT_LK2_RED   = 0x01  << 19,          //LK2GREEN
    T42_LIGHT_SPK       = 0x01  << 16,          //SPK
    T42_LIGHT_LK6_GREEN = 0x01  << 15,          //LK6RED
    T42_LIGHT_LK4_GREEN = 0x01  << 14,          //LK4RED
    T42_LIGHT_LK2_GREEN = 0x01  << 13,          //LK2RED
    T42_LIGHT_HEADSET   = 0x01  << 10,          //HEADSET
    T42_LIGHT_LK5_RED   = 0x01  << 9,           //LK5GREEN
    T42_LIGHT_LK3_RED   = 0x01  << 8,           //LK3GREEN
    T42_LIGHT_LK1_RED   = 0x01  << 7,           //LK1GREEN
    T42_LIGHT_MUTE      = 0x01  << 4,           //MUTE 逻辑上对换Mute和HeadSet按键
    T42_LIGHT_LK5_GREEN = 0x01  << 3,           //LK5RED
    T42_LIGHT_LK3_GREEN = 0x01  << 2,           //LK3RED
    T42_LIGHT_LK1_GREEN = 0x01  << 1,           //LK1RED
    T42_LIGHT_POWER     = 0x01  << 0,           //POWER_LIGHT
};

enum T40_LIGHT_INDEX
{
    T40_LIGHT_LK3_RED    = 0x01 << 3,       //LK3 RED
    T40_LIGHT_LK3_GREEN  = 0x01 << 2,       //LK3 GREEN
    T40_LIGHT_LK2_RED    = 0x01 << 4,       //LK2 RED
    T40_LIGHT_LK2_GREEN  = 0x01 << 1,       //LK2 GREEN
    T40_LIGHT_LK1_RED    = 0x01 << 5,       //LK1 RED
    T40_LIGHT_LK1_GREEN  = 0x01 << 6,       //LK1 GREEN
    T40_LIGHT_POWER      = 0x01 << 0,       //POWER_LIGHT
};

enum T5XS_LIGHT_INDEX
{
    T5XS_LIGHT_LK10_GREEN   = 0x01 << 17,      //LK10 GREEN
    T5XS_LIGHT_LK9_GREEN    = 0x01 << 16,      //LK9 GREEN
    T5XS_LIGHT_LK8_GREEN    = 0x01 << 15,      //LK8 GREEN
    T5XS_LIGHT_LK7_GREEN    = 0x01 << 14,      //LK7 GREEN
    T5XS_LIGHT_LK6_GREEN    = 0x01 << 13,      //LK6 GREEN
    T5XS_LIGHT_HEADSET      = 0x01 << 18,      // HEADSET
    T5XS_LIGHT_LK10_RED     = 0x01 << 23,      //LK10 RED
    T5XS_LIGHT_LK9_RED      = 0x01 << 22,      //LK9 RED
    T5XS_LIGHT_LK8_RED      = 0x01 << 21,      //LK8 RED
    T5XS_LIGHT_LK7_RED      = 0x01 << 20,      //LK7 RED
    T5XS_LIGHT_LK6_RED      = 0x01 << 19,      //LK6 RED
    T5XS_LIGHT_SPK          = 0x01 << 12,      //SPK
    T5XS_LIGHT_LK5_GREEN    = 0x01 << 5,      //LK5 GREEN
    T5XS_LIGHT_LK4_GREEN    = 0x01 << 4,      //LK4 GREEN
    T5XS_LIGHT_LK3_GREEN    = 0x01 << 3,       //LK3 GREEN
    T5XS_LIGHT_LK2_GREEN    = 0x01 << 2,       //LK2 GREEN
    T5XS_LIGHT_LK1_GREEN    = 0x01 << 1,       //LK1 GREEN
    T5XS_LIGHT_MUTE         = 0x01 << 6,       //MUTE
    T5XS_LIGHT_LK5_RED      = 0x01 << 11,       //LK5 RED
    T5XS_LIGHT_LK4_RED      = 0x01 << 10,       //LK4 RED
    T5XS_LIGHT_LK3_RED      = 0x01 << 9,       //LK3 RED
    T5XS_LIGHT_LK2_RED      = 0x01 << 8,       //LK2 RED
    T5XS_LIGHT_LK1_RED      = 0x01 << 7,       //LK1 RED
    T5XS_LIGHT_POWER        = 0x01 << 0,       //POWER_LIGHT
};

enum T46_LIGHT_INDEX
{
    T46_LIGHT_HEADSET       = 0x01 << 6,      //HEADSET
    T46_LIGHT_MUTE          = 0x01 << 12,    //MUTE
    T46_LIGHT_SPK           = 0x01 << 18,     // SPK
    T46_LIGHT_LK1_GREEN     = 0x01 << 1,      //LK1 GREEN
    T46_LIGHT_LK2_GREEN     = 0x01 << 2,      //LK2 GREEN
    T46_LIGHT_LK3_GREEN     = 0x01 << 3,      //LK3 GREEN
    T46_LIGHT_LK4_GREEN     = 0x01 << 4,      //LK4 GREEN
    T46_LIGHT_LK5_GREEN     = 0x01 << 5,      //LK5 GREEN
    T46_LIGHT_LK6_GREEN     = 0x01 << 13,     //LK6 GREEN
    T46_LIGHT_LK7_GREEN     = 0x01 << 14,     //LK7 GREEN
    T46_LIGHT_LK8_GREEN     = 0x01 << 15,     //LK8 GREEN
    T46_LIGHT_LK9_GREEN     = 0x01 << 16,     //LK9 GREEN
    T46_LIGHT_LK10_GREEN    = 0x01 << 17,     //LK10 GREEN
    T46_LIGHT_LK1_RED       = 0x01 << 7,      //LK1 RED
    T46_LIGHT_LK2_RED       = 0x01 << 8,      //LK2 RED
    T46_LIGHT_LK3_RED       = 0x01 << 9,      //LK3 RED
    T46_LIGHT_LK4_RED       = 0x01 << 10,     //LK4 RED
    T46_LIGHT_LK5_RED       = 0x01 << 11,     //LK5 RED
    T46_LIGHT_LK6_RED       = 0x01 << 19,     //LK6 RED
    T46_LIGHT_LK7_RED       = 0x01 << 20,     //LK7 RED
    T46_LIGHT_LK8_RED       = 0x01 << 21,     //LK8 RED
    T46_LIGHT_LK9_RED       = 0x01 << 22,     //LK9 RED
    T46_LIGHT_LK10_RED      = 0x01 << 23,     //LK10 RED
    T46_LIGHT_POWER         = 0x01 << 1,      //POWER_LIGHT
};

#endif //__LED_DATA_DEFINE_H__

