#ifndef __INPUTHOOK_H__
#define __INPUTHOOK_H__

#include <ylhashmap.h>
#include "inputhookdefine.h"

enum HOOK_TYPE
{
    HT_ALL = 0,
    HT_KEYPAD,
    HT_MOUSE,
};

enum MOUSE_EVENT_TYPE
{
    MET_UNKNOWN = -1,
    MET_PRESS,
    MET_RELEASE,
    MET_MOVE,
};

//模块的优先级在此处设置，并且与注册顺序无关
//在枚举中的顺序决定了调用的顺序
enum HOOK_MODULE
{
    HOOK_MODULE_ERROR = -1,
    HOOK_MODULE_PREHOOKBEGIN,
    HOOK_MODULE_SCREENSAVER,     //屏保模块
    HOOK_MODULE_TESTMODE,
    HOOK_MODULE_KEYGUARD,
    HOOK_MODULE_IME,
    HOOK_MODULE_MESSAGEBOX,
    HOOK_MODULE_XMLBROWSER,
    HOOK_MODULE_VVM,

    HOOK_MODULE_CAMERA,
    HOOK_MODULE_UIMANAGER,

    //这是所有事件结束后的处理
    HOOK_MODULE_PREHOOKEND,
};

//mouse事件暂时没有使用
typedef bool (*MOUSE_HOOK_PROC)(int key, bool bPressed);
typedef YLHashMap<HOOK_MODULE, MOUSE_HOOK_PROC> MOUSE_HOOK_MAP;

typedef bool (*KEYPAD_HOOK_PROC)(int key, bool bPressed);
typedef YLHashMap<HOOK_MODULE, KEYPAD_HOOK_PROC> KEY_HOOK_MAP;

#endif //__INPUTHOOK_H__
