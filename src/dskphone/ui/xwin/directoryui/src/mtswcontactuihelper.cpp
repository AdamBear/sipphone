#if IF_FEATURE_METASWITCH_DIRECTORY
#include "mtswcontactuihelper.h"
#include "mtswcontactcontroler.h"
#include "dirctrlfactory.h"
#include "directorymgr.h"
#include "moddirectoryui.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "uimanager/moduimanager.h"
#include "diruihelper.h"
#include "directorycommon.h"
#include "detailadapter.h"
#include "baseui/include/menulistitem.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/comboboxlistitem.h"
#include "iconmenulistitem.h"
#include "xwindow/xListView.h"
#include "browseadapter.h"
#include "settingui/include/modsettingui.h"

namespace
{
const char* const   kszPageMTSWCommportal = "mtswcommportal";
}

CMTSWContactUIHelper::CMTSWContactUIHelper()
    : CCommonUIHelper(DIRECTORY_TYPE_METASWICTH)
    , m_pDetailLayout(NULL)
{
}

CMTSWContactUIHelper::~CMTSWContactUIHelper()
{
}

CMTSWContactUIHelper * CMTSWContactUIHelper::GetUIHelper(IDirUIHelperPtr& pDirUIHelper)
{
    CDirUIHelper * pUIHelper = CDirUIHelper::GetDirUIHelper(pDirUIHelper);

    if (NULL == pUIHelper || DIRECTORY_TYPE_METASWICTH != pUIHelper->GetType())
    {
        return NULL;
    }

    return static_cast<CMTSWContactUIHelper *>(pUIHelper);
}

IMTSWUIHelper* CMTSWContactUIHelper::GetMTSWUIHelper(IDirUIHelperPtr & pDirUIHelper)
{
    CMTSWContactUIHelper * pMTSWUIHelper = CMTSWContactUIHelper::GetUIHelper(pDirUIHelper);

    return (IMTSWUIHelper *)pMTSWUIHelper;
}

void CMTSWContactUIHelper::OnLoginResultCallBack(bool bLoginSucceed)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CMTSWContactControler * pMTSWContact = CMTSWContactControler::GetControler(pControler);
    if (NULL == pMTSWContact)
    {
        return;
    }

    if (bLoginSucceed)
    {
        pMTSWContact->OnLoginSuccess();
    }
}

void CMTSWContactUIHelper::EnterMTSWLoginPage()
{
    if (NULL != m_pListView)
    {
        m_pListView->ClearAllItem();
    }

    SettingUI_EnterPage(kszPageMTSWCommportal, (void*)CMTSWContactUIHelper::OnLoginResultCallBack);
}

int CMTSWContactUIHelper::GetLisItemId(xListViewItem * pItem)
{
    CIconMenuListItem* pIconMenuItem = CIconMenuListItem::GetIconMenuListItem(pItem);
    if (NULL != pIconMenuItem)
    {
        return pIconMenuItem->GetId();
    }

    return CCommonUIHelper::GetLisItemId(pItem);
}

int CMTSWContactUIHelper::GetListItemType(int nState, int nDataIndex)
{
    if (DIR_UI_STATE_BROWSER == nState)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        CMTSWContactControler * pMTSWContact = CMTSWContactControler::GetControler(pControler);
        if (NULL != pMTSWContact && kszContactList == pMTSWContact->GetGroupId())
        {
            return CIconMenuListItem::GetIconMenuItemType();
        }
    }

    return CCommonUIHelper::GetListItemType(nState, nDataIndex);
}

