#ifndef __I_LIST_VIEW_ITEM_ACTION_H__
#define __I_LIST_VIEW_ITEM_ACTION_H__

enum LIST_VIEW_ITEM_ACTION
{
    LIST_VIEW_ITEM_ACTION_BASE          = 0,
    LIST_VIEW_ITEM_ACTION_FOCUS         = LIST_VIEW_ITEM_ACTION_BASE + 1,       // 列表项获得焦点
    LIST_VIEW_ITEM_ACTION_SELECT        = LIST_VIEW_ITEM_ACTION_BASE + 2,       // 列表项被选中
    LIST_VIEW_ITEM_ACTION_FOCUS_IN      = LIST_VIEW_ITEM_ACTION_BASE + 1,       // 列表项成为焦点
    LIST_VIEW_ITEM_ACTION_CUSTOM_BEGIN  = LIST_VIEW_ITEM_ACTION_BASE + 3,       // 列表项自定义Action开始
};

class xListView;
class xListViewItem;

class IListViewItemAction
{
public:
    IListViewItemAction() {}
    virtual ~IListViewItemAction() {}

public:
    // 列表项Action处理
    virtual void OnItemAction(xListView* pListView, xListViewItem* pItem, int nAction)  = 0;
};
typedef IListViewItemAction* IListViewItemActionPtr;

#endif
