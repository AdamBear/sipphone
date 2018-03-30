#include "localcontactuihelper.h"
#include "localcontactcontroler.h"
#include "localdetaillayout.h"

#include "directorymgr.h"
#include "directoryuicommon.h"
#include "moddirectoryui.h"
#include "detailadapter.h"
#include "dirmsgboxmgr.h"

#include "xwindow/xListView.h"

#include "baseui/include/comboboxlistitem.h"
#include "baseui/include/detaillistitem.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/menulistitem.h"
#include "baseui/include/t2xpicpath.h"
#include "commonunits/commonunits_def.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include <resource/modresource.h>
#if IF_DIR_SUPPORT_FAVORITE_ITEM
#include "favoritecontactlistitem.h"
#endif
#include <directoryui/include/directorydefine.h>

CLocalContactUIHelper::CLocalContactUIHelper()
    : CCommonUIHelper(DIRECTORY_TYPE_LOCAL)
    , m_pDetailLayout(NULL)
{
}

CLocalContactUIHelper::~CLocalContactUIHelper()
{
    ReleaseLayout();
}

void CLocalContactUIHelper::ReleaseLayout()
{
    if (NULL != m_pDetailLayout)
    {
        m_pDetailLayout->UnInit();

        delete m_pDetailLayout;
        m_pDetailLayout = NULL;
    }
}

CLocalContactUIHelper * CLocalContactUIHelper::GetUIHelper(IDirUIHelperPtr & pDirUIHelper)
{
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper || DIRECTORY_TYPE_LOCAL != pUIHelper->GetType())
    {
        return NULL;
    }

    return static_cast<CLocalContactUIHelper *>(pUIHelper);
}

ILocalContactUIHelper * CLocalContactUIHelper::GetLocalContactUIHelper(
    IDirUIHelperPtr & pDirUIHelper)
{
    CLocalContactUIHelper * pLocalUIHelper = CLocalContactUIHelper::GetUIHelper(pDirUIHelper);

    return (ILocalContactUIHelper *)pLocalUIHelper;
}

bool CLocalContactUIHelper::GetGroupDisplayName(int nGroupId, yl::string & strGroupName)
{
    if (kszBlackListGroupId == nGroupId)
    {
        strGroupName = _LANG(TRID_BLACKLIST);

        return true;
    }
    else if (kszAllContactId == nGroupId)
    {
        strGroupName = _LANG(TRID_ALL_CONTACTS);

        return true;
    }

    return false;
}

void CLocalContactUIHelper::UnInit()
{
    ReleaseLayout();

    CCommonUIHelper::UnInit();
}

bool CLocalContactUIHelper::GetLocalGroupData(ContactGroupData & groupData)
{
    if (NULL == m_pDetailLayout)
    {
        DIRUI_ERR("m_pDetailLayout is null when GetLocalGroupData");
        return false;
    }

    m_pDetailLayout->ReturnAllEdit();

    CEditListItemPtr pNameItem = m_pDetailLayout->GetNameEditItem();

    if (NULL == pNameItem)
    {
        DIRUI_ERR("GetNameEditItem() is null when GetLocalGroupData");
        return false;
    }

    groupData.m_strDisplayName = pNameItem->GetValue().trim_both();

    CComboBoxListItemPtr pRingItem = m_pDetailLayout->GetRingComboBoxItem();

    if (NULL != pRingItem)
    {
        groupData.m_strRing = pRingItem->GetItemStringData();
    }

    return true;
}

