#include "xtableview.h"
#include "keymap.h"
#include "commonapi/stringhelperapi.h"
#include "dsklog/log.h"
#include "xWindowManager.h"
#include "modviewdump.h"
#if IF_XLISTVIEW_SUPPORT_SLIDE
#include "xtableslideview.h"
#endif


#if IF_TABLE_VIEW_ENABLE

EXPORT_VIEW_CLASS(xTableView);

xTableView::xTableView()
    : xListView(NULL, new xTableViewStyle(NULL))
{
    REGISTER_VIEW_DUMP_CLASS(xTableView)

    xTableViewStylePtr pStyle = GetTableViewStyle();

    if (NULL != pStyle)
    {
        pStyle->SetTableView(this);
    }
}

xTableView::~xTableView()
{
}

void xTableView::loadAttributes(xml_node& node)
{
    xFrameLayout::loadAttributes(node);

    int nListStyle = LIST_VIEW_STYLE_TYPE_TABLE_VIEW;

    PopupAndroidInt(node, XWINDOW_NAME_LIST_STYLE, nListStyle);

    SetStyleType(nListStyle);

    xTableViewStylePtr pStyle = GetTableViewStyle();

    if (NULL != pStyle)
    {
        pStyle->loadAttributes(node);
    }

#if IF_XLISTVIEW_SUPPORT_SLIDE
    bool bSlidable = false;

    PopupAndroidBool(node, XWINDOW_NAME_SLIDABLE, bSlidable);

    SetSlidable(bSlidable);
#endif
}

int xTableView::wrapContentWidth()
{
    xTableViewStylePtr pStyle = GetTableViewStyle();

    if (NULL == pStyle)
    {
        return 0;
    }

    int nContentWidth = 0;

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    if (xTableViewStyle::TAB_COL_ORNT_HORIZONTAL == pStyle->m_nOrientation)
    {
        int nColumn = pStyle->m_nColumn;

        if (nColumn <= 0)
        {
            return nContentWidth;
        }

        int nItemCount = GetTotalItemCount();
        int nRowCount = (nItemCount + nColumn - 1) / nColumn;

        if (nRowCount > 0)
        {
            nContentHeight = nRowCount * pStyle->m_nItemWidth + (nRowCount - 1) * pStyle->m_nColSpace;
        }
    }
    else
    {
#endif
        nContentWidth = pStyle->m_nColumn * pStyle->m_nItemWidth + (pStyle->m_nColumn - 1) * pStyle->m_nColSpace;
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    }
#endif

    if (nContentWidth <= 0)
    {
        return 0;
    }

    nContentWidth = pStyle->m_nItemLeftMargin + nContentWidth + pStyle->m_nItemRightMargin;

#if IF_XTABLEVIEW_COLUMN_SPACE_BY_LINE
    if (pStyle->IsVerticalBoundLine())
    {
        nContentWidth = nContentWidth + pStyle->m_nColSpace + pStyle->m_nColSpace;
    }
#endif

    return nContentWidth;
}

int xTableView::wrapContentHeight()
{
    xTableViewStylePtr pStyle = GetTableViewStyle();

    if (NULL == pStyle)
    {
        return 0;
    }

    int nContentHeight = 0;

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    if (xTableViewStyle::TAB_COL_ORNT_HORIZONTAL == pStyle->m_nOrientation)
    {
        nContentHeight = pStyle->m_nColumn * pStyle->m_nItemHeight + (pStyle->m_nColumn - 1) * pStyle->m_nItemSpace;
    }
    else
    {
#endif
        int nColumn = pStyle->m_nColumn;

        if (nColumn <= 0)
        {
            return nContentHeight;
        }

        int nItemCount = GetTotalItemCount();
        int nRowCount = (nItemCount + nColumn - 1) / nColumn;

        if (nRowCount > 0)
        {
            nContentHeight = nRowCount * pStyle->m_nItemHeight + (nRowCount - 1) * pStyle->m_nItemSpace;
        }
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    }
#endif

    if (nContentHeight <= 0)
    {
        return 0;
    }

    nContentHeight = pStyle->m_nItemTopMargin + nContentHeight + pStyle->m_nItemBottomMargin;

#if IF_XTABLEVIEW_COLUMN_SPACE_BY_LINE
    if (pStyle->IsHorizontalBoundLine())
    {
        nContentHeight = nContentHeight + pStyle->m_nItemSpace + pStyle->m_nItemSpace;
    }
#endif

    return nContentHeight;
}

bool xTableView::setFocus(int nFocusReason/* = FOCUS_REASON_DEFAULT*/)
{
    if (ProcessSetFocus(nFocusReason))
    {
        return true;
    }

    return xListView::setFocus(nFocusReason);
}

