#ifndef __SETTINGUI_DATE_TIME_ADAPTER_H__
#define __SETTINGUI_DATE_TIME_ADAPTER_H__

#include "settinguiadapter.h"

class CSettingUIDateTimeAdapter : public CSettingUIAdapter
{
public:
    CSettingUIDateTimeAdapter();
    virtual ~CSettingUIDateTimeAdapter();

public:
    static CSettingUIAdapter * CreateDateTimeAdapter();

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


#endif // __SETTINGUI_DATE_TIME_ADAPTER_H__