bool CLocalContactUIHelper::GetLocalContactData(int & nGroupId, ContactGeneralItemData & itemData)
{
    if (NULL == m_pDetailLayout)
    {
        DIRUI_ERR("m_pDetailLayout is null when GetLocalContactData");
        return false;
    }

    m_pDetailLayout->ReturnAllEdit();

    CEditListItemPtr pNameItem = m_pDetailLayout->GetNameEditItem();

    if (NULL == pNameItem)
    {
        DIRUI_ERR("GetNameEditItem() is null when GetLocalContactData");
        return false;
    }

    itemData.m_strDisplayName = pNameItem->GetValue().trim_both();

    if (m_pDetailLayout->IsCopyLayout())
    {
        CComboBoxListItemPtr pOfficeNoItem = m_pDetailLayout->GetOfficeNoComboBoxItem();

        if (NULL != pOfficeNoItem)
        {
            itemData.m_strOfficePhoneNumber = pOfficeNoItem->GetItemStringData();
        }

        CComboBoxListItemPtr pMobileNoItem = m_pDetailLayout->GetMobileNoComboBoxItem();

        if (NULL != pMobileNoItem)
        {
            itemData.m_strMobilePhoneNumber = pMobileNoItem->GetItemStringData();
        }

        CComboBoxListItemPtr pOtherNoItem = m_pDetailLayout->GetOtherNoComboBoxItem();

        if (NULL != pOtherNoItem)
        {
            itemData.m_strOtherPhoneNumber = pOtherNoItem->GetItemStringData();
        }
    }
    else
    {
        CEditListItemPtr pOfficeNoItem = m_pDetailLayout->GetOfficeNoEditItem();

        if (NULL != pOfficeNoItem)
        {
            itemData.m_strOfficePhoneNumber = pOfficeNoItem->GetValue().trim_both();
        }

        CEditListItemPtr pMobileNoItem = m_pDetailLayout->GetMobileNoEditItem();

        if (NULL != pMobileNoItem)
        {
            itemData.m_strMobilePhoneNumber = pMobileNoItem->GetValue().trim_both();
        }

        CEditListItemPtr pOtherNoItem = m_pDetailLayout->GetOtherNoEditItem();

        if (NULL != pOtherNoItem)
        {
            itemData.m_strOtherPhoneNumber = pOtherNoItem->GetValue().trim_both();
        }
    }

    CComboBoxListItemPtr pAccountItem = m_pDetailLayout->GetAccountComboBoxItem();

    if (NULL != pAccountItem)
    {
        itemData.m_nLine = pAccountItem->GetItemData();
    }
    else
    {
        itemData.m_nLine = AUTO_ACCOUNT_IDENTIFY;
    }

    if (kszBlackListGroupId == nGroupId)
    {
        return true;
    }

    CComboBoxListItemPtr pGroupItem = m_pDetailLayout->GetGroupComboBoxItem();

    if (NULL != pGroupItem)
    {
        nGroupId = pGroupItem->GetItemData();
    }

    CComboBoxListItemPtr pRingItem = m_pDetailLayout->GetRingComboBoxItem();

    if (NULL != pRingItem)
    {
        itemData.m_strRing = pRingItem->GetItemStringData();
    }

#ifdef PHOTO_SHOW
    CDetailListItemPtr pPhotoItem = m_pDetailLayout->GetPhotoListItem();

    if (NULL != pPhotoItem)
    {
        itemData.m_strDefaultPhoto = pPhotoItem->GetItemStringData();
    }
#endif

    CEditListItemPtr pAutoDivertItem = m_pDetailLayout->GetAutoDivertEditItem();
    if (NULL != pAutoDivertItem)
    {
        itemData.m_strAutoDivert = pAutoDivertItem->GetValue();
    }

#if IF_FEATURE_FAVORITE_DIR
    if (LocalDir_IsFavoriteEnable())
    {
        CEditListItemPtr pFavoriteItem = m_pDetailLayout->GetFavoriteIndexEditItem();
        if (NULL != pFavoriteItem)
        {
            itemData.m_nFavoriteIndex = atoi(pFavoriteItem->GetValue().c_str());
        }
    }
#endif

    return true;
}

#if IF_FEATURE_FAVORITE_DIR
bool CLocalContactUIHelper::GetLocalContactFavorite(yl::string &strFavoriteIndex)
{
    if (NULL == m_pDetailLayout || !LocalDir_IsFavoriteEnable())
    {
        return false;
    }

    CEditListItemPtr pFavoriteItem = m_pDetailLayout->GetFavoriteIndexEditItem();
    if (NULL != pFavoriteItem)
    {
        strFavoriteIndex = pFavoriteItem->GetValue();
        return true;
    }
    return false;
}

void CLocalContactUIHelper::PopSureEditMsgBox(const char * pszTitle, const char * pszMsg, int nAction, int nId)
{
    g_DirMsgBoxMgr.PopSureEditMsgBox(pszTitle, pszMsg, nAction, nId);
}
#endif

