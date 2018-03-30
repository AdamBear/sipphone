#ifndef CDLGZERONETWORKX_MODE_H
#define CDLGZERONETWORKX_MODE_H

#include "settingui/src/cdlgbasesubpage.h"
#include <QDialog>

struct Network802Dot1xData;
class QComboBox;
class QLineEdit;

class CDlgZeroNetworkX_Mode : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgZeroNetworkX_Mode();
    }

public:
    explicit CDlgZeroNetworkX_Mode(QWidget * parent = 0);
    ~CDlgZeroNetworkX_Mode();

protected slots:
    //设置不同界面显示
    void OnCmbBoxX_ModeCurrentIndexChanged(int nIndex);

signals:
    void ReLayout(CBaseDialog * parent);

public:
    //通过控制层获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过控制层获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    //设置输入框获取包括在预输入状态的所有内容
    virtual void ReturnAllEdit();

    // 卸载界面上所有的输入法
    virtual void UninstallAllIME();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    // 控件判空
    bool JudgetWgtNull();

private:
    //比较页面前后数据
    bool ComparePageData(const Network802Dot1xData & beforePageData,
                         const Network802Dot1xData & afterPageData) const;
    //取得当前页面数据
    Network802Dot1xData & GetCurrentPageData(Network802Dot1xData & currentPageData) const;
    //设置当前页面数据
    void SetCurrentPageData(Network802Dot1xData & currentPageData);

private:
    QComboBox * m_pCmbBoxX_Mode;
    QLineEdit * m_pEditIdentity;
    QLineEdit * m_pEditPassword;
};

#endif // CDLGZERONETWORKX_MODE_H
