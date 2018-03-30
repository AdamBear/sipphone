#ifndef _SETTINGUI_CALL_CONTROLL_CFNR_DELEGATE_H_
#define _SETTINGUI_CALL_CONTROLL_CFNR_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE

#include "settinguixsidelegatebase.h"

class CSettinguiCallControllCFNRDelegate : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiCallControllCFNRDelegate();
    virtual ~CSettinguiCallControllCFNRDelegate();
    static CSettingUIDelegateBase * CreateCallControllCFNRDelegate();

public:
    static LRESULT OnMessage(msgObject & msg);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    void RefreshtCFNRData(bool bActive, const yl::string strPhoneNumber);

protected:
    virtual void OnCancleClick();
};

#endif //IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
#endif //_SETTINGUI_CALL_CONTROLL_CFNR_DELEGATE_H_
