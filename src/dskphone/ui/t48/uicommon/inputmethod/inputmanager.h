#pragma once
#include <list>
#include <map>
#include "datastruct.h"
#include "inputdlgmanager.h"
#include "fontsmanager.h"
#include <QObject>
#include <QtGui/QWidget>

class CTopWndChangeInfo;
using namespace std;
class CInputManager : public QObject
{
    Q_OBJECT
public:
    CInputManager(void);
    ~CInputManager(void);

    static CInputManager& GetInstance();

    void Init();

    // 设置控件可以切换的输入法
    void InstallIME(QWidget* p, QString str);

    // 设置控件绑定的全键盘初始化类型和样式
    void qSetKeyBoard(QWidget* p, bool bIsAutoShow, INPUT_DLGTYPE enTyep, INPUT_DIALOG_STYLE enStyle = INPUT_DIALOG_NORMAL);

    // 设置控件初始化显示的键盘类型
    void qSetKeyBoardType(QWidget* p, INPUT_DLGTYPE enType);

    //
    void qSetEnterType(EnterKeyType eEnterType);

    void qRaiseFullKeyboard();

    // 将预编辑的字符串输入控件，并返回控件上的内容
    QString qReturnEdit(QWidget* pWidget);

    // 输入预编辑内容
    // 返回true表示处于预编辑状态
    // 返回false表示不处于预编辑状态
    bool InputPreeditContent(QWidget* pWidget);
    bool IsPreedit(QWidget* pWidget);
    // 处理删除功能
    void DealBackspace(QWidget* pWidget);

    // 显示全键盘
    void ShowInputDlg(const QWidget* _pQwidget);

    // 隐藏全键盘
    void HideInputDlg(const QWidget* _pQwidget);

    //强制让全键盘隐藏  与qSetIsShowWnd 接口不同是  这边不用参数 直接隐藏
    //目前主要给 t49-premise 会议弹框使用
    void ForceHideInputDlg();

    // 禁止显示全键盘
    void BlockKeyboard(bool bBlock = false);
    // 获取全键盘是否显示
    bool IsShowKeyboard();

    // 切换到下一个输入法
    QString ChangeToNextFont(QWidget* p);
    // 获取keycode对应的第一个字符
    QString GetIMEFirstChar(QString lpszIMEName, int iKeyCode);
    // 获取当前输入法显示名称
    QString GetCurrIMETitle(const QWidget* p);

    QString GetCurrIMEName(const QWidget* p);

    void ReloadIMEFile();

    void SetCurrentIME(QWidget* p, QString lpszIMEName);

    void SetCurrentIMEByName(const QWidget* p, QString lpszIMEName);

    // 获取控件的所有输入法名称
    QString GetAllIMEName(const QWidget* p);

    void SetQTextEditMaxLength(const QWidget* pWidget, int imaxLen);

    int GetQTextEditMaxLength(const QWidget* pWidget);

    void SetNameFormat(QWidget* pWidget, bool bNameFormat);
    // 是否播放动画
    void SetIsPlayKeyboardAnimation(bool bPlay);

    void UseKeyBoardFromIME(QWidget* pWidget, INPUT_DIALOG_STYLE enStyle = INPUT_DIALOG_NORMAL, bool bIsAutoShow = false);

    bool ChangeKeyBoardStyle(const QWidget* pWidget, INPUT_DIALOG_STYLE enStyle = INPUT_DIALOG_NORMAL);

    const QWidget* GetCurrInputWgt();

    //将因全键盘变化引起的界面变化所需要执行的操作注册到输入法管理模块中
    bool RegisterKeyBoardShowHookProc(KEYBOARD_SHOW_HOOK_PROC keyboardShowHook, KEYBOARD_SHOW_HOOK_MODULE priority);
    bool UnRegisterKeyBoardShowHookProc(KEYBOARD_SHOW_HOOK_PROC keyboardShowHook);

signals:
    void SignKeyboardShow();

    void SignKeyboardHide();

    void SignKeyboardBeforeShow();

    void SignKeyboardBeforeHide();

public slots:
    // 控件注销输入法
    bool ReleaseWidget(QObject *pObj);

    void OnFocusChanged(QWidget* pOld, QWidget* pNow);

public:
    // 事件过滤器
    bool eventFilter(QObject *pObj, QEvent *pEvent);
private:

    // 获取到焦点的操作
    bool OnFocusIn(QWidget* pWidget);

    // 失去焦点的操作
    bool OnFocusOut(QWidget* pWidget);
    // 处理鼠标点击的操作
    bool OnMousePress(const QObject *pObj);
    // 处理按键点击事件
    bool OnKeyPress(QObject *pObj, int iKeyCode);

    bool OnKeyRelease(QObject *pObj, int iKeyCode);

    INPUT_DLGTYPE SetKeyBoardTypeAndStyle(const QWidget* p);

    bool OnEventHide(const QObject *pObj);

    INPUT_DLGTYPE GetKeyBoardTypeByFont(const std::string& strFontName);
private slots:
    // 全键盘发生切换时需要处理的事情
    void OnDlaTypeChange(INPUT_DLGTYPE enType);

    static bool InputDlgKeyHookCallbackFun(int nKeyCode, bool bPress);
    bool ProcessKeyEvent(int nKeyCode, bool bPress);

    static bool InputDlgMouseHookCallbackFun(QObject* pObject, QMouseEvent *pEvent);
    bool ProcessMouseEvent(QObject* pObject, QMouseEvent *pEvent);

    //static void InputDlgOnTopWndChange(const CTopWndChangeInfo& objInfo);
    //void ProcessOnTopWndChange(const CTopWndChangeInfo& objInfo);

    //private:
//  std::list<CWidgetInfo> m_lstWidgetInfo;

private:
    //CWidgetInfo*      m_pCurrWidgetInfo;
    CInputDlgManager    m_inputdlgmanager;
    CFontsManager       m_FontsManager;

    bool m_bBlockKeyboard;
};
#define g_inputmanager CInputManager::GetInstance()
