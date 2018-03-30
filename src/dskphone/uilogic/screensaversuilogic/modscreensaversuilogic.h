#if IF_FEATURE_XMLB_SCREENSAVER || IS_COLOR
#ifndef _MOD_SCREENSAVERS_UILOGIC_H_
#define _MOD_SCREENSAVERS_UILOGIC_H_

//初始化
void UIlogic_ScreenSaversInit();

void UIlogic_ScreenSaversUninit();

//进入屏保
void UIlogic_EnterScreenSavers();

//是否显示屏保
bool UIlogic_ScreenSaversIsShow();

//退出屏保
void UIlogic_ExitScreensaver();

//通话告知通话状态变化
void UILogic_ScreenSaversWakeUpByTalkCountChange(/*bool bFirstIntalk, bool bAllTalkExit*/);

//设置是否可以进入屏保
void UILogic_SetEnterScreenSaver(bool bNewAllow);

//按键事件
bool UIlogic_ScreenSaversProcessKeyEvent(int nKeyCode, int bPress);

//触摸事件
bool UIlogic_ScreenSaversProcessMouseEvent();

#endif // _MOD_SCREENSAVERS_H_

#endif
