#include "precisesearchdelegate.h"
#include "bsftcontactcontroler.h"
#include "bsftcontactuihelper.h"
#include "baseui/framelist.h"

#if IF_FEATURE_GBBOOK
#include "genbandcontactcontroler.h"
#include "genbandcontactuihelper.h"
#endif


CPreciseSearchDelegate::CPreciseSearchDelegate()
    : CDirectoryListDelegate(DIR_UI_STATE_PRECISE_SEARCH)
{
}

CPreciseSearchDelegate::~CPreciseSearchDelegate()
{
}

CPreciseSearchDelegate * CPreciseSearchDelegate::GetDirDelegate(CDirectoryListDelegatePtr &
        pDelegate)
{
    if (pDelegate.IsEmpty() || DIR_UI_STATE_PRECISE_SEARCH != pDelegate->GetState())
    {
        return NULL;
    }

    return (CPreciseSearchDelegate *)pDelegate.Get();
}

bool CPreciseSearchDelegate::Init(int nDirType, int nGroupId/* = knInvalidRId*/,
                                  int nPageDataIndex/* = 0*/)
{
    if (!CDirectoryListDelegate::Init(nDirType, nGroupId, nPageDataIndex))
    {
        return false;
    }

    CDirectoryControlerPtr pControler = GetControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL != pBSFTContact)
    {
        pBSFTContact->EnterPreciseSearch();
        return true;
    }

#if IF_FEATURE_GBBOOK
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);

    if (NULL != pGenbandContact)
    {
        pGenbandContact->EnterPreciseSearch();
        return true;
    }
#endif

    return false;
}

void CPreciseSearchDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    DIRUI_INFO("CPreciseSearchDelegate::OnLoadData(%p %d %d)", pFrameList, nDataIndex, nDataCount);

    CDirectoryControlerPtr pControler = GetControler();

    CDirUIHelperPtr pUIHelper = GetUIHelper();

    if (NULL == pFrameList || pFrameList != m_pFrameList)
    {
        return;
    }

    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);
    CBSFTContactUIHelper * pBSFTUIHelper = CBSFTContactUIHelper::GetUIHelper(pUIHelper);
    if (NULL != pBSFTContact && NULL != pBSFTUIHelper)
    {
        if (0 == nDataIndex && pBSFTContact->IsPreciseSearchResult())
        {
            pBSFTUIHelper->CheckPreciseSearchUI();
        }

        if (!pBSFTContact->LoadPreciseData(nDataIndex, nDataCount))
        {
            m_pFrameList->ClearContent(true);
        }
    }

#if IF_FEATURE_GBBOOK
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);
    CGenbandContactUIHelper * pGenbandUIHelper = CGenbandContactUIHelper::GetUIHelper(pUIHelper);
    if (pGenbandContact != NULL && pGenbandUIHelper != NULL)
    {
        if (!pGenbandContact->LoadPreciseData(nDataIndex, nDataCount))
        {
            m_pFrameList->ClearContent(true);
        }
    }
#endif
}
