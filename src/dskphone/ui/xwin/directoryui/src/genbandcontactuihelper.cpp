#if IF_FEATURE_GBBOOK

#include "genbandcontactuihelper.h"
#include "genbandcontactcontroler.h"
#include "directorymgr.h"
#include "dirmsgboxmgr.h"
#include "xwindow/xlistviewitem.h"
#include "xwindow/xListView.h"
#include "baseui/include/menulistitem.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/comboboxlistitem.h"
#include "baseui/include/modbaseui.h"
#include "contactlistitem.h"
#include "detailadapter.h"
#include "browseadapter.h"
#include "precisesearchadapter.h"
#include "account/include/modaccount.h"
#include "contacts/directory/include/commondata.h"
#include "contacts/directory/include/moddirinterface.h"
#include "configiddefine/configiddefine.h"
#include "configparser/modconfigparser.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CGenbandContactUIHelper::CGenbandContactUIHelper()
    : CCommonUIHelper(DIRECTORY_TYPE_GENBAND)
    , m_pSearchLayout(NULL)
    , m_pDetailLayout(NULL)
{
}

CGenbandContactUIHelper::~CGenbandContactUIHelper()
{
}

CGenbandContactUIHelper* CGenbandContactUIHelper::GetUIHelper(IDirUIHelperPtr& pUIHelper)
{
    CDirUIHelper * pDirUIHelper = CDirUIHelper::GetDirUIHelper(pUIHelper);

    if (NULL == pDirUIHelper || DIRECTORY_TYPE_GENBAND != pDirUIHelper->GetType())
    {
        return NULL;
    }

    return static_cast<CGenbandContactUIHelper*>(pDirUIHelper);
}

IGenbandContactUIHelper * CGenbandContactUIHelper::GetGenbandUIHelper(IDirUIHelperPtr & pDirUIHelper)
{
    CGenbandContactUIHelper * pUIHelper = CGenbandContactUIHelper::GetUIHelper(pDirUIHelper);

    return (IGenbandContactUIHelper *)pUIHelper;
}

void CGenbandContactUIHelper::EnterPreciseSearch()
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);

    if (NULL != pGenbandContact && DIR_UI_STATE_PRECISE_SEARCH == pGenbandContact->GetState())
    {
        if (pGenbandContact->IsPreciseSearchCondition())
        {
            return;
        }

        pGenbandContact->EnterPreciseSearch();

        if (NULL != m_pSearchLayout)
        {
            m_pSearchLayout->ClearEdit();
        }

        RefreshUI(DIR_UI_REFRESH_TYPE_ALL, DIR_UI_REFRESH_PARAM_ALL);

        return;
    }

    g_DirectoryMgr.EnterPreciseSearch();
}

void CGenbandContactUIHelper::OnEnterPreciseSearch()
{
    if (NULL == m_pSearchLayout)
    {
        m_pSearchLayout = new CGenbandSearchLayout;

        if (NULL == m_pSearchLayout)
        {
            return;
        }
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);
    if (NULL == pGenbandContact)
    {
        return;
    }

    int nMode = pGenbandContact->GetSearchMode();

    m_pSearchLayout->Init(nMode);

    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
    CPreciseSearchAdapter * pSearchAdapter = CPreciseSearchAdapter::GetPreciseSearchAdapter(pAdapter);

    if (NULL != pSearchAdapter)
    {
        pSearchAdapter->SetSearchLayout(m_pSearchLayout);
    }
}

bool CGenbandContactUIHelper::GetGABPreciseSearchData(yl::string & strKey)
{
    if (NULL == m_pSearchLayout)
    {
        return false;
    }

    m_pSearchLayout->ReturnAllEdit();

    CEditListItemPtr pFirstItem = m_pSearchLayout->GetFirstItem();

    if (NULL != pFirstItem)
    {
        strKey = pFirstItem->GetValue();
        return true;
    }

    return false;
}

void CGenbandContactUIHelper::OnEnterManageGroup(int nDirType, int nID)
{
    g_DirectoryMgr.EnterDirectory(nDirType, nID);
}

