#ifndef _SETTINGUI_ANONYMOUS_CALL_DELEGATE_H_
#define _SETTINGUI_ANONYMOUS_CALL_DELEGATE_H_

#include "settinguiaccountbasedelegate.h"

struct AnonymousCallData;
class CSettingUIDataValidInfo;

class CSettinguiAnonymousCallDelegate : public CSettingUIAccountDelegateBase
{
public:
    CSettinguiAnonymousCallDelegate();
    virtual ~CSettinguiAnonymousCallDelegate();
    static CSettingUIDelegateBase * CreateAnonymousCallDelegate();

public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    bool IsPageDataValid(const AnonymousCallData & stData, CSettingUIDataValidInfo & validInfo);
private:
    void GetPageData(AnonymousCallData & stAnonymousCallData);
};

#endif //_SETTINGUI_ANONYMOUS_CALL_DELEGATE_H_
