#include "pickupadapter.h"
#include "talkuicommonfunction.h"

#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xWindowManager.h"

namespace talkui
{

CPickUpAdapter::CPickUpAdapter()
{
}


CPickUpAdapter::~CPickUpAdapter()
{
}

void CPickUpAdapter::Init()
{
    m_vectorPickUpData.clear();
}

void CPickUpAdapter::Uninit()
{
    m_vectorPickUpData.clear();
}

void CPickUpAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    pListView->SetTotalDataCount(m_vectorPickUpData.size());
}

xListViewItem * CPickUpAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    TALKUI_INFO("CPickUpAdapter::PrepareItem nDataIndex = %d", nDataIndex);

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

bool CPickUpAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
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
    if (0 <= nDataIndex && nDataIndex < m_vectorPickUpData.size())
    {
        pNormalItem->SetShowText(m_vectorPickUpData[nDataIndex]);
    }

    return true;
}

void CPickUpAdapter::OnCleanContent(xListView * pListView)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return;
    }

    ResetIndex();
}

void CPickUpAdapter::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
{
    xListViewAdapter::OnItemAction(pListView, pItem, nAction);

    TALKUI_INFO("CPickUpAdapter::OnItemAction(xListView:%p Item:%p Action:%d)",
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

void CPickUpAdapter::SetPickUpInfo(const CPickUPMenuData * pPickUPMenuData)
{
    m_vectorPickUpData.clear();

    int iPickUpDataCount = pPickUPMenuData->vectorMenuName.size();
    for (int i = 0; i < iPickUpDataCount; i++)
    {
        char szIndex[8] = { 0 };
        sprintf(szIndex, "%d.", i + 1);
        yl::string strMenuString = yl::string(szIndex) + pPickUPMenuData->vectorMenuName[i];

        //加入队列中
        m_vectorPickUpData.push_back(strMenuString);
    }

    RefreshListView();
}

}
