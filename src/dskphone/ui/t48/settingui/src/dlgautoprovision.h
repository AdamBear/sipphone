#ifndef DLGAUTOPROVISION_H
#define DLGAUTOPROVISION_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"

struct AutoProvisionData;

class CDlgAutoProvision : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgAutoProvision(QWidget * parent = 0);
    ~CDlgAutoProvision();

public:
    //通过C层获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过C层获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgAutoProvision();
    }

    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

    virtual void UninstallAllIME();

private:
    //取得当前页面数据
    void GetCurrentPageData(AutoProvisionData & currentPageData) const;
    //设置当前页面数据
    void SetCurrentPageData(AutoProvisionData & currentPageData);

    void AddWidgetToDelegate();
    bool IsInitSucceed() const;

private:
    //用于记录加载时数据
    AutoProvisionData m_AutoProvisionData;

    QLineEdit * m_pEditURL;
    QLineEdit * m_pEditUserName;
    QLineEdit * m_pEditPassword;
    QLineEdit * m_pEditCommon;
    QLineEdit * m_pEditMACOriented;

    // 控件是否初始化成功
    bool m_bWidgetInitSucceed;
};

#endif // DLGAUTOPROVISION_H
