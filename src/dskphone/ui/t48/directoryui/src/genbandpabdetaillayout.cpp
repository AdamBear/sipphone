#include <QtGui>
#include "baseui/comboboxlistitem.h"
#include "baseui/detaillistitem.h"
#include "baseui/editlistitem.h"
#include "uimanager/basedialog.h"

#include "genbandpabdetaillayout.h"

#include "directorycommon.h"
//#include "directorystring.h"
#include "moddirectoryui.h"
#include "uicommon.h"
#include "account/include/modaccount.h"
#include "commonunits/modcommonunits.h"
#include "commonunits/commonunits_def.h"
#include "inputmethod/inputmethod.h"
#include "baseui/t4xpicpath.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/languagehelper.h"
#include <resource/modresource.h>
#if IF_FEATURE_GBBOOK

typedef YLSmartPtr<BaseResItem> CPhotoDataPtr;
typedef YLList<CPhotoDataPtr> LIST_PHOTO;


CGenbandDetailLayout::CGenbandDetailLayout(CBaseDialog * pDlg, bool bNewContact, bool bCopyLayout)
    : CDetailListItemLayout(DETAIL_LIST_ITEM_LAYOUT_GENBAND, pDlg)
    , m_bAddition(bNewContact)
    , m_bCopyLayout(bCopyLayout)
    , m_nDetailId(knInvalidRId)
{
}

CGenbandDetailLayout::~CGenbandDetailLayout(void)
{
}

bool CGenbandDetailLayout::IsGenbandDetailLayout(CDetailListItemLayoutPtr pLayout)
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


void CGenbandDetailLayout::Init()
{
    InitNameCtrl();
    InitGroupCtrl();

    InitNumberCtrl(TRID_PRIMARYCONTACT);

    InitNameCtrl(TRID_FIRST_NAME);
    InitNameCtrl(TRID_LAST_NAME);

    InitNumberCtrl(TRID_BUSINESS_NUMBER);
    InitNumberCtrl(TRID_HOME_NUMBER);
    InitNumberCtrl(TRID_MOBILE_NUMBER);
    InitNumberCtrl(TRID_PAGER);
    InitNumberCtrl(TRID_FAX);

    InitNameCtrl(TRID_EMAIL);

    InitNumberCtrl(TRID_CONFERENCE_URL);

}

// 初始化Name
void CGenbandDetailLayout::InitNameCtrl()
{
    CEditListItemPtr pEditListItem = new CEditListItem;

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetTitle(LANG_TRANSLATE(TRID_NICK_NAME));

    m_vecListItem.push_back(pEditListItem);

    QLineEdit * pEdit = pEditListItem->GetLineEdit();

    if (NULL != pEdit)
    {
        DirUI_SetNameIME(pEdit);

        qLineEditUtility::SetMaxLength(pEdit, EDIT_TYPE_CONTACT_NAME);

        if (FullKeyboardEnabled())
        {
            pEditListItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_CUSTOM, IME_Abc);
        }

        if (NULL != m_pDlg)
        {
            pEdit->installEventFilter(m_pDlg);
        }
    }
}

// 初始化Name
void CGenbandDetailLayout::InitNameCtrl(yl::string strNameTitle)
{
    CEditListItemPtr pEditListItem = new CEditListItem;

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetTitle(LANG_TRANSLATE(strNameTitle.c_str()));
    m_vecListItem.push_back(pEditListItem);

    QLineEdit * pEdit = pEditListItem->GetLineEdit();

    if (NULL != pEdit)
    {
        DirUI_SetNameIME(pEdit);

        qLineEditUtility::SetMaxLength(pEdit, EDIT_TYPE_CONTACT_NAME);

        if (FullKeyboardEnabled())
        {
            pEditListItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_CUSTOM, IME_Abc);
        }

        if (NULL != m_pDlg)
        {
            pEdit->installEventFilter(m_pDlg);
        }
    }
}

