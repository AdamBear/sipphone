#ifndef __SETTINGUI_ZERO_AUTOP_DELEGATE_H__
#define __SETTINGUI_ZERO_AUTOP_DELEGATE_H__

#include "settinguidatadelegatebase.h"

class CZeroAutopDelegate : public CSettinguiDataDelegateBase
{
public:
    CZeroAutopDelegate();
    virtual ~CZeroAutopDelegate();

    static CSettingUIDelegateBase * CreateZeroAutopDelegate();

    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    // 打开界面附加操作
    virtual void InitExtraData(void * pData);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    // 处理按键事件
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);

private:
    bool OnOKAction(bool bLongPress, int nDataIndex);

private:
    bool    m_bAutoPCodeAuth;
};

typedef CZeroAutopDelegate * CZeroAutopDelegatePtr;

#endif // __SETTINGUI_ZERO_AUTOP_DELEGATE_H__
