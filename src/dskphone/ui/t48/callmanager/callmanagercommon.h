#ifndef _CALL_MANAGER_COMMON_H_
#define _CALL_MANAGER_COMMON_H_

#include "commonapi/stringhelperapi.h"

enum CALLMGR_PAGE_TYPE
{
    CALLMGR_NONE = -1,
    CALLMGR_DIALING = 0,
    CALLMGR_DIRECTORY,
    CALLMGR_CALLLOG,
};

typedef void (*OpenCallMGRPage)();     // 打开对应界面的函数指针
typedef void (*OnShowCallMGRPage)
();   // 打开页面时调用的函数,主要用于恢复页面数据
typedef void (*OnHideCallMGRPage)
();   // 隐藏页面时调用的函数,主要用于备份页面数据
typedef void (*ClearCallMGRPageData)();// 退出对应界面的函数指针

#endif
