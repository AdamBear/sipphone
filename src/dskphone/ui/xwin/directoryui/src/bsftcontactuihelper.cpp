#include "bsftcontactuihelper.h"
#include "bsftcontactcontroler.h"
#include "browseadapter.h"
#include "detailadapter.h"
#include "precisesearchadapter.h"
#include "dirctrlfactory.h"
#include "directorymgr.h"
#include "moddirectoryui.h"
#include "bsftdetaillayout.h"
#include "bsftsearchlayout.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "xwindow/xListView.h"
#include "baseui/include/comboboxlistitem.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/menulistitem.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"


CBSFTContactUIHelper::CBSFTContactUIHelper()
    : CCommonUIHelper(DIRECTORY_TYPE_BROADSOFT)
    , m_pDetailLayout(NULL)
    , m_pSearchLayout(NULL)
{
}

CBSFTContactUIHelper::~CBSFTContactUIHelper()
{
    if (NULL != m_pDetailLayout)
    {
        delete m_pDetailLayout;
        m_pDetailLayout = NULL;
    }

    if (NULL != m_pSearchLayout)
    {
        delete m_pSearchLayout;
        m_pSearchLayout = NULL;
    }
}

CBSFTContactUIHelper * CBSFTContactUIHelper::GetUIHelper(IDirUIHelperPtr & pDirUIHelper)
{
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper || DIRECTORY_TYPE_BROADSOFT != pUIHelper->GetType())
    {
        return NULL;
    }

    return static_cast<CBSFTContactUIHelper *>(pUIHelper);
}

IBSFTContactUIHelper * CBSFTContactUIHelper::GetBSFTUIHelper(IDirUIHelperPtr & pDirUIHelper)
{
    CBSFTContactUIHelper * pBSFTContactUIHelper = CBSFTContactUIHelper::GetUIHelper(pDirUIHelper);

    return (IBSFTContactUIHelper *)pBSFTContactUIHelper;
}

bool CBSFTContactUIHelper::GetGroupDisplayName(int nGroupId, yl::string & strGroupName)
{
    if (kszAllContactId == nGroupId)
    {
        strGroupName = _LANG(TRID_ALL_CONTACTS);

        return true;
    }

    if (!strGroupName.empty())
    {
        return true;
    }

    if (kszPersonalId == nGroupId)
    {
        strGroupName = _LANG(TRID_PERSONAL);
    }
    else if (kszGroupId == nGroupId)
    {
        strGroupName = _LANG(TRID_GROUP);
    }
    else if (kszEnterPriseId == nGroupId)
    {
        strGroupName = _LANG(TRID_ENTERPRISE);
    }
    else if (kszGroupCommonId == nGroupId)
    {
        strGroupName = _LANG(TRID_GROUP_COMMON);
    }
    else if (kszEnterPriseCommonId == nGroupId)
    {
        strGroupName = _LANG(TRID_ENTERPRISE_COMMON);
    }
    else if (kszCustomId == nGroupId)
    {
        strGroupName = _LANG(TRID_CUSTOM_CONTACT);
    }
    else
    {
        return false;
    }

    return true;
}

bool CBSFTContactUIHelper::GetBSFTPersonalData(ContactBroadSoftPersonal & itemData)
{
    if (NULL == m_pDetailLayout)
    {
        return false;
    }

    CEditListItemPtr pNameItem = m_pDetailLayout->GetNameEditItem();
    CEditListItemPtr pNumberItem = m_pDetailLayout->GetNumberEditItem();

    if (NULL == pNameItem || NULL == pNumberItem)
    {
        return false;
    }

    pNameItem->ReturnAllInput();
    pNumberItem->ReturnAllInput();

    itemData.m_strDisplayName = pNameItem->GetValue().trim_both();

    yl::string strNumber = pNumberItem->GetValue().trim_both();

    itemData.SetNumber(strNumber);

    return true;
}

