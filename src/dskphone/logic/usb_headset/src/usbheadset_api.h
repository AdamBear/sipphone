/* ******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2016-06-26
*
*
* for export APIs header file
*
*    history:
*------------------------------------------------------------------
* 55.0.0.1: 初始版本
* 55.0.0.2: 增加调试log, 增加版本信息
* 55.0.0.3: 解决usb_audio_volume_get音量获取不对问题
* 55.0.0.4：按键回调接口增加relative标志，告知上层按键是通过relative上报，还是absolute上报
*------------------------------------------------------------------
*
*/

#ifndef __USBHEADSET_API_H__
#define __USBHEADSET_API_H__

/*
* API export
*/
#ifdef WIN32
#    define LIBRARY_API __declspec (dllimport)
#else /*Linux*/
#  define LIBRARY_API
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct usbheadset_device_info
{
    /** Platform-specific device path */
    char * path;
    /** Device Vendor ID */
    unsigned short vendor_id;
    /** Device Product ID */
    unsigned short product_id;
    /**  Device Version Number */
    unsigned short version;

    /** Manufacturer String */
    char * manufacturer_string;
    /** Product string */
    char * product_string;
    /** Serial Number */
    char * serial_number;

    /**support hid control? */
    int support_hid;

    /**usb audio card num*/
    int card;
} usbheadset_device_info_t;

#define  USBHEADSET_LOG_INV           -1
#define  USBHEADSET_LOG_ALL           0
#define  USBHEADSET_LOG_LEVEL_EMERG   0        /* Emergency: system is unusable */
#define  USBHEADSET_LOG_LEVEL_ALERT   1        /* Alert: action must be taken immediately */
#define  USBHEADSET_LOG_LEVEL_CRIT    2        /* Critical: critical conditions */
#define  USBHEADSET_LOG_LEVEL_ERR     3        /* Error: error conditions */
#define  USBHEADSET_LOG_LEVEL_WARN    4        /* Warning: warning conditions */
#define  USBHEADSET_LOG_LEVEL_NOTICE  5        /* Notice: normal but significant condition */
#define  USBHEADSET_LOG_LEVEL_INFO    6        /* Informational: informational messages */
#define  USBHEADSET_LOG_LEVEL_DEBUG   7        /* Debug: debug-level messages */
#define  USBHEADSET_LOG_LEVEL_SPY     8        /* reserved */
#define  USBHEADSET_LOG_LEVEL_ANY     9        /* reserved */

typedef void (*usbheadset_hotplug_callback_f)(usbheadset_device_info_t *,  void *);

/*
*-----------------------------------------------------------
*   version APIs
*-----------------------------------------------------------
*/
/*for version*/
LIBRARY_API const char * usb_headset_version(void);
LIBRARY_API const char * usb_headset_build(void);

/*
*-----------------------------------------------------------
*   APIs
*-----------------------------------------------------------
*/
LIBRARY_API int usb_headset_initialize(usbheadset_hotplug_callback_f usbheadset_added,
                                       usbheadset_hotplug_callback_f usbheadset_removed, void * user_data);

LIBRARY_API int usb_headset_uninitialize();

LIBRARY_API int usb_headset_log_set(int log_level);


#include "usbaudio_api.h"
#include "usbhid_api.h"

#ifdef __cplusplus
}
#endif
/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif/*__USBHEADSET_API_H__*/
