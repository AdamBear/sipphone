#pragma once
#include <map>
using namespace std;
#include "baseinputdlg.h"
//#include "animationframe/modanimationframe.h"
#include "globalinputhook/inputhook_include.h"
#include "datastruct.h"

enum ANIMATIONSTATE
{
    ANIMATION_EXIST = 0,            // 生存中
    ANIMATION_STOPING,              // 正在停止
    ANIMATION_STOP,                 // 终止
};

enum KEYBOARD_SHOW_HOOK_MODULE
{
    KEYBOARD_SHOW_HOOK_MODULE_ERROR = -1,
    KEYBOARD_SHOW_HOOK_MODULE_MESSAGEBOX,
    KEYBOARD_SHOW_HOOK_MODULE_MAINWND,
};

typedef bool (*KEYBOARD_SHOW_HOOK_PROC)(bool bKeyBoardShow);
typedef QMap<KEYBOARD_SHOW_HOOK_MODULE, KEYBOARD_SHOW_HOOK_PROC> KEYBOARD_SHOW_HOOK_MAP;

class CInputDlgManager : public QObject
{
    Q_OBJECT

public:
    CInputDlgManager(void);
    ~CInputDlgManager(void);
public:
    void            Init();
    void            Release();

#ifdef CUSTOME_KEYBOARD
    void            InitCustomKeyboard();
#endif

public:
    // 根据类型设置全键盘
    bool SetInputDlgByType(INPUT_DLGTYPE enType);

    // 设置全键盘进行显示
    void ShowInputDlg(const INPUT_DLGTYPE enType);

    // 设置全键盘样式
    void SetInputDlgStyle(INPUT_DIALOG_STYLE enStyle);

    void SetEnterType(EnterKeyType eEnterType);

    void RaiseFullKeyboard();

    // 全键盘是否显示
    bool IsDlgShow();
    // 是否播放动画
    void SetIsPlayKeyboardAnimation(bool bPlay);
    // 立即隐藏全键盘
    void HideInputDlgImmediately();

    void HideInputDlgAfter();

    void DealDlgToHide();

    bool ProcessKeyEvent(int nKeyCode, bool bPress);

    bool ProcessMouseEvent(QObject* pObject, QMouseEvent *pEvent);

    //将因全键盘变化引起的界面变化所需要执行的操作注册到输入法管理模块中
    bool RegisterKeyBoardShowHookProc(KEYBOARD_SHOW_HOOK_PROC keyboardShowHook, KEYBOARD_SHOW_HOOK_MODULE priority);
    bool UnRegisterKeyBoardShowHookProc(KEYBOARD_SHOW_HOOK_PROC keyboardShowHook);

private:
    // 根据类型获取全键盘
    CBaseInputDlg*  CreatInputDlgByType(INPUT_DLGTYPE enType);
    // 切换全键盘类型
    void ChangeDlgType(INPUT_DLGTYPE enType);

    //QAbstractAnimation* PlayAnimation( QWidget* pAnimWidget, bool IsShow, ANIMATION_TYPE AniType, int nTime);

    void ProcessKeyBoardShow(bool bKeyBoardShow);

private:
    // 启动计时器
    void startTimer();

    // 停止计时器
    void stopTimer();
    // 计时器事件
    void timerEvent(QTimerEvent* pEvent);

    bool SetAfterHideDlg();

#ifdef CUSTOME_KEYBOARD
    void ProcessCustomBtnClick();
    void ProcessCustomEnter();
#endif

private slots:
    // 全键盘类型发生变化
    void OnBtnClick(int iKeyCode);

    //void OnAnimationStateChange(QAbstractAnimation::State newState,
    //                          QAbstractAnimation::State oldState);

    //// 切换全键盘
    //bool  ChangeInputDlg(int iKeyCode);
public:
signals:
    // 全键盘切换时发射的切换信号
    void SignChangeDlg(INPUT_DLGTYPE enType);

    void SignKeyboardShow();

    void SignKeyboardHide();

    void SignKeyboardBeforeShow();

    void SignKeyboardBeforeHide();
private:
    std::map<INPUT_DLGTYPE, CBaseInputDlg*> m_mapInputDlgs;
    CBaseInputDlg*      m_pInputDlg;
    CBaseInputDlg*      m_pInputDlg_AfterHide;
    INPUT_DLGTYPE       m_byCurInputDlgType;
    bool                m_bInputDlgIsShow;
    INPUT_DIALOG_STYLE  m_byStyle;
    EnterKeyType        m_eEnterType;
private:
    // 定时器，延时隐藏
    int m_nTimerID;
    //QAbstractAnimation* m_pAnimation; // 动画对象
    ANIMATIONSTATE      m_enAnimationState;// 动画是否生存
    bool                m_bPlayAnimation;

    KEYBOARD_SHOW_HOOK_MAP      m_mapHookProc;

};
