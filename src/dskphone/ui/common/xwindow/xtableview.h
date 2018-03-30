#ifndef __X_TABLE_VIEW_H__
#define __X_TABLE_VIEW_H__

#include <xwindow/xwindowdefine.h>
#include "xListView.h"

#if IF_TABLE_VIEW_ENABLE
#include "xtableviewstyle.h"

class xTableView : public xListView
{
    DECLARE_VIEW_DUMP_CLASS(xTableView)

public:
    xTableView();
    virtual ~xTableView();

public:
    virtual void loadAttributes(xml_node& node);
    virtual int wrapContentWidth();
    virtual int wrapContentHeight();

    virtual bool setFocus(int nFocusReason = FOCUS_REASON_DEFAULT);

#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
    virtual bool onKeyPressedEvent(int key);
#endif

    // 选中状态更改时通知处理类
    virtual void OnItemFocusChange(xListViewItemPtr pItem, bool bPreFocus);

    xTableViewStylePtr GetTableViewStyle() const;
    xListViewItemPtr GetGridItem(int nRow, int nCol);

protected:
#if IF_MOVE_FOCUS_BY_KEY
    bool ProcessMoveFocus(int nFocusReason);
#endif

    bool ProcessSetFocus(int nFocusReason);
    void LocateAndFocusItem(int nDataIndex);

#if !IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
    // 总数据更新
    virtual void OnDataCountChange();
    // 响应清空数据
    virtual void OnClearContent();
#endif

#if IF_XLISTVIEW_SUPPORT_SLIDE
    /*--------------------------------------Slide View-----------------------------------*/
protected:
    virtual xListSlideView* AllocListSlideView();
    /*-----------------------------------------------------------------------------------*/
#endif
};


////////////////////////////////////////////////////////////////////////////////////////////
// Table型的MainMenu风格
class xItemNoneBGStyle : public xStyle
{
public:
    xItemNoneBGStyle();
    virtual ~xItemNoneBGStyle();

};
////////////////////////////////////////////////////////////////////////////////////////////

#endif

#endif