bool CBSFTContactUIHelper::GetBSFTPreciseSearchData(int & nGroupId,
        ContactBroadSoftItemData & itemData)
{
    if (NULL == m_pSearchLayout)
    {
        return false;
    }

    m_pSearchLayout->ReturnAllEdit();

    CEditListItemPtr pFirstNameItem = m_pSearchLayout->GetFirstNameItem();
    yl::string strFirstName;

    if (NULL != pFirstNameItem)
    {
        strFirstName = pFirstNameItem->GetValue().trim_both();
    }

    CEditListItemPtr pLastNameItem = m_pSearchLayout->GetLastNameItem();
    yl::string strLastName;

    if (NULL != pLastNameItem)
    {
        strLastName = pLastNameItem->GetValue().trim_both();
    }

    itemData.m_strDisplayName = strFirstName + "," + strLastName;

    CEditListItemPtr pNumberItem = m_pSearchLayout->GetNumberItem();

    if (NULL != pNumberItem)
    {
        itemData.m_strNumber = pNumberItem->GetValue().trim_both();
    }

    CEditListItemPtr pExtensionItem = m_pSearchLayout->GetExtensionItem();

    if (NULL != pExtensionItem)
    {
        itemData.m_strExtension = pExtensionItem->GetValue().trim_both();
    }

    CEditListItemPtr pMobileItem = m_pSearchLayout->GetMobileItem();

    if (NULL != pMobileItem)
    {
        itemData.m_strMobile = pMobileItem->GetValue().trim_both();
    }

    CEditListItemPtr pEMailItem = m_pSearchLayout->GetEmailItem();

    if (NULL != pEMailItem)
    {
        itemData.m_strEmail = pEMailItem->GetValue().trim_both();
    }

    CEditListItemPtr pDepartmentItem = m_pSearchLayout->GetDepartmentItem();

    if (NULL != pDepartmentItem)
    {
        itemData.m_strDepartment = pDepartmentItem->GetValue().trim_both();
    }

    CComboBoxListItemPtr pGroupItem = m_pSearchLayout->GetGroupItem();

    if (NULL != pGroupItem)
    {
        nGroupId = pGroupItem->GetItemData();
    }

    char szBuffer[32];

    snprintf(szBuffer, sizeof(szBuffer), "%d", nGroupId);
    szBuffer[sizeof(szBuffer) - 1] = '\0';
    itemData.m_strGroupId = szBuffer;

    return true;
}

void CBSFTContactUIHelper::SetInputFocus(const char * pszAttr)
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

        return;
    }

    if (0 == strcmp(pszAttr, DIR_ATTR_NUMBER))
    {
        CEditListItem * pNumberItem = m_pDetailLayout->GetNumberEditItem();

        if (NULL != pNumberItem)
        {
            pNumberItem->FocusItem(true);
        }
    }
}

void CBSFTContactUIHelper::EnterPreciseSearch()
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL != pBSFTContact && DIR_UI_STATE_PRECISE_SEARCH == pBSFTContact->GetState())
    {
        if (pBSFTContact->IsPreciseSearchCondition())
        {
            return;
        }

        pBSFTContact->EnterPreciseSearch();

        if (NULL != m_pSearchLayout)
        {
            m_pSearchLayout->ClearEdit();
        }

        RefreshUI(DIR_UI_REFRESH_TYPE_ALL, DIR_UI_REFRESH_PARAM_ALL);

        return;
    }

    g_DirectoryMgr.EnterPreciseSearch();
}

void CBSFTContactUIHelper::OnPreciseSearchResult()
{
    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
    CPreciseSearchAdapter * pSearchAdapter = CPreciseSearchAdapter::GetPreciseSearchAdapter(pAdapter);

    if (NULL != pSearchAdapter)
    {
        pSearchAdapter->RefreshUI(DIR_UI_REFRESH_TYPE_TITLE);
    }
}

void CBSFTContactUIHelper::FocusDefaultNumber()
{
    if (CheckFocusSearchNum(false))
    {
        return;
    }

    FocusFirstDetailNumber();
}

bool CBSFTContactUIHelper::GetTitlePageInfo(yl::string& strPageInfo)
{
    if (NULL == m_pListView || BsftDir_UpdateEnable())
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL == pBSFTContact || !pBSFTContact->IsHasNextPageData())
    {
        return false;
    }

    xListViewItemPtr pFocusItem = m_pListView->GetFocusItem();
    CMenuListItem* pMenuItem = CMenuListItem::GetMenuListItem(pFocusItem);
    if (NULL != pMenuItem && kszViewMoreId == pMenuItem->GetId())
    {
        strPageInfo = "";
        return true;
    }

    int nTotalCount = m_pListView->GetTotalDataCount();

    if (nTotalCount <= 0)
    {
        return false;
    }

    --nTotalCount;

    int nFocusIndex = m_pListView->GetFocusItemDataIndex();

    if (INVALID_ITEM_INDEX == nFocusIndex)
    {
        nFocusIndex = 0;
    }
    else
    {
        nFocusIndex += 1;
    }

    strPageInfo = commonAPI_FormatString("%d/%d", nFocusIndex, nTotalCount);

    return true;
}