void CLocalContactUIHelper::SetInputFocus(const char * pszAttr)
{
    if (NULL == m_pListView || NULL == m_pDetailLayout || NULL == pszAttr)
    {
        return;
    }

    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
    CDetailAdapter* pDetailAdapter = CDetailAdapter::GetDetailAdapter(pAdapter);
    if (NULL == pDetailAdapter)
    {
        return;
    }

    int nFocusDataIndex = 0;

    if (0 == strcmp(pszAttr, DIR_ATTR_NAME))
    {
        nFocusDataIndex = LOCAL_CONTACT_DETAIL_ITEM_INDEX_NAME;

    }
    else if (0 == strcmp(pszAttr, DIR_ATTR_OFFICENUMBER))
    {
        nFocusDataIndex = LOCAL_CONTACT_DETAIL_ITEM_INDEX_OFFICE_NO;
    }
    else  if (0 == strcmp(pszAttr, DIR_ATTR_MOBILENUMBER))
    {
        nFocusDataIndex = LOCAL_CONTACT_DETAIL_ITEM_INDEX_MOBILE_NO;
    }
    else if (0 == strcmp(pszAttr, DIR_ATTR_OTHERNUMBER))
    {
        nFocusDataIndex = LOCAL_CONTACT_DETAIL_ITEM_INDEX_OTHER_NO;
    }
#if IF_FEATURE_FAVORITE_DIR
    else if (0 == strcmp(pszAttr, DIR_ATTR_FAVORITE_INDEX))
    {
        nFocusDataIndex = LOCAL_CONTACT_DETAIL_ITEM_INDEX_FAVORITE_INDEX;
    }
#endif

    if (nFocusDataIndex >= m_pListView->GetTotalDataCount())
    {
        nFocusDataIndex = 0;
    }

    xListViewItemPtr pItem = m_pListView->GetItemByDataIndex(nFocusDataIndex);
    if (NULL != pItem)
    {
        pItem->FocusItem(true);
    }
    else
    {
        pDetailAdapter->SetPageDataIndex(nFocusDataIndex);
        pDetailAdapter->SetFocusIndex(nFocusDataIndex);
        pDetailAdapter->RefreshListView();
    }
}

int CLocalContactUIHelper::GetBrowsePageType(int nGroupId)
{
    if (kszBlackListGroupId == nGroupId)
    {
        return BROWSER_PAGE_TYPE_NORMAL;
    }

#if IF_FEATURE_FAVORITE_DIR
    if (kszFavoriteGroupId == nGroupId)
    {
        return BROWSER_PAGE_TYPE_NORMAL;
    }
#endif

    return CCommonUIHelper::GetBrowsePageType(nGroupId);
}

int CLocalContactUIHelper::GetListItemType(int nState, int nDataIndex)
{
#if IF_DIR_SUPPORT_FAVORITE_ITEM
    if (DIR_UI_STATE_BROWSER == nState)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);
        if (NULL != pLocalContact && kszFavoriteGroupId == pLocalContact->GetGroupId())
        {
            return CFavoriteContactListItem::GetFavoriteContactListItemType();
        }
    }
#endif

    return CCommonUIHelper::GetListItemType(nState, nDataIndex);
}

#if IF_DIR_SUPPORT_FAVORITE_ITEM
int CLocalContactUIHelper::GetLisItemId(xListViewItem * pItem)
{
    CFavoriteContactListItemPtr pFavoriteItem = CFavoriteContactListItem::GetFavoriteContactListItem(pItem);
    if (NULL != pFavoriteItem)
    {
        return pFavoriteItem->GetId();
    }

    return CCommonUIHelper::GetLisItemId(pItem);
}
#endif

bool CLocalContactUIHelper::GetOption(int nUIState, VEC_OPTION_ITEM_DATA & vecOption)
{
    // 本地联系人详情的Softkey刷新会走到这个流程
    if (DIR_UI_STATE_DETAIL == nUIState)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

    if (NULL == pLocalContact || NULL == m_pListView)
    {
        return false;
    }

    xListViewItemPtr pFocusItem = m_pListView->GetFocusItem();
    int nFocusId = GetLisItemId(pFocusItem);
    int nGroupId = pLocalContact->GetGroupId();

    // 群组
    if (GetGroupOption(nUIState, nGroupId, nFocusId, vecOption))
    {
        return true;
    }

    ST_OPTION_ITEM_DATA stOption;

    PushDetailOption(stOption, vecOption);

#if IF_FEATURE_FAVORITE_DIR
    if (kszFavoriteGroupId == nGroupId)
    {
        if (CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, 0) && LocalDir_IsFavoriteEnable())
        {
            stOption.m_nAction = ST_REMOVE_FROM_FAVORITE;
            stOption.m_strTitle = TRID_REMOVE_FROM_FAVORITE;
            vecOption.push_back(stOption);
        }
        return true;
    }
