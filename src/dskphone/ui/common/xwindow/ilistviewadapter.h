#ifndef __I_LIST_VIEW_ADAPTER_H__
#define __I_LIST_VIEW_ADAPTER_H__


#include <xwindow/xlistviewdefine.h>
#if IF_XLISTVIEW_SHOW_ITEM_PARTLY && IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
#include "ETLLib/ETLLib.hpp"
#endif

#if IF_XLISTVIEW_SUPPORT_SLIDE
enum LIST_VIEW_SLIDE_STATE
{
    LIST_VIEW_SLIDE_STATE_BEGIN = 0,
    LIST_VIEW_SLIDE_STATE_END = 1,
};
#endif

class xListView;
class xListViewItem;
class xListViewSection;

class IListViewAdapter
{
public:
    IListViewAdapter() {};
    virtual ~IListViewAdapter() {};

public:
    // 是否已经绑定特定的列表框
    virtual bool IsListViewAttach(xListView* pListView)         = 0;
    // 数据代理绑定
    virtual void OnListViewAttach(xListView* pListView)         = 0;
    // 数据代理解除绑定
    virtual void OnListViewDetach(xListView* pListView)         = 0;

    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView* pListView)       = 0;

#if IF_XLISTVIEW_SUPPORT_GROUP
    // 获取Group风格中的段信息
    virtual bool PrepareSection(xListView* pListView, int nSectionIndex, xListViewSection* pSection)    = 0;
#endif

    // 进行数据切换时进行数据索引校验，如果nDataIndex不是有效的数据，则返回false,并且将nDataIndex改为正确的值
    virtual bool PrecheckDataIndex(int& nDataIndex)             = 0;

#if IF_XLISTVIEW_ITEM_CUSTOM_WIDTH
    // 不定长列表项场景中，根据数据索引获取Item的宽度
    virtual bool GetItemWidth(xListView* pListView, int nDataIndex, int& nItemWidth)                    = 0;
#endif

#if IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
    // 不等高列表项场景中，根据数据索引获取Item的高度；
    virtual bool GetItemHeight(xListView* pListView, int nDataIndex, int& nItemHeight)                  = 0;
#endif

    // 根据位置获取列表项
    virtual xListViewItem* PrepareItem(xListView* pListView, int nDataIndex, xListViewItem* pItem)      = 0;
#if IF_XLISTVIEW_SHOW_ITEM_PARTLY && IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
    virtual xListViewItem* PrepareItemByOffset(xListView* pListView, int nOffset, int& nDataIndex, chRect& rcItem) = 0;
#endif
    // 刷新列表项数据
    virtual bool RefreshItem(xListView* pListView, xListViewItem* pItem)                                = 0;

#if IF_XLISTVIEW_CUSTOM_SWITCH_FOCUS
    // 获取真正有焦点的数据索引
    virtual int GetFocusDataIndex(int nDataIndex, bool bNext)   = 0;
#endif

    // 翻页时处理最后一页（如果处理完返回true）
    virtual bool OnLastItemNext()                               = 0;
    // 翻页时处理第一页（如果处理完返回true）
    virtual bool OnFirstItemPrevious()                          = 0;
    // 数据清空
    virtual void OnCleanContent(xListView* pListView)           = 0;

    virtual void OnContentPosChanged(xListView* pListView, int nContentPos) = 0;

#if IF_XLISTVIEW_SUPPORT_SLIDE
    virtual void OnSlideStateChanged(xListView* pListView, int nSlideState) = 0;
#endif
};
typedef IListViewAdapter* IListViewAdapterPtr;

#endif
