#ifndef PHONELIB_MESSAGEDEFINE_H_
#define PHONELIB_MESSAGEDEFINE_H_

#include "innermsgdefine.h"
#include "idle/phonestatemessagedefine.h"

enum BroadSoftMessageDefine
{
    BROADSOFT_MESSAGE_BEGIN = INNER_MSG_BROADSOFT_BEGIN,

    // BroadSoft display name changed.
    // LPARAM: account Id, Extra: the display name after change.
    BROADSOFT_DIRECTORY_DISPLAYNAME_CHANGED = BROADSOFT_MESSAGE_BEGIN + 1,
    // BroadSoft server changed.
    // LPARAM: account Id, Extra: the server after change.
    BROADSOFT_DIRECTORY_SERVER_CHANGED = BROADSOFT_MESSAGE_BEGIN + 2,
    // BroadSoft port changed.
    // LPARAM: account Id, Extra: the port after change.
    BROADSOFT_DIRECTORY_PORT_CHANGED = BROADSOFT_MESSAGE_BEGIN + 3,
    // BoradSoft username changed.
    // LPARAM: account Id, Extra: the username after change.
    BROADSOFT_DIRECTORY_USERNAME_CHANGED = BROADSOFT_MESSAGE_BEGIN + 4,
    // BroadSoft password changed.
    // LPARAM: account Id, Extra: the password after change.
    BROADSOFT_DIRECTORY_PASSWORD_CHANGED = BROADSOFT_MESSAGE_BEGIN + 5,

    // Call log display name changed.
    // LPARAM: Call log Id, Extra: the display name after change.
    BROADSOFT_CALLLOG_DISPLAYNAME_CHANGED = BROADSOFT_MESSAGE_BEGIN + 20,
    // Call log server changed.
    // LPARAM: Call log Id, Extra: the server after change.
    BROADSOFT_CALLLOG_SERVER_CHANGED = BROADSOFT_MESSAGE_BEGIN + 21,
    // Call log port changed.
    // LPARAM: Call log Id, Extra: the port after change.
    BROADSOFT_CALLLOG_PORT_CHANGED = BROADSOFT_MESSAGE_BEGIN + 22,
    // Call log username changed.
    // LPARAM: Call log Id, Extra: the username after change.
    BROADSOFT_CALLLOG_USERNAME_CHANGED = BROADSOFT_MESSAGE_BEGIN + 23,
    // Call log password changed.
    // LPARAM: Call log Id, Extra: the password after change.
    BROADSOFT_CALLLOG_PASSWORD_CHANGED = BROADSOFT_MESSAGE_BEGIN + 24,

    BROADSOFT_MESSAGE_END = INNER_MSG_BROADSOFT_END
};

enum CallWaitingMessageDefine
{
    // Message begin.
    CALLWAITING_MESSAGE_BEGIN = INNER_MSG_CALLWAITING_BEGIN,

    // Call waiting status changed, on->off or off->on.
    // LPARAM: 0-off, 1-on.
    CALLWAITING_STATUS_CHANGED = CALLWAITING_MESSAGE_BEGIN + 1,

    // Call waiting play tone changed, on->off or off->on.
    // LPARAM: 0-off, 1-on.
    CALLWAITING_PLAYTONE_CHANGED = CALLWAITING_MESSAGE_BEGIN + 2,

    // Message end.
    CALLWAITING_MESSAGE_END = INNER_MSG_CALLWAITING_END,
};

// Language message define.
enum LanguageMessageDefine
{
    LANGUAGE_MESSAGE_BEGIN = INNER_MSG_LANGUAGE_BEGIN,

    // Current language changed. Extra: language name after change.
    LANGUAGE_MESSAGE_LANGUAGE_CHANGED = LANGUAGE_MESSAGE_BEGIN + 1,

    LANGUAGE_MESSAGE_END = INNER_MSG_LANGUAGE_END,
};

// Phone lock message definition begin.
enum PhoneLockMessageDefine
{
    PHONE_LOCK_MESSAGE_BEGIN = INNER_MSG_PHONE_LOCK_BEGIN,

    // Phone lock type changed. LPARAM: phone lock type after change.
    PHONE_LOCK_TYPE_CHANGED = PHONE_LOCK_MESSAGE_BEGIN + 1,

    PHONE_LOCK_MESSAGE_END = INNER_MSG_PHONE_LOCK_END
};

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

// Advanced call feature message definition.
enum AdvanceCallFeatureMessage
{
    ADVANCED_CALL_FEATURE_MESSAGE_BEGIN = INNER_MSG_ADVANCED_CALL_FEATURE_BEGIN,

    // Key as send changed. Extra-changed key as send.
    ADVANCED_CALL_FEATURE_KEY_AS_SEND_CHANGED = ADVANCED_CALL_FEATURE_MESSAGE_BEGIN + 1,

    // History-save switch changed, LPARAM: 0-no to save, 1-to save.
    ADVANCED_CALL_FEATURE_HISTORY_SAVE_SWITCH_CHANGED = ADVANCED_CALL_FEATURE_MESSAGE_BEGIN + 2,
    // Hotline number changed. Extra-the changed hotline number.
    ADVANCED_CALL_FEATURE_HOTLINE_NUMBER_CHANGED = ADVANCED_CALL_FEATURE_MESSAGE_BEGIN + 3,
    // Hotline delay changed, LPARAM: The hotline delay after change.
    ADVANCED_CALL_FEATURE_HOTLINE_DELAY_CHANGED = ADVANCED_CALL_FEATURE_MESSAGE_BEGIN + 4,

    // Auto redial switch changed. LPRAM: 0-off, 1-on.
    ADVANCED_CALL_FEATURE_AUTO_REDIAL_SWITCH_CHANGED = ADVANCED_CALL_FEATURE_MESSAGE_BEGIN + 5,
    // Auto redial Interval changed. LPARAM: the new redial interval after change.
    ADVANCED_CALL_FEATURE_AUTO_REDIAL_INTERVAL_CHANGED = ADVANCED_CALL_FEATURE_MESSAGE_BEGIN + 6,
    // Auto redial times changed. LPARAM: the new auto redial times after change.
    ADVANCED_CALL_FEATURE_AUTO_REDIAL_TIMES_CHANGED = ADVANCED_CALL_FEATURE_MESSAGE_BEGIN + 7,

    // Call completion switch changed. LPRAM: 0-off, 1-on.
    ADVANCED_CALL_FEATURE_CALL_COMPLETION_SWITCH_CHANGED = ADVANCED_CALL_FEATURE_MESSAGE_BEGIN + 8,

    ADVANCED_CALL_FEATURE_TVOUTPUT_SWITCH_CHANGED = ADVANCED_CALL_FEATURE_MESSAGE_BEGIN + 9,

    ADVANCED_CALL_FEATURE_MESSAGE_END = INNER_MSG_ADVANCED_CALL_FEATURE_END
};

// mult paging cast message define.
enum MultPagingCastMessageDefine
{
    MULTICAST_MESSAGE_BEGIN = INNER_MSG_MULTICAST_MESSAGE_BEGIN,

    MULTICAST_MESSAGE_PAGING_LIST_CHANGED = MULTICAST_MESSAGE_BEGIN + 1,

    MULTICAST_MESSAGE_END = INNER_MSG_MULTICAST_MESSAGE_END,
};

#endif  // PHONELIB_MESSAGEDEFINE_H_
