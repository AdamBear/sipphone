#ifndef _SETTINGUI_CALL_CONTROLL_CALL_RECORD_DELEGATE_H_
#define _SETTINGUI_CALL_CONTROLL_CALL_RECORD_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING

#include "settinguixsidelegatebase.h"

class CSettinguiCallControllCallRecordDelegate : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiCallControllCallRecordDelegate();
    virtual ~CSettinguiCallControllCallRecordDelegate();
    static CSettingUIDelegateBase * CreateCallControllCallRecordDelegate();

public:
    static LRESULT OnMessage(msgObject & msg);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    void RefreshtCallRecordData();

protected:
    virtual void OnCancleClick();
};

#endif //IF_FEATURE_BROADSOFT_CALLRECORDING
#endif //_SETTINGUI_CALL_CONTROLL_CALL_RECORD_DELEGATE_H_