// 初始化Number
void CGenbandDetailLayout::InitNumberCtrl(yl::string strNumTitle)
{
#if 0
    if (m_bCopyLayout)
    {
        CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

        if (NULL == pComboBoxItem)
        {
            return;
        }

        pComboBoxItem->SetTitle(LANG_TRANSLATE(strNumTitle.c_str()));
        pComboBoxItem->SetShowIndex(true);
        m_vecListItem.push_back(pComboBoxItem);

        qComboBoxEx * pComboBox = pComboBoxItem->GetComboBox();

        if (NULL != pComboBox && NULL != m_pDlg)
        {
            pComboBox->installEventFilter(m_pDlg);
        }
        return;
    }
#endif

    CEditListItemPtr pEditListItem = new CEditListItem;

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetTitle(LANG_TRANSLATE(strNumTitle.c_str()));
    m_vecListItem.push_back(pEditListItem);

    QLineEdit * pEdit = pEditListItem->GetLineEdit();

    if (NULL != pEdit)
    {
        pEditListItem->SetInputInfo(EDIT_TYPE_CONTACT_NUMBER, IME_DIAL, IME_123_Dial);

        if (NULL != m_pDlg)
        {
            pEdit->installEventFilter(m_pDlg);
        }
    }
}



// 初始化群组ComboBox
void CGenbandDetailLayout::InitGroupCtrl()
{
    CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

    if (NULL == pComboBoxItem)
    {
        return;
    }

    pComboBoxItem->SetTitle(LANG_TRANSLATE(TRID_GROUP));
    m_vecListItem.push_back(pComboBoxItem);

    qComboBoxEx * pComboBox = pComboBoxItem->GetComboBox();

    if (NULL != pComboBox && NULL != m_pDlg)
    {
        pComboBox->installEventFilter(m_pDlg);
    }
}



