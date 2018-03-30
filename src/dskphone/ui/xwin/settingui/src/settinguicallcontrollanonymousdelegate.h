#ifndef _SETTINGUI_CALL_CONTROLL_ANONYMOUS_DELEGATE_H_
#define _SETTINGUI_CALL_CONTROLL_ANONYMOUS_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_CLID

#include "settinguixsidelegatebase.h"

class CSettinguiCallControllAnonymousDelegate : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiCallControllAnonymousDelegate();
    virtual ~CSettinguiCallControllAnonymousDelegate();
    static CSettingUIDelegateBase * CreateCallControllAnonymousDelegate();

public:
    static LRESULT OnMessage(msgObject & msg);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    void RefreshtAnonymousData();

protected:
    virtual void OnCancleClick();
    void LoadAnymousItem();
};

#endif //IF_FEATURE_BROADSOFT_CLID
#endif //_SETTINGUI_CALL_CONTROLL_ANONYMOUS_DELEGATE_H_
