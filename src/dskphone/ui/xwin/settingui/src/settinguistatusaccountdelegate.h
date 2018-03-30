#ifndef __SETTINGUI_STATUS_ACCOUNT_DELEGATE_H__
#define __SETTINGUI_STATUS_ACCOUNT_DELEGATE_H__

#include "basesettinguidelegate.h"

class CSettingUIStatusAccountDelegate : public CSettingUIDelegateBase
{
public:
    CSettingUIStatusAccountDelegate();
    virtual ~CSettingUIStatusAccountDelegate();

    static CSettingUIDelegateBase * CreateStatusAccountDelegate();
public:
    static LRESULT OnMessage(msgObject & msg);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
private:
    yl::string GetStatusStringByAccountID(int nAccountID);
    void LoadAccountListData();
    void AddItem(const yl::string & strItemName, const yl::string & strValue = "");
};

typedef CSettingUIStatusAccountDelegate * CSettingUIStatusAccountDelegatePtr;

#endif // __SETTINGUI_STATUS_ACCOUNT_DELEGATE_H__
