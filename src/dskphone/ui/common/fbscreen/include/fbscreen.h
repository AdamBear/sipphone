#ifndef __FB_SCREEN_H__
#define __FB_SCREEN_H__

#include "xwindow/xGraphic.h"
#if IF_CUSTOM_SCREEN_DEVICE
#include "iscreendevice.h"
#endif

#ifndef _WIN32
#define FB_SCREEN_EXPORT __attribute((visibility("default")))
#else
#define FB_SCREEN_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if IF_CUSTOM_SCREEN_DEVICE
FB_SCREEN_EXPORT xScreen* CreateCustomScreen(IScreenDevice* pDevice);
#endif

FB_SCREEN_EXPORT xScreen* CreateScreen();
FB_SCREEN_EXPORT bool ReleaseScreen(xScreen* pScreen);

#ifdef __cplusplus
};
#endif

#endif
