#ifndef PAGEUI_CONDITION_CONTROLLER
#define PAGEUI_CONDITION_CONTROLLER

#include <ylstring.h>

// 界面条件初始化, 加载page id/dir type和函数指针
void PageAllowConditionInit();

// ---- broadsoft
// 是否允许进入bsft call control 判断
bool IsPageBsftAllow(const yl::string & strPageName = "");
// 是否允许进入bsft call control 账号选择界面判断
bool IsPageBsftSelectLineAllow(const yl::string & strPageName = "");

// ---- directory
// 是否允许进入联系人界面判断
bool IsPageDirectoryAllow(const yl::string & strDirType = "");

// ---- setting
// 是否允许进入setting界面判断
bool IsPageSettingAllow(const yl::string & strPageName = "");

#endif
