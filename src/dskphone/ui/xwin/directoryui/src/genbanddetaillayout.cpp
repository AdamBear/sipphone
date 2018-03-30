#include "genbanddetaillayout.h"

#if IF_FEATURE_GBBOOK

#include "baseui/include/baseuicommon.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/comboboxlistitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "baseui/include/inputmethod.h"
#include "imeedit/imecommon.h"
#include "languagehelper/modlangtranslate.h"

#include "directorycommon.h"
#include "directoryuicommon.h"
#include "moddirectoryui.h"
#include "account/include/modaccount.h"
#include "commonunits/modcommonunits.h"
#include "commonunits/commonunits_def.h"

CGenbandDetailLayout::CGenbandDetailLayout()
    : CDetailLayout(DETAIL_LIST_ITEM_LAYOUT_GENBAND)
    , m_nDetailId(knInvalidRId)
    , m_nAction(DIR_ACTION_NONE)
{
}

CGenbandDetailLayout::~CGenbandDetailLayout(void)
{
}

bool CGenbandDetailLayout::IsGenbandDetailLayout(CDetailLayout* pLayout)
{
    if (NULL != pLayout && DETAIL_LIST_ITEM_LAYOUT_GENBAND == pLayout->GetType())
    {
        return true;
    }

    return false;
}

int CGenbandDetailLayout::GetDetailId()
{
    return m_nDetailId;
}

void CGenbandDetailLayout::SetAction(int nAction)
{
    m_nAction = nAction;
}

int CGenbandDetailLayout::GetAction() const
{
    return m_nAction;
}

bool CGenbandDetailLayout::IsContactLayout()
{
    if (DIR_ACTION_GENBAND_ADD_PAB == m_nAction
            || DIR_ACTION_GENBAND_EDIT_PAB == m_nAction
            || DIR_ACTION_GENBAND_ADD_TO_PAB == m_nAction)
    {
        return true;
    }

    return false;
}

bool CGenbandDetailLayout::IsGroupLayout()
{
    if (DIR_ACTION_GENBAND_EDIT_GROUP == m_nAction
            || DIR_ACTION_GENBAND_ADD_GROUP == m_nAction)
    {
        return true;
    }

    return false;
}

void CGenbandDetailLayout::Init()
{
    ReleaseAllItem();

    m_nDetailId = knInvalidRId;

    yl::string strNameDisp = "";
    if (IsGroupLayout())
    {
        if (DIR_ACTION_GENBAND_EDIT_GROUP == m_nAction)
        {
            strNameDisp = TRID_RENAME_GROUP;
        }
        else
        {
            strNameDisp = TRID_ADD_GROUP;
        }
    }
    else
    {
        strNameDisp = TRID_NICK_NAME;
    }

    if (IsContactLayout())
    {
        InitNameCtrl(strNameDisp, GENBAND_CONTACT_DETAIL_ITEM_INDEX_NICK_NAME);
        InitGroupCtrl();

        InitNumberCtrl(TRID_PRIMARYCONTACT, GENBAND_CONTACT_DETAIL_ITEM_INDEX_PRIMARY_CONTACT);

        InitNameCtrl(TRID_FIRST_NAME, GENBAND_CONTACT_DETAIL_ITEM_INDEX_FIRST_NAME);
        InitNameCtrl(TRID_LAST_NAME, GENBAND_CONTACT_DETAIL_ITEM_INDEX_LAST_NAME);

        InitNumberCtrl(TRID_BUSINESS_NUMBER, GENBAND_CONTACT_DETAIL_ITEM_INDEX_BUSINESS_NO);
        InitNumberCtrl(TRID_HOME_NUMBER, GENBAND_CONTACT_DETAIL_ITEM_INDEX_HOME_NO);
        InitNumberCtrl(TRID_MOBILE_NUMBER, GENBAND_CONTACT_DETAIL_ITEM_INDEX_MOBILE_NO);
        InitNumberCtrl(TRID_PAGER, GENBAND_CONTACT_DETAIL_ITEM_INDEX_PAGER);
        InitNumberCtrl(TRID_FAX, GENBAND_CONTACT_DETAIL_ITEM_INDEX_FAX);

        InitNameCtrl(TRID_EMAIL, GENBAND_CONTACT_DETAIL_ITEM_INDEX_EMAIL);

        InitNumberCtrl(TRID_CONFERENCE_URL, GENBAND_CONTACT_DETAIL_ITEM_INDEX_CONF_URL);

    }
    else if (IsGroupLayout())
    {
        InitNameCtrl(strNameDisp, GENBAND_GROUP_DETAIL_ITEM_INDEX_NAME);
    }
}

