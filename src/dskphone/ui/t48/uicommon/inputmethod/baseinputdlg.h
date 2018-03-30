#pragma once
#include <list>
#include <QtGui/QMouseEvent>
#include <QTimer>
#include "datastruct.h"
#include "qtcommon/qmisc.h"
//keymap
#include "globalinputhook/ckeymap.h"
#include <QtGui/QWidget>
#include <QtGui/QDialog>
#include <QtGui/QPalette>
#include "uikernel/quipushbutton.h"
#include "inputbutton.h"
#include "longpressbutton.h"

#ifdef CUSTOME_KEYBOARD
#include "inputmanagerdefine.h"
#endif

#define VIRTUAL_KEYBOARD_LINE_COUNT     4
typedef std::vector<CInputButton*> vecButton;

const QRect RECT_VIRTUAL_KEYBOARD_1_LINE(0, g_iFullKeyBoardY + 0, g_iFullKeyBoardWidth, 110);
const QRect RECT_VIRTUAL_KEYBOARD_2_LINE(0, g_iFullKeyBoardY + 110, g_iFullKeyBoardWidth, 102);
const QRect RECT_VIRTUAL_KEYBOARD_3_LINE(0, g_iFullKeyBoardY + 212, g_iFullKeyBoardWidth, 103);
const QRect RECT_VIRTUAL_KEYBOARD_4_LINE(0, g_iFullKeyBoardY + 315, g_iFullKeyBoardWidth, 112);

using namespace std;

class CBaseInputDlg : public QWidget
{
    Q_OBJECT
public:
    CBaseInputDlg(void);
    ~CBaseInputDlg(void);

    virtual void Init();
    // 获取全键盘类型
    virtual INPUT_DLGTYPE GetDlgType() = 0;

    virtual EnterKeyType EnterType()
    {
        return m_eEnterType;
    }

#ifdef CUSTOME_KEYBOARD
    //获取Ime的类型
    virtual IME_TYPE GetImeType()
    {
        return IME_TYPE_ERROR;
    }
#endif

    // 设置全键盘样式
    virtual void SetInputDlgStyle(INPUT_DIALOG_STYLE enStyle);

    virtual bool ProcessMouseEvent(QMouseEvent *pMouseEvent);

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress)
    {
        return false;
    }

    virtual void ResetKeyBoard(bool bshowInputBox);

#ifdef CUSTOME_KEYBOARD
    //获取当前键盘对应的Ime的Index
    virtual int GetKeyboardImeIndex()
    {
        return KEYBOARD_IME_ERROR_INDEX;
    }
    //获取当前按下的行
    int GetCurrentKeyLine()
    {
        return m_nButtonLine;
    }
    //获取当前按下的按钮的ID（Index）
    int GetCurrentKeyID()
    {
        return m_nButtonID;
    }

    virtual void SetImeDisplayName(const yl::string& strName) { }
#endif
    //长按备选框是否已经弹出
    virtual bool IsInputBoxShow()
    {
        return false;
    }
    //长按备选框选中的Index
    virtual int GetInputBoxIndex()
    {
        return -1;
    }

    virtual void SetEnterType(EnterKeyType eEnterType)
    {
        m_eEnterType = eEnterType;
    }

protected:
    bool event(QEvent *event);

protected:
    // 创建全键盘按键
    virtual CInputButton* CreateButton(QRect qRect, int iKeyCode,
                                       QString qstrNormalText, QString qstrPressText,
                                       QColor qColorNormalText, QColor qColorPressText,
                                       QColor qColorNormalBackGround, QColor qColorPressBackGround,
                                       QString qstrNormalIcon, QString qstrPressIcon, KButtonType kButtonType = KButtonType_Normal);
    // 设置全键盘位置
    virtual void SetPos();

    // 加载背景图等
    virtual void LoadBkGroundPic() = 0;
    // 创建全键盘按键
    virtual void NewButton() = 0;

protected slots:

    //void OnLongPress(int iKeyCode);

    void OnBtnClick(int iKeyCode);

signals:
    void SignalLongPress(int iKeyCode);

    void SignalBtnClick(int iKeyCode);

protected:
    void paintEvent(QPaintEvent* pe);

    void SetEnterBtnStyle(INPUT_DIALOG_STYLE enStyle);
protected:
    vecButton       m_arrButtonVec[VIRTUAL_KEYBOARD_LINE_COUNT];

    CInputButton* m_pInputButtonEnter;
    CInputButton* m_pInputButtonDelete;

    QPixmap m_qCurrPixMap;

    //如果已经有一个按钮处于按下状态
    bool            m_bIfOneButtonDown;
    //正在处于按下的按钮的行号（-1代表没有被按下的按钮）
    int         m_nButtonLine;
    //正在处于按下的按钮的ID（-1代表没有被按下的按钮）
    int         m_nButtonID;

    //
    EnterKeyType        m_eEnterType;
};
