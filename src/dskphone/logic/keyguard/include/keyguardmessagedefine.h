#ifndef DEFINE_KEYGUARD_MESSAGEDEFINE_H_
#define DEFINE_KEYGUARD_MESSAGEDEFINE_H_

#include "innermsgdefine.h"

// KeyGuard message definition.
enum KeyGuardMessageDefine
{
    KEYGUARD_MESSAGE_BEGIN = INNER_MSG_PHONE_LOCK_BEGIN,

    // keyGuard enable changed. WPARAM: 0/disEnable 1/enable.
    KEYGUARD_ENABLE_CHANGED = INNER_MSG_PHONE_LOCK_BEGIN + 1,

    // keyGuard type changed. WPARAM: 0/unLock 1/lock.
    KEYGUARD_LOCK_CHANGED = INNER_MSG_PHONE_LOCK_BEGIN + 2,

    // keyGuard auto lock time changed.
    KEYGUARD_AUTOLOCK_TIME_CHANGED = INNER_MSG_PHONE_LOCK_BEGIN + 3,

    KEYGUARD_MESSAGE_END = INNER_MSG_PHONE_LOCK_END
};

#endif  // DEFINE_VIDEOMESSAGEDEFINE_H_
