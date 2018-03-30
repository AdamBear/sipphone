#include "confmanageradapter.h"
#include "talkuicommonfunction.h"

#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xWindowManager.h"
#include "uimanager/moduimanager.h"

namespace talkui
{

CConfManagerAdapter::CConfManagerAdapter()
{
}


CConfManagerAdapter::~CConfManagerAdapter()
{
}

void CConfManagerAdapter::Init()
{
    m_vectorConfManangerData.clear();
}

void CConfManagerAdapter::Uninit()
{
    m_vectorConfManangerData.clear();
}

int CConfManagerAdapter::GetFocusCallID()
{
    if (NULL == m_pListView)
    {
        return false;
    }

    xListViewItem * pItem = m_pListView->GetFocusItem();

    CConfManagerItem * pConfManagerItem = CConfManagerItem::GetConfManagerItem(pItem);

    if (NULL == pConfManagerItem)
    {
        TALKUI_INFO("Get Conf Manager item fail in GetFocusCallID.");
        return -1;
    }

    return pConfManagerItem->GetContactCallID();
}

void CConfManagerAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    pListView->SetTotalDataCount(m_vectorConfManangerData.size());
}

xListViewItem * CConfManagerAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    TALKUI_INFO("CConfManagerAdapter::PrepareItem nDataIndex = %d", nDataIndex);

    if (NULL == pListView || pListView != m_pListView)
    {
        return NULL;
    }

    if (NULL != pItem && pItem->GetType() == LIST_ITEM_TYPE_CONF_MANAGER)
    {
        return pItem;
    }

    xListViewItem * pNewItem = g_ItemFactory.ApplyItem(LIST_ITEM_TYPE_CONF_MANAGER);

    if (NULL != pNewItem)
    {
        pNewItem->SetAction(this);
    }

    return pNewItem;
}

bool CConfManagerAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return false;
    }

    CConfManagerItem * pConfManagerItem = CConfManagerItem::GetConfManagerItem(pItem);

    if (NULL == pConfManagerItem)
    {
        TALKUI_INFO("Get Conf Manager item fail in refresh.");
        return false;
    }

    int nDataIndex = pConfManagerItem->GetDataIndex();

    //加载数据
    if (0 <= nDataIndex && nDataIndex < m_vectorConfManangerData.size())
    {
        pConfManagerItem->SetConfManangerContactInfo(m_vectorConfManangerData[nDataIndex]);
    }

    return true;
}

void CConfManagerAdapter::OnCleanContent(xListView * pListView)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return;
    }

    ResetIndex();
}

void CConfManagerAdapter::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
{
    xListViewAdapter::OnItemAction(pListView, pItem, nAction);

    TALKUI_INFO("CConfManagerAdapter::OnItemAction(xListView:%p Item:%p Action:%d)",
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

void CConfManagerAdapter::SetConfManagerData(talkui_vector<CContactInfo> & listContactInfo,
        bool bRefreshAll/* = true*/)
{
    m_vectorConfManangerData.clear();

    talkui_vector<CContactInfo>::iterator listIterator = listContactInfo.begin();
    int nIndexNumber = 1;
    char szIndex[10] = { 0 };
    yl::string strIndexText = "";


    for (; listContactInfo.end() != listIterator; listIterator++, nIndexNumber++)
    {
        CConfManangerData objConfManangerData;
        CContactInfo & objContactInfo = *listIterator;

        objConfManangerData.m_iCallID = objContactInfo.iCallID;

        sprintf(szIndex, " %d.", nIndexNumber);
        strIndexText = szIndex;
        objConfManangerData.m_strText = strIndexText + objContactInfo.strName;

        objConfManangerData.m_eAudioCodec = objContactInfo.m_eAudioCodec;
        objConfManangerData.m_bHeld = objContactInfo.bRemoteHold;
        objConfManangerData.m_bFarMute = objContactInfo.bFarMute;
        objConfManangerData.m_bFarHold = objContactInfo.bLocalHold || objContactInfo.bFarHold;
        objConfManangerData.m_bReceived = objContactInfo.m_bReceived;

        //放到队列中
        m_vectorConfManangerData.push_back(objConfManangerData);
    }

    CConfManagerAdapter::OnFlushList();
}

void CConfManagerAdapter::OnFlushList()
{
    RefreshListView();

    if (NULL != m_pListView)
    {
        m_pListView->setFocus();
    }
}

}
