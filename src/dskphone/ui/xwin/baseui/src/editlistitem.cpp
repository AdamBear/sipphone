#include "editlistitem.h"
#include "imeedit/ximeedit.h"
#include "xwindow/xListView.h"
#include "xwindow/xValueHandler.h"
#include "xwindow/xWindowManager.h"
#include "baseuicommon.h"
#include "inputmethod.h"
#include <baseuidefine.h>

namespace
{
#define EDIT_LIST_ITEM_IME_INDEX 1
#define EDIT_LIST_ITEM_DELETE_INDEX 2
#define EDIT_LIST_ITEM_INVALID_INDEX -1
}

CEditListItem::CEditListItem(IListViewItemAction * pAction/* = NULL*/)
    : CDetailListItemBase(pAction)
    , m_pEdit(NULL)
{
    m_nType |= LIST_ITEM_SUB_TYPE_DETAIL_EDIT;
}

CEditListItem::~CEditListItem()
{
}

int CEditListItem::GetEditListItemType()
{
    return (LIST_ITEM_TYPE_DETAIL | LIST_ITEM_SUB_TYPE_DETAIL_EDIT);
}

CEditListItem * CEditListItem::GetEditListItem(xListViewItem * pItem)
{
    if (NULL == pItem || GetEditListItemType() != pItem->GetType())
    {
        return NULL;
    }

    return (CEditListItem *)pItem;
}

xListViewItem * CEditListItem::CreateEditListItem()
{
    CEditListItem * pListItem = new CEditListItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent(BASE_UI_LAYOUT_FILE_EDIT_LIST_ITEM);
    }

    return pListItem;
}

void CEditListItem::loadChildrenElements(xml_node & node)
{
    CDetailListItemBase::loadChildrenElements(node);

    m_pEdit = static_cast<xEditTextView *>(getViewById(BASE_UI_ID_EDIT_VALUE));
}

void CEditListItem::Release()
{
    if (NULL != m_pEdit)
    {
        m_pEdit->SetPassword(false);
        m_pEdit->SetSuddenPassword(false);
        m_pEdit->SetMaxLength(-1);
        m_pEdit->EndIme();
        m_pEdit->SetInitText("");

        IME_UninstallIME(m_pEdit);
    }

    CDetailListItemBase::Release();
}

void CEditListItem::SetValue(const yl::string & strValue)
{
    if (NULL != m_pEdit)
    {
        m_pEdit->SetInitText(strValue);
    }
}

yl::string CEditListItem::GetValue() const
{
    if (NULL != m_pEdit)
    {
        return m_pEdit->GetText();
    }

    return "";
}

bool CEditListItem::onFocusEvent(bool bFocusIn)
{
#if IF_EDIT_TEXT_VIEW_SCROLL
    if (NULL != m_pEdit)
    {
        if (bFocusIn)
        {
            m_pEdit->StartScroll();
        }
        else
        {
            m_pEdit->StopScroll();
        }
    }
#endif

    return CDetailListItemBase::onFocusEvent(bFocusIn);
}

void CEditListItem::EndIME()
{
    if (NULL != m_pEdit)
    {
        m_pEdit->EndIme();
    }
}

void CEditListItem::InstallIME(const char * pszSupportIME, const char * pszIME)
{
    if (NULL == m_pEdit)
    {
        return;
    }

    IME_InstallIME(m_pEdit, pszSupportIME);

    m_pEdit->SetIme(pszIME);
}

void CEditListItem::UnInstallIME()
{
    if (NULL == m_pEdit)
    {
        return;
    }

    IME_UninstallIME(m_pEdit);
}

void CEditListItem::ConnectIMEChanged(xSlotHandler * pHandler, slotMethod pMethod)
{
    if (NULL == m_pEdit || NULL == pHandler || NULL == pMethod)
    {
        return;
    }

    m_pEdit->signalImeChanged.connect(pHandler, pMethod);
}

void CEditListItem::DisconnectIMEChanged(xSlotHandler * pHandler, slotMethod pMethod)
{
    if (NULL == m_pEdit || NULL == pHandler || NULL == pMethod)
    {
        return;
    }

    m_pEdit->signalImeChanged.disconnect(pHandler, pMethod);
}

void CEditListItem::SetPassword(bool bPassword)
{
    if (NULL != m_pEdit)
    {
        m_pEdit->SetPassword(bPassword);
    }
}

void CEditListItem::SetSuddenPassword(bool bSuddenPassword)
{
    if (NULL != m_pEdit)
    {
        m_pEdit->SetSuddenPassword(bSuddenPassword);
    }
}

void CEditListItem::SetDeleteEnable(bool bEnable, int nSoftkeyIndex/* = -1*/)
{
    if (NULL != m_pEdit)
    {
        if (bEnable)
        {
            m_pEdit->SetDeleteSoftkeyIndex(nSoftkeyIndex);
            m_pEdit->SetupDeleteSoftkey();
        }
        else
        {
            m_pEdit->ReleteDeleteSoftkey();
            m_pEdit->SetDeleteSoftkeyIndex(-1);
        }
    }
}

void CEditListItem::SetMaxLength(int nMaxLength)
{
    if (NULL != m_pEdit)
    {
        m_pEdit->SetMaxLength(nMaxLength);
    }
}

int CEditListItem::GetMaxLength()
{
    if (NULL != m_pEdit)
    {
        return m_pEdit->GetMaxLength();
    }
    return -1;
}

void CEditListItem::ConnectValueChanged(xSlotHandler* pHandler, slotMethod pMethod)
{
    if (NULL == m_pEdit || NULL == pHandler || NULL == pMethod)
    {
        return;
    }

    m_pEdit->signalTextConfirmChange.connect(pHandler, pMethod);
}

void CEditListItem::DisconnectValueChanged(xSlotHandler* pHandler, slotMethod pMethod)
{
    if (NULL == m_pEdit || NULL == pHandler || NULL == pMethod)
    {
        return;
    }

    m_pEdit->signalTextConfirmChange.disconnect(pHandler, pMethod);
}

void CEditListItem::ReturnAllInput()
{
    if (NULL != m_pEdit)
    {
        m_pEdit->EndIme();
    }
}

#if IF_EDIT_TEXT_VIEW_HINT_EMPTY
void CEditListItem::SetHintText(const yl::string & strHint)
{
    if (NULL != m_pEdit)
    {
        m_pEdit->SetHintText(strHint);
    }
}
#endif

xEditTextView * CEditListItem::GetEditText()
{
    return m_pEdit;
}

void CEditListItem::SetEditEnable(bool bEnable)
{
    if (NULL != m_pEdit)
    {
        m_pEdit->SetEnabled(bEnable);
        setDisable(false);
    }
}

#if IF_XLISTVIEW_SUPPORT_ITEM_CUSTOM_DECIDE_FOCUS_BG
bool CEditListItem::IsShowFocusBG()
{
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE
    return false;
#endif

    return xListViewItem::IsShowFocusBG();
}
#endif

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
void CEditListItem::UpdateTextView(bool bFocus)
{
    CDetailListItemBase::UpdateTextView(bFocus);

    UpdateTextViewColor(m_pEdit, bFocus);
}
#endif
