#ifndef _SETTINGUI_ADV_CLOUD_ACCOUNT_SETTING_DELEGATE_H_
#define _SETTINGUI_ADV_CLOUD_ACCOUNT_SETTING_DELEGATE_H_

#if IF_FEATURE_CLOUD_ACCOUNT

#include "settinguiadvcommonaccountsettingdelegate.h"
#include "messagebox/messageboxcallback.h"

enum CLOUD_LOGIN_TYPE
{
    CLOUD_LOGIN_TYPE_ERROR = -1,
    CLOUD_LOGIN_TYPE_PIN_LOGIN,
    CLOUD_LOGIN_TYPE_USERNAME_LOGIN,
};

enum ACCOUNT_STATUS
{
    ACCOUNT_STATUS_ERROR = -1,
    ACCOUNT_STATUS_COMMON,
    ACCOUNT_STATUS_YEALINK,
    ACCOUNT_STATUS_YEALINK_LOGINED,
};

class CCommonAccountInfo;
class CCloudAccountInfo;
struct CloudAccountInfo;

class CAdvCloudAccountSettingDelegate : public CAdvCommonAccountSettingDelegate
    , public CMsgBoxCallBackAction
{
public:
    CAdvCloudAccountSettingDelegate();
    ~CAdvCloudAccountSettingDelegate();

    static CSettingUIDelegateBase * CreateAdvCloudAccountSettingDelegate();

public:
    static LRESULT OnMessage(msgObject & msg);
    void ProcessLoginResult(int nAccountId, int iResult);
    void ProcessAccountStatusChange();
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);

    virtual bool OnPageAction(int eActionType, int iDataIndex);
    virtual bool OnAction(int eActionType, int iDataIndex);
    // ComboBox值改变响应函数
    virtual void OnComboBoxValueChange(int nIndex);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
#ifdef IF_SETTING_WIZARD
    bool IsFinish()
    {
        return m_bFinish;
    }
#endif

protected:

    void ReloadPageData();
    bool OnCloudLogin(bool bBackToPrevious = true, bool bPopHint = true);
    void OnCloudLogout();
    bool SaveCommonPageData(bool bBackToPrevious = true, bool bPopHint = true);

    bool SetInvalidData(int nIdex, const yl::string & strNote);
    void LoadAccountTypeItem();
    void LoadFromPlatForm();
    void LoadFromYealinkLoginMode();
    void LoadCloudLoginedPageData();
    void RefreshPageData(bool bReLoad);
    void RefreshYealinkUsertNameData(CloudAccountInfo * pCloudInfo, bool bReLoad);
    void GetYealinkUserNamePageData(CloudAccountInfo * pCloudInfo);
    void CacheCommonAccoutData();
    void CacheYealinkPinCodeData();
    void CacheYealinkUsernameData(bool bCheckSave = false);
    void LoadBaseDataItem(const yl::string & strDisplayName, const yl::string & strValue);
    void ShowMessageBox(int nType, int nTime, const yl::string & strNote, int nID,
                        const yl::string strTitle);

protected:
    int m_nAccountStatus; //账号平台
    int m_nLoginMode; //登录模式
    bool m_bComboboxChange;
    bool m_bLogout;
#ifdef IF_SETTING_WIZARD
    bool m_bFinish;
#endif
    CCommonAccountInfo * m_pCommonInfo;
    CCloudAccountInfo * m_pAccountInfo;
};

#endif // IF_FEATURE_CLOUD_ACCOUNT
#endif //_SETTINGUI_ADV_CLOUD_ACCOUNT_SETTING_DELEGATE_H_
