#include "textmessageviewadapter.h"
#include "baseui/include/listviewitemfactory.h"
#include "textmessageviewdelegate.h"
#include "xwindow/xListView.h"
#include "xwindow/xlistviewstyle.h"
#include "pingandtraceresultitem.h"

namespace
{
#define PAGE_ITEM_COUNT 4
}

CTextMessageViewAdapter::CTextMessageViewAdapter()
{

}

CTextMessageViewAdapter::~CTextMessageViewAdapter()
{

}

CSettingUIAdapter * CTextMessageViewAdapter::CreateTextMessageViewAdapter()
{
    CTextMessageViewAdapter * pAdapter = new CTextMessageViewAdapter;

    return pAdapter;
}


void CTextMessageViewAdapter::InitListViewLayout(xListView * pListView)
{
    CTextMessageViewDelegate * pDelegate = static_cast<CTextMessageViewDelegate *>
                                           (m_pSettingUIDelegate);
    if (NULL == pListView || NULL == pDelegate)
    {
        return;
    }

    int nTotal = pDelegate->GetTotalItemCount();
    pListView->SetTotalDataCount(nTotal);
}

xListViewItem * CTextMessageViewAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    if (NULL == pListView)
    {
        return NULL;
    }

    CPingAndTraceoutItemPtr pResultItem = CPingAndTraceoutItem::GetPingAndTraceoutListItem(pItem);

    if (NULL == pResultItem)
    {
        pResultItem = static_cast<CPingAndTraceoutItemPtr>(g_ItemFactory.ApplyItem(
                          CPingAndTraceoutItem::GetPingAndTraceoutItemType()));

        if (NULL == pResultItem)
        {
            return NULL;
        }
    }

    pResultItem->SetAction(this);
    return pResultItem;
}

bool CTextMessageViewAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || NULL == pItem)
    {
        return false;
    }

    // 设置Item数据
    int nDataIndex = pItem->GetDataIndex();
    //设置相应数据
    CPingAndTraceoutItemPtr pResultItem = CPingAndTraceoutItem::GetPingAndTraceoutListItem(pItem);

    if (NULL != pResultItem)
    {
        pResultItem->SetDataIndex(nDataIndex);

        yl::string strText = "";
        CTextMessageViewDelegate * pDelegate = static_cast<CTextMessageViewDelegate *>
                                               (m_pSettingUIDelegate);
        if (NULL != pDelegate)
        {
            strText = pDelegate->GetValueByIndex(nDataIndex);
        }

        pResultItem->SetValue(strText);
    }

    return true;
}
