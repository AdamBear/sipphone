#ifndef _SETTINGUI_AUTO_PROVISION_H_
#define _SETTINGUI_AUTO_PROVISION_H_

#include "settinguidatadelegatebase.h"

class CSettinguiAutoProvisionDelegate : public CSettinguiDataDelegateBase
{
public:
    CSettinguiAutoProvisionDelegate();
    ~CSettinguiAutoProvisionDelegate();

    static CSettingUIDelegateBase * CreateAutoProvisionDelegate();
public:
    virtual bool ProcessMsgCallBack(int nResult, int nID, void * pExtraData = NULL);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);

};

#endif //_SETTINGUI_AUTO_PROVISION_H_
