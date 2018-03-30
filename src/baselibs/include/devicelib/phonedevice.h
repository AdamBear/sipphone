/**********************************************************************************************/ /**
 * @file    phonedevice.h
 *
 * @brief   Declares the phonedevice class.
 **************************************************************************************************/
#ifndef __DEVICE_LIB_PHONE_DEVICE_H_
#define __DEVICE_LIB_PHONE_DEVICE_H_

#include <ETLLib/ETLLib.hpp>

#ifdef _MSC_VER

#ifndef EXPORT_API
#ifdef BASELIB_BUILD
#define EXPORT_API __declspec (dllexport)
#else
#define EXPORT_API __declspec (dllimport)
#endif
#endif

#endif

#ifndef EXPORT_API
#define EXPORT_API
#endif

extern EXPORT_API BYTE g_iPhoneType;

/**********************************************************************************************//**
 * @def DEVICELIB_IS_T42(x)
 *
 * @brief   A macro that defines devicelib is t 42.
 *
 * @param   x   The void to process.
 **************************************************************************************************/

#define DEVICELIB_IS_T42(x) (x == PT_T41 || x == PT_T42 || x == PT_T43P || x == PT_T43S)

/**********************************************************************************************//**
 * @def DEVICELIB_IS_T27(x)
 *
 * @brief   A macro that defines device is t 27.
 *
 * @param   x   The void to process.
 **************************************************************************************************/

#define DEVICELIB_IS_T27(x) (x == PT_T27 || x == PT_T27S || x == PT_T66)

/**********************************************************************************************//**
 * @def DEVICELIB_IS_T46(x)
 *
 * @brief   A macro that defines device is t 46.
 *
 * @param   x   The void to process.
 **************************************************************************************************/

#define DEVICELIB_IS_T46(x) (x == PT_T46S || x == PT_T46)

/**********************************************************************************************//**
 * @def DEVICELIB_IS_T48(x)
 *
 * @brief   A macro that defines device is t 48.
 *
 * @param   x   The void to process.
 **************************************************************************************************/

#define DEVICELIB_IS_T48(x) (x == PT_T48 || x == PT_T48S)

/**********************************************************************************************//**
 * @def DEVICELIB_IS_T69(x)
 *
 * @brief   A macro that defines device is t 69.
 *
 * @param   x   The void to process.
 **************************************************************************************************/

#define DEVICELIB_IS_T69(x) (x == PT_T69 || x == PT_T69S)

/**********************************************************************************************//**
 * @def DEVICELIB_IS_T23(x)
 *
 * @brief   A macro that defines device is t 23.
 *
 * @param   x   The void to process.
 **************************************************************************************************/

#define DEVICELIB_IS_T23(x) (x == PT_T23 || x == PT_T21 || x == PT_T40 || x == PT_T40G)

/**********************************************************************************************//**
 * @def DEVICELIB_IS_T40(x)
 *
 * @brief   A macro that defines device is t 40.
 *
 * @param   x   The void to process.
 **************************************************************************************************/

#define DEVICELIB_IS_T40(x) (x == PT_T40 || x == PT_T40G)

/**********************************************************************************************//**
 * @def DEVICELIB_IS_CP(x)
 *
 * @brief   A macro that defines device is cp phone.
 *
 * @param   x   The void to process.
 **************************************************************************************************/

#define DEVICELIB_IS_CP(x) (x == PT_CP920 || x == PT_CP860)


/**********************************************************************************************//**
 * @struct  lcd_icon
 *
 * @brief   A LCD icon.T42 绘制顶部通道图标.
 **************************************************************************************************/

struct lcd_icon
{
    int icon;
    int enable;
};

struct ScreenDC
{
    int iDeviceHandle; // 设备句柄
    int iWidth; // 话机液晶屏的宽
    int iHeight;       // 话机液晶屏的高
    int iBitsPerPixel; // 多少bit一个像素点
    char * pBuffer;    // 屏幕缓冲
};

