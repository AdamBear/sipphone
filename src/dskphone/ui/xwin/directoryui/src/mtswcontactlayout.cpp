#if IF_FEATURE_METASWITCH_DIRECTORY
#include "mtswcontactlayout.h"
#include "directorycommon.h"
#include "directoryuicommon.h"
#include "baseui/include/baseuicommon.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/comboboxlistitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "baseui/include/inputmethod.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CMTSWContactLayout::CMTSWContactLayout()
    : CDetailLayout(DETAIL_LIST_ITEM_LAYOUT_MTSW_CONTACT)
    , m_nDetailId(-1)
    , m_nAction(DIR_ACTION_NONE)
{
}

CMTSWContactLayout::~CMTSWContactLayout()
{
}

void CMTSWContactLayout::UnInit()
{
    m_nAction = DIR_ACTION_NONE;
    m_nDetailId = knInvalidRId;
    CDetailLayout::UnInit();
}

void CMTSWContactLayout::Init()
{
    if (0 != m_vecListItem.size())
    {
        ReleaseAllItem();
        m_nDetailId = knInvalidRId;
    }

    InitEditItem(MTSW_DETAIL_ITEM_INDEX_FIRST_NAME, MAX_LEN_DISPLAY_NAME, TRID_FIRST_NAME);
    InitEditItem(MTSW_DETAIL_ITEM_INDEX_LAST_NAME, MAX_LEN_DISPLAY_NAME, TRID_LAST_NAME);
    InitEditItem(MTSW_DETAIL_ITEM_INDEX_NICK_NAME, MAX_LEN_DISPLAY_NAME, TRID_MTSW_NICKNAME);
    InitEditItem(MTSW_DETAIL_ITEM_INDEX_JOBTITLE, MAX_LEN_DISPLAY_NAME, TRID_MTSW_JOBTITLE);
    InitEditItem(MTSW_DETAIL_ITEM_INDEX_ORGANISATION, MAX_LEN_DISPLAY_NAME, TRID_MTSW_ORGANISATION);

    if (IsCopyLayout())
    {
        InitComboboxItem(MTSW_DETAIL_ITEM_INDEX_HOME_NUMBER, TRID_MTSW_HOME_NUMBER);
        InitComboboxItem(MTSW_DETAIL_ITEM_INDEX_WORK_NUMBER, TRID_MTSW_WORK_NUMBER);
        InitComboboxItem(MTSW_DETAIL_ITEM_INDEX_MOBILE, TRID_MTSW_MOBILE_NUMBER);
    }
    else
    {
        InitEditItem(MTSW_DETAIL_ITEM_INDEX_HOME_NUMBER, MAX_LEN_PHONE_NUMBER_LONG, TRID_MTSW_HOME_NUMBER, false);
        InitEditItem(MTSW_DETAIL_ITEM_INDEX_WORK_NUMBER, MAX_LEN_PHONE_NUMBER_LONG, TRID_MTSW_WORK_NUMBER, false);
        InitEditItem(MTSW_DETAIL_ITEM_INDEX_MOBILE, MAX_LEN_PHONE_NUMBER_LONG, TRID_MTSW_MOBILE_NUMBER, false);
    }

    InitEditItem(MTSW_DETAIL_ITEM_INDEX_FAX_NUMBER, MAX_LEN_PHONE_NUMBER_LONG, TRID_MTSW_FAX_NUMBER, false);
    InitEditItem(MTSW_DETAIL_ITEM_INDEX_OTHER_NUMBER, MAX_LEN_PHONE_NUMBER_LONG, TRID_MTSW_OTHER_NUMBER, false);
}

void CMTSWContactLayout::SetAction(int nAction)
{
    m_nAction = nAction;
}

int CMTSWContactLayout::GetAction() const
{
    return m_nAction;
}

int CMTSWContactLayout::GetDetailId() const
{
    return m_nDetailId;
}

bool CMTSWContactLayout::IsCopyLayout() const
{
    if (DIR_ACTION_MTSW_ADD_TO_CONTACTLIST == m_nAction)
    {
        return true;
    }

    return false;
}

bool CMTSWContactLayout::IsAddLayout() const
{
    if (DIR_ACTION_MTSW_ADD_CONTACTLIST == m_nAction)
    {
        return true;
    }

    return false;
}

void CMTSWContactLayout::InitEditItem(int nDataIndex, int nMaxLength, const yl::string& strDisp, bool bAutoIme /*= true*/)
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CEditListItem::GetEditListItemType());
    CEditListItemPtr pEditListItem = CEditListItem::GetEditListItem(pItem);

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetDataIndex(nDataIndex);
    pEditListItem->SetShowIndex(true);
    pEditListItem->SetTitle(_LANG(strDisp));
    pEditListItem->SetValue("");
    pEditListItem->SetMaxLength(nMaxLength);

    m_vecListItem.push_back(pEditListItem);

    yl::string strSupportIME = IME_DIAL;
    yl::string strIME = IME_123_Dial;
    if (bAutoIme)
    {
        DIRUI_GetNameIME(strSupportIME, strIME);
    }

    pEditListItem->InstallIME(strSupportIME.c_str(), strIME.c_str());
}

