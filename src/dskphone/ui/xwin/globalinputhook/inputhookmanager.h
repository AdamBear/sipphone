#ifndef __INPUT_HOOK_MANAGER_H__
#define __INPUT_HOOK_MANAGER_H__

#include "inputhook_include.h"
#include "keypadhook.h"
#include "mousehook.h"

class CInputHookManager
{
private:
    CInputHookManager();
    virtual ~CInputHookManager();

public:
    static CInputHookManager & GetInstance();

public:
    //设置输入事件钩子状态
    void SetInputHook(bool bInputHook, HOOK_TYPE eHookType = HT_ALL);

    //事件过滤器
    virtual bool processKeyInput(int key, bool bPressed);
    virtual bool processMouseInput(int key, bool bPressed);

    //注册按键处理回调函数
    bool RegisterKeyPadHookProc(KEYPAD_HOOK_PROC keypadHook, HOOK_MODULE priority);

    //注销按键处理回调函数
    bool UnRegisterKeyPadHookProc(HOOK_MODULE priority);

    //注册鼠标处理回调函数
    bool RegisterMouseHookProc(MOUSE_HOOK_PROC mouseHook, HOOK_MODULE priority);

    //注销鼠标处理回调函数
    bool UnRegisterMouseHookProc(HOOK_MODULE priority);

#if IF_INPUT_HOOK_KEEP_KEY_RAW_VALUE
    int GetCurrentRawKey() const;
    void SetRawKey(int nKey);
#endif

private:
    CKeypadHook     m_objKeypadHook;
    CMouseHook      m_objMouseHook;

};

#define _InputHookManager (CInputHookManager::GetInstance())

#endif
