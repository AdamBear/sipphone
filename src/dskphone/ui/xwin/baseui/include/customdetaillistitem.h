#ifndef __CUSTOM_DETAIL_LIST_ITEM_H__
#define __CUSTOM_DETAIL_LIST_ITEM_H__

#include "detaillistitem.h"
#include <baseui/include/baseuidefine.h>

#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE

enum
{
    LIST_ITEM_SUB_TYPE_CUSTOM_DETAIL_ITEM      = 0x00002200,
};

class CCustomDetailListItem : public CDetailListItem
{
public:
    CCustomDetailListItem(IListViewItemAction * pAction = NULL);
    virtual ~CCustomDetailListItem();

    static int GetCustomDetailListItemType();
    static CCustomDetailListItem * GetCustomDetailListItem(xListViewItem * pItem);

    static xListViewItem * CreateCustomDetailListItem();

#if IF_XLISTVIEW_SUPPORT_ITEM_CUSTOM_DECIDE_FOCUS_BG
    virtual bool IsShowFocusBG();
#endif
};

#endif //IF_EDIT_COMBOBOX_SHOW_TWO_LINE
#endif //__CUSTOM_DETAIL_LIST_ITEM_H__
