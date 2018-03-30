#include "wifinetitem.h"

#ifdef IF_SUPPORT_WIFI
CWifiNetItem::CWifiNetItem(IListViewItemAction * pAction/* = NULL*/)
    : xListViewItem(LIST_ITEM_TYPE_WIFI_NET, pAction)
    , m_pStrengthIcon(NULL)
    , m_pName(NULL)
    , m_pConnectStatusIcon(NULL)
    , m_pEncryptIcon(NULL)
{
}

CWifiNetItem::~CWifiNetItem()
{
}

int CWifiNetItem::GetWifiNetItemType()
{
    return LIST_ITEM_TYPE_WIFI_NET;
}

CWifiNetItem * CWifiNetItem::GetWifiNetItem(xListViewItem * pItem)
{
    if (NULL == pItem
            || CWifiNetItem::GetWifiNetItemType() != pItem->GetType())
    {
        return NULL;
    }

    return (CWifiNetItem *)pItem;
}

xListViewItem * CWifiNetItem::CreateWifiNetItem()
{
    CWifiNetItem * pListItem = new CWifiNetItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent("setting/wifinetitem.xml");
    }

    return pListItem;
}

void CWifiNetItem::loadChildrenElements(xml_node & node)
{
    xListViewItem::loadChildrenElements(node);

    m_pStrengthIcon = static_cast<xImageView *>(getViewById("picSignalStrengthIcon"));
    m_pName = static_cast<xTextView *>(getViewById("txtName"));
    m_pConnectStatusIcon = static_cast<xImageView *>(getViewById("picConnectStatusIcon"));
    m_pEncryptIcon = static_cast<xImageView *>(getViewById("picEncryptIcon"));
}

void CWifiNetItem::ShowConnectStatusIcon(bool bShowConnectStatusIcon)
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

void CWifiNetItem::SetSignalStrengthIcon(const yl::string & strSignalStrengthIcon)
{
    if (NULL != m_pStrengthIcon)
    {
        if (strSignalStrengthIcon.empty())
        {
            m_pStrengthIcon->hide(true);
        }
        else
        {
            m_pStrengthIcon->setPixmap(strSignalStrengthIcon);
            m_pStrengthIcon->show();
        }
    }
}

void CWifiNetItem::SetName(const yl::string & strName)
{
    if (NULL != m_pName)
    {
        m_pName->SetText(strName);
    }
}

void CWifiNetItem::SetConnectStatusIcon(const yl::string & strConnectStatusIcon)
{
    if (NULL != m_pConnectStatusIcon)
    {
        m_pConnectStatusIcon->setPixmap(strConnectStatusIcon);
    }
}

void CWifiNetItem::SetEncryptIcon(const yl::string & strEncryptIcon)
{
    if (NULL != m_pEncryptIcon)
    {
        m_pEncryptIcon->setPixmap(strEncryptIcon);
    }
}

bool CWifiNetItem::onFocusEvent(bool bFocusIn)
{
    if (NULL != m_pName)
    {
        if (bFocusIn)
        {
            m_pName->EnableScroll(true);
            m_pName->SetOverLengthStyle(OVERLENGTH_TYPE_SCROLL);
        }
        else
        {
            m_pName->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
        }
    }

    return xListViewItem::onFocusEvent(bFocusIn);
}

#endif
