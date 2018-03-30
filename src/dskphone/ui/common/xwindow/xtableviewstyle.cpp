#include "xtableviewstyle.h"
#include "xtableview.h"


#if IF_TABLE_VIEW_ENABLE
xTableViewStyle::xTableViewStyle(xTableView* pTableView)
    : xListViewStyle(pTableView)
    , m_nColumn(1)
    , m_nColSpace(0)
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    , m_nOrientation(TAB_COL_ORNT_VERTICAL)
#endif
#if IF_XTABLEVIEW_COLUMN_SPACE_BY_LINE
    , m_nBoundLineType(TAB_BOUND_LINE_TYPE_NONE)
#endif
{
}

xTableViewStyle::~xTableViewStyle()
{
}

xListViewStyle* xTableViewStyle::CreateTableViewStyle(xListView* pTableView)
{
    xTableViewStyle* pStyle = new xTableViewStyle(static_cast<xTableView*>(pTableView));

    return pStyle;
}

void xTableViewStyle::RegisterStyle()
{
    currentStyleFactory.RegisterListStyle(LIST_VIEW_STYLE_TYPE_TABLE_VIEW, &xTableViewStyle::CreateTableViewStyle);
}

void xTableViewStyle::SetTableView(xTableView* pTableView)
{
    m_pListView = pTableView;
}

xTableView* xTableViewStyle::GetTableView()
{
    return static_cast<xTableView*>(m_pListView);
}

void xTableViewStyle::loadAttributes(xml_node& node)
{
    xListViewStyle::loadAttributes(node);

    PopupAndroidInt(node, XWINDOW_NAME_COLUMN, m_nColumn);
    PopupAndroidInt(node, XWINDOW_NAME_COLUMN_SPACE, m_nColSpace);

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    PopupAndroidInt(node, XWINDOW_NAME_ORIENTATION, m_nOrientation);
#endif

#if IF_XTABLEVIEW_COLUMN_SPACE_BY_LINE
    PopupAndroidInt(node, XWINDOW_NAME_BOUND_LINE_TYPE, m_nBoundLineType);
#endif
}

void xTableViewStyle::OnFinishInit()
{
    xTableView* pTableView = GetTableView();

    if (NULL == pTableView || m_nColumn <= 0)
    {
        return;
    }

    int nTotalCount = pTableView->GetTotalDataCount();

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
    {
        if (m_nItemWidth <= 0)
        {
            return;
        }

        int nRowHeight = m_nItemWidth + m_nColSpace;

        m_nPageHeight = m_nPageItemCount / m_nColumn * nRowHeight;
        m_nContentHeight = (nTotalCount + m_nColumn - 1) / m_nColumn * nRowHeight;

#if IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
        pTableView->OnContentChange(m_nPageHeight, m_nContentHeight, m_nContentPos);
#endif
        return;
    }
#endif

    if (m_nItemHeight <= 0)
    {
        return;
    }

    int nRowHeight = m_nItemHeight + m_nItemSpace;

    m_nPageHeight = m_nPageItemCount / m_nColumn * nRowHeight;
    m_nContentHeight = (nTotalCount + m_nColumn - 1) / m_nColumn * nRowHeight;

#if IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
    pTableView->OnContentChange(m_nPageHeight, m_nContentHeight, m_nContentPos);
#endif
}

