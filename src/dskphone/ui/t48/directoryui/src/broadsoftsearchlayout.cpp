#include "broadsoftsearchlayout.h"
#include "translateiddefine.h"
#include "baseui/comboboxlistitem.h"
#include "baseui/editlistitem.h"
#include "inputmethod/inputmethod.h"
#include "uimanager/basedialog.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qlineeditutility.h"


#define BROADSOFT_SUPER_SEARCH_ITEM_HEIGHT 46
#define BROADSOFT_DETAIL_TITLE_WIDHT 130


CBroadSoftSearchLayout::CBroadSoftSearchLayout(CBaseDialog * pDlg)
    : CDetailListItemLayout(DETAIL_LIST_ITEM_LAYOUT_BSFT_SEARCH, pDlg)
{
}

CBroadSoftSearchLayout::~CBroadSoftSearchLayout()
{
}

void CBroadSoftSearchLayout::Init()
{
    if (0 != m_vecListItem.size())
    {
        return;
    }

    InitFirstNameCtrl();
    InitLastNameCtrl();
    InitNumberCtrl();
    InitExtensionCtrl();
    InitMobileCtrl();
    InitDepartmentCtrl();
    InitEMailCtrl();
    InitGroupCtrl();
}

void CBroadSoftSearchLayout::InitFirstNameCtrl()
{
    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_FIRST_NAME));

    pEditItem->SetItemHeight(BROADSOFT_SUPER_SEARCH_ITEM_HEIGHT);
    pEditItem->SetTitleWidth(BROADSOFT_DETAIL_TITLE_WIDHT);
    pEditItem->SetLayout(true);

    pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_Name, IME_Abc);

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CBroadSoftSearchLayout::InitLastNameCtrl()
{
    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_LAST_NAME));

    pEditItem->SetItemHeight(BROADSOFT_SUPER_SEARCH_ITEM_HEIGHT);
    pEditItem->SetTitleWidth(BROADSOFT_DETAIL_TITLE_WIDHT);
    pEditItem->SetLayout(true);

    pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_Name, IME_Abc);

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CBroadSoftSearchLayout::InitNumberCtrl()
{
    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_NUMBER));

    pEditItem->SetItemHeight(BROADSOFT_SUPER_SEARCH_ITEM_HEIGHT);
    pEditItem->SetTitleWidth(BROADSOFT_DETAIL_TITLE_WIDHT);
    pEditItem->SetLayout(true);

    pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NUMBER, IME_DIAL, IME_123_Dial);

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CBroadSoftSearchLayout::InitExtensionCtrl()
{
    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_EXTENSION_NUMBER));

    pEditItem->SetItemHeight(BROADSOFT_SUPER_SEARCH_ITEM_HEIGHT);
    pEditItem->SetTitleWidth(BROADSOFT_DETAIL_TITLE_WIDHT);
    pEditItem->SetLayout(true);

    pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NUMBER, IME_DIAL, IME_123_Dial);

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CBroadSoftSearchLayout::InitMobileCtrl()
{
    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_MOBILE_NUMBER));

    pEditItem->SetItemHeight(BROADSOFT_SUPER_SEARCH_ITEM_HEIGHT);
    pEditItem->SetTitleWidth(BROADSOFT_DETAIL_TITLE_WIDHT);
    pEditItem->SetLayout(true);

    pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NUMBER, IME_DIAL, IME_123_Dial);

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CBroadSoftSearchLayout::InitDepartmentCtrl()
{
    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_DEPARTMENT));

    pEditItem->SetItemHeight(BROADSOFT_SUPER_SEARCH_ITEM_HEIGHT);
    pEditItem->SetTitleWidth(BROADSOFT_DETAIL_TITLE_WIDHT);
    pEditItem->SetLayout(true);

    pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_Name, IME_Abc);

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CBroadSoftSearchLayout::InitEMailCtrl()
{
    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_EMAIL));

    pEditItem->SetItemHeight(BROADSOFT_SUPER_SEARCH_ITEM_HEIGHT);
    pEditItem->SetTitleWidth(BROADSOFT_DETAIL_TITLE_WIDHT);
    pEditItem->SetLayout(true);

    pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_Name, IME_Abc);

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CBroadSoftSearchLayout::InitGroupCtrl()
{
    CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

    if (NULL == pComboBoxItem)
    {
        return;
    }

    pComboBoxItem->SetTitle(LANG_TRANSLATE(TRID_GROUP));

    pComboBoxItem->SetItemHeight(BROADSOFT_SUPER_SEARCH_ITEM_HEIGHT);
    pComboBoxItem->SetTitleWidth(BROADSOFT_DETAIL_TITLE_WIDHT);
    pComboBoxItem->SetLayout(true);

    if (NULL != m_pDlg)
    {
        pComboBoxItem->InstallComboBoxFilter(m_pDlg);
    }

    m_vecListItem.push_back(pComboBoxItem);
}

