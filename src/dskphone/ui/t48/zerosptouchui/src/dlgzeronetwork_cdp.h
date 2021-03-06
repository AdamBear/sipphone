#ifndef DLGZERONETWORKCDP_H
#define DLGZERONETWORKCDP_H

#include "setting/include/common.h"
#include "settingui/src/cdlgbasesubpage.h"

class qCheckBox;

class CDlgZeroNetwork_CDP : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgZeroNetwork_CDP(QWidget * parent = 0);
    ~CDlgZeroNetwork_CDP();

    // 取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgZeroNetwork_CDP();
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

signals:
    void ReLayout(CBaseDialog * parent);

public slots:
    void OnChkBoxEnableCDPChanged(bool bOn);

private:
    // 比较页面前后数据
    bool ComparePageData(const NetworkCDPData & beforePageData,
                         const NetworkCDPData & afterPageData) const;

    // 取得当前页面数据
    NetworkCDPData & GetCurrentPageData(NetworkCDPData & currentPageData) const;

    // 设置当前页面数据
    void SetCurrentPageData(NetworkCDPData & currentPageData);

    bool IsVaildVlanCDPInterval(const QString & strValue);

private:
    qCheckBox * m_pCheckBoxCDPStatus;
    QLineEdit * m_pEditInterval;
};

#endif