CEditListItem * CGenbandDetailLayout::GetNameEditItem()
{
    int nItemIndex = -1;

    nItemIndex = GENBAND_CONTACT_DETAIL_ITEM_INDEX_NICK_NAME;

    CDetailListItemBasePtr pDetailItem = GetDetailListItem(nItemIndex);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CGenbandDetailLayout::GetFirstNameEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(
            GENBAND_CONTACT_DETAIL_ITEM_INDEX_FIRST_NAME);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CGenbandDetailLayout::GetLastNameEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(GENBAND_CONTACT_DETAIL_ITEM_INDEX_LAST_NAME);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CGenbandDetailLayout::GetBusinessNoEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(
            GENBAND_CONTACT_DETAIL_ITEM_INDEX_BUSINESS_NO);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CGenbandDetailLayout::GetMobileNoEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(GENBAND_CONTACT_DETAIL_ITEM_INDEX_MOBILE_NO);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CGenbandDetailLayout::GetHomeNoEditItem()
{

    CDetailListItemBasePtr pDetailItem = GetDetailListItem(GENBAND_CONTACT_DETAIL_ITEM_INDEX_HOME_NO);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CGenbandDetailLayout::GetPagerEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(GENBAND_CONTACT_DETAIL_ITEM_INDEX_PAGER);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CGenbandDetailLayout::GetFaxEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(GENBAND_CONTACT_DETAIL_ITEM_INDEX_FAX);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CGenbandDetailLayout::GetEmailEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(GENBAND_CONTACT_DETAIL_ITEM_INDEX_EMAIL);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CGenbandDetailLayout::GetConfUrlEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(GENBAND_CONTACT_DETAIL_ITEM_INDEX_CONF_URL);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CGenbandDetailLayout::GetPriContactEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(
            GENBAND_CONTACT_DETAIL_ITEM_INDEX_PRIMARY_CONTACT);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CComboBoxListItem * CGenbandDetailLayout::GetGroupComboBoxItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(GENBAND_CONTACT_DETAIL_ITEM_INDEX_GROUP);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}


bool CGenbandDetailLayout::IsCopyLayout() const
{
    return m_bCopyLayout;
}

bool CGenbandDetailLayout::IsAdditionLayout() const
{
    return m_bAddition;
}

void CGenbandDetailLayout::ReturnAllEdit()
{
    //设置处于预输入状态的字符也有效
    CEditListItemPtr pNameEditItem = GetNameEditItem();
    if (NULL != pNameEditItem)
    {
        pNameEditItem->ReturnAllInput();
    }

    CEditListItemPtr pFirstNameEditItem = GetFirstNameEditItem();
    if (NULL != pFirstNameEditItem)
    {
        pFirstNameEditItem->ReturnAllInput();
    }

    CEditListItemPtr pLastNameEditItem = GetLastNameEditItem();
    if (NULL != pLastNameEditItem)
    {
        pLastNameEditItem->ReturnAllInput();
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

    CEditListItemPtr pPagerEditItem = GetPagerEditItem();
    if (NULL != pPagerEditItem)
    {
        pPagerEditItem->ReturnAllInput();
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

    CEditListItemPtr pConfUrlEditItem = GetConfUrlEditItem();
    if (NULL != pConfUrlEditItem)
    {
        pConfUrlEditItem->ReturnAllInput();
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
        pNameItem->SetValue(toQString(pContact->GetDisplayName(), true));
    }

    CComboBoxListItemPtr pGroupItem = GetGroupComboBoxItem();

    if (NULL != pGroupItem)
    {
        pGroupItem->SetItemData(pContact->m_nParentId);
    }

    CEditListItemPtr pFirstNameItem = GetFirstNameEditItem();
    if (NULL != pFirstNameItem)
    {
        pFirstNameItem->SetValue(toQString(pContact->m_strFirstName, true));
    }

    CEditListItemPtr pLastNameItem = GetLastNameEditItem();
    if (NULL != pLastNameItem)
    {
        pLastNameItem->SetValue(toQString(pContact->m_strLastName, true));
    }

    CEditListItemPtr pBussnessNoItem = GetBusinessNoEditItem();

    if (NULL != pBussnessNoItem)
    {
        yl::string strBussnessNo;

        pContact->GetNumberByName(kszGbBookbusinessNumber, strBussnessNo);
        pBussnessNoItem->SetValue(toQString(strBussnessNo, true));
    }

    CEditListItemPtr pHomeNoItem = GetHomeNoEditItem();

    if (NULL != pHomeNoItem)
    {
        yl::string strHomeNo;

        pContact->GetNumberByName(kszGbBookhomeNumber, strHomeNo);
        pHomeNoItem->SetValue(toQString(strHomeNo, true));
    }

    CEditListItemPtr pMobileNoItem = GetMobileNoEditItem();

    if (NULL != pMobileNoItem)
    {
        yl::string strMobileNo;

        pContact->GetNumberByName(kszGbBookmobile, strMobileNo);
        pMobileNoItem->SetValue(toQString(strMobileNo, true));
    }

    CEditListItemPtr pPagerItem = GetPagerEditItem();
    if (NULL != pPagerItem)
    {
        yl::string strTemp;
        pContact->GetNumberByName(kszGbBookpager, strTemp);
        pPagerItem->SetValue(toQString(strTemp, true));
    }

    CEditListItemPtr pFaxItem = GetFaxEditItem();
    if (NULL != pFaxItem)
    {
        yl::string strTemp;
        pContact->GetNumberByName(kszGbBookFax, strTemp);
        pFaxItem->SetValue(toQString(strTemp, true));
    }

    CEditListItemPtr pEmailItem = GetEmailEditItem();
    if (NULL != pEmailItem)
    {
        pEmailItem->SetValue(toQString(pContact->m_strEmail, true));
    }

    CEditListItemPtr pConfUrlItem = GetConfUrlEditItem();
    if (NULL != pConfUrlItem)
    {
        yl::string strTemp;
        pContact->GetNumberByName(kszGbconferenceURL, strTemp);
        pConfUrlItem->SetValue(toQString(strTemp, true));
    }

    CEditListItemPtr pPriContactItem = GetPriContactEditItem();
    if (NULL != pPriContactItem)
    {
        yl::string strTemp;
        pContact->GetNumberByName(kszGbBookprimaryContact, strTemp);
        pPriContactItem->SetValue(toQString(strTemp, true));
    }

    return true;
}

bool CGenbandDetailLayout::UpdateGroup(int nGroupId, DirGroupRecord * pGroup)
{
    if (NULL == pGroup)
    {
        return false;
    }

    if (nGroupId == m_nDetailId)
    {
        return true;
    }

    m_nDetailId = nGroupId;

    CEditListItemPtr pNameItem = GetNameEditItem();

    if (NULL != pNameItem)
    {
        pNameItem->SetValue(toQString(pGroup->GetDisplayName(), true));
    }

    return true;
}

bool CGenbandDetailLayout::IsInputEmpty()
{
    CEditListItemPtr pNameEditItem = GetNameEditItem();

    if (NULL == pNameEditItem)
    {
        return true;
    }

    QString strName = pNameEditItem->GetValue().trimmed();

    return strName.isEmpty();
}

#endif
