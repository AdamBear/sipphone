#ifndef __USB_AUDIO_LIST_ADAPTER_H__
#define __USB_AUDIO_LIST_ADAPTER_H__

#ifdef IF_USB_SUPPORT

#include "settinguiadapter.h"

class CUSBAudioListAdapter : public CSettingUIAdapter
{
public:
    CUSBAudioListAdapter();
    virtual ~CUSBAudioListAdapter();
    static CSettingUIAdapter * CreateUSBAudioListAdapter();

public:
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);
    virtual yl::string GetItemValueByDataIndex(int nDataIndex);
};

#endif
#endif //__USB_AUDIO_LIST_ADAPTER_H__
