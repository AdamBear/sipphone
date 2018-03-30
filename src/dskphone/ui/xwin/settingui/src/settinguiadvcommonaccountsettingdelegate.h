#ifndef _SETTINGUI_ADV_COMMON_ACCOUNT_SETTING_DELEGATE_H_
#define _SETTINGUI_ADV_COMMON_ACCOUNT_SETTING_DELEGATE_H_

#include "settinguiaccountbasedelegate.h"
#include "settinguilogic/include/common.h"

class CAdvCommonAccountSettingDelegate : public CSettingUIAccountDelegateBase
{
public:
    CAdvCommonAccountSettingDelegate();
    ~CAdvCommonAccountSettingDelegate();

    static CSettingUIDelegateBase * CreateAdvCommonAccountSettingDelegate();
public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool PageDataValid(CSettingUIPageDataBase * pPagedata, CSettingUIDataValidInfo & validInfo);
    virtual bool OnPageAction(int eActionType, int iDataIndex);
protected:
    bool SetInvalidData(CSettingUIDataValidInfo & validInfo, const yl::string& strItemID, const yl::string & strNote);
    void LoadCommonAccountPageData(CSettingUIPageDataBase * pPageData);
    void SetCommonAccountData(const AccountDetailData & stData);
    void GetCommonAccountData(AccountDetailData & stData);
    void LoadLineEditItem(CSettingUIPageDataBase * pPageData, const yl::string & strID,
                          const yl::string & strName
                          , int nMaxLength, const yl::string & strDefaultIme, bool bPassword = false,
                          const yl::string & strAuthorid = "");
    void LoadComboboxItem(CSettingUIPageDataBase * pPageData, const yl::string & strID,
                          const yl::string & strName
                          , const yl::string strValue1, const yl::string strValue2, const yl::string & strAuthorid = "");
};

#endif //_SETTINGUI_ADV_COMMON_ACCOUNT_SETTING_DELEGATE_H_
