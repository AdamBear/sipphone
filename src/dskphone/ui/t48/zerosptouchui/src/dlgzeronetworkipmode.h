#ifndef DLGZERONETWORKIPMODE_H
#define DLGZERONETWORKIPMODE_H

#include "settingui/src/cdlgbasesubpage.h"

class QComboBox;
class QLineEdit;
class CDlgZeroNetworkWANPort;

///////////////////////////////////////////////////////////////////////////////
class CDlgZeroNetworkIPMode : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgZeroNetworkIPMode();
    }

public:
    CDlgZeroNetworkIPMode(QWidget * parent = 0);
    ~CDlgZeroNetworkIPMode();

public:
    //初始化页面属性
    virtual void Init();
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

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //重新调整页面控件
    void DoReLayout();

public slots:
    //IP type配置选择类型变化处理操作
    void OnCmbBoxModeChanged(int nIndex);
    //Wan Type变化的处理
    void OnWANTypeChanged(CBaseDialog * pDlg);

signals:
    void ReLayout(CBaseDialog * parent, int iSelectIndex);

private:
    QComboBox * m_pCmbBoxMode;
    CDlgZeroNetworkWANPort * m_pIPV4WANPort;
    CDlgZeroNetworkWANPort * m_pIPV6WANPort;
    int                     m_nMode;
    int                     m_nPreMode; //进入页面时的mode,用于比较mode是否改变

};
#endif // DLGZERONETWORKIPMODE_H
