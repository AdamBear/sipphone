#include "bluetoothdevitem.h"

#ifdef IF_SUPPORT_BLUETOOTH
CBluetoothDevItem::CBluetoothDevItem(IListViewItemAction * pAction/* = NULL*/)
    : xListViewItem(LIST_ITEM_TYPE_BLUETOOTH_DEV, pAction)
    , m_pDevIcon(NULL)
    , m_pName(NULL)
    , m_pConnectStatusIcon(NULL)
    , m_pMac(NULL)
{
}

CBluetoothDevItem::~CBluetoothDevItem()
{
}

int CBluetoothDevItem::GetBluetoothDevItemType()
{
    return LIST_ITEM_TYPE_BLUETOOTH_DEV;
}

CBluetoothDevItem * CBluetoothDevItem::GetBluetoothDevItem(xListViewItem * pItem)
{
    if (NULL == pItem
            || CBluetoothDevItem::GetBluetoothDevItemType() != pItem->GetType())
    {
        return NULL;
    }

    return (CBluetoothDevItem *)pItem;
}

xListViewItem * CBluetoothDevItem::CreateBluetoothDevItem()
{
    CBluetoothDevItem * pListItem = new CBluetoothDevItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent("setting/bluetoothdevitem.xml");
    }

    return pListItem;
}

void CBluetoothDevItem::loadChildrenElements(xml_node & node)
{
    xListViewItem::loadChildrenElements(node);

    m_pDevIcon = static_cast<xImageView *>(getViewById("picDevIcon"));
    m_pName = static_cast<xTextView *>(getViewById("txtName"));
    m_pConnectStatusIcon = static_cast<xImageView *>(getViewById("picConnectStatusIcon"));
    m_pMac = static_cast<xTextView *>(getViewById("txtMac"));
}

bool CBluetoothDevItem::onFocusEvent(bool bFocusIn)
{
    if (NULL != m_pName)
    {
        if (bFocusIn)
        {
            m_pName->EnableScroll(true);
            m_pName->SetOverLengthStyle(OVERLENGTH_TYPE_SCROLL);
            m_pMac->EnableScroll(true);
            m_pMac->SetOverLengthStyle(OVERLENGTH_TYPE_SCROLL);
        }
        else
        {
            m_pName->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
            m_pMac->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
        }
    }

    return xListViewItem::onFocusEvent(bFocusIn);
}

void CBluetoothDevItem::ShowConnectStatusIcon(bool bShowConnectStatusIcon)
{
    if (NULL != m_pConnectStatusIcon && bShowConnectStatusIcon != m_pConnectStatusIcon->isVisible())
    {
        if (bShowConnectStatusIcon)
        {
            m_pConnectStatusIcon->show();
        }
        else
        {
            m_pConnectStatusIcon->hide();
        }
    }
}

void CBluetoothDevItem::SetDevIcon(const yl::string & strDevIcon)
{
    if (NULL != m_pDevIcon)
    {
        m_pDevIcon->setPixmap(strDevIcon);
    }
}

void CBluetoothDevItem::SetName(const yl::string & strName)
{
    if (NULL != m_pName)
    {
        m_pName->SetText(strName);
    }
}

void CBluetoothDevItem::SetConnectStatusIcon(const yl::string & strConnectStatusIcon)
{
    if (NULL != m_pConnectStatusIcon)
    {
        m_pConnectStatusIcon->setPixmap(strConnectStatusIcon);
    }
}

void CBluetoothDevItem::SetMac(const yl::string & strMac)
{
    if (NULL != m_pMac)
    {
        m_pMac->SetText(strMac);
    }
}
#endif
