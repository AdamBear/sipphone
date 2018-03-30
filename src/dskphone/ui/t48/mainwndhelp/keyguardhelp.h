#ifndef _KEY_GUARD_HOOK_H_
#define _KEY_GUARD_HOOK_H_


#include "uicommon/messagebox/modmessagebox.h"

class CKeyGuardHelper : public QObject, public CMsgBoxCallBackAction
{
    Q_OBJECT

public:
    CKeyGuardHelper();
    virtual ~CKeyGuardHelper();

public:
    static CKeyGuardHelper * GetInstance();

    static void OnTopWndChangeCallbackFun(CBaseDialog * dlgPreTop, CBaseDialog * dlgNewTop);
    static LRESULT OnMessage(msgObject & refMessage);
    static void RemoveUnlockBox();

    void KeyGuardHelp_Init();

    // 关闭键盘锁的计时器
    void StopKeyLockTimer();
    // 重置计时器
    void ResetKeypadLock();

    bool IsTimerActive();

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

private:
    // 启动键盘锁的计时器
    void StartKeyLockTimer();

private slots:
    // 锁定键盘的计时器时间到
    void OnKeyLockTimerTimeout();

private:
    // 自动锁定键盘的计时器
    QTimer m_timerKeyLock;
};

#define g_pKeyGuardHelper (CKeyGuardHelper::GetInstance())
#endif // _KEY_GUARD_HOOK_H_
