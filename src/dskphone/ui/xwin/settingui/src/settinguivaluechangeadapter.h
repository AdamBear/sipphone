#ifndef __SETTINGUI_VALUE_CHANGE_ADAPTER_H__
#define __SETTINGUI_VALUE_CHANGE_ADAPTER_H__

#include "settinguicomboboxadapter.h"

class CSettingUIValueChangeAdapter : public CSettingUIComboboxAdapter
{
public:
    CSettingUIValueChangeAdapter();
    virtual ~CSettingUIValueChangeAdapter();

public:
    static CSettingUIAdapter* CreateValueChangeAdapter();

public:
    // 根据位置获取列表项
    virtual xListViewItem* PrepareItem(xListView* pListView, int nDataIndex, xListViewItem* pItem);
public:
    // combobox value change
    void OnEditValueChange(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                           LPVOID pData);

    // 刷新Softkey
    virtual void RefreshSoftkey();
};


#endif // __SETTINGUI_VALUE_CHANGE_ADAPTER_H__
