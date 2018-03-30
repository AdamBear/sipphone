#ifndef _SETTINGUI_CALL_CONTROLL_FORWARD_SELECTIVE_DELEGATE_H_
#define _SETTINGUI_CALL_CONTROLL_FORWARD_SELECTIVE_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE

#include "settinguixsidelegatebase.h"

class CSettinguiCallControllForwardSelectiveDelegate : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiCallControllForwardSelectiveDelegate();
    virtual ~CSettinguiCallControllForwardSelectiveDelegate();
    static CSettingUIDelegateBase * CreateCallControllForwardSelectiveDelegate();

public:
    static LRESULT OnMessage(msgObject & msg);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    void RefreshtForwardSelectiveData();
    void SetRequest(bool bRequest);

protected:
    virtual void OnCancleClick();
    void AddCriteriaItem(const yl::string & strItemID, const yl::string & strDiasplay, bool bOn);
protected:
    bool m_bRequest;
};

#endif //IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE

#endif //_SETTINGUI_CALL_CONTROLL_FORWARD_SELECTIVE_DELEGATE_H_