#endif

    stOption.m_nAction = ST_DELETE;
    stOption.m_strTitle = _LANG(TRID_DELETE);
    vecOption.push_back(stOption);

    if (DIR_UI_STATE_BROWSER == nUIState)
    {
        stOption.m_nAction = ST_DELETEALL;
        stOption.m_strTitle = _LANG(TRID_DELETE_ALL);
        vecOption.push_back(stOption);
    }

    if (kszBlackListGroupId == nGroupId)
    {
        if (CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, 0))
        {
            stOption.m_nAction = ST_MOVE_CONTACT;
            stOption.m_strTitle = _LANG(TRID_ADD_TO_CONTACT);
            vecOption.push_back(stOption);
        }
    }
    else
    {
        if (CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId))
        {
            stOption.m_nAction = ST_MOVE_TO_BLACKLIST;
            stOption.m_strTitle = _LANG(TRID_ADD_TO_BLACK_LIST);
            vecOption.push_back(stOption);
        }

#if IF_FEATURE_FAVORITE_DIR
        if (!LocalDir_IsFavoriteEnable())
        {
            return true;
        }

        if (!LocalDir_IsContactInFavorite(nFocusId))
        {
            stOption.m_nAction = ST_ADD_TO_FAVORITE;
            stOption.m_strTitle = TRID_ADD_TO_FAVORITE;
            vecOption.push_back(stOption);
        }
        else
        {
            stOption.m_nAction = ST_REMOVE_FROM_FAVORITE;
            stOption.m_strTitle = TRID_REMOVE_FROM_FAVORITE;
            vecOption.push_back(stOption);
        }
#endif
    }

    return true;
}

bool CLocalContactUIHelper::GetGroupOption(int nUIState, int nCurGroupId, int nFocusId,
        VEC_OPTION_ITEM_DATA & vecOption)
{
    if (DIR_UI_STATE_BROWSER != nUIState || knInvalidRId != nCurGroupId)
    {
        return false;
    }

    if (knInvalidRId == nFocusId)
    {
        return true;
    }

    ST_OPTION_ITEM_DATA stOption;

    PushDetailOption(stOption, vecOption);

    stOption.m_nAction = ST_DELETE_GROUP;
    stOption.m_strTitle = _LANG(TRID_DELETE);
    vecOption.push_back(stOption);

    stOption.m_nAction = ST_DELETE_ALL_GROUP;
    stOption.m_strTitle = _LANG(TRID_DELETE_ALL);
    vecOption.push_back(stOption);

    return true;
}

void CLocalContactUIHelper::GetMenuTitle(yl::string & strTitle)
{
    strTitle = _LANG(TRID_LOCAL_DIRECTORY);
}