void xTableViewStyle::AdjustContentPosAndIndex(int& nDataIndex)
{
    if (m_nColumn <= 0)
    {
        return;
    }

    nDataIndex = nDataIndex - (nDataIndex % m_nColumn);

    int nItemHeight = m_nItemHeight;
    int nItemSpace = m_nItemSpace;

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
    {
        nItemHeight = m_nItemWidth;
        nItemSpace = m_nColSpace;
    }
#endif

    if (nItemSpace < 0)
    {
        return;
    }

    if (nItemHeight > 0)
    {
        int nRealItemHeight = nItemHeight + nItemSpace;
        int nContentPos = nRealItemHeight * nDataIndex / m_nColumn;

        if (IsScrollByPage())
        {
            if (m_nPageHeight <= 0)
            {
                return;
            }

            nContentPos = nContentPos - (nContentPos % m_nPageHeight);
            nDataIndex = (nContentPos % nItemHeight) * m_nColumn;
        }
        else
        {
            int nMaxPos = GetMaxContentPos();

            if (nContentPos > nMaxPos)
            {
                nContentPos = nMaxPos;

                nDataIndex = nContentPos / nRealItemHeight;
            }
        }

        SetContentPos(nContentPos);

        return;
    }

#if IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
    xTableView* pTableView = GetTableView();

    if (NULL == pTableView)
    {
        return;
    }

    IListViewAdapterPtr pAdapter = pTableView->GetAdapter();

    if (NULL == pAdapter)
    {
        return;
    }

    int nTotalCount = pTableView->GetTotalDataCount();

    if (nTotalCount <= 0)
    {
        return;
    }

    int nContentPos = 0;

    if (IsScrollByPage())
    {
        int nPageHeight = 0;
        int nPageIndex = 0;

        for (int i = 0; i < nTotalCount && i <= nDataIndex; i += m_nColumn)
        {
            int nItemHeight = 0;

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
            if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
            {
                pAdapter->GetItemWidth(pTableView, i, nItemHeight);
            }
            else
            {
#endif
                pAdapter->GetItemHeight(pTableView, i, nItemHeight);
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
            }
#endif

            if (nItemHeight <= 0)
            {
                return;
            }

            int nRealItemHeight = nItemHeight + nItemSpace;

            if ((nPageHeight + nRealItemHeight) > m_nPageHeight)
            {
                nContentPos += nPageHeight;
                nPageIndex = i;

                nPageHeight = 0;
            }
            else
            {
                nPageHeight += nRealItemHeight;
            }
        }

        SetContentPos(nContentPos);
        nDataIndex = nPageIndex;

        return;
    }

    int nMaxPos = GetMaxContentPos();

    for (int i = 0; i < nTotalCount && i <= nDataIndex; i += m_nColumn)
    {
        int nItemHeight = 0;

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
        if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
        {
            pAdapter->GetItemWidth(pTableView, i, nItemHeight);
        }
        else
        {
#endif
            pAdapter->GetItemHeight(pTableView, i, nItemHeight);
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
        }
#endif

        int nRealItemHeight = nItemHeight + nItemSpace;

        if ((nContentPos + nRealItemHeight) > nMaxPos)
        {
            break;
        }
        else
        {
            nContentPos += nRealItemHeight;
            nDataIndex = i;
        }
    }

    SetContentPos(nContentPos);
#endif
}

bool xTableViewStyle::GetItemLayoutInfo(int nItemIndex, int nDataIndex,
                                        int& nLeftMargin, int& nTopMargin,
                                        int& nWidth, int& nHeight)
{
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
    {
        return GetItemLayoutInfoHorzCol(nItemIndex, nDataIndex,
                                        nLeftMargin, nTopMargin,
                                        nWidth, nHeight);
    }
#endif

#if IF_XLISTVIEW_ITEM_CUSTOM_WIDTH
    return GetItemLayoutInfoVertCol(nItemIndex, nDataIndex,
                                    nLeftMargin, nTopMargin,
                                    nWidth, nHeight);
#else
    chRect rcContainter;
    chRect rcItems;

    if (!GetContainerGeometry(rcContainter) || !GetContentGeometry(rcItems))
    {
        return false;
    }

    if (m_nColumn <= 0)
    {
        return false;
    }

    int nRow = nItemIndex / m_nColumn;
    int nCol = nItemIndex % m_nColumn;

    nLeftMargin = rcItems.left - rcContainter.left + nCol * (m_nItemWidth + m_nColSpace);
    nTopMargin = rcItems.top - rcContainter.top + nRow * (m_nItemHeight + m_nItemSpace);

#if IF_XLISTVIEW_SUPPORT_SLIDE
    nTopMargin += m_nItemOffset;
#endif

    nWidth = m_nItemWidth;
    nHeight = m_nItemHeight;

    if ((rcContainter.top + nTopMargin + nHeight) <= (rcItems.top + m_nPageHeight))
    {
        return true;
    }

    return false;
#endif
}

