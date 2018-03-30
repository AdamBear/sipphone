#ifndef _CDLG_RESET_LOCAL_CONFIG_H_
#define _CDLG_RESET_LOCAL_CONFIG_H_

#include "cdlgbasesubpage.h"
class qUIPushButton;

class CDlgResetLocalConfig : public CDlgBaseSubPage
{
    //按钮位置及大小
#define BTNSTARTX 100
#define BTNSTARTY 149
#define BTNWIDTH  349
#define BTNHEIGHT 53

    Q_OBJECT

public:
    CDlgResetLocalConfig(QWidget * parent = 0);
    ~CDlgResetLocalConfig();

public:
    virtual bool LoadPageData();
    virtual bool IsStatusChanged();
    virtual bool SavePageData();
    virtual void InitData();
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgResetLocalConfig();
    }

private slots:
    void OnBtnResetLocalClicked();

private:
    qUIPushButton * m_btnResetLocal;

};

#endif //_CDLG_RESET_LOCAL_CONFIG_H_
