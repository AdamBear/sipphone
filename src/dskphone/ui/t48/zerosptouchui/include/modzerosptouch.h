#ifndef MOD_ZERO_SP_TOUCH
#define MOD_ZERO_SP_TOUCH

#include <ylstring.h>

/************************************************************************/
/* 接口  ： ZeroSPTouch_RegisterDlgClass()                               */
/* 功能  ： 实现窗体类注册                                              */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void ZeroSPTouch_RegisterDlgClass();

/************************************************************************/
/* 接口  ： ZeroSPTouch_CreateDlg                                       */
/* 功能  ： 打开ZeroSPTouch的界面                                       */
/* 参数说明 ：strPageAction 类型：string                                */
/*      strPageAction 需要打开界面的Action                               */
/* 返回值 ： 失败--false 成功--true                                     */
/************************************************************************/
bool ZeroSPTouch_OpenDlg(const yl::string & strPageAction, bool bRepaint = false);

/************************************************************************/
/* 接口  ： ZeroSPTouch_CreateDlg                                       */
/* 功能  ： 销毁打开的ZeroSPTouch界面                                       */
/* 返回值 ： 失败--false 成功--true                                     */
/************************************************************************/
bool ZeroSPTouch_Destroy();

#if IF_FEATURE_METASWITCH
/************************************************************************/
/* 接口  ： ZeroSPTouch_EnterAutopDirect                                */
/* 功能  ： 直接进入autop用户名与密码输入界面                           */
/* 返回值 ： 失败--false 成功--true                                     */
/************************************************************************/
bool ZeroSPTouch_EnterAutopDirect();
#endif

bool ZeroSPTouch_IsAllowExit();

#endif // MOD_ZERO_SP_TOUCH
