#include "netconflistadapter.h"
#include "talkuicommonfunction.h"

#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xWindowManager.h"
#include "uimanager/moduimanager.h"

namespace talkui
{

CNetConfListAdapter::CNetConfListAdapter()
    : m_nAlign(0)
{
}


CNetConfListAdapter::~CNetConfListAdapter()
{
}

void CNetConfListAdapter::Init()
{
    m_vectorNetConfList.clear();
}

void CNetConfListAdapter::Uninit()
{
    m_vectorNetConfList.clear();
}

void CNetConfListAdapter::SetTextAlign(int nAlign)
{
    m_nAlign = nAlign;
}

int CNetConfListAdapter::GetFocusCallID()
{
    if (NULL == m_pListView)
    {
        return false;
    }

    xListViewItem * pItem = m_pListView->GetFocusItem();

    CTalkNormalItem * pNormalItem = CTalkNormalItem::GetNormalItem(pItem);

    if (NULL == pNormalItem)
    {
        TALKUI_INFO("Get Net Conf item fail in GetFocusCallID.");
        return -1;
    }

    return pNormalItem->GetDataIndex();
}

void CNetConfListAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    pListView->SetTotalDataCount(m_vectorNetConfList.size());
}

xListViewItem * CNetConfListAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    TALKUI_INFO("CNetConfListAdapter::PrepareItem nDataIndex = %d", nDataIndex);

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

bool CNetConfListAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return false;
    }

    CTalkNormalItem * pNormalItem = CTalkNormalItem::GetNormalItem(pItem);

    if (NULL == pNormalItem)
    {
        TALKUI_INFO("Get Net Conf item fail in refresh.");
        return false;
    }

    int nDataIndex = pNormalItem->GetDataIndex();

    //加载数据
    if (0 <= nDataIndex && nDataIndex < m_vectorNetConfList.size())
    {
        pNormalItem->SetTextAlign(m_nAlign);
        pNormalItem->SetShowText(m_vectorNetConfList[nDataIndex]);
    }

    return true;
}

void CNetConfListAdapter::OnCleanContent(xListView * pListView)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return;
    }

    ResetIndex();
}

void CNetConfListAdapter::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
{
    xListViewAdapter::OnItemAction(pListView, pItem, nAction);

    TALKUI_INFO("CNetConfListAdapter::OnItemAction(xListView:%p Item:%p Action:%d)",
                pListView, pItem, nAction);

    if (NULL == pListView || pListView != m_pListView || NULL == pItem)
    {
        return;
    }

    if (LIST_VIEW_ITEM_ACTION_FOCUS == nAction)
    {
        UpdateIndex();

        CBaseDialog * pTopDlg = UIManager_GetTopWindow();
        if (NULL != pTopDlg && eWT_Talk == pTopDlg->GetWindowType())
        {
            pTopDlg->RefreshSoftkey();
        }

        return;
    }
}

void CNetConfListAdapter::SetNetConfListData(talkui_vector<yl::string> & listContactInfo)
{
    m_vectorNetConfList.clear();

    m_vectorNetConfList = listContactInfo;

    CNetConfListAdapter::OnFlushList();
}

void CNetConfListAdapter::OnFlushList()
{
    RefreshListView();

    if (NULL != m_pListView)
    {
        m_pListView->setFocus();
    }
}

}
