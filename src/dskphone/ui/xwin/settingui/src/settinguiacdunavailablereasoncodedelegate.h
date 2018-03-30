#ifndef _SETTINGUI_ACD_UNAVAILABLE_REASON_CODE_DELEGATE_H_
#define _SETTINGUI_ACD_UNAVAILABLE_REASON_CODE_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
#include "settinguidatadelegatebase.h"

class CSettinguiACDUnavailableReasonCodeDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettinguiACDUnavailableReasonCodeDelegate();
    virtual ~CSettinguiACDUnavailableReasonCodeDelegate();

    static CSettingUIDelegateBase * CreateACDUnavailableReasonCodeDelegate();

public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
protected:
    void LoadListCodeItem(CSettingUIPageDataBase * pPageData);
};

#endif //IF_FEATURE_BROADSOFT_CALL_CENTER
#endif //_SETTINGUI_ACD_UNAVAILABLE_REASON_CODE_DELEGATE_H_
