#ifndef __SETTINGUI_COMBOBOX_ADAPTER_H__
#define __SETTINGUI_COMBOBOX_ADAPTER_H__

#include "settinguiadapter.h"

class CSettingUIComboboxAdapter : public CSettingUIAdapter
{
public:
    CSettingUIComboboxAdapter();
    virtual ~CSettingUIComboboxAdapter();

public:
    static CSettingUIAdapter * CreateComboboxAdapter();

public:
    // 根据位置获取列表项
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);
public:
    // combobox value change
    void OnComboBoxValueChange(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                               LPVOID pData);
    // textview value change
    void OnTextViewValueChange(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                               LPVOID pData);
protected:
    bool m_bComboxBoxValue;
};


#endif // __SETTINGUI_COMBOBOX_ADAPTER_H__
