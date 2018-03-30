#ifndef __USB_PHOTO_LIST_ADAPTER_H__
#define __USB_PHOTO_LIST_ADAPTER_H__

#include "settinguilogic/include/settingdefine.h"
#if defined(IF_USB_SUPPORT) && IF_SUPPORT_USB_PHOTO

#include "settinguiadapter.h"

class CUSBPhotoListAdapter : public CSettingUIAdapter
{
public:
    CUSBPhotoListAdapter();
    virtual ~CUSBPhotoListAdapter();
    static CSettingUIAdapter* CreateUSBPhotoListAdapter();

public:
    virtual xListViewItem* PrepareItem(xListView* pListView, int nDataIndex, xListViewItem* pItem);
    virtual bool RefreshItem(xListView* pListView, xListViewItem* pItem);
    virtual yl::string GetItemValueByDataIndex(int nDataIndex);
};

#endif
#endif //__USB_PHOTO_LIST_ADAPTER_H__
