#include "xWindowManager.h"
#include "xListView.h"
#include "xlistviewitem.h"
#include "commonapi/stringhelperapi.h"
#if IF_MOVE_FOCUS_BY_KEY
#include "keymap.h"
#endif
#include "modviewdump.h"


namespace
{
#define LIST_ITEM_DELAY_CLICK_CHECK 40
}


FUN_FREE_ITEM xListViewItem::s_pFunFreeItem = NULL;

xListViewItem::xListViewItem(int nType, IListViewItemAction* pAction/* = NULL*/)
    : m_pListView(NULL)
    , m_pAction(NULL)
    , m_nType(nType)
    , m_nDataIndex(INVALID_ITEM_INDEX)
    , m_bFocus(false)
{
    m_bFocusPresent = true;

    REGISTER_VIEW_DUMP_CLASS(xListViewItem)

#if IF_GESTURE_ENABLE
    grabGesture(GESTURE_TYPE_TAP);
#endif
}

xListViewItem::~xListViewItem()
{
#if IF_GESTURE_ENABLE
    clearGesture();
#endif
}

void xListViewItem::SetFunFreeItem(FUN_FREE_ITEM pFunFreeItem)
{
    s_pFunFreeItem = pFunFreeItem;
}

void xListViewItem::Release()
{
    SetListView(NULL);
    SetAction(NULL);

    if (NULL != s_pFunFreeItem)
    {
        (*s_pFunFreeItem)(this);
    }
    else
    {
        close();
    }
}

void xListViewItem::SetListView(xListView* pListView)
{
    m_pListView = pListView;
}

void xListViewItem::SetAction(IListViewItemActionPtr pAction)
{
    m_pAction = pAction;
}

void xListViewItem::ClearContent()
{
    int nDataIndex = m_nDataIndex;
    bool bFocus = m_bFocus;

    m_nDataIndex = INVALID_ITEM_INDEX;
    m_bFocus = false;

    if (NULL != m_pListView)
    {
        if (nDataIndex != m_nDataIndex)
        {
            m_pListView->OnDataIndexChange(this);
        }

        if (bFocus != m_bFocus)
        {
            m_pListView->OnItemFocusChange(this, bFocus);
        }
    }
}

xListView* xListViewItem::GetListView() const
{
    return m_pListView;
}

int xListViewItem::GetType() const
{
    return m_nType;
}

int xListViewItem::GetDataIndex() const
{
    return m_nDataIndex;
}

void xListViewItem::SetDataIndex(int nIndex)
{
    m_nDataIndex = nIndex;

    m_strId = commonAPI_FormatString("Item_%d", m_nDataIndex);

    if (NULL != m_pListView)
    {
        m_pListView->OnDataIndexChange(this);
    }
}

bool xListViewItem::IsItemFocus() const
{
    return m_bFocus;
}

void xListViewItem::SelectItem(bool bSelect)
{
    FocusItem(bSelect, LIST_VIEW_ITEM_ACTION_SELECT);
}

void xListViewItem::FocusItem(bool bFocus, int nAction/* = LIST_VIEW_ITEM_ACTION_FOCUS*/)
{
    bool bPreFocus = m_bFocus;

    SetItemFocus(bFocus);

    if (m_bFocus)
    {
        if (NULL != m_pListView)
        {
            m_pListView->OnItemFocusChange(this, bPreFocus);
        }

        DoAction(nAction);
    }
}

void xListViewItem::SetItemFocus(bool bFocus)
{
    m_bFocus = bFocus;

    if (!m_bFocus || NULL == m_pListView)
    {
        return;
    }

    if (m_pListView->hasFocus() || m_pListView->containFocus())
    {
        if (!checkVisible())
        {
            ownerStation().show_helper(this, false);
        }

        if (!hasFocus() && !containFocus())
        {
            setFocus();

            xWorkStation& station = ownerStation();

            if (hasFocused())
            {
                xViewEvent evEnter(VIEW_FOCUS_ENTER);

                station.dispatchDirectionalViewEvent(this, &evEnter);

                station.UpdateFocusWidget(this);
            }
        }

        return;
    }

    xView* pWindow = m_pListView->getRootView();

    if (NULL == pWindow)
    {
        return;
    }

    dataWindowObject* pWindowObject = pWindow->m_pWindow;

    if (NULL == pWindowObject)
    {
        return;
    }

    // 非活动窗口时，先设置为focusView
    if (this != pWindowObject->focusView && (m_pListView == pWindowObject->focusView
            || xWorkStation::containChildView(m_pListView, pWindowObject->focusView)))
    {
        ownerStation().setFocus_helper(this);
    }
}