// 严禁直接使用枚举值, 此处暂时为网页保留
// html 中直接使用了枚举值
enum PHONE_TYPE_DEF
{
    PT_UNKNOWN = 0,
    PT_T19 = 19,
    PT_T21 = 21,
    PT_T23 = 23,
    PT_T27 = 27,
    PT_T29 = 29,
    PT_CP860 = 37,
    PT_T40 = 40,
    PT_T41 = 41,
    PT_T42 = 42,
    PT_T46 = 46,
    PT_T48 = 48,
    PT_T49 = 49,
    PT_DECT = 56,
    PT_T61 = 61,
    PT_T66 = 66,
    PT_T68 = 68,
    PT_T69 = 69,
    PT_T27S = 70,
    PT_T43S = 71, //T42S
    PT_T46S = 72,
    PT_T69S = 73,
    PT_T48S = 74,
    PT_T43P = 75, // T41S
    PT_T54S = 76,
    PT_CP960 = 77,
    PT_T52 = 78,
    PT_T40G = 79,
    PT_W600 = 80,
    PT_CP920 = 81,
};

enum DEV_SERIES_TYPE
{
    SERIES_UNKNOWN = 0,
};

/**********************************************************************************************//**
 * @fn  const char *devicelib_GetDevSeriesName();
 *
 * @brief   获取设备的系列名称.
 *
 * @return  Null if it fails, else a pointer to a const char.
 **************************************************************************************************/

const char * devicelib_GetDevSeriesName();

/**********************************************************************************************//**
 * @fn  yl::string devicelib_GetDevCusSeriesName(const char *szDiffReason);
 *
 * @brief   获取设备的客制化系列名称.
 *
 * @param   szDiffReason    The diff reason.
 *
 * @return  A yl::string.
 **************************************************************************************************/

yl::string devicelib_GetDevCusSeriesName(const char * szDiffReason);

/**********************************************************************************************//**
 * @fn  DEV_SERIES_TYPE devicelib_GetDevSeriesType();
 *
 * @brief   获取设备的系列类型.
 *
 * @return  A DEV_SERIES_TYPE.
 **************************************************************************************************/

DEV_SERIES_TYPE devicelib_GetDevSeriesType();

/**********************************************************************************************//**
 * @fn  void devicelib_Init();
 *
 * @brief   Devicelib init.
 **************************************************************************************************/

void devicelib_Init();

/**********************************************************************************************//**
 * @fn  void devicelib_InitLcd();
 *
 * @brief   devicelib Lcd init. open /dev/fb0
 **************************************************************************************************/

void devicelib_InitLcd();

/**********************************************************************************************//**
 * @fn  void devicelib_InitExtLed();
 *
 * @brief   devicelib ext led init. for cp860
 **************************************************************************************************/

void devicelib_InitExtLed();

/**********************************************************************************************//**
 * @fn  void devicelib_UnInit();
 *
 * @brief   Devicelib un init.
 **************************************************************************************************/

void devicelib_UnInit();

/**********************************************************************************************//**
 * @fn  void devicelib_UnInit();
 *
 * @brief   Devicelib un init.
 **************************************************************************************************/

void devicelib_UnInitLcd();

/**********************************************************************************************//**
 * @fn  void devicelib_UnInit();
 *
 * @brief   Devicelib un init.
 **************************************************************************************************/

void devicelib_UnInitExtLed();

/**********************************************************************************************//**
 * @fn  const char *devicelib_GetHardwareVersion();
 *
 * @brief   Devicelib get hardware version.
 *
 * @return  Null if it fails, else a pointer to a const char.
 **************************************************************************************************/

const char * devicelib_GetHardwareVersion();

/**********************************************************************************************//**
 * @fn  const char *devicelib_GetHardwareName();
 *
 * @brief   Devicelib get hardware name.
 *
 * @return  Null if it fails, else a pointer to a const char.
 **************************************************************************************************/

const char * devicelib_GetHardwareName();

/**********************************************************************************************//**
 * @fn  const char *devicelib_GetPhoneName();
 *
 * @brief   获取话机设备名称，用于出厂配置合并一个版本使用，谨慎修改。.
 *
 * @return  Null if it fails, else a pointer to a const char.
 **************************************************************************************************/

