#ifndef PHONELIB_MESSAGEDEFINE_H_
#define PHONELIB_MESSAGEDEFINE_H_

#include "innermsgdefine.h"

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

enum PhoneStateMessageDefine
{
    PHONESTATE_MESSAGE_BEGIN = INNER_MSG_PHONESTATE_BEGIN,

    PHONESTATE_STATE_CHANGED = INNER_MSG_PHONESTATE_BEGIN + 1,  // WPARAM: PhoneState, LPARAM: 0-off, 1-on.

    // States changed, and posts some top of states..
    PHONESTATE_TOP_LIST_CHANGED = INNER_MSG_PHONESTATE_BEGIN + 2,

#if SUPPORT_SMS
    PHONESTATE_SMS_CHANGED  = INNER_MSG_PHONESTATE_BEGIN + 3,
    PHONESTATE_SMS_POPUP_PROCESS    = INNER_MSG_PHONESTATE_BEGIN + 4,   //WPARAM:0/1,0:Destory,1:Check LPARAM:0/1, IsPlayRing
    PHONESTATE_SMS_FULLSCREEN_EXIT  = INNER_MSG_PHONESTATE_BEGIN + 5,   //WPARAM: 0/1,IsSingleSessionOnly
#endif

    PHONESTATE_ACD_STATUS_CHANGED = INNER_MSG_PHONESTATE_BEGIN + 6, //WPARAM:ACD Status

    PHONESTATE_MESSAGE_END = INNER_MSG_PHONESTATE_END,
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

enum AdvancedMessageDefine
{
    // hotline number change
    ADVANCED_CALL_FEATURE_HOTLINE_NUMBER_CHANGED = INNER_MSG_FORWARD_BEGIN,
    // hotline delay change
    ADVANCED_CALL_FEATURE_HOTLINE_DELAY_CHANGED,
    // intercom allowance changed
    INTERCOM_ALLOWANCE_CHANGED,
    // intercom mute status change
    INTERCOM_MUTE_STATUS_CHANGED,
    // intercom tone status changed
    INTERCOM_TONE_STATUS_CHANGED,
    // intercom barge status changed
    INTERCOM_BARGE_STATUS_CHAGNED,
    // histroy save changed
    ADVANCED_CALL_FEATURE_HISTORY_SAVE_SWITCH_CHANGED

};

#endif  // PHONELIB_MESSAGEDEFINE_H_
