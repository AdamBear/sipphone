#include "settinguicomboboxadapter.h"
#include "xwindow/xListView.h"
#include "xwindow/xViewHandler.h"
#include "xwindow/xWindowManager.h"
#include "xwindow/xcombobox.h"
#include "basesettinguidelegate.h"
#include "settingui/include/itemnamedefine.h"
#include "baseui/include/comboboxlistitem.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"


CSettingUIComboboxAdapter::CSettingUIComboboxAdapter()
    : m_bComboxBoxValue(false)
{
}

CSettingUIComboboxAdapter::~CSettingUIComboboxAdapter()
{
}

CSettingUIAdapter * CSettingUIComboboxAdapter::CreateComboboxAdapter()
{
    CSettingUIComboboxAdapter * pAdapter = new CSettingUIComboboxAdapter();

    return pAdapter;
}

xListViewItem * CSettingUIComboboxAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    xListViewItemPtr pNewItem = CSettingUIAdapter::PrepareItem(pListView, nDataIndex, pItem);

    if (NULL == m_pSettingUIDelegate)
    {
        return pNewItem;
    }

    CComboBoxListItemPtr pComboBoxItem = CComboBoxListItem::GetComboBoxListItem(pNewItem);

    if (NULL != pComboBoxItem)
    {
        xComboBox * pComboBox = pComboBoxItem->GetComboBox();
        xComboBoxHandler * pComboBoxHandle = NULL;

        if (NULL != pComboBox)
        {
            SLOT_CONNECT(pComboBox, signalTextChanged, this, CSettingUIComboboxAdapter::OnTextViewValueChange);
            pComboBoxHandle = pComboBox->GetComboBoxHandle();
        }

        if (NULL != pComboBoxHandle)
        {
            SLOT_CONNECT(pComboBoxHandle, signalValueChanged, this,
                         CSettingUIComboboxAdapter::OnComboBoxValueChange);
        }
    }

    return pNewItem;
}

void CSettingUIComboboxAdapter::OnTextViewValueChange(xSignal * sender, WPARAM wParam,
        LPARAM lParam, int nDataBytes, LPVOID pData)
{
    if (NULL != m_pSettingUIDelegate && NULL != m_pListView && m_bComboxBoxValue)
    {
        CComboBoxListItemPtr pCustomCmbItem = CComboBoxListItem::GetComboBoxListItem(
                m_pListView->GetFocusItem());
        if (NULL != pCustomCmbItem)
        {
            int nIndex = pCustomCmbItem->GetDataIndex();
            m_pSettingUIDelegate->SetItemDataValueByDataIndex(nIndex, pCustomCmbItem->GetValue());

            m_pSettingUIDelegate->OnComboBoxValueChange(nIndex);
        }
    }

    m_bComboxBoxValue = false;
}

void CSettingUIComboboxAdapter::OnComboBoxValueChange(xSignal * sender, WPARAM wParam,
        LPARAM lParam, int nDataBytes, LPVOID pData)
{
    m_bComboxBoxValue = true;
}
