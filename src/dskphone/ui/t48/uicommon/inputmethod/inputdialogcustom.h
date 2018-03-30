#ifndef INPUTDIALOGCUSTOM_H
#define INPUTDIALOGCUSTOM_H

#include "baseinputdlg.h"
#include "inputmanagerdefine.h"
#include "inputboxbutton.h"
#include "custombutton.h"

typedef std::vector< CCustomButton* > vecButtonLine;
typedef std::vector< vecButtonLine > vecButtonTable;

class CInputDialogCustom : public CBaseInputDlg
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    virtual QString GetTestInfo();
#endif
public:
    CInputDialogCustom(int nImeIndex);
    ~CInputDialogCustom();

    //获取对应Ime的Index
    virtual int GetKeyboardImeIndex()
    {
        return m_nImeIndex;
    }
    //获取全键盘类型
    virtual INPUT_DLGTYPE GetDlgType();
    //获取输入类型
    virtual IME_TYPE GetImeType()
    {
        return m_eImeType;
    }

    //长按备选框是否已经弹出
    virtual bool IsInputBoxShow()
    {
        return m_bIsInputBoxShow;
    }
    //长按备选框选中的Index
    virtual int GetInputBoxIndex()
    {
        return m_nInputBoxIndex;
    }

    virtual void SetImeDisplayName(const yl::string& strName);
#ifndef IF_LYNC
    virtual void SetInputDlgStyle(INPUT_DIALOG_STYLE enStyle);
#endif
public:
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

    virtual bool ProcessMouseEvent(QMouseEvent *pMouseEvent);

    virtual bool ProcessInputBoxMouseEvent(QMouseEvent *pMouseEvent);

    virtual void ResetKeyBoard(bool bClearInputBox);

    virtual void SetEnterType(EnterKeyType eEnterType);

protected:
    bool event(QEvent *event);
    void paintEvent(QPaintEvent* pe);

protected slots:
    void OnLongPressToBox(bool bShowOption);

    void OnInputBoxPress();
protected:
    // 加载背景图，绘制按钮
    virtual void LoadBkGroundPic();
    // 创建全键盘按键
    virtual void NewButton();
    //根据ime信息，创建一个按钮
    virtual CCustomButton* CreateCustomButton(const SKeyBoardImeKey& tImeKey, const SKeyboardLayoutKey& tLayoutKey);

private:
    //根据功能按键的名字，设置按键的特殊显示
    void ManagerButtonFuction(const yl::string& strFunction, CCustomButton* pInputButton);
    //根据字符按键的备选输入内容，设置按键的显示
    void ManagerButtonChar(const SKeyBoardImeKey& tImeKey, CCustomButton* pInputButton);

    //根据字符串列表，加载备选框
    bool SetCharToInputButton(vecInputBoxString& vecC, bool bShowOption, yl::string& strKeyFuntion);

private:
    //将按钮先绘制到背景上

    // 现只支持 两态，按下和高亮是一个背景图
    //绘制按钮背景
    void paintButtonBackground(QPainter& pixPainter, CCustomButton* pButtonKey, bool bPressed = false);
    //绘制按钮图标
    void paintButtonIcon(QPainter& pixPainter, CCustomButton* pButtonKey, bool bPressed = false);
    //绘制按钮文字
    void paintButtonText(QPainter& pixPainter, CCustomButton* pButtonKey, bool bPressed = false);

private:
    //标识当前Ime的序号，用作INPUT_DLGTYPE的标识
    int                         m_nImeIndex;
    //标识当前Ime的类型
    IME_TYPE                m_eImeType;

    //
    INPUT_DIALOG_STYLE      m_eDlgStyle;
    //Ime显示名
    yl::string                  m_strDisplayName;

    //键盘本身的全局位置
    QRect                       m_GlobleRect;

    //记录最后按下的位置
    QPoint                      m_nGlobalPos;

    //记录所有按钮的列表
    vecButtonTable      m_listButton;

    //记录ime切换的按钮
    vecButtonLine       m_listImeButton;
    //记录回车键
    vecButtonLine       m_listEnterButton;

    //记录function键
    vecButtonLine       m_listFunctionButton;

    //备选框已经显示出来
    bool            m_bIsInputBoxShow;
    //备选框选中的Index
    int         m_nInputBoxIndex;
};

#endif // INPUTDIALOGCUSTOM_H
