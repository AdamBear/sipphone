#include "usbaudiolistadapter.h"
#ifdef IF_USB_SUPPORT
#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xListView.h"
#include "xwindow/xlistviewstyle.h"
#include "usbaudiolistitem.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "usbaudiolistdelegate.h"

CUSBAudioListAdapter::CUSBAudioListAdapter()
{

}

CUSBAudioListAdapter::~CUSBAudioListAdapter()
{

}

CSettingUIAdapter * CUSBAudioListAdapter::CreateUSBAudioListAdapter()
{
    CUSBAudioListAdapter * pAdapter = new CUSBAudioListAdapter;

    return pAdapter;
}

xListViewItem * CUSBAudioListAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    if (NULL == pListView)
    {
        return NULL;
    }

    CUSBAudioListItemPtr pResultItem = CUSBAudioListItem::GetUSBAudioListItem(pItem);
    if (NULL == pResultItem)
    {
        pResultItem = static_cast<CUSBAudioListItemPtr>(g_ItemFactory.ApplyItem(
                          CUSBAudioListItem::GetUSBAudioListItemType()));

        if (NULL != pResultItem)
        {
            pResultItem->SetAction(this);
        }
    }

    return pResultItem;
}

bool CUSBAudioListAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView
            || NULL == m_pSettingUIDelegate || NULL == pItem)
    {
        return false;
    }

    int nDataIndex = pItem->GetDataIndex();
    CSettingUIItemDataBasePtr pItemData = m_pSettingUIDelegate->GetItemDataByDataIndex(nDataIndex);
    CUSBAudioListItemPtr pUSBItem = CUSBAudioListItem::GetUSBAudioListItem(pItem);
    if (NULL != pUSBItem && NULL != pItemData)
    {
        pUSBItem->SetShowColon(pItemData->IsShowColon());
        pUSBItem->SetShowIndex(pItemData->IsShowIndex());
        pUSBItem->SetTitle(pItemData->GetDisplayName().c_str());
        pUSBItem->SetValue(pItemData->GetValue());
        return true;
    }

    return false;
}

yl::string CUSBAudioListAdapter::GetItemValueByDataIndex(int nDataIndex)
{
    xListViewPtr pListView = GetListView();
    if (NULL != pListView)
    {
        xListViewItem * pItem = pListView->GetItemByDataIndex(nDataIndex);
        CUSBAudioListItemPtr pUSBItem = CUSBAudioListItem::GetUSBAudioListItem(pItem);
        if (NULL != pUSBItem)
        {
            return pUSBItem->GetValue();
        }
    }

    return yl::string("");
}

#endif
