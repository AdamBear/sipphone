#include "keymap.h"
#include "xListView.h"
#include "xWindowManager.h"
#include "commonapi/stringhelperapi.h"
#include "dsklog/log.h"
#include "modviewdump.h"


EXPORT_VIEW_CLASS(xListView);

xListView::xListView(xScrollCtrlPtr pScrollCtrl/* = NULL*/, xListViewStylePtr pStyle/* = NULL*/)
    : m_pAdapter(NULL)
    , m_pHintView(NULL)
    , m_pScrollCtrl(pScrollCtrl)
#if IF_XLISTVIEW_SUPPORT_TITLE
    , m_pTitle(NULL)
#if IF_XLISTVIEW_SUPPORT_SLIDE
    , m_bScrollTitle(false)
#endif // IF_XLISTVIEW_SUPPORT_SLIDE
#endif // IF_XLISTVIEW_SUPPORT_TITLE
    , m_pStyle(pStyle)
    , m_nDataCount(0)
    , m_nFocusIndex(INVALID_ITEM_INDEX)
    , m_bAttachItem(false)
#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE
    , m_pFlateItem(NULL)
#endif
#if IF_XLISTVIEW_SUPPORT_SLIDE
    , m_pSlideView(NULL)
#endif
{
    REGISTER_VIEW_DUMP_CLASS(xListView)

#if IF_XLISTVIEW_FOCUS_INVERT_EFFECT
    setMask(true, false);
#endif
}

xListView::~xListView()
{
    if (NULL != m_pAdapter)
    {
        m_pAdapter->OnListViewDetach(this);
        m_pAdapter = NULL;
    }

    ReleaseStyle();

    ClearAllItem();

#if IF_XLISTVIEW_SUPPORT_TITLE
    ReleaseTitle();
#endif
}

void xListView::loadAttributes(xml_node& node)
{
    xFrameLayout::loadAttributes(node);

    int nListStyle = LIST_VIEW_STYLE_TYPE_DEFAULT;

    PopupAndroidInt(node, XWINDOW_NAME_LIST_STYLE, nListStyle);

    SetStyleType(nListStyle);

    if (NULL != m_pStyle)
    {
        m_pStyle->loadAttributes(node);
    }

#if IF_XLISTVIEW_SUPPORT_SLIDE
    bool bSlidable = false;

    PopupAndroidBool(node, XWINDOW_NAME_SLIDABLE, bSlidable);

    SetSlidable(bSlidable);
#endif
}

void xListView::loadChildrenElements(xml_node& node)
{
    xFrameLayout::loadChildrenElements(node);

    xView* pHintView = getViewById("hintNone");

    if (NULL != pHintView)
    {
        m_pHintView = static_cast<xTextView*>(pHintView);
        m_pHintView->hide();
    }

#if IF_XLISTVIEW_SUPPORT_TITLE
    xView* pTitle = getViewById("title");

    if (NULL != pTitle)
    {
        m_pTitle = static_cast<xListViewTitlePtr>(pTitle);
        m_pTitle->hide();
    }
#endif

    xView* pScrollCtrl = getViewById("scrollCtrl");

    if (NULL != pScrollCtrl)
    {
        m_pScrollCtrl = static_cast<xScrollCtrlPtr>(pScrollCtrl);
        m_pScrollCtrl->hide();
    }

#if XLIST_VIEW_DEBUG
    XWINDOW_INFO("ListView:%p HintView:%p ScrollCtrl:%p", this, pHintView, pScrollCtrl);
#endif
}

bool xListView::onRectChanged(const chRect& rectOld)
{
    if (rectOld == m_rectLayout || NULL == m_pStyle)
    {
        return false;
    }

    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();
    int nLeftMargin = 0;
    int nTopMargin = 0;
    int nWidth = 0;
    int nHeight = 0;

    for (int i = 0; iter != m_vecItem.end(); ++i, ++iter)
    {
        xListViewItemPtr pItem = *iter;

        if (NULL == pItem)
        {
            continue;
        }

        if (m_pStyle->GetItemLayoutInfo(i, pItem->GetDataIndex(), nLeftMargin, nTopMargin, nWidth, nHeight))
        {
            SetItemLayoutInfo(pItem, nLeftMargin, nTopMargin, nWidth, nHeight);
        }
    }

    return true;
}

void xListView::LayoutContentInRect(const chRect& rectFrame)
{
    if (NULL == m_pStyle)
    {
        return;
    }

#if IF_XLISTVIEW_SUPPORT_SLIDE
    // 滑动或动画过程中不进行重新布局
    if (NULL != m_pSlideView && m_pSlideView->m_bSliding)
    {
        xWorkStation::resetChildrenRelayout(this);

        return;
    }
#endif

    m_pStyle->Relayout();

#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE
    xListViewItemPtr pFlateItem = m_pFlateItem;
    chRect rcFlateItem;

    if (NULL != pFlateItem)
    {
        rcFlateItem = pFlateItem->geometry();
    }
#endif

    LayoutItems();

#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE
    if (NULL != pFlateItem)
    {
        m_pFlateItem = NULL;

        int nOriItemHeight = rcFlateItem.Height();
        int nNewItemHeight = pFlateItem->geometry().Height();

        if (nOriItemHeight < nNewItemHeight)
        {
            OnItemInflate(pFlateItem);
        }
#if IF_XLISTVIEW_SUPPORT_GROUP
        else if (nOriItemHeight > nNewItemHeight && m_pStyle->IsSectionDrift())
        {
            m_pStyle->SetSectionDrift(false);
            m_pStyle->ResetSectionDrift();
        }
#endif

        update(true);
    }
#endif
}

void xListView::LayoutItems()
{
    if (m_vecItem.size() <= 0)
    {
        return;
    }

    chRect rcContainer;

    if (NULL == m_pStyle || !m_pStyle->GetContainerGeometry(rcContainer))
    {
        return ;
    }

    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();
    int nContainerWidth = rcContainer.Width();
    int nContainerHeight = rcContainer.Height();

    for (int i = 0; iter != m_vecItem.end(); ++i, ++iter)
    {
        xListViewItemPtr pItem = *iter;

        if (NULL == pItem)
        {
            continue;
        }

        int nLayoutHeight = pItem->layout_height;

        if (wrap_content == nLayoutHeight)
        {
            nLayoutHeight = pItem->wrapFrameHeight();
        }

        pItem->layout_marginRight = nContainerWidth - pItem->layout_width - pItem->layout_marginLeft;
        pItem->layout_marginBottom = nContainerHeight - nLayoutHeight - pItem->layout_marginTop;

        pItem->LayoutInBoundRect(rcContainer);
    }
}

int xListView::wrapContentWidth()
{
    int nContentWidth = 0;

    if (NULL != m_pStyle)
    {
        nContentWidth = m_pStyle->m_nItemLeftMargin + m_pStyle->m_nItemWidth + m_pStyle->m_nItemRightMargin;
    }

    return nContentWidth;
}

