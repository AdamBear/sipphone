#ifndef MTSWPASSWORDBOX_H
#define MTSWPASSWORDBOX_H

#include "config.h"

#if IF_FEATURE_METASWITCH

#include "messageboxbase.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qscrolltextlabel.h"
#include "uicommon/messagebox/messageboxcallback.h"

class qScrollTextLabel;

class CMTSWCommportalBox : public CMessageBoxBase, public CMsgBoxCallBackAction
{
    Q_OBJECT

public:
    explicit CMTSWCommportalBox(CMaskFrame * pMaskFrame = NULL);
    virtual ~CMTSWCommportalBox(void);

public:
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    // 设置默认的输入法
    // |eType|密码框的输入类型
    void SetDefaultIME(LPCSTR lpszIME);

    virtual void PrintMsgBoxInfo(bool bAdd) {};

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
    QLineEdit               m_EditUserId;
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
#endif
#endif // MTSWPASSWORDBOX_H


