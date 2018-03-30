#ifndef __DSSKEY_MSG_DEFINE_H__
#define __DSSKEY_MSG_DEFINE_H__
#include <innermsgdefine.h>

enum DssKeyMessageDefine
{
    DSSKEY_MESSAGE_BEGIN = INNER_MSG_DSSKEY_MESSAGE_BEGIN,
    DSSKEY_CONF_CHANGE,
    DSSKEY_RELOAD_FINISHED,         //
    DSSKEY_MODULE_REMOVE,           // 某个模块移除
};

#endif // __DSSKEY_MSG_DEFINE_H__
