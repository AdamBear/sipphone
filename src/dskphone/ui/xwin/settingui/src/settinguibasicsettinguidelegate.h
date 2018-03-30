#ifndef __SETTING_UI_BASIC_SETTING_DELEGATE_H__
#define __SETTING_UI_BASIC_SETTING_DELEGATE_H__

#include "basesettinguidelegate.h"

class CSettingUIBasicSettingDelegate : public CSettingUIDelegateBase
{
public:
    CSettingUIBasicSettingDelegate();
    virtual ~CSettingUIBasicSettingDelegate();

    static CSettingUIDelegateBase* CreateBasicSettingDelegate();

public:
    virtual bool PageDataFilter(CSettingUIPageDataBase* pPagedata, bool bReload = false);
    //界面Item统一操作
    virtual bool ProcessAction(const yl::string strAction, int iDataIndex);
    // 处理弹出框回调
    virtual bool ProcessMsgCallBack(int nResult, int nID, void* pExtraData = NULL);
};

#endif //__SETTING_UI_BASIC_SETTING_DELEGATE_H__
