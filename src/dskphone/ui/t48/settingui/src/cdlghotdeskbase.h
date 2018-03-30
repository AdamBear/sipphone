/************************************************************************
 * FileName  : CDlgHotDeskBase.h (header file)
 * Purpose   :
 * Date      : 2013/03/25 14:57:56
 ************************************************************************/

#ifndef _CDlgHotDeskBase_H_
#define _CDlgHotDeskBase_H_

#include "cdlgbasesubpage.h"

class QLineEdit;
struct HotDeskBaseData;

class CDlgHotDeskBase : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgHotDeskBase();
    }

public:
    CDlgHotDeskBase(QWidget * parent = 0);

    virtual ~CDlgHotDeskBase();

public:
    virtual void Init();
    virtual void Uninit();

public:
    //通过控制获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过控制获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);
    virtual void UninstallAllIME();

    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    void ProcessRegisterResult(bool bRegistered);

private:
    //取得当前页面数据
    void GetCurrentPageData(HotDeskBaseData & currentPageData);

private:
    QLineEdit * m_pEditRegisterName; //RegisterName
    QLineEdit * m_pEditUserName;    //UserName
    QLineEdit * m_pEditPassword;        //Password
    QLineEdit * m_pEditSIP_Server1;     //SIP_Server1
    QLineEdit * m_pEditSIP_Server2;     //SIP_Server2
    QLineEdit * m_pEditOutboundProxy1; //OutboundProxy1 Server
    QLineEdit * m_pEditOutboundProxy2; //OutboundProxy1 Server
    bool       m_bWaitForRegister;
};

#endif