void CMTSWContactUIHelper::OnEnterDetail(int nAction)
{
    DIRUI_DEBUG("OnEnterDetail Action:%d", nAction);

    if (DIR_ACTION_COMMON_DETAIL == nAction)
    {
        return;
    }

    if (NULL == m_pDetailLayout)
    {
        m_pDetailLayout = new CMTSWContactLayout;

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
    CMTSWContactControler* pMTSWContact = CMTSWContactControler::GetControler(pControler);

    if (NULL == pMTSWContact)
    {
        return;
    }

    pDetailAdapter->SetDetailLayout(m_pDetailLayout);

    if (m_pDetailLayout->IsCopyLayout())
    {
        pMTSWContact->ResetContactId();

        InitCopyLayout();
    }
    else if (m_pDetailLayout->IsAddLayout())
    {
        pMTSWContact->ResetContactId();
    }
    else
    {
        int nContactId = pMTSWContact->GetContactId();
        SmartPtr pDetail = pMTSWContact->GetDetail(nContactId);
        ContactMetaSwitchContactList* pMWSTRecord = CMTSWContactControler::GetMetaSwitchContact(pDetail);
        if (NULL != pMWSTRecord)
        {
            m_pDetailLayout->UpdateContact(nContactId, pMWSTRecord);
        }
    }
}

void CMTSWContactUIHelper::InitCopyLayout()
{
    if (NULL == m_pDetailLayout)
    {
        return;
    }

    yl::string strFirstName =  "";
    yl::string strLastName = "";
    yl::string strNumber1 = "";
    yl::string strNumber2 = "";

    CDirectoryControler::GetAttributeByName(0, DIR_ATTR_MTSW_FIRST_NAME, strFirstName);
    CDirectoryControler::GetAttributeByName(0, DIR_ATTR_MTSW_LAST_NAME, strLastName);
    CDirectoryControler::GetAttributeByName(0, DIR_ATTR_MTSW_HOMENUMBER, strNumber1);
    CDirectoryControler::GetAttributeByName(1, DIR_ATTR_MTSW_WORKNUMBER, strNumber2);

    DIRUI_INFO("InitCopyLayout FirstName:%s LastName:%s Number1:%s Number2:%s",
               strFirstName.c_str(), strLastName.c_str(), strNumber1.c_str(), strNumber2.c_str());

    CEditListItemPtr pFirstNameItem = m_pDetailLayout->GetFirstNameItem();
    if (NULL != pFirstNameItem)
    {
        pFirstNameItem->SetValue(strFirstName);
    }

    CEditListItemPtr pLastNameItem = m_pDetailLayout->GetLastNameItem();
    if (NULL != pLastNameItem)
    {
        pLastNameItem->SetValue(strLastName);
    }

    // 本地联系人复制界面赋值
    CComboBoxListItemPtr pHomeNoItem = m_pDetailLayout->GetHomeNumberComboBoxItem();
    CComboBoxListItemPtr pWorkNoItem = m_pDetailLayout->GetWorkNumberComboBoxItem();
    CComboBoxListItemPtr pMobileNoItem = m_pDetailLayout->GetMobileComboBoxItem();

    if (NULL == pHomeNoItem || NULL == pWorkNoItem || NULL == pMobileNoItem)
    {
        return;
    }

    if (strNumber1.empty() && strNumber2.empty())
    {
        pHomeNoItem->AddValue(_LANG(TRID_OLD_NUMBER), "");
        pWorkNoItem->AddValue(_LANG(TRID_OLD_NUMBER), "");
        pMobileNoItem->AddValue(_LANG(TRID_OLD_NUMBER), "");
        return;
    }

    AddCombobxOption(strNumber1, strNumber2, 1, pHomeNoItem);
    AddCombobxOption(strNumber2, strNumber1, 0, pWorkNoItem);
    AddCombobxOption(strNumber1, strNumber2, 0, pMobileNoItem);
}

void CMTSWContactUIHelper::AddCombobxOption(const yl::string& strNumber1, const yl::string& strNumber2
        , int nFocusIndex, CComboBoxListItem* pComboboxItem)
{
    if (NULL == pComboboxItem)
    {
        return;
    }

    pComboboxItem->ClearValue();
    pComboboxItem->AddValue(_LANG(TRID_OLD_NUMBER), "");
    if (strNumber1.empty())
    {
        pComboboxItem->AddValue(strNumber2, strNumber2);
    }
    else
    {
        pComboboxItem->AddValue(strNumber1, strNumber1);
    }
    pComboboxItem->SetCurrentIndex(nFocusIndex);
}

bool CMTSWContactUIHelper::IsAddition()
{
    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
    CDetailAdapter * pDetailAdapter = CDetailAdapter::GetDetailAdapter(pAdapter);

    if (NULL == m_pDetailLayout || NULL == pDetailAdapter || m_pDetailLayout != pDetailAdapter->GetDetailLayout())
    {
        return false;
    }

    if (m_pDetailLayout->IsAddLayout() || m_pDetailLayout->IsCopyLayout())
    {
        return true;
    }

    return false;
}

void CMTSWContactUIHelper::SetInputFocus(const char * pszAttr)
{
    if (NULL == pszAttr || NULL == m_pDetailLayout)
    {
        return;
    }

    if (0 == strcmp(pszAttr, DIR_ATTR_NAME))
    {
        CEditListItem* pNameItem = m_pDetailLayout->GetFirstNameItem();
        if (NULL != pNameItem)
        {
            pNameItem->FocusItem(true);
        }
        return;
    }
    else if (0 == strcmp(pszAttr, DIR_ATTR_MTSW_HOMENUMBER))
    {
        CEditListItem* pNumberItem = m_pDetailLayout->GetHomeNumberItem();
        if (NULL != pNumberItem)
        {
            pNumberItem->FocusItem(true);
        }
    }
    else if (0 == strcmp(pszAttr, DIR_ATTR_MTSW_WORKNUMBER))
    {
        CEditListItem* pNumberItem = m_pDetailLayout->GetWorkNumberItem();
        if (NULL != pNumberItem)
        {
            pNumberItem->FocusItem(true);
        }
    }
    else if (0 == strcmp(pszAttr, DIR_ATTR_MTSW_MOBILENUMBER))
    {
        CEditListItem* pNumberItem = m_pDetailLayout->GetMobileItem();
        if (NULL != pNumberItem)
        {
            pNumberItem->FocusItem(true);
        }
    }
    else if (0 == strcmp(pszAttr, DIR_ATTR_MTSW_FAXNUMBER))
    {
        CEditListItem* pNumberItem = m_pDetailLayout->GetFaxNumberItem();
        if (NULL != pNumberItem)
        {
            pNumberItem->FocusItem(true);
        }
    }
    else  if (0 == strcmp(pszAttr, DIR_ATTR_MTSW_OTHERNUMBER))
    {
        CEditListItem* pNumberItem = m_pDetailLayout->GetOtherNumberItem();
        if (NULL != pNumberItem)
        {
            pNumberItem->FocusItem(true);
        }
    }
}

bool CMTSWContactUIHelper::GetOption(int nUIState, VEC_OPTION_ITEM_DATA& listOption)
{
    if (DIR_UI_STATE_BROWSER != nUIState && DIR_UI_STATE_SEARCH != nUIState)
    {
        return false;
    }

    if (NULL == m_pListView)
    {
        return false;
    }

    xListViewItemPtr pFocusItem = m_pListView->GetFocusItem();
    if (NULL == pFocusItem)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CMTSWContactControler * pMTSWContact = CMTSWContactControler::GetControler(pControler);

    if (NULL == pMTSWContact)
    {
        return false;
    }

    ST_OPTION_ITEM_DATA stOption;

    int nFocusId = GetLisItemId(pFocusItem);

    if (pMTSWContact->IsContactList(nFocusId))
    {
        stOption.m_nAction = ST_DETAIL;
        stOption.m_strTitle = TRID_DETAIL;
        listOption.push_back(stOption);

        stOption.m_nAction = ST_ADD;
        stOption.m_strTitle = TRID_ADD;
        listOption.push_back(stOption);

        stOption.m_nAction = ST_MTSW_SORT;
        if (MtswDir_GetSortType() == SORT_BY_LASTNAME)
        {
            stOption.m_strTitle = TRID_MTSW_SORT_FIRSTNAME;
        }
        else
        {
            stOption.m_strTitle = TRID_MTSW_SORT_LASTNAME;
        }
        listOption.push_back(stOption);

        stOption.m_nAction = ST_DELETE;
        stOption.m_strTitle = TRID_DELETE;
        listOption.push_back(stOption);

        stOption.m_nAction = ST_CLEAR;
        stOption.m_strTitle = TRID_DELETE_ALL;
        listOption.push_back(stOption);
    }
    else
    {
        stOption.m_nAction = ST_VIEW;
        stOption.m_strTitle = TRID_DETAIL;
        listOption.push_back(stOption);

        stOption.m_nAction = ST_MTSW_ADD_TO_LOCAL;
        stOption.m_strTitle = TRID_ADD_TO_PHONEBOOK;
        listOption.push_back(stOption);
    }

    return true;
}

void CMTSWContactUIHelper::GetMenuTitle(yl::string & strTitle)
{
    strTitle = _LANG(TRID_NETWORK_CONTACTS);
}

bool CMTSWContactUIHelper::GetNormalTitle(yl::string & strTitle, int nGroupId/* = knInvalidRId*/)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CMTSWContactControler * pMTSWContact = CMTSWContactControler::GetControler(pControler);

    if (NULL == pMTSWContact)
    {
        return false;
    }

    if (knInvalidRId == nGroupId)
    {
        nGroupId = pMTSWContact->GetGroupId();
    }

    yl::string strGroupName;

    if (kszAllContactId != nGroupId && !pMTSWContact->GetGroupName(nGroupId, strGroupName))
    {
        return false;
    }

    GetMetaSwitchDisplayName(nGroupId, strGroupName, strTitle);

    return true;
}

