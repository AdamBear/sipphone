#include "bluetoothcontactuihelp.h"
#include "bluetoothcontactcontroler.h"
#include "directorymgr.h"

#ifdef IF_SUPPORT_BLUETOOTH_CONTACT

#include "uimanager/moduimanager.h"
#include "moddirectoryui.h"

#include "baseui/include/detaillistitem.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"


CBluetoothUIHelper::CBluetoothUIHelper()
    : CCommonUIHelper(DIRECTORY_TYPE_BLUETOOTH)
{
}

CBluetoothUIHelper::~CBluetoothUIHelper()
{
}

CBluetoothUIHelper* CBluetoothUIHelper::GetUIHelper(CDirUIHelperPtr& pUIHelper)
{
    if (pUIHelper.IsEmpty() || DIRECTORY_TYPE_BLUETOOTH != pUIHelper->GetType())
    {
        return NULL;
    }

    return (CBluetoothUIHelper*)pUIHelper.Get();
}

int CBluetoothUIHelper::GetBrowsePageType(int nGroupId)
{
    return BROWSER_PAGE_TYPE_NORMAL;
}

bool CBluetoothUIHelper::GetOption(int nUIState, VEC_OPTION_ITEM_DATA& vecOption)
{
    ST_OPTION_ITEM_DATA stOption;

    if (DIR_UI_STATE_SEARCH != nUIState)
    {
        stOption.m_nAction = ST_UPDATE;
        stOption.m_strTitle = TRID_UPDATE;
        vecOption.push_back(stOption);
    }

    stOption.m_nAction = ST_DETAIL;
    stOption.m_strTitle = TRID_DETAIL;
    vecOption.push_back(stOption);

    if (CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, 0))
    {
        stOption.m_nAction = ST_COPY_TO_LOCAL_GROUP;
        stOption.m_strTitle = _LANG(TRID_ADD_TO_CONTACT);

        vecOption.push_back(stOption);
    }

    if (CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId))
    {
        stOption.m_nAction = ST_COPY_TO_BLACKLIST;
        stOption.m_strTitle = _LANG(TRID_ADD_TO_BLACK_LIST);

        vecOption.push_back(stOption);
    }

    return true;
}

bool CBluetoothUIHelper::GetNormalTitle(yl::string& strTitle, int nGroupId /* = knInvalidRId */)
{
    strTitle = _LANG(TRID_MOBILE_CONTACTS);

    return true;
}

bool CBluetoothUIHelper::GetSoftKey(xListViewItem* pFocusItem, int nUIState,
                                    CArraySoftKeyBarData& vecSoftKey)
{
    if (vecSoftKey.size() < SOFTKEY_NUM)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBluetoothContactControler* pBlutoothContact = CBluetoothContactControler::GetControler(pControler);

    if (NULL == pBlutoothContact)
    {
        return false;
    }

    int nFocusId = GetLisItemId(pFocusItem);
    int nGroupId = pBlutoothContact->GetGroupId();

    if (knInvalidRId == nFocusId)
    {
        if (DIR_UI_STATE_BROWSER == nUIState && CDirectoryControler::IsDirCanEdit(m_nType, nGroupId))
        {
            vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_UPDATE);
            vecSoftKey[2].m_iType = ST_UPDATE;
        }

        return false;
    }

    if (DIR_UI_STATE_SEARCH != nUIState)
    {
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
        vecSoftKey[1].m_iType = ST_SEARCH;
    }

    GetContactSoftKey(pFocusItem, nUIState, nGroupId, vecSoftKey);

    return true;
}

int CBluetoothUIHelper::GetActionByUIAction(int nUIState, int nUIAction)
{
    int nAction = GetContactCopyAction(nUIAction);

    if (DIR_ACTION_NONE != nAction)
    {
        return nAction;
    }

    if (ST_UPDATE == nUIAction)
    {
        return DIR_ACTION_BLUETOOTH_UPDATE;
    }

    return CCommonUIHelper::GetActionByUIAction(nUIState, nUIAction);
}

bool CBluetoothUIHelper::UpdateListItem(xListViewItem* pListItem, int nId, SmartPtr& pDetail)
{
    LocalDirContactRecord* pRecord = CBluetoothContactControler::GetLocalContact(pDetail);
    if (NULL == pRecord)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBluetoothContactControler* pBlutoothContact = CBluetoothContactControler::GetControler(pControler);
    if (NULL == pBlutoothContact)
    {
        return false;
    }

    yl::string strNumber = "";
    if (pBlutoothContact->IsSearchState())
    {
        strNumber = pRecord->GetSearchShowNumber(pBlutoothContact->GetSearchKey());
    }
    else
    {
        if (!pRecord->GetNumberByName(kszOfficeNumber, strNumber) || strNumber.empty())
        {
            if (!pRecord->GetNumberByName(kszMobileNumber, strNumber) || strNumber.empty())
            {
                pRecord->GetNumberByName(kszOtherNumber, strNumber);
            }
        }
    }

    return UpdateContactItem(pListItem, nId, pRecord->GetDisplayName(), strNumber);
}
#endif // IF_SUPPORT_BLUETOOTH_CONTACT
