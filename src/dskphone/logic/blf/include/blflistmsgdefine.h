#ifndef __BLFLIST_MSG_DEFINE_H__
#define __BLFLIST_MSG_DEFINE_H__
#include <innermsgdefine.h>

enum BLFMessageDefine
{
    BLFLIST_MESSAGE_BEGIN = INNER_MSG_BLFLIST_MESSAGE_BEGIN,

    BLF_MESSAGE_BLFDSSKEY_CONFIGCHANGE, // BLF配置更改时往BLF模块推送的消息
    BLF_MESSAGE_BLFLISTDSSKEY_CONFIGCHANGE,
    BLFLIST_DATA_UPDATE,                // BLFList数据更新消息, 提供给AutoSet使用
    BLF_CUSTOM_DATA_CHANAGE,            // 自定义配置更改
    BLF_MESSAGE_FLUSH,                  // pickup或者blf刷新

    BLFLIST_MESSAGE_END = INNER_MSG_BLFLIST_MESSAGE_END
};

#endif // __BLFLIST_MSG_DEFINE_H__
