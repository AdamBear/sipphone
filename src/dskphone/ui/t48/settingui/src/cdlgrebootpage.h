#ifndef CDLGREBOOTPAGE_H
#define CDLGREBOOTPAGE_H

#include "cdlgbasesubpage.h"

class CDlgRebootPage : public CDlgBaseSubPage
{
    //按钮位置及大小
#define BTNSTARTX 100
#define BTNSTARTY 149
#define BTNWIDTH  349
#define BTNHEIGHT 53

    Q_OBJECT

public:
    CDlgRebootPage(QWidget * parent = 0);
    ~CDlgRebootPage();

public:
    //通过控制获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过控制获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    //设置softkey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

    virtual void UpdateWnd();

    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgRebootPage();
    }

private slots:
    void OnBtnRebootClicked();

private:
    qUIPushButton * m_qUIBtnReboot;

};

#endif // CDLGREBOOTPAGE_H