#if IF_XLISTVIEW_ITEM_CUSTOM_WIDTH
bool xTableViewStyle::GetItemLayoutInfoVertCol(int nItemIndex, int nDataIndex,
        int& nLeftMargin, int& nTopMargin,
        int& nWidth, int& nHeight)
{
    xTableView* pTableView = GetTableView();

    if (NULL == pTableView || m_nColumn <= 0)
    {
        return false;
    }

    chRect rcContainer;
    chRect rcItems;

    if (!GetContainerGeometry(rcContainer) || !GetContentGeometry(rcItems))
    {
        return false;
    }

    nLeftMargin = rcItems.left - rcContainer.left;
    nTopMargin = rcItems.top - rcContainer.top;

#if IF_XLISTVIEW_SUPPORT_SLIDE
    xListSlideView* pSlideView = m_pListView->GetListSlideView();
#endif

    int nPreRowItemIndex = nItemIndex - m_nColumn;
    xListViewItemPtr pPreRowItem = NULL;

    if (nPreRowItemIndex >= 0)
    {
        pPreRowItem = pTableView->GetItemByIndex(nPreRowItemIndex);
    }

    if (NULL != pPreRowItem)
    {
        nTopMargin = pPreRowItem->layout_marginTop + pPreRowItem->layout_height + m_nItemSpace;
        nWidth = pPreRowItem->layout_width;
    }
    else
    {
#if IF_XTABLEVIEW_COLUMN_SPACE_BY_LINE
        if (IsHorizontalBoundLine())
        {
            nTopMargin += m_nItemSpace;
        }
#endif

#if IF_XLISTVIEW_SUPPORT_SLIDE
        if (NULL != pSlideView)
        {
            nTopMargin += m_nItemOffset;
        }
#endif

#if IF_XLISTVIEW_ITEM_CUSTOM_WIDTH
        if (0 == m_nItemWidth)
        {
            IListViewAdapterPtr pAdapter = pTableView->GetAdapter();

            if (NULL == pAdapter || !pAdapter->GetItemWidth(pTableView, nDataIndex, nWidth))
            {
                return false;
            }
        }
        else
#endif
        {
            nWidth = m_nItemWidth;
        }
    }

    int nPreItemIndex = nItemIndex - 1;
    xListViewItemPtr pPreItem = NULL;

    if (nPreItemIndex >= 0 && (nPreItemIndex / m_nColumn) == (nItemIndex / m_nColumn))
    {
        pPreItem = pTableView->GetItemByIndex(nPreItemIndex);
    }
#if IF_XTABLEVIEW_COLUMN_SPACE_BY_LINE
    else if (IsVerticalBoundLine())
    {
        nLeftMargin += m_nColSpace;
    }
#endif

    if (NULL != pPreItem)
    {
        nLeftMargin = pPreItem->layout_marginLeft + pPreItem->layout_width + m_nColSpace;
        nHeight = pPreItem->layout_height;
    }
    else
    {
#if IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
        if (m_nItemHeight <= 0)
        {
            IListViewAdapterPtr pAdapter = pTableView->GetAdapter();

            if (NULL != pAdapter && (!pAdapter->GetItemHeight(pTableView, nDataIndex, nHeight)))
            {
                return false;
            }
        }
        else
#endif
        {
            nHeight = m_nItemHeight;
        }
    }

#if IF_XLISTVIEW_SUPPORT_SLIDE
    if (NULL != pSlideView && pSlideView->IsSliding())
    {
#if IF_XLIST_VIEW_SLIDE_DEBUG
        XWINDOW_INFO("Item Geometry:%d %d %d %d Content:%d %d %d %d",
                     rcItem.left, rcItem.top, rcItem.right, rcItem.bottom,
                     rcItems.left, rcItems.top, rcItems.right, rcItems.bottom);
#endif

        // 滑动时xListView已经布局好，可以直接跟rcContent进行比较
        return ((rcContainer.top + nTopMargin + nHeight) >= rcContent.top
                && (rcContainer.top + nTopMargin) < rcContent.bottom);
    }
#endif

    return ((rcContainer.top + nTopMargin + nHeight) <= (rcItems.top + m_nPageHeight));
}
#endif

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
bool xTableViewStyle::GetItemLayoutInfoHorzCol(int nItemIndex, int nDataIndex,
        int& nLeftMargin, int& nTopMargin,
        int& nWidth, int& nHeight)
{
    xTableView* pTableView = GetTableView();

    if (NULL == pTableView || m_nColumn <= 0)
    {
        return false;
    }

    chRect rcContainer;
    chRect rcItems;

    if (!GetContainerGeometry(rcContainer) || !GetContentGeometry(rcItems))
    {
        return false;
    }

    nLeftMargin = rcItems.left - rcContainer.left;
    nTopMargin = rcItems.top - rcContainer.top;

    int nPreRowItemIndex = nItemIndex - m_nColumn;
    xListViewItemPtr pPreRowItem = NULL;

    if (nPreRowItemIndex >= 0)
    {
        pPreRowItem = pTableView->GetItemByIndex(nPreRowItemIndex);
    }

    if (NULL != pPreRowItem)
    {
        nLeftMargin = pPreRowItem->layout_marginLeft + pPreRowItem->layout_width + m_nColSpace;
        nHeight = pPreRowItem->layout_height;
    }
    else
    {
#if IF_XTABLEVIEW_COLUMN_SPACE_BY_LINE
        if (IsVerticalBoundLine())
        {
            nLeftMargin += m_nColSpace;
        }
#endif

#if IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
        if (0 == m_nItemHeight)
        {
            IListViewAdapterPtr pAdapter = pTableView->GetAdapter();

            if (NULL == pAdapter || !pAdapter->GetItemHeight(pTableView, nDataIndex, nHeight))
            {
                return false;
            }
        }
        else
#endif
        {
            nHeight = m_nItemHeight;
        }
    }

    int nPreItemIndex = nItemIndex - 1;
    xListViewItemPtr pPreItem = NULL;

    if (nPreItemIndex >= 0 && (nPreItemIndex / m_nColumn) == (nItemIndex / m_nColumn))
    {
        pPreItem = pTableView->GetItemByIndex(nPreItemIndex);
    }
#if IF_XTABLEVIEW_COLUMN_SPACE_BY_LINE
    else if (IsHorizontalBoundLine())
    {
        nTopMargin += m_nItemSpace;
    }
#endif

    if (NULL != pPreItem)
    {
        nTopMargin = pPreItem->layout_marginLeft + pPreItem->layout_width + m_nItemSpace;
        nWidth = pPreItem->layout_width;
    }
    else
    {
#if IF_XLISTVIEW_ITEM_CUSTOM_WIDTH
        if (m_nItemWidth <= 0)
        {
            IListViewAdapterPtr pAdapter = pTableView->GetAdapter();

            if (NULL != pAdapter && (!pAdapter->GetItemWidth(pTableView, nDataIndex, nWidth)))
            {
                return false;
            }
        }
        else
#endif
        {
            nWidth = m_nItemWidth;
        }
    }

    if ((nLeftMargin + nWidth) <= (rcItems.left + m_nPageHeight))
    {
        return true;
    }

    return false;
}
#endif

