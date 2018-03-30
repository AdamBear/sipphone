#ifndef __DEFINE_AUTHORITYMESSAGEDEFINE_H__
#define __DEFINE_AUTHORITYMESSAGEDEFINE_H__

#include "innermsgdefine.h"

// Language message define.
enum AuthorityMessageDefine
{
    AUTHORITY_MESSAGE_BEGIN = INNER_MSG_AUTHORITY_MESSAGE_BEGIN,

    // Current language changed. Extra: language name after change.
    AUTHORITY_MESSAGE_AUTHORITY_CHANGED = AUTHORITY_MESSAGE_BEGIN + 1,

    AUTHORITY_MESSAGE_END = INNER_MSG_AUTHORITY_MESSAGE_END,
};

#endif  // __DEFINE_AUTHORITYMESSAGEDEFINE_H__
