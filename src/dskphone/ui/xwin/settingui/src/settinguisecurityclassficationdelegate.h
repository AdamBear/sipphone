#ifndef _SETTINGUI_SECURITY_CLASSFICATION_DELEGATE_H_
#define _SETTINGUI_SECURITY_CLASSFICATION_DELEGATE_H_

#include "basesettinguidelegate.h"

class CSettinguiSecurityClassficationDelegate : public CSettingUIDelegateBase
{
public:
    CSettinguiSecurityClassficationDelegate();
    virtual ~CSettinguiSecurityClassficationDelegate();

    static CSettingUIDelegateBase * CreateSecurityClassficationDelegate();
public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
};

#endif //_SETTINGUI_SECURITY_CLASSFICATION_DELEGATE_H_