#if IF_MOVE_FOCUS_BY_KEY
bool xTableViewStyle::IsLeftMostItem(int nFocusDataIndex, int nFirstDataIndex)
{
    if (m_nColumn <= 0 || INVALID_ITEM_INDEX == nFocusDataIndex)
    {
        return false;
    }

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
    {
        return (nFocusDataIndex < m_nColumn);
    }
#endif

    return (0 == ((nFocusDataIndex - nFirstDataIndex) % m_nColumn));
}

bool xTableViewStyle::IsTopMostItem(int nDataIndex)
{
    if (m_nColumn <= 0 || INVALID_ITEM_INDEX == nDataIndex)
    {
        return false;
    }

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
    {
        return (0 == (nDataIndex % m_nColumn));
    }
#endif

    return (nDataIndex < m_nColumn);
}

bool xTableViewStyle::IsRightMostItem(int nFocusDataIndex, int nFirstDataIndex)
{
    xTableView* pTableView = GetTableView();

    if (NULL == pTableView || m_nColumn <= 0 || INVALID_ITEM_INDEX == nFocusDataIndex)
    {
        return false;
    }

    int nTotalCount = pTableView->GetTotalDataCount();

    if (nTotalCount <= 0)
    {
        return false;
    }

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
    {
        return ((nFocusDataIndex + m_nColumn) >= nTotalCount);
    }
#endif

    return (0 == ((nFocusDataIndex + 1 - nFirstDataIndex) % m_nColumn) || nFocusDataIndex == (nTotalCount - 1));
}

bool xTableViewStyle::IsBottomMostItem(int nDataIndex)
{
    xTableView* pTableView = GetTableView();

    if (NULL == pTableView || m_nColumn <= 0 || INVALID_ITEM_INDEX == nDataIndex)
    {
        return false;
    }

    int nTotalCount = pTableView->GetTotalDataCount();

    if (nTotalCount <= 0)
    {
        return false;
    }

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
    {
        return (0 == ((nDataIndex + 1) % m_nColumn) || nDataIndex == (nTotalCount - 1));
    }
#endif

    int nLineFirstIndex = (nDataIndex / m_nColumn) * m_nColumn;

    return ((nLineFirstIndex + m_nColumn) >= nTotalCount);
}

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
void xTableViewStyle::SetHorizontalColumn(bool bHorizontal)
{
    if (bHorizontal)
    {
        m_nOrientation = TAB_COL_ORNT_HORIZONTAL;
    }
    else
    {
        m_nOrientation = TAB_COL_ORNT_VERTICAL;
    }
}

int xTableViewStyle::GetFocusReasonHorzCol(int nFocusReason)
{
    if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
    {
        if (xView::FOCUS_REASON_LEFT == nFocusReason)
        {
            nFocusReason = xView::FOCUS_REASON_UP;
        }
        else if (xView::FOCUS_REASON_RIGHT == nFocusReason)
        {
            nFocusReason = xView::FOCUS_REASON_DOWN;
        }
        else if (xView::FOCUS_REASON_UP == nFocusReason)
        {
            nFocusReason = xView::FOCUS_REASON_LEFT;
        }
        else if (xView::FOCUS_REASON_DOWN == nFocusReason)
        {
            nFocusReason = xView::FOCUS_REASON_RIGHT;
        }
    }

    return nFocusReason;
}
#endif