int CBSFTContactUIHelper::GetListItemType(int nState, int nDataIndex)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);
    if (NULL != pBSFTContact && pBSFTContact->IsViewMoreId(nDataIndex))
    {
        return CMenuListItem::GetMenuItemType();
    }

    if (DIR_UI_STATE_BROWSER == nState
            && NULL != pBSFTContact
            && kszCustomId == pBSFTContact->GetGroupId())
    {
        return CMenuListItem::GetMenuItemType();
    }

    return CCommonUIHelper::GetListItemType(nState, nDataIndex);
}

bool CBSFTContactUIHelper::RefreshItem(int nState, xListViewItem * pItem)
{
    CMenuListItem* pMenuItem = CMenuListItem::GetMenuListItem(pItem);
    if (NULL != pMenuItem)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);
        if (NULL != pBSFTContact && pBSFTContact->IsViewMoreId(pMenuItem->GetDataIndex()))
        {
            pMenuItem->SetMenu(_LANG(TRID_VIEW_MORE_CONTACTS));
            pMenuItem->SetShowIndex(false);
            pMenuItem->SetId(kszViewMoreId);
            return true;
        }
    }

    return CCommonUIHelper::RefreshItem(nState, pItem);
}

bool CBSFTContactUIHelper::GetOption(int nUIState, VEC_OPTION_ITEM_DATA & vecOption)
{
    if (DIR_UI_STATE_BROWSER != nUIState && DIR_UI_STATE_SEARCH != nUIState
            && DIR_UI_STATE_PRECISE_SEARCH != nUIState)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL == pBSFTContact || NULL == m_pListView)
    {
        return false;
    }

    xListViewItemPtr pFocusItem = m_pListView->GetFocusItem();

    if (NULL == pFocusItem)
    {
        return false;
    }

    ST_OPTION_ITEM_DATA stOption;

    PushDetailOption(stOption, vecOption);

#ifdef IF_FEATURE_DIR_OPT
    stOption.m_nAction = ST_UPDATE;
    stOption.m_strTitle = _LANG(TRID_UPDATE);

    vecOption.push_back(stOption);
#endif

    if (!CBSFTContactControler::IsExitPersonal()
            || !CDirectoryControler::IsDirCanEdit(DIRECTORY_TYPE_BROADSOFT, kszPersonalId))
    {
        return true;
    }

    stOption.m_nAction = ST_ADD;
    stOption.m_strTitle = _LANG(TRID_ADD);
    vecOption.push_back(stOption);

    int nFocusId = GetLisItemId(pFocusItem);

    if (pBSFTContact->IsPersonal(nFocusId))
    {
        stOption.m_nAction = ST_DELETE;
        stOption.m_strTitle = _LANG(TRID_DELETE);
        vecOption.push_back(stOption);
    }
    else
    {
        stOption.m_nAction = ST_COPY_TO_PERSONAL;
        stOption.m_strTitle = _LANG(TRID_ADD_TO_PERSONAL);
        vecOption.push_back(stOption);
    }

    return true;
}

void CBSFTContactUIHelper::GetMenuTitle(yl::string & strTitle)
{
    strTitle = _LANG(TRID_NETWORK_DIRECTORY);
}

bool CBSFTContactUIHelper::GetNormalTitle(yl::string & strTitle, int nGroupId/* = knInvalidRId*/)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBsftContact = CBSFTContactControler::GetControler(pControler);

    if (NULL == pBsftContact)
    {
        return false;
    }

    if (DIR_UI_STATE_PRECISE_SEARCH == pBsftContact->GetState())
    {
        if (pBsftContact->IsPreciseSearchResult())
        {
            strTitle = _LANG(TRID_SEARCH_RESULT);
        }
        else
        {
            strTitle = _LANG(TRID_SEARCH);
        }

        return true;
    }

    if (knInvalidRId == nGroupId)
    {
        nGroupId = pBsftContact->GetGroupId();
    }

    if (kszAllContactId != nGroupId && !pBsftContact->GetGroupName(nGroupId, strTitle))
    {
        return false;
    }

    return GetGroupDisplayName(nGroupId, strTitle);
}

