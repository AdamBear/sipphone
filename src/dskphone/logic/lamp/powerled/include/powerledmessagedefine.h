#ifndef DEFINE_POWERLEDMESSAGEDEFINE_H_
#define DEFINE_POWERLEDMESSAGEDEFINE_H_

#include "innermsgdefine.h"

// Power Led message definition.
enum PowerLedMessageDefine
{
    POWER_LED_MESSAGE_BEGIN = INNER_MSG_POWER_LED_MESSAGE_BEGIN,

    // Incoming call flash
    POWER_LED_INCOMING_CALL,

    // Talking
    POWER_LED_TALKING,

    // Voice Mail
    POWER_LED_VOICE_MAIL,

    // Text Message
    POWER_LED_TEXT_MESSAGE,

    POWER_LED_MESSAGE_END = INNER_MSG_POWER_LED_MESSAGE_END
};

#endif  // DEFINE_VIDEOMESSAGEDEFINE_H_
