#include "lineselectadapter.h"
#include "talkuicommonfunction.h"

#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xWindowManager.h"

namespace talkui
{

CLineSelectAdapter::CLineSelectAdapter()
{
}


CLineSelectAdapter::~CLineSelectAdapter()
{
}

void CLineSelectAdapter::Init()
{
    m_vectorLineInfo.clear();
}

void CLineSelectAdapter::Uninit()
{
    m_vectorLineInfo.clear();
}

void CLineSelectAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    pListView->SetTotalDataCount(m_vectorLineInfo.size());
}

xListViewItem * CLineSelectAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    TALKUI_INFO("CLineSelectAdapter::PrepareItem nDataIndex = %d", nDataIndex);

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

bool CLineSelectAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
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
    if (0 <= nDataIndex && nDataIndex < m_vectorLineInfo.size())
    {
        pNormalItem->SetShowText(m_vectorLineInfo[nDataIndex]);
    }

    return true;
}

void CLineSelectAdapter::OnCleanContent(xListView * pListView)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return;
    }

    ResetIndex();
}

void CLineSelectAdapter::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
{
    xListViewAdapter::OnItemAction(pListView, pItem, nAction);

    TALKUI_INFO("CLineSelectAdapter::OnItemAction(xListView:%p Item:%p Action:%d)",
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

void CLineSelectAdapter::SetLineInfo(const talkui_vector<yl::string> & vectorLineInfo)
{
    m_vectorLineInfo.clear();

    int nLineInfoCount = vectorLineInfo.size();
    for (int i = 0; i < nLineInfoCount; i++)
    {
        //加入队列中
        m_vectorLineInfo.push_back(vectorLineInfo[i]);
    }

    CLineSelectAdapter::OnFlushList();
}

void CLineSelectAdapter::OnFlushList()
{
    RefreshListView();
}

}
