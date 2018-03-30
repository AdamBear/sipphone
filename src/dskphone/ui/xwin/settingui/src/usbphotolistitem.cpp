#include "usbphotolistitem.h"

#if defined(IF_USB_SUPPORT) && IF_SUPPORT_USB_PHOTO
namespace
{
const char* USB_PHOTO_UI_XML = "setting/usbphotolistitem.xml";
const char* USB_PHOTO_UI_ID_ICON = "imgIcon";
}

CUSBPhotoListItem::CUSBPhotoListItem(IListViewItemAction* pAction/* = NULL*/)
    : CDetailListItem(pAction)
{
    m_nType = LIST_ITEM_TYPE_USB_PHOTO_LIST;
}

CUSBPhotoListItem::~CUSBPhotoListItem()
{
}

int CUSBPhotoListItem::GetUSBPhotoListItemType()
{
    return LIST_ITEM_TYPE_USB_PHOTO_LIST;
}

CUSBPhotoListItem* CUSBPhotoListItem::GetUSBPhotoListItem(xListViewItem* pItem)
{
    if (NULL == pItem || LIST_ITEM_TYPE_USB_PHOTO_LIST != pItem->GetType())
    {
        return NULL;
    }

    return (CUSBPhotoListItem*)pItem;
}

xListViewItem* CUSBPhotoListItem::CreateUSBPhotoListItem()
{
    CUSBPhotoListItem* pListItem = new CUSBPhotoListItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent(USB_PHOTO_UI_XML);
    }

    return pListItem;
}

void CUSBPhotoListItem::loadChildrenElements(xml_node& node)
{
    CDetailListItem::loadChildrenElements(node);
    m_pIcon = (xImageView*)getViewById(USB_PHOTO_UI_ID_ICON);
}

void CUSBPhotoListItem::SetIcon(const yl::string& strIcon)
{
    if (NULL != m_pIcon)
    {
        m_pIcon->setPixmap(strIcon);
    }
}

#endif