bool CMTSWContactUIHelper::GetDetailTitle(yl::string & strTitle)
{
    if (CCommonUIHelper::GetDetailTitle(strTitle))
    {
        return true;
    }

    if (NULL != m_pDetailLayout)
    {
        switch (m_pDetailLayout->GetAction())
        {
        case DIR_ACTION_MTSW_ADD_CONTACTLIST:
        case DIR_ACTION_MTSW_ADD_TO_CONTACTLIST:
            {
                strTitle = _LANG(TRID_ADD_MTSW_CONTACT);
            }
            break;
        case DIR_ACTION_MTSW_ADD_TO_LOCAL:
            {
                strTitle = _LANG(TRID_ADD_TO_CONTACT);
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

bool CMTSWContactUIHelper::GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                                      CArraySoftKeyBarData & vecSoftKey)
{
    if (vecSoftKey.size() < 4)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CMTSWContactControler * pMTSWContact = CMTSWContactControler::GetControler(pControler);

    if (NULL == pMTSWContact)
    {
        return false;
    }

    if (DIR_UI_STATE_DETAIL == nUIState || DIR_UI_STATE_ADDITION == nUIState)
    {
        CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
        CDetailAdapter* pDetailAdapter = CDetailAdapter::GetDetailAdapter(pAdapter);
        if (NULL != pDetailAdapter && NULL != pDetailAdapter->GetDetailLayout())
        {
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SAVE);
            vecSoftKey[3].m_iType = ST_SAVE;
        }

        return true;
    }

    bool bEditContact = CDirectoryControler::IsDirCanEdit(DIRECTORY_TYPE_METASWICTH, pMTSWContact->GetGroupId());
    int nFocusId = GetLisItemId(pFocusItem);

    if (knInvalidRId == nFocusId)
    {
        if (DIR_UI_STATE_BROWSER == nUIState)
        {
            CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
            CBrowseAdapter* pBrowserAdapter = CBrowseAdapter::GetBrowserAdapter(pAdapter);
            if (NULL == pBrowserAdapter || pBrowserAdapter->IsMenuPage())
            {
                return true;
            }

            vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
            vecSoftKey[1].m_iType = ST_SEARCH;

            if (bEditContact && (kszContactList == pMTSWContact->GetGroupId()
                                 || kszMtswAllContact == pMTSWContact->GetGroupId()))
            {
                vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ADD);
                vecSoftKey[3].m_iType = ST_ADD;
            }

            return true;
        }

        return false;
    }

    int iGroupId = pMTSWContact->GetGroupId();
    if (DIR_UI_STATE_BROWSER == nUIState
            && (IsMenuID(nFocusId) || kszContactList == iGroupId))
    {
        if (bEditContact)
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

// 焦点在联系人上（包括搜索界面）
    if (DIR_UI_STATE_SEARCH != nUIState)
    {
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
        vecSoftKey[1].m_iType = ST_SEARCH;
    }

    if (bEditContact)
    {
        if (kszMtswAllContact == iGroupId || IsMenuID(iGroupId))
        {
            vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_OPTION);
            vecSoftKey[2].m_iType = ST_OPTION;
        }
        else
        {
            vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_DETAIL);
            vecSoftKey[2].m_iType = ST_VIEW;
        }
    }

    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_DIAL);
    vecSoftKey[3].m_iType = ST_SEND;

    return true;
}

