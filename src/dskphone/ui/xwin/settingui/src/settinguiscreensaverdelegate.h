#ifndef _SETTINGUI_SCREEN_SAVER_DELEGATE_H_
#define _SETTINGUI_SCREEN_SAVER_DELEGATE_H_

#include "settinguidefine.h"
#if IF_SUPPORT_SCREEN_SAVER
#include "settinguidatadelegatebase.h"

class CSettingUIScreenSaverDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettingUIScreenSaverDelegate();
    ~CSettingUIScreenSaverDelegate();

    static CSettingUIDelegateBase* CreateSettingUIScreenSaverDelegate();

public:
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    // 界面数据过滤
    virtual bool PageDataFilter(CSettingUIPageDataBase* pPagedata, bool bReload = false);
    // 保存配置数据
    virtual bool SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);

private:
    void AdjustWaitTime(int& nTimes);
    bool IsHasCumtomPicture();
};
#endif

#endif //_SETTINGUI_SCREEN_SAVER_DELEGATE_H_