bool CGenbandContactUIHelper::GetGenbandContactData(ContactGBBookUnit& itemData)
{
    if (NULL == m_pDetailLayout)
    {
        return false;
    }

    m_pDetailLayout->ReturnAllEdit();

    CEditListItemPtr pNameItem = m_pDetailLayout->GetNameEditItem();

    if (NULL == pNameItem)
    {
        DIRUI_ERR("GetNameEditItem() is null when GetGenbandContactData");
        return false;
    }
    itemData.m_strDisplayName = pNameItem->GetValue();

    CEditListItemPtr pFirstNameItem = m_pDetailLayout->GetFirstNameEditItem();
    if (NULL != pFirstNameItem)
    {
        itemData.m_strFirstName = pFirstNameItem->GetValue();
    }

    CEditListItemPtr pLastNameItem = m_pDetailLayout->GetLastNameEditItem();
    if (NULL != pLastNameItem)
    {
        itemData.m_strLastName = pLastNameItem->GetValue();
    }

    CEditListItemPtr pEmailItem = m_pDetailLayout->GetEmailEditItem();
    if (NULL != pEmailItem)
    {
        itemData.m_strEmail = pEmailItem->GetValue();
    }

    CEditListItemPtr pBusinessNoItem = m_pDetailLayout->GetBusinessNoEditItem();
    GetItemDataByName(pBusinessNoItem, kszGbBookbusinessNumber, itemData);

    CEditListItemPtr pHomeNoItem = m_pDetailLayout->GetHomeNoEditItem();
    GetItemDataByName(pHomeNoItem, kszGbBookhomeNumber, itemData);

    CEditListItemPtr pMobileNoItem = m_pDetailLayout->GetMobileNoEditItem();
    GetItemDataByName(pMobileNoItem, kszGbBookmobile, itemData);

    CEditListItemPtr pPagerItem = m_pDetailLayout->GetPagerEditItem();
    GetItemDataByName(pPagerItem, kszGbBookpager, itemData);

    CEditListItemPtr pFaxItem = m_pDetailLayout->GetFaxEditItem();
    GetItemDataByName(pFaxItem, kszGbBookFax, itemData);

    CEditListItemPtr pConfUrlItem = m_pDetailLayout->GetConfUrlEditItem();
    GetItemDataByName(pConfUrlItem, kszGbconferenceURL, itemData);

    CEditListItemPtr pPriContactNoItem = m_pDetailLayout->GetPriContactEditItem();
    GetItemDataByName(pPriContactNoItem, kszGbBookprimaryContact, itemData);

    CComboBoxListItemPtr pGroupItem = m_pDetailLayout->GetGroupComboBoxItem();

    if (NULL != pGroupItem)
    {
        itemData.m_nParentId = pGroupItem->GetItemData();
    }
    else
    {
        itemData.m_nParentId = kszGbNAGroupId;
    }

    return true;
}

bool CGenbandContactUIHelper::GetGenbandGroupData(yl::string& strGroupName)
{
    if (NULL == m_pDetailLayout)
    {
        return false;
    }

    CEditListItemPtr pNameItem = m_pDetailLayout->GetNameEditItem();

    if (NULL == pNameItem)
    {
        DIRUI_ERR("GetNameEditItem is null when GetGenbandGroupData");
        return false;
    }

    strGroupName = pNameItem->GetValue();

    return true;
}

void CGenbandContactUIHelper::GetItemDataByName(CEditListItem* pEditItem, const yl::string& strName, ContactGBBookUnit& itemData)
{
    if (NULL == pEditItem)
    {
        return;
    }

    stNumber number;
    number.strName = strName;
    number.strValue = pEditItem->GetValue();
    if (!number.strValue.empty())
    {
        itemData.m_listNumber.push_back(number);
    }
}

void CGenbandContactUIHelper::SetInputFocus(const char * pszAttr)
{
    if (NULL == pszAttr || NULL == m_pDetailLayout)
    {
        return;
    }

    if (0 == strcmp(pszAttr, DIR_ATTR_NAME))
    {
        CEditListItem * pNameItem = m_pDetailLayout->GetNameEditItem();

        if (NULL != pNameItem)
        {
            pNameItem->FocusItem(true);
        }
    }
    else if (0 == strcmp(pszAttr, kszGbBookprimaryContact))
    {
        CEditListItem * pNameItem = m_pDetailLayout->GetPriContactEditItem();

        if (NULL != pNameItem)
        {
            pNameItem->FocusItem(true);
        }
    }
}

