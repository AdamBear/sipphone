#include "precisesearchadapter.h"
#include "precisesearchcontrollbase.h"
#include "bsftsearchlayout.h"
#include "directorymgr.h"
#include "dlgdirectory.h"
#include "xwindow/xListView.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "uimanager/moduimanager.h"


CPreciseSearchAdapter::CPreciseSearchAdapter()
    : CDirectoryAdapter(DIR_UI_STATE_PRECISE_SEARCH)
    , m_pSearchLayout(NULL)
{
}

CPreciseSearchAdapter::~CPreciseSearchAdapter()
{
}

CPreciseSearchAdapter * CPreciseSearchAdapter::GetPreciseSearchAdapter(
    CDirectoryAdapterPtr & pAdapter)
{
    if (pAdapter.IsEmpty() || DIR_UI_STATE_PRECISE_SEARCH != pAdapter->GetState())
    {
        return NULL;
    }

    return static_cast<CPreciseSearchAdapter *>(pAdapter.Get());
}

bool CPreciseSearchAdapter::Init(int nDirType, int nGroupId/* = knInvalidRId*/)
{
    if (!CDirectoryAdapter::Init(nDirType, nGroupId))
    {
        return false;
    }

    CPreciseSearchControlBase* pPreciseSearch = GetPreciseSearchControll();

    if (NULL == pPreciseSearch)
    {
        return false;
    }

    pPreciseSearch->EnterPreciseSearch();

    return true;
}

void CPreciseSearchAdapter::UnInit()
{
    if (NULL != m_pSearchLayout)
    {
        m_pSearchLayout = NULL;
    }

#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE || IF_HIDE_TITLE_BUT_EDIT
    OnChangeListViewParam(false);
#endif

    CDirectoryAdapter::UnInit();
}

// 处理SoftKey
bool CPreciseSearchAdapter::ProcessAction(int nAction)
{
    if (ST_BACK == nAction)
    {
        CPreciseSearchControlBase* pPreciseSearch = GetPreciseSearchControll();

        if (NULL != pPreciseSearch && pPreciseSearch->OnBack())
        {
            return true;
        }

        return false;
    }

    return CDirectoryAdapter::ProcessAction(nAction);
}

bool CPreciseSearchAdapter::ProcessKeyPress(int nKey)
{
    if (PHONE_KEY_OK == nKey)
    {
        CPreciseSearchControlBase* pPreciseSearch = GetPreciseSearchControll();
        if (NULL != pPreciseSearch && pPreciseSearch->IsPreciseSearchCondition())
        {
            pPreciseSearch->PreciseSearch();

            return true;
        }
    }

    return CDirectoryAdapter::ProcessKeyPress(nKey);
}

void CPreciseSearchAdapter::EnterPreciseSearch()
{
    CPreciseSearchControlBase* pPreciseSearch = GetPreciseSearchControll();
    if (NULL != pPreciseSearch)
    {
        pPreciseSearch->OnEnterPreciseSearch();
    }

    RefreshUI();
}

void CPreciseSearchAdapter::SetSearchLayout(CDetailLayout* pSearchLayout)
{
    m_pSearchLayout = pSearchLayout;
}

bool CPreciseSearchAdapter::IsPreciseSearchResult()
{
    CPreciseSearchControlBase* pPreciseSearch = GetPreciseSearchControll();

    if (NULL == pPreciseSearch)
    {
        return false;
    }

    return pPreciseSearch->IsPreciseSearchResult();
}

bool CPreciseSearchAdapter::IsPreciseSearchCondition()
{
    CPreciseSearchControlBase* pPreciseSearch = GetPreciseSearchControll();

    if (NULL == pPreciseSearch)
    {
        return false;
    }

    return pPreciseSearch->IsPreciseSearchCondition();
}

int CPreciseSearchAdapter::GetTotalItemCount()
{
    CPreciseSearchControlBase* pPreciseSearch = GetPreciseSearchControll();

    if (NULL != pPreciseSearch && pPreciseSearch->IsPreciseSearchResult())
    {
        return pPreciseSearch->GetSearchItemAmount();
    }
    else if (NULL != m_pSearchLayout)
    {
        return m_pSearchLayout->GetItemCount();
    }

    return 0;
}

CPreciseSearchControlBase* CPreciseSearchAdapter::GetPreciseSearchControll()
{
    CDirectoryControlerPtr pControll = GetControler();
    return CPreciseSearchControlBase::GetPreciseSearchControl(pControll);
}

void CPreciseSearchAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    bool bSearchResult = IsPreciseSearchResult();
    if (bSearchResult)
    {
        pListView->ClearContent();
    }
    else
    {
        pListView->SetItemAttach();
    }

#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE || IF_HIDE_TITLE_BUT_EDIT
    OnChangeListViewParam(!bSearchResult);
#endif

    int nTotalCount = GetTotalItemCount();

    pListView->SetTotalDataCount(nTotalCount);
}

bool CPreciseSearchAdapter::PrecheckDataIndex(int & nDataIndex)
{
    return CheckPageIndex(GetTotalItemCount(), nDataIndex);
}

xListViewItem * CPreciseSearchAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    if (IsPreciseSearchResult())
    {
        return CDirectoryAdapter::PrepareItem(pListView, nDataIndex, pItem);
    }

    if (NULL == m_pSearchLayout)
    {
        return NULL;
    }

    CDetailListItemBasePtr pDetailItem = m_pSearchLayout->GetDetailListItem(nDataIndex);

    if (NULL != pDetailItem)
    {
        pDetailItem->SetAction(this);
    }

    return pDetailItem;
}

bool CPreciseSearchAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (!IsPreciseSearchResult())
    {
        return true;
    }

    return CDirectoryAdapter::RefreshItem(pListView, pItem);
}

bool CPreciseSearchAdapter::IsShowPageInfo()
{
    if (IsPreciseSearchCondition())
    {
        return false;
    }

    return true;
}

bool CPreciseSearchAdapter::GetTitle(yl::string & strTitle)
{
    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper)
    {
        return false;
    }

    return pUIHelper->GetNormalTitle(strTitle);
}
