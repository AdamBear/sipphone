#ifndef _SETTINGUI_HOSTDESK_BASE_DELEGATE_H_
#define _SETTINGUI_HOSTDESK_BASE_DELEGATE_H_

#ifdef IF_FEATURE_HOTDESKING
#include "settinguiaccountbasedelegate.h"
#include "settinguilogic/include/common.h"
#include "messagebox/messageboxcallback.h"

class CSettingUIComboBoxItemData;

class CHotDeskBaseSettingDelegate : public CSettingUIAccountDelegateBase,
    public CMsgBoxCallBackAction
{
public:
    CHotDeskBaseSettingDelegate();
    ~CHotDeskBaseSettingDelegate();

    static CSettingUIDelegateBase * CreateHotDeskBaseSettingDelegate();
    static LRESULT OnMessage(msgObject & msg);
public:
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool PageDataValid(CSettingUIPageDataBase * pPagedata, CSettingUIDataValidInfo & validInfo);
    // 打开界面附加操作
    virtual void InitExtraData(void * pData);
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
protected:
    bool SetInvalidData(CSettingUIDataValidInfo & validInfo, int nIndex, const yl::string & strNote);
    void LoadCommonAccountPageData(CSettingUIPageDataBase * pPageData);
    void GetHotDeskData(HotDeskBaseData & stData);
    void LoadLineEditItem(CSettingUIPageDataBase * pPageData, const yl::string & strID,
                          const yl::string & strName
                          , int nMaxLength, const yl::string & strIme, const yl::string & strDefaultIme,
                          bool bPassword = false, const yl::string & strAuthorid = "");
    void AddComboboxItemValue(CSettingUIComboBoxItemData * pComboBoxItem,
                              const yl::string & strDisplayName, const int & nValue);
    void AfterLogonWizardRegisterFail();
private:
    HotDeskType m_eHotdeskType;
    bool        m_bRegisterName;
    bool        m_bUserName;
    bool        m_bPassword;
    bool        m_bServer;
    bool        m_bOutBound;
    bool        m_bAutopUrl;
    int         m_nIndexRegisterName;
    int         m_nIndexUserName;
    int         m_nIndexPassword;
    int         m_nIndexServer;
    int         m_nIndexOutBound;
    int         m_nIndexAutopUrl;
};
#endif

#endif //_SETTINGUI_HOSTDESK_BASE_DELEGATE_H_
