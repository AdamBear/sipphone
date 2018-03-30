#include "settinguivaluechangeadapter.h"
#include "xwindow/xListView.h"
#include "xwindow/xViewHandler.h"
#include "xwindow/xWindowManager.h"
#include "basesettinguidelegate.h"
#include "settingui/include/itemnamedefine.h"
#include "baseui/include/editlistitem.h"
#include "settinguilogic/src/settinguilineedititemdata.h"


CSettingUIValueChangeAdapter::CSettingUIValueChangeAdapter()
{
}

CSettingUIValueChangeAdapter::~CSettingUIValueChangeAdapter()
{
}

CSettingUIAdapter* CSettingUIValueChangeAdapter::CreateValueChangeAdapter()
{
    CSettingUIValueChangeAdapter* pAdapter = new CSettingUIValueChangeAdapter();

    return pAdapter;
}

xListViewItem* CSettingUIValueChangeAdapter::PrepareItem(xListView* pListView, int nDataIndex,
        xListViewItem* pItem)
{
    xListViewItemPtr pNewItem = CSettingUIComboboxAdapter::PrepareItem(pListView, nDataIndex, pItem);

    if (NULL == m_pSettingUIDelegate)
    {
        return pNewItem;
    }

    CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(pNewItem);

    if (NULL != pEditItem)
    {
        pEditItem->ConnectValueChanged(this, static_cast<slotMethod>(&CSettingUIValueChangeAdapter::OnEditValueChange));
    }

    return pNewItem;
}

void CSettingUIValueChangeAdapter::OnEditValueChange(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    if (NULL != m_pSettingUIDelegate && NULL != m_pListView)
    {
        CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(m_pListView->GetFocusItem());
        if (NULL != pEditItem)
        {
            int nIndex = pEditItem->GetDataIndex();
            m_pSettingUIDelegate->SetItemDataValueByDataIndex(nIndex, pEditItem->GetValue());
            m_pSettingUIDelegate->OnEditValueChange(nIndex);
        }
    }
}

void CSettingUIValueChangeAdapter::RefreshSoftkey()
{
    CSettingUIAdapter::RefreshSoftkey();
    if (NULL == m_pSettingUIDelegate || NULL == m_pListView)
    {
        return;
    }

    CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(m_pListView->GetFocusItem());
    if (NULL == pEditItem)
    {
        return;
    }

    int nIndex = pEditItem->GetDataIndex();
    CSettingUIItemDataBase* pItemData = m_pSettingUIDelegate->GetItemDataByDataIndex(nIndex);
    CSettingUILineEditItemData* pEditItemData = CSettingUILineEditItemData::GetLineEditDataItem(pItemData);
    if (NULL == pEditItemData)
    {
        return;
    }

    pEditItem->SetDeleteEnable(pEditItemData->IsDeleteSoftkeyEnable(), pEditItemData->GetDeleteSoftkeyIndex());
}

