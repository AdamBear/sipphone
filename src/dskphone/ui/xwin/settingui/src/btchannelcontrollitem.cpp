#include "btchannelcontrollitem.h"
#include "xwindow/xWindow.h"

#ifdef IF_SUPPORT_BLUETOOTH
CBTChannelControllItem::CBTChannelControllItem(IListViewItemAction * pAction/* = NULL*/)
    : CComboBoxListItem(pAction)
    , m_pExtraName(NULL)
    , m_pExtraValue(NULL)
{
    m_nType |= LIST_ITEM_BT_CHANNEL_CONTROLL;
}

CBTChannelControllItem::~CBTChannelControllItem()
{
}

int CBTChannelControllItem::GetBTChannelControllItemType()
{
    return (LIST_ITEM_TYPE_DETAIL | LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX | LIST_ITEM_BT_CHANNEL_CONTROLL);
}

CBTChannelControllItem * CBTChannelControllItem::GetBTChannelControllItem(xListViewItem * pItem)
{
    if (NULL == pItem || GetBTChannelControllItemType() != pItem->GetType())
    {
        return NULL;
    }

    return (CBTChannelControllItem *)pItem;
}

xListViewItem * CBTChannelControllItem::CreateBTChannelControllItem()
{
    CBTChannelControllItem * pListItem = new CBTChannelControllItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent("setting/btchannelcontrollitem.xml");
    }

    return pListItem;
}

void CBTChannelControllItem::SetItemFocus(bool bFocus)
{
    CComboBoxListItem::SetItemFocus(bFocus);
    update();
}

void CBTChannelControllItem::loadChildrenElements(xml_node & node)
{
    CComboBoxListItem::loadChildrenElements(node);

    m_pExtraName = static_cast<xTextView *>(getViewById("txtdetailTitle"));

    m_pExtraValue = static_cast<xTextView *>(getViewById("txtdetailValue"));
}

void CBTChannelControllItem::SetExtraName(const yl::string & strExtraName)
{
    if (NULL != m_pExtraName)
    {
        yl::string strText = strExtraName;
        strText += ":";
        m_pExtraName->SetText(strText);
    }
}

void CBTChannelControllItem::SetExtraValue(const yl::string & strExtraValue)
{
    if (NULL != m_pExtraValue)
    {
        m_pExtraValue->SetText(strExtraValue);
    }
}
#endif
