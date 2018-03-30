#ifndef _SETTINGUI_LINEKEY_LIST_DELEGATE_H_
#define _SETTINGUI_LINEKEY_LIST_DELEGATE_H_

#include "basesettinguidelegate.h"

class CSettinguiLinekeyListDelegate : public CSettingUIDelegateBase
{
public:
    CSettinguiLinekeyListDelegate();
    virtual ~CSettinguiLinekeyListDelegate();

    static CSettingUIDelegateBase* CreateLinekeyListDelegate();
public:
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
};

#endif //_SETTINGUI_LINEKEY_LIST_DELEGATE_H_
