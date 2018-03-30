#ifndef __USB_PHOTO_LIST_ITEM_H__
#define __USB_PHOTO_LIST_ITEM_H__

#include "settinguilogic/include/settingdefine.h"
#if defined(IF_USB_SUPPORT) && IF_SUPPORT_USB_PHOTO
#include "baseui/include/detaillistitem.h"

enum
{
    LIST_ITEM_TYPE_USB_PHOTO_LIST   = 0x00006700,
};

class CUSBPhotoListItem : public CDetailListItem
{
public:
    explicit CUSBPhotoListItem(IListViewItemAction* pAction = NULL);
    virtual ~CUSBPhotoListItem();

    static int GetUSBPhotoListItemType();
    static CUSBPhotoListItem* GetUSBPhotoListItem(xListViewItem* pItem);

    static xListViewItem* CreateUSBPhotoListItem();

public:
    virtual void loadChildrenElements(xml_node& node);
public:
    void SetIcon(const yl::string& strIcon);

protected:
    xImageView*     m_pIcon;                    // ÎÄ±¾
};

typedef CUSBPhotoListItem* CUSBPhotoListItemPtr;
#endif

#endif//__USB_PHOTO_LIST_ITEM_H__
