#ifndef _SETTINGUI_PHONE_UNLOCK_PING_DELEGATE_H_
#define _SETTINGUI_PHONE_UNLOCK_PING_DELEGATE_H_

#include "settinguidatadelegatebase.h"

class CSettingUIDataValidInfo;

class CSettinguiChangePingDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettinguiChangePingDelegate();
    virtual ~CSettinguiChangePingDelegate();

    static CSettingUIDelegateBase * CreateChangePingDelegate();
public:
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    // 判断数据是否合法
    bool IsPageDataValid(CSettingUIDataValidInfo & validInfo, const yl::string & strNewPin);
    virtual void InitExtraData(void * pData);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
protected:
    bool    m_bPwdVerified;
};

#endif //_SETTINGUI_PHONE_UNLOCK_PING_DELEGATE_H_
