#if IF_FEATURE_GBBOOK
#include "genbandsearchlayout.h"
#include "translateiddefine.h"
#include "baseui/comboboxlistitem.h"
#include "baseui/editlistitem.h"
#include "inputmethod/inputmethod.h"
#include "uimanager/basedialog.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qlineeditutility.h"


#define GENBAND_SUPER_SEARCH_ITEM_HEIGHT 46
#define GENBAND_DETAIL_TITLE_WIDHT 130


CGenbandSearchLayout::CGenbandSearchLayout(CBaseDialog * pDlg)
    : CDetailListItemLayout(DETAIL_LIST_ITEM_LAYOUT_GENBAND_SEARCH, pDlg)
{
}

CGenbandSearchLayout::~CGenbandSearchLayout()
{
}

void CGenbandSearchLayout::Init()
{
    if (0 != m_vecListItem.size())
    {
        return;
    }

    InitSearchTypeCtrl();
    InitNameCtrl();
}

void CGenbandSearchLayout::Init(GBBOOK_OPERATION_TYPE eMode)
{
    if (0 != m_vecListItem.size())
    {
        return;
    }

    switch (eMode)
    {
    case GAB_SEARCH_NAME:
    case GAB_SEARCH_FIRSTNAME:
    case GAB_SEARCH_LASTNAME:
        {
            InitNameCtrl(eMode);
        }
        break;
    case GAB_SEARCH_NUMBER:
        {
            InitNumberCtrl();
        }
        break;
    default:
        break;
    }
}

void CGenbandSearchLayout::InitNameCtrl()
{
    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_GENBAND_SEARCH_KEY));

    pEditItem->SetItemHeight(GENBAND_SUPER_SEARCH_ITEM_HEIGHT);
    pEditItem->SetTitleWidth(GENBAND_DETAIL_TITLE_WIDHT);
    pEditItem->SetLayout(true);

    pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_Name, IME_Abc);

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CGenbandSearchLayout::InitNameCtrl(GBBOOK_OPERATION_TYPE eSearchType)
{
    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    switch (eSearchType)
    {
    case GAB_SEARCH_NAME:
        {
            pEditItem->SetTitle(LANG_TRANSLATE(TRID_NAME));
        }
        break;
    case GAB_SEARCH_FIRSTNAME:
        {
            pEditItem->SetTitle(LANG_TRANSLATE(TRID_FIRST_NAME));
        }
        break;
    case GAB_SEARCH_LASTNAME:
        {
            pEditItem->SetTitle(LANG_TRANSLATE(TRID_LAST_NAME));
        }
        break;
    }

    pEditItem->SetItemHeight(GENBAND_SUPER_SEARCH_ITEM_HEIGHT);
    pEditItem->SetTitleWidth(GENBAND_DETAIL_TITLE_WIDHT);
    pEditItem->SetLayout(true);

    pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NAME, IME_Name, IME_Abc);

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}


void CGenbandSearchLayout::InitNumberCtrl()
{
    CEditListItemPtr pEditItem = new CEditListItem;

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetTitle(LANG_TRANSLATE(TRID_NUMBER));
    pEditItem->SetItemHeight(GENBAND_SUPER_SEARCH_ITEM_HEIGHT);
    pEditItem->SetTitleWidth(GENBAND_DETAIL_TITLE_WIDHT);
    pEditItem->SetLayout(true);

    pEditItem->SetInputInfo(EDIT_TYPE_CONTACT_NUMBER, IME_DIAL, IME_123_Dial);

    if (NULL != m_pDlg)
    {
        pEditItem->InstallEditFilter(m_pDlg);
    }

    m_vecListItem.push_back(pEditItem);
}

void CGenbandSearchLayout::InitSearchTypeCtrl()
{
    CComboBoxListItemPtr pComboBoxItem = new CComboBoxListItem;

    if (NULL == pComboBoxItem)
    {
        return;
    }

    pComboBoxItem->SetTitle(LANG_TRANSLATE(TRID_GENBAND_SEARCH_TYPE));

    pComboBoxItem->SetItemHeight(GENBAND_SUPER_SEARCH_ITEM_HEIGHT);
    pComboBoxItem->SetTitleWidth(GENBAND_DETAIL_TITLE_WIDHT);
    pComboBoxItem->SetLayout(true);

    if (NULL != m_pDlg)
    {
        pComboBoxItem->InstallComboBoxFilter(m_pDlg);
    }

    for (int i = GAB_SEARCH_NAME; i <= GAB_SEARCH_NUMBER; ++i)
    {
        QString strDisplayName;
        GetNameBySearchType((GBBOOK_OPERATION_TYPE)i, strDisplayName);
        pComboBoxItem->AddValue(strDisplayName, QVariant::fromValue<int>(i));
    }

    m_vecListItem.push_back(pComboBoxItem);
}


void CGenbandSearchLayout::ReturnAllEdit()
{
    //设置处于预输入状态的字符也有效
    CEditListItemPtr pFirstNameItem = GetSearchKeyItem();

    if (NULL != pFirstNameItem)
    {
        pFirstNameItem->ReturnAllInput();
    }

}

void CGenbandSearchLayout::ClearEdit()
{
    CEditListItemPtr pFirstNameItem = GetSearchKeyItem();


    if (NULL != pFirstNameItem)
    {
        pFirstNameItem->SetValue(QLatin1String(""));
    }
}

CEditListItem * CGenbandSearchLayout::GetSearchKeyItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(GB_SEARCH_ITEM_INDEX_NAME);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CComboBoxListItem * CGenbandSearchLayout::GetSearchTypeItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(GB_SEARCH_ITEM_INDEX_SEARCH_TYPE);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return (CComboBoxListItem *)pDetailItem;
    }

    return NULL;
}


void CGenbandSearchLayout::GetNameBySearchType(GBBOOK_OPERATION_TYPE eSearchType,
        QString & strDisName)
{
    switch (eSearchType)
    {
    case GAB_SEARCH_NAME:
        strDisName = LANG_TRANSLATE(TRID_GENBAND_SEARCH_BY_NAME);
        break;
    case GAB_SEARCH_FIRSTNAME:
        strDisName = LANG_TRANSLATE(TRID_GENBAND_SEARCH_BY_FIRST_NAME);
        break;
    case GAB_SEARCH_LASTNAME:
        strDisName = LANG_TRANSLATE(TRID_GENBAND_SEARCH_BY_LAST_NAME);
        break;
    case GAB_SEARCH_NUMBER:
        strDisName = LANG_TRANSLATE(TRID_GENBAND_SEARCH_BY_NUMBER);
        break;
    }
}

#endif