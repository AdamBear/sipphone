#ifndef _SETTINGUI_ADV_FLASH_TIME_DELEGATE_H_
#define _SETTINGUI_ADV_FLASH_TIME_DELEGATE_H_

#ifdef IF_FEATURE_PSTN

#include "basesettinguidelegate.h"

class CSetttinguiPSTNFlashTimeDelegate : public CSettingUIDelegateBase
{
public:
    CSetttinguiPSTNFlashTimeDelegate();
    virtual ~CSetttinguiPSTNFlashTimeDelegate();

    static CSettingUIDelegateBase * CreatePSTNFlashTimeDelegate();
public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
};

#endif //IF_FEATURE_PSTN
#endif //_SETTINGUI_ADV_FLASH_TIME_DELEGATE_H_
