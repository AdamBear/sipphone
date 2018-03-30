#include "scamenuadapter.h"
#include "talkuicommonfunction.h"

#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xWindowManager.h"

namespace talkui
{

CSCAMenuAdapter::CSCAMenuAdapter()
{
}


CSCAMenuAdapter::~CSCAMenuAdapter()
{
}

void CSCAMenuAdapter::Init()
{
    m_vectorSCAMenuData.clear();
}

void CSCAMenuAdapter::Uninit()
{
    m_vectorSCAMenuData.clear();
}

void CSCAMenuAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    pListView->SetTotalDataCount(m_vectorSCAMenuData.size());
}

xListViewItem * CSCAMenuAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    TALKUI_INFO("CSCAMenuAdapter::PrepareItem nDataIndex = %d", nDataIndex);

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

bool CSCAMenuAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
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
    if (0 <= nDataIndex && nDataIndex < m_vectorSCAMenuData.size())
    {
        pNormalItem->SetShowText(m_vectorSCAMenuData[nDataIndex]);
    }

    return true;
}

void CSCAMenuAdapter::OnCleanContent(xListView * pListView)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return;
    }

    ResetIndex();
}

void CSCAMenuAdapter::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
{
    xListViewAdapter::OnItemAction(pListView, pItem, nAction);

    TALKUI_INFO("CSCAMenuAdapter::OnItemAction(xListView:%p Item:%p Action:%d)",
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

void CSCAMenuAdapter::SetSCAMenuInfo(const CSCAMenuUIData * pSCAMenuUIData)
{
    m_vectorSCAMenuData.clear();

    int iSCADataCount = pSCAMenuUIData->vectorSCAMenuItemData.size();
    for (int i = 0; i < iSCADataCount; i++)
    {
        const CSCAMenuItemData & objSCAMenuItemData = pSCAMenuUIData->vectorSCAMenuItemData[i];
        char szIndex[8] = { 0 };
        sprintf(szIndex, "%d.", i + 1);
        yl::string strSCAData = yl::string(szIndex) + objSCAMenuItemData.strStatus;

        strSCAData = strSCAData + " : ";
        strSCAData = strSCAData + objSCAMenuItemData.strDisplayName;

        //加入队列中
        m_vectorSCAMenuData.push_back(strSCAData);
    }

    RefreshListView();
}
}
