#include "acdinfoadapter.h"
#include "talkuicommonfunction.h"

#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xWindowManager.h"

namespace talkui
{

CACDInfoAdapter::CACDInfoAdapter()
{
    m_pScrollBar = NULL;
}


CACDInfoAdapter::~CACDInfoAdapter()
{
    m_pScrollBar = NULL;
}

void CACDInfoAdapter::Init()
{
    m_vectorACDInfoData.clear();
}

void CACDInfoAdapter::Uninit()
{
    m_vectorACDInfoData.clear();
}

void CACDInfoAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    pListView->SetTotalDataCount(m_vectorACDInfoData.size());
}

xListViewItem * CACDInfoAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    TALKUI_INFO("CACDInfoAdapter::PrepareItem nDataIndex = %d", nDataIndex);

    if (NULL == pListView || pListView != m_pListView)
    {
        return NULL;
    }

    if (NULL != pItem && pItem->GetType() == LIST_ITEM_TYPE_NORMAL_ITEM)
    {
        return pItem;
    }

    xListViewItem * pNewItem = g_ItemFactory.ApplyItem(LIST_ITEM_TYPE_NORMAL_ITEM);

    if (NULL != pNewItem)
    {
        pNewItem->SetAction(this);
    }

    return pNewItem;
}

bool CACDInfoAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return false;
    }

    CTalkNormalItem * pNormalItem = CTalkNormalItem::GetNormalItem(pItem);

    if (NULL == pNormalItem)
    {
        TALKUI_INFO("Get Normal item fail in refresh.");
        return false;
    }

    int nDataIndex = pNormalItem->GetDataIndex();

    //加载数据
    if (0 <= nDataIndex && nDataIndex < m_vectorACDInfoData.size())
    {
        pNormalItem->SetShowText(m_vectorACDInfoData[nDataIndex]);
    }

    return true;
}

void CACDInfoAdapter::OnCleanContent(xListView * pListView)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return;
    }

    ResetIndex();
}

void CACDInfoAdapter::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
{
    xListViewAdapter::OnItemAction(pListView, pItem, nAction);

    TALKUI_INFO("CACDInfoAdapter::OnItemAction(xListView:%p Item:%p Action:%d)",
                pListView, pItem, nAction);

    if (NULL == pListView || pListView != m_pListView || NULL == pItem)
    {
        return;
    }

    if (LIST_VIEW_ITEM_ACTION_FOCUS == nAction)
    {
        UpdateIndex();

        return;
    }
}

void CACDInfoAdapter::SetACDInfo(const yl::string & strWaitTime, const yl::string & strCCName,
                                 const yl::string & strCCUID, const yl::string & strQueue, const yl::string & strCallsLongest)
{
    m_vectorACDInfoData.clear();

    //waittime
    m_vectorACDInfoData.push_back(strWaitTime);
    //ccname
    m_vectorACDInfoData.push_back(strCCName);
    //ccuid
    m_vectorACDInfoData.push_back(strCCUID);
    //queue
    m_vectorACDInfoData.push_back(strQueue);
    //callslongest
    //不为空 才显示
    if (!strCallsLongest.empty())
    {
        m_vectorACDInfoData.push_back(strCallsLongest);
    }
    else    //为空不显示
    {
        //
    }

    if (NULL != m_pScrollBar)
    {
        m_pScrollBar->show(xView::SHOW_CMD_BRING_TO_TOP);
    }

    CACDInfoAdapter::OnFlushList();
}

void CACDInfoAdapter::OnFlushList()
{
    RefreshListView();

    if (NULL != m_pScrollBar)
    {
        int nTotalPage = 0;
        int nPageData = 0;
        nPageData = m_pListView->GetPageDataIndex();

        int nPageCount = m_pListView->GetStyle()->GetPageItemCount();

        if (nPageCount > 0)
        {
            nTotalPage = m_vectorACDInfoData.size() / nPageCount;
            nTotalPage++;
        }

        if (nTotalPage > 1)
        {
            m_pScrollBar->show(xView::SHOW_CMD_BRING_TO_TOP);
            m_pScrollBar->SetScrollInfo(1, nTotalPage, nPageData / nPageCount);
        }
        else
        {
            m_pScrollBar->hide(true);
        }
    }

    return FALSE;
}
}
