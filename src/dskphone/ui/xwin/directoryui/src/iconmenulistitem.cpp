#if IF_FEATURE_METASWITCH_DIRECTORY

#include "iconmenulistitem.h"
#include "xwindow/xWindow.h"

CIconMenuListItem::CIconMenuListItem(IListViewItemAction* pAction/* = NULL*/)
    : CMenuListItem(pAction)
    , m_pImagePhoto(NULL)
{
    m_nType = LIST_ITEM_TYPE_ICON_MENU;
}

CIconMenuListItem::~CIconMenuListItem()
{
}

int CIconMenuListItem::GetIconMenuItemType()
{
    return LIST_ITEM_TYPE_ICON_MENU;
}

CIconMenuListItem* CIconMenuListItem::GetIconMenuListItem(xListViewItem * pItem)
{
    if (NULL == pItem || LIST_ITEM_TYPE_ICON_MENU != pItem->GetType())
    {
        return NULL;
    }

    return static_cast<CIconMenuListItem *>(pItem);
}

xListViewItem * CIconMenuListItem::CreateIconMenuListItem()
{
    CIconMenuListItem* pListItem = new CIconMenuListItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent("directory/iconmenuitem.xml");
    }

    return pListItem;
}

void CIconMenuListItem::onLoadChildElement(xml_node & nodeChild)
{
    CMenuListItem::onLoadChildElement(nodeChild);

    m_pImagePhoto = static_cast<xImageView*>(getViewById("ImagePhoto"));
}

void CIconMenuListItem::SetPhoto(const yl::string& strPhoto)
{
    if (NULL != m_pImagePhoto)
    {
        m_pImagePhoto->setPixmap(strPhoto);
    }
}

#endif
