#include "remotecontactcontroler.h"
#include "remoteuihelper.h"
#include "browseadapter.h"
#include "directorymgr.h"
#include "dirmsgboxmgr.h"
#include "baseui/include/menulistitem.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"


CRemoteUIHelper::CRemoteUIHelper()
    : CCommonUIHelper(DIRECTORY_TYPE_REMOTE)
{
}

CRemoteUIHelper::~CRemoteUIHelper()
{
}

CRemoteUIHelper * CRemoteUIHelper::GetUIHelper(IDirUIHelperPtr & pDirUIHelper)
{
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper || DIRECTORY_TYPE_REMOTE != pUIHelper->GetType())
    {
        return NULL;
    }

    return static_cast<CRemoteUIHelper *>(pUIHelper);
}

IRemoteUIHelper * CRemoteUIHelper::GetRemoteUIHelper(IDirUIHelperPtr & pDirUIHelper)
{
    CRemoteUIHelper * pRemoteUIHelper = CRemoteUIHelper::GetUIHelper(pDirUIHelper);

    return (IRemoteUIHelper *)pRemoteUIHelper;
}

void CRemoteUIHelper::OnFinishDownLoad(int nGroupID, bool bSuccess)
{
    if (!bSuccess)
    {
        g_DirMsgBoxMgr.ShowMsgBox(TRID_FAIL_TO_DOWNLOAD_REMOTE_PHONEBOOK);
    }
}

void CRemoteUIHelper::OnFinishAllDownLoad(int nGroupID)
{

}

int CRemoteUIHelper::GetListItemType(int nState, int nDataIndex)
{
    if (DIR_UI_STATE_DETAIL != nState && DIR_UI_STATE_CHOOSE_NUMBER != nState)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        CRemoteContactControler * pRemoteContact = CRemoteContactControler::GetControler(pControler);

        if (NULL != pRemoteContact)
        {
            int nId = knInvalidRId;
            SmartPtr pDetail = pRemoteContact->GetDetailByIndex(nId, nDataIndex);
            ContactRemoteMenu * pGroup = CRemoteContactControler::GetRemoteGroup(pDetail);

            if (NULL != pGroup)
            {
                return CMenuListItem::GetMenuItemType();
            }
        }
    }

    return CCommonUIHelper::GetListItemType(nState, nDataIndex);
}

void CRemoteUIHelper::GetMenuTitle(yl::string & strTitle)
{
    const yl::string & strDisplayName = RemoteDir_GetRemoteDisplayName();

    if (strDisplayName.empty())
    {
        strTitle = _LANG(TRID_REMOTE_PHONEBOOK);
    }
    else
    {
        strTitle = strDisplayName;
    }
}

bool CRemoteUIHelper::GetNormalTitle(yl::string & strTitle, int nGroupId/* = knInvalidRId*/)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CRemoteContactControler * pRemoteContact = CRemoteContactControler::GetControler(pControler);

    if (NULL == pRemoteContact)
    {
        return false;
    }

    if (knInvalidRId == nGroupId)
    {
        nGroupId = pRemoteContact->GetGroupId();
    }

    if (pRemoteContact->GetGroupTitle(nGroupId, strTitle))
    {
        if (strTitle.empty())
        {
            strTitle = _LANG(TRID_UNKNOWN);
        }

        return true;
    }

    return false;
}

bool CRemoteUIHelper::GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                                 CArraySoftKeyBarData & vecSoftKey)
{
    if (DIR_UI_STATE_DETAIL == nUIState)
    {
        return true;
    }

    if (vecSoftKey.size() < SOFTKEY_NUM)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CRemoteContactControler * pRemoteContact = CRemoteContactControler::GetControler(pControler);

    if (NULL == pRemoteContact)
    {
        return false;
    }

    int nGroupId = pRemoteContact->GetGroupId();
    int nFocusId = GetLisItemId(pFocusItem);

    if (DIR_UI_STATE_BROWSER == nUIState)
    {
        if (pRemoteContact->IsGroupId(nFocusId))
        {
            if (kszRootId == nGroupId && !CRemoteContactControler::IsServerSearchGroup(nFocusId))
            {
                vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_UPDATE);
                vecSoftKey[1].m_iType = ST_UPDATE;
            }

            return true;
        }

        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
        vecSoftKey[1].m_iType = ST_SEARCH;
    }

    // 焦点在联系人上（包括搜索界面）
    GetContactSoftKey(pFocusItem, nUIState, nGroupId, vecSoftKey);

    return true;
}

int CRemoteUIHelper::GetActionByUIAction(int nUIState, int nUIAction)
{
    int nAction = GetContactCopyAction(nUIAction);

    if (DIR_ACTION_NONE != nAction)
    {
        return nAction;
    }

    if (ST_UPDATE == nUIAction)
    {
        return DIR_ACTION_REMOTE_UPDATE;
    }

    return CCommonUIHelper::GetActionByUIAction(nUIState, nUIAction);
}

bool CRemoteUIHelper::UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CRemoteContactControler * pRemoteControler = CRemoteContactControler::GetControler(pControler);

    if (NULL == pRemoteControler || NULL == pListItem || pDetail.IsEmpty())
    {
        return false;
    }

    CMenuListItemPtr pMenuItem = CMenuListItem::GetMenuListItem(pListItem);

    if (NULL != pMenuItem)
    {
        bool bInMenuPage = false;

        if (kszRootId == pRemoteControler->GetGroupId())
        {
            bInMenuPage = true;
        }

        return UpdateMenuItem(pMenuItem, nId, pDetail->GetDisplayName(), bInMenuPage);
    }

    ContactRemoteUnit * pContact = CRemoteContactControler::GetRemoteContact(pDetail);
    if (NULL == pContact)
    {
        return false;
    }

    yl::string strShowNumber = "";
    if (pRemoteControler->IsSearchState())
    {
        strShowNumber = pContact->GetSearchShowNumber(pRemoteControler->GetSearchKey());
    }
    else
    {
        for (int i = 0; i < pContact->m_listNumber.size(); ++i)
        {
            const yl::string & strNumber = pContact->GetNumberByIndex(i);

            if (!strNumber.empty())
            {
                strShowNumber = strNumber;
                break;
            }
        }
    }

    return UpdateContactItem(pListItem, nId, pContact->GetDisplayName(), strShowNumber);
}
