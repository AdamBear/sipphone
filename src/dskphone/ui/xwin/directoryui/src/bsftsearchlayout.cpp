#include "bsftsearchlayout.h"

#include "translateiddefine.h"

#include "baseui/include/baseuicommon.h"
#include "baseui/include/comboboxlistitem.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "baseui/include/inputmethod.h"
#include "imeedit/imecommon.h"
#include "languagehelper/modlangtranslate.h"


CBSFTSearchLayout::CBSFTSearchLayout()
    : CDetailLayout(DETAIL_LIST_ITEM_LAYOUT_BSFT_SEARCH)
{
}

CBSFTSearchLayout::~CBSFTSearchLayout()
{
}

void CBSFTSearchLayout::Init()
{
    if (0 != m_vecListItem.size())
    {
        ClearEdit();
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

void CBSFTSearchLayout::InitFirstNameCtrl()
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CEditListItem::GetEditListItemType());
    CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(pItem);

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetShowIndex(false);
    pEditItem->SetTitle(_LANG(TRID_FIRST_NAME));
    pEditItem->InstallIME(IME_Name, IME_Abc);
    pEditItem->SetMaxLength(MAX_LEN_DISPLAY_NAME);

    m_vecListItem.push_back(pEditItem);
}

void CBSFTSearchLayout::InitLastNameCtrl()
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CEditListItem::GetEditListItemType());
    CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(pItem);

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetShowIndex(false);
    pEditItem->SetTitle(_LANG(TRID_LAST_NAME));
    pEditItem->InstallIME(IME_Name, IME_Abc);
    pEditItem->SetMaxLength(MAX_LEN_DISPLAY_NAME);

    m_vecListItem.push_back(pEditItem);
}

void CBSFTSearchLayout::InitNumberCtrl()
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CEditListItem::GetEditListItemType());
    CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(pItem);

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetShowIndex(false);
    pEditItem->SetTitle(_LANG(TRID_NUMBER));
    pEditItem->InstallIME(IME_DIAL, IME_123_Dial);
    pEditItem->SetMaxLength(MAX_LEN_PHONE_NUMBER_LONG);

    m_vecListItem.push_back(pEditItem);
}

void CBSFTSearchLayout::InitExtensionCtrl()
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CEditListItem::GetEditListItemType());
    CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(pItem);

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetShowIndex(false);
    pEditItem->SetTitle(_LANG(TRID_EXTENSION_NUMBER));
    pEditItem->InstallIME(IME_DIAL, IME_123_Dial);
    pEditItem->SetMaxLength(MAX_LEN_PHONE_NUMBER_LONG);

    m_vecListItem.push_back(pEditItem);
}

void CBSFTSearchLayout::InitMobileCtrl()
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CEditListItem::GetEditListItemType());
    CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(pItem);

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetShowIndex(false);
    pEditItem->SetTitle(_LANG(TRID_MOBILE_NUMBER));
    pEditItem->InstallIME(IME_DIAL, IME_123_Dial);
    pEditItem->SetMaxLength(MAX_LEN_PHONE_NUMBER_LONG);

    m_vecListItem.push_back(pEditItem);
}

void CBSFTSearchLayout::InitDepartmentCtrl()
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CEditListItem::GetEditListItemType());
    CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(pItem);

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetShowIndex(false);
    pEditItem->SetTitle(_LANG(TRID_DEPARTMENT));
    pEditItem->InstallIME(IME_Name, IME_Abc);
    pEditItem->SetMaxLength(MAX_LEN_DISPLAY_NAME);

    m_vecListItem.push_back(pEditItem);
}

void CBSFTSearchLayout::InitEMailCtrl()
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CEditListItem::GetEditListItemType());
    CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(pItem);

    if (NULL == pEditItem)
    {
        return;
    }

    pEditItem->SetShowIndex(false);
    pEditItem->SetTitle(_LANG(TRID_EMAIL));
    pEditItem->InstallIME(IME_Name, IME_Abc);
    pEditItem->SetMaxLength(MAX_LEN_DISPLAY_NAME);

    m_vecListItem.push_back(pEditItem);
}

void CBSFTSearchLayout::InitGroupCtrl()
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CComboBoxListItem::GetComboBoxListItemType());
    CComboBoxListItemPtr pComboBoxItem = CComboBoxListItem::GetComboBoxListItem(pItem);

    if (NULL == pComboBoxItem)
    {
        return;
    }

    pComboBoxItem->SetShowIndex(false);
    pComboBoxItem->SetTitle(_LANG(TRID_GROUP));

    m_vecListItem.push_back(pComboBoxItem);
}

void CBSFTSearchLayout::ReturnAllEdit()
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

void CBSFTSearchLayout::ClearEdit()
{
    CEditListItemPtr pFirstNameItem = GetFirstNameItem();

    if (NULL != pFirstNameItem)
    {
        pFirstNameItem->SetValue(yl::string(""));
    }

    CEditListItemPtr pLastNameItem = GetLastNameItem();

    if (NULL != pLastNameItem)
    {
        pLastNameItem->SetValue(yl::string(""));
    }

    CEditListItemPtr pNumberItem = GetNumberItem();

    if (NULL != pNumberItem)
    {
        pNumberItem->SetValue(yl::string(""));
    }

    CEditListItemPtr pExtensionItem = GetExtensionItem();

    if (NULL != pExtensionItem)
    {
        pExtensionItem->SetValue(yl::string(""));
    }

    CEditListItemPtr pMobileItem = GetMobileItem();

    if (NULL != pMobileItem)
    {
        pMobileItem->SetValue(yl::string(""));
    }

    CEditListItemPtr pDepartmentItem = GetDepartmentItem();

    if (NULL != pDepartmentItem)
    {
        pDepartmentItem->SetValue(yl::string(""));
    }

    CEditListItemPtr pEMailItem = GetEmailItem();

    if (NULL != pEMailItem)
    {
        pEMailItem->SetValue(yl::string(""));
    }
}

CEditListItem * CBSFTSearchLayout::GetFirstNameItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_FIRST_NAME);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return static_cast<CEditListItem *>(pDetailItem);
    }

    return NULL;
}

CEditListItem * CBSFTSearchLayout::GetLastNameItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_LAST_NAME);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return static_cast<CEditListItem *>(pDetailItem);
    }

    return NULL;
}

CEditListItem * CBSFTSearchLayout::GetNumberItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_NUMBER);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return static_cast<CEditListItem *>(pDetailItem);
    }

    return NULL;
}

CEditListItem * CBSFTSearchLayout::GetExtensionItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_EXTENSION);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return static_cast<CEditListItem *>(pDetailItem);
    }

    return NULL;
}

CEditListItem * CBSFTSearchLayout::GetMobileItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_MOBILE);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return static_cast<CEditListItem *>(pDetailItem);
    }

    return NULL;
}

CEditListItem * CBSFTSearchLayout::GetDepartmentItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_DEPARTMENT);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return static_cast<CEditListItem *>(pDetailItem);
    }

    return NULL;
}

CEditListItem * CBSFTSearchLayout::GetEmailItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_EMAIL);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return static_cast<CEditListItem *>(pDetailItem);
    }

    return NULL;
}

CComboBoxListItem * CBSFTSearchLayout::GetGroupItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BS_SEARCH_ITEM_INDEX_GROUP);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        return static_cast<CComboBoxListItem *>(pDetailItem);
    }

    return NULL;
}
