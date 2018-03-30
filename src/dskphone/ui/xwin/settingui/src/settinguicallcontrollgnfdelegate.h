#ifndef _SETTINGUI_CALL_CONTROLL_GROUP_NIGHT_FPRWARD_DELEGATE_H_
#define _SETTINGUI_CALL_CONTROLL_GROUP_NIGHT_FPRWARD_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD

#include "settinguixsidelegatebase.h"

class CSettinguiCallControllGNFDelegate : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiCallControllGNFDelegate();
    virtual ~CSettinguiCallControllGNFDelegate();
    static CSettingUIDelegateBase * CreateCallControllGNFDelegate();

public:
    static LRESULT OnMessage(msgObject & msg);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    void RefreshtGNFData();

protected:
    virtual void OnCancleClick();
    void SetGNFItemShow(const yl::string & strItemID);
};

#endif //IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD

#endif //_SETTINGUI_CALL_CONTROLL_GROUP_NIGHT_FPRWARD_DELEGATE_H_