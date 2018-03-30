#include <QtGui>
#include "baseui/comboboxlistitem.h"
#include "baseui/editlistitem.h"
#include "uimanager/basedialog.h"

#include "mtswdetaillayout.h"

#include "directorycommon.h"
#include "translateiddefine.h"
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


typedef YLSmartPtr<BaseResItem> CResDataPtr;
typedef YLList<CResDataPtr> LIST_RESOURCE;


CMTSWDetailLayout::CMTSWDetailLayout(CBaseDialog * pDlg, bool bNewContact, bool bCopyLayout)
    : CDetailListItemLayout(DETAIL_LIST_ITEM_LAYOUT_MTSW_CONTACT, pDlg)
    , m_bNewContact(bNewContact)
    , m_bCopyLayout(bCopyLayout)
    , m_nDetailId(knInvalidRId)
{
}

CMTSWDetailLayout::~CMTSWDetailLayout(void)
{
}

bool CMTSWDetailLayout::IsMTSWDetailLayout(CDetailListItemLayoutPtr pLayout)
{
    if (NULL != pLayout && DETAIL_LIST_ITEM_LAYOUT_MTSW_CONTACT == pLayout->GetType())
    {
        return true;
    }

    return false;
}

void CMTSWDetailLayout::Init()
{
    InitFirstNameCtrl();
    InitLastNameCtrl();
    InitNickNameCtrl();
    InitJobTiTleCtrl();
    InitOrgCtrl();

    InitHomeNumberCtrl();
    InitWorkNumberCtrl();
    InitMobileCtrl();
    InitFaxNumberCtrl();
    InitOtherNumberCtrl();
}

