#ifndef __X_LIST_VIEW_H__
#define __X_LIST_VIEW_H__

#include "xWindow.h"
#include <xwindow/xlistviewdefine.h>
#include "ilistviewadapter.h"
#include "xlistviewitem.h"
#include "xlistviewstyle.h"
#include "xwindow-kernel/include/xmessagehandler.h"


#if IF_XLISTVIEW_SCROLL_BY_SCROLL_BAR
typedef xScrollBar xScrollCtrl;
typedef xScrollBar* xScrollCtrlPtr;
#else
#include "xpagectrl.h"
typedef xPageCtrl xScrollCtrl;
typedef xPageCtrl* xScrollCtrlPtr;
#endif
#if IF_XLISTVIEW_SUPPORT_TITLE
#include "xlistviewtitle.h"
#endif
#if IF_XLISTVIEW_SUPPORT_SLIDE
#include "xlistslideview.h"
#endif

class XWINDOW_EXPORT xListView : public xFrameLayout
#if IF_XLISTVIEW_SCROLL_BY_PAGE_CTRL
    , public IScrollNotify
#endif
{
    DECLARE_VIEW_DUMP_CLASS(xListView)

#if IF_XLISTVIEW_SUPPORT_SLIDE
    friend class xListSlideView;
    friend class CListViewRollAnimation;
#endif
public:
    explicit xListView(xScrollCtrlPtr pScrollCtrl = NULL, xListViewStylePtr pStyle = NULL);
    virtual ~xListView();

public:
    virtual void loadAttributes(xml_node& node);
    virtual void loadChildrenElements(xml_node& node);

    virtual bool onRectChanged(const chRect& rectOld);
    virtual void LayoutContentInRect(const chRect& rectFrame);

    virtual int wrapContentWidth();
    virtual int wrapContentHeight();

#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
    virtual bool onFocusCustom(int nFocusReason);
    virtual bool onKeyPressedEvent(int key);
#endif

protected:
    virtual bool onPaintContent(xPainter& p);
#if IF_XLISTVIEW_FOCUS_INVERT_EFFECT
    virtual bool onPaintMask(xPainter& p);
#endif

    /*--------------------------------------Adapter--------------------------------------*/
public:
    void SetAdapter(IListViewAdapterPtr pAdapter, bool bLoadData = false);
    inline void ResetAdapter()
    {
        m_pAdapter = NULL;
    }

    inline IListViewAdapterPtr GetAdapter()
    {
        return m_pAdapter;
    }

    // 刷新数据
    virtual void RefreshData(int nDataIndex, int nContentPos = 0);

    // 清理所有的内容
    virtual void ClearContent();

protected:
    IListViewAdapterPtr m_pAdapter;
    /*-----------------------------------------------------------------------------------*/

    /*--------------------------------------xListView------------------------------------*/
public:
    virtual bool setFocus(int nFocusReason = FOCUS_REASON_DEFAULT);

    virtual xView* getDefaultFocusView();

    xTextView* GetHintView();

    // 设置提示信息
    void SetHint(const yl::string& strHint);
    // 设置提示信息颜色
    void SetHintFontColor(const xColor& color);
    // 设置提示信息字体大小
    void SetHintFontSize(int nFontSize);
    // 设置固定位置
    void SetHintFixRect(const chRect& rcHint);

    void SetHintRect(const chRect& rcHint);

    void CheckHintVisible();

    // 重置Action
    void ResetAction();

protected:
    void InitListViewLayout();

protected:
    // 提示Label
    xTextView* m_pHintView;
    /*-----------------------------------------------------------------------------------*/

    /*-------------------------------------ScrollCtrl------------------------------------*/
// IScrollNotify
public:
    inline xScrollCtrlPtr GetScrollCtrl()
    {
        return m_pScrollCtrl;
    }

    void SetScrollCtrlFixRect(const chRect& rcScrollCtrl);
    void CheckScrollCtrlVisible();

#if IF_XLISTVIEW_SHOW_ITEMS_BY_PAGE && !IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
    void UpdateScrollCtrlByPage();
#endif

    // 通知向前滚动
    virtual bool OnNotifyScrollPrevious(bool bCycle = true);
    // 通知向后滚动
    virtual bool OnNotifyScrollNext(bool bCycle = true);

protected:
    xScrollCtrlPtr m_pScrollCtrl;
    /*-----------------------------------------------------------------------------------*/

    /*----------------------------------------Title--------------------------------------*/
#if IF_XLISTVIEW_SUPPORT_TITLE
public:
    // 设置标题栏
    void SetTitle(xListViewTitlePtr pTitle);
    // 获取标题栏
    inline xListViewTitlePtr GetTitle()
    {
        return m_pTitle;
    }

#if IF_XLISTVIEW_SUPPORT_SLIDE
    inline void SetScrollTitle(bool bScrollTitle)
    {
        m_bScrollTitle = bScrollTitle;
    }

    inline bool IsScrollTitle() const
    {
        return m_bScrollTitle;
    }
#endif

    void ReleaseTitle();

protected:
    xListViewTitlePtr m_pTitle;
#if IF_XLISTVIEW_SUPPORT_SLIDE
    bool m_bScrollTitle;
#endif
#endif
    /*-----------------------------------------------------------------------------------*/

    /*----------------------------------------Style--------------------------------------*/
public:
    void SetStyleType(int nStyleType);
    inline xListViewStylePtr GetStyle() const
    {
        return m_pStyle;
    }

protected:
    void ReleaseStyle();

protected:
    xListViewStylePtr m_pStyle;
    /*-----------------------------------------------------------------------------------*/

    /*----------------------------------------Item---------------------------------------*/
public:
    // 设置每页列表项数量
    void SetPageItemCount(int nPageCount);
    // 获得每页列表项数量
    int GetPageItemCount();

#if IF_XLISTVIEW_SHOW_ITEMS_BY_PAGE
    void SwitchFocusToNextItem(bool bCycle = true);
    void SwitchFocusToPreItem(bool bCycle = true);
    void SwitchFocusToNextPage(bool bCycle = true);
    void SwitchFocusToPrePage(bool bCycle = true);
    void SwitchToNextPage(bool bCycle = true);
    void SwitchToPrePage(bool bCycle = true);
#endif

    // 获得当前显示的列表项数量
    inline int GetTotalItemCount() const
    {
        return m_vecItem.size();
    }

    // 设置数据总数量
    void SetTotalDataCount(int nTotalCount);
    // 获得数据总数量
    inline int GetTotalDataCount() const
    {
        return m_nDataCount;
    }

    // 获得选中行号
    inline int GetFocusItemIndex() const
    {
        return m_nFocusIndex;
    }
    // 根据行号选中一行，传INVALID_ITEM_INDEX时将取消选中(会通知外部处理选中)
    void SelectItemByIndex(int nIndex);
    // 根据行号设置焦点
    void FocusItemByIndex(int nIndex);
    // 焦点设置到最后一行
    void FocusLastItem();
    // 设置选中行（不会通知外部处理选中）
    void SetItemFocusByIndex(int nIndex);

    // 获取选中行的数据索引
    int GetFocusItemDataIndex();
    // 根据数据索引选中一行，传INVALID_ITEM_INDEX时将取消选中
    void SelectItemByDataIndex(int nIndex);
    // 根据数据索引设置焦点
    void FocusItemByDataIndex(int nIndex);

    // Detach后，标识自动清除
    void SetItemAttach();

    inline bool IsItemAttach() const
    {
        return m_bAttachItem;
    }

    // 添加列表项（注意判断返回值，为false时要注意释放指针）
    bool AddItem(xListViewItemPtr pItem);

    // 只清除列表项，不清除FrameList的总数量等信息
    void ClearAllItem();

    // 根据行号删除一行（成功返回true，失败返回false）
    virtual bool DelItemByIndex(int nIndex);
    // 根据数据索引删除一行（成功返回true，失败返回false）
    virtual bool DelItemByDataIndex(int nIndex);

    // 从DataIndex开始加载数据
    // 返回值:true表示有添加或删除Item
    void LoadListItem(int nDataIndex, bool bCheckIndex = true);

    // 根据行号获得相应的列表项
    xListViewItemPtr GetItemByIndex(int nIndex);
    // 根据数据索引获得相应的列表项
    xListViewItemPtr GetItemByDataIndex(int nIndex);
    // 获得焦点行的列表项
    xListViewItemPtr GetFocusItem();

    xListViewItemPtr GetFirstItem();
    xListViewItemPtr GetLastItem();

    // 由列表项获得对应的行号
    int GetIndexByItem(xListViewItemPtr pItem);

    // 由列表项数据索引获得对应的行号
    int GetIndexByDataIndex(int nDataIndex);

    int GetPageDataIndex();

    bool IsCanReuseItem(int nItemType);

    bool IsAllSameItem(int nItemType);

    bool IsHadConflictItem(int nItemType, int nMaskType);

#if IF_XLISTVIEW_SUPPORT_SLIDE
    // Item项是否完成显示
    bool IsItemShowFully(int nIndex);
#endif

    // 是否自动设置焦点
    bool IsAutoFocusItem();
    // 是否有Item焦点
    bool IsItemFocusable();

    // 数据索引更改时通知处理类
    virtual void OnDataIndexChange(xListViewItemPtr pItem);
    // 选中状态更改时通知处理类
    virtual void OnItemFocusChange(xListViewItemPtr pItem, bool bPreFocus);

    void LayoutItems();

#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE
    void RelayoutItem(int nBeginDataIndex, int nEndDataIndex,
                      const chRect& rcContent, int nYOffset);
    void OnItemInflate(xListViewItemPtr pItem);
    void SetFlateItem(xListViewItemPtr pItem);
#endif

#if IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
    void OnContentChange(int nPageHeight, int nContentHeight, int nContentPos);
    void OnContentPosChange(int nPosition);
#endif

protected:
    // 重置数据
    void ResetListData();
    bool LoadItem(xListViewItemPtr pItem, int nDataIndex,
                  int nLeftMargin, int nTopMargin,
                  int nWidth, int nHeight);
    void SetItemLayoutInfo(xListViewItemPtr pItem,
                           int nLeftMargin, int nTopMargin,
                           int nWidth, int nHeight);

    bool DeleteItem(int nIndex);
    // 取消行焦点
    void DefocusItem();
    // 切换列表项的焦点
    void ChangeItemFocus(xListViewItemPtr pItem, bool bPreFocus);

    void LocateItem(int nDataIndex);

    virtual void OnAddItem(xListViewItemPtr pItem);
    virtual void OnDeleteItem(xListViewItemPtr pItem);
    virtual void OnClearAllItem();

    // 总数据更新
    virtual void OnDataCountChange();
    // 响应清空数据
    virtual void OnClearContent();

    void ShowAllItem();

protected:
    // 列表项
    VEC_LIST_ITEM m_vecItem;

private:
    // 总的数据
    int m_nDataCount;
    // 焦点所在的行号
    int m_nFocusIndex;
    // Item是否由外部管理
    bool m_bAttachItem;
#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE
    xListViewItemPtr m_pFlateItem;
#endif
    /*-----------------------------------------------------------------------------------*/

#if IF_XLISTVIEW_SUPPORT_SLIDE
    /*--------------------------------------Slide View-----------------------------------*/
public:
    void SetSlidable(bool bSlidable);
    inline xListSlideView* GetListSlideView() const
    {
        return m_pSlideView;
    }

protected:
    void BeginSlide();
    void EndSlide();
    void LocateByOffset(int nOffset, bool bCheckBound);
    bool RelocateFocusItem();

    virtual xListSlideView* AllocListSlideView();

protected:
    xListSlideView* m_pSlideView;
    /*-----------------------------------------------------------------------------------*/
#endif
};
typedef xListView* xListViewPtr;

#endif // __xListView_h__
