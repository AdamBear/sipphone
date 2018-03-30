#ifndef _SETTINGUI_FEATURES_DND_DELEGATE_H_
#define _SETTINGUI_FEATURES_DND_DELEGATE_H_

#include "settinguiaccountbasedelegate.h"

class CSettinguiFeaturesDNDDelegate : public CSettingUIAccountDelegateBase
{
public:
    CSettinguiFeaturesDNDDelegate();
    virtual ~CSettinguiFeaturesDNDDelegate();
    static CSettingUIDelegateBase * CreateFeatureDNDDelegate();

public:
    virtual bool ProcessMsgCallBack(int nResult, int nID, void * pExtraData = NULL);
    virtual void InitExtraData(void * pData);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
};

#endif //_SETTINGUI_FEATURES_DND_DELEGATE_H_
