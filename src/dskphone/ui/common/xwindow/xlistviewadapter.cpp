#include "xlistviewadapter.h"
#include "xListView.h"
#include "xlistviewitem.h"

xListViewAdapter::xListViewAdapter()
{

}

xListViewAdapter::~xListViewAdapter()
{
    ClearListView();
}

IListViewAdapterPtr xListViewAdapter::GetAdapter() const
{
    return const_cast<xListViewAdapter*>(this);
}

bool xListViewAdapter::IsListViewAttach(xListView* pListView)
{
    return IsAttach(pListView);
}

void xListViewAdapter::OnListViewAttach(xListView* pListView)
{
    if (NULL != pListView)
    {
        UnInitListView();
    }

    Attach(pListView);
    InitListView();

    if (INVALID_ITEM_INDEX == m_nFocusIndex)
    {
        ResetIndex();
    }
}

void xListViewAdapter::OnListViewDetach(xListView* pListView)
{
    UnInitListView();
    Detach(pListView);
}

#if IF_XLISTVIEW_SUPPORT_GROUP
bool xListViewAdapter::PrepareSection(xListView* pListView, int nSectionIndex, xListViewSection* pSection)
{
    return false;
}
#endif

#if IF_XLISTVIEW_SHOW_ITEM_PARTLY && IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
xListViewItem* xListViewAdapter::PrepareItemByOffset(xListView* pListView, int nOffset, int& nDataIndex, chRect& rcItem)
{
    return NULL;
}
#endif

bool xListViewAdapter::PrecheckDataIndex(int& nDataIndex)
{
    return true;
}

#if IF_XLISTVIEW_ITEM_CUSTOM_WIDTH
bool xListViewAdapter::GetItemWidth(xListView* pListView, int nDataIndex, int& nItemWidth)
{
    return false;
}
#endif

#if IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
bool xListViewAdapter::GetItemHeight(xListView* pListView, int nDataIndex, int& nItemHeight)
{
    return false;
}
#endif

#if IF_XLISTVIEW_CUSTOM_SWITCH_FOCUS
int xListViewAdapter::GetFocusDataIndex(int nDataIndex, bool bNext)
{
    return nDataIndex;
}
#endif


bool xListViewAdapter::OnLastItemNext()
{
    return false;
}

bool xListViewAdapter::OnFirstItemPrevious()
{
    return false;
}

void xListViewAdapter::OnCleanContent(xListView* pListView)
{

}

void xListViewAdapter::OnContentPosChanged(xListView* pListView, int nContentPos)
{
    if (NULL == pListView || NULL == m_pListView || pListView != m_pListView)
    {
        return;
    }

    m_nContentPos = nContentPos;
}

#if IF_XLISTVIEW_SUPPORT_SLIDE
void xListViewAdapter::OnSlideStateChanged(xListView* pListView, int nSlideState)
{

}
#endif

void xListViewAdapter::OnItemAction(xListView* pListView, xListViewItem* pItem, int nAction)
{
    if (NULL == pListView || NULL == pItem || pListView != m_pListView)
    {
        return;
    }

    if (LIST_VIEW_ITEM_ACTION_FOCUS == nAction)
    {
        SetFocusIndex(pListView->GetFocusItemIndex());
    }
}
