#ifndef __X_LIST_VIEW_ADAPTER_H__
#define __X_LIST_VIEW_ADAPTER_H__

#include "xlistviewadapterbase.h"
#include "ilistviewitemaction.h"

class xListView;
class xListViewAdapter : public xListViewAdapterBase
    , public IListViewAdapter
    , public IListViewItemAction
{
public:
    xListViewAdapter();
    virtual ~xListViewAdapter();

// IListViewAdapter
public:
    // 是否已经绑定特定的列表框
    virtual bool IsListViewAttach(xListView* pListView);
    // 数据代理绑定
    virtual void OnListViewAttach(xListView* pListView);
    // 数据代理解除绑定
    virtual void OnListViewDetach(xListView* pListView);

#if IF_XLISTVIEW_SUPPORT_GROUP
    // 获取Group风格中的段信息
    virtual bool PrepareSection(xListView* pListView, int nSectionIndex, xListViewSection* pSection);
#endif

#if IF_XLISTVIEW_SHOW_ITEM_PARTLY && IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
    virtual xListViewItem* PrepareItemByOffset(xListView* pListView, int nOffset, int& nDataIndex, chRect& rcItem);
#endif

    // 进行数据切换时进行数据索引校验，如果nDataIndex不是有效的数据，则返回false,并且将nDataIndex改为正确的值
    virtual bool PrecheckDataIndex(int& nDataIndex);

#if IF_XLISTVIEW_ITEM_CUSTOM_WIDTH
    // 不定长列表项场景中，根据数据索引获取Item的宽度
    virtual bool GetItemWidth(xListView* pListView, int nDataIndex, int& nItemWidth);
#endif

#if IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
    // 不等高列表项场景中，根据数据索引获取Item的高度；
    virtual bool GetItemHeight(xListView* pListView, int nDataIndex, int& nItemHeight);
#endif

#if IF_XLISTVIEW_CUSTOM_SWITCH_FOCUS
    // 获取真正有焦点的数据索引
    virtual int GetFocusDataIndex(int nDataIndex, bool bNext);
#endif

    // 翻页时处理最后一页（如果处理完返回true）
    virtual bool OnLastItemNext();
    // 翻页时处理第一页（如果处理完返回true）
    virtual bool OnFirstItemPrevious();
    // 数据清空
    virtual void OnCleanContent(xListView* pListView);

    virtual void OnContentPosChanged(xListView* pListView, int nContentPos);

#if IF_XLISTVIEW_SUPPORT_SLIDE
    virtual void OnSlideStateChanged(xListView* pListView, int nSlideState);
#endif

// IListViewItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(xListView* pListView, xListViewItem* pItem, int nAction);

protected:
    // 获取真正代理指针
    virtual IListViewAdapterPtr GetAdapter() const;
};

#endif
