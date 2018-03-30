#ifndef _SETTINGUI_TIME_AND_DATE_DELEGATE_H_
#define _SETTINGUI_TIME_AND_DATE_DELEGATE_H_

#include "basesettinguidelegate.h"

class CSettingUITimeAndDateDelegate : public CSettingUIDelegateBase
{
public:
    CSettingUITimeAndDateDelegate();
    virtual ~CSettingUITimeAndDateDelegate();

    static CSettingUIDelegateBase * CreateTimeAndDateDelegate();

    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
protected:
    void SetCheck(const yl::string & strItemID);
};

#endif //_SETTINGUI_TIME_AND_DATE_DELEGATE_H_
