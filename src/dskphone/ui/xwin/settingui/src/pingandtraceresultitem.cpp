#include "pingandtraceresultitem.h"
#include "baseui/include/baseuicommon.h"


CPingAndTraceoutItem::CPingAndTraceoutItem(IListViewItemAction * pAction/* = NULL*/)
    : xListViewItem(LIST_ITEM_TYPE_PING_AND_TRACE, pAction)
    , m_pText(NULL)
{
}

CPingAndTraceoutItem::~CPingAndTraceoutItem()
{
}

int CPingAndTraceoutItem::GetPingAndTraceoutItemType()
{
    return LIST_ITEM_TYPE_PING_AND_TRACE;
}

CPingAndTraceoutItem * CPingAndTraceoutItem::GetPingAndTraceoutListItem(xListViewItem * pItem)
{
    if (NULL == pItem || LIST_ITEM_TYPE_PING_AND_TRACE != pItem->GetType())
    {
        return NULL;
    }

    return (CPingAndTraceoutItem *)pItem;
}

xListViewItem * CPingAndTraceoutItem::CreatePingAndTraceoutListItem()
{
    CPingAndTraceoutItem * pListItem = new CPingAndTraceoutItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent("setting/pingandtraceoutitem.xml");
    }

    return pListItem;
}

void CPingAndTraceoutItem::loadChildrenElements(xml_node & node)
{
    xListViewItem::loadChildrenElements(node);

    m_pText = static_cast<xTextView *>(getViewById("txtresult"));
}

void CPingAndTraceoutItem::SetItemFocus(bool bFocus)
{

}

// 设置标题
void CPingAndTraceoutItem::SetValue(const yl::string & strValue)
{
    if (NULL != m_pText)
    {
        m_pText->SetText(strValue);
        return;
    }
}

