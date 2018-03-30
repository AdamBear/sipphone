
#ifndef __I_LIST_ITEM_ACTION_H__
#define __I_LIST_ITEM_ACTION_H__

enum LIST_ITEM_ACTION
{
    LIST_ITEM_ACTION_BASE           = 0,
    LIST_ITEM_ACTION_FOCUS          = LIST_ITEM_ACTION_BASE + 1,        // 列表项获得焦点
    LIST_ITEM_ACTION_SELECT         = LIST_ITEM_ACTION_BASE + 2,        // 列表项被选中
    LIST_ITEM_ACTION_CUSTOM_BEGIN   = LIST_ITEM_ACTION_BASE + 3,        // 列表项自定义Action开始
};

class CFrameListBase;
class CListItem;

class IListItemAction
{
public:
    IListItemAction() {}
    virtual ~IListItemAction() {}

public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)    = 0;
};

#endif
