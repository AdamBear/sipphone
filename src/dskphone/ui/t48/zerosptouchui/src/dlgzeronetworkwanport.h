#ifndef DLGZERONETWORKWANPORT_H
#define DLGZERONETWORKWANPORT_H

#include "settingui/src/cdlgbasesubpage.h"

struct NetworkWanPppoeData;
struct NetworkWanStaticIpData;
struct NetworkWanTypeData;
struct NetworkWanDnsServerData;
class qCheckBox;

//#define DEBUG_IME_WGT

///////////////////////////////////////////////////////////////////////////////
class CDlgZeroNetworkWANPort : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgZeroNetworkWANPort();
    }

public:
    CDlgZeroNetworkWANPort(QWidget * parent = 0);
    ~CDlgZeroNetworkWANPort();

public:
    //通过C层获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过C层获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    //设置输入框获取包括在预输入状态的所有内容
    virtual void ReturnAllEdit();

    // 卸载界面所有的输入法
    virtual void UninstallAllIME();

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    //隐藏控件
    void hideWidget();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    // 控件判空
    bool JudgetWgtNull();
#ifdef DEBUG_IME_WGT
    void Print(bool bSt = true);
#endif

public slots:
    //IP type配置选择类型变化处理操作
    void OnCmbBoxTypeChanged(int nIndex);

    void OnSwitchDnsServerStatus(bool bOn);

signals:
    void ReLayout(CBaseDialog * parent);

private:
#if IF_FEATURE_PPPOE
    //比较页面前后数据
    bool ComparePageData(const NetworkWanPppoeData & beforePageData,
                         const NetworkWanPppoeData & afterPageData) const;
    //取得当前页面数据
    virtual NetworkWanPppoeData & GetCurrentPageData(NetworkWanPppoeData & currentPageData) const;
    //设置当前页面数据
    virtual void SetCurrentPageData(NetworkWanPppoeData & currentPageData);
#endif

    //比较页面前后数据
    bool ComparePageData(const NetworkWanStaticIpData & beforePageData,
                         const NetworkWanStaticIpData & afterPageData) const;
    //取得当前页面数据
    NetworkWanStaticIpData & GetCurrentPageData(NetworkWanStaticIpData & currentPageData) const;
    //设置当前页面数据
    void SetCurrentPageData(NetworkWanStaticIpData & currentPageData);

    //用于设置配置类型项
    void SetCurrentPageData(NetworkWanTypeData & currentPageData);

    //比较页面前后数据
    bool ComparePageData(const NetworkWanDnsServerData & beforePageData,
                         const NetworkWanDnsServerData & afterPageData) const;
    //取得当前页面数据
    NetworkWanDnsServerData & GetCurrentPageData(NetworkWanDnsServerData & currentPageData) const;
    //设置DHCP下的静态DNS
    void SetCurrentPageData(NetworkWanDnsServerData & currentPageData);

    //设置网络类型，只能设置IPv4和IPv6中的一种，不能设置为Both
    void SetMode(int iMode);
    //根据网络类型决定是非要在wanType的选项中添加PPPOE
    void AddPageContent();
private:
    //存储打开界面时配置的值，在最后退出时用于判断配置是否经过改变
#if IF_FEATURE_PPPOE
    NetworkWanPppoeData m_eInitPPPoE;
#endif
    NetworkWanStaticIpData m_eInitStaticIp;
    NetworkWanTypeData m_eInitWanType;
    NetworkWanDnsServerData m_eInitDnsServerData;

    NetworkWanTypeData m_eCurrentPageWanType;

    QComboBox * m_pCmbBoxType;
    QLineEdit * m_pEditIP;
    QLineEdit * m_pEditSubnetMask;
    QLineEdit * m_pEditGateway;
    QLineEdit * m_pEditPriDNS;
    QLineEdit * m_pEditSecDNS;
#if IF_FEATURE_PPPOE
    QLineEdit * m_pEditPPPoEUser;
    QLineEdit * m_pEditPPPoEPWD;
#endif
    qCheckBox * m_pCheckBoxDHCP;
    QLineEdit * m_pEditDHCPPriDNS;
    QLineEdit * m_pEditDHCPSecDNS;

    int             m_nMode;
    friend class CDlgZeroNetworkIPMode;
};
#endif // DLGZERONETWORKWANPORT_H