bool xTableView::ProcessSetFocus(int nFocusReason)
{
    int nTotalCount = GetTotalDataCount();

    if (nTotalCount <= 0)
    {
        return false;
    }

    xTableViewStylePtr pStyle = GetTableViewStyle();

    if (NULL == pStyle || pStyle->m_nColumn <= 0)
    {
        return false;
    }

    xListViewItemPtr pFocusItem = GetFocusItem();
    xListViewItemPtr pFirstItem = GetFirstItem();
    int nFocusDataIndex = INVALID_ITEM_INDEX;
    int nMaxDataIndex = nTotalCount - 1;

    if (NULL == pFirstItem)
    {
        return false;
    }

    if (NULL == pFocusItem)
    {
        if (FOCUS_REASON_RIGHT == nFocusReason || FOCUS_REASON_DOWN == nFocusReason)
        {
            nFocusDataIndex = 0;
        }
        else
        {
            nFocusDataIndex = nMaxDataIndex;
        }
    }
    else
    {
        nFocusDataIndex = pFocusItem->GetDataIndex();
    }

    if (INVALID_ITEM_INDEX == nFocusDataIndex)
    {
        return false;
    }

    if (FOCUS_REASON_LEFT == nFocusReason && !pStyle->IsRightMostItem(nFocusDataIndex, pFirstItem->GetDataIndex()))
    {
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
        if (xTableViewStyle::TAB_COL_ORNT_HORIZONTAL == pStyle->m_nOrientation)
        {
            nFocusDataIndex = pStyle->GetColLastDataIndex(nFocusDataIndex, nMaxDataIndex);
        }
        else
        {
#endif
            nFocusDataIndex = pStyle->GetRowLastDataIndex(nFocusDataIndex, nMaxDataIndex);
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
        }
#endif
    }
    else if (FOCUS_REASON_RIGHT == nFocusReason && !pStyle->IsLeftMostItem(nFocusDataIndex, pFirstItem->GetDataIndex()))
    {
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
        if (xTableViewStyle::TAB_COL_ORNT_HORIZONTAL == pStyle->m_nOrientation)
        {
            nFocusDataIndex = pStyle->GetColFirstDataIndex(nFocusDataIndex, nMaxDataIndex);
        }
        else
        {
#endif
            nFocusDataIndex = pStyle->GetRowFirstDataIndex(nFocusDataIndex, nMaxDataIndex);
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
        }
#endif
    }
    else if (FOCUS_REASON_UP == nFocusReason && !pStyle->IsBottomMostItem(nFocusDataIndex))
    {
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
        if (xTableViewStyle::TAB_COL_ORNT_HORIZONTAL == pStyle->m_nOrientation)
        {
            nFocusDataIndex = pStyle->GetRowLastDataIndex(nFocusDataIndex, nMaxDataIndex);
        }
        else
        {
#endif
            nFocusDataIndex = pStyle->GetColLastDataIndex(nFocusDataIndex, nMaxDataIndex);
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
        }
#endif
    }
    else if (FOCUS_REASON_DOWN == nFocusReason && !pStyle->IsTopMostItem(nFocusDataIndex))
    {
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
        if (xTableViewStyle::TAB_COL_ORNT_HORIZONTAL == pStyle->m_nOrientation)
        {
            nFocusDataIndex = pStyle->GetRowFirstDataIndex(nFocusDataIndex, nMaxDataIndex);
        }
        else
        {
#endif
            nFocusDataIndex = pStyle->GetColFirstDataIndex(nFocusDataIndex, nMaxDataIndex);
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
        }
#endif
    }
    else
    {
        return false;
    }

    if (!hasFocus() && !containFocus() && xWorkStation::checkViewFocusble(this))
    {
        ownerStation().setFocus_helper(this);
    }

    LocateAndFocusItem(nFocusDataIndex);

    return true;
}

#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
bool xTableView::onKeyPressedEvent(int key)
{
    key = GET_KEY_VALUE(key);

#if IF_MOVE_FOCUS_BY_KEY
    if (PHONE_KEY_LEFT == key)
    {
        return ProcessMoveFocus(FOCUS_REASON_LEFT);
    }
    else if (PHONE_KEY_RIGHT == key)
    {
        return ProcessMoveFocus(FOCUS_REASON_RIGHT);
    }
    else if (PHONE_KEY_UP == key)
    {
        return ProcessMoveFocus(FOCUS_REASON_UP);
    }
    else if (PHONE_KEY_DOWN == key)
    {
        return ProcessMoveFocus(FOCUS_REASON_DOWN);
    }
#endif

    return false;
}

