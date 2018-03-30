#ifndef __USB_RECORD_DELEGATE_H__
#define __USB_RECORD_DELEGATE_H__

#include "basesettinguidelegate.h"

#ifdef IF_USB_SUPPORT
class CUSBRecordDelegate : public CSettingUIDelegateBase
{
public:
    CUSBRecordDelegate();
    virtual ~CUSBRecordDelegate();

    static CSettingUIDelegateBase * CreateUSBRecordDelegate();

public:
    // 界面数据过滤
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
	virtual void OnEnterPage(const yl::string& strPage, int iDataIndex);
};
#endif

#endif