void CGenbandContactUIHelper::GetGenbandNumber(LIST_ATTRIBUTE& lsAttr, ContactGBBookUnit* pContact)
{
    if (NULL == pContact)
    {
        return;
    }

    GetNumberByName(pContact, lsAttr, kszGbBookbusinessNumber, TRID_BUSINESS_NUMBER);
    GetNumberByName(pContact, lsAttr, kszGbBookhomeNumber, TRID_HOME_NUMBER);
    GetNumberByName(pContact, lsAttr, kszGbBookmobile, TRID_MOBILE_NUMBER);
    GetNumberByName(pContact, lsAttr, kszGbBookpager, TRID_PAGER);
    GetNumberByName(pContact, lsAttr, kszGbBookFax, TRID_FAX);
    GetNumberByName(pContact, lsAttr, kszGbconferenceURL, TRID_CONFERENCE_URL);
    GetNumberByName(pContact, lsAttr, kszGbBookprimaryContact, TRID_PRIMARYCONTACT);
}

void CGenbandContactUIHelper::GetNumberByName(ContactGBBookUnit* pContact, LIST_ATTRIBUTE & lsAttr,
        const yl::string& strName, const yl::string& strDispName)
{
    if (NULL == pContact)
    {
        return;
    }

    ST_CONTACT_ATTRIBUTE stAttr;

    if (pContact->GetNumberByName(strName, stAttr.m_strValue) && !stAttr.m_strValue.empty())
    {
        stAttr.m_strName = strDispName;
        lsAttr.push_back(stAttr);
    }
}

void CGenbandContactUIHelper::OnChooseAccountResult(int nAccountID)
{
    CDirUIHelper::SetDialAccountId(nAccountID);
    g_DirectoryMgr.LeaveCurrentState();
}

int CGenbandContactUIHelper::GetBrowsePageType(int nGroupId)
{
    return BROWSER_PAGE_TYPE_NORMAL;
}

int CGenbandContactUIHelper::GetListItemType(int nState, int nDataIndex)
{
    if (DIR_UI_STATE_BROWSER == nState)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

        if (pControler.IsEmpty())
        {
            return false;
        }

        int nGroupId = pControler->GetGroupId();

        if (knInvalidRId == nGroupId || kszRootId == nGroupId || kszPabBookGroupId == nGroupId)
        {
            return CMenuListItem::GetMenuItemType();
        }

        return CContactListItem::GetContactListItemType();
    }

    return CDirUIHelper::GetListItemType(nState, nDataIndex);
}

bool CGenbandContactUIHelper::GetOption(int nUIState, VEC_OPTION_ITEM_DATA & vecOption)
{
    if (DIR_UI_STATE_BROWSER != nUIState && DIR_UI_STATE_SEARCH != nUIState && DIR_UI_STATE_PRECISE_SEARCH != nUIState)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);
    if (NULL == pGenbandContact || NULL == m_pListView)
    {
        return false;
    }

    xListViewItemPtr pFocusItem = m_pListView->GetFocusItem();

    if (NULL == pFocusItem)
    {
        return false;
    }

    ST_OPTION_ITEM_DATA stOption;

    int nFocusId = GetLisItemId(pFocusItem);
    if (pGenbandContact->IsGroupId(nFocusId))
    {
        //管理组界面--PAB
        stOption.m_nAction = ST_GBBOOK_EDIT_GROUP;
        stOption.m_strTitle = TRID_RENAME_GROUP;
        vecOption.push_back(stOption);

        stOption.m_nAction = ST_DELETE_GROUP;
        stOption.m_strTitle = TRID_DELETE_GROUP;
        vecOption.push_back(stOption);
        return true;
    }
    else
    {
        stOption.m_nAction = ST_DETAIL;
        stOption.m_strTitle = TRID_DETAIL;
        vecOption.push_back(stOption);

        if (Dir_GetParentIdById(nFocusId, GB_DIR) != kszGabBookGroupId)
        {
            //PAB界面
            stOption.m_nAction = ST_EDIT;
            stOption.m_strTitle = TRID_EDIT;
            vecOption.push_back(stOption);

            stOption.m_nAction = ST_DELETE;
            stOption.m_strTitle = TRID_DELETE;
            vecOption.push_back(stOption);

            if (pGenbandContact->GetGroupId() == kszAllContactId)
            {
                //是否在管理组界面
                stOption.m_nAction = ST_GBBOOK_ENTER_GROUP_MANAGER;
                stOption.m_strTitle = TRID_MANAGE_GROUP;
                vecOption.push_back(stOption);
            }
        }
        else
        {
            if (1 == configParser_GetConfigInt(kszGbPabBookEnable))
            {
                stOption.m_nAction = ST_COPY_TO_PERSONAL;
                stOption.m_strTitle = TRID_ADD_TO_PERSONAL;
                vecOption.push_back(stOption);
            }

            if (CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_GENBAND, pGenbandContact->GetGroupId()))
            {
                stOption.m_nAction = ST_UPDATE;
                stOption.m_strTitle = TRID_REFRESH;
                vecOption.push_back(stOption);
            }
        }
    }

    return true;
}

