#ifndef DLGWIFIADDEDIT_H
#define DLGWIFIADDEDIT_H

#include "settingui/src/cdlgbasesubpage.h"

#include "wireless/wifi/include/modwifi.h"
#include "wireless/wifi/include/wifidefines.h"

class CDlgWifiAddEdit : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgWifiAddEdit();
    }

public:
    CDlgWifiAddEdit(QWidget * parent = NULL);
    ~CDlgWifiAddEdit(void);

    virtual bool LoadPageData();
    virtual bool SavePageData();

    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    // 查看页面状态是否改变
    virtual bool IsStatusChanged();

    // 对输入的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    // print
    void Print();

    void SetPageDataByType();

private:
    void GetCurPageData();

    bool GetNetWorkInfoByID(int iNetID, WiFiNetInfo & netInfo);

private slots:
    void OnCmbModeChanged(int iIdx);

private:
    void InitData();
    void InitListItem();

    bool IsNeedPassword() const;

    int GetSecurityModeIndexBySecurityType(int nSecurityType);

private:
    int                     m_iPageType;
    int                     m_iNetID;

    QComboBox       *       m_pCmbSecurityMode;
    QLineEdit       *       m_pEditProfileName;
    QLineEdit       *       m_pEditSSID;
    QComboBox       *       m_pCmbCipherType;
    QLineEdit       *       m_pEditUserName;
    QLineEdit       *       m_pEditWPASharedKey;
    WiFiNetInfo             m_wifiInfo;
};

#endif