int xTableViewStyle::GetColFirstDataIndex(int nDataIndex, int nMaxIndex)
{
    if (m_nColumn > 0)
    {
        nDataIndex = nDataIndex % m_nColumn;

        if (nDataIndex > nMaxIndex)
        {
            nDataIndex = nMaxIndex;
        }
    }

    return nDataIndex;
}

int xTableViewStyle::GetColLastDataIndex(int nDataIndex, int nMaxIndex)
{
    if (m_nColumn > 0)
    {
        nDataIndex = (nMaxIndex / m_nColumn) * m_nColumn + (nDataIndex % m_nColumn);

        if (nDataIndex > nMaxIndex)
        {
            nDataIndex = nMaxIndex;
        }
    }

    return nDataIndex;
}

int xTableViewStyle::GetRowFirstDataIndex(int nDataIndex, int nMaxIndex)
{
    if (m_nColumn > 0)
    {
        nDataIndex = nDataIndex - (nDataIndex % m_nColumn);
    }

    return nDataIndex;
}

int xTableViewStyle::GetRowLastDataIndex(int nDataIndex, int nMaxIndex)
{
    if (m_nColumn > 0)
    {
        nDataIndex = nDataIndex - (nDataIndex % m_nColumn) + m_nColumn - 1;

        if (nDataIndex > nMaxIndex)
        {
            nDataIndex = nMaxIndex;
        }
    }

    return nDataIndex;
}

int xTableViewStyle::GetFocusDataIndex(int nDataIndex, int nFocusReason)
{
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    nFocusReason = GetFocusReasonHorzCol(nFocusReason);
#endif

    if (xView::FOCUS_REASON_LEFT == nFocusReason)
    {
        return (nDataIndex - 1);
    }
    else if (xView::FOCUS_REASON_RIGHT == nFocusReason)
    {
        return (nDataIndex + 1);
    }
    else if (xView::FOCUS_REASON_UP == nFocusReason)
    {
        return (nDataIndex - m_nColumn);
    }
    else if (xView::FOCUS_REASON_DOWN == nFocusReason)
    {
        return (nDataIndex + m_nColumn);
    }

    return INVALID_ITEM_INDEX;
}

int xTableViewStyle::GetCycleFocusDataIndex(int nDataIndex, int nFocusReason)
{
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
    {
        return GetCycleFocusDataIndexHorzCol(nDataIndex, nFocusReason);
    }
#endif

    xTableView* pTableView = GetTableView();

    if (NULL == pTableView)
    {
        return INVALID_ITEM_INDEX;
    }

    int nTotalCount = pTableView->GetTotalDataCount();

    if (nTotalCount <= 0 || m_nColumn <= 0)
    {
        return INVALID_ITEM_INDEX;
    }

    int nMaxIndex = nTotalCount - 1;

    if (xView::FOCUS_REASON_LEFT == nFocusReason)
    {
        nDataIndex -= 1;

        if (nDataIndex < 0)
        {
            nDataIndex = nMaxIndex;
        }
    }
    else if (xView::FOCUS_REASON_RIGHT == nFocusReason)
    {
        nDataIndex += 1;

        if (nDataIndex >= nTotalCount)
        {
            nDataIndex = 0;
        }
    }
    else if (xView::FOCUS_REASON_UP == nFocusReason)
    {
        nDataIndex -= m_nColumn;

        if (nDataIndex < 0)
        {
            int nLastRowIndex = nMaxIndex - (nMaxIndex % m_nColumn);

            nDataIndex = nLastRowIndex + nDataIndex + m_nColumn;

            if (nDataIndex > nMaxIndex)
            {
                nDataIndex = nMaxIndex;
            }
        }
    }
    else if (xView::FOCUS_REASON_DOWN == nFocusReason)
    {
        nDataIndex += m_nColumn;

        if (nDataIndex > nMaxIndex)
        {
            if ((nDataIndex / m_nColumn) == (nMaxIndex / m_nColumn))
            {
                nDataIndex = nMaxIndex;
            }
            else
            {
                nDataIndex = nDataIndex % m_nColumn;
            }
        }
    }

    return nDataIndex;
}

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
int xTableViewStyle::GetCycleFocusDataIndexHorzCol(int nDataIndex, int nFocusReason)
{
    xTableView* pTableView = GetTableView();

    if (NULL == pTableView)
    {
        return INVALID_ITEM_INDEX;
    }

    int nTotalCount = pTableView->GetTotalDataCount();

    if (nTotalCount <= 0 || m_nColumn <= 0)
    {
        return INVALID_ITEM_INDEX;
    }

    int nMaxIndex = nTotalCount - 1;

    if (xView::FOCUS_REASON_LEFT == nFocusReason)
    {
        int nColIndex = nDataIndex % m_nColumn;

        nColIndex -= 1;

        if (nColIndex < 0)
        {
            if (0 == ((nMaxIndex + 1) % m_nColumn))
            {
                nDataIndex = nMaxIndex;
            }
            else
            {
                int nLastFullRowIndex = (nMaxIndex / m_nColumn) - 1;

                if (nLastFullRowIndex < 0)
                {
                    nDataIndex = nMaxIndex;
                }
                else
                {
                    nDataIndex = nLastFullRowIndex * m_nColumn + m_nColumn - 1;
                }
            }
        }
        else
        {
            int nMaxRowIndex = nMaxIndex / m_nColumn;

            nDataIndex = nMaxRowIndex * m_nColumn + nColIndex;

            if (nDataIndex > nMaxIndex)
            {
                nDataIndex = (nMaxRowIndex - 1) * m_nColumn + nColIndex;
            }
        }
    }
    else if (xView::FOCUS_REASON_RIGHT == nFocusReason)
    {
        int nMaxColIndex = m_nColumn - 1;

        if (nMaxColIndex > nMaxIndex)
        {
            nMaxColIndex = nMaxIndex;
        }

        int nColIndex = nDataIndex % m_nColumn;

        nColIndex += 1;

        if (nColIndex > nMaxColIndex)
        {
            nDataIndex = 0;
        }
        else
        {
            nDataIndex = nColIndex;
        }
    }
    else if (xView::FOCUS_REASON_UP == nFocusReason)
    {
        nDataIndex = nDataIndex + m_nColumn - 1;

        if (nDataIndex > nMaxIndex)
        {
            int nMaxColIndex = m_nColumn - 1;

            if (nMaxColIndex > nMaxIndex)
            {
                nDataIndex = nMaxIndex;
            }
            else
            {
                nDataIndex -= m_nColumn;
            }
        }
    }
    else if (xView::FOCUS_REASON_DOWN == nFocusReason)
    {
        if (nDataIndex == nMaxIndex && nDataIndex < m_nColumn)
        {
            nDataIndex = 0;
        }
        else
        {
            nDataIndex = nDataIndex + 1 - m_nColumn;
        }
    }

    return nDataIndex;
}
#endif
#endif

