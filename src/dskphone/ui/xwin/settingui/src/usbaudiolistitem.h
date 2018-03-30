#ifndef __USB_AUDIO_LIST_ITEM_H__
#define __USB_AUDIO_LIST_ITEM_H__

#ifdef IF_USB_SUPPORT
#include "baseui/include/detaillistitem.h"

enum
{
    LIST_ITEM_TYPE_USB_AUDIO_LIST   = 0x00006600,
};

class CUSBAudioListItem : public CDetailListItem
{
public:
    explicit CUSBAudioListItem(IListViewItemAction * pAction = NULL);
    virtual ~CUSBAudioListItem();

    static int GetUSBAudioListItemType();
    static CUSBAudioListItem * GetUSBAudioListItem(xListViewItem * pItem);

    static xListViewItem * CreateUSBAudioListItem();
};

typedef CUSBAudioListItem * CUSBAudioListItemPtr;
#endif

#endif//__USB_AUDIO_LIST_ITEM_H__