bool CGenbandContactUIHelper::GetNormalTitle(yl::string& strTitle, int nGroupId/* = knInvalidRId*/)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);

    if (NULL == pGenbandContact)
    {
        return false;
    }

    if (DIR_UI_STATE_PRECISE_SEARCH == pGenbandContact->GetState())
    {
        if (pGenbandContact->IsPreciseSearchCondition())
        {
            return GetTitleBySearchMode(pGenbandContact->GetSearchMode(), strTitle);
        }
        else
        {
            strTitle = _LANG(TRID_GB_GABBOOK);
        }
        return true;
    }

    if (knInvalidRId == nGroupId)
    {
        nGroupId = pGenbandContact->GetGroupId();
    }

    yl::string strGroupName;

    pGenbandContact->GetGroupName(nGroupId, strGroupName);

    GetGroupDisplayName(nGroupId, strGroupName, strTitle);

    return true;
}

bool CGenbandContactUIHelper::GetDetailTitle(yl::string & strTitle)
{
    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
    CDetailAdapter * pDetailAdapter = CDetailAdapter::GetDetailAdapter(pAdapter);

    if (NULL != m_pDetailLayout && m_pDetailLayout == pDetailAdapter->GetDetailLayout())
    {
        int nAction = m_pDetailLayout->GetAction();

        switch (nAction)
        {
        case DIR_ACTION_GENBAND_ADD_PAB:
            {
                strTitle = _LANG(TRID_ADD_PAB);
                return true;
            }
            break;
        case DIR_ACTION_GENBAND_ADD_TO_PAB:
            {
                strTitle = _LANG(TRID_ADD_TO_PERSONAL);
                return true;
            }
            break;
        case DIR_ACTION_GENBAND_ADD_GROUP:
            {
                strTitle = _LANG(TRID_ADD_GROUP);
                return true;
            }
            break;
        default:
            break;
        }
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);

    if (NULL == pGenbandContact)
    {
        return false;
    }

    int nUiStatus = pGenbandContact->GetState();
    if (DIR_UI_STATE_CHOOSE_NUMBER == nUiStatus)
    {
        int nDialAccountId = GetDialAccountId();
        if (-1 == nDialAccountId)
        {
            nDialAccountId = acc_GetDefaultAccount();
        }
        strTitle = _LANG(TRID_ACCOUNT_COLON) + acc_GetUsername(nDialAccountId);
        return true;
    }
    else if (DIR_UI_STATE_CHOOSE_ACCOUNT == nUiStatus)
    {
        strTitle = _LANG(TRID_CHOOSE_ACCOUNT);
        return true;
    }

    return CCommonUIHelper::GetDetailTitle(strTitle);
}

