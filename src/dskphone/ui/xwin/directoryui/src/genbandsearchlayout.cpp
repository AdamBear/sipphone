#if IF_FEATURE_GBBOOK
#include "genbandsearchlayout.h"
#include "directorycommon.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/baseuicommon.h"
#include "baseui/include/listviewitemfactory.h"
#include "baseui/include/inputmethod.h"
#include "imeedit/imecommon.h"
#include "contacts/directory/include/moddirinterface.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CGenbandSearchLayout::CGenbandSearchLayout()
    : CDetailLayout(DETAIL_LIST_ITEM_LAYOUT_GENBAND_SEARCH)
{
}

CGenbandSearchLayout::~CGenbandSearchLayout()
{
}

void CGenbandSearchLayout::Init(int eMode)
{
    ReleaseAllItem();

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

void CGenbandSearchLayout::InitNameCtrl(int eSearchType)
{
    xListViewItem* pItem = g_ItemFactory.ApplyItem(CEditListItem::GetEditListItemType());
    CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(pItem);

    if (NULL == pEditItem)
    {
        return;
    }

    yl::string strTitle = GetTitleByType(eSearchType);
    pEditItem->SetTitle(_LANG(strTitle));
    pEditItem->SetShowIndex(false);
    pEditItem->InstallIME(IME_Name, IME_Abc);
    pEditItem->SetMaxLength(MAX_LEN_DISPLAY_NAME);

    if (GAB_GetCurrentGabOptType() == eSearchType)
    {
        pEditItem->SetValue(GAB_GetCurentGabKey());
    }

    m_vecListItem.push_back(pEditItem);
}


void CGenbandSearchLayout::InitNumberCtrl()
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

    if (GAB_SEARCH_NUMBER == GAB_GetCurrentGabOptType())
    {
        pEditItem->SetValue(GAB_GetCurentGabKey());
    }

    m_vecListItem.push_back(pEditItem);
}

yl::string CGenbandSearchLayout::GetTitleByType(int eSearchType)
{
    yl::string strTitle = "";
    switch (eSearchType)
    {
    case GAB_SEARCH_NAME:
        {
            strTitle = TRID_NAME;
        }
        break;
    case GAB_SEARCH_FIRSTNAME:
        {
            strTitle = TRID_FIRST_NAME;
        }
        break;
    case GAB_SEARCH_LASTNAME:
        {
            strTitle = TRID_LAST_NAME;
        }
        break;
    default:
        break;
    }

    return strTitle;
}

void CGenbandSearchLayout::ReturnAllEdit()
{
    CEditListItemPtr pFirstNameItem = GetFirstItem();

    if (NULL != pFirstNameItem)
    {
        pFirstNameItem->ReturnAllInput();
    }
}

void CGenbandSearchLayout::SetSearchMode(int eMode)
{
    m_eSearchMode = eMode;
}

int CGenbandSearchLayout::GetSearchMode()
{
    return m_eSearchMode;
}

void CGenbandSearchLayout::ClearEdit()
{
    CEditListItemPtr pFirstNameItem = GetFirstItem();

    if (NULL != pFirstNameItem)
    {
        pFirstNameItem->SetValue("");
    }
}

CEditListItem * CGenbandSearchLayout::GetFirstItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(0);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}


#endif
