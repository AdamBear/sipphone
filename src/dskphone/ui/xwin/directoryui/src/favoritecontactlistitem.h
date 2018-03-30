#ifndef __FAVORITE_CONTACT_LIST_ITEM_H__
#define __FAVORITE_CONTACT_LIST_ITEM_H__

#include <directorydefine.h>
#include "contactlistitem.h"
#if IF_DIR_SUPPORT_FAVORITE_ITEM

enum
{
    LIST_ITEM_SUB_TYPE_FAVORITE_CONTACT_ITEM         = 0x00001100,
};

class CFavoriteContactListItem : public CContactListItem
{
public:
    explicit CFavoriteContactListItem(IListViewItemAction * pAction = NULL);
    virtual ~CFavoriteContactListItem();

    static int GetFavoriteContactListItemType();
    static CFavoriteContactListItem * GetFavoriteContactListItem(xListViewItem * pItem);

    static xListViewItem * CreateFavoriteContactListItem();
public:
    virtual void onLoadChildElement(xml_node & nodeChild);
    void SetFavoriteIndex(const yl::string& strFavorite);
protected:
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    virtual void UpdateTextView(bool bFocus);
#endif
protected:
    xTextView* m_pTextFavorite;
};
typedef CFavoriteContactListItem * CFavoriteContactListItemPtr;

#endif //IF_FEATURE_FAVORITE_DIR
#endif //__FAVORITE_CONTACT_LIST_ITEM_H__