void CMTSWContactLayout::InitComboboxItem(int nDataIndex, const yl::string& strDisp)
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CComboBoxListItem::GetComboBoxListItemType());
    CComboBoxListItemPtr pComboBoxItem = CComboBoxListItem::GetComboBoxListItem(pItem);

    if (NULL == pComboBoxItem)
    {
        return;
    }

    pComboBoxItem->SetDataIndex(nDataIndex);
    pComboBoxItem->SetShowIndex(true);
    pComboBoxItem->SetTitle(_LANG(strDisp));

    m_vecListItem.push_back(pComboBoxItem);
}

void CMTSWContactLayout::ReturnAllEdit()
{
    //设置处于预输入状态的字符也有效
    CEditListItemPtr pFirstNameItem = GetFirstNameItem();
    if (NULL != pFirstNameItem)
    {
        pFirstNameItem->ReturnAllInput();
    }

    CEditListItemPtr pLastNameItem = GetLastNameItem();
    if (NULL != pLastNameItem)
    {
        pLastNameItem->ReturnAllInput();
    }

    CEditListItemPtr pNickNameItem = GetNickNameItem();
    if (NULL != pNickNameItem)
    {
        pNickNameItem->ReturnAllInput();
    }

    CEditListItemPtr pJobItem = GetJobTitleItem();
    if (NULL != pJobItem)
    {
        pJobItem->ReturnAllInput();
    }

    CEditListItemPtr pOrgItem = GetOrgItem();
    if (NULL != pOrgItem)
    {
        pOrgItem->ReturnAllInput();
    }

    CEditListItemPtr pHomeNumberItem = GetHomeNumberItem();
    if (NULL != pHomeNumberItem)
    {
        pHomeNumberItem->ReturnAllInput();
    }

    CEditListItemPtr pWorkItem = GetWorkNumberItem();

    if (NULL != pWorkItem)
    {
        pWorkItem->ReturnAllInput();
    }

    CEditListItemPtr pMobileItem = GetMobileItem();
    if (NULL != pMobileItem)
    {
        pMobileItem->ReturnAllInput();
    }

    CEditListItemPtr pFaxItem = GetFaxNumberItem();
    if (NULL != pFaxItem)
    {
        pFaxItem->ReturnAllInput();
    }

    CEditListItemPtr pOtherItem = GetOtherNumberItem();
    if (NULL != pOtherItem)
    {
        pOtherItem->ReturnAllInput();
    }
}

void CMTSWContactLayout::ClearEdit()
{
    CEditListItemPtr pFirstNameItem = GetFirstNameItem();
    if (NULL != pFirstNameItem)
    {
        pFirstNameItem->SetValue("");
    }

    CEditListItemPtr pLastNameItem = GetLastNameItem();
    if (NULL != pLastNameItem)
    {
        pLastNameItem->SetValue("");
    }

    CEditListItemPtr pNickNameItem = GetNickNameItem();
    if (NULL != pNickNameItem)
    {
        pNickNameItem->SetValue("");
    }

    CEditListItemPtr pJobItem = GetJobTitleItem();
    if (NULL != pJobItem)
    {
        pJobItem->SetValue("");
    }

    CEditListItemPtr pOrgItem = GetOrgItem();
    if (NULL != pOrgItem)
    {
        pOrgItem->SetValue("");
    }

    CEditListItemPtr pHomeNumberItem = GetHomeNumberItem();
    if (NULL != pHomeNumberItem)
    {
        pHomeNumberItem->SetValue("");
    }

    CEditListItemPtr pWorkItem = GetWorkNumberItem();
    if (NULL != pWorkItem)
    {
        pWorkItem->SetValue("");
    }

    CEditListItemPtr pMobileItem = GetMobileItem();
    if (NULL != pMobileItem)
    {
        pMobileItem->SetValue("");
    }

    CEditListItemPtr pFaxItem = GetFaxNumberItem();
    if (NULL != pFaxItem)
    {
        pFaxItem->SetValue("");
    }

    CEditListItemPtr pOtherItem = GetOtherNumberItem();
    if (NULL != pOtherItem)
    {
        pOtherItem->SetValue("");
    }
}