bool xTableView::ProcessMoveFocus(int nFocusReason)
{
#if IF_XLISTVIEW_SUPPORT_SLIDE
    if (NULL != m_pSlideView && m_pSlideView->IsSliding())
    {
        return true;
    }
#endif
    xTableViewStylePtr pStyle = GetTableViewStyle();
    xListViewItemPtr pFocusItem = GetFocusItem();
    xListViewItemPtr pFirstItem = GetFirstItem();
    int nPageItemCount = GetPageItemCount();

    if (NULL == pStyle || NULL == pFirstItem || nPageItemCount <= 0)
    {
        return false;
    }

    int nFocusDataIndex = INVALID_ITEM_INDEX;
    int nFirstDataIndex = INVALID_ITEM_INDEX;

#if IF_XLISTVIEW_SUPPORT_SLIDE
    if (NULL == pFocusItem)
    {
        if (NULL != m_pSlideView)
        {
            m_pSlideView->CancelSlide();
            nFocusDataIndex = m_pSlideView->GetFocusDataIndex();
            nFirstDataIndex = nFocusDataIndex - nFocusDataIndex % nPageItemCount;
        }
    }
    else
#endif
    {
        // Item内部切换焦点
        if (pFocusItem->IsSwitchInsideFocus(nFocusReason))
        {
            return false;
        }

        nFocusDataIndex = pFocusItem->GetDataIndex();
        nFirstDataIndex = pFirstItem->GetDataIndex();
    }

    if (nFocusDataIndex < 0 || nFirstDataIndex < 0)
    {
        return false;
    }

    // 非最边缘的Item，直接切换焦点
    bool bMarginalItem = true;

    if (FOCUS_REASON_LEFT == nFocusReason)
    {
        bMarginalItem = pStyle->IsLeftMostItem(nFocusDataIndex, nFirstDataIndex);
    }
    else if (FOCUS_REASON_RIGHT == nFocusReason)
    {
        bMarginalItem = pStyle->IsRightMostItem(nFocusDataIndex, nFirstDataIndex);
    }
    else if (FOCUS_REASON_UP == nFocusReason)
    {
        bMarginalItem = pStyle->IsTopMostItem(nFocusDataIndex);
    }
    else if (FOCUS_REASON_DOWN == nFocusReason)
    {
        bMarginalItem = pStyle->IsBottomMostItem(nFocusDataIndex);
    }

    if (!bMarginalItem)
    {
#if IF_XLISTVIEW_SUPPORT_SLIDE
        if (NULL != m_pSlideView)
        {
            m_pSlideView->ResetFocusDataIndex();
        }
#endif
        nFocusDataIndex = pStyle->GetFocusDataIndex(nFocusDataIndex, nFocusReason);
        if (FOCUS_REASON_DOWN == nFocusReason)
        {
            int nTotalItemCount = GetTotalDataCount();
            if (nFocusDataIndex >= nTotalItemCount)
            {
                nFocusDataIndex = nTotalItemCount - 1;
            }
        }

        LocateAndFocusItem(nFocusDataIndex);
        return true;
    }

    // 界面内有向左切换焦点的需求
    if (ownerStation().SwitchWindowFocus(m_pRootView, this, nFocusReason))
    {
        return true;
    }

    // 界面间有切换焦点的需求
    if (NULL != m_pParent)
    {
        xViewEvent_int eventCustom(VIEW_FOCUS_CUSTOM, nFocusReason);

        if (xWorkStation::dispatchNonDirectionalViewKeyEvent(m_pParent, &eventCustom))
        {
            return true;
        }
    }

    // 循环切焦点
    nFocusDataIndex = pStyle->GetCycleFocusDataIndex(nFocusDataIndex, nFocusReason);

    LocateAndFocusItem(nFocusDataIndex);

    return true;
}
#endif

void xTableView::OnItemFocusChange(xListViewItemPtr pItem, bool bPreFocus)
{
    ChangeItemFocus(pItem, bPreFocus);

#if !IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
    xTableViewStyle* pStyle = GetTableViewStyle();

    if (NULL == pStyle || NULL == m_pScrollCtrl || NULL == pItem || !pItem->IsItemFocus())
    {
        return;
    }

    int nRowIndex = pItem->GetDataIndex();

    if (pStyle->m_nColumn > 0)
    {
        nRowIndex = nRowIndex / pStyle->m_nColumn;
    }

    m_pScrollCtrl->SetScrollPos(nRowIndex);
#endif
#endif
}

