#include "ldappwdadapter.h"

#if IF_FEATURE_34162
#include "directorycommon.h"
#include "commonuihelper.h"
#include "dirmsgboxmgr.h"
#include "dlgdirectory.h"
#include "commonunits/modcommonunits.h"
#include "languagehelper/modlangtranslate.h"
#include "baseui/include/baseuicommon.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "baseui/include/inputmethod.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "imeedit/imecommon.h"
#include "xwindow/xListView.h"
#include "uimanager/moduimanager.h"


namespace
{
#define LDAP_PWD_ITEM_INDEX_USER    0
#define LDAP_PWD_ITEM_INDEX_PWD     1
#define LDAP_PWD_ITEM_COUNT         2
}

CLDAPPwdAdapter::CLDAPPwdAdapter()
    : CDirectoryAdapter(DIR_UI_STATE_LDAP_CHG_PWD)
    , m_pUserItem(NULL)
    , m_pPwdItem(NULL)
{
}

CLDAPPwdAdapter::~CLDAPPwdAdapter()
{
    ReleaseListItem();
}

CLDAPPwdAdapter * CLDAPPwdAdapter::GetLDAPPwdAdapter(CDirectoryAdapterPtr & pAdapter)
{
    if (pAdapter.IsEmpty() || DIR_UI_STATE_LDAP_CHG_PWD != pAdapter->GetState())
    {
        return NULL;
    }

    return static_cast<CLDAPPwdAdapter *>(pAdapter.Get());
}

void CLDAPPwdAdapter::UnInit()
{
    CDirectoryAdapter::UnInit();

    ReleaseListItem();
}

void CLDAPPwdAdapter::ReleaseListItem()
{
    if (NULL != m_pUserItem)
    {
        g_ItemFactory.ReleaseItem(m_pUserItem);
        m_pUserItem = NULL;
    }

    if (NULL != m_pPwdItem)
    {
        g_ItemFactory.ReleaseItem(m_pPwdItem);
        m_pPwdItem = NULL;
    }
}

void CLDAPPwdAdapter::EnterPwdSetting()
{
    if (NULL == m_pUserItem)
    {
        m_pUserItem = static_cast<CEditListItem *>(g_ItemFactory.ApplyItem(
                          CEditListItem::GetEditListItemType()));
    }

    if (NULL == m_pPwdItem)
    {
        m_pPwdItem = static_cast<CEditListItem *>(g_ItemFactory.ApplyItem(
                         CEditListItem::GetEditListItemType()));
    }

    yl::string strUserID = configParser_GetConfigString(kszLDAPuser).c_str();
    yl::string strPassword = configParser_GetConfigString(kszLDAPpswd).c_str();
    yl::string strIme;

    if (NULL != m_pUserItem)
    {
        m_pUserItem->ClearContent();
        m_pUserItem->SetMaxLength(MAX_LEN_DISPLAY_NAME);
        m_pUserItem->SetTitle(_LANG(TRID_USER_NAME));
        m_pUserItem->SetValue(strUserID);

#if IF_FEATURE_AUTOP_IME
        strIme = commonUnits_GetIMEByType(kszLDAPUserIME);

        if (strIme.empty())
        {
            strIme = IME_2aB;
        }
#else
        strIme = IME_2aB;
#endif

        m_pUserItem->InstallIME(IME_ALL, strIme.c_str());
    }

    if (NULL != m_pPwdItem)
    {
        m_pPwdItem->ClearContent();
        m_pPwdItem->SetMaxLength(MAX_LEN_PASSWORD);
        m_pPwdItem->SetTitle(_LANG(TRID_PASSWORD));
        m_pPwdItem->SetValue(strPassword);
        m_pPwdItem->SetPassword(true);
        m_pPwdItem->SetSuddenPassword(true);

#if IF_FEATURE_AUTOP_IME
        strIme = commonUnits_GetIMEByType(kszLDAPPasswordIME);

        if (strIme.empty())
        {
            strIme = IME_2aB_Pwd;
        }
#else
        strIme = IME_2aB_Pwd;
#endif

        m_pPwdItem->InstallIME(IME_PWD, strIme.c_str());
    }

    SetFocusIndex(LDAP_PWD_ITEM_INDEX_USER);

    RefreshUI();
}

void CLDAPPwdAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    pListView->SetTotalDataCount(LDAP_PWD_ITEM_COUNT);
    pListView->SetItemAttach();
}

bool CLDAPPwdAdapter::PrecheckDataIndex(int & nDataIndex)
{
    return CheckPageIndex(LDAP_PWD_ITEM_COUNT, nDataIndex);
}

xListViewItem * CLDAPPwdAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    if (LDAP_PWD_ITEM_INDEX_USER == nDataIndex)
    {
        return m_pUserItem;
    }

    if (LDAP_PWD_ITEM_INDEX_PWD == nDataIndex)
    {
        return m_pPwdItem;
    }

    return NULL;
}

bool CLDAPPwdAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    return true;
}

bool CLDAPPwdAdapter::GetTitle(yl::string & strTitle)
{
    strTitle = _LANG(TRID_LDAP_AHTHORITY);

    return true;
}

bool CLDAPPwdAdapter::GetStateSoftKey(xListViewItem * pFocusItem, CArraySoftKeyBarData & vecSoftKey)
{
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_OK);
    vecSoftKey[3].m_iType = ST_OK;

    return true;
}

bool CLDAPPwdAdapter::ProcessKeyPress(int nKey)
{
    if (PHONE_KEY_OK == nKey)
    {
        return ProcessAction(ST_OK);
    }

    return CDirectoryAdapter::ProcessKeyPress(nKey);
}

bool CLDAPPwdAdapter::ProcessLineKeyPress(int nKey)
{
    return false;
}

bool CLDAPPwdAdapter::ProcessAction(int nAction)
{
    if (ST_OK == nAction)
    {
        yl::string strUserID;

        if (NULL != m_pUserItem)
        {
            m_pUserItem->ReturnAllInput();

            strUserID = m_pUserItem->GetValue();
        }

        yl::string strPassword;

        if (NULL != m_pPwdItem)
        {
            m_pPwdItem->ReturnAllInput();

            strPassword = m_pPwdItem->GetValue();
        }

        configParser_SetConfigString(kszLDAPuser, strUserID.c_str());
        configParser_SetConfigString(kszLDAPpswd, strPassword.c_str());

        IDirUIHelperPtr pDirUIHelper = GetUIHelper();
        CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

        ProcessCtrlAction(pUIHelper, DIR_ACTION_LDAP_CHG_PWD);

        return true;
    }

    return CDirectoryAdapter::ProcessAction(nAction);
}

#endif  //IF_FEATURE_34162
