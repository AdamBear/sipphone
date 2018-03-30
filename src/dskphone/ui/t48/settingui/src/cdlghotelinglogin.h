/************************************************************************
 * FileName  : CDlgHotelingLogin.h (header file)
 * Purpose   :
 * Date      : 2012/12/21 11:28:47
 ************************************************************************/

#ifndef _CDLGHOTELINGLOGIN_H_
#define _CDLGHOTELINGLOGIN_H_

#include "cdlgbasesubpage.h"

class QComboBox;
class QLineEdit;

#if IF_FEATURE_XMLBROWSER_HOTELING
enum HOTELING_TYPE
{
    HOTELING_NOMAL = 0,
    HOTELING_XMLBROWSER = 1,
};
#endif

class CDlgHotelingLogin : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgHotelingLogin();
    }

public:
    CDlgHotelingLogin(QWidget * parent = NULL);

    virtual ~CDlgHotelingLogin();

    virtual void Init();
    virtual void Uninit();

    virtual bool LoadPageData();    // 读取页面数据
    virtual bool IsStatusChanged(); // 查看页面状态是否改变
    virtual bool SavePageData();    // 保存页面数据
    virtual bool SaveCompleted();       // 保存成功后的其他操作
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData);
    virtual void SetData(void * pData /* = NULL */);

    void OnMsgLoginSuccess();

private:
    void InitData();

private:
    QLineEdit * m_pUserID;
    QLineEdit * m_pPwd;
    QComboBox * m_pAutoLoginCmb;
    QComboBox * m_pHostCmb;
#if IF_FEATURE_XMLBROWSER_HOTELING
    HOTELING_TYPE   m_eType;
#endif
};

#endif