// 初始化Name
void CGenbandDetailLayout::InitNameCtrl(const yl::string& strTtle, int nDataIndex)
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CEditListItem::GetEditListItemType());
    CEditListItemPtr pEditListItem = CEditListItem::GetEditListItem(pItem);

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetDataIndex(nDataIndex);
    pEditListItem->SetShowIndex(true);
    pEditListItem->SetValue("");
    pEditListItem->SetMaxLength(MAX_LEN_DISPLAY_NAME);
    pEditListItem->SetTitle(_LANG(strTtle));

    m_vecListItem.push_back(pEditListItem);

    yl::string strSupportIME;
    yl::string strIME;

    DIRUI_GetNameIME(strSupportIME, strIME);

    pEditListItem->InstallIME(strSupportIME.c_str(), strIME.c_str());
    pEditListItem->ConnectIMEChanged(this, static_cast<slotMethod>(&CDetailLayout::OnDetailIMEChanged));
}

// 初始化Number
void CGenbandDetailLayout::InitNumberCtrl(const yl::string& strNumTitle, int nDataIndex)
{

    xListViewItem * pItem = g_ItemFactory.ApplyItem(CEditListItem::GetEditListItemType());
    CEditListItemPtr pEditListItem = CEditListItem::GetEditListItem(pItem);

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetDataIndex(nDataIndex);
    pEditListItem->SetShowIndex(true);
    pEditListItem->SetTitle(_LANG(strNumTitle));
    pEditListItem->SetValue("");
    pEditListItem->SetMaxLength(MAX_LEN_PHONE_NUMBER);
    m_vecListItem.push_back(pEditListItem);

    pEditListItem->InstallIME(IME_DIAL, IME_123_Dial);
}

// 初始化群组ComboBox
void CGenbandDetailLayout::InitGroupCtrl()
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CComboBoxListItem::GetComboBoxListItemType());
    CComboBoxListItemPtr pComboBoxItem = CComboBoxListItem::GetComboBoxListItem(pItem);

    if (NULL == pComboBoxItem)
    {
        return;
    }

    pComboBoxItem->SetDataIndex(GENBAND_CONTACT_DETAIL_ITEM_INDEX_GROUP);
    pComboBoxItem->SetShowIndex(true);
    pComboBoxItem->SetTitle(_LANG(TRID_GROUP));

    m_vecListItem.push_back(pComboBoxItem);
}

CEditListItem * CGenbandDetailLayout::GetNameEditItem()
{
    if (IsContactLayout())
    {
        return GetEditItemByIndex(GENBAND_CONTACT_DETAIL_ITEM_INDEX_NICK_NAME, false);
    }
    else if (IsGroupLayout())
    {
        return GetEditItemByIndex(GENBAND_GROUP_DETAIL_ITEM_INDEX_NAME, false);
    }

    return NULL;
}

CEditListItem * CGenbandDetailLayout::GetFirstNameEditItem()
{
    return GetEditItemByIndex(GENBAND_CONTACT_DETAIL_ITEM_INDEX_FIRST_NAME);
}

CEditListItem * CGenbandDetailLayout::GetLastNameEditItem()
{
    return GetEditItemByIndex(GENBAND_CONTACT_DETAIL_ITEM_INDEX_LAST_NAME);
}

CEditListItem * CGenbandDetailLayout::GetBusinessNoEditItem()
{
    return GetEditItemByIndex(GENBAND_CONTACT_DETAIL_ITEM_INDEX_BUSINESS_NO);
}