void xTableView::LocateAndFocusItem(int nDataIndex)
{
    xListViewItemPtr pFirstItem = GetFirstItem();
    xListViewItemPtr pLastItem = GetLastItem();

    if (NULL == pFirstItem || NULL == pLastItem)
    {
        return;
    }

    if (nDataIndex >= pFirstItem->GetDataIndex() && nDataIndex <= pLastItem->GetDataIndex())
    {
        FocusItemByDataIndex(nDataIndex);
        return;
    }

    xTableViewStylePtr pStyle = GetTableViewStyle();

    if (NULL == pStyle)
    {
        return;
    }

    int nTotalCount = GetTotalDataCount();
    int nPageItemCount = pStyle->GetPageItemCount();

    if (nTotalCount <= 0 || nDataIndex >= nTotalCount || pStyle->m_nColumn <= 0 || nPageItemCount <= 0)
    {
        XWINDOW_WARN("LocateAndFocusItem fail! TotalCount:%d DataIndex:%d Column:%d PageItemCount:%d",
                     nTotalCount, nDataIndex, pStyle->m_nColumn, nPageItemCount);
        return;
    }

    if (pStyle->IsScrollByPage())
    {
        int nPageDataIndex = nDataIndex - nDataIndex % nPageItemCount;

        pStyle->SetContentPos(pStyle->GetContentPosByIndex(nPageDataIndex));

        LoadListItem(nPageDataIndex);

        FocusItemByDataIndex(nDataIndex);

        return;
    }

    int nFirstIndex = pFirstItem->GetDataIndex();
    int nLastIndex = pLastItem->GetDataIndex();

    if (nDataIndex > nLastIndex)
    {
        nFirstIndex = (nDataIndex - nDataIndex % pStyle->m_nColumn) - (GetPageItemCount() - pStyle->m_nColumn);
    }
    else
    {
        nFirstIndex -= ((nFirstIndex - nDataIndex) / pStyle->m_nColumn + 1) * pStyle->m_nColumn;
    }

    if (nFirstIndex < 0)
    {
        nFirstIndex = 0;
    }

    int nPageBeginIndex = nFirstIndex;

    pStyle->SetContentPos(pStyle->GetContentPosByIndex(nPageBeginIndex));

    LoadListItem(nPageBeginIndex);
    FocusItemByDataIndex(nDataIndex);
}

xTableViewStylePtr xTableView::GetTableViewStyle() const
{
    return static_cast<xTableViewStylePtr>(m_pStyle);
}

xListViewItemPtr xTableView::GetGridItem(int nRow, int nCol)
{
    xTableViewStylePtr pStyle = GetTableViewStyle();

    if (NULL == pStyle)
    {
        return NULL;
    }

    if (nRow < 0 || nCol < 0 || nCol >= pStyle->m_nColumn)
    {
        return NULL;
    }

    int nIndex = nRow * pStyle->m_nColumn + nCol;

    return GetItemByIndex(nIndex);
}

#if !IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
void xTableView::OnDataCountChange()
{
    xTableViewStylePtr pStyle = GetTableViewStyle();

    if (NULL == pStyle)
    {
        return;
    }

    int nPageItemCount = pStyle->m_nPageItemCount;
    int nTotalCount = GetTotalDataCount();
    int nPageRow = nPageItemCount;
    int nTotalRow = nTotalCount;

    if (pStyle->m_nColumn > 0)
    {
        nPageRow = nPageItemCount / pStyle->m_nColumn;
        nTotalRow = (nTotalCount + pStyle->m_nColumn - 1) / pStyle->m_nColumn;
    }

    if (NULL != m_pScrollCtrl)
    {
        m_pScrollCtrl->SetScrollInfo(nPageRow, nTotalRow, 0);
    }
}

void xTableView::OnClearContent()
{
    xTableViewStylePtr pStyle = GetTableViewStyle();

    if (NULL == pStyle)
    {
        return;
    }

    int nPageRow = pStyle->m_nPageItemCount;

    if (pStyle->m_nColumn > 0)
    {
        nPageRow = nPageRow / pStyle->m_nColumn;
    }

    if (NULL != m_pScrollCtrl)
    {
        m_pScrollCtrl->SetScrollInfo(nPageRow, 0, 0);
    }

    if (NULL != m_pAdapter)
    {
        m_pAdapter->OnCleanContent(this);
    }
}
#endif

#if IF_XLISTVIEW_SUPPORT_SLIDE
xListSlideView* xTableView::AllocListSlideView()
{
    xTableSlideView* pSlideView = new xTableSlideView(this);

    return pSlideView;
}
#endif

EXPORT_STYLE_CLASS(xItemNoneBGStyle);
xItemNoneBGStyle::xItemNoneBGStyle()
{

}

xItemNoneBGStyle::~xItemNoneBGStyle()
{

}
#endif