bool CBSFTContactUIHelper::GetDetailTitle(yl::string & strTitle)
{
    if (CCommonUIHelper::GetDetailTitle(strTitle))
    {
        return true;
    }

    if (NULL != m_pDetailLayout)
    {
        switch (m_pDetailLayout->GetAction())
        {
        case DIR_ACTION_BSFT_ADD_PERSONAL:
            {
                strTitle = _LANG(TRID_ADD_PERSONAL);
            }
            break;
        case DIR_ACTION_BSFT_ADD_TO_PERSONAL:
            {
                strTitle = _LANG(TRID_ADD_TO_PERSONAL);
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

    return false;
}

bool CBSFTContactUIHelper::GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                                      CArraySoftKeyBarData & vecSoftKey)
{
    // 详情和添加界面
    if (DIR_UI_STATE_DETAIL == nUIState)
    {
        CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
        CDetailAdapter * pDetailAdapter = CDetailAdapter::GetDetailAdapter(pAdapter);

        if (NULL != pDetailAdapter && NULL != pDetailAdapter->GetDetailLayout())
        {
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SAVE);
            vecSoftKey[3].m_iType = ST_SAVE;
        }

        return true;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL == pBSFTContact)
    {
        return false;
    }

    if (DIR_UI_STATE_PRECISE_SEARCH == pBSFTContact->GetState())
    {
        if (!pBSFTContact->IsPreciseSearchResult())
        {
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
            vecSoftKey[3].m_iType = ST_BSFT_PRECISE_SEARCH;

            return true;
        }

        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
        vecSoftKey[1].m_iType = ST_BSFT_ENTER_PRECISE_SEARCH;
    }

    bool bEditContact = CBSFTContactControler::IsExitPersonal();

    if (bEditContact)
    {
        bEditContact = CDirectoryControler::IsDirCanEdit(DIRECTORY_TYPE_BROADSOFT, kszPersonalId);
    }

    int nFocusId = GetLisItemId(pFocusItem);
    bool bUpdate = BsftDir_UpdateEnable();

    //焦点在View More上
    if (kszViewMoreId == nFocusId && !bUpdate)
    {
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
        vecSoftKey[1].m_iType = ST_BSFT_ENTER_PRECISE_SEARCH;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_OK);
        vecSoftKey[3].m_iType = ST_OK;
        return true;
    }

    if (knInvalidRId == nFocusId)
    {
        if (DIR_UI_STATE_PRECISE_SEARCH == nUIState)
        {
            if (NULL == pFocusItem)
            {
                vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
                vecSoftKey[1].m_iType = ST_BSFT_ENTER_PRECISE_SEARCH;

                if (bEditContact && kszPersonalId == pBSFTContact->GetGroupId())
                {
                    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ADD);
                    vecSoftKey[3].m_iType = ST_ADD;
                }
            }
            else
            {
                vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
                vecSoftKey[3].m_iType = ST_BSFT_PRECISE_SEARCH;
            }

            return true;
        }
        else if (DIR_UI_STATE_SEARCH == nUIState && !bUpdate)
        {
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
            vecSoftKey[3].m_iType = ST_SEARCH;
            return true;
        }
        else if (DIR_UI_STATE_BROWSER == nUIState)
        {
            CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
            CBrowseAdapter * pBrowserAdapter = CBrowseAdapter::GetBrowserAdapter(pAdapter);

            if (NULL == pBrowserAdapter || pBrowserAdapter->IsMenuPage())
            {
                return true;
            }

            if (XSI_SUCCESS == CBaseHelper::GetXSIStatus())
            {
                vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
                vecSoftKey[1].m_iType = ST_BSFT_ENTER_PRECISE_SEARCH;
            }

            if (bEditContact && kszPersonalId == pBSFTContact->GetGroupId())
            {
                vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ADD);
                vecSoftKey[3].m_iType = ST_ADD;
            }

            return true;
        }

        return false;
    }

    if (DIR_UI_STATE_BROWSER == nUIState && pBSFTContact->IsGroupId(nFocusId))
    {
        if (bEditContact && bUpdate)
        {
            vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_UPDATE);
            vecSoftKey[1].m_iType = ST_UPDATE;
        }

        if (!bUpdate && kszCustomId == nFocusId)
        {
            vecSoftKey[2].m_strSoftkeyTitle = "";
            vecSoftKey[2].m_iType = ST_EMPTY;
        }

        return true;
    }

    // 焦点在联系人上（包括搜索界面）
    vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
    vecSoftKey[1].m_iType = ST_BSFT_ENTER_PRECISE_SEARCH;

    GetContactSoftKey(pFocusItem, nUIState, pBSFTContact->GetGroupId(), vecSoftKey);

    return true;
}

