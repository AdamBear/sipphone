#include "usbaudiolistitem.h"

#ifdef IF_USB_SUPPORT
CUSBAudioListItem::CUSBAudioListItem(IListViewItemAction * pAction/* = NULL*/)
    : CDetailListItem(pAction)
{
    m_nType = LIST_ITEM_TYPE_USB_AUDIO_LIST;
}

CUSBAudioListItem::~CUSBAudioListItem()
{
}

int CUSBAudioListItem::GetUSBAudioListItemType()
{
    return LIST_ITEM_TYPE_USB_AUDIO_LIST;
}

CUSBAudioListItem * CUSBAudioListItem::GetUSBAudioListItem(xListViewItem * pItem)
{
    if (NULL == pItem || LIST_ITEM_TYPE_USB_AUDIO_LIST != pItem->GetType())
    {
        return NULL;
    }

    return (CUSBAudioListItem *)pItem;
}

xListViewItem * CUSBAudioListItem::CreateUSBAudioListItem()
{
    CUSBAudioListItem * pListItem = new CUSBAudioListItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent("setting/usbaudiolist.xml");
    }

    return pListItem;
}
#endif

