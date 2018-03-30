#include "xlistviewadapterbase.h"
#include "xListView.h"

xListViewAdapterBase::xListViewAdapterBase(void)
    : m_pListView(NULL)
    , m_nPageDataIndex(0)
    , m_nContentPos(0)
    , m_nTotalCount(0)
    , m_nFocusIndex(INVALID_ITEM_INDEX)
{
}

xListViewAdapterBase::~xListViewAdapterBase(void)
{
    ClearListView();
}

void xListViewAdapterBase::ClearListView()
{
    if (NULL != m_pListView)
    {
        m_pListView->ResetAction();
        m_pListView->ResetAdapter();
        m_pListView = NULL;
    }
}

void xListViewAdapterBase::InitListView()
{

}

void xListViewAdapterBase::UnInitListView()
{
    if (NULL != m_pListView)
    {
        m_pListView->ClearContent();
    }
}

void xListViewAdapterBase::SetListView(xListView* pListView)
{
    bool bChangeList = false;

    if (pListView != m_pListView)
    {
        bChangeList = true;
    }

    if (bChangeList && NULL != m_pListView)
    {
        m_pListView->ClearContent();
        m_pListView->ResetAction();
        m_pListView->ResetAdapter();
    }

    if (NULL != m_pListView)
    {
        UnInitListView();
    }

    m_pListView = pListView;

    IListViewAdapterPtr pAdapter = GetAdapter();

    if (bChangeList && NULL != m_pListView && m_pListView->GetAdapter() != pAdapter)
    {
        m_pListView->SetAdapter(pAdapter);
    }

    if (NULL != m_pListView)
    {
        InitListView();
    }
}

xListView* xListViewAdapterBase::GetListView() const
{
    return m_pListView;
}

#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
int xListViewAdapterBase::AdjustPageDataIndex(int nTotalCount, int nDataIndex)
{
    if (NULL == m_pListView)
    {
        return nDataIndex;
    }

    int nPageItemCount = m_pListView->GetPageItemCount();
    xListViewStylePtr pStyle = m_pListView->GetStyle();

    if (NULL == pStyle || nPageItemCount <= 0)
    {
        return nDataIndex;
    }

    // 只有一页数据时
    if (nTotalCount <= nPageItemCount)
    {
        return 0;
    }

    // 超过一页数据
    if (nPageItemCount > 0 && (nDataIndex + nPageItemCount) > nTotalCount)
    {
        if (pStyle->IsScrollByPage())
        {
            int nRemainder = nTotalCount % nPageItemCount;

            if (0 == nRemainder)
            {
                return (nTotalCount - nPageItemCount);
            }
            else
            {
                return (nTotalCount - nRemainder);
            }
        }
        else
        {
            return (nTotalCount - nPageItemCount);
        }
    }

    return nDataIndex;
}

void xListViewAdapterBase::SetTotalCount(int nTotalCount)
{
    m_nTotalCount = nTotalCount;
}

void xListViewAdapterBase::SetPageDataIndex(int nDataIndex)
{
    if (nDataIndex < 0)
    {
        nDataIndex = 0;
    }

    m_nPageDataIndex = AdjustPageDataIndex(m_nTotalCount, nDataIndex);
    m_nContentPos = 0;
}

void xListViewAdapterBase::SetContentPos(int nContentPos)
{
    m_nContentPos = nContentPos;
}

void xListViewAdapterBase::ResetIndex()
{
    m_nTotalCount = 0;
    m_nPageDataIndex = 0;
    m_nContentPos = 0;

    if (NULL != m_pListView && m_pListView->IsAutoFocusItem())
    {
        m_nFocusIndex = 0;
    }
    else
    {
        m_nFocusIndex = INVALID_ITEM_INDEX;
    }
}

void xListViewAdapterBase::UpdateIndex()
{
    if (NULL == m_pListView)
    {
        return;
    }

    xListViewItemPtr pItem = m_pListView->GetFirstItem();

    if (NULL == pItem)
    {
        ResetIndex();
        return;
    }

    m_nTotalCount = m_pListView->GetTotalDataCount();
    m_nPageDataIndex = pItem->GetDataIndex();
    m_nFocusIndex = m_pListView->GetFocusItemIndex();

    xListViewStylePtr pStyle = m_pListView->GetStyle();

    if (NULL != pStyle)
    {
        m_nContentPos = pStyle->GetContentPos();
    }
}

int xListViewAdapterBase::GetTotalCount() const
{
    return m_nTotalCount;
}

int xListViewAdapterBase::GetPageDataIndex() const
{
    return m_nPageDataIndex;
}

int xListViewAdapterBase::GetFocusIndex() const
{
    return m_nFocusIndex;
}
#endif

void xListViewAdapterBase::SetFocusIndex(int nIndex)
{
    m_nFocusIndex = nIndex;

    if (NULL == m_pListView || INVALID_ITEM_INDEX == nIndex)
    {
        return;
    }

    int nPageItemCount = m_pListView->GetPageItemCount();

    if (nPageItemCount <= 0 || m_nFocusIndex < nPageItemCount)
    {
        return;
    }

    m_nFocusIndex = m_nFocusIndex % nPageItemCount;
}

void xListViewAdapterBase::RefreshListView()
{
    if (NULL == m_pListView || (!m_pListView->isVisible()))
    {
        return;
    }

    int nFocusIndex = m_nFocusIndex;

    m_pListView->RefreshData(m_nPageDataIndex, m_nContentPos);

    int nItemCount = m_pListView->GetTotalItemCount();

    if (nItemCount <= 0)
    {
        ResetIndex();
        return;
    }

    if (m_pListView->IsItemFocusable() && INVALID_ITEM_INDEX != nFocusIndex)
    {
        if (nFocusIndex >= nItemCount)
        {
            nFocusIndex = nItemCount - 1;
        }

        if (m_pListView->GetFocusItemIndex() != nFocusIndex)
        {
            m_pListView->FocusItemByIndex(nFocusIndex);
        }
    }

    UpdateIndex();
}

bool xListViewAdapterBase::Release()
{
    delete this;
    return true;
}

bool xListViewAdapterBase::IsAttach(xListView* pListView)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return false;
    }

    return true;
}

void xListViewAdapterBase::Attach(xListView* pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    IListViewAdapterPtr pAdapter = GetAdapter();

    if (NULL != m_pListView && m_pListView->GetAdapter() == pAdapter)
    {
        m_pListView->ResetAction();
        m_pListView->ResetAdapter();
    }

    m_pListView = pListView;
}

void xListViewAdapterBase::Detach(xListView* pListView)
{
    if (NULL == pListView || NULL == m_pListView)
    {
        return;
    }

    if (pListView == m_pListView)
    {
        m_pListView->ResetAction();
        m_pListView->ResetAdapter();
        m_pListView =  NULL;
    }
}