int xTableViewStyle::GetDataIndexByPos(int nContentPos)
{
    if (m_nColumn <= 0)
    {
        return INVALID_ITEM_INDEX;
    }

    // 等高列表项
    if (0 != m_nItemHeight)
    {
        int nItemTotalHeight = m_nItemHeight + m_nItemSpace;

        if (nItemTotalHeight <= 0)
        {
            XWINDOW_ERR("GetDataIndexByPos(%d) can't find item ItemTotalHeight:%d",
                        nContentPos, nItemTotalHeight);

            return INVALID_ITEM_INDEX;
        }

        return ((nContentPos + nItemTotalHeight - 1) / nItemTotalHeight * m_nColumn);
    }

#if IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
    xTableView* pTableView = GetTableView();

    if (NULL == pTableView)
    {
        return INVALID_ITEM_INDEX;
    }

    IListViewAdapterPtr pAdapter = pTableView->GetAdapter();

    if (NULL == pAdapter)
    {
        return INVALID_ITEM_INDEX;
    }

    // 非等高列表项
    int nTotalDataCount = pTableView->GetTotalDataCount();
    int nItemHeight = 0;
    int nSectionPos = 0;
    int nIndex = 0;

    for (; nIndex < nTotalDataCount; nIndex += m_nColumn)
    {
        if (nSectionPos >= nContentPos)
        {
            return nIndex;
        }

        if (!pAdapter->GetItemHeight(pTableView, nIndex, nItemHeight))
        {
            XWINDOW_ERR("GetDataIndexByPos(%d) can't find item GetItemHeight:%d %d",
                        nContentPos, nIndex, nItemHeight);

            return INVALID_ITEM_INDEX;
        }

        nSectionPos = nSectionPos + nItemHeight + m_nItemSpace;
    }

    XWINDOW_ERR("GetDataIndexByPos(%d) can't find item nTotalDataCount:%d", nContentPos, nTotalDataCount);
#else
    XWINDOW_ERR("GetDataIndexByPos(%d) not support disunion item height");
#endif

    return INVALID_ITEM_INDEX;
}