CEditListItem * CGenbandDetailLayout::GetMobileNoEditItem()
{
    return GetEditItemByIndex(GENBAND_CONTACT_DETAIL_ITEM_INDEX_MOBILE_NO);
}

CEditListItem * CGenbandDetailLayout::GetHomeNoEditItem()
{
    return GetEditItemByIndex(GENBAND_CONTACT_DETAIL_ITEM_INDEX_HOME_NO);
}

CEditListItem * CGenbandDetailLayout::GetPagerEditItem()
{
    return GetEditItemByIndex(GENBAND_CONTACT_DETAIL_ITEM_INDEX_PAGER);
}

CEditListItem * CGenbandDetailLayout::GetFaxEditItem()
{
    return GetEditItemByIndex(GENBAND_CONTACT_DETAIL_ITEM_INDEX_FAX);
}

CEditListItem * CGenbandDetailLayout::GetEmailEditItem()
{
    return GetEditItemByIndex(GENBAND_CONTACT_DETAIL_ITEM_INDEX_EMAIL);
}

CEditListItem * CGenbandDetailLayout::GetConfUrlEditItem()
{
    return GetEditItemByIndex(GENBAND_CONTACT_DETAIL_ITEM_INDEX_CONF_URL);
}

CEditListItem * CGenbandDetailLayout::GetPriContactEditItem()
{
    return GetEditItemByIndex(GENBAND_CONTACT_DETAIL_ITEM_INDEX_PRIMARY_CONTACT);
}

CEditListItem* CGenbandDetailLayout::GetEditItemByIndex(int nIndex, bool bCheckLayout /*= true*/)
{
    if (bCheckLayout && !IsContactLayout())
    {
        return NULL;
    }

    CDetailListItemBasePtr pDetailItem = GetDetailListItem(nIndex);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return static_cast<CEditListItem *>(pDetailItem);
    }

    return NULL;
}

CComboBoxListItem * CGenbandDetailLayout::GetGroupComboBoxItem()
{
    if (!IsContactLayout())
    {
        return NULL;
    }

    CDetailListItemBasePtr pDetailItem = GetDetailListItem(GENBAND_CONTACT_DETAIL_ITEM_INDEX_GROUP);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return static_cast<CComboBoxListItem *>(pDetailItem);
    }

    return NULL;
}

void CGenbandDetailLayout::ReturnAllEdit()
{
    //设置处于预输入状态的字符也有效
    CEditListItemPtr pNameEditItem = GetNameEditItem();

    if (NULL != pNameEditItem)
    {
        pNameEditItem->ReturnAllInput();
    }

    CEditListItemPtr pFirstEditItem = GetFirstNameEditItem();

    if (NULL != pFirstEditItem)
    {
        pFirstEditItem->ReturnAllInput();
    }

    CEditListItemPtr pLastEditItem = GetLastNameEditItem();

    if (NULL != pLastEditItem)
    {
        pLastEditItem->ReturnAllInput();
    }

    CEditListItemPtr pBusinessNoEditItem = GetBusinessNoEditItem();

    if (NULL != pBusinessNoEditItem)
    {
        pBusinessNoEditItem->ReturnAllInput();
    }

    CEditListItemPtr pMobileNoEditItem = GetMobileNoEditItem();

    if (NULL != pMobileNoEditItem)
    {
        pMobileNoEditItem->ReturnAllInput();
    }

    CEditListItemPtr pHomeNoEditItem = GetHomeNoEditItem();

    if (NULL != pHomeNoEditItem)
    {
        pHomeNoEditItem->ReturnAllInput();
    }

    CEditListItemPtr pPageEditItem = GetPagerEditItem();

    if (NULL != pPageEditItem)
    {
        pPageEditItem->ReturnAllInput();
    }

    CEditListItemPtr pFaxEditItem = GetFaxEditItem();

    if (NULL != pFaxEditItem)
    {
        pFaxEditItem->ReturnAllInput();
    }

    CEditListItemPtr pEmailEditItem = GetEmailEditItem();

    if (NULL != pEmailEditItem)
    {
        pEmailEditItem->ReturnAllInput();
    }

    CEditListItemPtr pConfNoEditItem = GetConfUrlEditItem();

    if (NULL != pConfNoEditItem)
    {
        pConfNoEditItem->ReturnAllInput();
    }

    CEditListItemPtr pPriContactEditItem = GetPriContactEditItem();

    if (NULL != pPriContactEditItem)
    {
        pPriContactEditItem->ReturnAllInput();
    }
}

