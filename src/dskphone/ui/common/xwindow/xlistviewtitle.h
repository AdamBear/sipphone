#ifndef __X_LIST_VIEW_TITLE_H__
#define __X_LIST_VIEW_TITLE_H__

#include "xWindow.h"
#include <xwindow/xlistviewdefine.h>
#include "ilistviewtitleaction.h"

#if IF_XLISTVIEW_SUPPORT_TITLE
enum
{
    LIST_VIEW_TITLE_TYPE_NONE           = 0,
};

class xListView;
class xListViewTitle : public xFrameLayout
{
public:
    explicit xListViewTitle(int nType, IListViewTitleAction* pAction = NULL);
    virtual ~xListViewTitle();

public:
    // 设置标题栏Action
    void SetAction(IListViewTitleAction* pAction);

    void SetListView(xListView* pListView);

    int GetType() const;

    int GetTitleHeight();

protected:
    // 通知处理用户操作Action
    void DoAction(int nAction);

protected:
    int m_nType;

private:
    IListViewTitleAction* m_pAction;
    xListView* m_pListView;
};
typedef xListViewTitle* xListViewTitlePtr;
#endif

#endif
