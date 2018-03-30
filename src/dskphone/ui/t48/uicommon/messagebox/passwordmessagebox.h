#ifndef PASSWORDMESSAGEBOX_H
#define PASSWORDMESSAGEBOX_H

#include "messageboxbase.h"
#include "uikernel/qlineeditutility.h"
#include "messageboxcallback.h"

class CPasswordMessageBox : public CMessageBoxBase, public CMsgBoxCallBackAction
{
    Q_OBJECT

public:
    explicit CPasswordMessageBox(CMaskFrame * pMaskFrame,
                                 const QString & strNode,
                                 const QString & strWaringText,
                                 const QString & strPassWord,
                                 const QString & strUserLevel,
                                 int nMaxInput = -1);

    explicit CPasswordMessageBox(CMaskFrame * pMaskFrame
                                 , const QString & strNode
                                 , int nMaxInput);

    ~CPasswordMessageBox();

    // 设置Label
    void SetNote(const QString & strNote);

    // 设置自动退出时间
    void MySetExitTime(int iTimeToExit);

    // 设置密码错误时显示的文字
    void SetWaringText(const QString & strWaringText);

    // 设置用户等级
    // 设置用户等级后就会使用用户等级对应的密码进行判断
    void SetPassWordType(const QString & strUserLevel);

    // 设置密码
    void SetPassWord(const QString & strPassWord);

    // 设置默认的输入法
    // |eType|密码框的输入类型
    void SetDefaultIME(LPCSTR lpszIME, const yl::string & strIMEType = "");

    bool eventFilter(QObject * pObject, QEvent * pEvent);

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox) {}

    void SetFeedBackShowType(bool bIsMaskFeedback);

    void SetIme(LPCSTR lpszImeName);

    virtual void PrintMsgBoxInfo(bool bAdd);

    QLineEdit * GetLineEdit();

private:
    //初始化
    void Init();

    // 更新输入框及其标题数据
    void UpdateEditLineData();

    // 更新softkey数据
    void UpdateSoftkeyData();

    //密码检查
    bool CheckPassword(bool & bOpenByUserPassword);

    virtual bool event(QEvent * pEvent);

private slots:
    //响应点击Cancel按钮事件
    void OnBtnCancelClicked();

    //响应点击OK按钮事件
    void OnBtnOKClicked();

    //响应点击Delete按钮事件
    void OnBtnDeleteClicked();

    //响应点击IME按钮事件
    void OnBtnIMEClicked();

private:
    QLabel      m_labNote;
    QLineEdit   m_lineEditPassword;
    QString     m_strUserLevel;
    QString     m_strWaringText;
    QString     m_strPassWord;

    int         m_nMaxInput;

    //设置输入法切换按钮
    qUIPushButton * m_pBtnIME;
    //设置删除输入按钮
    qUIPushButton * m_pBtnDelete;
};

#endif // PASSWORDMESSAGEBOX_H
