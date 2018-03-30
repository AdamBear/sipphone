#include "inputhookmanager.h"
#include "ETLLib.hpp"
#include "baseui/include/baseuicommon.h"


#if IF_MULTI_WORKSTATION_ENBALE
IMPLEMENT_MULTI_WORKSTATION_INSTANCE(CInputHookManager)
#else
IMPLEMENT_WORKSTATION_INSTANCE(CInputHookManager)
#endif

CInputHookManager::CInputHookManager()
{
}

CInputHookManager::~CInputHookManager()
{
}

void CInputHookManager::SetInputHook(bool bInputHook, HOOK_TYPE eHookType)
{
    //设置键盘钩子状态
    if (eHookType == HT_ALL
            || eHookType == HT_KEYPAD)
    {
        m_objKeypadHook.SetKeypadHook(bInputHook);
    }

    //设置鼠标钩子状态
    if (eHookType == HT_ALL
            || eHookType == HT_MOUSE)
    {
        m_objMouseHook.SetMouseHook(bInputHook);
    }
}

//注册按键处理回调函数
bool CInputHookManager::RegisterKeyPadHookProc(KEYPAD_HOOK_PROC keypadHook, HOOK_MODULE priority)
{
    return m_objKeypadHook.RegisterHookProc(keypadHook, priority);
}

//注销按键处理回调函数
bool CInputHookManager::UnRegisterKeyPadHookProc(HOOK_MODULE priority)
{
    return m_objKeypadHook.UnRegisterHookProc(priority);
}

//注册鼠标处理回调函数
bool CInputHookManager::RegisterMouseHookProc(MOUSE_HOOK_PROC mouseHook, HOOK_MODULE priority)
{
    return m_objMouseHook.RegisterHookProc(mouseHook, priority);
}

//注销鼠标处理回调函数
bool CInputHookManager::UnRegisterMouseHookProc(HOOK_MODULE priority)
{
    return m_objMouseHook.UnRegisterHookProc(priority);
}

#if IF_INPUT_HOOK_KEEP_KEY_RAW_VALUE
int CInputHookManager::GetCurrentRawKey() const
{
    return m_objKeypadHook.GetCurrentRawKey();
}

void CInputHookManager::SetRawKey(int nKey)
{
    m_objKeypadHook.SetRawKey(nKey);
}
#endif

//键盘事件处理函数
bool CInputHookManager::processKeyInput(int key, bool bPressed)
{
    //直接将按键事件传递给按键钩子处理
    if (m_objKeypadHook.processKeyInput(key, bPressed))
    {
        return true;
    }

    return false;
}

//鼠标事件处理函数
bool CInputHookManager::processMouseInput(int key, bool bPressed)
{
    //直接将按键事件传递给按键钩子处理
    if (m_objMouseHook.processMouseInput(key, bPressed))
    {
        return true;
    }

    return false;
}
