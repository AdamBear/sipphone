#include "searchadapter.h"
#include "dlgdirsearch.h"
#include "xwindow/xListView.h"
#include "uimanager/moduimanager.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "translateiddefine.h"


CSearchAdapter::CSearchAdapter()
    : CDirectoryAdapter(DIR_UI_STATE_SEARCH)
    , m_nSearchGroupId(kszAllContactId)
    , m_strSearchKey("")
{
}

CSearchAdapter::~CSearchAdapter(void)
{
}

CSearchAdapter * CSearchAdapter::GetSearchAdapter(CDirectoryAdapterPtr & pAdapter)
{
    if (pAdapter.IsEmpty() || DIR_UI_STATE_SEARCH != pAdapter->GetState())
    {
        return NULL;
    }

    return static_cast<CSearchAdapter *>(pAdapter.Get());
}

bool CSearchAdapter::Init(int nDirType, int nGroupId/* = knInvalidRId*/)
{
    if (!CDirectoryAdapter::Init(nDirType, nGroupId))
    {
        return false;
    }

    m_nSearchGroupId = nGroupId;
    m_strSearchKey = "";

    if (DIRECTORY_TYPE_BROADSOFT == nDirType && !BsftDir_UpdateEnable())
    {
        BsftDir_ClearContactInfo(kszServerSearchGroupId);
    }

    return true;
}

void CSearchAdapter::OnSearchChange(const yl::string & strSearchKey)
{
    if (NULL == m_pListView)
    {
        return;
    }

    //重复内容也可以搜索
    //if (strSearchKey == m_strSearchKey && "" != strSearchKey)
    //{
    //  return;
    //}

    m_strSearchKey = strSearchKey;
    ResetIndex();

    DIRUI_INFO("OnSearchChange:%s", m_strSearchKey.c_str());

    if (OnBeginSearch())
    {
        RefreshListView();
    }
    else
    {
        m_pListView->ClearContent();
    }
}

int CSearchAdapter::GetSearchGroupId() const
{
    return m_nSearchGroupId;
}

const yl::string & CSearchAdapter::GetSearchKey() const
{
    return m_strSearchKey;
}

bool CSearchAdapter::OnBeginSearch()
{
    CDirectoryControlerPtr pControler = GetControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    if (pControler->PreSearchContact())
    {
        return true;
    }

    return false;
}

void CSearchAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView || m_pControler.IsEmpty())
    {
        return;
    }

    int nTotalCount = m_pControler->GetSearchItemAmount();

    pListView->SetTotalDataCount(nTotalCount);
}

bool CSearchAdapter::PrecheckDataIndex(int & nDataIndex)
{
    if (m_pControler.IsEmpty())
    {
        return false;
    }

    int nTotalCount = m_pControler->GetSearchItemAmount();

    return CheckPageIndex(nTotalCount, nDataIndex);
}

bool CSearchAdapter::IsShowPageInfo()
{
    return true;
}

bool CSearchAdapter::GetTitle(yl::string & strTitle)
{
    IDirUIHelperPtr pDirUIHelper = GetUIHelper();
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL != pUIHelper)
    {
        pUIHelper->GetSearchTitle(strTitle);

        return true;
    }

    return false;
}

bool CSearchAdapter::GetStateSoftKey(xListViewItem * pFocusItem, CArraySoftKeyBarData & vecSoftKey)
{
    CDirectoryControlerPtr pControler = GetControler();

    if (NULL == m_pListView || pControler.IsEmpty() || !pControler->IsManualSearch())
    {
        return false;
    }

    if (m_pListView->hasFocus() || m_pListView->containFocus())
    {
        return false;
    }

    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
    vecSoftKey[3].m_iType = ST_SEARCH;

    return true;
}
