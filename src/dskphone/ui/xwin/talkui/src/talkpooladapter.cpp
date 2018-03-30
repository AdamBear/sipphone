#include "talkpooladapter.h"
#include "talkuicommonfunction.h"

#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xWindowManager.h"

namespace talkui
{

CTalkPoolAdapter::CTalkPoolAdapter()
{
}


CTalkPoolAdapter::~CTalkPoolAdapter()
{
}

void CTalkPoolAdapter::Init()
{
    m_vectorDirInfo.clear();
}

void CTalkPoolAdapter::Uninit()
{
    m_vectorDirInfo.clear();
}

void CTalkPoolAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    pListView->SetTotalDataCount(m_vectorDirInfo.size());
}

xListViewItem * CTalkPoolAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    TALKUI_INFO("CTalkPoolAdapter::PrepareItem nDataIndex = %d", nDataIndex);

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

bool CTalkPoolAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
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
    if (0 <= nDataIndex && nDataIndex < m_vectorDirInfo.size())
    {
        pNormalItem->SetShowText(m_vectorDirInfo[nDataIndex]);
    }

    return true;
}

void CTalkPoolAdapter::OnCleanContent(xListView * pListView)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return;
    }

    ResetIndex();
}

void CTalkPoolAdapter::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
{
    xListViewAdapter::OnItemAction(pListView, pItem, nAction);

    TALKUI_INFO("CTalkPoolAdapter::OnItemAction(xListView:%p Item:%p Action:%d)",
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

void CTalkPoolAdapter::SetDirInfo(const talkui_vector<yl::string> & vectorDirInfo)
{
    m_vectorDirInfo.clear();

    int nLineInfoCount = vectorDirInfo.size();
    for (int i = 0; i < nLineInfoCount; i++)
    {
        //加入队列中
        m_vectorDirInfo.push_back(vectorDirInfo[i]);
    }

    RefreshListView();
}
}