#if IF_MOVE_FOCUS_BY_KEY
bool xListViewItem::IsSwitchInsideFocus(int nFocusReason)
{
    xView* pFocus = ownerStation().getFocusView();

    if (pFocus == this)
    {
        return false;
    }

    if (NULL == pFocus || !xWorkStation::containChildView(this, pFocus))
    {
        return false;
    }

    yl::string strPropertyName = "";

    if (FOCUS_REASON_LEFT == nFocusReason)
    {
        strPropertyName = XWINDOW_NAME_NEXT_FOCUS_LEFT;
    }
    else if (FOCUS_REASON_RIGHT == nFocusReason)
    {
        strPropertyName = XWINDOW_NAME_NEXT_FOCUS_RIGHT;
    }
    else if (FOCUS_REASON_UP == nFocusReason)
    {
        strPropertyName = XWINDOW_NAME_NEXT_FOCUS_UP;
    }
    else if (FOCUS_REASON_DOWN == nFocusReason)
    {
        strPropertyName = XWINDOW_NAME_NEXT_FOCUS_DOWN;
    }
    else
    {
        return false;
    }

    yl::string strNextViewName = pFocus->getStringProperty(strPropertyName, "");

    if (strNextViewName.empty())
    {
        return false;
    }

    return true;
}
#endif

xStyle* xListViewItem::GetItemStyle() const
{
    if (NULL == m_pListView)
    {
        return NULL;
    }

    xListViewStyle* pStyle = m_pListView->GetStyle();

    if (NULL == pStyle)
    {
        return NULL;
    }

    yl::string strStyle = pStyle->GetItemStyle();

    if (strStyle.empty())
    {
        return NULL;
    }

    return xStyle::getObjectByName(strStyle);
}

#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
bool xListViewItem::onKeyPressedEvent(int key)
{
    if (PHONE_KEY_OK == key && IsItemFocus())
    {
        DoAction(LIST_VIEW_ITEM_ACTION_SELECT);

        return true;
    }

    return false;
}
#endif

#if IF_GESTURE_ENABLE
bool xListViewItem::onGestureEvent(xGestureEvent* pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    xGesturePtr pGesture = pEvent->GetGestureByType(GESTURE_TYPE_TAP);
    xTapGesture* pTapGesture = xTapGesture::GetTapGesture(pGesture.Get());

    if (NULL != pTapGesture && GESTURE_STATE_FINISH == pTapGesture->GetState())
    {
        if (hasFocused() || containFocused())
        {
            DoAction(LIST_VIEW_ITEM_ACTION_SELECT);
        }
        else if (NULL != m_pListView)
        {
            g_WorkStation.CheckViewActiveAndFocus(this, m_pListView);

            m_pListView->FocusItemByDataIndex(m_nDataIndex);
#if IF_VIEW_ASYNC_CLICK
            g_WorkStation.AsyncClick(this);
#endif
        }

        return true;
    }

    return false;
}

void xListViewItem::onClick()
{
    if (hasFocused() || containFocused())
    {
        DoAction(LIST_VIEW_ITEM_ACTION_SELECT);
    }
}
#endif

#if IF_XLISTVIEW_SUPPORT_ITEM_CUSTOM_DECIDE_FOCUS_BG
bool xListViewItem::IsShowFocusBG()
{
#if IF_XLISTVIEW_ITEM_DEFAULT_BG_BY_ITEM_FOCUS
    return m_bFocus;
#else
    return (hasFocus() || containFocus());
#endif
}
#endif

bool xListViewItem::onPaintBackground(xPainter& p)
{
    xStyle* pStyle = GetItemStyle();

    if (NULL != pStyle)
    {
        return pStyle->onViewPaintBackground(this, p);
    }

    return false;
}

void xListViewItem::DoAction(int nAction)
{
    if (NULL == m_pAction || NULL == m_pListView || !isVisible())
    {
        return;
    }

    m_pAction->OnItemAction(m_pListView, this, nAction);
}