bool CLocalContactUIHelper::GetNormalTitle(yl::string & strTitle, int nGroupId/* = knInvalidRId*/)
{
    if (kszBlackListGroupId == nGroupId || kszAllContactId == nGroupId)
    {
        return GetGroupDisplayName(nGroupId, strTitle);
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

    if (NULL == pLocalContact)
    {
        return false;
    }

    if (knInvalidRId == nGroupId)
    {
        nGroupId = pLocalContact->GetGroupId();
    }

    yl::string strGroupName = strTitle;
    bool bResult = pLocalContact->GetGroupName(nGroupId, strGroupName);
    strTitle = _LANG(strGroupName);
    return bResult;
}

bool CLocalContactUIHelper::GetDetailTitle(yl::string & strTitle)
{
    if (CCommonUIHelper::GetDetailTitle(strTitle))
    {
        return true;
    }

    if (NULL != m_pDetailLayout)
    {
        int nAction = m_pDetailLayout->GetAction();

        switch (nAction)
        {
        case DIR_ACTION_LOCAL_ADD_GROUP:
            {
                strTitle = _LANG(TRID_ADD_GROUP);
            }
            break;
        case DIR_ACTION_LOCAL_ADD_CONTACT:
            {
                CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
                int nGroupId = kszAllContactId;

                if (!pControler.IsEmpty())
                {
                    nGroupId = pControler->GetGroupId();
                }

                if (kszBlackListGroupId == nGroupId)
                {
                    strTitle = _LANG(TRID_ADD_BLACKLIST_CONTACT);
                }
                else
                {
                    strTitle = _LANG(TRID_ADD_CONTACT);
                }
            }
            break;
        case DIR_ACTION_LOCAL_COPY_TO_CONTACTS:
            {
                strTitle = _LANG(TRID_ADD_TO_CONTACT);
            }
            break;
        case DIR_ACTION_LOCAL_COPY_TO_BLACKLIST:
            {
                strTitle = _LANG(TRID_ADD_TO_BLACK_LIST);
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

bool CLocalContactUIHelper::GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                                       CArraySoftKeyBarData & vecSoftKey)
{
    if (vecSoftKey.size() < SOFTKEY_NUM)
    {
        return false;
    }

    // 详情和添加界面
    if (GetDetailSoftKey(pFocusItem, nUIState, vecSoftKey))
    {
        return true;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

    if (NULL == pLocalContact)
    {
        return false;
    }

    int nGroupId = pLocalContact->GetGroupId();
    bool bEditContact = CDirectoryControler::IsDirCanEdit(DIRECTORY_TYPE_LOCAL, nGroupId);
    int nFocusId = GetLisItemId(pFocusItem);

    if (DIR_UI_STATE_BROWSER == nUIState
            && GetBrowseSoftKey(nGroupId, nFocusId, bEditContact, vecSoftKey))
    {
        return true;
    }

    // 焦点在联系人上（包括搜索界面）
    if (bEditContact
#if IF_FEATURE_FAVORITE_DIR
            && kszFavoriteGroupId != nGroupId
#endif
       )
    {
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_ADD);
        vecSoftKey[1].m_iType = ST_ADD;
    }

    GetContactSoftKey(pFocusItem, nUIState, nGroupId, vecSoftKey);

    return true;
}

bool CLocalContactUIHelper::GetDetailSoftKey(xListViewItem * pFocusItem, int nUIState,
        CArraySoftKeyBarData & vecSoftKey)
{
    if (DIR_UI_STATE_DETAIL != nUIState && DIR_UI_STATE_ADDITION != nUIState)
    {
        return false;
    }

#ifdef PHOTO_SHOW
    if (NULL != m_pDetailLayout && NULL != pFocusItem
            && pFocusItem == m_pDetailLayout->GetPhotoListItem())
    {
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_ENTER);
        vecSoftKey[2].m_iType = ST_ENTER;
    }
#endif

#if IF_DIR_SHOW_ADD_WHEN_ADD
    if (NULL != m_pDetailLayout && m_pDetailLayout->IsAdditionLayout())
    {
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ADD);
    }
    else
#endif
    {
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SAVE);
    }

    vecSoftKey[3].m_iType = ST_SAVE;

    return true;
}

bool CLocalContactUIHelper::GetBrowseSoftKey(int nCurGroupId, int nFocusId, bool bEdit,
        CArraySoftKeyBarData & vecSoftKey)
{
    // 群组界面
    if (knInvalidRId == nCurGroupId)
    {
        if (bEdit)
        {
            //http://10.2.1.199/Bug.php?BugID=60380
            vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_ADD_GROUP_SOFTKEY);
            vecSoftKey[1].m_iType = ST_ADDGROUP;

            if (kszAllContactId != nFocusId)
            {
                vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_OPTION);
                vecSoftKey[2].m_iType = ST_OPTION;
            }
            else
            {
                vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
                vecSoftKey[2].m_iType = ST_SEARCH;
            }

            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ENTER);
            vecSoftKey[3].m_iType = ST_ENTER;
        }

        return true;
    }

    return false;
}

int CLocalContactUIHelper::GetActionByKeyPress(int nUIState, int nKey)
{
    if (DIR_UI_STATE_DETAIL == nUIState && PHONE_KEY_OK == nKey && NULL != m_pDetailLayout)
    {
        return m_pDetailLayout->GetAction();
    }

    return CCommonUIHelper::GetActionByKeyPress(nUIState, nKey);
}

