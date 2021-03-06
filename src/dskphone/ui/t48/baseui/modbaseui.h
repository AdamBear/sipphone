#ifndef MOD_BASE_UI_H
#define MOD_BASE_UI_H

#include <ylstring.h>

class CDlgMenu;

/************************************************************************/
/* 接口  ： BaseUI_Init()                                                */
/* 功能  ： BaseUI模块初始                                              */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void BaseUI_Init();

/************************************************************************/
/* 接口  ： BaseUI_RegisterDlgClass()                                   */
/* 功能  ： 实现窗体类注册                                              */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void BaseUI_RegisterDlgClass();

/************************************************************************/
/* 接口  ： BaseUI_CreateMenu                                           */
/* 功能  ： 实现菜单显示                                                */
/* 参数说明 ：strMenuAction 类型：str::string                           */
/* 返回值 ： 失败--false 成功--true                                     */
/************************************************************************/
bool BaseUI_CreateMenu(yl::string strMenuAction, bool bTopWindow = false);

/************************************************************************/
/* 接口  ： BaseUI_DestroyMenu                                          */
/* 功能  ： 实现退出菜单                                                */
/* 返回值 ： 无                                                            */
/************************************************************************/
void BaseUI_DestroyMenu();

#endif // MOD_BASE_UI_H
