#include "remoteserversearchuihelper.h"
#include "remoteserversearchcontroler.h"
#include "directorylistaction.h"
#include "directorymgr.h"
#include "translateiddefine.h"
#include "baseui/framelist.h"
#include "uikernel/languagehelper.h"
#include "remotelisttitle.h"


CRemoteServerSearchUIHelper::CRemoteServerSearchUIHelper()
{
    m_nType = DIRECTORY_TYPE_REMOTE_SEARCH;
}

CRemoteServerSearchUIHelper::~CRemoteServerSearchUIHelper()
{
}

CRemoteServerSearchUIHelper * CRemoteServerSearchUIHelper::GetServerSearchUIHelper(
    CDirUIHelperPtr & pUIHelper)
{
    if (pUIHelper.IsEmpty() || DIRECTORY_TYPE_REMOTE_SEARCH != pUIHelper->GetType())
    {
        return NULL;
    }

    return (CRemoteServerSearchUIHelper *)pUIHelper.Get();
}

bool CRemoteServerSearchUIHelper::RefreshListItemAmount(int nUIState, int nFromIndex,
        int nTotalCount)
{
    if (NULL == m_pFrameList)
    {
        return false;
    }

    if (DIR_UI_STATE_SEARCH == nUIState)
    {
        RefreshServerSearchTitle(nTotalCount);
        return true;
    }

    return CRemoteUIHelper::RefreshListItemAmount(nUIState, nFromIndex, nTotalCount);
}

int CRemoteServerSearchUIHelper::GetActionByTitle(int nUIState, int nTitleAction)
{
    if (DIR_UI_STATE_SEARCH == nUIState)
    {
        if (LIST_TITLE_ACTION_PRE_PAGE == nTitleAction)
        {
            return DIR_ACTION_REMOTE_PREVPAGE;
        }
        else if (LIST_TITLE_ACTION_NEXT_PAGE == nTitleAction)
        {
            return DIR_ACTION_REMOTE_NEXTPAGE;
        }
    }

    return CRemoteUIHelper::GetActionByTitle(nUIState, nTitleAction);
}

void CRemoteServerSearchUIHelper::RefreshServerSearchTitle(int nTotalCount)
{
    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    CDirectoryControlerPtr pControler = pDelegate->GetControler();
    CRemoteServerSearchControler * pServerSearchControler =
        CRemoteServerSearchControler::GetServerSearchControler(pControler);

    if (NULL == pServerSearchControler || NULL == m_pFrameList)
    {
        return;
    }

    int nPreGroupId = pServerSearchControler->GetPreGroupId();
    int nNextGroupId = pServerSearchControler->GetNextGroupId();

    if (0 == nTotalCount && knInvalidRId == nPreGroupId && knInvalidRId == nNextGroupId)
    {
        m_pFrameList->SetFrameListTitle(NULL);
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_NONE_TITLE_ITEM_COUNT);
        return;
    }

    CFrameListTitlePtr pFrameTitle = m_pFrameList->GetFrameListTitle();
    CRemoteListTitlePtr pTitle = CRemoteListTitle::GetRemoteListTitle(pFrameTitle);

    if (NULL != pFrameTitle && NULL == pTitle)
    {
        m_pFrameList->SetFrameListTitle(NULL);
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_NONE_TITLE_ITEM_COUNT);
    }

    bool bAdd = false;

    if (NULL == pTitle)
    {
        bAdd = true;
        pTitle = new CRemoteListTitle;
    }

    if (NULL != pTitle)
    {
        pTitle->SetAction(pDelegate.Get());
        pTitle->SetRemoteTip(TRID_SEARCH_RESULT, nTotalCount);

        int nBtnCount = 0;

        if (knInvalidRId != nPreGroupId)
        {
            nBtnCount += 1;
        }

        if (knInvalidRId != nNextGroupId)
        {
            nBtnCount += 1;
        }

        pTitle->SetBtnCount(nBtnCount);

        if (knInvalidRId != nPreGroupId)
        {
            pTitle->AddButton(LIST_TITLE_ACTION_PRE_PAGE, LANG_TRANSLATE(TRID_SOFTKEY_PRE_PAGE));
        }

        if (knInvalidRId != nNextGroupId)
        {
            pTitle->AddButton(LIST_TITLE_ACTION_NEXT_PAGE, LANG_TRANSLATE(TRID_SOFTKEY_NEXT_PAGE));
        }

        if (bAdd)
        {
            m_pFrameList->SetFrameListTitle(pTitle);

            m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_TITLE_ITEM_COUNT);
        }
        else
        {
            pTitle->RefreshTitle();
        }
    }
}
