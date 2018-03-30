#ifndef __WIFIBOX_H
#define __WIFIBOX_H

#include <QDialog>
#include "messageboxbase.h"
#include "uikernel/qlineeditutility.h"
#include "baseui/framelist.h"

class qUIPushButton;

class CWifiDetailBox : public CMessageBoxBase
{
    Q_OBJECT

public:
    explicit CWifiDetailBox(CMaskFrame * pMaskFrame = 0);
    ~CWifiDetailBox();

    void SetMessageBoxInfo(const QString & lpszText = "", int timeTodie = -1);

    virtual void PrintMsgBoxInfo(bool bAdd);

private:
    //设置弹出框背景
    void SetMessageBoxBackground(const QString & strPic);

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

protected slots:
    //响应点击OK事件
    void OnBtnDeleteClick();

    //响应点击Cancel事件
    void OnBtnCancelClick();

    void OnBtnMoveUpClick();
    void OnBtnMoveDownClick();

    void OnBtnEditClick();

private:
    qUIPushButton * m_pBtnCancel;
    qUIPushButton * m_pBtnMoveUp;
    qUIPushButton * m_pBtnMoveDown;
    qUIPushButton * m_pBtnDelete;
    qUIPushButton * m_pBtnEdit;
};

#endif // __WIFIBOX_H