bool CGenbandContactUIHelper::GetSoftKey(xListViewItem * pFocusItem, int nUIState,
        CArraySoftKeyBarData & vecSoftKey)
{
    if (vecSoftKey.size() < 4)
    {
        return false;
    }

    if (DIR_UI_STATE_CHOOSE_NUMBER == nUIState)
    {
        //号码选择界面
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_ACCOUNT);
        vecSoftKey[1].m_iType = ST_SELECT;
        return true;
    }

    if (DIR_UI_STATE_CHOOSE_ACCOUNT == nUIState)
    {
        //账号选择界面
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_OK);
        vecSoftKey[3].m_iType = ST_OK;
        return true;
    }

    if (DIR_UI_STATE_PRECISE_SEARCH == nUIState)
    {
        CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
        CPreciseSearchAdapter * pSearchAdapter = CPreciseSearchAdapter::GetPreciseSearchAdapter(pAdapter);
        if (NULL != pSearchAdapter)
        {
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
            vecSoftKey[3].m_iType = ST_GBBOOK_PRECISE_SEARCH;
        }
        return true;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);
    if (NULL == pGenbandContact)
    {
        return false;
    }

    if (DIR_UI_STATE_DETAIL == nUIState || DIR_UI_STATE_ADDITION == nUIState)
    {
        // 详情和添加的编辑界面
        CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
        CDetailAdapter * pDetailAdapter = CDetailAdapter::GetDetailAdapter(pAdapter);

        if (NULL != pDetailAdapter && NULL != pDetailAdapter->GetDetailLayout())
        {
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SAVE);
            vecSoftKey[3].m_iType = ST_SAVE;
        }
        else if (!pGenbandContact->IsPreciseSearchResult())
        {
            vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_EDIT);
            vecSoftKey[1].m_iType = ST_EDIT;
        }

        return true;
    }

    bool bEditContact = CDirectoryControler::IsDirCanEdit(DIRECTORY_TYPE_GENBAND, pGenbandContact->GetGroupId());
    int nFocusId = GetLisItemId(pFocusItem);
    if (knInvalidRId == nFocusId)
    {
        if (DIR_UI_STATE_BROWSER == nUIState)
        {
            CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
            CBrowseAdapter* pBrowseAdapter = CBrowseAdapter::GetBrowserAdapter(pAdapter);
            if (NULL == pBrowseAdapter || pBrowseAdapter->IsMenuPage())
            {
                return true;
            }

            if (kszGabBookGroupId == pGenbandContact->GetGroupId())
            {
                vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
                vecSoftKey[1].m_iType = ST_GBBOOK_PRECISE_SEARCH_MODE_CHOOSE;
            }
            else if (bEditContact)
            {
                vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_ADD);
                vecSoftKey[1].m_iType = ST_ADD;
            }

            return true;
        }

        return false;
    }

    if (DIR_UI_STATE_BROWSER == nUIState && pGenbandContact->IsGroupId(nFocusId))
    {
        //组管理界面
        if (bEditContact)
        {
            vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_ADD);
            vecSoftKey[1].m_iType = ST_ADDGROUP;

            if (nFocusId != kszGbNAGroupId)
            {
                vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_OPTION);
                vecSoftKey[2].m_iType = ST_OPTION;
            }
            else
            {
                vecSoftKey[2].m_strSoftkeyTitle = "";
                vecSoftKey[2].m_iType = ST_EMPTY;
            }
        }

        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ENTER);
        vecSoftKey[3].m_iType = ST_ENTER;

        return true;
    }

    // 焦点在联系人上（包括搜索界面）
    if (pGenbandContact->GetGroupId() == kszGabBookGroupId)
    {
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
        vecSoftKey[1].m_iType = ST_GBBOOK_PRECISE_SEARCH_MODE_CHOOSE;
    }
    else
    {
        if (DIR_UI_STATE_SEARCH != nUIState && bEditContact)
        {
            vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_ADD);
            if (pGenbandContact->GetGroupId() == kszPabBookGroupId)
            {
                vecSoftKey[1].m_iType = ST_ADDGROUP;
            }
            else
            {
                vecSoftKey[1].m_iType = ST_ADD;
            }
        }
    }

    GetContactSoftKey(pFocusItem, nUIState, pGenbandContact->GetGroupId(), vecSoftKey);

    return true;
}

