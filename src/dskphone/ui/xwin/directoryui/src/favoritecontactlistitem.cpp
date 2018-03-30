#include "favoritecontactlistitem.h"
#if IF_DIR_SUPPORT_FAVORITE_ITEM

CFavoriteContactListItem::CFavoriteContactListItem(IListViewItemAction * pAction/* = NULL*/)
    : CContactListItem(pAction)
    , m_pTextFavorite(NULL)
{
    m_nType |= LIST_ITEM_SUB_TYPE_FAVORITE_CONTACT_ITEM;
}

CFavoriteContactListItem::~CFavoriteContactListItem()
{
}

int CFavoriteContactListItem::GetFavoriteContactListItemType()
{
    return (LIST_ITEM_TYPE_CONTACT_BASE | LIST_ITEM_SUB_TYPE_FAVORITE_CONTACT_ITEM);
}

CFavoriteContactListItem * CFavoriteContactListItem::GetFavoriteContactListItem(xListViewItem * pItem)
{
    if (NULL == pItem || pItem->GetType() != GetFavoriteContactListItemType())
    {
        return NULL;
    }

    return static_cast<CFavoriteContactListItem *>(pItem);
}

xListViewItem * CFavoriteContactListItem::CreateFavoriteContactListItem()
{
    CFavoriteContactListItem * pListItem = new CFavoriteContactListItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent("directory/favoritecontactitem.xml");
    }

    return pListItem;
}

void CFavoriteContactListItem::onLoadChildElement(xml_node & nodeChild)
{
    CContactListItem::onLoadChildElement(nodeChild);

    m_pTextFavorite = static_cast<xTextView*>(getViewById("txtFavorite"));
}

void CFavoriteContactListItem::SetFavoriteIndex(const yl::string& strFavorite)
{
    if (NULL != m_pTextFavorite)
    {
        m_pTextFavorite->SetText(strFavorite);
    }
}

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
void CFavoriteContactListItem::UpdateTextView(bool bFocus)
{
    CContactListItem::UpdateTextView(bFocus);

    UpdateTextViewColor(m_pTextFavorite, bFocus);
}
#endif


#endif
