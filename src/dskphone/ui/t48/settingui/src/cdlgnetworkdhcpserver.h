#ifndef CDlgNetworkDHCPServer_H
#define CDlgNetworkDHCPServer_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include "qtcommon/qmisc.h"
#include <QDialog>

struct NetworkRouterData;
///////////////////////////////////////////////////////////////////////////////
class CDlgNetworkDHCPServer : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgNetworkDHCPServer(QWidget * parent = 0);
    ~CDlgNetworkDHCPServer();

    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgNetworkDHCPServer();
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



private:
    //比较页面前后数据
    bool ComparePageData(const NetworkRouterData & beforePageData,
                         const NetworkRouterData & afterPageData) const;
    //取得当前页面数据
    NetworkRouterData & GetCurrentPageData(NetworkRouterData & currentPageData) const;
    //设置当前页面数据
    void SetCurrentPageData(NetworkRouterData & currentPageData);

private:
    QComboBox * m_pComServerStatus;

    QLineEdit * m_pEditStartIP;
    QLineEdit * m_pEditEndIP;
};

#endif // CDlgNetworkDHCPServer_H
