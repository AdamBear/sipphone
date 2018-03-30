#include "formattextadapter.h"
#include "baseui/include/menulistitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xListView.h"

#if XML_BROWSER_SUPPORT_UI

////////////////////////////////////////////////////////////////////////////////
CFormatTextAdapter::CFormatTextAdapter()
    : xListViewAdapter()
{
    m_vecItemData.clear();
}

CFormatTextAdapter::~CFormatTextAdapter()
{

}

void CFormatTextAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    xListViewStyle* pStyle = pListView->GetStyle();

    if (NULL != pStyle)
    {
        pStyle->SetItemHeight(0);
        pStyle->SetPageItemCount(m_vecItemData.size());
        pStyle->SetPageHeight(272);
        pStyle->SetContentHeight(272);
#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
        pStyle->SetItemLineIgnoreEnd(true);
#endif

    }
    pListView->SetTotalDataCount(m_vecItemData.size());
}

xListViewItem * CFormatTextAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return NULL;
    }

    CMenuListItemPtr pMenuItem = CMenuListItem::GetMenuListItem(pItem);

    if (NULL == pMenuItem)
    {
        pMenuItem = static_cast<CMenuListItemPtr>(g_ItemFactory.ApplyItem(
                        CMenuListItem::GetMenuItemType()));
    }


    return pMenuItem;
}

bool CFormatTextAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return false;
    }

    CMenuListItemPtr pMenuItem = CMenuListItem::GetMenuListItem(pItem);

    if (NULL == pMenuItem)
    {
        return false;
    }

    pMenuItem->SetAction(this);

    int nDataIndex = pItem->GetDataIndex();

    if (nDataIndex >= 0 && nDataIndex < m_vecItemData.size())
    {
        const stScrollData & stMenuData = m_vecItemData[nDataIndex];
        pMenuItem->SetGravity(stMenuData.nAlgin);
        pMenuItem->SetMenu(stMenuData.m_strText);
        pMenuItem->SetColor(stMenuData.m_Color);
        pMenuItem->SetFontSize(stMenuData.eFont);

    }

    return true;
}

void CFormatTextAdapter::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
{
    if (NULL == pListView || pListView != m_pListView || NULL == pItem)
    {
        return;
    }

    if (LIST_VIEW_ITEM_ACTION_FOCUS == nAction)
    {
        xListViewAdapter::OnItemAction(pListView, pItem, nAction);
        UpdateIndex();
    }
}

void CFormatTextAdapter::SetData(const VEC_SCROLL_ITEM_DATA & vecData)
{
    m_vecItemData = vecData;
}

bool CFormatTextAdapter::GetItemHeight(xListView * pListView, int nDataIndex, int& nItemHeight)
{
    if (m_vecItemData.size() < nDataIndex + 1)
    {
        return false;
    }

    nItemHeight = m_vecItemData[nDataIndex].eFont;
    return true;
}

#endif
