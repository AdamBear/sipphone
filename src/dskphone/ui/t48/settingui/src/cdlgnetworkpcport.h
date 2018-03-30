#ifndef CDLGNETWORKPCPORT_H
#define CDLGNETWORKPCPORT_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>
#include "settinguicommon.h"

class CDlgNetworkPCPort : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgNetworkPCPort(QWidget * parent = 0);
    ~CDlgNetworkPCPort();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgNetworkPCPort();
    }

    // 卸载界面上所有的输入法
    virtual void UninstallAllIME();

private:

    //通过C层获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //判断数据是否有改动
    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();


    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    virtual bool OnAction(const QString & strAction);

    bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //取得当前页面数据
    NetworkRouterData & GetPageRouterData(NetworkRouterData & currentPageData) const;
    //比较页面前后数据
    bool CompareRouterData(const NetworkRouterData & beforePageData,
                           const NetworkRouterData & afterPageData) const;
    //设置当前页面数据
    void SetRouterData(NetworkRouterData & currentPageData);

    bool LoadData();

protected slots:
    void OnCmbTypeChanged(int);

private:
    PCPORT_MODE m_eCurrentMode;
    bool m_bDHCPEnabled;

    QComboBox * m_pCmbType;
    QLabel * m_pLabDHCPServer;
    QLineEdit * m_pEditRouterIP;
    QLineEdit * m_pEditSubnetMask;
};

#endif // CDLGNETWORKPCPORT_H
