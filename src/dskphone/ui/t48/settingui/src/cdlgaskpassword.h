/************************************************************************
* FileName  : CDlgAskPassword.h (header file)
* Purpose   :
* Date      : 2013/02/04 9:53:40
************************************************************************/

#ifndef _CDLGXSIPASSWORD_H_
#define _CDLGXSIPASSWORD_H_

#include "settingui/src/cdlgbasesubpage.h"

class QLineEdit;

class CDlgAskPassword : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgAskPassword();
    }

    enum ASK_PASSWORD_TYPE
    {
        ASK_PASSWORD_COMMON,
        ASK_PASSWORD_WIFI_COMMON,
        ASK_PASSWORD_WIFI_AUTHENTICATION,
    };

public:
    CDlgAskPassword(QWidget * parent = 0);

    virtual ~CDlgAskPassword();

    virtual void Init();
    virtual void Uninit();

    virtual bool LoadPageData();    // 读取页面数据
    virtual bool IsStatusChanged(); // 查看页面状态是否改变
    virtual bool SavePageData();    // 保存页面数据
    virtual bool SaveCompleted();
    virtual void SetData(void * pData /* = NULL */);
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData);
    // 用于退出界面时的特殊操作
    virtual void OnBack();
    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);
    void SetType(ASK_PASSWORD_TYPE eType)
    {
        m_eType = eType;
    }

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

Q_SIGNALS:
    void SubmitPassword(const yl::string & strPswd);
    void submitNameAndPassword(const yl::string & strName, const yl::string & strPwd);
    void CancelInputPassword();

private:
    void InitData();
    void InitListItem();

    void InitEditProperty();

private:
    QLineEdit     *     m_pUserName;
    QLineEdit     *     m_pPswdEdit;
    ASK_PASSWORD_TYPE   m_eType;
};

#endif