int CLocalContactUIHelper::GetActionByUIAction(int nUIState, int nUIAction)
{
    if (ST_ADDGROUP == nUIAction)
    {
        return DIR_ACTION_LOCAL_ENTER_ADD_GROUP;
    }
    else if (ST_DELETE_GROUP == nUIAction)
    {
        return DIR_ACTION_LOCAL_DEL_GROUP;
    }
    else if (ST_DELETE_ALL_GROUP == nUIAction)
    {
        return DIR_ACTION_LOCAL_DEL_ALL_GROUP;
    }
    else if (ST_ADD == nUIAction)
    {
        return DIR_ACTION_LOCAL_ENTER_ADD_CONTACT;
    }
    else if (ST_DELETE == nUIAction)
    {
        return DIR_ACTION_LOCAL_DEL_CONTACT;
    }
    else if (ST_DELETEALL == nUIAction)
    {
        return DIR_ACTION_LOCAL_DEL_ALL_CONTACT;
    }
    else if (ST_SAVE == nUIAction)
    {
        if (NULL != m_pDetailLayout)
        {
            return m_pDetailLayout->GetAction();
        }
    }
    else if (ST_MOVE_CONTACT == nUIAction)
    {
        return DIR_ACTION_LOCAL_MOVE_TO_CONTACTS;
    }
    else if (ST_MOVE_TO_BLACKLIST == nUIAction)
    {
        return DIR_ACTION_LOCAL_MOVE_TO_BLACKLIST;
    }
    else if (ST_ENTER == nUIAction && DIR_UI_STATE_DETAIL == nUIState)
    {
        return DIR_ACTION_LOCAL_ENTER_CHOOSE_PHOTO;
    }
#if IF_FEATURE_FAVORITE_DIR
    else if (ST_ADD_TO_FAVORITE == nUIAction)
    {
        return DIR_ACTION_LOCAL_COPY_TO_FAVORITE;
    }
    else if (ST_REMOVE_FROM_FAVORITE == nUIAction)
    {
        return DIR_ACTION_LOCAL_REMOVE_FROM_FAVORITE;
    }
#endif

    return CCommonUIHelper::GetActionByUIAction(nUIState, nUIAction);
}