void CMTSWDetailLayout::InitFirstNameCtrl()
{
    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_FIRST_NAME));
    //pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_Name, IME_Abc);
    //pEditItem->SetShowIndex(true);

    QLineEdit * pEdit = pEditItem->GetLineEdit();

    if (NULL != pEdit)
    {
        DirUI_SetNameIME(pEdit);
        qLineEditUtility::SetMaxLength(pEdit, EDIT_TYPE_CONTACT_NAME);

        if (FullKeyboardEnabled())
        {
            pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_CUSTOM, IME_Abc);
        }
    }

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CMTSWDetailLayout::InitLastNameCtrl()
{
    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_LAST_NAME));
    //pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_Name, IME_Abc);
    //pEditItem->SetShowIndex(true);

    QLineEdit * pEdit = pEditItem->GetLineEdit();

    if (NULL != pEdit)
    {
        DirUI_SetNameIME(pEdit);
        qLineEditUtility::SetMaxLength(pEdit, EDIT_TYPE_CONTACT_NAME);
        if (FullKeyboardEnabled())
        {
            pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_CUSTOM, IME_Abc);
        }
    }

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CMTSWDetailLayout::InitNickNameCtrl()
{
    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_NICK_NAME));
    //pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_Name, IME_Abc);
    //pEditItem->SetShowIndex(true);

    QLineEdit * pEdit = pEditItem->GetLineEdit();

    if (NULL != pEdit)
    {
        DirUI_SetNameIME(pEdit);
        qLineEditUtility::SetMaxLength(pEdit, EDIT_TYPE_CONTACT_NAME);
        if (FullKeyboardEnabled())
        {
            pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_CUSTOM, IME_Abc);
        }
    }

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CMTSWDetailLayout::InitJobTiTleCtrl()
{
    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_MTSW_JOBTITLE));
    //pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_Name, IME_Abc);
    //pEditItem->SetShowIndex(true);

    QLineEdit * pEdit = pEditItem->GetLineEdit();

    if (NULL != pEdit)
    {
        DirUI_SetNameIME(pEdit);
        qLineEditUtility::SetMaxLength(pEdit, EDIT_TYPE_CONTACT_NAME);
        if (FullKeyboardEnabled())
        {
            pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_CUSTOM, IME_Abc);
        }
    }

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CMTSWDetailLayout::InitOrgCtrl()
{
    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_MTSW_ORGANISATION));
    //pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_Name, IME_Abc);
    //pEditItem->SetShowIndex(true);

    QLineEdit * pEdit = pEditItem->GetLineEdit();

    if (NULL != pEdit)
    {
        DirUI_SetNameIME(pEdit);
        qLineEditUtility::SetMaxLength(pEdit, EDIT_TYPE_CONTACT_NAME);
        if (FullKeyboardEnabled())
        {
            pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_CUSTOM, IME_Abc);
        }
    }

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CMTSWDetailLayout::InitHomeNumberCtrl()
{
    if (m_bCopyLayout)
    {
        CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

        if (NULL == pComboBoxItem)
        {
            return;
        }

        pComboBoxItem->SetTitle(LANG_TRANSLATE(TRID_MTSW_HOME_NUMBER));
        //pComboBoxItem->SetShowIndex(true);
        m_vecListItem.push_back(pComboBoxItem);

        qComboBoxEx * pComboBox = pComboBoxItem->GetComboBox();

        if (NULL != pComboBox && NULL != m_pDlg)
        {
            pComboBox->installEventFilter(m_pDlg);
        }
        return;
    }

    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_MTSW_HOME_NUMBER));
    pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NUMBER, IME_DIAL, IME_123_Dial);
    //pEditItem->SetShowIndex(true);

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CMTSWDetailLayout::InitWorkNumberCtrl()
{
    if (m_bCopyLayout)
    {
        CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

        if (NULL == pComboBoxItem)
        {
            return;
        }

        pComboBoxItem->SetTitle(LANG_TRANSLATE(TRID_MTSW_WORK_NUMBER));
        //pComboBoxItem->SetShowIndex(true);
        m_vecListItem.push_back(pComboBoxItem);

        qComboBoxEx * pComboBox = pComboBoxItem->GetComboBox();

        if (NULL != pComboBox && NULL != m_pDlg)
        {
            pComboBox->installEventFilter(m_pDlg);
        }
        return;
    }

    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_MTSW_WORK_NUMBER));
    pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NUMBER, IME_DIAL, IME_123_Dial);
    //pEditItem->SetShowIndex(true);

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CMTSWDetailLayout::InitMobileCtrl()
{
    if (m_bCopyLayout)
    {
        CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

        if (NULL == pComboBoxItem)
        {
            return;
        }

        pComboBoxItem->SetTitle(LANG_TRANSLATE(TRID_MOBILE_NUMBER));
        //pComboBoxItem->SetShowIndex(true);
        m_vecListItem.push_back(pComboBoxItem);

        qComboBoxEx * pComboBox = pComboBoxItem->GetComboBox();

        if (NULL != pComboBox && NULL != m_pDlg)
        {
            pComboBox->installEventFilter(m_pDlg);
        }
        return;
    }

    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_MOBILE_NUMBER));
    pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NUMBER, IME_DIAL, IME_123_Dial);
    //pEditItem->SetShowIndex(true);

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CMTSWDetailLayout::InitFaxNumberCtrl()
{
    if (0)
    {
        CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

        if (NULL == pComboBoxItem)
        {
            return;
        }

        pComboBoxItem->SetTitle(LANG_TRANSLATE(TRID_MTSW_FAX_NUMBER));
        //pComboBoxItem->SetShowIndex(true);
        m_vecListItem.push_back(pComboBoxItem);

        qComboBoxEx * pComboBox = pComboBoxItem->GetComboBox();

        if (NULL != pComboBox && NULL != m_pDlg)
        {
            pComboBox->installEventFilter(m_pDlg);
        }
        return;
    }

    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_MTSW_FAX_NUMBER));
    pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NUMBER, IME_DIAL, IME_123_Dial);
    //pEditItem->SetShowIndex(true);

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CMTSWDetailLayout::InitOtherNumberCtrl()
{
    if (0)
    {
        CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

        if (NULL == pComboBoxItem)
        {
            return;
        }

        pComboBoxItem->SetTitle(LANG_TRANSLATE(TRID_MTSW_OTHER_NUMBER));
        //pComboBoxItem->SetShowIndex(true);
        m_vecListItem.push_back(pComboBoxItem);

        qComboBoxEx * pComboBox = pComboBoxItem->GetComboBox();

        if (NULL != pComboBox && NULL != m_pDlg)
        {
            pComboBox->installEventFilter(m_pDlg);
        }
        return;
    }

    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_MTSW_OTHER_NUMBER));
    pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NUMBER, IME_DIAL, IME_123_Dial);
    //pEditItem->SetShowIndex(true);

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}


CEditListItem * CMTSWDetailLayout::GetFirstNameItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(MTSW_DETAIL_ITEM_INDEX_FIRST_NAME);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CMTSWDetailLayout::GetLastNameItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(MTSW_DETAIL_ITEM_INDEX_LAST_NAME);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CMTSWDetailLayout::GetOrgItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(MTSW_DETAIL_ITEM_INDEX_ORGANISATION);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CMTSWDetailLayout::GetNickNameItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(MTSW_DETAIL_ITEM_INDEX_NICK_NAME);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CMTSWDetailLayout::GetJobTitleItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(MTSW_DETAIL_ITEM_INDEX_JOBTITLE);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CMTSWDetailLayout::GetMobileItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(MTSW_DETAIL_ITEM_INDEX_MOBILE);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CMTSWDetailLayout::GetHomeNumberItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(MTSW_DETAIL_ITEM_INDEX_HOME_NUMBER);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CMTSWDetailLayout::GetWorkNumberItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(MTSW_DETAIL_ITEM_INDEX_WORK_NUMBER);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CMTSWDetailLayout::GetFaxNumberItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(MTSW_DETAIL_ITEM_INDEX_FAX_NUMBER);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CMTSWDetailLayout::GetOtherNumberItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(MTSW_DETAIL_ITEM_INDEX_OTHER_NUMBER);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

