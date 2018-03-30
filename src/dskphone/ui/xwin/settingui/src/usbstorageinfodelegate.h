#ifndef __USB_STORAGE_INFO_DELEGATE_H__
#define __USB_STORAGE_INFO_DELEGATE_H__

#include "basesettinguidelegate.h"


#ifdef IF_USB_SUPPORT
class CUSBStorageInfoDelegate : public CSettingUIDelegateBase
{
public:
    CUSBStorageInfoDelegate();
    virtual ~CUSBStorageInfoDelegate();

    static CSettingUIDelegateBase * CreateStorageInfoDelegate();

public:
    // 加载配置数据
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);

protected:
    void UpdateItemValue(const char * pszID, const yl::string & strValue);
};
#endif

#endif
