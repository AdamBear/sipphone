#include "settinguiscrollbypageadapter.h"
#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xListView.h"
#include "xwindow/xlistviewstyle.h"
#include "pingandtraceresultitem.h"
#include "settingui/include/pageiddefine.h"
#include "settinguiscrollbypagedelegatebase.h"

namespace
{
#define PAGE_ITEM_COUNT 4
}

CSettinguiScrollByPageAdapter::CSettinguiScrollByPageAdapter()
{

}

CSettinguiScrollByPageAdapter::~CSettinguiScrollByPageAdapter()
{

}

CSettingUIAdapter * CSettinguiScrollByPageAdapter::CreateScrollByPageAdapter()
{
    CSettinguiScrollByPageAdapter * pAdapter = new CSettinguiScrollByPageAdapter;

    return pAdapter;
}


void CSettinguiScrollByPageAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView || !IsScrollBypageDelegate())
    {
        return;
    }

    CSettinguiScrollByPageDelegateBase * pDelegate = (CSettinguiScrollByPageDelegateBase *)
            m_pSettingUIDelegate;
    if (NULL != pDelegate)
    {
        int nTotal = pDelegate->GetTotalItemCount();
        pListView->SetTotalDataCount(nTotal);
    }
}

xListViewItem * CSettinguiScrollByPageAdapter::PrepareItem(xListView * pListView, int nDataIndex,
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

bool CSettinguiScrollByPageAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView ||  NULL == pItem)
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
        if (IsScrollBypageDelegate())
        {
            CSettinguiScrollByPageDelegateBase * pDelegate = (CSettinguiScrollByPageDelegateBase *)
                    m_pSettingUIDelegate;
            if (NULL != pDelegate)
            {
                strText = pDelegate->GetValueByIndex(nDataIndex);
            }
        }

        pResultItem->SetValue(strText);
    }

    return true;
}

bool CSettinguiScrollByPageAdapter::IsScrollBypageDelegate()
{
    if (NULL != m_pSettingUIDelegate)
    {
        yl::string strPageID = m_pSettingUIDelegate->GetPageID();
        if (kszPageIDPingResult == strPageID
                || kszPageIDTraceResult == strPageID
                || kszPageIDStatusRTP == strPageID
                || kszPageIDTextMsgView == strPageID)
        {
            return true;
        }
    }

    return false;
}
