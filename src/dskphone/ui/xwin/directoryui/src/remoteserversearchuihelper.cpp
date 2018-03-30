#include "remoteserversearchuihelper.h"
#include "remoteserversearchcontroler.h"
#include "directoryuicommon.h"
#include "dirctrlfactory.h"
#include "directorymgr.h"
#include "dirmsgboxmgr.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "translateiddefine.h"
#include "baseui/include/menulistitem.h"


CRemoteServerSearchUIHelper::CRemoteServerSearchUIHelper()
{
    m_nType = DIRECTORY_TYPE_REMOTE_SEARCH;
}

CRemoteServerSearchUIHelper::~CRemoteServerSearchUIHelper()
{
}

CRemoteServerSearchUIHelper * CRemoteServerSearchUIHelper::GetServerSearchUIHelper(
    IDirUIHelperPtr & pDirUIHelper)
{
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper || DIRECTORY_TYPE_REMOTE_SEARCH != pUIHelper->GetType())
    {
        return NULL;
    }

    return static_cast<CRemoteServerSearchUIHelper *>(pUIHelper);
}

void CRemoteServerSearchUIHelper::OnFinishDownLoad(int nGroupID, bool bSuccess)
{
    if (!bSuccess)
    {
        g_DirMsgBoxMgr.ShowMsgBox(TRID_NO_RESULT);
    }
}

int CRemoteServerSearchUIHelper::GetBrowsePageType(int nGroupId)
{
    CDirectoryControlerPtr pControler = g_DirCtrlFactory.FindControler(m_nType);
    CRemoteServerSearchControler * pRemoteControler =
        CRemoteServerSearchControler::GetServerSearchControler(pControler);

    if (NULL != pRemoteControler && pRemoteControler->IsServerSearchGroup(nGroupId))
    {
        return BROWSER_PAGE_TYPE_NORMAL;
    }

    return CRemoteUIHelper::GetBrowsePageType(nGroupId);
}

bool CRemoteServerSearchUIHelper::GetSoftKey(xListViewItem * pFocusItem, int nUIState,
        CArraySoftKeyBarData & vecSoftKey)
{
    if (NULL == m_pListView || DIR_UI_STATE_SEARCH != nUIState)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CRemoteServerSearchControler * pRemoteControler =
        CRemoteServerSearchControler::GetServerSearchControler(pControler);

    if (NULL == pRemoteControler)
    {
        return false;
    }

    if (pRemoteControler->GetPreGroupId() > 0)
    {
        vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_SOFTKEY_PRE_PAGE);
        vecSoftKey[0].m_iType = ST_PREV_PAGE;
    }

    if (pRemoteControler->GetNextGroupId() > 0)
    {
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SOFTKEY_NEXT_PAGE);
        vecSoftKey[1].m_iType = ST_NEXT_PAGE;
    }

    CMenuListItemPtr pMenuItem = CMenuListItem::GetMenuListItem(pFocusItem);

    if (NULL != pMenuItem)
    {
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ENTER);
        vecSoftKey[3].m_iType = ST_ENTER;

        return true;
    }

    int nGroupId = pRemoteControler->GetGroupId();

    GetContactSoftKey(pFocusItem, nUIState, nGroupId, vecSoftKey);

    return true;
}

int CRemoteServerSearchUIHelper::GetActionByUIAction(int nUIState, int nUIAction)
{
    if (ST_PREV_PAGE == nUIAction)
    {
        return DIR_ACTION_REMOTE_PREVPAGE;
    }
    else if (ST_NEXT_PAGE == nUIAction)
    {
        return DIR_ACTION_REMOTE_NEXTPAGE;
    }

    return CRemoteUIHelper::GetActionByUIAction(nUIState, nUIAction);
}
