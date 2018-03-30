#include "ldapuihelper.h"
#include "directorymgr.h"
#include "translateiddefine.h"
#include "ldapcontactcontroler.h"
#include "languagehelper/modlangtranslate.h"
#include "xwindow/xListView.h"
#include "xwindow/xlistviewitem.h"
#include <directorydefine.h>


CLDAPUIHelper::CLDAPUIHelper()
    : CDirUIHelper(DIRECTORY_TYPE_LDAP)
{
}

CLDAPUIHelper::~CLDAPUIHelper()
{
}

CLDAPUIHelper * CLDAPUIHelper::GetUIHelper(IDirUIHelperPtr & pDirUIHelper)
{
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper || DIRECTORY_TYPE_LDAP != pUIHelper->GetType())
    {
        return NULL;
    }

    return static_cast<CLDAPUIHelper *>(pUIHelper);
}

bool CLDAPUIHelper::RefreshItem(int nState, xListViewItem * pItem)
{
    if (CDirUIHelper::RefreshItem(nState, pItem))
    {
        return true;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLDAPContactControler * pLDAPContact = CLDAPContactControler::GetControler(pControler);

    if (NULL == pItem || NULL == pLDAPContact)
    {
        return false;
    }

    int nDataIndex = pItem->GetDataIndex();
    LdapContactData ldapContact;

    if (!pLDAPContact->GetDetailByIndex(nDataIndex, ldapContact))
    {
        return false;
    }

    yl::string strName = ldapContact.m_strDisplayName;

    if (strName.empty())
    {
        strName = _LANG(TRID_NAME_UNKNOWN);
    }

    yl::string strShowNumber = "";
    if (pLDAPContact->IsSearchState())
    {
        strShowNumber = ldapContact.GetSearchShowNumber(pLDAPContact->GetSearchKey());
    }
    else
    {
        strShowNumber = (*ldapContact.m_listNumber.begin()).strValue;
    }

#if IF_LDAP_SEARCH_RESULT_SHOW_ONE_TEXT
    if (!strShowNumber.empty())
    {
        strName = strName + "<" + strShowNumber + ">";
    }
#endif

    if (strShowNumber.empty())
    {
        strShowNumber = _LANG(TRID_NONE);
    }

    yl::string strPhoto = "";
#if IS_COLOR
    strPhoto = PIC_CONTACT_NORMAL;
#else
    if (DEVICELIB_IS_CP(devicelib_GetPhoneType()))
    {
        strPhoto = PIC_CONTACT_NORMAL;
    }
#endif

    return UpdateContactItem(pItem, nDataIndex, strName, strShowNumber, strPhoto);

}

void CLDAPUIHelper::GetSearchTitle(yl::string & strTitle)
{
    strTitle = _LANG(TRID_LDAP_SEARCH);
}

bool CLDAPUIHelper::GetDetailTitle(yl::string & strTitle)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLDAPContactControler * pLDAPContact = CLDAPContactControler::GetControler(pControler);

    if (NULL == pLDAPContact)
    {
        return false;
    }

    pLDAPContact->GetContactShowName(strTitle);

    if (strTitle.empty())
    {
        strTitle = _LANG(TRID_NAME_UNKNOWN);
    }

    return true;
}

bool CLDAPUIHelper::GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                               CArraySoftKeyBarData & vecSoftKey)
{
    if (DIR_UI_STATE_DETAIL == nUIState)
    {
        return true;
    }

    return GetContactSoftKey(pFocusItem, nUIState, knInvalidRId, vecSoftKey);
}

int CLDAPUIHelper::GetActionByUIAction(int nUIState, int nUIAction)
{
    int nAction = GetContactCopyAction(nUIAction);

    if (DIR_ACTION_NONE != nAction)
    {
        return nAction;
    }

    return CDirUIHelper::GetActionByUIAction(nUIState, nUIAction);
}

int CLDAPUIHelper::GetActionByItem(int nItemAction, int nItemId)
{
    if (LIST_VIEW_ITEM_ACTION_SELECT == nItemAction)
    {
        return DIR_ACTION_COMMON_SEND;
    }

    return CDirUIHelper::GetActionByItem(nItemAction, nItemId);
}
