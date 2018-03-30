#include "modinputhook.h"
#include "inputhookmanager.h"
#include "keymap.h"
#include "xwindow/xWindowManager.h"

//启用输入钩子
void inputHook_EnableHook(HOOK_TYPE eHookType)
{
    _InputHookManager.SetInputHook(true, eHookType);
}

//禁用输入钩子
void inputHook_DisableHook(HOOK_TYPE eHookType)
{
    _InputHookManager.SetInputHook(false, eHookType);
}

//重发按键
void inputHook_SendKey(int nKeyCode, bool bPress)
{
    g_WorkStation.dispatchKeypadEvent(nKeyCode, bPress);
}

void inputHook_RegisterKeyPadHookProc(KEYPAD_HOOK_PROC keypadHook, HOOK_MODULE priority)
{
    _InputHookManager.RegisterKeyPadHookProc(keypadHook, priority);
}

void inputHook_UnRegisterKeyPadHookProc(HOOK_MODULE priority)
{
    _InputHookManager.UnRegisterKeyPadHookProc(priority);
}

void inputHook_RegisterMouseHookProc(MOUSE_HOOK_PROC mouseHook, HOOK_MODULE priority)
{
    _InputHookManager.RegisterMouseHookProc(mouseHook, priority);
}

void inputHook_UnRegisterMouseHookProc(HOOK_MODULE priority)
{
    _InputHookManager.UnRegisterMouseHookProc(priority);
}

bool inputHook_ProcessKeyInput(int nKey, bool bPressed)
{
    return _InputHookManager.processKeyInput(nKey, bPressed);
}

bool inputHook_ProcessMouseInput(int nKey, bool bPressed)
{
    return _InputHookManager.processMouseInput(nKey, bPressed);
}

#if IF_INPUT_HOOK_KEEP_KEY_RAW_VALUE
int inputHook_GetCurrentRawKey()
{
    return _InputHookManager.GetCurrentRawKey();
}

void inputHook_SetRawKey(int nKey)
{
    _InputHookManager.SetRawKey(nKey);
}
#endif
