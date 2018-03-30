#ifndef __ICON_MENU_LIST_ITEM_H__
#define __ICON_MENU_LIST_ITEM_H__

#if IF_FEATURE_METASWITCH_DIRECTORY

#include <ylstring.h>
#include "baseui/include/menulistitem.h"

enum
{
    LIST_ITEM_TYPE_ICON_MENU     = 0x00110000,
};

class xImageView;

class CIconMenuListItem : public CMenuListItem
{
public:
    explicit CIconMenuListItem(IListViewItemAction * pAction = NULL);
    virtual ~CIconMenuListItem();

    static int GetIconMenuItemType();
    static CIconMenuListItem* GetIconMenuListItem(xListViewItem * pItem);

    static xListViewItem * CreateIconMenuListItem();

public:
    virtual void onLoadChildElement(xml_node & nodeChild);
    void SetPhoto(const yl::string& strPhoto);

protected:
    xImageView* m_pImagePhoto;
};

#endif //IF_FEATURE_METASWITCH_DIRECTORY
#endif //__ICON_MENU_LIST_ITEM_H__
