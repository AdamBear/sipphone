#ifndef _SETTINGUI_BACK_LIGHT_DELEGATE_H_
#define _SETTINGUI_BACK_LIGHT_DELEGATE_H_

#include "settinguidefine.h"
#if IF_SUPPORT_BACK_LIGHT_LEVEL
#include "settinguidatadelegatebase.h"

class CSettingUIBackLightDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettingUIBackLightDelegate();
    ~CSettingUIBackLightDelegate();

    static CSettingUIDelegateBase* CreateSettingUIBackLightDelegate();

public:
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    // 保存配置数据
    virtual bool SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    // ComboBox值改变响应函数
    virtual void OnComboBoxValueChange(int nIndex);

private:
    // 重设背景的亮度
    void ResetBacklight(int nLightLevel = -1);
    void AdjustBackLightTime(int& nTimes);
};
#endif

#endif //_SETTINGUI_BACK_LIGHT_DELEGATE_H_
