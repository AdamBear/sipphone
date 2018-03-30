#include "talkuisearchadapter.h"
#include "talkuicommonfunction.h"

#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xWindowManager.h"

namespace talkui
{

CTalkUISearchAdapter::CTalkUISearchAdapter()
{
    m_bSearchResult = false;
    m_nDataCount = 0;
}


CTalkUISearchAdapter::~CTalkUISearchAdapter()
{
}

void CTalkUISearchAdapter::Init()
{
    SetNoFocus();

    m_strSearchKey = "";

    m_vectorTransferData.clear();

    m_objProcessor.Init();

    m_objProcessor.SetSuperSearchReceiver(this);

    m_bSearchResult = false;
}

void CTalkUISearchAdapter::Uninit()
{
    m_objProcessor.Uninit();

    m_strSearchKey = "";

    m_vectorTransferData.clear();
}

void CTalkUISearchAdapter::SetNoFocus()
{
    if (NULL != m_pListView)
    {
        m_pListView->SelectItemByDataIndex(INVALID_ITEM_INDEX);
    }
}

bool CTalkUISearchAdapter::GetListHaveData()
{
    if (!m_bSearchResult)
    {
        if (m_strSearchKey.empty() && (m_vectorTransferData.size() > 0))
        {
            return true;
        }
    }
    else
    {
        if (m_nDataCount > 0
                && m_pListView != NULL
                && m_pListView->GetTotalDataCount() > 0)
        {
            return true;
        }
    }

    return false;
}

void CTalkUISearchAdapter::OnTextChange(const yl::string & strSearchData)
{
    m_bSearchResult = false;
    m_nDataCount = 0;

    m_strSearchKey = strSearchData;

    //清空数据
    m_objProcessor.SetSuperSearchResultEnable(false);
    //总是刷新显示
    CTalkUISearchAdapter::OnFlushList();

    //调用processor 去搜索数据
    m_objProcessor.DealSearchData(m_strSearchKey);
}

void CTalkUISearchAdapter::DealSuperSearchResult(bool bResult, msgObject & refMessage)
{
    TALKUI_INFO("-- DealSuperSearchResult %d", bResult);
    m_bSearchResult = bResult;

    //搜索结果返回了 ， 无论是搜索成功还是不成功 都需要改变显示数据
    //调用显示数据
    if (!bResult)       //搜索失败 那么清除数据
    {
        m_objProcessor.SetSuperSearchResultEnable(false);
    }

    //刷新显示
    CTalkUISearchAdapter::OnFlushList();

    signalSearchResultChange.emitSignal(0, 0, 0, NULL);
}

bool CTalkUISearchAdapter::GetFocusItemData(yl::string & strDisplayName, yl::string & strNumber,
        int & iCallID)
{
    if (NULL == m_pListView)
    {
        return false;
    }

    xListViewItem * pItem = m_pListView->GetFocusItem();
    CSearchWidgetItem * pSearchItem = CSearchWidgetItem::GetSearchItem(pItem);

    if (NULL == pSearchItem)
    {
        return false;
    }

    pSearchItem->GetContactData(strDisplayName, strNumber, iCallID);

    return true;
}

void CTalkUISearchAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    //重置数据
    m_nDataCount = 0;

    //先获取穿梭数据
    if (m_strSearchKey.empty() && m_vectorTransferData.size() > 0)
    {
        m_nDataCount = m_vectorTransferData.size();
    }

    //再取搜索数据
    SuperSearchItemListData objSuperSearchItemListData;
    //  objSuperSearchItemListData.m_strSearch = "";
    objSuperSearchItemListData.m_nCountToGet = 1;       //只取一个数据
    objSuperSearchItemListData.m_nIndexFrom = 0;
    objSuperSearchItemListData.m_nTotalCount = 0;   //初始化设置获取到的总数为0

    //调用processor 的接口取数据, 这边主要是给list 设置总数据个数

    //需要判断是否有取成功
    if (true == m_objProcessor.GetSuperSearchData(objSuperSearchItemListData))
    {
        if (objSuperSearchItemListData.m_nTotalCount > 0)
        {
            //加上搜索数量
            m_nDataCount += objSuperSearchItemListData.m_nTotalCount;
        }
    }

    pListView->SetTotalDataCount(m_nDataCount);
}

xListViewItem * CTalkUISearchAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    TALKUI_INFO("CTalkUISearchAdapter::PrepareItem nDataIndex = %d", nDataIndex);

    if (NULL == pListView || pListView != m_pListView)
    {
        return NULL;
    }

    if (NULL != pItem && pItem->GetType() == LIST_ITEM_TYPE_TALK_SEARCH)
    {
        return pItem;
    }

    xListViewItem * pNewItem = g_ItemFactory.ApplyItem(LIST_ITEM_TYPE_TALK_SEARCH);

    if (NULL != pNewItem)
    {
        pNewItem->SetAction(this);
    }

    return pNewItem;
}

