#ifndef __USB_AUDIO_RECORD_DELEGATE_H__
#define __USB_AUDIO_RECORD_DELEGATE_H__

#include "basesettinguidelegate.h"


#if (defined IF_USB_SUPPORT) && (defined IF_USB_RECORD)
class CUSBAudioRecordDelegate : public CSettingUIDelegateBase
{
public:
    CUSBAudioRecordDelegate();
    virtual ~CUSBAudioRecordDelegate();

    static CSettingUIDelegateBase * CreateUSBAudioRecordDelegate();

public:
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);

    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
};
#endif

#endif
