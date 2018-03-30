#ifndef MODIDLESCREENUI_H
#define MODIDLESCREENUI_H

#include <ylstl/yllist.h>

// 前向声明
struct CustomNotify;

// 注册模块的窗口
void idleScreenUI_RegisterDlgClass();

// 刷新XMLBrowser的数据
void idleScreenUI_UpdateXMLBrowser();

void idleScreenUI_ResetScreensaver();

// 显示dsskey Lock password
void idleScreenUI_ShowDsskeyLockPassword(int iDssKeyID);

#endif // MODIDLESCREENUI_H
