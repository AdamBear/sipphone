#include "framelist.h"
#include "framelistdelegatebase.h"

CFrameListDelegateBase::CFrameListDelegateBase(void)
    : m_pFrameList(NULL)
    , m_nPageDataIndex(0)
    , m_bItemFocus(true)
    , m_bDefaultFocus(true)
    , m_nFocusIndex(0)
{
}

CFrameListDelegateBase::~CFrameListDelegateBase(void)
{
    if (NULL != m_pFrameList)
    {
        m_pFrameList->ResetAction();
        m_pFrameList->ResetDelegate();
        m_pFrameList = NULL;
    }
}

void CFrameListDelegateBase::SetFrameList(CFrameList * pFrameList)
{
    bool bChangeList = false;

    if (pFrameList != m_pFrameList)
    {
        bChangeList = true;
    }

    if (bChangeList && NULL != m_pFrameList)
    {
        m_pFrameList->ResetAction();
        m_pFrameList->ResetDelegate();
    }

    m_pFrameList = pFrameList;

    IFrameListDelegatePtr pDelegate = GetDelegate();

    if (bChangeList && NULL != m_pFrameList && m_pFrameList->GetFrameDelegate() != pDelegate)
    {
        m_pFrameList->SetDelegate(pDelegate);
    }
}

CFrameList * CFrameListDelegateBase::GetFrameList() const
{
    return m_pFrameList;
}

void CFrameListDelegateBase::SetPageDataIndex(int nDataIndex)
{
    m_nPageDataIndex = nDataIndex;

    if (NULL == m_pFrameList)
    {
        return;
    }

    int nMaxPageItemCount = m_pFrameList->GetMaxPageItemCount();

    if (nMaxPageItemCount <= 0)
    {
        return;
    }

    m_nPageDataIndex = m_nPageDataIndex - (m_nPageDataIndex % nMaxPageItemCount);
}

void CFrameListDelegateBase::SetDefaultFocus(bool bDefaultFocus)
{
    m_bDefaultFocus = bDefaultFocus;

    if (bDefaultFocus)
    {
        if (INVALID_ITEM_INDEX == m_nFocusIndex)
        {
            m_nFocusIndex = 0;
        }
    }
    else if (0 == m_nFocusIndex)
    {
        m_nFocusIndex = INVALID_ITEM_INDEX;
    }
}

void CFrameListDelegateBase::SetFocusIndex(int nIndex)
{
    m_nFocusIndex = nIndex;

    if (NULL == m_pFrameList)
    {
        return;
    }

    int nMaxPageItemCount = m_pFrameList->GetMaxPageItemCount();

    if (nMaxPageItemCount <= 0 || m_nFocusIndex < nMaxPageItemCount)
    {
        return;
    }

    m_nFocusIndex = m_nFocusIndex % nMaxPageItemCount;
}

void CFrameListDelegateBase::ResetIndex()
{
    m_nPageDataIndex = 0;

    if (m_bDefaultFocus)
    {
        m_nFocusIndex = 0;
    }
    else
    {
        m_nFocusIndex = INVALID_ITEM_INDEX;
    }
}

void CFrameListDelegateBase::UpdateIndex()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CListItemPtr pItem = m_pFrameList->GetItemByIndex(0);

    if (NULL == pItem)
    {
        ResetIndex();
        return;
    }

    m_nPageDataIndex = pItem->GetDataIndex();
    m_nFocusIndex = m_pFrameList->GetFocusItemIndex();
}

int CFrameListDelegateBase::GetPageDataIndex() const
{
    return m_nPageDataIndex;
}

int CFrameListDelegateBase::GetFocusIndex() const
{
    return m_nFocusIndex;
}

void CFrameListDelegateBase::RefreshFrameList()
{
    if (NULL == m_pFrameList || (!m_pFrameList->isVisible()))
    {
        return;
    }

    int nFocusIndex = m_nFocusIndex;

    m_pFrameList->RefreshData(m_nPageDataIndex);

    int nItemCount = m_pFrameList->GetTotalItemCount();

    if (nItemCount <= 0)
    {
        ResetIndex();
        return;
    }

    if (m_bItemFocus)
    {
        if (nFocusIndex >= nItemCount)
        {
            nFocusIndex = nItemCount - 1;
        }

        m_pFrameList->FocusItemByIndex(nFocusIndex);
    }

    UpdateIndex();
}

bool CFrameListDelegateBase::Release()
{
    delete this;
    return true;
}

bool CFrameListDelegateBase::IsAttach(CFrameListBase * pFrameList)
{
    if (NULL == pFrameList || pFrameList != m_pFrameList)
    {
        return false;
    }

    return true;
}

void CFrameListDelegateBase::Attach(CFrameListBase * pFrameList)
{
    if (NULL == pFrameList)
    {
        return;
    }

    IFrameListDelegatePtr pDelegate = GetDelegate();

    if (NULL != m_pFrameList && m_pFrameList->GetFrameDelegate() == pDelegate)
    {
        m_pFrameList->ResetAction();
        m_pFrameList->ResetDelegate();
    }

    m_pFrameList = (CFrameList *)pFrameList;
}

void CFrameListDelegateBase::Detach(CFrameListBase * pFrameList)
{
    if (NULL == pFrameList)
    {
        return;
    }

    if (pFrameList == m_pFrameList)
    {
        m_pFrameList->ResetAction();
        m_pFrameList =  NULL;
    }
}
