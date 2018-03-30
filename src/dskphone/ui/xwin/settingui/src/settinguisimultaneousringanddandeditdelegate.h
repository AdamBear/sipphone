#ifndef _SETTINGUI_SIMULTANEOUS_RING_ADD_AND_EDIT_DELEGATE_H_
#define _SETTINGUI_SIMULTANEOUS_RING_ADD_AND_EDIT_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_PRIORITY_ALERT

#include "settinguixsidelegatebase.h"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"

class CSettinguiSimultaneousRingAddAndEditDeletae : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiSimultaneousRingAddAndEditDeletae();
    virtual ~CSettinguiSimultaneousRingAddAndEditDeletae();
    static CSettingUIDelegateBase * CreateSimultaneousRingAddAndEditDelegate();

public:
    static LRESULT OnMessage(msgObject & msg);
    virtual void InitExtraData(void * pData);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
protected:
    virtual void OnCancleClick();
    bool SetInvalidInfo(const yl::string & strNote);
protected:
    SSimultaneousRing m_simRingInfo;
    bool m_bActiveWhenAdd;
    bool m_bInAdd;
};

#endif //IF_FEATURE_BROADSOFT_PRIORITY_ALERT
#endif //_SETTINGUI_SIMULTANEOUS_RING_ADD_AND_EDIT_DELEGATE_H_
