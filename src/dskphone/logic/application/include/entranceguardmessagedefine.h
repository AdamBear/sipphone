#ifndef __ENTRANCE_GUARD_MESSAGE_H_
#define __ENTRANCE_GUARD_MESSAGE_H_

#include "innermsgdefine.h"

enum EntranceGuardMessageDefine
{
    ENTRANCE_GUARD_MESSAGE_BEGIN = INTER_MSG_DOOR_PHONE_MESSAGE_BEGIN,

    ENTRANCE_GUARD_MESSAGE_START,                       // 开始

    ENTRANCE_GUARD_MESSAGE_STOP,                        // 结束

    ENTRANCE_GUARD_MESSAGE_PAUSE,                       // 暂停

    ENTRANCE_GUARD_MESSAGE_REFRESH,                     // 刷新UI

    ENTRANCE_GUARD_MESSAGE_TIME_OUT,                    // 提示超时

    ENTRANCE_GUARD_MESSAGE_ERROR,                       // 提示错误

    ENTRANCE_GUARD_MESSAGE_END = INTER_MSG_DOOR_PHONE_MESSAGE_END,
};

#endif  // __ENTRANCE_GUARD_MESSAGE_H_
