#include "bsftdetaillayout.h"
#include "directorycommon.h"
#include "directoryuicommon.h"

#include "translateiddefine.h"

#include "baseui/include/baseuicommon.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "baseui/include/inputmethod.h"
#include "imeedit/imecommon.h"
#include "languagehelper/modlangtranslate.h"


CBSFTDetailLayout::CBSFTDetailLayout()
    : CDetailLayout(DETAIL_LIST_ITEM_LAYOUT_LOCAL)
    , m_nAction(DIR_ACTION_NONE)
    , m_nDetailId(knInvalidRId)
{
}

CBSFTDetailLayout::~CBSFTDetailLayout()
{
}

void CBSFTDetailLayout::Init()
{
    ReleaseAllItem();

    InitNameEditItem();
    InitNumberEditItem();
}

void CBSFTDetailLayout::InitNameEditItem()
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CEditListItem::GetEditListItemType());
    CEditListItemPtr pEditListItem = CEditListItem::GetEditListItem(pItem);

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetDataIndex(BROADSOFT_CONTACT_DETAIL_ITEM_NAME);
    pEditListItem->SetShowIndex(true);
    pEditListItem->SetTitle(_LANG(TRID_NAME));
    pEditListItem->SetValue("");
    pEditListItem->SetMaxLength(MAX_LEN_BSFT_NAME);

    m_vecListItem.push_back(pEditListItem);

    yl::string strSupportIME;
    yl::string strIME;

    DIRUI_GetNameIME(strSupportIME, strIME);

    pEditListItem->InstallIME(strSupportIME.c_str(), strIME.c_str());
    pEditListItem->ConnectIMEChanged(this, static_cast<slotMethod>(&CDetailLayout::OnDetailIMEChanged));
}

void CBSFTDetailLayout::InitNumberEditItem()
{
    xListViewItem * pItem = g_ItemFactory.ApplyItem(CEditListItem::GetEditListItemType());
    CEditListItemPtr pEditListItem = CEditListItem::GetEditListItem(pItem);

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetDataIndex(BROADSOFT_CONTACT_DETAIL_ITEM_NUMBER);
    pEditListItem->SetShowIndex(true);
    pEditListItem->SetTitle(_LANG(TRID_NUMBER));
    pEditListItem->SetValue("");
    pEditListItem->SetMaxLength(MAX_LEN_BSFT_NUMBER);

    m_vecListItem.push_back(pEditListItem);

    pEditListItem->InstallIME(IME_DIAL, IME_123_Dial);
}

void CBSFTDetailLayout::UnInit()
{
    CEditListItemPtr pNameEditItem = GetNameEditItem();

    if (NULL != pNameEditItem)
    {
        pNameEditItem->DisconnectIMEChanged(this,
                                            static_cast<slotMethod>(&CDetailLayout::OnDetailIMEChanged));
    }

    CDetailLayout::UnInit();

    m_nAction = DIR_ACTION_NONE;
    m_nDetailId = knInvalidRId;
}

void CBSFTDetailLayout::SetAction(int nAction)
{
    m_nAction = nAction;
}

bool CBSFTDetailLayout::UpdateContact(int nContactId, ContactBroadSoftPersonal * pContact)
{
    bool bUpdate = false;

    m_nDetailId = nContactId;

    if (NULL == pContact)
    {
        return bUpdate;
    }

    CEditListItemPtr pNameItem = GetNameEditItem();

    if (NULL != pNameItem)
    {
        pNameItem->SetValue(pContact->GetDisplayName());
        bUpdate = true;
    }

    CEditListItemPtr pNumberItem = GetNumberEditItem();

    if (NULL != pNumberItem)
    {
        yl::string strNumber;

        pContact->GetNumberByName(kszBsftNumber, strNumber);

        pNumberItem->SetValue(strNumber);

        bUpdate = true;
    }

    return bUpdate;
}

int CBSFTDetailLayout::GetAction() const
{
    return m_nAction;
}

int CBSFTDetailLayout::GetDetailId() const
{
    return m_nDetailId;
}

void CBSFTDetailLayout::ReturnAllEdit()
{
    CEditListItem * pNameItem = GetNameEditItem();

    if (NULL != pNameItem)
    {
        pNameItem->ReturnAllInput();
    }

    CEditListItem * pNumberItem = GetNumberEditItem();

    if (NULL != pNumberItem)
    {
        pNumberItem->ReturnAllInput();
    }
}

CEditListItem * CBSFTDetailLayout::GetNameEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BROADSOFT_CONTACT_DETAIL_ITEM_NAME);

    return CEditListItem::GetEditListItem(pDetailItem);
}

CEditListItem * CBSFTDetailLayout::GetNumberEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BROADSOFT_CONTACT_DETAIL_ITEM_NUMBER);

    return CEditListItem::GetEditListItem(pDetailItem);
}

bool CBSFTDetailLayout::IsCopyLayout() const
{
    return (DIR_ACTION_BSFT_ADD_TO_PERSONAL == m_nAction);
}

bool CBSFTDetailLayout::IsAdditionLayout() const
{
    return (DIR_ACTION_BSFT_ADD_PERSONAL == m_nAction);
}
