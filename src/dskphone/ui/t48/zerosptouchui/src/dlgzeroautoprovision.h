#ifndef DLGZEROAUTOPROVISION_H
#define DLGZEROAUTOPROVISION_H

#include "settingui/src/cdlgbasesubpage.h"

struct AutoProvisionData;

enum AUTOP_PAGE_TYPE
{
    AUTOP_PAGE_NORMAL = 0,
    AUTOP_PAGE_AUTHENTICATION,

};

class DlgZeroAutoProvision : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    DlgZeroAutoProvision(QWidget * parent);
    ~DlgZeroAutoProvision();

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

    //设置一个过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 控件判空
    bool JudgetWgtNull();

    void SetPageType(int nPageType);

private slots:
    void OnRefreshSoftkey();

private:
    //比较页面前后数据
    bool ComparePageData(const AutoProvisionData & beforePageData,
                         const AutoProvisionData & afterPageData) const;
    //取得当前页面数据
    AutoProvisionData & GetCurrentPageData(AutoProvisionData & currentPageData) const;
    //设置当前页面数据
    void SetCurrentPageData(AutoProvisionData & currentPageData);

    void InitListItem();

    bool IsPageShowUrl() const;

private:
    //用于记录加载时数据
    AutoProvisionData m_AutoProvisionData;

    QLineEdit * m_pEditURL;
    QLineEdit * m_pEditUserName;
    QLineEdit * m_pEditPassword;

    int m_nPageType;
};

#endif // DLGZEROAUTOPROVISION_H