bool CLocalContactUIHelper::UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail)
{
    if (NULL == pListItem || pDetail.IsEmpty())
    {
        return false;
    }

    CMenuListItem * pMenuItem = CMenuListItem::GetMenuListItem(pListItem);

    if (NULL != pMenuItem)
    {
        UpdateMenuItem(pMenuItem, nId, pDetail->GetDisplayName(), true);

        return true;
    }

    LocalDirContactRecord * pContact = CLocalContactControler::GetLocalContact(pDetail);

    if (NULL == pContact)
    {
        DIRUI_ERR("UpdateListItem fail Id:%d", nId);
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CLocalContactControler * pLocalControll = CLocalContactControler::GetControler(pControler);
    if (NULL == pLocalControll)
    {
        return false;
    }

    yl::string strNumber = "";
    if (pLocalControll->IsSearchState())
    {
        strNumber = pContact->GetSearchShowNumber(pLocalControll->GetSearchKey());
    }
    else
    {
        if (!pContact->GetNumberByName(kszOfficeNumber, strNumber) || strNumber.empty())
        {
            if (!pContact->GetNumberByName(kszMobileNumber, strNumber) || strNumber.empty())
            {
                pContact->GetNumberByName(kszOtherNumber, strNumber);
            }
        }
    }

#if IF_DIR_SUPPORT_FAVORITE_ITEM
    CDirectoryControlerPtr pControl = g_DirectoryMgr.GetActiveControler();
    CLocalContactControler* pLocalControl = CLocalContactControler::GetControler(pControl);
    if (NULL == pLocalControl)
    {
        return false;
    }
    int nGroupId = pLocalControl->GetGroupId();
    if (kszFavoriteGroupId == nGroupId && DIR_UI_STATE_BROWSER == pLocalControl->GetState())
    {
        return UpdateFavoriteContactItem(pListItem, nId, pContact->GetDisplayName(), strNumber, pContact->m_nFavoriteIndex);
    }
#endif
    yl::string strPhoto = PIC_CONTACT_NORMAL;
#if IS_COLOR
    if (LocalDir_IsContactInFavorite(nId) && LocalDir_IsFavoriteEnable())
    {
        strPhoto = PIC_CONTACT_FAVORITE;
    }
#endif

    return UpdateContactItem(pListItem, nId, pContact->GetDisplayName(), strNumber, strPhoto);
}

#ifdef PHOTO_SHOW
void CLocalContactUIHelper::GetDetailLayoutPhoto(yl::string & strPhoto)
{
    if (NULL == m_pDetailLayout)
    {
        return;
    }

    CDetailListItemPtr pPhotoListItem = m_pDetailLayout->GetPhotoListItem();

    if (NULL == pPhotoListItem)
    {
        return;
    }

    strPhoto = pPhotoListItem->GetValue();
}

void CLocalContactUIHelper::SetDetailLayoutPhoto(yl::string & strPhoto)
{
    if (NULL == m_pDetailLayout)
    {
        return;
    }

    m_pDetailLayout->SetPhoto(strPhoto);
}
#endif

void CLocalContactUIHelper::OnEnterDetail(int nAction)
{
    DIRUI_INFO("OnEnterDetail Action:%d", nAction);

    if (NULL == m_pDetailLayout)
    {
        m_pDetailLayout = new CLocalDetailLayout();

        if (NULL == m_pDetailLayout)
        {
            return;
        }
    }

    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
    CDetailAdapter * pDetailAdapter = CDetailAdapter::GetDetailAdapter(pAdapter);

    if (NULL == pDetailAdapter)
    {
        return;
    }

    CDirectoryControlerPtr pControler = pDetailAdapter->GetControler();
    CLocalContactControler * pLocalContact = CLocalContactControler::GetControler(pControler);

    if (NULL == pLocalContact)
    {
        return;
    }

    int nGroupId = pLocalContact->GetGroupId();

    m_pDetailLayout->SetAction(nAction);
    m_pDetailLayout->Init(nGroupId);

    pDetailAdapter->SetDetailLayout(m_pDetailLayout);

    if (m_pDetailLayout->IsCopyLayout())
    {
        pLocalContact->ResetContactId();

        int nSrcDirType = pLocalContact->GetAttributeType();
        yl::string strName;

        pLocalContact->GetAttributeByName(0, DIR_ATTR_NAME, strName);

        int nNumberCount = pLocalContact->GetAttributeCount(DIR_ATTR_NUMBER);
        VEC_STRING vecNumber;

        for (int i = 0; i < nNumberCount; ++i)
        {
            yl::string strNumber;

            pLocalContact->GetAttributeByName(i, DIR_ATTR_NUMBER, strNumber);

            if (!strNumber.empty())
            {
                vecNumber.push_back(strNumber);
            }
        }

        InitCopyLayout(nSrcDirType, strName, vecNumber);
    }
    else if (m_pDetailLayout->IsAdditionLayout())
    {
        pLocalContact->ResetContactId();
#if IF_FEATURE_FAVORITE_DIR
        if (m_pDetailLayout->IsDsskeyAddLayout())
        {
            m_pDetailLayout->UpdateFavoriteIndex();
        }
#endif
    }
    else
    {
        int nContactId = pLocalContact->GetContactId();
        SmartPtr pDetail = pLocalContact->GetDetail(nContactId);

        if (m_pDetailLayout->IsContactLayout())
        {
            LocalDirContactRecord * pContact = CLocalContactControler::GetLocalContact(pDetail);

            m_pDetailLayout->UpdateContact(nContactId, pContact);

            if (NULL != pContact)
            {
                nGroupId = pContact->m_nParentId;
            }
        }
        else
        {
            DirGroupRecord * pGroup = CCommonDirCtrl::GetGroupRecord(pDetail);

            m_pDetailLayout->UpdateGroup(nContactId, pGroup);
        }
    }

    if (kszBlackListGroupId == nGroupId)
    {
        return;
    }

    CComboBoxListItemPtr pGroupComboBoxItem = m_pDetailLayout->GetGroupComboBoxItem();

    if (NULL == pGroupComboBoxItem)
    {
        return;
    }

    // 初始化普通群组
    VEC_CONTACT_ID vecGroupId;
    yl::string strGroupName;

    pLocalContact->GetMenuGroupId(vecGroupId);

    int nSize = vecGroupId.size();

    for (int i = 0; i < nSize; ++i)
    {
        int nGroupId = vecGroupId[i];

        if (!GetGroupDisplayName(nGroupId, strGroupName)
                && !pLocalContact->GetGroupName(nGroupId, strGroupName))
        {
            continue;
        }

        pGroupComboBoxItem->AddValue(strGroupName, nGroupId);
    }

    pGroupComboBoxItem->SetItemData(nGroupId);
}

void CLocalContactUIHelper::InitCopyLayout(int nSrcDirType, const yl::string & strName,
        VEC_STRING & vecNumber)
{
    if (NULL == m_pDetailLayout)
    {
        return;
    }

    int nNumberCount = vecNumber.size();

    DIRUI_INFO("InitCopyLayout Name:%s NumberCount:%d", strName.c_str(), nNumberCount);

    CEditListItemPtr pNameItem = m_pDetailLayout->GetNameEditItem();

    if (NULL != pNameItem)
    {
        // 兼容ldap名字为空的情况
        if (DIRECTORY_TYPE_LDAP == nSrcDirType && strName.empty())
        {
            yl::string strNameUnknown = _TRAN(_LANG(TRID_NAME_UNKNOWN));

            pNameItem->SetValue(strNameUnknown);
        }
        else if (CALL_LOG_TYPE_LOCAL == nSrcDirType)
        {
            yl::string strTransName;

            TransBsftFACText(strName, strTransName);

            strTransName = _TRAN(strTransName);
            pNameItem->SetValue(strTransName);
        }
        else
        {
            pNameItem->SetValue(strName);
        }
    }

    // 本地联系人复制界面赋值
    CComboBoxListItemPtr pOfficeNoItem = m_pDetailLayout->GetOfficeNoComboBoxItem();
    CComboBoxListItemPtr pMobileNoItem = m_pDetailLayout->GetMobileNoComboBoxItem();
    CComboBoxListItemPtr pOtherNoItem = m_pDetailLayout->GetOtherNoComboBoxItem();

    if (NULL == pOfficeNoItem || NULL == pMobileNoItem || NULL == pOtherNoItem)
    {
        return;
    }

    if (0 == nNumberCount)
    {
        pOfficeNoItem->AddValue(_LANG(TRID_OLD_NUMBER), "");
        pMobileNoItem->AddValue(_LANG(TRID_OLD_NUMBER), "");
        pOtherNoItem->AddValue(_LANG(TRID_OLD_NUMBER), "");
        return;
    }

    pOfficeNoItem->ClearValue();
    pMobileNoItem->ClearValue();
    pOtherNoItem->ClearValue();

    pOfficeNoItem->AddValue(_LANG(TRID_OLD_NUMBER), "");
    pMobileNoItem->AddValue(_LANG(TRID_OLD_NUMBER), "");
    pOtherNoItem->AddValue(_LANG(TRID_OLD_NUMBER), "");

    for (int i = 0; i < nNumberCount; ++i)
    {
        yl::string & strNumber = vecNumber[i];

        pOfficeNoItem->AddValue(strNumber, strNumber);
        pMobileNoItem->AddValue(strNumber, strNumber);
        pOtherNoItem->AddValue(strNumber, strNumber);
    }

    int nOfficeIndex = nNumberCount > 0 ? 1 : 0;
    pOfficeNoItem->SetCurrentIndex(nOfficeIndex);

    int nMobileIndex = nNumberCount > 1 ? 2 : 0;
    pMobileNoItem->SetCurrentIndex(nMobileIndex);

    int nOtherIndex = nNumberCount > 2 ? 3 : 0;
    pOtherNoItem->SetCurrentIndex(nOtherIndex);
}

#if IF_DIR_SUPPORT_FAVORITE_ITEM
bool CLocalContactUIHelper::UpdateFavoriteContactItem(xListViewItem * pItem, int nId,
        const yl::string & strName, const yl::string & strNumber, int nFavoriteIndex)
{
    CFavoriteContactListItem* pFavoriteItem = CFavoriteContactListItem::GetFavoriteContactListItem(pItem);

    if (NULL == pFavoriteItem)
    {
        return false;
    }

    pFavoriteItem->SetId(nId);
    pFavoriteItem->SetName(strName);

    if (strNumber.empty())
    {
        pFavoriteItem->SetInfo(_LANG(TRID_NONE));
    }
    else
    {
        pFavoriteItem->SetInfo(strNumber);
    }

    yl::string strFavorite = commonAPI_FormatString("%d", nFavoriteIndex);
    pFavoriteItem->SetFavoriteIndex(strFavorite);

    return true;
}

yl::string CLocalContactUIHelper::GetFocusContactNumber()
{
    if (NULL == m_pListView)
    {
        return "";
    }

    xListViewItemPtr pItem = m_pListView->GetFocusItem();
    CFavoriteContactListItem* pContactItem = CFavoriteContactListItem::GetFavoriteContactListItem(pItem);
    if (NULL != pContactItem)
    {
        return pContactItem->GetInfo();
    }

    return CCommonUIHelper::GetFocusContactNumber();
}
#endif