int xListView::wrapContentHeight()
{
    int nContentHeight = 0;

    if (NULL == m_pStyle)
    {
        return nContentHeight;
    }


    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (; iter != m_vecItem.end(); ++iter)
    {
        xListViewItemPtr pTmpItem = *iter;

        if (NULL == pTmpItem)
        {
            continue;
        }

        if (LayoutParams::wrap_content == pTmpItem->layout_height)
        {
            nContentHeight += pTmpItem->wrapFrameHeight();
        }
        else
        {
            nContentHeight += pTmpItem->layout_height;
        }

        nContentHeight += m_pStyle->m_nItemSpace;
    }

#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
    if (m_pStyle->m_bSpaceLineIgnoreEnd)
    {
        nContentHeight -= m_pStyle->m_nItemSpace;
    }
#endif

#if IF_XLISTVIEW_SUPPORT_TITLE
    if (NULL != m_pTitle)
    {
        if (LayoutParams::wrap_content == m_pTitle->layout_height)
        {
            nContentHeight += m_pTitle->wrapFrameHeight();
        }
        else
        {
            nContentHeight += m_pTitle->layout_height;
        }

        nContentHeight += m_pStyle->m_nTitleSpace;
        nContentHeight += m_pStyle->m_nItemBottomMargin;
    }
    else if (nContentHeight > 0)
#else
    if (nContentHeight > 0)
#endif
    {
        nContentHeight = m_pStyle->m_nItemTopMargin + nContentHeight + m_pStyle->m_nItemBottomMargin;
    }

    return nContentHeight;
}

