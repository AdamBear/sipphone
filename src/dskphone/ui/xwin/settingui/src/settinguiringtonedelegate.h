#ifndef _SETTINGUI_RING_TONE_DELEGATE_H_
#define _SETTINGUI_RING_TONE_DELEGATE_H_

#include "basesettinguidelegate.h"

class CSettingUIRingToneDelegate : public CSettingUIDelegateBase
{
public:
    CSettingUIRingToneDelegate();
    virtual ~CSettingUIRingToneDelegate();

    static CSettingUIDelegateBase * CreateRingToneDelegate();
public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool OnAction(int eActionType, int iDataIndex);

protected:
    void AddListItem(CSettingUIPageDataBase * pPageData);
    void AddRingToneMenuItem(CSettingUIPageDataBase * pPageData, const yl::string & strDisp,
                             int nAccountID);
    int GetRealAccountID(int nDataIndex);
};

#endif //_SETTINGUI_RING_TONE_DELEGATE_H_