int xTableViewStyle::GetContentPosByIndex(int nDataIndex)
{
    if (m_nColumn <= 0)
    {
        return 0;
    }

    // 等高列表项
    if (0 != m_nItemHeight)
    {
        int nItemTotalHeight = m_nItemHeight + m_nItemSpace;

        return (nDataIndex / m_nColumn * nItemTotalHeight);
    }

#if IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
    xTableView* pTableView = GetTableView();

    if (NULL == pTableView)
    {
        return INVALID_ITEM_INDEX;
    }

    IListViewAdapterPtr pAdapter = pTableView->GetAdapter();

    if (NULL == pAdapter)
    {
        return INVALID_ITEM_INDEX;
    }


    // 非等高列表项
    int nTotalDataCount = pTableView->GetTotalDataCount();
    int nItemHeight = 0;
    int nSectionPos = 0;
    int nIndex = 0;

    for (; nIndex < nTotalDataCount; nIndex += m_nColumn)
    {
        if ((nIndex + m_nColumn) > nDataIndex)
        {
            return nSectionPos;
        }

        if (!pAdapter->GetItemHeight(pTableView, nIndex, nItemHeight))
        {
            return INVALID_ITEM_INDEX;
        }

        nSectionPos = nSectionPos + nItemHeight + m_nItemSpace;
    }

    return nSectionPos;
#else
    return 0;
#endif
}

#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
void xTableViewStyle::OnItemFocus(xListViewItem* pItem)
{
#if !IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
    xTableView* pTableView = GetTableView();

    if (NULL == pTableView || NULL == pItem)
    {
        return;
    }

    xScrollCtrlPtr pScrollCtrl = pTableView->GetScrollCtrl();

    if (NULL == pScrollCtrl || !pScrollCtrl->isVisible() || m_nColumn <= 0)
    {
        return;
    }

    pScrollCtrl->SetScrollPos(pItem->GetDataIndex() / m_nColumn);
#endif
}
#endif

#if IF_XTABLEVIEW_COLUMN_SPACE_BY_LINE
#if IF_XLISTVIEW_SUPPORT_TITLE
void xTableViewStyle::PaintTitle(xPainter& painter)
{
    if (LINE_TYPE_NONE != m_nSpaceLineStyle)
    {
        DrawTitleSpaceLine(painter);
    }
}
#endif

void xTableViewStyle::PaintContent(xPainter& painter)
{
    xTableView* pTableView = GetTableView();

    if (NULL == pTableView)
    {
        return;
    }

    if (LINE_TYPE_NONE == m_nSpaceLineStyle)
    {
#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
        DrawFocusItemBG(painter);
#endif
        return;
    }

    chRect rcTableView;
    chRect rcContent;

    if (!GetContainerGeometry(rcTableView) || !GetContentGeometry(rcContent))
    {
        return;
    }

    // 画Item之间的线
    int nColHeight = 0;
    int nRowWidth = 0;

    DrawItemSpaceLine(painter, rcTableView, rcContent, nRowWidth, nColHeight);

    // 画四周的线
    if (TAB_BOUND_LINE_TYPE_NONE != m_nBoundLineType && 0 != nColHeight && 0 != nRowWidth)
    {
        int nBoundRight = nRowWidth;
        int nBoundBottom = nColHeight;

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
        if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
        {
            nBoundRight = nColHeight;
            nBoundBottom = nRowWidth;
        }
#endif

        int nContentLeft = rcContent.left - rcTableView.left;
        int nContentTop = rcContent.top - rcTableView.top;

        nBoundRight -= rcTableView.left;
        nBoundBottom -= rcTableView.top;

        if (TAB_BOUND_LINE_TYPE_HORIZONTAL == (TAB_BOUND_LINE_TYPE_HORIZONTAL & m_nBoundLineType))
        {
            painter.setLineWidth(m_nItemSpace);

            painter.drawLineX(nContentTop, nContentLeft, nBoundRight, m_clrSpaceLine);
            painter.drawLineX(nBoundBottom, nContentLeft, nBoundRight, m_clrSpaceLine);
        }

        if (TAB_BOUND_LINE_TYPE_VERTICAL == (TAB_BOUND_LINE_TYPE_VERTICAL & m_nBoundLineType))
        {
            painter.setLineWidth(m_nColSpace);

            painter.drawLineY(nContentLeft, nContentTop, nBoundBottom, m_clrSpaceLine);
            painter.drawLineY(nBoundRight, nContentTop, nBoundBottom, m_clrSpaceLine);
        }
    }

#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
    DrawFocusItemBG(painter);
#endif
}

