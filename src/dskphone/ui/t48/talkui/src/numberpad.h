#ifndef NUMBERPAD_H
#define NUMBERPAD_H

#include "cdialbutton.h"
#include "baseui/fontcolor.h"
#include <QWidget>
#include <QSignalMapper>

#include "keymap.h"
#include "globalinputhook/modinputhook.h"

#include <QMessageBox>

class qIMEButton : public qUIPushButton
{
    Q_OBJECT

public:
    explicit qIMEButton(QWidget * parent = 0);
    ~qIMEButton();

protected:
    void paintEvent(QPaintEvent * paintEvent);
};

// 全键盘按钮
class qKeypadButton : public qUIPushButton
{
    Q_OBJECT

public:
    explicit qKeypadButton(QWidget *parent = 0);
    ~qKeypadButton();

protected:
    void paintEvent(QPaintEvent *paintEvent);

};

class NumberPad : public QWidget
{
    Q_OBJECT

public:
    NumberPad(QWidget * parent = 0);
    ~NumberPad();

private slots:
    void OnPressedKey(int key);
    void OnReleasedkey(int key);

private:
    CDialButton * m_btnOne;
    CDialButton * m_btnTwo;
    CDialButton * m_btnThree;
    CDialButton * m_btnFour;
    CDialButton * m_btnFive;
    CDialButton * m_btnSix;
    CDialButton * m_btnSeven;
    CDialButton * m_btnEight;
    CDialButton * m_btnNine;
    CDialButton * m_btnStar;
    CDialButton * m_btnZero;
    CDialButton * m_btnPound;
};

//////////////////////////////////////////////////////////////////////////
class DialPad : public QWidget
{
    Q_OBJECT

public:
    DialPad(QWidget * parent = 0);
    ~DialPad();

    // 设置拨号键的图标
    void SetDialBtnIcon(const QString & strIcon);

    // 设置拨号键的Action
    void SetDialBtnAction(const QString & strAction);

    // 设置拨号键的文字
    void SetDialBtnText(const QString & strText);

    // 设置Transfer Send键的图标
    void SetTransSendBtnIcon(const QString & strIcon);

    // 设置Transfer Send键的Action
    void SetTransSendBtnAction(const QString & strAction);

    // 设置Transfer Send键的文字
    void SetTransSendBtnText(const QString & strText);

    // 设置IME按钮上的文字
    void SetIMEBtnText(const QString & strText);
#if IF_BUG_25055
    // 是否需要IME按钮
    void SetIMEBtnVisible(bool bVisible);

    // 设置扩展键的文字
    void SetExtensionBtnText(const QString & strText);

    // 设置扩展键的Action
    void SetExtensionBtnAction(const QString & strAction);

    // 是否显示Extension按键
    void SetExtensionBtnVisible(bool bVisible);
#endif

    void SetTransferState(bool bBtnVisible);

    // 隐藏输入法btn
    void HideIMEBtn();

signals:
    void BtnClicked(const QString & strBtnAction);

private:
    NumberPad      m_NumberPad;
    qUIPushButton * m_btnDial;
    qUIPushButton * m_btnTransSend;
    qUIPushButton * m_btnDsskey;
    qIMEButton  *  m_btnIME;
#if IF_BUG_25055
    // 扩展按键，用于替换IME
    qUIPushButton * m_btnExtension;
#endif
};

////////////////继承拨号盘，重写IME按钮为全键盘按钮////////////////////////////
class KeyPadDialPad : public DialPad
{
    Q_OBJECT

public:
    KeyPadDialPad(QWidget * parent = 0);
    virtual ~KeyPadDialPad();

public:

private:
    qKeypadButton* m_btnKeypad;
};

#endif //NUMBERPAD_H