int CGenbandContactUIHelper::GetActionByUIAction(int nUIState, int nUIAction)
{
    int nAction = GetContactCopyAction(nUIAction);

    if (DIR_ACTION_NONE != nAction)
    {
        return nAction;
    }

    if (ST_GBBOOK_PRECISE_SEARCH_MODE_CHOOSE == nUIAction)
    {
        return DIR_ACTION_GBBOOK_SEARCH_MODE_CHANGE;
    }
    else if (ST_GBBOOK_PRECISE_SEARCH_NAME == nUIAction)
    {
        return DIR_ACTION_GENBAND_SEARCH_BY_NAME;
    }
    else if (ST_GBBOOK_PRECISE_SEARCH_FIRST_NAME == nUIAction)
    {
        return DIR_ACTION_GENBAND_SEARCH_BY_FIRSTNAME;
    }
    else if (ST_GBBOOK_PRECISE_SEARCH_LAST_NAME == nUIAction)
    {
        return DIR_ACTION_GENBAND_SEARCH_BY_LASTNAME;
    }
    else if (ST_GBBOOK_PRECISE_SEARCH_NUMBER == nUIAction)
    {
        return DIR_ACTION_GENBAND_SEARCH_BY_NUMBER;
    }
    else if (ST_GBBOOK_PRECISE_SEARCH == nUIAction)
    {
        return DIR_ACTION_GENBAND_PRECISE_SEARCH;
    }
    else if (ST_ADD == nUIAction)
    {
        return DIR_ACTION_GENBAND_ENTER_ADD_PAB;
    }
    else if (ST_EDIT == nUIAction)
    {
        return DIR_ACTION_GENBAND_ENTER_EDIT_PAB;
    }
    else if (ST_ADDGROUP == nUIAction)
    {
        return DIR_ACTION_GENBAND_ENTER_ADD_GROUP;
    }
    else if (ST_GBBOOK_EDIT_GROUP == nUIAction)
    {
        return DIR_ACTION_GENBAND_ENTER_EDIT_GROUP;
    }
    else if (ST_DELETE == nUIAction)
    {
        return DIR_ACTION_GENBAND_DEL_PAB;
    }
    else if (ST_DELETE_GROUP == nUIAction)
    {
        return DIR_ACTION_GENBAND_DEL_GROUP;
    }
    else if (ST_GBBOOK_ENTER_GROUP_MANAGER == nUIAction)
    {
        return DIR_ACTION_GENBAND_ENTER_MANAGER_GROUP;
    }
    else if (ST_COPY_TO_PERSONAL == nUIAction)
    {
        return DIR_ACTION_GENBAND_ENTER_ADD_TO_PAB;
    }
    else if (ST_UPDATE == nUIAction)
    {
        return DIR_ACTION_GENBAND_UPDATE;
    }
    else if (ST_SELECT == nUIAction)
    {
        return DIR_ACTION_GENBAND_SELECT_ACCOUNT;
    }
    else if (ST_SAVE == nUIAction)
    {
        if (NULL != m_pDetailLayout)
        {
            return m_pDetailLayout->GetAction();
        }
    }

    return CCommonUIHelper::GetActionByUIAction(nUIState, nUIAction);
}

int CGenbandContactUIHelper::GetActionByKeyPress(int nUIState, int nKey)
{
    if (DIR_UI_STATE_BROWSER == nUIState && PHONE_KEY_CANCEL == nKey)
    {
        return DIR_ACTION_COMMON_BACK;
    }

    return CCommonUIHelper::GetActionByKeyPress(nUIState, nKey);
}

void CGenbandContactUIHelper::OnEnterDetail(int nAction)
{
    DIRUI_DEBUG("OnEnterDetail Action:%d", nAction);

    if (DIR_ACTION_COMMON_DETAIL == nAction)
    {
        return;
    }

    if (NULL == m_pDetailLayout)
    {
        m_pDetailLayout = new CGenbandDetailLayout;

        if (NULL == m_pDetailLayout)
        {
            return;
        }
    }

    m_pDetailLayout->SetAction(nAction);
    m_pDetailLayout->Init();

    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
    CDetailAdapter * pDetailAdapter = CDetailAdapter::GetDetailAdapter(pAdapter);

    if (NULL == pDetailAdapter)
    {
        return;
    }

    CDirectoryControlerPtr pControler = pDetailAdapter->GetControler();
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);

    if (NULL == pGenbandContact)
    {
        return;
    }

    pDetailAdapter->SetDetailLayout(m_pDetailLayout);

    if (DIR_ACTION_GENBAND_ADD_GROUP == nAction)
    {
        pGenbandContact->ResetContactId();
    }
    else if (DIR_ACTION_GENBAND_EDIT_GROUP == nAction)
    {
        int nContactId = pGenbandContact->GetContactId();
        SmartPtr pDetail = pGenbandContact->GetDetail(nContactId);
        m_pDetailLayout->UpdateGroup(nContactId, (DirGroupRecord*)pDetail.Get());
    }
    else if (DIR_ACTION_GENBAND_ADD_PAB == nAction)
    {
        pGenbandContact->ResetContactId();
        AddGroupOption();
    }
    else
    {
        int nContactId = pGenbandContact->GetContactId();
        SmartPtr pDetail = pGenbandContact->GetDetail(nContactId);
        ContactGBBookUnit* pGenbandRecord = CGenbandContactControler::GetGenbandContact(pDetail);
        m_pDetailLayout->UpdateContact(nContactId, pGenbandRecord);
        AddGroupOption();

        if (DIR_ACTION_GENBAND_ADD_TO_PAB == nAction)
        {
            pGenbandContact->ResetContactId();
        }
    }
}

