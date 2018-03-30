#ifndef __POWERSAVING_MSG_DEFINE_H__
#define __POWERSAVING_MSG_DEFINE_H__

#include "innermsgdefine.h"

enum POWERSAVING_MODE
{
    POWERSAVING_MODE_PRE_CLOSE = 0, //预节电
    POWERSAVING_MODE_CLOSE,         //节电
};
enum POWERSAVINGMEG
{
    POWERSAVINGMEG_BEGIN = INNER_MSG_POWERSAVING_MESSAGE_BEGIN,
    // FIXME: INNER_MSG_POWERSAVING_MESSAGE_BEGIN + 1会和 DSK_MSG_LIGHT_LEVEL_CHANGE 消息值冲突??

    /*
    ** 进入节电模式，关闭屏幕（熄灭背光灯）
    ** wParam: 节电模式状态POWERSAVING_MODE
    ** lParam: 背光灯亮度
    ** 消息方向：mod powersaving -> other mods
    */
    POWERSAVINGMEG_SCREEN_CLOSE,

    /*
    ** 退出节电模式，唤醒屏幕（点亮背光灯）
    ** 消息方向：mod powersaving -> other mods
    */
    POWERSAVINGMEG_SCREEN_AWAKE,

    /*
    ** 用户操作消息
    ** wParam: 0=被动操作 1=主动操作
    ** 消息方向：other mods -> mod powersaving
    */
    POWERSAVIGNMEG_NOTIFY_OPERATE,

    POWERSAVINGMEG_END = INNER_MSG_POWERSAVING_MESSAGE_END,
};

#endif // !__POWERSAVING_MESSAGE_DEFINE_H__
