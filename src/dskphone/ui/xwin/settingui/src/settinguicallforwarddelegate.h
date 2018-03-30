#ifndef __SETTINGUI_CALL_FORWARD_DELEGATE_H__
#define __SETTINGUI_CALL_FORWARD_DELEGATE_H__

#include "settinguiaccountbasedelegate.h"

class CSettingUICallForwardDelegate : public CSettingUIAccountDelegateBase
{
public:
    CSettingUICallForwardDelegate();
    virtual ~CSettingUICallForwardDelegate();

    static CSettingUIDelegateBase* CreateCallForwardDelegate();

    virtual void OnEnterPage(const yl::string& strPage, int iDataIndex);
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData& objSoftkey);

private:
    void LoadCallForwardData(CSettingUIPageDataBase* pPageData);
    void LoadPrivateData(CSettingUIPageDataBase* pPagedata);
    bool IsForwardEnable(int nType);
    void SetValue(const yl::string& strItemId, bool bOn);
    bool AddCallForwardStatusItem(CSettingUIPageDataBase* pPageData, const yl::string& strDisplayName,
                                  const yl::string& strValue, const yl::string& strID = "",
                                  bool bShowIndex = true, bool bShowColon = true,
                                  const yl::string& strClickAction = "");
};

typedef CSettingUICallForwardDelegate* CSettingUICallForwardDelegatePtr;

#endif // __SETTINGUI_CALL_FORWARD_DELEGATE_H__