void CGenbandContactUIHelper::AddGroupOption()
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CGenbandContactControler * pGenbandContact = CGenbandContactControler::GetControler(pControler);
    if (NULL == pGenbandContact)
    {
        return;
    }

    CComboBoxListItemPtr pGroupComboBoxItem = m_pDetailLayout->GetGroupComboBoxItem();

    if (NULL == pGroupComboBoxItem)
    {
        return;
    }

    // 初始化普通群组
    yl::string strGroupName = "";
    yl::string strGroupShowName = "";

    ContactListData listData;
    listData.m_nIndexFrom = 0;
    listData.m_nCountToGet = 0;
    Dir_GetContactListByGroupId(&listData, kszPabBookGroupId, GB_DIR);

    int nSize = listData.m_listId.size();

    for (int i = 0; i < nSize; ++i)
    {
        int nGroupId = listData.m_listId[i];

        if (!pGenbandContact->GetGroupName(nGroupId, strGroupName))
        {
            continue;
        }

        GetGroupDisplayName(nGroupId, strGroupName, strGroupShowName);

        pGroupComboBoxItem->AddValue(strGroupShowName, nGroupId);
    }

    int nCurGroupId = pGenbandContact->GetGroupId();
    pGroupComboBoxItem->SetItemData(nCurGroupId <= 0 ? kszGbNAGroupId : nCurGroupId);
}

bool CGenbandContactUIHelper::UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail)
{
    if (NULL == pListItem || pDetail.IsEmpty())
    {
        return false;
    }

    CMenuListItemPtr pMenuItem = CMenuListItem::GetMenuListItem(pListItem);
    if (NULL != pMenuItem)
    {
        yl::string strDispName = "";
        yl::string strGroupName = pDetail->GetDisplayName();

        GetGroupDisplayName(nId, strGroupName, strDispName);

        return UpdateMenuItem(pMenuItem, nId, strDispName, true);
    }

    ContactGBBookUnit* pRecord = CGenbandContactControler::GetGenbandContact(pDetail);
    if (NULL == pRecord)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CGenbandContactControler* pGenbandContact = CGenbandContactControler::GetControler(pControler);
    if (NULL == pGenbandContact)
    {
        return false;
    }

    yl::string strDisplayName = pRecord->m_strFirstName;
    if (!pRecord->m_strLastName.empty())
    {
        strDisplayName += " ";
        strDisplayName += pRecord->m_strLastName;
    }

    if (strDisplayName.empty())
    {
        strDisplayName = pRecord->GetDisplayName();
    }

    yl::string strNum = "";
    if (pGenbandContact->IsSearchState())
    {
        yl::string strSearchKey = pGenbandContact->GetSearchKey();
        strNum = pRecord->GetSearchShowNumber(strSearchKey);
    }
    else
    {
        if (!pRecord->GetNumberByName(kszGbBookbusinessNumber, strNum) || strNum.empty())
        {
            if (!pRecord->GetNumberByName(kszGbBookhomeNumber, strNum) || strNum.empty())
            {
                pRecord->GetNumberByName(kszGbBookmobile, strNum);
            }
        }
    }

    return UpdateContactItem(pListItem, nId, strDisplayName, strNum);
}