#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE
void xListView::RelayoutItem(int nBeginDataIndex, int nEndDataIndex,
                             const chRect& rcContent, int nYOffset)
{
    if (NULL == m_pStyle)
    {
        return;
    }

    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (int i = 0; iter != m_vecItem.end(); ++i, ++iter)
    {
        xListViewItemPtr pItem = *iter;

        if (NULL == pItem)
        {
            continue;
        }

        if (INVALID_ITEM_INDEX != nBeginDataIndex)
        {
            if (pItem->GetDataIndex() != nBeginDataIndex)
            {
                continue;
            }

            nBeginDataIndex = INVALID_ITEM_INDEX;
        }

        if (INVALID_ITEM_INDEX != nEndDataIndex)
        {
            if (pItem->GetDataIndex() == nEndDataIndex)
            {
                break;
            }
        }

        chRect rcItem = pItem->geometry();
        bool bReSize = false;

        if (wrap_content == layout_width && rcItem.Width() != m_pStyle->m_nItemWidth)
        {
            bReSize = true;
        }

        if (0 != nYOffset || bReSize)
        {
            rcItem.OffsetRect(0, nYOffset);

            if (bReSize)
            {
                rcItem.right = rcItem.left + m_pStyle->m_nItemWidth;
            }

#if XLIST_VIEW_DEBUG
            XWINDOW_INFO("Item(%d %d) Geometry:%d %d %d %d", i, pItem->GetDataIndex(),
                         rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
#endif

            pItem->LayoutInBoundRect(rcItem);
        }

        if (rcItem.top < rcContent.top || rcItem.bottom > rcContent.bottom)
        {
            ownerStation().hide_helper(pItem, false);
        }
        else if (!pItem->isVisible())
        {
            ownerStation().show_helper(pItem, false);
        }
    }
}

void xListView::OnItemInflate(xListViewItemPtr pItem)
{
    if (NULL == pItem || NULL == m_pStyle)
    {
        return;
    }

    chRect rcContent;

    if (!m_pStyle->GetContentGeometry(rcContent))
    {
        return;
    }

    chRect rcItem = pItem->geometry();
    int nYOffset = 0;

    // 计算出要向上偏移几个Item项，才能显示出扩大的Item项
    if (rcItem.bottom > rcContent.bottom)
    {
        VEC_LIST_ITEM::iterator iter = m_vecItem.begin();
        int nItemTop = -1;

        for (; iter != m_vecItem.end(); ++iter)
        {
            xListViewItemPtr pTmpItem = *iter;

            if (NULL == pTmpItem)
            {
                continue;
            }

            chRect rcTmpItem = pTmpItem->geometry();

            if (nItemTop < 0)
            {
                nItemTop = rcTmpItem.top;
                continue;
            }

            nYOffset += (nItemTop - rcTmpItem.top);

            if ((nYOffset + rcItem.bottom) <= rcContent.bottom)
            {
                break;
            }

            nItemTop = rcTmpItem.top;
        }
    }

#if IF_XLISTVIEW_SUPPORT_GROUP
    if (m_pStyle->IsSectionDrift() && 0 != nYOffset)
    {
        xListViewItemPtr pFirstItem = GetFirstItem();
        xListViewItemPtr pLastItem = GetLastItem();

        if (NULL != pFirstItem && NULL != pLastItem)
        {
            m_pStyle->DriftSection(pFirstItem->GetDataIndex(), pLastItem->GetDataIndex(), nYOffset);
        }
    }
#endif

    RelayoutItem(INVALID_ITEM_INDEX, INVALID_ITEM_INDEX, rcContent, nYOffset);
}

void xListView::SetFlateItem(xListViewItemPtr pItem)
{
    // LoadListItem时删除Item不处理
    if (NULL != pItem && !pItem->m_bAppear)
    {
        return;
    }

    m_pFlateItem = pItem;

    if (NULL == m_pFlateItem || NULL == m_pStyle)
    {
        return;
    }

    xListViewItemPtr pFirstItem = GetFirstItem();

    if (NULL == pFirstItem)
    {
        return;
    }

    int nDataIndex = pFirstItem->GetDataIndex();

    if (pFirstItem != m_pFlateItem
#if IF_XLISTVIEW_SUPPORT_SLIDE
            && (!m_pStyle->IsItemShowFully(m_pFlateItem))
#endif
       )
    {
        int nNextIndex = nDataIndex + 1;

        m_pStyle->AdjustContentPosAndIndex(nNextIndex);

        if ((nDataIndex + 1) == nNextIndex)
        {
            DeleteItem(0);

            return;
        }
    }

#if IF_XLISTVIEW_SUPPORT_GROUP
    // Section的Title是根据ContentPos来绘制的，有滑动的话需要调整回来
    if (m_pStyle->m_nSectionCount > 0 && !m_pStyle->IsSectionDrift())
    {
        m_pStyle->AdjustContentPosAndIndex(nDataIndex);
    }
#endif
}
#endif

#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
bool xListView::onFocusCustom(int nFocusReason)
{
    ownerStation().SwitchWindowFocus(m_pRootView, this, nFocusReason);

    return true;
}

bool xListView::onKeyPressedEvent(int key)
{
#if IF_XLISTVIEW_SUPPORT_TITLE
    if (NULL != m_pTitle && (m_pTitle->hasFocus() || m_pTitle->containFocus()))
    {
        return false;
    }
#endif

#if IF_XLISTVIEW_SUPPORT_SLIDE
    if (NULL != m_pSlideView)
    {
        m_pSlideView->CancelSlide();
    }
#endif

    key = GET_KEY_VALUE(key);

    if (PHONE_KEY_DOWN != key && PHONE_KEY_UP != key)
    {
        return false;
    }

    xListViewItemPtr pFocusItem = GetFocusItem();

#if IF_XLISTVIEW_SUPPORT_SLIDE
    if (NULL == pFocusItem && RelocateFocusItem())
    {
        pFocusItem = GetFocusItem();
    }
#endif

    if (NULL != pFocusItem && !pFocusItem->hasFocus() && !pFocusItem->containFocus())
    {
        return false;
    }

    if (PHONE_KEY_UP == key)
    {
#if IF_MOVE_FOCUS_BY_KEY
        if (NULL != pFocusItem && pFocusItem->IsSwitchInsideFocus(FOCUS_REASON_UP))
        {
            return false;
        }

        if (OnNotifyScrollPrevious(false))
        {
            return true;
        }

        if (ownerStation().SwitchWindowFocus(m_pRootView, this, FOCUS_REASON_UP))
        {
            return true;
        }

        // 循环回到最后一行
        setFocus(FOCUS_REASON_UP);

        return true;
#else
        OnNotifyScrollPrevious();
        return true;
#endif
    }
    else if (PHONE_KEY_DOWN == key)
    {
#if IF_MOVE_FOCUS_BY_KEY
        if (NULL != pFocusItem && pFocusItem->IsSwitchInsideFocus(FOCUS_REASON_DOWN))
        {
            return false;
        }

        if (OnNotifyScrollNext(false))
        {
            return true;
        }

        if (ownerStation().SwitchWindowFocus(m_pRootView, this, FOCUS_REASON_DOWN))
        {
            return true;
        }

        // 循环回到第一行
        setFocus(FOCUS_REASON_DOWN);

        return true;
#else
        OnNotifyScrollNext();
        return true;
#endif
    }

    return false;
}
#endif

bool xListView::onPaintContent(xPainter& p)
{
    if (NULL == m_pStyle)
    {
        return false;
    }

#if IF_XLISTVIEW_SUPPORT_TITLE
    m_pStyle->DrawTitleSpaceLine(p);
#endif

#if IF_XLISTVIEW_SUPPORT_SLIDE
    if (NULL == m_pSlideView)
    {
#endif
        m_pStyle->PaintContent(p);
#if IF_XLISTVIEW_SUPPORT_SLIDE
    }
#endif

    return true;
}

#if IF_XLISTVIEW_FOCUS_INVERT_EFFECT
bool xListView::onPaintMask(xPainter& p)
{
    if (NULL == m_pStyle)
    {
        return false;
    }

    m_pStyle->PaintMask(p);

    return true;
}
#endif

/*--------------------------------------Adapter--------------------------------------*/
void xListView::SetAdapter(IListViewAdapterPtr pAdapter, bool bLoadData/* = false*/)
{
    if (pAdapter != m_pAdapter)
    {
        if (NULL != m_pAdapter && m_pAdapter->IsListViewAttach(this))
        {
            m_pAdapter->OnListViewDetach(this);
        }

        m_pAdapter = pAdapter;

        if (NULL != m_pAdapter && !m_pAdapter->IsListViewAttach(this))
        {
            m_pAdapter->OnListViewAttach(this);
        }
    }

    if (bLoadData)
    {
        RefreshData(0);
    }
}

void xListView::RefreshData(int nDataIndex, int nContentPos/* = 0*/)
{
    if (NULL == m_pAdapter || NULL == m_pStyle)
    {
        return;
    }

    int nFocusIndex = m_nFocusIndex;

    // 清除m_bAttach，防止切换至非Attach场景导致内存泄漏
    if (m_bAttachItem)
    {
        ClearAllItem();
    }

    ResetListData();

    InitListViewLayout();

    if (0 == nContentPos && 0 != nDataIndex)
    {
        m_pStyle->AdjustContentPosAndIndex(nDataIndex);
    }
    else
    {
        m_pStyle->SetContentPos(nContentPos);
    }

    LoadListItem(nDataIndex, false);

    // 设置焦点
    if (IsAutoFocusItem() && INVALID_ITEM_INDEX == m_nFocusIndex)
    {
        if (nFocusIndex < 0 || nFocusIndex >= m_vecItem.size())
        {
            nFocusIndex = 0;
        }

        FocusItemByIndex(nFocusIndex);
    }

    update(true);
}

void xListView::ClearContent()
{
    ClearAllItem();

    ResetListData();

    OnClearContent();

    CheckScrollCtrlVisible();
    CheckHintVisible();
}
/*-----------------------------------------------------------------------------------*/

/*--------------------------------------xListView------------------------------------*/
void xListView::SetHint(const yl::string& strHint)
{
    xTextView* pHintView = GetHintView();

    if (NULL == pHintView)
    {
        return;
    }

    pHintView->SetText(strHint);
}

void xListView::SetHintFontColor(const xColor& color)
{
    xTextView* pHintView = GetHintView();

    if (NULL == pHintView)
    {
        return;
    }

    pHintView->SetTextColor(color);
}

void xListView::SetHintFontSize(int nFontSize)
{
    xTextView* pHintView = GetHintView();

    if (NULL == pHintView)
    {
        return;
    }

    xFont font = pHintView->GetFont();

    font.setPointSize(nFontSize);

    pHintView->SetFont(font);
}

void xListView::SetHintFixRect(const chRect& rcNote)
{
    xTextView* pHintView = GetHintView();

    if (NULL == pHintView)
    {
        return;
    }

    ownerStation().LayoutViewInBoundRect(pHintView, rcNote);

    if (NULL != m_pStyle)
    {
        m_pStyle->SetHintFixPos(true);
    }
}

void xListView::SetHintRect(const chRect& rcNote)
{
    if (NULL != m_pHintView)
    {
        m_pHintView->LayoutInBoundRect(rcNote);
    }
}

void xListView::CheckHintVisible()
{
    if (NULL == m_pHintView)
    {
        return;
    }

    if (m_pHintView->GetText().empty() || 0 != m_nDataCount)
    {
        m_pHintView->hide();
    }
    else
    {
        m_pHintView->show();
    }
}

bool xListView::setFocus(int nFocusReason/* = FOCUS_REASON_DEFAULT*/)
{
    if (FOCUS_REASON_DOWN == nFocusReason)
    {
        if (!hasFocus() && !containFocus() && xWorkStation::checkViewFocusble(this))
        {
            ownerStation().setFocus_helper(this);
        }

        xListViewItemPtr pItem = GetFirstItem();
        int nFocusDataIndex = 0;

#if IF_XLISTVIEW_CUSTOM_SWITCH_FOCUS
        if (NULL != m_pAdapter)
        {
            nFocusDataIndex = m_pAdapter->GetFocusDataIndex(nFocusDataIndex, true);
        }
#endif

        // *INDENT-OFF*
        if (m_nDataCount > 0 && (NULL == pItem || 0 != pItem->GetDataIndex()
#if IF_XLISTVIEW_SUPPORT_SLIDE
            || (NULL != m_pSlideView && pItem->m_rectLayout.top < m_pSlideView->m_rectLayout.top)
#endif
            ))// *INDENT-ON*
        {
            LocateItem(nFocusDataIndex);
        }

        FocusItemByDataIndex(nFocusDataIndex);

        return true;
    }
    else if (FOCUS_REASON_UP == nFocusReason)
    {
        if (!hasFocus() && !containFocus() && xWorkStation::checkViewFocusble(this))
        {
            ownerStation().setFocus_helper(this);
        }

        xListViewItemPtr pItem = GetLastItem();
        int nLastDataIndex = m_nDataCount - 1;
        int nFocusDataIndex = nLastDataIndex;

#if IF_XLISTVIEW_CUSTOM_SWITCH_FOCUS
        if (NULL != m_pAdapter)
        {
            nFocusDataIndex = m_pAdapter->GetFocusDataIndex(nFocusDataIndex, false);
        }
#endif

        // *INDENT-OFF*
        if (m_nDataCount > 0 && (NULL == pItem || nLastDataIndex != pItem->GetDataIndex()
#if IF_XLISTVIEW_SUPPORT_SLIDE
            || (NULL != m_pSlideView && pItem->m_rectLayout.bottom > m_pSlideView->m_rectLayout.bottom)
#endif
            ))// *INDENT-ON*
        {
            LocateItem(nFocusDataIndex);
        }

        FocusItemByDataIndex(nFocusDataIndex);

        return true;
    }

    xListViewItemPtr pFocusItem = GetFocusItem();

#if IF_XLISTVIEW_SUPPORT_SLIDE
    if (NULL == pFocusItem && RelocateFocusItem())
    {
        pFocusItem = GetFocusItem();
    }
#endif

    if (NULL != pFocusItem && xWorkStation::checkViewFocusble(pFocusItem))
    {
        if (!pFocusItem->hasFocus() && !pFocusItem->containFocus())
        {
            ownerStation().setFocus_helper(pFocusItem);
        }
        return true;
    }

    return xFrameLayout::setFocus(nFocusReason);
}

void xListView::LocateItem(int nDataIndex)
{
    if (NULL != m_pStyle)
    {
        m_pStyle->AdjustContentPosAndIndex(nDataIndex);
    }

    LoadListItem(nDataIndex, true);
}

xView* xListView::getDefaultFocusView()
{
    xListViewItemPtr pFirstItem = GetFirstItem();

    if (NULL != pFirstItem && xWorkStation::checkViewFocusble(pFirstItem))
    {
        return pFirstItem;
    }

    return xFrameLayout::getDefaultFocusView();
}

xTextView* xListView::GetHintView()
{
    if (NULL == m_pHintView)
    {
        m_pHintView = new xTextView;

        if (NULL == m_pHintView)
        {
            return NULL;
        }

        m_pHintView->SetAlign(gravity_x_center | gravity_y_center);

        addChildView(m_pHintView);

        m_pHintView->hide();
    }

    return m_pHintView;
}

void xListView::ResetAction()
{
    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (; iter != m_vecItem.end(); ++iter)
    {
        xListViewItemPtr pListItem = *iter;

        if (NULL != pListItem)
        {
            pListItem->SetAction(NULL);
        }
    }

#if IF_XLISTVIEW_SUPPORT_TITLE
    if (NULL != m_pTitle)
    {
        m_pTitle->SetAction(NULL);
    }
#endif
}
/*-----------------------------------------------------------------------------------*/

/*-------------------------------------ScrollCtrl------------------------------------*/
void xListView::SetScrollCtrlFixRect(const chRect& rcScrollCtrl)
{
    if (NULL == m_pScrollCtrl)
    {
        return;
    }

    m_pScrollCtrl->SetRect(rcScrollCtrl);

    if (NULL != m_pStyle)
    {
        m_pStyle->SetScrollCtrlFixPos(true);
    }
}

void xListView::CheckScrollCtrlVisible()
{
    if (NULL == m_pScrollCtrl || NULL == m_pStyle)
    {
        return;
    }

#if IF_XLISTVIEW_SCROLL_BAR_DENOTE_ITEM_COUNT
    if (m_nDataCount > 1)
#else
    if (m_pStyle->m_nContentHeight > m_pStyle->m_nPageHeight && m_pStyle->m_nPageHeight > 0)
#endif
    {
        m_pScrollCtrl->show();
    }
    else
    {
        m_pScrollCtrl->hide();
    }
}

bool xListView::OnNotifyScrollPrevious(bool bCycle/* = true*/)
{
    if (NULL == m_pStyle)
    {
        return false;
    }

    if (m_pStyle->IsScrollByItem())
    {
        return m_pStyle->OnPreItem(m_pStyle->IsScrollByPage(), bCycle);
    }
    else if (m_pStyle->IsScrollByPage())
    {
        return m_pStyle->OnPrePage(bCycle);
    }

    return false;
}

bool xListView::OnNotifyScrollNext(bool bCycle/* = true*/)
{
    if (NULL == m_pStyle)
    {
        return false;
    }

    if (m_pStyle->IsScrollByItem())
    {
        return m_pStyle->OnNextItem(m_pStyle->IsScrollByPage(), bCycle);
    }
    else if (m_pStyle->IsScrollByPage())
    {
        return m_pStyle->OnNextPage(bCycle);
    }

    return false;
}

/*-----------------------------------------------------------------------------------*/

/*----------------------------------------Title--------------------------------------*/
#if IF_XLISTVIEW_SUPPORT_TITLE
void xListView::SetTitle(xListViewTitlePtr pTitle)
{
    if (pTitle == m_pTitle)
    {
        return;
    }

    SetRedrawRelayoutContent();

    ReleaseTitle();

    m_pTitle = pTitle;

    if (NULL != m_pTitle)
    {
        addChildView(m_pTitle);

        m_pTitle->SetListView(this);
    }

    if (NULL != m_pTitle)
    {
        m_pTitle->show();
    }
}

void xListView::ReleaseTitle()
{
    xListViewTitlePtr pTitle = m_pTitle;

    if (NULL == pTitle)
    {
        return;
    }

    m_pTitle = NULL;

    pTitle->SetAction(NULL);
    pTitle->SetListView(NULL);

    pTitle->close();
}
#endif
/*-----------------------------------------------------------------------------------*/

/*----------------------------------------Style--------------------------------------*/
void xListView::SetStyleType(int nStyleType)
{
    ReleaseStyle();

    m_pStyle = currentStyleFactory.CreateListStyle(nStyleType, this);
}

void xListView::ReleaseStyle()
{
    if (NULL != m_pStyle)
    {
        delete m_pStyle;
        m_pStyle = NULL;
    }
}
/*-----------------------------------------------------------------------------------*/

/*----------------------------------------Item---------------------------------------*/
void xListView::SetPageItemCount(int nPageCount)
{
    if (NULL != m_pStyle)
    {
        m_pStyle->SetPageItemCount(nPageCount);
    }
}

int xListView::GetPageItemCount()
{
    if (NULL != m_pStyle)
    {
        return m_pStyle->GetPageItemCount();
    }

    return 0;
}

#if IF_XLISTVIEW_SHOW_ITEMS_BY_PAGE
void xListView::SwitchFocusToNextItem(bool bCycle/* = true*/)
{
    if (NULL != m_pStyle && m_pStyle->IsItemFocusable())
    {
        m_pStyle->OnNextItem(m_pStyle->IsScrollByPage(), bCycle);
    }
}

void xListView::SwitchFocusToPreItem(bool bCycle/* = true*/)
{
    if (NULL != m_pStyle && m_pStyle->IsItemFocusable())
    {
        m_pStyle->OnPreItem(m_pStyle->IsScrollByPage(), bCycle);
    }
}

void xListView::SwitchFocusToNextPage(bool bCycle/* = true*/)
{
    if (NULL == m_pStyle || !m_pStyle->IsItemFocusable())
    {
        return;
    }

    if (!m_pStyle->IsScrollByPage())
    {
        m_pStyle->OnNextFullPage(bCycle);
    }
    else
    {
        int nFocusIndex = m_nFocusIndex;

        if (m_pStyle->OnNextPage(bCycle) && INVALID_ITEM_INDEX != nFocusIndex)
        {
            int nTotalItemCount = GetTotalItemCount();

            if (nFocusIndex >= nTotalItemCount)
            {
                nFocusIndex = nTotalItemCount - 1;
            }

            FocusItemByIndex(nFocusIndex);
        }
    }
}

void xListView::SwitchFocusToPrePage(bool bCycle/* = true*/)
{
    if (NULL == m_pStyle || !m_pStyle->IsItemFocusable())
    {
        return;
    }

    if (!m_pStyle->IsScrollByPage())
    {
        m_pStyle->OnPreFullPage(bCycle);
    }
    else
    {
        int nFocusIndex = m_nFocusIndex;

        if (m_pStyle->OnPrePage(bCycle) && INVALID_ITEM_INDEX != nFocusIndex)
        {
            int nTotalItemCount = GetTotalItemCount();

            if (nFocusIndex >= nTotalItemCount)
            {
                nFocusIndex = nTotalItemCount - 1;
            }

            FocusItemByIndex(nFocusIndex);
        }
    }
}

void xListView::SwitchToNextPage(bool bCycle/* = true*/)
{
    if (NULL == m_pStyle || !m_pStyle->IsScrollByPage())
    {
        return;
    }

    m_pStyle->OnNextPage(bCycle);

#if !IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
    UpdateScrollCtrlByPage();
#endif
}

void xListView::SwitchToPrePage(bool bCycle/* = true*/)
{
    if (NULL == m_pStyle || !m_pStyle->IsScrollByPage())
    {
        return;
    }

    m_pStyle->OnPrePage(bCycle);

#if !IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
    UpdateScrollCtrlByPage();
#endif
}

#if !IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
void xListView::UpdateScrollCtrlByPage()
{
    if (NULL == m_pStyle || NULL == m_pScrollCtrl)
    {
        return;
    }

    xListViewItemPtr pFirstItem = GetFirstItem();
    int nPageDataIndex = 0;;

    if (NULL != pFirstItem)
    {
        nPageDataIndex = pFirstItem->GetDataIndex();
    }

    if (m_pStyle->IsScrollByItem())
    {
        m_pScrollCtrl->SetScrollPos(nPageDataIndex);
    }
    else
    {
        int nPageNo = 0;

        if (m_pStyle->m_nPageItemCount > 0)
        {
            nPageNo = nPageDataIndex / m_pStyle->m_nPageItemCount;
        }

        m_pScrollCtrl->SetScrollPos(nPageNo);
    }
}
#endif
#endif

void xListView::SetTotalDataCount(int nTotalCount)
{
    bool bNotify = false;

    if (m_nDataCount != nTotalCount)
    {
        bNotify = true;
    }

    m_nDataCount = nTotalCount;

    if (bNotify)
    {
        OnDataCountChange();
    }
}

void xListView::SelectItemByIndex(int nIndex)
{
    if (INVALID_ITEM_INDEX == nIndex)
    {
        DefocusItem();
        return;
    }

    xListViewItemPtr pItem = GetItemByIndex(nIndex);

    if (NULL == pItem)
    {
        return;
    }

    DefocusItem();

    m_nFocusIndex = nIndex;
    pItem->SelectItem(true);
}

void xListView::FocusItemByIndex(int nIndex)
{
    if (INVALID_ITEM_INDEX == nIndex)
    {
        DefocusItem();

        return;
    }

    xListViewItemPtr pItem = GetItemByIndex(nIndex);

    if (NULL == pItem)
    {
        return;
    }

    DefocusItem();

    m_nFocusIndex = nIndex;
    pItem->FocusItem(true);
}

void xListView::FocusLastItem()
{
    int nSize = m_vecItem.size();
    int nLastIndex = INVALID_ITEM_INDEX;

    if (nSize > 0)
    {
        nLastIndex = nSize - 1;
    }

    FocusItemByIndex(nLastIndex);
}

void xListView::SetItemFocusByIndex(int nIndex)
{
    if (INVALID_ITEM_INDEX == nIndex)
    {
        DefocusItem();
        return;
    }

    xListViewItemPtr pItem = GetItemByIndex(nIndex);

    if (NULL == pItem)
    {
        return;
    }

    DefocusItem();

    m_nFocusIndex = nIndex;
    pItem->SetItemFocus(true);
}

int xListView::GetFocusItemDataIndex()
{
    xListViewItemPtr pItem = GetItemByIndex(m_nFocusIndex);

    if (NULL != pItem)
    {
        return pItem->GetDataIndex();
    }

    return INVALID_ITEM_INDEX;
}

void xListView::SelectItemByDataIndex(int nIndex)
{
    if (INVALID_ITEM_INDEX == nIndex)
    {
        DefocusItem();
#if IF_XLISTVIEW_SUPPORT_SLIDE
        if (NULL != m_pSlideView)
        {
            m_pSlideView->ResetFocusDataIndex();
        }
#endif
        return;
    }

    xListViewItemPtr pItem = GetItemByDataIndex(nIndex);

    if (NULL == pItem)
    {
        return;
    }

    DefocusItem();

    m_nFocusIndex = GetIndexByItem(pItem);
    pItem->SelectItem(true);
}

void xListView::FocusItemByDataIndex(int nIndex)
{
    if (INVALID_ITEM_INDEX == nIndex)
    {
        DefocusItem();
#if IF_XLISTVIEW_SUPPORT_SLIDE
        if (NULL != m_pSlideView)
        {
            m_pSlideView->ResetFocusDataIndex();
        }
#endif
        return;
    }

    xListViewItemPtr pItem = GetItemByDataIndex(nIndex);

    if (NULL == pItem)
    {
        return;
    }

    DefocusItem();

    m_nFocusIndex = GetIndexByItem(pItem);
    pItem->FocusItem(true);
}

void xListView::SetItemAttach()
{
    // 清除原来非attach的item，防止内存泄漏
    ClearAllItem();

    m_bAttachItem = true;
}

bool xListView::AddItem(xListViewItemPtr pItem)
{
    if (NULL == pItem)
    {
        return false;
    }

    int nSize = m_vecItem.size();

    pItem->SetListView(this);

#if IF_XLISTVIEW_SUPPORT_SLIDE
    if (NULL != m_pSlideView)
    {
        m_pSlideView->addChildView(pItem);
    }
    else
    {
        addChildView(pItem);
    }
#else
    addChildView(pItem);
#endif

    pItem->show();

    m_vecItem.push_back(pItem);

    if (0 == nSize)
    {
        OnDataIndexChange(pItem);
    }

    OnAddItem(pItem);

    return true;
}

void xListView::ClearAllItem()
{
    SetRedrawRelayoutContent();

    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (; iter != m_vecItem.end(); ++iter)
    {
        xListViewItemPtr pItem = *iter;

        if (NULL == pItem)
        {
            continue;
        }

        pItem->hide(true);

        if (m_bAttachItem)
        {
#if IF_XLISTVIEW_SUPPORT_SLIDE
            if (NULL != m_pSlideView)
            {
                m_pSlideView->popupChildView(pItem);
            }
            else
            {
                popupChildView(pItem);
            }
#else
            popupChildView(pItem);
#endif

            pItem->SetListView(NULL);
            pItem->SetAction(NULL);
        }
        else
        {
            pItem->Release();
        }
    }

    m_vecItem.clear();
    m_bAttachItem = false;

    OnClearAllItem();
}

bool xListView::DelItemByIndex(int nIndex)
{
    if (!DeleteItem(nIndex))
    {
        return false;
    }

    if (0 == m_vecItem.size())
    {
        m_nFocusIndex = INVALID_ITEM_INDEX;
        OnClearContent();
    }
    else if (nIndex <= m_nFocusIndex)
    {
        if (IsAutoFocusItem())
        {
            // 重新选中一行
            int nFocusIndex = m_nFocusIndex - 1;

            if (nFocusIndex < 0)
            {
                nFocusIndex = 0;
            }

            FocusItemByIndex(nFocusIndex);
        }
        else
        {
            m_nFocusIndex = INVALID_ITEM_INDEX;
        }
    }

    return true;
}

bool xListView::DeleteItem(int nIndex)
{
    int nSize = m_vecItem.size();

    if (nIndex < 0 || nIndex >= nSize)
    {
        return false;
    }

    xListViewItemPtr pItem = m_vecItem[nIndex];

    if (NULL != pItem)
    {
        pItem->hide(true);

        if (m_bAttachItem)
        {
#if IF_XLISTVIEW_SUPPORT_SLIDE
            if (NULL != m_pSlideView)
            {
                m_pSlideView->popupChildView(pItem);
            }
            else
            {
                popupChildView(pItem);
            }
#else
            popupChildView(pItem);
#endif

            pItem->SetListView(NULL);
            pItem->SetAction(NULL);
        }
        else
        {
            pItem->Release();
        }
    }

    m_vecItem.removeAt(nIndex);

    if (m_vecItem.size() <= 0 && m_bAttachItem)
    {
        m_bAttachItem = false;
    }

    OnDeleteItem(pItem);

    return true;
}

bool xListView::DelItemByDataIndex(int nIndex)
{
    int nDelIndex = GetIndexByDataIndex(nIndex);

    return DelItemByIndex(nDelIndex);
}

xListViewItemPtr xListView::GetItemByIndex(int nIndex)
{
    if (INVALID_ITEM_INDEX == nIndex)
    {
        return NULL;
    }

    int nSize = m_vecItem.size();

    if (nIndex < 0 || nIndex >= nSize)
    {
        return NULL;
    }

    return m_vecItem[nIndex];
}

xListViewItemPtr xListView::GetItemByDataIndex(int nIndex)
{
    if (INVALID_ITEM_INDEX == nIndex)
    {
        return NULL;
    }

    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (; iter != m_vecItem.end(); ++iter)
    {
        xListViewItemPtr pItem = *iter;

        if (NULL != pItem && pItem->GetDataIndex() == nIndex)
        {
            return pItem;
        }
    }

    return NULL;
}

xListViewItemPtr xListView::GetFocusItem()
{
    if (INVALID_ITEM_INDEX == m_nFocusIndex)
    {
        return NULL;
    }

    return GetItemByIndex(m_nFocusIndex);
}

xListViewItemPtr xListView::GetFirstItem()
{
    if (m_vecItem.size() <= 0)
    {
        return NULL;
    }

    return m_vecItem[0];
}

xListViewItemPtr xListView::GetLastItem()
{
    int nSize = m_vecItem.size();

    if (nSize <= 0)
    {
        return NULL;
    }

    return m_vecItem[nSize - 1];
}

int xListView::GetIndexByItem(xListViewItemPtr pItem)
{
    if (NULL == pItem)
    {
        return INVALID_ITEM_INDEX;
    }

    int nIndex = INVALID_ITEM_INDEX;
    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (int i = 0; iter != m_vecItem.end(); ++i, ++iter)
    {
        xListViewItemPtr pTmpItem = *iter;

        if (pTmpItem == pItem)
        {
            nIndex = i;
            break;
        }
    }

    return nIndex;
}

int xListView::GetIndexByDataIndex(int nDataIndex)
{
    int nIndex = INVALID_ITEM_INDEX;
    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (int i = 0; iter != m_vecItem.end(); ++i, ++iter)
    {
        xListViewItemPtr pTmpItem = *iter;

        if (NULL != pTmpItem && pTmpItem->GetDataIndex() == nDataIndex)
        {
            nIndex = i;
            break;
        }
    }

    return nIndex;
}

int xListView::GetPageDataIndex()
{
    xListViewItemPtr pFirstItem = GetFirstItem();

    if (NULL != pFirstItem)
    {
        return pFirstItem->GetDataIndex();
    }

    return INVALID_ITEM_INDEX;
}

bool xListView::IsCanReuseItem(int nItemType)
{
    if (0 == m_vecItem.size())
    {
        return true;
    }

    xListViewItemPtr pItem = m_vecItem[0];

    if (NULL != pItem && pItem->GetType() == nItemType)
    {
        return true;
    }

    return false;
}

bool xListView::IsAllSameItem(int nItemType)
{
    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (; iter != m_vecItem.end(); ++iter)
    {
        xListViewItemPtr pItem = *iter;

        if (NULL == pItem || nItemType != pItem->GetType())
        {
            return false;
        }
    }

    return true;
}

bool xListView::IsHadConflictItem(int nItemType, int nMaskType)
{
    if (0 == m_vecItem.size())
    {
        return false;
    }

    xListViewItemPtr pItem = m_vecItem[0];

    if (NULL != pItem && (pItem->GetType() & nMaskType) == nItemType)
    {
        return false;
    }

    return true;
}

#if IF_XLISTVIEW_SUPPORT_SLIDE
bool xListView::IsItemShowFully(int nIndex)
{
    if (nIndex < 0 || nIndex >= m_vecItem.size())
    {
        return false;
    }

    xListViewItemPtr pItem = m_vecItem[nIndex];

    if (NULL == m_pStyle)
    {
        return false;
    }

    return m_pStyle->IsItemShowFully(pItem);
}
#endif

void xListView::OnDataIndexChange(xListViewItemPtr pItem)
{

}

void xListView::OnItemFocusChange(xListViewItemPtr pItem, bool bPreFocus)
{
    ChangeItemFocus(pItem, bPreFocus);

#if !IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
    if (NULL == m_pScrollCtrl || NULL == pItem || !pItem->IsItemFocus())
    {
        return;
    }

    m_pScrollCtrl->SetScrollPos(pItem->GetDataIndex());
#endif
#endif
}

void xListView::ResetListData()
{
#if IF_XLISTVIEW_SUPPORT_SLIDE
    if (NULL != m_pSlideView)
    {
        m_pSlideView->CancelSlide();
        m_pSlideView->ResetFocusDataIndex();
    }
#endif

    m_nFocusIndex = INVALID_ITEM_INDEX;
    m_nDataCount = 0;
}

void xListView::InitListViewLayout()
{
    if (NULL == m_pStyle || NULL == m_pAdapter)
    {
        return;
    }

    m_pAdapter->InitListViewLayout(this);

#if IF_XLISTVIEW_SUPPORT_GROUP
    m_pStyle->InitGroupSection();
#endif

    m_pStyle->OnFinishInit();

    CheckScrollCtrlVisible();
    CheckHintVisible();
}

void xListView::LoadListItem(int nDataIndex, bool bCheckIndex/* = true*/)
{
    // 有超过一页的数据，建议使用Adapter；不支持无Adapter而有多页的场景；
    if (NULL == m_pStyle || NULL == m_pAdapter)
    {
        return;
    }

    SetRedrawRelayoutContent();
#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE && IF_XLISTVIEW_SUPPORT_GROUP
    m_pStyle->SetSectionDrift(false);
#endif

    if (m_bAttachItem)
    {
        ClearAllItem();

        m_bAttachItem = true;
    }

    if (bCheckIndex && !m_pAdapter->PrecheckDataIndex(nDataIndex))
    {
        ClearContent();

        InitListViewLayout();
    }

    xListViewItemPtr pPrepareItem = NULL;
    int nItemLeftMargin = 0;
    int nItemTopMargin = 0;
    int nItemWidth = 0;
    int nItemHeight = 0;

    int nSize = m_vecItem.size();
    int i = 0;

    // 重用原有Item，碰到第一个不能重用的Item时结束重用
    for (; i < nSize && nDataIndex < m_nDataCount; ++i, ++nDataIndex)
    {
        xListViewItemPtr pCurrentItem = m_vecItem[i];

        if (NULL == pCurrentItem)
        {
            XWINDOW_ERR("Item is Empty in LoadListItem");
            continue;
        }

        if (!m_pStyle->GetItemLayoutInfo(i, nDataIndex, nItemLeftMargin, nItemTopMargin, nItemWidth, nItemHeight))
        {
            break;
        }

#if XLIST_VIEW_DEBUG
        XWINDOW_INFO("Item(%d %d) LeftMargin:%d TopMargin:%d Width:%d Height:%d",
            i, nDataIndex, nItemLeftMargin, nItemTopMargin, nItemWidth, nItemHeight);
#endif

        xListViewItemPtr pNewItem = m_pAdapter->PrepareItem(this, nDataIndex, pCurrentItem);

        if (NULL == pNewItem)
        {
            break;
        }

        if (pCurrentItem != pNewItem)
        {
            pPrepareItem = pNewItem;
#if XLIST_VIEW_DEBUG
            XWINDOW_INFO("New Item:%p Index:%d", pNewItem, nDataIndex);
#endif
            break;
        }

        pNewItem->SetDataIndex(nDataIndex);
        m_pAdapter->RefreshItem(this, pNewItem);

        if (nItemLeftMargin != pNewItem->layout_marginLeft || nItemTopMargin != pNewItem->layout_marginTop
            || nItemWidth != pNewItem->layout_width || nItemHeight != pNewItem->layout_height)
        {
            onChildrenLayoutChanged();
        }
    }

    // 没有新Item需要添加
    if (NULL == pPrepareItem && i < nSize)
    {
        for (int j = i; j < nSize; ++j)
        {
            DelItemByIndex(i);
        }

        return;
    }

    // 有新类型的Item
    if (NULL != pPrepareItem)
    {
        // 删除剩下的Items
        for (int j = i; j < nSize; ++j)
        {
            DeleteItem(i);
        }

        if (LoadItem(pPrepareItem, nDataIndex, nItemLeftMargin, nItemTopMargin, nItemWidth, nItemHeight))
        {
            ++i;
            ++nDataIndex;
        }
    }

    for (; nDataIndex < m_nDataCount; ++i, ++nDataIndex)
    {
        if (!m_pStyle->GetItemLayoutInfo(i, nDataIndex, nItemLeftMargin, nItemTopMargin, nItemWidth, nItemHeight))
        {
            break;
        }

#if XLIST_VIEW_DEBUG
        XWINDOW_INFO("Item(%d %d) Geometry:%d %d %d %d", i, nDataIndex, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
#endif

        xListViewItemPtr pNewItem = m_pAdapter->PrepareItem(this, nDataIndex, NULL);

#if XLIST_VIEW_DEBUG
        XWINDOW_INFO("New Item:%p Index:%d", pNewItem, nDataIndex);
#endif

        if (!LoadItem(pNewItem, nDataIndex, nItemLeftMargin, nItemTopMargin, nItemWidth, nItemHeight))
        {
            break;
        }
    }
}

bool xListView::LoadItem(xListViewItemPtr pItem, int nDataIndex,
    int nLeftMargin, int nTopMargin,
    int nWidth, int nHeight)
{
    if (NULL == pItem || NULL == m_pAdapter || NULL == m_pStyle)
    {
        return false;
    }

    pItem->SetDataIndex(nDataIndex);

    SetItemLayoutInfo(pItem, nLeftMargin, nTopMargin, nWidth, nHeight);

    m_pAdapter->RefreshItem(this, pItem);

    AddItem(pItem);

    return true;
}

void xListView::SetItemLayoutInfo(xListViewItemPtr pItem,
    int nLeftMargin, int nTopMargin,
    int nWidth, int nHeight)
{
    if (NULL == pItem)
    {
        return;
    }

    pItem->layout_marginLeft = nLeftMargin;
    pItem->layout_marginTop = nTopMargin;
    pItem->layout_width = nWidth;

    if (wrap_content != pItem->layout_height)
    {
        pItem->layout_height = nHeight;
    }
}

bool xListView::IsAutoFocusItem()
{
    if (NULL != m_pStyle)
    {
        return m_pStyle->IsAutoFocusItem();
    }

    return false;
}

bool xListView::IsItemFocusable()
{
    if (NULL != m_pStyle)
    {
        return m_pStyle->IsItemFocusable();
    }

    return false;
}

void xListView::DefocusItem()
{
    if (INVALID_ITEM_INDEX == m_nFocusIndex)
    {
        return;
    }

    xListViewItemPtr pFocusItem = GetItemByIndex(m_nFocusIndex);

    if (NULL != pFocusItem)
    {
        pFocusItem->SetItemFocus(false);
    }

    m_nFocusIndex = INVALID_ITEM_INDEX;
}

void xListView::ChangeItemFocus(xListViewItemPtr pItem, bool bPreFocus)
{
    if (NULL == pItem)
    {
        return;
    }

    xListViewItemPtr pFocusItem = GetItemByIndex(m_nFocusIndex);

    // 防止递归死循环，只处理获得焦点状态
    if (!pItem->IsItemFocus())
    {
        if (NULL != pFocusItem && !pFocusItem->IsItemFocus())
        {
            m_nFocusIndex = INVALID_ITEM_INDEX;
        }
        return;
    }

    if ((INVALID_ITEM_INDEX == m_nFocusIndex && NULL == pFocusItem)
            || (NULL != pFocusItem && pFocusItem != pItem))
    {
        m_nFocusIndex = GetIndexByItem(pItem);
    }

    int nSize = m_vecItem.size();

    for (int i = 0; i < nSize; ++i)
    {
        xListViewItemPtr pTmpItem = m_vecItem[i];

        if (NULL == pTmpItem || pTmpItem == pItem)
        {
            continue;
        }

        pTmpItem->FocusItem(false);
    }
}

void xListView::OnAddItem(xListViewItemPtr pItem)
{

}

void xListView::OnDeleteItem(xListViewItemPtr pItem)
{
#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE
    if (m_pFlateItem == pItem)
    {
        m_pFlateItem = NULL;
    }
#endif
}

void xListView::OnClearAllItem()
{
#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE
    m_pFlateItem = NULL;
#endif
}

void xListView::OnDataCountChange()
{
#if !IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
    if (NULL != m_pScrollCtrl && NULL != m_pStyle)
    {
#if IF_XLISTVIEW_SHOW_ITEMS_BY_PAGE
        if (m_pStyle->IsScrollByItem())
        {
            m_pScrollCtrl->SetScrollInfo(m_pStyle->m_nPageItemCount, m_nDataCount, 0);
        }
        else
        {
            int nTotalPage = 0;

            if (m_pStyle->m_nPageItemCount > 0)
            {
                nTotalPage = (m_nDataCount + m_pStyle->m_nPageItemCount - 1) / m_pStyle->m_nPageItemCount;
            }

            m_pScrollCtrl->SetScrollInfo(1, nTotalPage, 0);
        }
#else
#if IF_XLISTVIEW_SCROLL_BAR_DENOTE_ITEM_COUNT
        m_pScrollCtrl->SetScrollInfo(1, m_nDataCount, 0);
#else
        m_pScrollCtrl->SetScrollInfo(m_pStyle->m_nPageItemCount, m_nDataCount, 0);
#endif
#endif
    }
#endif
}

void xListView::OnClearContent()
{
    if (NULL != m_pScrollCtrl && NULL != m_pStyle)
    {
#if IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
        m_pScrollCtrl->SetScrollInfo(m_pStyle->m_nPageHeight, 0, 0);
#else
#if IF_XLISTVIEW_SHOW_ITEMS_BY_PAGE
        if (m_pStyle->IsScrollByItem())
        {
            m_pScrollCtrl->SetScrollInfo(m_pStyle->m_nPageItemCount, 0, 0);
        }
        else
        {
            m_pScrollCtrl->SetScrollInfo(1, 0, 0);
        }
#else
        m_pScrollCtrl->SetScrollInfo(m_pStyle->m_nPageItemCount, 0, 0);
#endif
#endif
    }

    if (NULL != m_pAdapter)
    {
        m_pAdapter->OnCleanContent(this);
    }
}

#if IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
void xListView::OnContentChange(int nPageHeight, int nContentHeight, int nContentPos)
{
    if (NULL != m_pScrollCtrl && NULL != m_pStyle)
    {
        if (m_pStyle->IsScrollByPage())
        {
            if (nPageHeight <= 0)
            {
                return;
            }

            int nPageCount = (nContentHeight + nPageHeight - 1) / nPageHeight;
            int nPageNo = nContentPos / nPageHeight;

            m_pScrollCtrl->SetScrollInfo(1, nPageCount, nPageNo);
        }
        else
        {
            m_pScrollCtrl->SetScrollInfo(nPageHeight, nContentHeight, nContentPos);
        }
    }
}

void xListView::OnContentPosChange(int nPosition)
{
    if (NULL != m_pScrollCtrl && NULL != m_pStyle)
    {
        if (m_pStyle->IsScrollByPage())
        {
            int nPageHeight = m_pStyle->GetPageHeight();

            if (nPageHeight <= 0)
            {
                return;
            }

            int nPageNo = nPosition / nPageHeight;

            m_pScrollCtrl->SetScrollPos(nPageNo);
        }
        else
        {
            m_pScrollCtrl->SetScrollPos(nPosition);
        }
    }

    if (NULL != m_pAdapter)
    {
        m_pAdapter->OnContentPosChanged(this, nPosition);
    }
}
#endif
/*-----------------------------------------------------------------------------------*/

/*--------------------------------------Slide View------------------------------------*/
#if IF_XLISTVIEW_SUPPORT_SLIDE
void xListView::SetSlidable(bool bSlidable)
{
    if (bSlidable)
    {
        if (NULL == m_pSlideView)
        {
            m_pSlideView = AllocListSlideView();

            if (NULL == m_pSlideView)
            {
                return;
            }

            addChildView(m_pSlideView);

            m_pSlideView->show();
        }
    }
    else if (NULL != m_pSlideView)
    {
        m_pSlideView->close();
        m_pSlideView = NULL;
    }
}

xListSlideView* xListView::AllocListSlideView()
{
    xListSlideView* pSlideView = new xListSlideView(this);

    return pSlideView;
}

void xListView::BeginSlide()
{
    if (NULL == m_pSlideView)
    {
        return;
    }

    m_pSlideView->OnBeginSlide();
}

void xListView::EndSlide()
{
    if (NULL == m_pSlideView)
    {
        return;
    }

    m_pSlideView->OnEndSlide();
}

void xListView::LocateByOffset(int nOffset, bool bCheckBound)
{
    if (NULL == m_pSlideView || NULL == m_pStyle || 0 == nOffset)
    {
        return;
    }

#if IF_XLIST_VIEW_SLIDE_DEBUG
    XWINDOW_INFO("LocateByOffset:%d", nOffset);
#endif

    int nDataIndex = INVALID_ITEM_INDEX;
    // ContentPos与Y坐标反向，需要取反一次
    nOffset = -nOffset;

    if (!m_pStyle->ShiftContentPos(nOffset, nDataIndex, bCheckBound))
    {
#if IF_XLIST_VIEW_SLIDE_DEBUG
        XWINDOW_INFO("ShiftContentPos fail Offset:%d", nOffset);
#endif
        return;
    }

    DefocusItem();

    // 全键盘显示时滑动，全键盘会直接消失，不需要更新布局
    if (NULL != m_pFlateItem)
    {
        m_pFlateItem = NULL;

        ShowAllItem();
    }

    LoadListItem(nDataIndex, false);

    m_pStyle->m_nItemOffset = 0;

    // 还原回焦点状态
    int nItemIndex = GetIndexByDataIndex(m_pSlideView->m_nFocusDataIndex);

    if (INVALID_ITEM_INDEX != nItemIndex)
    {
        SetItemFocusByIndex(nItemIndex);
    }
    else if (!(NULL != m_pTitle && m_pTitle->containFocused()) && containFocused())
    {
        ownerStation().setFocus_helper(this);
    }

#if IF_XLIST_VIEW_SLIDE_DEBUG
    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();
    int i = 0;

    for (; iter != m_vecItem.end(); ++iter, ++i)
    {
        xListViewItemPtr pItem = *iter;

        if (NULL == pItem)
        {
            continue;
        }

        chRect rcItem = pItem->geometry();

        XWINDOW_INFO("Item Info(Index:%d DataIndex:%d Rect:%d %d %d %d)",
                     i, pItem->GetDataIndex(), rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
    }
#endif

    m_pSlideView->update(true);
}

void xListView::ShowAllItem()
{
    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (; iter != m_vecItem.end(); ++iter)
    {
        xListViewItemPtr pItem = *iter;

        if (NULL != pItem && !pItem->isVisible())
        {
            ownerStation().show_helper(pItem, false);
        }
    }
}

bool xListView::RelocateFocusItem()
{
    if (NULL == m_pSlideView)
    {
        return false;
    }

    m_pSlideView->CancelSlide();

    int nFocusDataIndex = m_pSlideView->m_nFocusDataIndex;

    if (INVALID_ITEM_INDEX != nFocusDataIndex)
    {
        LocateItem(nFocusDataIndex);
        FocusItemByDataIndex(nFocusDataIndex);
        update();
    }

    m_pSlideView->m_nFocusDataIndex = INVALID_ITEM_INDEX;

    return true;
}
#endif
/*-----------------------------------------------------------------------------------*/
