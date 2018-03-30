#include "radiolistitem.h"
#include "xwindow/xWindow.h"
#include "baseuicommon.h"

namespace
{
#define RADIO_BTN_ID       "radiobtn"
}

CRadioListItem::CRadioListItem(IListViewItemAction * pAction/* = NULL*/)
    : CMenuListItem(pAction)
    , m_pRadioBtn(NULL)
{
    m_nType |= LIST_ITEM_SUB_TYPE_MENU_RADIO;
}

CRadioListItem::~CRadioListItem()
{

}

int CRadioListItem::GetRadioItemType()
{
    return (LIST_ITEM_TYPE_MENU | LIST_ITEM_SUB_TYPE_MENU_RADIO);
}

CRadioListItem * CRadioListItem::GetRadioListItem(xListViewItem * pItem)
{
    int nRadioItemType = GetRadioItemType();

    if (NULL == pItem && nRadioItemType != (pItem->GetType() & nRadioItemType))
    {
        return NULL;
    }

    return static_cast<CRadioListItem *>(pItem);
}

xListViewItem * CRadioListItem::CreateRadioListItem()
{
    CRadioListItem * pRadioItem = new CRadioListItem;

    if (NULL != pRadioItem)
    {
        pRadioItem->loadContent(BASE_UI_LAYOUT_FILE_RADIO_LIST_ITEM);
    }

    return pRadioItem;
}

void CRadioListItem::loadChildrenElements(xml_node & node)
{
    CMenuListItem::loadChildrenElements(node);

    m_pRadioBtn = static_cast<xRadioButton*>(getViewById(BASE_UI_ID_BTN_RADIO));
}

void CRadioListItem::SetCheck(bool bCheck)
{
    if (NULL == m_pRadioBtn)
    {
        return;
    }

    if (bCheck)
    {
        m_pRadioBtn->SetChecked(true);
    }
    else
    {
        m_pRadioBtn->SetChecked(false);
    }
}
