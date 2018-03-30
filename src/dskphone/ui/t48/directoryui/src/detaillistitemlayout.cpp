#include <QtGui>
#include "baseui/comboboxlistitem.h"
#include "baseui/editlistitem.h"
#include "uimanager/basedialog.h"
#include "detaillistitemlayout.h"
#include "inputmethod/inputmethod.h"

CDetailListItemLayout::CDetailListItemLayout(int nType, CBaseDialog * pDlg)
    : m_nType(nType)
    , m_pDlg(pDlg)
{
}

CDetailListItemLayout::~CDetailListItemLayout(void)
{
    ReleaseAllItem();
}

void CDetailListItemLayout::Init()
{
    ReleaseAllItem();
    m_pDlg = NULL;
}

void CDetailListItemLayout::ReleaseAllItem()
{
    VEC_DETAIL_LIST_ITEM::iterator iter = m_vecListItem.begin();

    for (; iter != m_vecListItem.end(); ++iter)
    {
        CDetailListItemBasePtr pListItem = *iter;

        if (NULL == pListItem)
        {
            continue;
        }

        int nDetailType = pListItem->GetDetailSubType();

        if (LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == nDetailType)
        {
            CComboBoxListItemPtr pComboBoxItem = (CComboBoxListItemPtr)pListItem;
            qComboBoxEx * pComboBox = pComboBoxItem->GetComboBox();

            if (NULL != pComboBox)
            {
                pComboBox->removeEventFilter(m_pDlg);
            }
        }
        else if (LIST_ITEM_SUB_TYPE_DETAIL_EDIT == nDetailType)
        {
            CEditListItemPtr pEditItem = (CEditListItemPtr)pListItem;
            QLineEdit * pEdit = pEditItem->GetLineEdit();

            if (NULL != pEdit)
            {
                pEdit->removeEventFilter(m_pDlg);
                qUninstallIME(pEdit);
            }
        }

        pListItem->Release();
    }

    m_vecListItem.clear();
}

bool CDetailListItemLayout::IsLayoutItem(CListItemPtr pListItem)
{
    if (NULL == pListItem)
    {
        return false;
    }

    VEC_DETAIL_LIST_ITEM::iterator iter = m_vecListItem.begin();

    for (; iter != m_vecListItem.end(); ++iter)
    {
        CDetailListItemBasePtr pDetailItem = *iter;

        if (pDetailItem == pListItem)
        {
            return true;
        }
    }

    return false;
}

int CDetailListItemLayout::GetType() const
{
    return m_nType;
}

int CDetailListItemLayout::GetItemCount() const
{
    return m_vecListItem.size();
}

CDetailListItemBasePtr CDetailListItemLayout::GetDetailListItem(int nIndex)
{
    int nSize = m_vecListItem.size();

    if (nIndex < 0 || nIndex >= nSize)
    {
        return NULL;
    }

    return m_vecListItem[nIndex];
}

bool CDetailListItemLayout::IsFirstItemFocus()
{
    int nFirstItemIndex = 0;
    CDetailListItemBasePtr pFirstItem = GetFirstVisbileItem(nFirstItemIndex);

    if (NULL == pFirstItem)
    {
        return false;
    }

    int nFocusIndex = GetFocusIndex();

    return (nFirstItemIndex == nFocusIndex);
}

bool CDetailListItemLayout::IsLastItemFocus()
{
    int nLastItemIndex = 0;
    CDetailListItemBasePtr pLastItem = GetLastVisbileItem(nLastItemIndex);

    if (NULL == pLastItem)
    {
        return false;
    }

    int nFocusIndex = GetFocusIndex();

    return (nLastItemIndex == nFocusIndex);
}

void CDetailListItemLayout::SetFirstItemFocus()
{
    int nFirstItemIndex = 0;
    CDetailListItemBasePtr pFirstItem = GetFirstVisbileItem(nFirstItemIndex);

    if (NULL != pFirstItem)
    {
        SetItemFocus(pFirstItem);
    }
}

void CDetailListItemLayout::SetLastItemFocus()
{
    int nLastItemIndex = 0;
    CDetailListItemBasePtr pLastItem = GetLastVisbileItem(nLastItemIndex);

    if (NULL != pLastItem)
    {
        SetItemFocus(pLastItem);
    }
}

void CDetailListItemLayout::SwitchFocusToPre()
{
    int nFocusIndex = GetFocusIndex();

    if (-1 == nFocusIndex)
    {
        return;
    }

    int nFirstItemIndex = 0;
    int nLastItemIndex = 0;
    CDetailListItemBasePtr pFirstItem = GetFirstVisbileItem(nFirstItemIndex);
    CDetailListItemBasePtr pLastItem = GetLastVisbileItem(nLastItemIndex);

    --nFocusIndex;

    if (nFocusIndex >= nFirstItemIndex && nFocusIndex <= nLastItemIndex)
    {
        CDetailListItemBasePtr pFocusItem = GetDetailListItem(nFocusIndex);

        SetItemFocus(pFocusItem);

        return;
    }

    if (nFocusIndex < nFirstItemIndex && NULL != pLastItem)
    {
        SetItemFocus(pLastItem);
        return;
    }

    SetItemFocus(pFirstItem);
}

