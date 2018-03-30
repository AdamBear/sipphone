#include <QtGui>
#include "baseui/detaillistitem.h"
#include "baseui/editlistitem.h"
#include "uimanager/basedialog.h"

#include "broadsoftdetaillayout.h"

#include "directorycommon.h"
#include "translateiddefine.h"
#include "moddirectoryui.h"
#include "account/include/modaccount.h"
#include "commonunits/modcommonunits.h"
#include "inputmethod/inputmethod.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/qlineeditutility.h"


CBroadSoftDetailLayout::CBroadSoftDetailLayout(CBaseDialog * pDlg)
    : CDetailListItemLayout(DETAIL_LIST_ITEM_LAYOUT_BROADSOFT, pDlg)
{
}

CBroadSoftDetailLayout::~CBroadSoftDetailLayout(void)
{
}

bool CBroadSoftDetailLayout::IsBroadSoftDetailLayout(CDetailListItemLayoutPtr pLayout)
{
    if (NULL != pLayout && DETAIL_LIST_ITEM_LAYOUT_BROADSOFT == pLayout->GetType())
    {
        return true;
    }

    return false;
}

void CBroadSoftDetailLayout::Init()
{
    InitGroupCtrl();
    InitNameCtrl();
    InitNumberCtrl();
}

// 初始化Name
void CBroadSoftDetailLayout::InitNameCtrl()
{
    CEditListItemPtr pEditListItem = new CEditListItem;

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetTitle(LANG_TRANSLATE(TRID_NAME));
    m_vecListItem.push_back(pEditListItem);

    QLineEdit * pEdit = pEditListItem->GetLineEdit();

    if (NULL != pEdit)
    {
        DirUI_SetNameIME(pEdit);

        pEditListItem->SetMaxLength(MAX_LEN_BSFT_NAME);
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
void CBroadSoftDetailLayout::InitNumberCtrl()
{
    CEditListItemPtr pEditListItem = new CEditListItem;

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetTitle(LANG_TRANSLATE(TRID_NUMBER));

    m_vecListItem.push_back(pEditListItem);

    QLineEdit * pEdit = pEditListItem->GetLineEdit();

    if (NULL != pEdit)
    {
        pEditListItem->SetInputInfo(EDIT_TYPE_CONTACT_NUMBER, IME_DIAL, IME_123_Dial);
        pEditListItem->SetMaxLength(MAX_LEN_BSFT_NUMBER);

        if (NULL != m_pDlg)
        {
            pEdit->installEventFilter(m_pDlg);
        }
    }
}

// 初始化群组ComboBox
void CBroadSoftDetailLayout::InitGroupCtrl()
{
    CEditListItemPtr pEditListItem = new CEditListItem;

    if (NULL == pEditListItem)
    {
        return;
    }

    pEditListItem->SetTitle(LANG_TRANSLATE(TRID_GROUP));
    pEditListItem->SetReadOnly(true);
    m_vecListItem.push_back(pEditListItem);
}

CEditListItem * CBroadSoftDetailLayout::GetNameEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BROADSOFT_CONTACT_DETAIL_ITEM_NAME);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

CEditListItem * CBroadSoftDetailLayout::GetNumberEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BROADSOFT_CONTACT_DETAIL_ITEM_NUMBER);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}


CEditListItem * CBroadSoftDetailLayout::GetGroupEditItem()
{
    CDetailListItemBasePtr pDetailItem = GetDetailListItem(BROADSOFT_CONTACT_DETAIL_ITEM_INDEX_GROUP);

    if (NULL != pDetailItem && LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        return (CEditListItem *)pDetailItem;
    }

    return NULL;
}

void CBroadSoftDetailLayout::ReturnAllEdit()
{
    //设置处于预输入状态的字符也有效
    CEditListItemPtr pNameEditItem = GetNameEditItem();

    if (NULL != pNameEditItem)
    {
        pNameEditItem->ReturnAllInput();
    }

    CEditListItemPtr pNumberEditItem = GetNumberEditItem();

    if (NULL != pNumberEditItem)
    {
        pNumberEditItem->ReturnAllInput();
    }
}
