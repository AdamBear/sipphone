#include "cuiregionbaselistscreen.h"
#include "keymap.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "modxmlbrowserui.h"
#include "xmlbrowseruicommon.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "baseui/include/inputmethod.h"
#include "xwindow/xListView.h"
#include "xwindow/xWindowManager.h"

#if XML_BROWSER_SUPPORT_UI

namespace
{
#define XML_REGION_BASE_LIST_SCREEN_XML             "xmlbrowserui/xmlregionbaselistscreen.xml"
#define XML_REGION_BASE_LIST_SCREEN_TITLE           "txtTitle"
#define XML_REGION_BASE_LIST_SCREEN_LIST_VIEW       "listView"
#define XML_REGION_BASE_LIST_SCREEN_SCROLL_BAR      "scrollCtrl"
#define XML_REGION_BASE_LIST_SCREEN_TITLE_HEIGHT_MIN            19
#define XML_REGION_BASE_LIST_SCREEN_TITLE_HEIGHT_MAX            50
#define XML_REGION_BASE_LIST_SCREEN_ITEM_DEFAULT_HEIGHT         18

#define XML_REGION_BASE_LIST_SCREEN_ITEM_DEFAULT_HEIGHT         18
#define XML_REGION_BASE_LIST_SCREEN_HEIHGT                      95
}


////////////////////////////////////////////////////////////////////////////////
CUIRegionBaseListScreen::CUIRegionBaseListScreen(bool bLoadXml/* = true*/)
    : CUIRegionBase()
    , m_pTxtTitle(NULL)
    , m_pListView(NULL)
    , m_pScroll(NULL)
{
    if (bLoadXml)
    {
        loadContent(XML_REGION_BASE_LIST_SCREEN_XML);
    }
}

CUIRegionBaseListScreen::~CUIRegionBaseListScreen()
{

}

int CUIRegionBaseListScreen::GetCurrentItemIndex()
{
    if (NULL != m_pListView)
    {
        return m_pListView->GetFocusItemDataIndex();
    }
    return 0;
}

void CUIRegionBaseListScreen::loadChildrenElements(xml_node & node)
{
    CUIRegionBase::loadChildrenElements(node);
    m_pTxtTitle = static_cast<xTextView *>(getViewById(XML_REGION_BASE_LIST_SCREEN_TITLE));
    m_pListView = static_cast<xListView *>(getViewById(XML_REGION_BASE_LIST_SCREEN_LIST_VIEW));
    m_pScroll = static_cast<xScrollBar *>(getViewById(XML_REGION_BASE_LIST_SCREEN_SCROLL_BAR));

    if (NULL != m_pTxtTitle)
    {
        m_pTxtTitle->SetWrapLimitedHeight(XML_REGION_BASE_LIST_SCREEN_TITLE_HEIGHT_MAX);
    }
    if (NULL != m_pListView)
    {
        m_pListView->SetAdapter(this);
    }
}

void CUIRegionBaseListScreen::OnItemAction(xListView * pListView, xListViewItem * pItem,
        int nAction)
{
    if (NULL == pListView || pListView != m_pListView || NULL == pItem)
    {
        return;
    }

    if (LIST_VIEW_ITEM_ACTION_FOCUS == nAction)
    {
        xListViewAdapter::OnItemAction(pListView, pItem, nAction);
        UpdateIndex();
        m_signalAction.emitSignal(REGION_ACTION_CURRENT_ITEM_CHANGE, 0, 0, NULL);
    }
}

void CUIRegionBaseListScreen::UpdateContent()
{
    RefreshListView();
    if (NULL != m_pListView && m_stBaseData.m_bIsDefaultIndex)
    {
        int nDefaultIndex = m_stBaseData.m_nDefaultIndex;
        if (nDefaultIndex < -1 || nDefaultIndex >= m_pListView->GetTotalDataCount())
        {
            nDefaultIndex = -1;
        }
        xListViewItemPtr pItem = m_pListView->GetItemByDataIndex(nDefaultIndex);
        if (NULL == pItem)
        {
            SetPageDataIndex(nDefaultIndex);
            m_pListView->RefreshData(nDefaultIndex);
        }
        m_pListView->FocusItemByDataIndex(nDefaultIndex);
        m_stBaseData.m_bIsDefaultIndex = false;
    }
}

void CUIRegionBaseListScreen::RelayoutByContent()
{
    if (NULL == m_pTxtTitle
            || NULL == m_pListView)
    {
        return ;
    }

    m_pTxtTitle->SetText(m_stBaseData.m_strTitle);

    if (m_stBaseData.m_bIsWrapTitle)
    {
        int nWrapHeight = m_pTxtTitle->wrapBoundHeight();

        if (nWrapHeight < XML_REGION_BASE_LIST_SCREEN_TITLE_HEIGHT_MIN)
        {
            nWrapHeight = XML_REGION_BASE_LIST_SCREEN_TITLE_HEIGHT_MIN;
        }

        int nItemHeight = GetItemHeight();

        if (0 == nItemHeight)
        {
            return;
        }

        int nItemCount = (XML_REGION_BASE_LIST_SCREEN_HEIHGT - nWrapHeight - 1) / nItemHeight;

        m_pListView->layout_height = nItemCount * nItemHeight;
        m_pListView->layout_marginTop = nWrapHeight;
    }
    else
    {
        m_pTxtTitle->SetMultiLineEnable(false);
        m_pTxtTitle->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
        m_pTxtTitle->layout_height = XML_REGION_BASE_LIST_SCREEN_TITLE_HEIGHT_MIN ;
    }
}
void CUIRegionBaseListScreen::LayoutContentInRect(const chRect& rectParentContent)
{
    CUIRegionBase::LayoutContentInRect(rectParentContent);
    RelayoutByContent();
}

void CUIRegionBaseListScreen::InitListViewLayout(xListView* pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    xListViewStylePtr pStyle = pListView->GetStyle();
    if (NULL == pStyle)
    {
        return;
    }

    int nItemHeight = GetItemHeight();
    if (0 == nItemHeight)
    {
        return;
    }

    int nItemCount = pListView->layout_height / nItemHeight;
    pStyle->SetItemHeight(nItemHeight);
    pListView->SetPageItemCount(nItemCount);
}

int CUIRegionBaseListScreen::GetItemHeight()
{
    return XML_REGION_BASE_LIST_SCREEN_ITEM_DEFAULT_HEIGHT;
}
#endif
