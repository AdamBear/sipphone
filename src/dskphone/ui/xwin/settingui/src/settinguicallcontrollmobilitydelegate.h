#ifndef _SETTINGUI_CALL_CONTROLL_MOBILITY_DELEGATE_H_
#define _SETTINGUI_CALL_CONTROLL_MOBILITY_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_MOBILITY

#include "settinguixsidelegatebase.h"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"

class CSettinguiCallControllMobilityDelegate : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiCallControllMobilityDelegate();
    virtual ~CSettinguiCallControllMobilityDelegate();
    static CSettingUIDelegateBase * CreateCallControllMobilityDelegate();

public:
    static LRESULT OnMessage(msgObject & msg);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);

    void GetMobilityData(SMobility & mobilityInfo);
    void RefreshtMobilityData(const SMobility & mobilityInfo);

protected:
    virtual void OnCancleClick();
};

#endif //IF_FEATURE_BROADSOFT_MOBILITY
#endif //_SETTINGUI_CALL_CONTROLL_MOBILITY_DELEGATE_H_
