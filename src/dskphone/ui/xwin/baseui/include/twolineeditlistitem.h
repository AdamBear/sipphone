#ifndef __TWO_LINE_EDIT_LIST_ITEM_H__
#define __TWO_LINE_EDIT_LIST_ITEM_H__

#include "editlistitem.h"

enum
{
    LIST_ITEM_SUB_TYPE_DETAIL_TWO_LINE_EDIT     = 0x00090000,
};


class xEditTextView;

class CTwoLineEditListItem : public CEditListItem
{
public:
    explicit CTwoLineEditListItem(IListViewItemAction* pAction = NULL);
    virtual ~CTwoLineEditListItem();

    static int GetTwoLineEditListItemType();
    static CTwoLineEditListItem* GetTwoLineEditListItem(xListViewItem* pItem);
    static xListViewItem* CreateTwoLineEditListItem();
};

typedef CTwoLineEditListItem* CTwoLineEditListItemPtr;

#endif // __TWO_LINE_EDIT_LIST_ITEM_H__
