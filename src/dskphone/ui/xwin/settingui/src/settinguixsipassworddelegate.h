#ifndef _SETTINGUI_XSI_PASSWORD_DELEGATE_H_
#define _SETTINGUI_XSI_PASSWORD_DELEGATE_H_

#include "basesettinguidelegate.h"

class CSettinguiXSIPasswordDelegate : public CSettingUIDelegateBase
{
public:
    CSettinguiXSIPasswordDelegate();
    virtual ~CSettinguiXSIPasswordDelegate();

    static CSettingUIDelegateBase * CreateXSIPasswordDelegate();
public:
    virtual void InitExtraData(void * pData);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
protected:
    yl::string m_strNextPage;
};

#endif //_SETTINGUI_XSI_PASSWORD_DELEGATE_H_
