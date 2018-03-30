
#ifndef __FRAME_LIST_ACTION_H__
#define __FRAME_LIST_ACTION_H__

#include "uikernel/ilistitemaction.h"
#include "baseui/iframetitleaction.h"

// 带复选框列表标题Action
enum
{
    LIST_TITLE_ACTION_CHECK_STATE           = LIST_TITLE_ACTION_BASE + 1,           // 选择框选中
};

// 联系人列表项Action
enum
{
    CONTACT_ITEM_ACTION_CHECK               = LIST_ITEM_ACTION_CUSTOM_BEGIN,
    CONTACT_ITEM_ACTION_DETAIL              = LIST_ITEM_ACTION_CUSTOM_BEGIN + 1,
    CONTACT_ITEM_ACTION_SEARCH              = LIST_ITEM_ACTION_CUSTOM_BEGIN + 2,
};

// wifi detail title action
enum
{
    WIFI_TITLE_ITEM_ACTION_CON              = 1,
    WIFI_TITLE_ITEM_ACTION_ADD              = 2,
};

#endif
