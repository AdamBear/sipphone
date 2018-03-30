#ifndef __I_LISTVIEW_TITLE_ACTION_H__
#define __I_LISTVIEW_TITLE_ACTION_H__

#include <xwindow/xlistviewdefine.h>

#if IF_XLISTVIEW_SUPPORT_TITLE
class xListView;
class xListViewTitle;

enum LIST_TITLE_ACTION
{
    LIST_TITLE_ACTION_BASE      = 0,            // 标题栏ACTION
};

class IListViewTitleAction
{
public:
    IListViewTitleAction() {};
    virtual ~IListViewTitleAction() {};

public:
    // 标题框Action处理
    virtual void OnTitleAction(xListView* pListView, xListViewTitle* pTitle, int nAction)   = 0;
};
#endif

#endif
