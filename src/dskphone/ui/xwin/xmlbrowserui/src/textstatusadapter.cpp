#include "textstatusadapter.h"
#include "baseui/include/textstatusitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xListView.h"
#include "xmlbrowseruicommon.h"
#include "imagecache/include/modimagecache.h"
#include "baseui/include/t2xpicpath.h"

#if XML_BROWSER_SUPPORT_UI
#define XML_STATUS_ITEMNUM_MAX 4
////////////////////////////////////////////////////////////////////////////////
CTextStatusAdapter::CTextStatusAdapter()
    : xListViewAdapter()
{
    m_vecItemData.clear();
}

CTextStatusAdapter::~CTextStatusAdapter()
{

}

void CTextStatusAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    xListViewStyle* pStyle = pListView->GetStyle();

    if (NULL != pStyle)
    {
        pStyle->SetItemHeight(0);
        pStyle->SetPageItemCount(m_vecItemData.size() > XML_STATUS_ITEMNUM_MAX ? XML_STATUS_ITEMNUM_MAX : m_vecItemData.size());
        pStyle->SetPageHeight(180);
        pStyle->SetContentHeight(180);
#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
        pStyle->SetItemLineIgnoreEnd(true);
#endif

    }
    pListView->SetTotalDataCount(m_vecItemData.size() > XML_STATUS_ITEMNUM_MAX ? XML_STATUS_ITEMNUM_MAX : m_vecItemData.size());
}

xListViewItem * CTextStatusAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return NULL;
    }

    CTextStatusItemPtr pTextStatusItem = CTextStatusItem::GetStatusItem(pItem);

    if (NULL == pTextStatusItem)
    {
        pTextStatusItem = static_cast<CTextStatusItemPtr>(g_ItemFactory.ApplyItem(
                              CTextStatusItem::GetStatusItemType()));
    }


    return pTextStatusItem;
}

bool CTextStatusAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return false;
    }

    CTextStatusItemPtr pTextStatusItem = CTextStatusItem::GetStatusItem(pItem);

    if (NULL == pTextStatusItem)
    {
        return false;
    }

    pTextStatusItem->SetAction(this);

    int nDataIndex = pItem->GetDataIndex();

    if (nDataIndex >= 0 && nDataIndex < m_vecItemData.size())
    {
        const StatusItem & stMenuData = m_vecItemData[nDataIndex];
        xPixmap pmIcon;
        yl::string strIconPath = commonAPI_FormatString("%s%s", PIC_COMMON_PATH,
                                 stMenuData.strIconPath.c_str());
        pmIcon.load(strIconPath);
        pTextStatusItem->SetIcon(pmIcon);
        pTextStatusItem->SetGravity(TypeToAlignmentFlag(stMenuData.iAlign));
        pTextStatusItem->SetMenu(stMenuData.strMsg);
        pTextStatusItem->SetColor(TypeToColor(stMenuData.eColor));
        pTextStatusItem->SetFontSize(TypeToFontPointSize(stMenuData.eSize));

    }

    return true;
}

void CTextStatusAdapter::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
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


int CTextStatusAdapter::TypeToItemHeight(X_FONT_TYPE eFontSize)
{
    switch (eFontSize)
    {
    case X_FONT_SMALL:
    case X_FONT_NORMAL:
    case X_FONT_DOUBLE:
        return 28;
    case X_FONT_LARGE:
        return 32;
    default:
        break;
    }

    return 28;
}

void CTextStatusAdapter::SetData(const VEC_STATUS_ITEM_DATA & vecData)
{
    m_vecItemData = vecData;
}

bool CTextStatusAdapter::GetItemHeight(xListView * pListView, int nDataIndex, int& nItemHeight)
{
    if (m_vecItemData.size() < nDataIndex + 1)
    {
        return false;
    }

    nItemHeight = TypeToItemHeight(m_vecItemData[nDataIndex].eSize);
    return true;
}


#endif
