#ifndef _SETTINGUI_CALL_CONTROLL_ACD_STATUS_SET_DELEGATE_H_
#define _SETTINGUI_CALL_CONTROLL_ACD_STATUS_SET_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
#include "settinguidatadelegatebase.h"

class CSettinguiCallControllACDStatusSetDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettinguiCallControllACDStatusSetDelegate();
    virtual ~CSettinguiCallControllACDStatusSetDelegate();

    static CSettingUIDelegateBase * CreateACDStatusSetDelegate();

public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
protected:
    void SetCheckByID(const yl::string & strItemID);
protected:
    bool m_bHideSomeItem;
};

#endif //IF_FEATURE_BROADSOFT_CALL_CENTER
#endif //_SETTINGUI_CALL_CONTROLL_ACD_STATUS_SET_DELEGATE_H_
