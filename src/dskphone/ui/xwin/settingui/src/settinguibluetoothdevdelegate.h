#ifndef _SETTINGUI_BLUETOOTH_DEV_DELEGATE_H_
#define _SETTINGUI_BLUETOOTH_DEV_DELEGATE_H_

#ifdef IF_SUPPORT_BLUETOOTH
#include "settinguibtwifidelegate.h"

class CSettingUIBluetoothDevDelegate : public CSettingUIBtWifiDelegate
{
public:
    CSettingUIBluetoothDevDelegate();
    virtual ~CSettingUIBluetoothDevDelegate();

    static CSettingUIDelegateBase * CreateBluetoothDevDelegate();

protected:
    virtual CSettingUIItemDataBase * CreateDefaultItemData(const ViewInfo & viewInfo);
    virtual bool InitListItemData(CSettingUIItemDataBase * pItemData, const ViewInfo & viewInfo);
};

#endif // IF_SUPPORT_BLUETOOTH
#endif //_SETTINGUI_BLUETOOTH_DEV_DELEGATE_H_
