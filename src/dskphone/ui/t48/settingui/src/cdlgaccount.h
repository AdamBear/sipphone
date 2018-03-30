#ifndef CDLGACCOUNT_H
#define CDLGACCOUNT_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"

struct AccountDetailData;

class CDlgAccount : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgAccount(QWidget * parent = 0);
    ~CDlgAccount();

public:

    //通过控制获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过控制获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgAccount();
    }

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

private:
    //比较页面前后数据
    bool ComparePageData(const AccountDetailData & beforePageData,
                         const AccountDetailData & afterPageData) const;
    //取得当前页面数据
    AccountDetailData & GetCurrentPageData(AccountDetailData & currentPageData) const;
    //设置当前页面数据
    void SetCurrentPageData(AccountDetailData & currentPageData);

private:

    //设置Activation对应的滑块开关
    QComboBox * m_pCmbBoxActivation;

    //设置Outbound Status对应的滑块开关
    QComboBox * m_pCmbBoxOutboundStatus;
    QComboBox * m_pCmbBoxNATStatus;

    QLineEdit * m_pEditLabel;
    QLineEdit * m_pEditDisplayName;
    QLineEdit * m_pEditRegisterName;
    QLineEdit * m_pEditUserName;
    QLineEdit * m_pEditPassword;
    QLineEdit * m_pEditSIP_Server1;
    QLineEdit * m_pEditSIP_Server2;
    QLineEdit * m_pEditOutboundProxy;

    QLineEdit * m_pEditOutboundProxy2;
    QLineEdit * m_pEditProxyFallbackInterval;
};

#endif // CDLGACCOUNT_H
