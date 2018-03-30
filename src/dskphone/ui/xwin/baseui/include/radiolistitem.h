#ifndef __RADIO_LIST_ITEM_H__
#define __RADIO_LIST_ITEM_H__

#include "menulistitem.h"

enum
{
    LIST_ITEM_SUB_TYPE_MENU_RADIO   = 0x00010000,
};


class xRadioButton;
class CRadioListItem : public CMenuListItem
{
public:
    explicit CRadioListItem(IListViewItemAction * pAction = NULL);
    virtual ~CRadioListItem();

    static int GetRadioItemType();
    static CRadioListItem * GetRadioListItem(xListViewItem * pItem);

    static xListViewItem * CreateRadioListItem();

public:
    virtual void loadChildrenElements(xml_node & node);

    void SetCheck(bool bCheck);

protected:
    xRadioButton*     m_pRadioBtn;
};

#endif
