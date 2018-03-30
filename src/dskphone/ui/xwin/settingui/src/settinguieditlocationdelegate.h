#ifndef _SETTINGUI_EDIT_LOCATION_DELEGATE_H_
#define  _SETTINGUI_EDIT_LOCATION_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_ANYWHERE

#include "settinguixsidelegatebase.h"
#include "broadsoft/anywhere/include/anywherecommon.h"

class CSettinguiEditLocationDeletae : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiEditLocationDeletae();
    virtual ~CSettinguiEditLocationDeletae();
    static CSettingUIDelegateBase * CreateEditLocationDelegate();

public:
    static LRESULT OnMessage(msgObject & msg);
    virtual void InitExtraData(void * pData);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
public:
    void OnGetDetailLocation();

protected:
    virtual void OnCancleClick();
    bool CheckIsSipURI(const yl::string & strNumber);
    bool IsOutBountPhoneNumberValid(const yl::string & strOutboundNumber);
protected:
    SAYWLocation m_LocationDetail;
};

#endif //IF_FEATURE_BROADSOFT_ANYWHERE
#endif //_SETTINGUI_EDIT_LOCATION_DELEGATE_H_