void CBroadSoftSearchLayout::ReturnAllEdit()
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

    CEditListItemPtr pNumberItem = GetNumberItem();

    if (NULL != pNumberItem)
    {
        pNumberItem->ReturnAllInput();
    }

    CEditListItemPtr pExtensionItem = GetExtensionItem();

    if (NULL != pExtensionItem)
    {
        pExtensionItem->ReturnAllInput();
    }

    CEditListItemPtr pMobileItem = GetMobileItem();

    if (NULL != pMobileItem)
    {
        pMobileItem->ReturnAllInput();
    }

    CEditListItemPtr pDepartmentItem = GetDepartmentItem();

    if (NULL != pDepartmentItem)
    {
        pDepartmentItem->ReturnAllInput();
    }

    CEditListItemPtr pEMailItem = GetEmailItem();

    if (NULL != pEMailItem)
    {
        pEMailItem->ReturnAllInput();
    }
}

void CBroadSoftSearchLayout::ClearEdit()
{
    CEditListItemPtr pFirstNameItem = GetFirstNameItem();

    if (NULL != pFirstNameItem)
    {
        pFirstNameItem->SetValue(QLatin1String(""));
    }

    CEditListItemPtr pLastNameItem = GetLastNameItem();

    if (NULL != pLastNameItem)
    {
        pLastNameItem->SetValue(QLatin1String(""));
    }

    CEditListItemPtr pNumberItem = GetNumberItem();

    if (NULL != pNumberItem)
    {
        pNumberItem->SetValue(QLatin1String(""));
    }

    CEditListItemPtr pExtensionItem = GetExtensionItem();

    if (NULL != pExtensionItem)
    {
        pExtensionItem->SetValue(QLatin1String(""));
    }

    CEditListItemPtr pMobileItem = GetMobileItem();

    if (NULL != pMobileItem)
    {
        pMobileItem->SetValue(QLatin1String(""));
    }

    CEditListItemPtr pDepartmentItem = GetDepartmentItem();

    if (NULL != pDepartmentItem)
    {
        pDepartmentItem->SetValue(QLatin1String(""));
    }

    CEditListItemPtr pEMailItem = GetEmailItem();

    if (NULL != pEMailItem)
    {
        pEMailItem->SetValue(QLatin1String(""));
    }
}

CEditListItem * CBroadSoftSearchLayout::GetFirstNameItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_FIRST_NAME);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CBroadSoftSearchLayout::GetLastNameItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_LAST_NAME);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CBroadSoftSearchLayout::GetNumberItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_NUMBER);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CBroadSoftSearchLayout::GetExtensionItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_EXTENSION);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CBroadSoftSearchLayout::GetMobileItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_MOBILE);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CBroadSoftSearchLayout::GetDepartmentItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_DEPARTMENT);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CBroadSoftSearchLayout::GetEmailItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_EMAIL);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CComboBoxListItem * CBroadSoftSearchLayout::GetGroupItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_GROUP);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}
