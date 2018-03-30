#ifndef _SETTINGUI_BT_CHANNEL_CONTROLL_DELEGATE_H_
#define _SETTINGUI_BT_CHANNEL_CONTROLL_DELEGATE_H_

#ifdef IF_SUPPORT_BLUETOOTH
#include "settinguibtwifidelegate.h"

class CSettingUIBtChannelControllDelegate : public CSettingUIBtWifiDelegate
{
public:
    CSettingUIBtChannelControllDelegate();
    virtual ~CSettingUIBtChannelControllDelegate();

    static CSettingUIDelegateBase * CreateBtChannelControllDelegate();

protected:
    virtual CSettingUIItemDataBase * GetItem(int nIndex, int nDataID);
    virtual CSettingUIItemDataBase * CreateDefaultItemData(const ViewInfo & viewInfo);
    virtual bool InitListItemData(CSettingUIItemDataBase * pItemData, const ViewInfo & viewInfo);
};

#endif // IF_SUPPORT_BLUETOOTH
#endif //_SETTINGUI_BT_CHANNEL_CONTROLL_DELEGATE_H_
