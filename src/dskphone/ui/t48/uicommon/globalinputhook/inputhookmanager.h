#ifndef __INPUT_HOOK_MANAGER_H__
#define __INPUT_HOOK_MANAGER_H__

#include <QtGui>
#include "keypadhook.h"
#include "mousehook.h"

#include "inputhook_include.h"

class CInputHookManager : public QObject
{
    Q_OBJECT
public:
    CInputHookManager(void);
    virtual ~CInputHookManager(void);

public:
    static CInputHookManager & GetInputHookManager();

public:
    //设置输入事件钩子状态
    void SetInputHook(bool bInputHook, HOOK_TYPE eHookType = HT_ALL);

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    //注册按键处理回调函数
    bool RegisterKeyPadHookProc(KEYPAD_HOOK_PROC keypadHook, HOOK_MODULE priority);

    //注销按键处理回调函数
    bool UnRegisterKeyPadHookProc(KEYPAD_HOOK_PROC keypadHook);

    //注册鼠标处理回调函数
    bool RegisterMouseHookProc(MOUSE_HOOK_PROC mouseHook, HOOK_MODULE priority);

    //注销鼠标处理回调函数
    bool UnRegisterMouseHookProc(MOUSE_HOOK_PROC mouseHook);

private:
    CKeypadHook     m_objKeypadHook;
    CMouseHook      m_objMouseHook;
};

#endif //__INPUT_HOOK_MANAGER_H__