int CBSFTContactUIHelper::GetActionByKeyPress(int nUIState, int nKey)
{
    if (PHONE_KEY_OK == nKey && NULL != m_pListView)
    {
        xListViewItem* pItem = m_pListView->GetFocusItem();
        CMenuListItemPtr pMenuItem = CMenuListItem::GetMenuListItem(pItem);
        if (NULL != pMenuItem && kszViewMoreId == pMenuItem->GetId())
        {
            return DIR_ACTION_COMMON_SELECT;
        }
    }

    if (DIR_UI_STATE_DETAIL == nUIState && PHONE_KEY_OK == nKey && NULL != m_pDetailLayout)
    {
        CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
        CDetailAdapter* pDetailAdapter = CDetailAdapter::GetDetailAdapter(pAdapter);
        if (NULL != pDetailAdapter && m_pDetailLayout == pDetailAdapter->GetDetailLayout())
        {
            return m_pDetailLayout->GetAction();
        }
    }

    if (PHONE_KEY_CANCEL == nKey)
    {
        int nAction = GetBackAction();
        if (DIR_ACTION_NONE != nAction)
        {
            return nAction;
        }
    }

    return CCommonUIHelper::GetActionByKeyPress(nUIState, nKey);
}

int CBSFTContactUIHelper::GetActionByUIAction(int nUIState, int nUIAction)
{
    if (ST_ADD == nUIAction)
    {
        return DIR_ACTION_BSFT_ENTER_ADD_PERSONAL;
    }
    else if (ST_DELETE == nUIAction)
    {
        return DIR_ACTION_BSFT_DEL_PERSONAL;
    }
    else if (ST_COPY_TO_PERSONAL == nUIAction)
    {
        return DIR_ACTION_BSFT_ENTER_ADD_TO_PERSONAL;
    }
    else if (ST_UPDATE == nUIAction)
    {
        return DIR_ACTION_BSFT_UPDATE;
    }
    else if (ST_SAVE == nUIAction)
    {
        if (NULL != m_pDetailLayout)
        {
            return m_pDetailLayout->GetAction();
        }

        return DIR_ACTION_NONE;
    }
    else if (ST_BSFT_ENTER_PRECISE_SEARCH == nUIAction)
    {
        return DIR_ACTION_BSFT_ENTER_PRECISE_SEARCH;
    }
    else if (ST_BSFT_PRECISE_SEARCH == nUIAction)
    {
        return DIR_ACTION_BSFT_PRECISE_SEARCH;
    }
    else if (ST_OK == nUIAction)
    {
        return DIR_ACTION_COMMON_SELECT;
    }
    else if (ST_BACK == nUIAction)
    {
        int nAction = GetBackAction();
        if (DIR_ACTION_NONE != nAction)
        {
            return nAction;
        }
    }

    return CCommonUIHelper::GetActionByUIAction(nUIState, nUIAction);
}

int CBSFTContactUIHelper::GetBackAction()
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBSFTControler = CBSFTContactControler::GetControler(pControler);
    if (NULL == pBSFTControler)
    {
        return DIR_ACTION_NONE;
    }

    if (pBSFTControler->IsBackToMenuList())
    {
        return DIR_ACTION_BSFT_BACK_TO_MENU_LIST;
    }

    if (pBSFTControler->IsBackToSearchResult())
    {
        return DIR_ACTION_BSFT_BACK_TO_SEARCH_RESULT;
    }

    return DIR_ACTION_NONE;
}

