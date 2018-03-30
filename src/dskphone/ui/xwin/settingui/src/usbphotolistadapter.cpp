#include "usbphotolistadapter.h"

#if defined(IF_USB_SUPPORT) && IF_SUPPORT_USB_PHOTO
#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xListView.h"
#include "xwindow/xlistviewstyle.h"
#include "usbphotolistitem.h"
#include "usbphotolistdelegate.h"
#include "settinguilogic/src/settinguibaseitemdata.h"

CUSBPhotoListAdapter::CUSBPhotoListAdapter()
{

}

CUSBPhotoListAdapter::~CUSBPhotoListAdapter()
{

}

CSettingUIAdapter* CUSBPhotoListAdapter::CreateUSBPhotoListAdapter()
{
    CUSBPhotoListAdapter* pAdapter = new CUSBPhotoListAdapter;

    return pAdapter;
}

xListViewItem* CUSBPhotoListAdapter::PrepareItem(xListView* pListView, int nDataIndex,
        xListViewItem* pItem)
{
    if (NULL == pListView)
    {
        return NULL;
    }

    CUSBPhotoListItemPtr pResultItem = CUSBPhotoListItem::GetUSBPhotoListItem(pItem);
    if (NULL == pResultItem)
    {
        pResultItem = static_cast<CUSBPhotoListItemPtr>(g_ItemFactory.ApplyItem(
                          CUSBPhotoListItem::GetUSBPhotoListItemType()));

        if (NULL != pResultItem)
        {
            pResultItem->SetAction(this);
        }
    }

    return pResultItem;
}

bool CUSBPhotoListAdapter::RefreshItem(xListView* pListView, xListViewItem* pItem)
{
    if (NULL == pListView || pListView != m_pListView
            || NULL == m_pSettingUIDelegate || NULL == pItem)
    {
        return false;
    }

    int nDataIndex = pItem->GetDataIndex();
    CSettingUIItemDataBasePtr pItemData = m_pSettingUIDelegate->GetItemDataByDataIndex(nDataIndex);
    CUSBPhotoListItemPtr pUSBItem = CUSBPhotoListItem::GetUSBPhotoListItem(pItem);
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

yl::string CUSBPhotoListAdapter::GetItemValueByDataIndex(int nDataIndex)
{
    xListViewPtr pListView = GetListView();
    if (NULL != pListView)
    {
        xListViewItem* pItem = pListView->GetItemByDataIndex(nDataIndex);
        CUSBPhotoListItemPtr pUSBItem = CUSBPhotoListItem::GetUSBPhotoListItem(pItem);
        if (NULL != pUSBItem)
        {
            return pUSBItem->GetValue();
        }
    }

    return yl::string("");
}

#endif
