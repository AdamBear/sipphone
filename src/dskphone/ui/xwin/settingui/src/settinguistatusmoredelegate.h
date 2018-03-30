#ifndef _SETTINGUI_STATUS_MORE_DELEGATE_H_
#define _SETTINGUI_STATUS_MORE_DELEGATE_H_

#include "settinguidatadelegatebase.h"

class CSettinguiStatusMoregDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettinguiStatusMoregDelegate();
    virtual ~CSettinguiStatusMoregDelegate();

    static CSettingUIDelegateBase * CreateStatusMoreDelegate();
public:
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
};


#endif //_SETTINGUI_STATUS_MORE_DELEGATE_H_