bool CBSFTContactUIHelper::UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail)
{
    if (NULL == pListItem || pDetail.IsEmpty())
    {
        return false;
    }

    CMenuListItemPtr pMenuItem = CMenuListItem::GetMenuListItem(pListItem);

    if (NULL != pMenuItem)
    {
        yl::string strGroupName = pDetail->GetDisplayName();

        GetGroupDisplayName(nId, strGroupName);

        return UpdateMenuItem(pMenuItem, nId, strGroupName, true);
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBSFTControler = CBSFTContactControler::GetControler(pControler);

    if (NULL == pBSFTControler)
    {
        return false;
    }

    yl::string strShowNumber;

    if (!pBSFTControler->GetContactShowNumber(pDetail, strShowNumber))
    {
        return false;
    }

    return UpdateContactItem(pListItem, nId, pDetail->GetDisplayName(), strShowNumber);
}

void CBSFTContactUIHelper::OnEnterDetail(int nAction)
{
    DIRUI_DEBUG("OnEnterDetail Action:%d", nAction);

    if (DIR_ACTION_COMMON_DETAIL == nAction)
    {
        return;
    }

    if (NULL == m_pDetailLayout)
    {
        m_pDetailLayout = new CBSFTDetailLayout;

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
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL == pBSFTContact)
    {
        return;
    }

    pDetailAdapter->SetDetailLayout(m_pDetailLayout);

    if (m_pDetailLayout->IsCopyLayout())
    {
        pBSFTContact->ResetContactId();

        yl::string strName;
        yl::string strNumber;

        pBSFTContact->GetAttributeByName(0, DIR_ATTR_NAME, strName);
        pBSFTContact->GetAttributeByName(0, DIR_ATTR_NUMBER, strNumber);

        CEditListItemPtr pNameItem = m_pDetailLayout->GetNameEditItem();
        CEditListItemPtr pNumberItem = m_pDetailLayout->GetNumberEditItem();

        if (NULL != pNameItem)
        {
            pNameItem->SetValue(strName);
        }

        if (NULL != pNumberItem)
        {
            pNumberItem->SetValue(strNumber);
        }
    }
    else if (m_pDetailLayout->IsAdditionLayout())
    {
        pBSFTContact->ResetContactId();
    }
    else
    {
        int nContactId = pBSFTContact->GetContactId();
        SmartPtr pDetail = pBSFTContact->GetDetail(nContactId);
        ContactBroadSoftPersonal * pPersonalContact = CBSFTContactControler::GetPersonalContact(pDetail);

        m_pDetailLayout->UpdateContact(nContactId, pPersonalContact);
    }
}

bool CBSFTContactUIHelper::IsBackMenuPageOver(int nPageType, int nGroupID)
{
    if (!BsftDir_UpdateEnable() && kszCustomId == nGroupID)
    {
        return true;
    }

    return false;
}

void CBSFTContactUIHelper::OnEnterPreciseSearch()
{
    if (NULL == m_pSearchLayout)
    {
        m_pSearchLayout = new CBSFTSearchLayout;

        if (NULL == m_pSearchLayout)
        {
            return;
        }
    }

    m_pSearchLayout->Init();

    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
    CPreciseSearchAdapter * pSearchAdapter = CPreciseSearchAdapter::GetPreciseSearchAdapter(pAdapter);

    if (NULL == pSearchAdapter)
    {
        return;
    }

    pSearchAdapter->SetSearchLayout(m_pSearchLayout);

    CDirectoryControlerPtr pControler = pSearchAdapter->GetControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL == pBSFTContact)
    {
        return;
    }

    CComboBoxListItemPtr pGroupItem = m_pSearchLayout->GetGroupItem();

    if (NULL == pGroupItem)
    {
        return;
    }

    // 获取组名列表数据成功, 加入可选择的组
    int nGroupId = pBSFTContact->GetGroupId();
    VEC_CONTACT_ID vecId;

    pBSFTContact->GetAllGroup(vecId);

    YLVector<yl::string> vecGroupName;
    int nSize = vecId.size();
    int nIndex = 0;

    for (int i = 0; i < nSize; ++i)
    {
        int nId = vecId[i];

        if (nId == nGroupId)
        {
            nIndex = i;
        }

        SmartPtr pDetail = pBSFTContact->GetDetail(nId);

        if (pDetail.IsEmpty())
        {
            continue;
        }

        yl::string strGroupName = pDetail->GetDisplayName();

        GetGroupDisplayName(nId, strGroupName);

        pGroupItem->AddValue(strGroupName, nId);
    }

    pGroupItem->SetCurrentIndex(nIndex);
}
