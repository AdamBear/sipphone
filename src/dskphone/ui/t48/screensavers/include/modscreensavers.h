#ifndef _MOD_SCREENSAVERS_H_
#define _MOD_SCREENSAVERS_H_

void ScreenSavers_Init();

void ScreenSavers_Enter();

void ScreenSavers_ExitScreensaver();

#if IF_FEATURE_XMLB_SCREENSAVER
//xmlbrowser方式屏保入口
bool ScreenSavers_XMBEnter(void * pData);
#endif //#if IF_FEATURE_XMLB_SCREENSAVER

bool ScreenSavers_IsExist();

#endif // _MOD_SCREENSAVERS_H_
