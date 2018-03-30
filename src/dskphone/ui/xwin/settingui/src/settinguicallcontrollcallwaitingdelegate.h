#ifndef _SETTINGUI_CALL_CONTROLL_CALL_WAITING_DELEGATE_H_
#define _SETTINGUI_CALL_CONTROLL_CALL_WAITING_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING

#include "settinguixsidelegatebase.h"

class CSettinguiCallControllCallWaitingDelegate : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiCallControllCallWaitingDelegate();
    virtual ~CSettinguiCallControllCallWaitingDelegate();
    static CSettingUIDelegateBase * CreateCallControllCallWaitingDelegate();

public:
    static LRESULT OnMessage(msgObject & msg);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    void RefreshtCallWaitingData();

protected:
    virtual void OnCancleClick();
};

#endif //IF_FEATURE_BROADSOFT_CALL_WAITING
#endif //_SETTINGUI_CALL_CONTROLL_CALL_WAITING_DELEGATE_H_