void xTableViewStyle::DrawItemSpaceLine(xPainter& painter, const chRect& rcTableView, const chRect& rcContent,
                                        int& nRowWidth, int& nColHeight)
{
    if (NULL == m_pListView)
    {
        return;
    }

    int nTotalItemCount = m_pListView->GetTotalItemCount();
    int i = 0;

    // 画每一行的线
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
    {
        painter.setLineWidth(m_nColSpace);
    }
    else
    {
#endif
        painter.setLineWidth(m_nItemSpace);
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    }
#endif

    for (; i < nTotalItemCount; i += m_nColumn)
    {
        xListViewItemPtr pItem = m_pListView->GetItemByIndex(i);

        if (NULL == pItem)
        {
            return;
        }

        chRect rcItem = pItem->geometry();

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
        if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
        {
            if (nColHeight < rcItem.right)
            {
                nColHeight = rcItem.right;
            }

            if (m_bSpaceLineIgnoreEnd && (i + m_nColumn) >= nTotalItemCount)
            {
                continue;
            }

            painter.drawLineY(rcItem.right - rcTableView.left, rcContent.top - rcTableView.top,
                              rcContent.bottom - rcTableView.top, m_clrSpaceLine);
            continue;
        }
#endif

        if (nColHeight < rcItem.bottom)
        {
            nColHeight = rcItem.bottom;
        }

        if (m_bSpaceLineIgnoreEnd && (i + m_nColumn) >= nTotalItemCount)
        {
            continue;
        }

        painter.drawLineX(rcItem.bottom - rcTableView.top, rcContent.left - rcTableView.left,
                          rcContent.right - rcTableView.left, m_clrSpaceLine);
    }

    // 画每一列的线
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
    {
        painter.setLineWidth(m_nItemSpace);
    }
    else
    {
#endif
        painter.setLineWidth(m_nColSpace);
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    }
#endif

    for (i = 0; i < m_nColumn; ++i)
    {
        xListViewItemPtr pItem = m_pListView->GetItemByIndex(i);

        if (NULL == pItem)
        {
            return;
        }

        chRect rcItem = pItem->geometry();

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
        if (TAB_COL_ORNT_HORIZONTAL == m_nOrientation)
        {
            if (nRowWidth < rcItem.bottom)
            {
                nRowWidth = rcItem.bottom;
            }

            if (m_bSpaceLineIgnoreEnd && (i + 1) >= m_nColumn)
            {
                continue;
            }

            painter.drawLineX(rcItem.bottom - rcTableView.top, rcContent.left - rcTableView.left,
                              nColHeight - rcTableView.left, m_clrSpaceLine);
            continue;
        }
#endif

        if (nRowWidth < rcItem.right)
        {
            nRowWidth = rcItem.right;
        }

        if (m_bSpaceLineIgnoreEnd && (i + 1) >= m_nColumn)
        {
            continue;
        }

        painter.drawLineY(rcItem.right - rcTableView.left, rcContent.top - rcTableView.top,
                          nColHeight - rcTableView.top, m_clrSpaceLine);
    }
}
#endif  // IF_XTABLEVIEW_COLUMN_SPACE_BY_LINE

#if IF_XLISTVIEW_SUPPORT_SLIDE
bool xTableViewStyle::ShiftContentPos(int nOffset, int& nDataIndex, bool bCheckBound)
{
    if (bCheckBound && m_nContentHeight <= m_nPageHeight)
    {
        return false;
    }

    int nItemTotalHeight = m_nItemHeight + m_nItemSpace;

    if (m_nColumn <= 0 || m_nItemHeight <= 0 || nItemTotalHeight <= 0)
    {
        return false;
    }

    // 一次最多只能滚动1页
    if (nOffset > m_nPageHeight)
    {
        nOffset = m_nPageHeight;
    }
    else if (nOffset < (-m_nPageHeight))
    {
        nOffset = -m_nPageHeight;
    }

    int nContentPos = m_nContentPos + nOffset;

    if (bCheckBound)
    {
        int nMaxContentPos = m_nContentHeight - nItemTotalHeight;

        if (nContentPos < 0)
        {
            nOffset = 0 - m_nContentPos;
            nContentPos = 0;
        }
        else if (nContentPos > nMaxContentPos)
        {
            nOffset = nMaxContentPos - m_nContentPos;
            nContentPos = nMaxContentPos;
        }
    }

    if (0 == nOffset)
    {
        return false;
    }

    int nRowIndex = nContentPos / nItemTotalHeight;

    nDataIndex = nRowIndex * m_nColumn;

    // Y轴坐标与nContentPos反向-(m_nContentPos - nRowIndex * nItemTotalHeight)
    m_nItemOffset = nRowIndex * nItemTotalHeight - nContentPos;

    if ((m_nItemOffset + m_nItemHeight) <= 0)
    {
        nDataIndex += m_nColumn;
        m_nItemOffset += nItemTotalHeight;
    }

    SetContentPos(nContentPos);

    return true;
}
#endif

#endif  // IF_TABLE_VIEW_ENABLE