bool CGenbandDetailLayout::UpdateContact(int nContactId, ContactGBBookUnit * pContact)
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

    CEditListItemPtr pNameItem = GetNameEditItem();
    if (NULL != pNameItem)
    {
        if (DIR_ACTION_GENBAND_EDIT_PAB == m_nAction)
        {
            pNameItem->SetValue(pContact->GetDisplayName());
        }
        else
        {
            yl::string strDisplayName = pContact->m_strFirstName;
            if (!pContact->m_strLastName.empty())
            {
                strDisplayName += " ";
                strDisplayName += pContact->m_strLastName;
            }

            if (strDisplayName.empty())
            {
                strDisplayName = pContact->GetDisplayName();
            }

            pNameItem->SetValue(strDisplayName);
        }
    }

    CComboBoxListItemPtr pGroupItem = GetGroupComboBoxItem();
    if (NULL != pGroupItem)
    {
        pGroupItem->SetItemData(pContact->m_nParentId);
    }

    CEditListItemPtr pFirstNameItem = GetFirstNameEditItem();
    if (NULL != pFirstNameItem)
    {
        pFirstNameItem->SetValue(pContact->m_strFirstName);
    }

    CEditListItemPtr pLastNameItem = GetLastNameEditItem();
    if (NULL != pLastNameItem)
    {
        pLastNameItem->SetValue(pContact->m_strLastName);
    }

    CEditListItemPtr pBussnessNoItem = GetBusinessNoEditItem();
    UpdateEditValueByName(pBussnessNoItem, pContact, kszGbBookbusinessNumber);

    CEditListItemPtr pHomeNoItem = GetHomeNoEditItem();
    UpdateEditValueByName(pHomeNoItem, pContact, kszGbBookhomeNumber);

    CEditListItemPtr pMobileNoItem = GetMobileNoEditItem();
    UpdateEditValueByName(pMobileNoItem, pContact, kszGbBookmobile);

    CEditListItemPtr pPagerItem = GetPagerEditItem();
    UpdateEditValueByName(pPagerItem, pContact, kszGbBookpager);

    CEditListItemPtr pFaxItem = GetFaxEditItem();
    UpdateEditValueByName(pFaxItem, pContact, kszGbBookFax);

    CEditListItemPtr pEmailItem = GetEmailEditItem();
    if (NULL != pEmailItem)
    {
        pEmailItem->SetValue(pContact->m_strEmail);
    }

    CEditListItemPtr pConfUrlItem = GetConfUrlEditItem();
    UpdateEditValueByName(pConfUrlItem, pContact, kszGbconferenceURL);

    CEditListItemPtr pPriContactItem = GetPriContactEditItem();
    UpdateEditValueByName(pPriContactItem, pContact, kszGbBookprimaryContact);

    return true;
}

void CGenbandDetailLayout::UpdateEditValueByName(CEditListItem* pEditItem, ContactGBBookUnit * pContact, const yl::string& strName)
{
    if (NULL == pEditItem || NULL == pContact)
    {
        return;
    }

    yl::string strTemp = "";
    pContact->GetNumberByName(strName, strTemp);
    pEditItem->SetValue(strTemp);
}

bool CGenbandDetailLayout::UpdateGroup(int nGroupId, DirGroupRecord * pGroup)
{
    if (NULL == pGroup || nGroupId == m_nDetailId)
    {
        return false;
    }

    m_nDetailId = nGroupId;

    CEditListItemPtr pNameItem = GetNameEditItem();

    if (NULL != pNameItem)
    {
        pNameItem->SetValue(pGroup->GetDisplayName());
    }

    return true;
}

#endif
