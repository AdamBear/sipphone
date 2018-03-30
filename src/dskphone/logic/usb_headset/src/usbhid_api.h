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
*
*/
#ifndef __USBHID_API_H__
#define __USBHID_API_H__
/*
*-----------------------------------------------------------
*   INCLUDE FILES
*-----------------------------------------------------------
*/
#include <stdbool.h>
/*
* API export
*/
#ifdef WIN32
#    define LIBRARY_API __declspec (dllimport)
#else /*Linux*/
#  define LIBRARY_API
#endif

/*
*-----------------------------------------------------------
*   EXPORTED TYPES AND DEFINITIONS
*-----------------------------------------------------------
*/
#ifdef __cplusplus
extern "C"
{
#endif


// Predefined inputs enum
typedef enum usbhid_input
{
    Undefined = 0,
    OffHook,
    Mute,
    Flash,
    Redial,
    Hold,
    Line,
    Reject,
    SpeedDial,
    LedRinger,
    LineBusyTone,
    Key0,
    Key1,
    Key2,
    Key3,
    Key4,
    Key5,
    Key6,
    Key7,
    Key8,
    Key9,
    KeyStar,
    KeyPound,
    KeyVolumeUp,
    KeyVolumeDown,
    KeyMax
} usbhid_input_t;



typedef void (*usbhid_button_in_data_raw_f)(unsigned short usage_page, unsigned short usage,
        int buttonInData, bool relative);
typedef void (*usbhid_button_in_data_translated_f)(usbhid_input_t input, int buttonInData,
        bool relative);

typedef int usbhid_handle_t;
/*
*-----------------------------------------------------------
*   library APIs
*-----------------------------------------------------------
*/

LIBRARY_API usbhid_handle_t usb_hid_open(char * path,
        usbhid_button_in_data_raw_f raw_data_callback_f,
        usbhid_button_in_data_translated_f translated_data_callback_f);

LIBRARY_API void usb_hid_close(usbhid_handle_t handle);

/*
*-----------------------------------------------------------
*    APIs
*-----------------------------------------------------------
*/
LIBRARY_API int usb_hid_set_offhook(usbhid_handle_t handle, int on);
LIBRARY_API int usb_hid_set_led_ring(usbhid_handle_t handle, int on);
LIBRARY_API int usb_hid_set_tel_ringer(usbhid_handle_t handle, int on);
LIBRARY_API int usb_hid_set_mute(usbhid_handle_t handle, int on);
LIBRARY_API int usb_hid_set_micphonemute(usbhid_handle_t handle, int on);
LIBRARY_API int usb_hid_set_hold(usbhid_handle_t handle, int on);
LIBRARY_API int usb_hid_set_online(usbhid_handle_t handle, int on);

#ifdef __cplusplus
}
#endif

#endif /*__USBHID_API_H__*/
