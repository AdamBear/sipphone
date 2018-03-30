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
#ifndef __USBAUDIO_API_H__
#define __USBAUDIO_API_H__
/*
*-----------------------------------------------------------
*   INCLUDE FILES
*-----------------------------------------------------------
*/

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

/*
*-----------------------------------------------------------
*   library APIs
*-----------------------------------------------------------
*/

LIBRARY_API int usb_audio_volume_initialize(int card);
LIBRARY_API int usb_audio_volume_uninitialize();

/*
*-----------------------------------------------------------
*    APIs
*-----------------------------------------------------------
*/

LIBRARY_API int usb_audio_volume_get_range(int * min, int * max);
LIBRARY_API int usb_audio_volume_get(int * volume);
LIBRARY_API int usb_audio_volume_set(int volume);
LIBRARY_API int usb_audio_volume_mute(int mute);
LIBRARY_API int usb_audio_volume_inc();
LIBRARY_API int usb_audio_volume_dec();

#ifdef __cplusplus
};
#endif


#endif /*__USB_AUDIO_API_H__*/
