#include "rtpstatusadapter.h"
#include "talkuicommonfunction.h"

#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xWindowManager.h"

namespace talkui
{

CRTPStatusAdapter::CRTPStatusAdapter()
{
    m_pScrollBar = NULL;
    m_nPageIndex = 0;
}


CRTPStatusAdapter::~CRTPStatusAdapter()
{
    m_pScrollBar = NULL;
}

void CRTPStatusAdapter::Init()
{
    m_vectorRTPStatusData.clear();
    m_nPageIndex = 0;
}

void CRTPStatusAdapter::Uninit()
{
    m_vectorRTPStatusData.clear();
}

void CRTPStatusAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    pListView->SetTotalDataCount(m_vectorRTPStatusData.size());
}

xListViewItem * CRTPStatusAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    TALKUI_INFO("CRTPStatusAdapter::PrepareItem nDataIndex = %d", nDataIndex);

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

bool CRTPStatusAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
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
    if (0 <= nDataIndex && nDataIndex < m_vectorRTPStatusData.size())
    {
        pNormalItem->SetShowText(
            m_vectorRTPStatusData[nDataIndex].strTag
            + m_vectorRTPStatusData[nDataIndex].strValue);
    }

    return true;
}

void CRTPStatusAdapter::OnCleanContent(xListView * pListView)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return;
    }

    ResetIndex();
}

void CRTPStatusAdapter::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
{
    xListViewAdapter::OnItemAction(pListView, pItem, nAction);

    TALKUI_INFO("CRTPStatusAdapter::OnItemAction(xListView:%p Item:%p Action:%d)",
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

void CRTPStatusAdapter::SetRTPStatusData(const talkui_vector<CRTPStatusData> & vectorRTPStatusData)
{
    m_vectorRTPStatusData.clear();
    if (NULL != m_pListView)
    {
        m_pListView->RefreshData(0);
    }

    int nSize = vectorRTPStatusData.size();
    for (int i = 0; i < nSize; ++i)
    {
        //放到队列中
        m_vectorRTPStatusData.push_back(vectorRTPStatusData[i]);
    }

    if (NULL != m_pScrollBar)
    {
        m_pScrollBar->show(xView::SHOW_CMD_BRING_TO_TOP);
    }

    CRTPStatusAdapter::OnFlushList();
}

void CRTPStatusAdapter::OnFlushList()
{
//      RefreshListView();

    if (NULL == m_pListView)
    {
        return FALSE;
    }

    m_pListView->RefreshData(m_nPageIndex);

    if (NULL != m_pScrollBar)
    {
        int nTotalPage = 0;
        int nPageCount = m_pListView->GetStyle()->GetPageItemCount();

        if (nPageCount > 0)
        {
            nTotalPage = m_vectorRTPStatusData.size() / nPageCount;
            nTotalPage++;
        }

        if (nTotalPage > 1)
        {
            m_pScrollBar->show(xView::SHOW_CMD_BRING_TO_TOP);
            m_pScrollBar->SetScrollInfo(1, nTotalPage, m_nPageIndex / nPageCount);
        }
        else
        {
            m_pScrollBar->hide(true);
        }
    }

    return FALSE;
}
}
