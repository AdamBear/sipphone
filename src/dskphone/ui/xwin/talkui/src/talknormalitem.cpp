#include "talknormalitem.h"
#include "talkuicommonfunction.h"

#include "talkuixmlpath.h"
#include "talkuixmlitemname.h"

namespace talkui
{
CTalkNormalItem::CTalkNormalItem(IListViewItemAction * pAction/* = NULL*/)
    : xListViewItem(LIST_ITEM_TYPE_NORMAL_ITEM, pAction)
    , m_pText(NULL)
{
}


CTalkNormalItem::~CTalkNormalItem()
{
}

int CTalkNormalItem::GetNormalItemType()
{
    return LIST_ITEM_TYPE_NORMAL_ITEM;
}

CTalkNormalItem * CTalkNormalItem::GetNormalItem(xListViewItem * pItem)
{
    if (NULL == pItem || LIST_ITEM_TYPE_NORMAL_ITEM != pItem->GetType())
    {
        return NULL;
    }

    return (CTalkNormalItem *)pItem;
}

xListViewItem * CTalkNormalItem::CreateNormalItem()
{
    CTalkNormalItem * pListItem = new CTalkNormalItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent(TALKUI_NORMAL_ITEM_XML_PATH);
    }

    return pListItem;
}

void CTalkNormalItem::loadChildrenElements(xml_node & node)
{
    xListViewItem::loadChildrenElements(node);

    m_pText = static_cast<xTextView *>(getViewById(TALKUI_NORMAL_ITEM_TEXT));
}

void CTalkNormalItem::SetShowText(const yl::string & strText)
{
    //设置名称
    if (NULL != m_pText)
    {
        m_pText->show(xView::SHOW_CMD_BRING_TO_TOP);
        m_pText->SetText(strText);
    }
}

void CTalkNormalItem::SetTextAlign(int nAlign)
{
    //设置名称
    if (NULL != m_pText)
    {
        m_pText->SetAlign(nAlign);
        m_pText->SetGravity(nAlign);
    }
}

}
