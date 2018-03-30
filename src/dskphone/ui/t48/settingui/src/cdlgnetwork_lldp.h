#ifndef _CDLGNETWORK_LLDP_H__
#define _CDLGNETWORK_LLDP_H__

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"

class qCheckBox;

///////////////////////////////////////////////////////////////////////////////
class CDlgNetwork_LLDP : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgNetwork_LLDP(QWidget * parent = 0);
    ~CDlgNetwork_LLDP();

    // 取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgNetwork_LLDP();
    }

public:
    // 通过控制获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    // 通过控制获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    // 读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 设置输入框获取包括在预输入状态的所有内容
    virtual void ReturnAllEdit();

    // 卸载界面所有的输入法
    virtual void UninstallAllIME();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    // 用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

private:
    // 比较页面前后数据
    bool ComparePageData(const NetworkLLDPData & beforePageData,
                         const NetworkLLDPData & afterPageData) const;

    // 取得当前页面数据
    NetworkLLDPData & GetCurrentPageData(NetworkLLDPData & currentPageData) const;

    // 设置当前页面数据
    void SetCurrentPageData(NetworkLLDPData & currentPageData);

private:
    qCheckBox * m_pCheckBoxLLDPStatus;
    QLineEdit * m_pEditPacketInterval;

};

#endif // _CDLGNETWORK_LLDP_H__
