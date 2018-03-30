#ifndef MOD_ZERO_TOUCH_UI_H
#define MOD_ZERO_TOUCH_UI_H

#include <ylstring.h>

void ZeroTouchUI_Init();

/************************************************************************/
/* 接口  ： ZeroSPTouch_CreateDlg                                       */
/* 功能  ： 打开ZeroSPTouch的界面                                       */
/* 参数说明 ：strPageAction 类型：string                                */
/*      strPageAction 需要打开界面的Action                               */
/* 返回值 ： 失败--false 成功--true                                     */
/************************************************************************/
bool ZeroTouchUI_EnterPage(const yl::string & strPageAction);

/************************************************************************/
/* 接口  ： ZeroSPTouch_CreateDlg                                       */
/* 功能  ： 销毁打开的ZeroSPTouch界面                                       */
/* 返回值 ： 失败--false 成功--true                                     */
/************************************************************************/
bool ZeroTouchUI_Destroy();

/************************************************************************/
/* 接口  ： ZeroTouchUI_EnterPageByType                                       */
/* 功能  ： 打开ZeroSPTouch的界面                                       */
/* 参数说明 ：strPageAction 类型：string                                */
/*      strPageAction 需要打开界面的Action                               */
/* 返回值 ： 失败--false 成功--true                                     */
/************************************************************************/
bool ZeroTouchUI_EnterPageByType(int iType);

#if IF_FEATURE_METASWITCH
/************************************************************************/
/* 接口  ： ZeroSPTouch_EnterAutopDirect                                */
/* 功能  ： 直接进入autop用户名与密码输入界面                           */
/* 返回值 ： 失败--false 成功--true                                     */
/************************************************************************/
bool ZeroTouchUI_EnterAutopDirect();
#endif

#endif // MOD_ZERO_TOUCH_UI_H
