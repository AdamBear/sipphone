#ifndef CDlgNetworkRouter_H
#define CDlgNetworkRouter_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>

struct NetworkRouterData;

class CDlgNetworkRouter : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgNetworkRouter(QWidget * parent = 0);
    ~CDlgNetworkRouter();

    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgNetworkRouter();
    }

public:
    //通过控制获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //判断数据是否有改动
    virtual bool IsStatusChanged();

    //读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

protected:
    virtual void showEvent(QShowEvent * e);

private:
    //比较页面前后数据
    bool ComparePageData(const NetworkRouterData & beforePageData,
                         const NetworkRouterData & afterPageData) const;
    //取得当前页面数据
    NetworkRouterData & GetCurrentPageData(NetworkRouterData & currentPageData) const;
    //设置当前页面数据
    void SetCurrentPageData(NetworkRouterData & currentPageData);

private:
    virtual bool OnAction(const QString & strAction);

private:
    QLabel * m_pLabDHCPServer;

    QLineEdit * m_pEditRouterIP;
    QLineEdit * m_pEditSubnetMask;
};

#endif // CDlgNetworkRouter_H
