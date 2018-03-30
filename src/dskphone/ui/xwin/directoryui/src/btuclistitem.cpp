#if FEATURE_BROADTOUCH_UC

#include "btuclistitem.h"
#include "contacts/directory/include/commondata.h"
#include "imagecache/include/imagecachedefine.h"
#include "imagecache/include/modimagecache.h"
#include "baseui/include/t2xpicpath.h"

CBTUCListItem::CBTUCListItem(IListViewItemAction* pAction/* = NULL*/)
    : xListViewItem(LIST_ITEM_TYPE_BTUC_CONTACT, pAction)
    , m_pImagePhoto(NULL)
    , m_pImagePresence(NULL)
    , m_pTextName(NULL)
    , m_pTextSignature(NULL)
{
}

CBTUCListItem::~CBTUCListItem()
{
}

int CBTUCListItem::GetBTUCItemType()
{
    return LIST_ITEM_TYPE_BTUC_CONTACT;
}

CBTUCListItem* CBTUCListItem::GetBTUCListItem(xListViewItem * pItem)
{
    if (NULL == pItem || LIST_ITEM_TYPE_BTUC_CONTACT != pItem->GetType())
    {
        return NULL;
    }

    return static_cast<CBTUCListItem *>(pItem);
}

xListViewItem * CBTUCListItem::CreateBTUCListItem()
{
    CBTUCListItem * pListItem = new CBTUCListItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent("directory/btucitem.xml");
    }

    return pListItem;
}

void CBTUCListItem::onLoadChildElement(xml_node & nodeChild)
{
    xListViewItem::onLoadChildElement(nodeChild);

    m_pImagePhoto = static_cast<xImageView*>(getViewById("ImagePhoto"));
    m_pImagePresence = static_cast<xImageView*>(getViewById("ImagePresence"));
    m_pTextName = static_cast<xTextView*>(getViewById("TextName"));
    m_pTextSignature = static_cast<xTextView*>(getViewById("TextSignature"));
}

void CBTUCListItem::SetId(int nId)
{
    m_nId = nId;
}

void CBTUCListItem::SetPresence(const yl::string& strPresence)
{
    if (NULL == m_pImagePresence)
    {
        return;
    }

    m_pImagePresence->setPixmap(strPresence);
}

void CBTUCListItem::SetName(const yl::string & strName)
{
    if (NULL != m_pTextName)
    {
        m_pTextName->SetText(strName);
    }
}

void CBTUCListItem::SetSignature(const yl::string & strSignature)
{
    if (NULL != m_pTextSignature)
    {
        m_pTextSignature->SetText(strSignature);
    }
}

void CBTUCListItem::SetPhoto(const yl::string& strPhoto)
{
    if (NULL == m_pImagePhoto)
    {
        return;
    }

    if (!strPhoto.empty())
    {
        xPixmap pmPhoto;
        yl::string strPhotoPath = commonAPI_FormatString("%s%s", TEMP_PATH, strPhoto.c_str());
        if (Image_GetImage(pmPhoto, strPhotoPath, IMG_CACHE_TYPE_MANUAL))
        {
            m_pImagePhoto->setPixmap(pmPhoto);
            return;
        }
    }

    m_pImagePhoto->setPixmap(PIC_CONTACTS_DEFAULT_IMAGE);
}

int CBTUCListItem::GetId() const
{
    return m_nId;
}

bool CBTUCListItem::onFocusEvent(bool bFocusIn)
{
    if (bFocusIn)
    {
        if (NULL != m_pTextSignature)
        {
            m_pTextSignature->EnableScroll(true);
            m_pTextSignature->SetOverLengthStyle(OVERLENGTH_TYPE_SCROLL);
        }
    }
    else
    {
        if (NULL != m_pTextSignature)
        {
            m_pTextSignature->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
        }
    }

    return xListViewItem::onFocusEvent(bFocusIn);
}

#endif