bool CMTSWDetailLayout::IsCopyLayout() const
{
    return m_bCopyLayout;
}

bool CMTSWDetailLayout::IsNewLayout() const
{
    return m_bNewContact;
}

void CMTSWDetailLayout::ReturnAllEdit()
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

bool CMTSWDetailLayout::UpdateContact(int nContactId, ContactMetaSwitchContactList * pContact)
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
        pFirstNameItem->SetValue(toQString(pContact->m_strFirstName, true));
    }

    CEditListItemPtr pLastNameItem = GetLastNameItem();

    if (NULL != pLastNameItem)
    {
        pLastNameItem->SetValue(toQString(pContact->m_strLastName, true));
    }

    CEditListItemPtr pNickNameItem = GetNickNameItem();

    if (NULL != pNickNameItem)
    {
        pNickNameItem->SetValue(toQString(pContact->m_strNickName, true));
    }

    CEditListItemPtr pJobItem = GetJobTitleItem();

    if (NULL != pJobItem)
    {
        pJobItem->SetValue(toQString(pContact->m_strJobTitle, true));
    }

    CEditListItemPtr pOrgItem = GetOrgItem();

    if (NULL != pOrgItem)
    {
        pOrgItem->SetValue(toQString(pContact->m_strOrganisation, true));
    }

    CEditListItemPtr pHomeNoItem = GetHomeNumberItem();

    if (NULL != pHomeNoItem)
    {
        yl::string strNo;

        pContact->GetNumberByName(kszMtswHomeNumber, strNo);
        pHomeNoItem->SetValue(toQString(strNo, true));
    }


    CEditListItemPtr pWorkNoItem = GetWorkNumberItem();

    if (NULL != pWorkNoItem)
    {
        yl::string strNo;

        pContact->GetNumberByName(kszMtswWorkNumber, strNo);
        pWorkNoItem->SetValue(toQString(strNo, true));
    }

    CEditListItemPtr pMobileNoItem = GetMobileItem();

    if (NULL != pMobileNoItem)
    {
        yl::string strMobileNo;

        pContact->GetNumberByName(kszMtswMobile, strMobileNo);
        pMobileNoItem->SetValue(toQString(strMobileNo, true));
    }

    CEditListItemPtr pFaxNoItem = GetFaxNumberItem();

    if (NULL != pFaxNoItem)
    {
        yl::string strNo;

        pContact->GetNumberByName(kszMtswFax, strNo);
        pFaxNoItem->SetValue(toQString(strNo, true));
    }

    CEditListItemPtr pOtherNoItem = GetOtherNumberItem();

    if (NULL != pOtherNoItem)
    {
        yl::string strOtherNo;

        pContact->GetNumberByName(kszMtswOtherNumber, strOtherNo);
        pOtherNoItem->SetValue(toQString(strOtherNo, true));
    }

    return true;
}

bool CMTSWDetailLayout::IsInputEmpty()
{
    CEditListItemPtr pFirstNameEditItem = GetFirstNameItem();
    CEditListItemPtr pLastNameEditItem = GetLastNameItem();

    if (NULL == pFirstNameEditItem || NULL == pLastNameEditItem)
    {
        return true;
    }

    QString strFirstName = pFirstNameEditItem->GetValue().trimmed();
    QString strLastName = pLastNameEditItem->GetValue().trimmed();

    return (strFirstName.isEmpty() && strLastName.isEmpty());

    return true;
}

CComboBoxListItem * CMTSWDetailLayout::GetHomeNumberComboBoxItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(MTSW_DETAIL_ITEM_INDEX_HOME_NUMBER);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}

CComboBoxListItem * CMTSWDetailLayout::GetWorkNumberComboBoxItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(MTSW_DETAIL_ITEM_INDEX_WORK_NUMBER);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}

CComboBoxListItem * CMTSWDetailLayout::GetMobileComboBoxItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(MTSW_DETAIL_ITEM_INDEX_MOBILE);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}

CComboBoxListItem * CMTSWDetailLayout::GetFaxNumberComboBoxItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(MTSW_DETAIL_ITEM_INDEX_FAX_NUMBER);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}

CComboBoxListItem * CMTSWDetailLayout::GetOtherNumberComboBoxItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(MTSW_DETAIL_ITEM_INDEX_OTHER_NUMBER);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}