int CMTSWContactUIHelper::GetActionByUIAction(int nUIState, int nUIAction)
{
    if (ST_ADD == nUIAction)
    {
        return DIR_ACTION_MTSW_ENTER_ADD_CONTACTLISTL;
    }
    else if (ST_VIEW == nUIAction)
    {
        return DIR_ACTION_COMMON_DETAIL;
    }
    else if (ST_DETAIL == nUIAction)
    {
        return DIR_ACTION_MTSW_ENTER_EDIT_CONTACTLISTL;
    }
    else if (ST_MTSW_SORT == nUIAction)
    {
        return DIR_ACTION_MTSW_SORT;
    }
    else if (ST_UPDATE == nUIAction)
    {
        return DIR_ACTION_MTSW_UPDATE;
    }
    else if (ST_DELETE == nUIAction)
    {
        return DIR_ACTION_MTSW_DEL_CONTACTLIST;
    }
    else if (ST_CLEAR == nUIAction)
    {
        return DIR_ACTION_MTSW_DEL_ALL_CONTACTLIST;
    }
    else if (ST_MTSW_ADD_TO_LOCAL == nUIAction)
    {
        return DIR_ACTION_MTSW_ENTER_ADD_TO_LOCAL;
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

yl::string CMTSWContactUIHelper::GetNumberAttribute(const yl::string & strName)
{
    if (TRID_MTSW_HOME_NUMBER == strName)
    {
        return yl::string(DIR_ATTR_MTSW_HOMENUMBER);
    }
    else if (TRID_MTSW_WORK_NUMBER == strName)
    {
        return yl::string(DIR_ATTR_MTSW_WORKNUMBER);
    }
    else if (TRID_MTSW_OTHER_NUMBER == strName)
    {
        return yl::string(DIR_ATTR_MTSW_OTHERNUMBER);
    }
    else if (TRID_MTSW_FAX_NUMBER == strName)
    {
        return yl::string(DIR_ATTR_MTSW_FAXNUMBER);
    }
    else if (TRID_MTSW_MOBILE_NUMBER == strName)
    {
        return yl::string(DIR_ATTR_MTSW_MOBILENUMBER);
    }

    return yl::string(DIR_ATTR_NUMBER);
}

void CMTSWContactUIHelper::GetMetaSwitchDisplayName(int nGroupId, const yl::string & strGroupName,
        yl::string & strDisplayName)
{
    if (kszAllContactId == nGroupId || kszMtswAllContact == nGroupId)
    {
        strDisplayName = _LANG(TRID_ALL_CONTACTS);
        return;
    }

    if (!strGroupName.empty())
    {
        strDisplayName = strGroupName;
        return;
    }

    if (kszContactList == nGroupId)
    {
        strDisplayName = _LANG(TRID_MTSW_GROUP_NAME_CONTACTLIST);
    }
    else if (kszExtention == nGroupId)
    {
        strDisplayName = _LANG(TRID_MTSW_GROUP_NAME_EXTENTION);
    }
    else if (kszMLHG == nGroupId)
    {
        strDisplayName = _LANG(TRID_MTSW_GROUP_NAME_MLHG);
    }
}

bool CMTSWContactUIHelper::GetMTSWContactListData(ContactMetaSwitchContactList & itemData)
{
    if (NULL == m_pDetailLayout)
    {
        return false;
    }

    m_pDetailLayout->ReturnAllEdit();

    CEditListItemPtr pFirstNameItem = m_pDetailLayout->GetFirstNameItem();
    if (pFirstNameItem != NULL)
    {
        itemData.m_strFirstName = pFirstNameItem->GetValue().trim_both();
    }

    CEditListItemPtr pLastNameItem = m_pDetailLayout->GetLastNameItem();
    if (pLastNameItem != NULL)
    {
        itemData.m_strLastName = pLastNameItem->GetValue().trim_both();
    }

    itemData.m_strDisplayName = itemData.m_strFirstName;
    if (!itemData.m_strFirstName.empty() && !itemData.m_strLastName.empty())
    {
        itemData.m_strDisplayName.append(",");
    }
    itemData.m_strDisplayName.append(itemData.m_strLastName);


    CEditListItemPtr pNickNameItem = m_pDetailLayout->GetNickNameItem();
    if (pNickNameItem != NULL)
    {
        itemData.m_strNickName = pNickNameItem->GetValue().trim_both();
    }

    CEditListItemPtr pJobItem = m_pDetailLayout->GetJobTitleItem();
    if (pJobItem != NULL)
    {
        itemData.m_strJobTitle = pJobItem->GetValue().trim_both();
    }

    CEditListItemPtr pOrgItem = m_pDetailLayout->GetOrgItem();
    if (pOrgItem != NULL)
    {
        itemData.m_strOrganisation = pOrgItem->GetValue().trim_both();
    }

    if (m_pDetailLayout->IsCopyLayout())
    {
        CComboBoxListItemPtr pHomeNoItem = m_pDetailLayout->GetHomeNumberComboBoxItem();
        if (NULL != pHomeNoItem)
        {
            itemData.SetNumber(kszMtswHomeNumber, pHomeNoItem->GetItemStringData());
        }

        CComboBoxListItemPtr pWorkNoItem = m_pDetailLayout->GetWorkNumberComboBoxItem();
        if (NULL != pWorkNoItem)
        {
            itemData.SetNumber(kszMtswWorkNumber, pWorkNoItem->GetItemStringData());
        }

        CComboBoxListItemPtr pMobileNoItem = m_pDetailLayout->GetMobileComboBoxItem();
        if (NULL != pMobileNoItem)
        {
            itemData.SetNumber(kszMtswMobile, pMobileNoItem->GetItemStringData());
        }
    }
    else
    {
        CEditListItemPtr pHomeNoItem = m_pDetailLayout->GetHomeNumberItem();
        if (NULL != pHomeNoItem)
        {
            itemData.SetNumber(kszMtswHomeNumber, pHomeNoItem->GetValue().trim_both());
        }

        CEditListItemPtr pWorkNoItem = m_pDetailLayout->GetWorkNumberItem();
        if (NULL != pWorkNoItem)
        {
            itemData.SetNumber(kszMtswWorkNumber, pWorkNoItem->GetValue().trim_both());
        }

        CEditListItemPtr pMobileNoItem = m_pDetailLayout->GetMobileItem();
        if (NULL != pMobileNoItem)
        {
            itemData.SetNumber(kszMtswMobile, pMobileNoItem->GetValue().trim_both());
        }
    }

    CEditListItemPtr pFaxNoItem = m_pDetailLayout->GetFaxNumberItem();
    if (NULL != pFaxNoItem)
    {
        itemData.SetNumber(kszMtswFax, pFaxNoItem->GetValue().trim_both());
    }

    CEditListItemPtr pOtherNoItem = m_pDetailLayout->GetOtherNumberItem();
    if (NULL != pOtherNoItem)
    {
        itemData.SetNumber(kszMtswOtherNumber, pOtherNoItem->GetValue().trim_both());
    }

    return true;
}

bool CMTSWContactUIHelper::UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail)
{
    if (NULL == pListItem || pDetail.IsEmpty())
    {
        return false;
    }

    int nDetailType = pDetail->GetType();
    if (DIR_TYPE_GROUP == nDetailType || DIR_TYPE_METASWITCH_MENU == nDetailType)
    {
        yl::string strDisplayName;

        GetMetaSwitchDisplayName(nId, pDetail->GetDisplayName(), strDisplayName);

        bool bShowIndex = IsMenuID(nId);
        return UpdateMenuItem((CMenuListItem*)pListItem, nId, strDisplayName, bShowIndex);
    }

    yl::string strNumber = "";
    if (!GetNumber(pDetail, nDetailType, strNumber))
    {
        return false;
    }

    return UpdateContactItem(pListItem, nId, pDetail->GetDisplayName(), strNumber);
}

bool CMTSWContactUIHelper::GetNumber(SmartPtr & pDetail, int nType, yl::string & strNumber)
{
    if (DIR_TYPE_METASWITCH_ITEM_EXTENTION == nType)
    {
        ContactMetaSwitchExtention* pContact = (ContactMetaSwitchExtention*)pDetail.Get();
        if (NULL == pContact)
        {
            return false;
        }
        if (!pContact->GetNumberByName(kszMtswExtention, strNumber) || strNumber.empty())
        {
            pContact->GetNumberByName(kszMtswDirectoryNumber, strNumber);
        }
    }
    else if (DIR_TYPE_METASWITCH_ITEM_MLHG == nType)
    {
        ContactMetaSwitchMLHG* pContact = (ContactMetaSwitchMLHG*)pDetail.Get();
        if (NULL == pContact)
        {
            return false;
        }
        pContact->GetNumberByName(kszMtswDirectoryNumber, strNumber);
    }
    else if (DIR_TYPE_METASWITCH_ITEM_CONTACTLIST == nType)
    {
        return GetContactListNumber(pDetail, strNumber);
    }
    else
    {
        return false;
    }

    return true;
}

bool CMTSWContactUIHelper::GetContactListNumber(SmartPtr & pDetail, yl::string & strNumber)
{
    ContactMetaSwitchContactList* pContact = CMTSWContactControler::GetMetaSwitchContact(pDetail);
    if (NULL == pContact)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CMTSWContactControler * pMtswControler = CMTSWContactControler::GetControler(pControler);
    if (NULL == pMtswControler)
    {
        return false;
    }

    if (pMtswControler->IsSearchState())
    {
        strNumber = pContact->GetSearchShowNumber(pMtswControler->GetSearchKey());
    }
    else
    {
        if (!pContact->GetNumberByName(kszMtswHomeNumber, strNumber) || strNumber.empty())
        {
            if (!pContact->GetNumberByName(kszMtswWorkNumber, strNumber) || strNumber.empty())
            {
                if (!pContact->GetNumberByName(kszMtswMobile, strNumber) || strNumber.empty())
                {
                    if (!pContact->GetNumberByName(kszMtswFax, strNumber) || strNumber.empty())
                    {
                        pContact->GetNumberByName(kszMtswOtherNumber, strNumber);
                    }
                }
            }
        }
    }

    return true;
}

bool CMTSWContactUIHelper::IsMenuID(int nID)
{
    if (kszContactList == nID || kszExtention == nID || kszMLHG == nID)
    {
        return true;
    }
    return false;
}

#endif
