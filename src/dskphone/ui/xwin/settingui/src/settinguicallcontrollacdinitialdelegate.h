#ifndef _SETTINGUI_CALL_CONTROLL_ACD_INITIAL_DELEGATE_H_
#define _SETTINGUI_CALL_CONTROLL_ACD_INITIAL_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
#include "settinguidatadelegatebase.h"

class CSettinguiCallControllACDinitialDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettinguiCallControllACDinitialDelegate();
    virtual ~CSettinguiCallControllACDinitialDelegate();

    static CSettingUIDelegateBase * CreateACDinitialDelegate();

    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
};

#endif //IF_FEATURE_BROADSOFT_CALL_CENTER
#endif //_SETTINGUI_CALL_CONTROLL_ACD_INITIAL_DELEGATE_H_
