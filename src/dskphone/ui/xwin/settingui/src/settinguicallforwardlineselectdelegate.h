#ifndef _SETTINGUI_CALL_FORWARD_SELECT_DELEGATE_H_
#define _SETTINGUI_CALL_FORWARD_SELECT_DELEGATE_H_

#include "basesettinguidelegate.h"

class CSettinguiCallForwardLineSelectDelegate : public CSettingUIDelegateBase
{
public:
    CSettinguiCallForwardLineSelectDelegate();
    virtual ~CSettinguiCallForwardLineSelectDelegate();

    static CSettingUIDelegateBase * CreateCallForwardLineSelectDelegate();
public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
protected:
    bool IsForwardOn(int nAccountID);
    void AddAccountListItem(CSettingUIPageDataBase * pPageData);
};

#endif //_SETTINGUI_CALL_FORWARD_SELECT_DELEGATE_H_