const char * devicelib_GetPhoneName();

/**********************************************************************************************//**
 * @fn  PHONE_TYPE_DEF devicelib_GetPhoneType();
 *
 * @brief   获取机器标识.
 *
 * @return  A PHONE_TYPE_DEF.
 **************************************************************************************************/

PHONE_TYPE_DEF devicelib_GetPhoneType();

/**********************************************************************************************//**
 * @fn  void devicelib_RefreshScreen(ScreenDC *hDc);
 *
 * @brief   Devicelib refresh screen.
 *
 * @param [in,out]  hDc If non-null, the device-context.
 **************************************************************************************************/

void devicelib_RefreshScreen(ScreenDC * hDc);

/**********************************************************************************************//**
 * @fn  void devicelib_SetLedStatus(unsigned int uStatus);
 *
 * @brief   设置灯的状态.
 *
 * @param   uStatus The status.
 **************************************************************************************************/

void devicelib_SetLedStatus(unsigned int uStatus);

/**********************************************************************************************//**
 * @fn  void devicelib_SetBackLightLevel(int nLevel);
 *
 * @brief   设置液晶背光的亮度.
 *
 * @param   nLevel  The level.
 **************************************************************************************************/

void devicelib_SetBackLightLevel(int nLevel);

/**********************************************************************************************//**
 * @fn  void devicelib_SetWatchDog(int nSeconds);
 *
 * @brief   看门狗操作函数.
 *
 * @param   nSeconds    The seconds.
 **************************************************************************************************/

void devicelib_SetWatchDog(int nSeconds);

/**********************************************************************************************//**
 * @fn  void devicelib_CloseWatchDog();
 *
 * @brief   Devicelib close watch dog.
 **************************************************************************************************/

void devicelib_CloseWatchDog();

/**********************************************************************************************//**
 * @fn  void devicelib_FeedWatchDog(int iIntervalSeconds = 0);
 *
 * @brief   Devicelib feed watch dog.
 *
 * @param   iIntervalSeconds    (Optional) The interval in seconds.
 **************************************************************************************************/

void devicelib_FeedWatchDog(int iIntervalSeconds = 0);

/**********************************************************************************************//**
 * @fn  void devicelib_startWatchDog();
 *
 * @brief   Devicelib start watch dog.
 **************************************************************************************************/

void devicelib_startWatchDog();

/**********************************************************************************************//**
 * @fn  void devicelib_RebootByWatchDog();
 *
 * @brief   系统重启.
 **************************************************************************************************/

void devicelib_RebootByWatchDog();

/**********************************************************************************************//**
 * @fn  bool devicelib_BeLcdW132H64();
 *
 * @brief   fix me.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

bool devicelib_BeLcdW132H64();

/**********************************************************************************************//**
 * @fn  ScreenDC *devicelib_GetScreenDC();
 *
 * @brief   Devicelib get screen device-context.
 *
 * @return  Null if it fails, else a pointer to a ScreenDC.
 **************************************************************************************************/

ScreenDC * devicelib_GetScreenDC();

/**********************************************************************************************//**
 * @fn  void devicelib_SetLcdContrast(int contrastLevel);
 *
 * @brief   Devicelib set LCD contrast.
 *
 * @param   contrastLevel   The contrast level.
 **************************************************************************************************/

void devicelib_SetLcdContrast(int contrastLevel);

/**********************************************************************************************//**
 * @fn  int devicelib_SetIcon(int icon, int enable);
 *
 * @brief   T42 绘制顶部通道图标.
 *
 * @param   icon    The icon.
 * @param   enable  The enable.
 *
 * @return  An int.
 **************************************************************************************************/

int devicelib_SetIcon(int icon, int enable);

/**********************************************************************************************//**
 * @fn  void devicelib_SetExtLed(int iLedStatus);
 *
 * @brief   设置扩展口的灯   ，cp860 外接麦克风的灯控制
 *
 * @param   iLedStatus
 **************************************************************************************************/

void devicelib_SetExtLed(int iLedStatus);
#endif //!__DEVICE_LIB_PHONE_DEVICE_H_
