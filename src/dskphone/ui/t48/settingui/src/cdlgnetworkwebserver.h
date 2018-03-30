#ifndef CDLGNETWORKWEBSERVER_H
#define CDLGNETWORKWEBSERVER_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>

struct NetworkWebServerData;
///////////////////////////////////////////////////////////////////////////////
class CDlgNetworkWebServer : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgNetworkWebServer(QWidget * parent = 0);
    ~CDlgNetworkWebServer();

public:
    //读取当前界面的值
    //bool GetPageData(* pData);

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

    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgNetworkWebServer();
    }

private:
    //比较页面前后数据
    bool ComparePageData(const NetworkWebServerData & beforePageData,
                         const NetworkWebServerData & afterPageData) const;
    //取得当前页面数据
    NetworkWebServerData & GetCurrentPageData(NetworkWebServerData & currentPageData) const;
    //设置当前页面数据
    void SetCurrentPageData(NetworkWebServerData & currentPageData);

private:
    //http与https的开关
    QComboBox * m_pCmbBoxHTTP;
    QComboBox * m_pCmbBoxHTTPS;

    QLineEdit * m_pEditHTTP_Port;
    QLineEdit * m_pEditHPPTS_Port;
};

#endif // CDLGNETWORKWEBSERVER_H
