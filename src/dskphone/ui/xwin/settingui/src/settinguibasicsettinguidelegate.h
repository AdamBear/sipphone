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
    //����Itemͳһ����
    virtual bool ProcessAction(const yl::string strAction, int iDataIndex);
    // ��������ص�
    virtual bool ProcessMsgCallBack(int nResult, int nID, void* pExtraData = NULL);
};

#endif //__SETTING_UI_BASIC_SETTING_DELEGATE_H__
