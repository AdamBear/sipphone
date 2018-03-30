#ifndef __USB_AUDIO_PLAYER_DELEGATE_H__
#define __USB_AUDIO_PLAYER_DELEGATE_H__

#include "basesettinguidelegate.h"

#ifdef IF_USB_SUPPORT
class CUSBAudioPlayerDelegate : public CSettingUIDelegateBase
{
public:
    CUSBAudioPlayerDelegate();
    virtual ~CUSBAudioPlayerDelegate();

    static CSettingUIDelegateBase * CreateUSBAudioPlayerDelegate();

public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);

    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);

    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
};
#endif

#endif