bool CMTSWContactLayout::UpdateContact(int nContactId, ContactMetaSwitchContactList * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    if (nContactId == m_nDetailId)
    {
        return true;
    }

    m_nDetailId = nContactId;

    CEditListItemPtr pFirstNameItem = GetFirstNameItem();
    if (NULL != pFirstNameItem)
    {
        pFirstNameItem->SetValue(pContact->m_strFirstName);
    }

    CEditListItemPtr pLastNameItem = GetLastNameItem();
    if (NULL != pLastNameItem)
    {
        pLastNameItem->SetValue(pContact->m_strLastName);
    }

    CEditListItemPtr pNickNameItem = GetNickNameItem();
    if (NULL != pNickNameItem)
    {
        pNickNameItem->SetValue(pContact->m_strNickName);
    }

    CEditListItemPtr pJobItem = GetJobTitleItem();
    if (NULL != pJobItem)
    {
        pJobItem->SetValue(pContact->m_strJobTitle);
    }

    CEditListItemPtr pOrgItem = GetOrgItem();
    if (NULL != pOrgItem)
    {
        pOrgItem->SetValue(pContact->m_strOrganisation);
    }

    CEditListItemPtr pHomeNoItem = GetHomeNumberItem();
    SetNumberByName(pContact, kszMtswHomeNumber, pHomeNoItem);

    CEditListItemPtr pWorkNoItem = GetWorkNumberItem();
    SetNumberByName(pContact, kszMtswWorkNumber, pWorkNoItem);

    CEditListItemPtr pMobileNoItem = GetMobileItem();
    SetNumberByName(pContact, kszMtswMobile, pMobileNoItem);

    CEditListItemPtr pFaxNoItem = GetFaxNumberItem();
    SetNumberByName(pContact, kszMtswFax, pFaxNoItem);

    CEditListItemPtr pOtherNoItem = GetOtherNumberItem();
    SetNumberByName(pContact, kszMtswOtherNumber, pOtherNoItem);

    return true;
}

void CMTSWContactLayout::SetNumberByName(ContactMetaSwitchContactList* pContact, const yl::string& strName, CEditListItem* pEditItem)
{
    if (NULL == pEditItem || NULL == pContact)
    {
        return;
    }

    yl::string strNumber = "";

    pContact->GetNumberByName(strName, strNumber);
    pEditItem->SetValue(strNumber);
}

CEditListItem * CMTSWContactLayout::GetFirstNameItem()
{
    return GetEditItemIndex(MTSW_DETAIL_ITEM_INDEX_FIRST_NAME);
}

CEditListItem * CMTSWContactLayout::GetLastNameItem()
{
    return GetEditItemIndex(MTSW_DETAIL_ITEM_INDEX_LAST_NAME);
}

CEditListItem * CMTSWContactLayout::GetOrgItem()
{
    return GetEditItemIndex(MTSW_DETAIL_ITEM_INDEX_ORGANISATION);
}

CEditListItem * CMTSWContactLayout::GetNickNameItem()
{
    return GetEditItemIndex(MTSW_DETAIL_ITEM_INDEX_NICK_NAME);
}

CEditListItem * CMTSWContactLayout::GetJobTitleItem()
{
    return GetEditItemIndex(MTSW_DETAIL_ITEM_INDEX_JOBTITLE);
}

CEditListItem * CMTSWContactLayout::GetMobileItem()
{
    return GetEditItemIndex(MTSW_DETAIL_ITEM_INDEX_MOBILE);
}

CEditListItem * CMTSWContactLayout::GetHomeNumberItem()
{
    return GetEditItemIndex(MTSW_DETAIL_ITEM_INDEX_HOME_NUMBER);
}

CEditListItem * CMTSWContactLayout::GetWorkNumberItem()
{
    return GetEditItemIndex(MTSW_DETAIL_ITEM_INDEX_WORK_NUMBER);
}

CEditListItem * CMTSWContactLayout::GetFaxNumberItem()
{
    return GetEditItemIndex(MTSW_DETAIL_ITEM_INDEX_FAX_NUMBER);
}

CEditListItem * CMTSWContactLayout::GetOtherNumberItem()
{
    return GetEditItemIndex(MTSW_DETAIL_ITEM_INDEX_OTHER_NUMBER);
}

CEditListItem* CMTSWContactLayout::GetEditItemIndex(int nIndex)
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(nIndex);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CComboBoxListItem * CMTSWContactLayout::GetHomeNumberComboBoxItem()
{
    return GetComboboxItemByIndex(MTSW_DETAIL_ITEM_INDEX_HOME_NUMBER);
}

CComboBoxListItem * CMTSWContactLayout::GetWorkNumberComboBoxItem()
{
    return GetComboboxItemByIndex(MTSW_DETAIL_ITEM_INDEX_WORK_NUMBER);
}

CComboBoxListItem * CMTSWContactLayout::GetMobileComboBoxItem()
{
    return GetComboboxItemByIndex(MTSW_DETAIL_ITEM_INDEX_MOBILE);
}

CComboBoxListItem * CMTSWContactLayout::GetFaxNumberComboBoxItem()
{
    return GetComboboxItemByIndex(MTSW_DETAIL_ITEM_INDEX_FAX_NUMBER);
}

CComboBoxListItem * CMTSWContactLayout::GetOtherNumberComboBoxItem()
{
    return GetComboboxItemByIndex(MTSW_DETAIL_ITEM_INDEX_OTHER_NUMBER);
}

CComboBoxListItem * CMTSWContactLayout::GetComboboxItemByIndex(int nIndex)
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(nIndex);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}

#endif //#if IF_FEATURE_METASWITCH_DIRECTORY
