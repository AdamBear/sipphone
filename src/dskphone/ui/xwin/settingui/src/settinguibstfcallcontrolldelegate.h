#ifndef _SETTINGUI_BSTF_CALL_CONTROLL_DELEGATE_H_
#define _SETTINGUI_BSTF_CALL_CONTROLL_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_BASE

#include "basesettinguidelegate.h"

class CSettinguiBSTFCallControllDelegate : public CSettingUIDelegateBase
{
public:
    CSettinguiBSTFCallControllDelegate();
    ~CSettinguiBSTFCallControllDelegate();

    static CSettingUIDelegateBase * CreateBSTFCallControllDelegate();

public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual void OnEnterPage(const yl::string & strPage, int iDataIndex);
protected:
    void LoadBrosoftListItem();
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    void AddExecutiveAssistantCallControlItem();
#endif
    void CreateSecurityClassfication();
    void LoadMenuItem(const yl::string & strID, const yl::string & strDisplayName,
                      const yl::string & strPageID
                      , int nAccountCount, const yl::string & strExtra = "");
    bool IsEnableToEnterNextPage(const yl::string & strPageID);

    void LoadMoreAccountItem(const yl::string & strItemID, const yl::string & strPageID,
                             const yl::string & strValue, int nAccountCount);
};

#endif // IF_FEATURE_BROADSOFT_BASE
#endif //_SETTINGUI_BSTF_CALL_CONTROLL_DELEGATE_H_