void CGenbandContactUIHelper::ShowSearchModeOption(int nUIState)
{
    VEC_OPTION_ITEM_DATA listOption;

    if (!GetSearchModeOption(nUIState, listOption))
    {
        return;
    }

    int nSelectIndex = GetSearchTypeIndex();
    bool bShowIcon = (-1 != nSelectIndex);
    g_DirMsgBoxMgr.ShowOptionMsgBox(listOption, -1, nSelectIndex, bShowIcon);

}

bool CGenbandContactUIHelper::GetSearchModeOption(int nUIState, VEC_OPTION_ITEM_DATA & vecOption)
{
    if (DIR_UI_STATE_BROWSER != nUIState && DIR_UI_STATE_SEARCH != nUIState
            && DIR_UI_STATE_PRECISE_SEARCH != nUIState)
    {
        return false;
    }

    ST_OPTION_ITEM_DATA stOption;

    stOption.m_nAction = ST_GBBOOK_PRECISE_SEARCH_NAME;
    stOption.m_strTitle = _LANG(TRID_GENBAND_SEARCH_BY_NAME);
    vecOption.push_back(stOption);

    stOption.m_nAction = ST_GBBOOK_PRECISE_SEARCH_FIRST_NAME;
    stOption.m_strTitle = _LANG(TRID_GENBAND_SEARCH_BY_FIRST_NAME);
    vecOption.push_back(stOption);

    stOption.m_nAction = ST_GBBOOK_PRECISE_SEARCH_LAST_NAME;
    stOption.m_strTitle = _LANG(TRID_GENBAND_SEARCH_BY_LAST_NAME);
    vecOption.push_back(stOption);

    stOption.m_nAction = ST_GBBOOK_PRECISE_SEARCH_NUMBER;
    stOption.m_strTitle = _LANG(TRID_GENBAND_SEARCH_BY_NUMBER);
    vecOption.push_back(stOption);

    return true;
}

int CGenbandContactUIHelper::GetSearchTypeIndex()
{
    GBBOOK_OPERATION_TYPE eType = GAB_GetCurrentGabOptType();
    int iIndex = -1;
    if (GAB_SEARCH_NAME == eType)
    {
        iIndex = 0;
    }
    if (GAB_SEARCH_FIRSTNAME == eType)
    {
        iIndex = 1;
    }
    if (GAB_SEARCH_LASTNAME == eType)
    {
        iIndex = 2;
    }
    if (GAB_SEARCH_NUMBER == eType)
    {
        iIndex = 3;
    }

    return iIndex;
}

void CGenbandContactUIHelper::GetGroupDisplayName(int nGroupId, const yl::string & strGroupName,
        yl::string & strDisplayName)
{
    if (kszAllContactId == nGroupId)
    {
        strDisplayName = _LANG(TRID_GB_PABBOOK);
    }
    else if (kszGabBookGroupId == nGroupId)
    {
        strDisplayName = _LANG(TRID_GB_GABBOOK);
    }
    else if (kszPabBookGroupId == nGroupId)
    {
        strDisplayName = _LANG(TRID_MANAGE_GROUP);
    }
    else if (kszGbNAGroupId == nGroupId)
    {
        strDisplayName = _LANG(TRID_NO_GROUP);
    }
    else if (!strGroupName.empty())
    {
        strDisplayName = strGroupName;
    }
}

bool CGenbandContactUIHelper::GetTitleBySearchMode(int nSearchMode, yl::string& strTitle)
{
    switch (nSearchMode)
    {
    case GAB_SEARCH_NAME:
        {
            strTitle = _LANG(TRID_GENBAND_SEARCH_BY_NAME);
        }
        break;
    case GAB_SEARCH_FIRSTNAME:
        {
            strTitle = _LANG(TRID_GENBAND_SEARCH_BY_FIRST_NAME);
        }
        break;
    case GAB_SEARCH_LASTNAME:
        {
            strTitle = _LANG(TRID_GENBAND_SEARCH_BY_LAST_NAME);
        }
        break;
    case GAB_SEARCH_NUMBER:
        {
            strTitle = _LANG(TRID_GENBAND_SEARCH_BY_NUMBER);
        }
        break;
    default:
        {
            return false;
        }
        break;
    }

    return true;
}

#endif
