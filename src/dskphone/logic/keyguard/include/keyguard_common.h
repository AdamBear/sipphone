#ifndef _KET_GUARD_COMMON_H_
#define _KET_GUARD_COMMON_H_

#include <ylstring.h>

// 键盘锁类型
enum KEY_GUARD_TYPE
{
    KG_ALL_NONE = -1,
    KG_ALL_KEYS,    // Allkey类型锁定所有按键,可以紧急拨号
    KG_FUN_KEY,     // Funkey类型,锁定所有功能型按键
    KG_MENU_KEY,    // MenuKey类型,锁定所有配置
};

enum KEY_GUARD_MODULE
{
    KGM_MODULE_SETTING,     // 设置模块包括Setting、Menu等设置相关模块
    KGM_MODULE_DSSKEY,      // Dsskey模块
    KGM_MODULE_DIRECTORY,   // 联系人模块,包括Directory、Calllog
    KGM_MODULE_XMLBROWSER,  // XMLBrowser模块
    KGM_MODULE_TALK,        // Talk模块
    KGM_MODULE_MESSAGE,     //message硬按键
    KGM_MODULE_LINE,            //切换账号
};

enum PinType
{
    PIN_INCORRECT, // 密码错误
    PIN_CORRECT_ADMIN, // 密码正确,admin的密码
    PIN_CORRECT_LOCK, // 密码正确,键盘锁密码
};

// 判断是否解锁的回调函数
typedef bool (*CheckUnlockPswFun)();

// 弹出是否是密码拨号的提示
typedef void (*PopupEmergencyNumNote)(const yl::string &);

#endif // #ifndef _KET_GUARD_COMMON_H_

