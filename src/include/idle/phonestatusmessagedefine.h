#ifndef DEFINE_PHONESTATUSMESSAGEDEFINE_H_
#define DEFINE_PHONESTATUSMESSAGEDEFINE_H_

#include "innermsgdefine.h"

enum PhoneStatusMessageDefine
{
    PHONE_STATUS_MESSAGE_BEGIN = INNER_MSG_PHONESTATUS_BEGIN,

    // Phone model changed, Extra data-the changed model.
    PHONE_STATUS_MODEL_CHANGED = PHONE_STATUS_MESSAGE_BEGIN + 1,
    // Phone hardware changed. Extra data-the changed hardware.
    PHONE_STATUS_HARDWARE_CHANGED = PHONE_STATUS_MESSAGE_BEGIN + 2,
    // Phone firmware changed. Extra data-the changed firmware.
    PHONE_STATUS_FIRMWARE_CHANGED = PHONE_STATUS_MESSAGE_BEGIN + 3,
    // Phone product Id changed. Extra data-the changed product Id.
    PHONE_STATUS_PRODUCT_ID_CHANGED = PHONE_STATUS_MESSAGE_BEGIN + 4,
    // Phone Mac changed. Extra data-the changed Mac.
    PHONE_STATUS_MAC_CHANGED = PHONE_STATUS_MESSAGE_BEGIN + 5,

    PHONE_STATUS_MESSAGE_END = INNER_MSG_PHONESTATE_END
};

#endif  // DEFINE_PHONESTATUSMESSAGEDEFINE_H_
