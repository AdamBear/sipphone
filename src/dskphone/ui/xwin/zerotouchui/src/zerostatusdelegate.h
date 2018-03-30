#ifndef __SETTINGUI_ZERO_STATUS_LIST_DELEGATE_H__
#define __SETTINGUI_ZERO_STATUS_LIST_DELEGATE_H__

#include "basesettinguidelegate.h"

class CZeroStatusDelegate : public CSettingUIDelegateBase
{
public:
    CZeroStatusDelegate();
    virtual ~CZeroStatusDelegate();

    static CSettingUIDelegateBase * CreateZeroStatusDelegate();

    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious = true, bool bPopHint = true);
    virtual bool PageDataFilter(CSettingUIPageDataBase* pPagedata, bool bReload = false);

private:
    void SetValue(const yl::string & strItemId, const yl::string & strValue);
};

typedef CZeroStatusDelegate * CZeroStatusDelegatePtr;

#endif // __SETTINGUI_ZERO_STATUS_LIST_DELEGATE_H__