bool CTalkUISearchAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return false;
    }

    CSearchWidgetItem * pSearchItem = CSearchWidgetItem::GetSearchItem(pItem);

    if (NULL == pSearchItem)
    {
        TALKUI_INFO("Get search item fail in refresh.");
        return false;
    }

    bool bResult = false;

    int nDataIndex = pSearchItem->GetDataIndex();

    //加载穿梭数据
    if (m_strSearchKey.empty() && m_vectorTransferData.size() > 0)
    {
        if (0 <= nDataIndex && nDataIndex < m_vectorTransferData.size())
        {
            pSearchItem->SetSearchTableItemData(m_vectorTransferData[nDataIndex]);

            //如果已经成功加载穿梭数据，则不需要再加载
            bResult = true;
        }
    }

    //如果不是穿梭数据，则加载搜索数据
    if (!bResult)
    {
        if (m_strSearchKey.empty() && m_vectorTransferData.size() > 0)
        {
            //减去穿梭数据的偏移
            nDataIndex -= m_vectorTransferData.size();
            if (nDataIndex < 0)
            {
                return false;
            }
        }

        SuperSearchItemListData objSuperSearchItemListData;
        //key值会在 processor 中赋值
        //  objSuperSearchItemListData.m_strSearch = "";
        objSuperSearchItemListData.m_nCountToGet = 1;       //每次取一个数据
        objSuperSearchItemListData.m_nIndexFrom = nDataIndex;
        objSuperSearchItemListData.m_nTotalCount = 0;   //初始化设置获取到的总数为0

        //需要判断是否有取成功
        if (true == m_objProcessor.GetSuperSearchData(objSuperSearchItemListData))
        {
            if (objSuperSearchItemListData.m_nTotalCount > 0)
            {
                YLList<DirectorySearchData>::iterator iter = objSuperSearchItemListData.m_listContact.begin();

                if (objSuperSearchItemListData.m_listContact.end() != iter)
                {
                    CSearchItemData objSearchItemData;
                    objSearchItemData.GetSearchItemDataFromDirectorySearchData(*iter);

                    pSearchItem->SetSearchTableItemData(objSearchItemData);
                }

                bResult = true;
            }
        }
    }

    return bResult;
}

void CTalkUISearchAdapter::OnCleanContent(xListView * pListView)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return;
    }

    ResetIndex();
}

void CTalkUISearchAdapter::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
{
    xListViewAdapter::OnItemAction(pListView, pItem, nAction);

    TALKUI_INFO("CTalkUISearchAdapter::OnItemAction(xListView:%p Item:%p Action:%d)",
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

void CTalkUISearchAdapter::SetTranCallData(YLList<CCallInfo *> & listTranCallInfo)
{
    talkui_vector<CSearchItemData> vectorTranData;

    //穿梭数据清除
    vectorTranData.clear();

    YLList<CCallInfo *>::iterator ite = listTranCallInfo.begin();

    for (; listTranCallInfo.end() != ite; ite++)
    {
        CCallInfo * pCallInfo = *ite;

        if (NULL != pCallInfo)
        {
            CSearchItemData objSearchItemData;

            objSearchItemData.m_strDisplayName = GetNameFromCallInfo(pCallInfo);    //设置显示名称
            objSearchItemData.m_strNumber = GetNumberFromCallInfo(pCallInfo);   //设置号码
            objSearchItemData.m_strContactImage = pCallInfo->m_strRemotePhoto;  //设置头像
            objSearchItemData.m_iCallID = pCallInfo->GetCallID();     //设置callid
            objSearchItemData.m_bHold = true;       //穿梭数据一定是hold的

            //插入到队列中
            vectorTranData.push_back(objSearchItemData);
        }
    }

    //两个数组不相同 才要继续， 防止重复设置穿梭数据
    if (false == IsContaceInfoVectorEqual(m_vectorTransferData, vectorTranData))
    {
        m_vectorTransferData = vectorTranData;

        //如果 搜索的 key 是空 才需要加载 穿梭数据 ，  要不然会导致数据显示紊乱
        if (m_strSearchKey.empty())
        {
            if (NULL != m_pListView)
            {
                m_pListView->ClearContent();
            }

            //调用显示数据
            CTalkUISearchAdapter::OnFlushList();
        }
    }

    signalSearchResultChange.emitSignal(0, 0, 0, NULL);
}

bool CTalkUISearchAdapter::IsContaceInfoVectorEqual(talkui_vector<CSearchItemData> &
        vectorTranData1, talkui_vector<CSearchItemData> & vectorTranData2)
{
    //如果个数不一样那么 肯定不是一样的
    if (vectorTranData1.size() != vectorTranData2.size())
    {
        return false;
    }

    int iCount = vectorTranData1.size();

    //循环查找 只要有一个不相同 那么就不相同
    for (int i = 0; i < iCount; i++)
    {
        if (!IsSearchItemDataEqual(vectorTranData1[i], vectorTranData2[i]))
            //  if(vectorTranData1[i] != vectorTranData2[i])
        {
            return false;
        }
    }

    return true;
}

void CTalkUISearchAdapter::OnFlushList()
{
    RefreshListView();
}

}
