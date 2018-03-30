#ifndef __SETTING_UI_ADVANCED_DELEGATE_H__
#define __SETTING_UI_ADVANCED_DELEGATE_H__

#include "basesettinguidelegate.h"

class CSettingUIAdvancedDelegate : public CSettingUIDelegateBase
{
public:
    CSettingUIAdvancedDelegate();
    virtual ~CSettingUIAdvancedDelegate();

    static CSettingUIDelegateBase * CreateAdvancedDelegate();

public:

    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void * pExtraData = NULL);

    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
};

#endif
