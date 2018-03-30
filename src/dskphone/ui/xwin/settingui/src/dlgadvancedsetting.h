#ifndef _DLG_ADVANCED_SETTING_H_
#define _DLG_ADVANCED_SETTING_H_

#include "dlgsettingui.h"

class CDlgAdvancedSetting : public CDlgSettingUI
{
public:
    static CBaseDialog * CreateInstance();

public:
    CDlgAdvancedSetting();
    virtual ~CDlgAdvancedSetting();

public:
    virtual void Init();
    virtual void Uninit();
    // 弹出框回调处理
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    void OnAppearSlot(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
protected:
    void CheckNetwork();
};

#endif //_DLG_ADVANCED_SETTING_H_
