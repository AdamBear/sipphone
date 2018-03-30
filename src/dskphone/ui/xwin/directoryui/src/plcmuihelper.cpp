#include "plcmuihelper.h"
#include "languagehelper/modlangtranslate.h"
#include "baseui/include/menulistitem.h"
#include "plcmcontactcontroler.h"
#include "contactlistitem.h"
#include "directorymgr.h"
#include "dirmsgboxmgr.h"


#if IF_BUG_PLCMDIR
CPlcmUIHelper::CPlcmUIHelper()
    : CCommonUIHelper(DIRECTORY_TYPE_PLCM)
{
}

CPlcmUIHelper::~CPlcmUIHelper()
{
}

CPlcmUIHelper * CPlcmUIHelper::GetUIHelper(IDirUIHelperPtr & pDirUIHelper)
{
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper || DIRECTORY_TYPE_PLCM != pUIHelper->GetType())
    {
        return NULL;
    }

    return static_cast<CPlcmUIHelper *>(pDirUIHelper.Get());
}

IPLCMUIHelper * CPlcmUIHelper::GetPLCMUIHelper(IDirUIHelperPtr & pDirUIHelper)
{
    CPlcmUIHelper * pPlcmUIHelper = CPlcmUIHelper::GetUIHelper(pDirUIHelper);

    return (IPLCMUIHelper *)pPlcmUIHelper;
}

int CPlcmUIHelper::GetBrowsePageType(int nGroupId)
{
    return BROWSER_PAGE_TYPE_NORMAL;
}

int CPlcmUIHelper::GetListItemType(int nState, int nDataIndex)
{
    if (DIR_UI_STATE_BROWSER == nState)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        CPlcmContactControler * pPlcmContact = CPlcmContactControler::GetControler(pControler);

        if (NULL != pPlcmContact)
        {
            int nID = knInvalidRId;
            SmartPtr pDetail = pPlcmContact->GetDetailByIndex(nID, nDataIndex);
            ContactRemoteMenu * pGroup = CPlcmContactControler::GetPlcmGroup(pDetail);

            if (NULL != pGroup)
            {
                return CMenuListItem::GetMenuItemType();
            }

            return CContactListItem::GetContactListItemType();
        }
    }

    return CCommonUIHelper::GetListItemType(nState, nDataIndex);
}

bool CPlcmUIHelper::UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail)
{
    if (NULL == pListItem || pDetail.IsEmpty())
    {
        return false;
    }

    CMenuListItemPtr pMenuItem = CMenuListItem::GetMenuListItem(pListItem);

    if (NULL != pMenuItem)
    {
        bool bInMenuPage = false;

        if (kszPlcmDirGroupId == nId)
        {
            bInMenuPage = true;
        }

        return UpdateMenuItem(pMenuItem, nId, pDetail->GetDisplayName(), bInMenuPage);
    }

    ContactRemoteUnit * pContact = CPlcmContactControler::GetPlcmContact(pDetail);

    if (NULL == pContact)
    {
        return false;
    }

    int nSize = pContact->m_listNumber.size();

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CPlcmContactControler * pPlcmControler = CPlcmContactControler::GetControler(pControler);
    if (NULL == pPlcmControler)
    {
        return false;
    }

    yl::string strShowNumber = "";
    if (pPlcmControler->IsSearchState())
    {
        strShowNumber = pContact->GetSearchShowNumber(pPlcmControler->GetSearchKey());
    }
    else
    {
        for (int i = 0; i < nSize; ++i)
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

bool CPlcmUIHelper::GetNormalTitle(yl::string & strTitle, int nGroupId/* = knInvalidRId*/)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CPlcmContactControler * pPlcmContact = CPlcmContactControler::GetControler(pControler);

    if (NULL == pPlcmContact)
    {
        return false;
    }

    if (knInvalidRId == nGroupId)
    {
        nGroupId = pPlcmContact->GetGroupId();
    }

    if (kszPlcmDirGroupId == nGroupId)
    {
        strTitle = _LANG(TRID_PLCMBOOK);
    }
    else
    {
        pPlcmContact->GetGroupTitle(nGroupId, strTitle);
    }

    return true;
}

bool CPlcmUIHelper::GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                               CArraySoftKeyBarData & vecSoftKey)
{
    if (DIR_UI_STATE_DETAIL == nUIState)
    {
        return true;
    }

    if (vecSoftKey.size() < 4)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CPlcmContactControler * pPlcmContact = CPlcmContactControler::GetControler(pControler);

    if (NULL == pPlcmContact)
    {
        return false;
    }

    int nGroupId = pPlcmContact->GetGroupId();
    int nFocusId = GetLisItemId(pFocusItem);

    if (knInvalidRId == nFocusId)
    {
        return false;
    }

    if (DIR_UI_STATE_BROWSER == nUIState && pPlcmContact->IsGroupId(nFocusId))
    {
        if (kszPlcmDirGroupId == pPlcmContact->GetGroupId())
        {
            vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_UPDATE);
            vecSoftKey[1].m_iType = ST_UPDATE;
        }

        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
        vecSoftKey[2].m_iType = ST_SEARCH;

        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ENTER);
        vecSoftKey[3].m_iType = ST_ENTER;

        return true;
    }

    if (DIR_UI_STATE_SEARCH == nUIState && pPlcmContact->IsGroupId(nFocusId))
    {
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ENTER);
        vecSoftKey[3].m_iType = ST_ENTER;

        return true;
    }

    // 焦点在联系人上（包括搜索界面）
    if (DIR_UI_STATE_SEARCH != nUIState)
    {
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
        vecSoftKey[1].m_iType = ST_SEARCH;
    }

    GetContactSoftKey(pFocusItem, nUIState, nGroupId, vecSoftKey);

    return true;
}

bool CPlcmUIHelper::GetOption(int nUIState, VEC_OPTION_ITEM_DATA & vecOption)
{
    if (!CCommonUIHelper::GetOption(nUIState, vecOption))
    {
        return false;
    }

    if (DIR_UI_STATE_BROWSER == nUIState)
    {
        ST_OPTION_ITEM_DATA stOption;

        stOption.m_nAction = ST_UPDATE;
        stOption.m_strTitle = _LANG(TRID_UPDATE);

        vecOption.push_back(stOption);
    }

    return true;
}

int CPlcmUIHelper::GetActionByUIAction(int nUIState, int nUIAction)
{
    int nAction = GetContactCopyAction(nUIAction);

    if (DIR_ACTION_NONE != nAction)
    {
        return nAction;
    }

    if (ST_UPDATE == nUIAction)
    {
        return DIR_ACTION_PLCM_UPDATE;
    }

    return CCommonUIHelper::GetActionByUIAction(nUIState, nUIAction);
}

void CPlcmUIHelper::OnDownLoadGroup(int nGroupID, bool bSuccess)
{
    if (!bSuccess)
    {
        g_DirMsgBoxMgr.ShowMsgBox(TRID_FAIL_TO_DOWNLOAD_REMOTE_PHONEBOOK);
    }
}

void CPlcmUIHelper::OnFinishAllDownLoad(int nGroupID)
{

}
#endif
