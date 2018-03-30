#ifndef __X_LIST_VIEW_ITEM_H__
#define __X_LIST_VIEW_ITEM_H__

#include "xWindow.h"
#include "ilistviewitemaction.h"
#include <xwindow/xlistviewdefine.h>

enum
{
    // 列表项类型(每个继承类使用一个字节来区分)
    LIST_ITEM_TYPE_BASE     = 0x00000000,
    LIST_ITEM_TYPE_MARK     = 0xFF000000,
};


class xListViewItem;

typedef void (*FUN_FREE_ITEM)(xListViewItem* pItem);

class xListView;
class xListViewItem : public xFrameLayout
{
    DECLARE_VIEW_DUMP_CLASS(xListViewItem)

public:
    explicit xListViewItem(int nType, IListViewItemAction* pAction = NULL);
    virtual ~xListViewItem();

    static void SetFunFreeItem(FUN_FREE_ITEM pFunFreeItem);

public:
    // 释放
    virtual void Release();

    virtual void SetListView(xListView* pListView);

    // 设置Action处理对象
    void SetAction(IListViewItemActionPtr pAction);

    // 清除内容
    virtual void ClearContent();

    // 选中或取消选中此选项(会有选中事件通知)
    void SelectItem(bool bSelect);

    // 设置焦点(会有选中事件通知)
    void FocusItem(bool bFocus, int nAction = LIST_VIEW_ITEM_ACTION_FOCUS);

    // 设置焦点状态
    virtual void SetItemFocus(bool bFocus);

    // 获取类型
    xListView* GetListView() const;

    // 获取体类型
    int GetType() const;

    // 获取数据索引
    int GetDataIndex() const;

    // 设置索引
    void SetDataIndex(int nIndex);

    // 是否被选中或者有焦点
    bool IsItemFocus() const;

#if IF_MOVE_FOCUS_BY_KEY
    virtual bool IsSwitchInsideFocus(int nFocusReason);
#endif

public:
#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
    virtual bool onKeyPressedEvent(int key);
#endif
    virtual bool onPaintBackground(xPainter& p);

#if IF_GESTURE_ENABLE
    virtual bool onGestureEvent(xGestureEvent* pEvent);
    virtual void onClick();
#endif

#if IF_XLISTVIEW_SUPPORT_ITEM_CUSTOM_DECIDE_FOCUS_BG
    virtual bool IsShowFocusBG();
#endif

protected:
    // 通知处理Action
    void DoAction(int nAction);

    xStyle* GetItemStyle() const;

protected:
    xListView* m_pListView;
    // 列表项用户操作接口
    IListViewItemAction* m_pAction;

    // 列表项类型
    int m_nType;

    friend class xListView;

private:
    // 列表项索引，从0开始
    int m_nDataIndex;
    // 是否有焦点
    bool m_bFocus;

    static FUN_FREE_ITEM s_pFunFreeItem;
};

typedef xListViewItem* xListViewItemPtr;
typedef YLVector<xListViewItemPtr> VEC_LIST_ITEM;

#endif
