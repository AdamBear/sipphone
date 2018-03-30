#ifndef _SETTINGUI_ADD_LOCATION_DELEGATE_H_
#define _SETTINGUI_ADD_LOCATION_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_ANYWHERE

#include "settinguixsidelegatebase.h"

class CSettinguiAddLocationDeletae : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiAddLocationDeletae();
    virtual ~CSettinguiAddLocationDeletae();
    static CSettingUIDelegateBase * CreateAddLocationDelegate();

public:
    static LRESULT OnMessage(msgObject & msg);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);

protected:
    virtual void OnCancleClick();
    bool SetInvalidInfo(const yl::string & strNoteo);
};

#endif //IF_FEATURE_BROADSOFT_ANYWHERE
#endif //_SETTINGUI_ADD_LOCATION_DELEGATE_H_
