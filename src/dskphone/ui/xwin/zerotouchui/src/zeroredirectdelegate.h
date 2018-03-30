#ifndef __SETTINGUI_ZERO_REDIRECT_DELEGATE_H__
#define __SETTINGUI_ZERO_REDIRECT_DELEGATE_H__

#include "basesettinguidelegate.h"

class CZeroRedirectDelegate : public CSettingUIDelegateBase
{
public:
    CZeroRedirectDelegate();
    virtual ~CZeroRedirectDelegate();

    static CSettingUIDelegateBase * CreateZeroRedirectDelegate();
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    // 是否跳过Return To Idle，包括通话、Idle
    virtual bool IsRejectReturnToIdle();
    // 打开界面附加操作
    virtual void InitExtraData(void * pData);
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);

private:
    void GetValueById(const yl::string & strItemId, yl::string & strValue);
    void OnStartRedirect();
};

typedef CZeroRedirectDelegate * CZeroRedirectDelegatePtr;

#endif // __SETTINGUI_ZERO_REDIRECT_DELEGATE_H__