void CDetailListItemLayout::SwitchFocusToNext()
{
    int nFocusIndex = GetFocusIndex();

    if (-1 == nFocusIndex)
    {
        return;
    }

    int nFirstItemIndex = 0;
    int nLastItemIndex = 0;
    CDetailListItemBasePtr pFirstItem = GetFirstVisbileItem(nFirstItemIndex);

    GetLastVisbileItem(nLastItemIndex);
    ++nFocusIndex;

    if (nFocusIndex >= nFirstItemIndex && nFocusIndex <= nLastItemIndex)
    {
        CDetailListItemBasePtr pFocusItem = GetDetailListItem(nFocusIndex);

        SetItemFocus(pFocusItem);

        return;
    }

    SetItemFocus(pFirstItem);
}

int CDetailListItemLayout::GetFocusIndex()
{
    if (NULL == m_pDlg)
    {
        return -1;
    }

    QWidget * pFocusWidget = m_pDlg->focusWidget();

    if (NULL == pFocusWidget)
    {
        return -1;
    }

    VEC_DETAIL_LIST_ITEM::iterator iter = m_vecListItem.begin();

    for (int i = 0; iter != m_vecListItem.end(); ++iter, ++i)
    {
        CDetailListItemBasePtr pItem = *iter;

        if (NULL == pItem)
        {
            continue;
        }

        if (pFocusWidget->parent() == pItem)
        {
            return i;
        }
    }

    return -1;
}

bool CDetailListItemLayout::IsEditItem(int nIndex)
{
    CDetailListItemBase* pFocusItem = GetDetailListItem(nIndex);
    CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(pFocusItem);
    if (pEditItem != NULL)
    {
        return true;
    }
    return false;
}

bool CDetailListItemLayout::IsLastEditItem()
{
    return (GetFocusIndex() == GetLastEditItemIndex());
}

bool CDetailListItemLayout::IsNextLastEditItem()
{
    int nIndex = GetFocusIndex() + 1;
    return (nIndex == GetLastEditItemIndex());
}

int CDetailListItemLayout::GetLastEditItemIndex()
{

    for (int i = m_vecListItem.size() - 1; i != -1; --i)
    {
        if (IsEditItem(i))
        {
            return i;
        }
    }
    return -1;
}

void CDetailListItemLayout::SetItemFocus(CDetailListItemBasePtr pDetailItem)
{
    if (NULL == pDetailItem || NULL == m_pDlg)
    {
        return;
    }

    QWidget * pFocusWidget = m_pDlg->focusWidget();
    QWidget * pParentWidget = NULL;

    if (NULL != pFocusWidget)
    {
        pParentWidget = (QWidget *)pFocusWidget->parent();
    }

    if (LIST_ITEM_SUB_TYPE_DETAIL_EDIT == pDetailItem->GetDetailSubType())
    {
        CEditListItemPtr pEditItem = (CEditListItemPtr)pDetailItem;

        pEditItem->FocusItem(true);

        if (NULL != pParentWidget)
        {
            pParentWidget->update();
        }
        return;
    }

    if (LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == pDetailItem->GetDetailSubType())
    {
        CComboBoxListItemPtr pComboBoxItem = (CComboBoxListItemPtr)pDetailItem;

        pComboBoxItem->FocusItem(true);

        if (NULL != pParentWidget)
        {
            pParentWidget->update();
        }
    }
}

bool CDetailListItemLayout::IsCanFocus(CDetailListItemBasePtr pDetailItem)
{
    if (NULL == pDetailItem)
    {
        return false;
    }

    int nSubType = pDetailItem->GetDetailSubType();

    if (LIST_ITEM_SUB_TYPE_DETAIL_EDIT == nSubType)
    {
        CEditListItemPtr pEditItem = (CEditListItemPtr)pDetailItem;
        QLineEdit * pEdit = pEditItem->GetLineEdit();

        if (NULL != pEdit && pEdit->isReadOnly())
        {
            return false;
        }

        return true;
    }
    else if (LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX == nSubType)
    {
        CComboBoxListItemPtr pComboBoxItem = (CComboBoxListItemPtr)pDetailItem;

        if (pComboBoxItem->IsOnlyShowText())
        {
            return false;
        }

        return true;
    }

    return false;
}

CDetailListItemBasePtr CDetailListItemLayout::GetFirstVisbileItem(int & nItemIndex)
{
    VEC_DETAIL_LIST_ITEM::iterator iter = m_vecListItem.begin();

    for (int i = 0; iter != m_vecListItem.end(); ++iter, ++i)
    {
        CDetailListItemBasePtr pItem = *iter;

        if (NULL != pItem && pItem->isVisible() && IsCanFocus(pItem))
        {
            nItemIndex = i;
            return pItem;
        }
    }

    return NULL;
}

CDetailListItemBasePtr CDetailListItemLayout::GetLastVisbileItem(int & nItemIndex)
{
    CDetailListItemBasePtr pLastItem = NULL;
    VEC_DETAIL_LIST_ITEM::iterator iter = m_vecListItem.begin();

    for (int i = 0; iter != m_vecListItem.end(); ++iter, ++i)
    {
        CDetailListItemBasePtr pItem = *iter;

        if (NULL != pItem && pItem->isVisible() && IsCanFocus(pItem))
        {
            nItemIndex = i;
            pLastItem = pItem;
        }
    }

    return pLastItem;
}
