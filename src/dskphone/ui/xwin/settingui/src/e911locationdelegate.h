#ifndef _E911_LOCATION_DELEGATE_H_
#define _E911_LOCATION_DELEGATE_H_

#if IF_FEATURE_GENBAND_E911
#include "basesettinguidelegate.h"

class CE911LocationDelegate : public CSettingUIDelegateBase
{
public:
    CE911LocationDelegate();
    virtual ~CE911LocationDelegate();
    static CSettingUIDelegateBase* CreateE911LocationDelegate();

public:
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad /*= false*/);
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);

private:
    void OnResetAction();

};
#endif // IF_FEATURE_GENBAND_E911

#endif //_E911_LOCATION_DELEGATE_H_
