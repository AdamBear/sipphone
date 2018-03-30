#ifndef DEFINE_BROADSOFTMESSAGEDEFINE_H_
#define DEFINE_BROADSOFTMESSAGEDEFINE_H_

#include "innermsgdefine.h"

// BroadSoft message define.
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

#endif  // DEFINE_BROADSOFTMESSAGEDEFINE_H_
