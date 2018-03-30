#include "twolineeditlistitem.h"
#include "xwindow/xListView.h"
#include "xwindow/xWindowManager.h"
#include "baseuicommon.h"


CTwoLineEditListItem::CTwoLineEditListItem(IListViewItemAction* pAction/* = NULL*/)
    : CEditListItem(pAction)
{
    m_nType = LIST_ITEM_TYPE_DETAIL | LIST_ITEM_SUB_TYPE_DETAIL_TWO_LINE_EDIT;
}

CTwoLineEditListItem::~CTwoLineEditListItem()
{
}

int CTwoLineEditListItem::GetTwoLineEditListItemType()
{
    return (LIST_ITEM_TYPE_DETAIL | LIST_ITEM_SUB_TYPE_DETAIL_TWO_LINE_EDIT);
}

CTwoLineEditListItem* CTwoLineEditListItem::GetTwoLineEditListItem(xListViewItem* pItem)
{
    if (NULL == pItem || GetTwoLineEditListItemType() != pItem->GetType())
    {
        return NULL;
    }

    return (CTwoLineEditListItem*)pItem;
}

xListViewItem* CTwoLineEditListItem::CreateTwoLineEditListItem()
{
    CTwoLineEditListItem* pListItem = new CTwoLineEditListItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent(BASE_UI_LAYOUT_FILE_TWO_LINE_EDIT_LIST_ITEM);
    }

    return pListItem;
}

