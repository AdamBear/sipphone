#include "customdetaillistitem.h"

#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE

CCustomDetailListItem::CCustomDetailListItem(IListViewItemAction * pAction/* = NULL*/)
    : CDetailListItem(pAction)
{
    m_nType |= LIST_ITEM_SUB_TYPE_CUSTOM_DETAIL_ITEM;
}

CCustomDetailListItem::~CCustomDetailListItem()
{
}

int CCustomDetailListItem::GetCustomDetailListItemType()
{
    return (LIST_ITEM_TYPE_DETAIL | LIST_ITEM_SUB_TYPE_DETAIL_ITEM | LIST_ITEM_SUB_TYPE_CUSTOM_DETAIL_ITEM);
}

CCustomDetailListItem * CCustomDetailListItem::GetCustomDetailListItem(xListViewItem * pItem)
{
    if (NULL == pItem || GetCustomDetailListItemType() != pItem->GetType())
    {
        return NULL;
    }

    return (CCustomDetailListItem *)pItem;
}

xListViewItem * CCustomDetailListItem::CreateCustomDetailListItem()
{
    CCustomDetailListItem * pDetailItem = new CCustomDetailListItem;

    if (NULL != pDetailItem)
    {
        pDetailItem->loadContent("customdetailitem.xml");
    }

    return pDetailItem;
}

#if IF_XLISTVIEW_SUPPORT_ITEM_CUSTOM_DECIDE_FOCUS_BG
bool CCustomDetailListItem::IsShowFocusBG()
{
    return false;
}
#endif

#endif //IF_EDIT_COMBOBOX_SHOW_TWO_LINE
