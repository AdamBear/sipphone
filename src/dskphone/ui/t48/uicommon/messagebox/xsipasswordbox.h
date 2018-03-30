#ifndef XSIPASSWORDBOX_H
#define XSIPASSWORDBOX_H

#include "messageboxbase.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qscrolltextlabel.h"

class qScrollTextLabel;

class CXSIPasswordBox : public CMessageBoxBase
{
    Q_OBJECT

public:
    explicit CXSIPasswordBox(CMaskFrame * pMaskFrame = NULL);
    virtual ~CXSIPasswordBox(void);

public:
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

    // 设置默认的输入法
    // |eType|密码框的输入类型
    void SetDefaultIME(LPCSTR lpszIME);

    void SetUserStr(const QString & strUserTitle,
                    const QString & strUserName,
                    const QString & strPasswordTitle);

    virtual void PrintMsgBoxInfo(bool bAdd);

protected:
    //初始化
    void Init();

    //更新输入框及其标题
    void UpdateEditLineData();

    // 更新softkey数据
    void UpdateSoftkeyData();

    //密码检查
    bool CheckPassword();

    virtual bool event(QEvent * pEvent);

protected Q_SLOTS:
    // 响应点击Cancel按钮事件
    void OnBtnCancelClicked();

    // 响应点击OK按钮事件
    void OnBtnOKClicked();

    // 响应点击Delete按钮事件
    void OnBtnDeleteClicked();

    // 响应点击IME按钮事件
    void OnBtnIMEClicked();

private:
    // UserID标题
    QLabel                  m_LabUserId;
    // UserID值
    qScrollTextLabel    m_ValueUserId;
    // 密码标题
    QLabel                  m_LabPwd;
    // 密码输入框
    QLineEdit               m_EditPwd;

    //设置输入法切换按钮
    qUIPushButton * m_pBtnIME;
    //设置删除输入按钮
    qUIPushButton * m_pBtnDelete;

    QString m_strPwd;
};

#endif // XSIPASSWORDBOX_H
