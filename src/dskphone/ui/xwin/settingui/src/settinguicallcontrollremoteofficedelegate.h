#ifndef _SETTINGUI_CALL_CONTROLL_REMOTE_OFFICE_DELEGATE_H_
#define _SETTINGUI_CALL_CONTROLL_REMOTE_OFFICE_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_REMOTE_OFFICE

#include "settinguixsidelegatebase.h"

class CSettinguiCallControllRemoteOfficeDelegate : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiCallControllRemoteOfficeDelegate();
    virtual ~CSettinguiCallControllRemoteOfficeDelegate();
    static CSettingUIDelegateBase * CreateCallControllRemoteOfficeDelegate();

public:
    static LRESULT OnMessage(msgObject & msg);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    void RefreshtRemoteOfficeData(bool bActive, const yl::string strPhoneNumber);

protected:
    virtual void OnCancleClick();
};

#endif //IF_FEATURE_BROADSOFT_REMOTE_OFFICE
#endif //_SETTINGUI_CALL_CONTROLL_REMOTE_OFFICE_DELEGATE_H_
