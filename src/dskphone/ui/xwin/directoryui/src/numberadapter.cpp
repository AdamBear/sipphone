#include "numberadapter.h"
#include "diruihelper.h"
#include "languagehelper/modlangtranslate.h"
#include "xwindow/xListView.h"


CNumberAdapter::CNumberAdapter()
{
    m_nState = DIR_UI_STATE_CHOOSE_NUMBER;
}

CNumberAdapter::~CNumberAdapter()
{
}

CNumberAdapter * CNumberAdapter::GetNumberAdapter(CDirectoryAdapterPtr & pAdapter)
{
    if (pAdapter.IsEmpty() || DIR_UI_STATE_CHOOSE_NUMBER != pAdapter->GetState())
    {
        return NULL;
    }

    return static_cast<CNumberAdapter *>(pAdapter.Get());
}

void CNumberAdapter::EnterNumberChoose()
{
    RefreshUI();

    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL != pUIHelper)
    {
        pUIHelper->FocusDefaultNumber();
    }
}

void CNumberAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView || m_pControler.IsEmpty())
    {
        return;
    }

    int nTotalCount = CDirectoryControler::GetAttributeSize();

    pListView->SetTotalDataCount(nTotalCount);
}

bool CNumberAdapter::PrecheckDataIndex(int & nDataIndex)
{
    int nTotalCount = CDirectoryControler::GetAttributeSize();

    return CheckPageIndex(nTotalCount, nDataIndex);
}

bool CNumberAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    return CDirectoryAdapter::RefreshItem(pListView, pItem);
}

bool CNumberAdapter::GetStateSoftKey(xListViewItem * pFocusItem, CArraySoftKeyBarData & vecSoftKey)
{
#if IF_FEATURE_GBBOOK
    if (!m_pControler.IsEmpty() && DIRECTORY_TYPE_GENBAND == m_pControler->GetType())
    {
        return false;
    }
#endif

    if (!m_pControler.IsEmpty() && CALL_LOG_TYPE_LOCAL == m_pControler->GetType())
    {
        return false;
    }

    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_DIAL);
    vecSoftKey[3].m_iType = ST_SEND;

    return true;
}
