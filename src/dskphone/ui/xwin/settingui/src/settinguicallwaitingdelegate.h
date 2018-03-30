#ifndef _SETTINGUI_CALL_WAITING_DELEGATE_H_
#define _SETTINGUI_CALL_WAITING_DELEGATE_H_

#include "settinguidatadelegatebase.h"

class CSettinguiCallWaitingDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettinguiCallWaitingDelegate();
    virtual ~CSettinguiCallWaitingDelegate();

    static CSettingUIDelegateBase * CreateCallWaitingDelegate();

    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
};


#endif //_SETTINGUI_CALL_WAITING_DELEGATE_H_
