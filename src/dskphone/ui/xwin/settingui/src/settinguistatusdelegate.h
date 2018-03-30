#ifndef __SETTINGUI_STATUS_DELEGATE_H__
#define __SETTINGUI_STATUS_DELEGATE_H__

#include "basesettinguidelegate.h"

class CSettingUIStatusDelegate : public CSettingUIDelegateBase
{
public:
    CSettingUIStatusDelegate();
    virtual ~CSettingUIStatusDelegate();

    static CSettingUIDelegateBase * CreateStatusDelegate();

    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);

private:
    void SetValue(const yl::string & strItemId, const yl::string & strValue);
    void SetItemShow(CSettingUIPageDataBase * pPagedata, const yl::string & strItemId);
};

typedef CSettingUIStatusDelegate * CSettingUIStatusDelegatePtr;

#endif // __SETTINGUI_STATUS_DELEGATE_H__
