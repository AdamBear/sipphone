#ifndef _SETINGUI_BLF_RING_TONE_DELEGATE_H_
#define _SETINGUI_BLF_RING_TONE_DELEGATE_H_

#include "settinguilogic/include/settingdefine.h"
#if IF_SUPPORT_BLF_RING
#include "settinguiringtonesettingdelegate.h"
#include "xwindow-kernel/include/xthreadtimer.h"

class CSettingUIBLFRingToneDelegate : public CSettingUIRingToneSettingDelegate
{
public:
    CSettingUIBLFRingToneDelegate();
    virtual ~CSettingUIBLFRingToneDelegate();

    static CSettingUIDelegateBase* CreateBLFRingToneDelegate();
public:
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool OnAction(int eActionType, int iDataIndex);
protected:
    void AddBlfListRing(CSettingUIPageDataBase* pPageData);

};
#endif

#endif //_SETINGUI_ACCOUNT_RING_TONE_SETTING_DELEGATE_H_
