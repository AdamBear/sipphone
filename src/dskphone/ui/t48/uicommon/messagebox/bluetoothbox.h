#ifndef BLUETOOTHBOX_H
#define BLUETOOTHBOX_H

#include <QDialog>
#include "messageboxbase.h"
#include "uikernel/qlineeditutility.h"
#include "baseui/framelist.h"

class qUIPushButton;
class QLabel;
class qLine;
class qCheckBox;

class CBluetoothBox : public CMessageBoxBase
{
    Q_OBJECT

public:
    explicit CBluetoothBox(CMaskFrame * pMaskFrame = 0);
    ~CBluetoothBox();

    void SetMessageBoxInfo(const QString & lpszText = "", int timeTodie = -1);

    void SetAutoConnect(bool & bAutoConnect);
    qCheckBox * GetCheckBox() const;

    virtual void PrintMsgBoxInfo(bool bAdd);

private:
    //设置弹出框背景
    void SetMessageBoxBackground(const QString & strPic);

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

private slots:
    void OnSwitchAutoStatus(bool bOn);

protected slots:
    //响应点击OK事件
    void OnBtnDeleteClick();

    //响应点击Cancel事件
    void OnBtnCancelClick();

    void OnBtnMoveUpClick();
    void OnBtnMoveDownClick();

private:
    bool           m_bAutoConnect;
    qUIPushButton * m_pBtnCancel;
    qUIPushButton * m_pBtnMoveUp;
    qUIPushButton * m_pBtnMoveDown;
    qUIPushButton * m_pBtnDelete;
    QLabel    *    m_pLabelTitle;
    QLabel    *    m_pLabelList;
    qLine     *    m_pLine;
    qCheckBox   *  m_pCheckBox;
};

#endif // BLUETOOTHBOX_H
