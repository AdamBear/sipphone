#include "modinputhook.h"
#include "inputhookmanager.h"
#include "ckeymap.h"
#include "dsklog/log.h"
#ifndef WIN32
#include <QWSServer>
#endif


//启用输入钩子
void inputHook_EnableHook(HOOK_TYPE eHookType)
{
    CInputHookManager::GetInputHookManager().SetInputHook(true, eHookType);
}

//禁用输入钩子
void inputHook_DisableHook(HOOK_TYPE eHookType)
{
    CInputHookManager::GetInputHookManager().SetInputHook(false, eHookType);
}

void inputHook_SendKey(int nKeyCode, bool bPress)
{
    QWidget * pKeyWidget = QApplication::focusWidget();

    if (NULL == pKeyWidget)
    {
        INPUTMETHOD_WARN("inputHook_SendKey focusWidget is empty.");

        pKeyWidget = QApplication::activeWindow();

        if (NULL == pKeyWidget)
        {
            INPUTMETHOD_ERR("inputHook_SendKey return when KeyWidget is empty.");
            return;
        }
    }

    CKeyMap * pKeyMap = CKeyMap::GetKeyMap();
    int nUiKeyCode = nKeyCode;

    if (NULL != pKeyMap)
    {
        nUiKeyCode = pKeyMap->MapPhoneKeyToUiKey(nKeyCode, bPress);
    }

    QEvent::Type eType = QEvent::KeyRelease;

    if (bPress)
    {
        eType = QEvent::KeyPress;
    }

    QKeyEvent keyEvent(eType, nUiKeyCode, Qt::NoModifier);

    QApplication::sendEvent(pKeyWidget, &keyEvent);
}

void inputHook_PostKey(int nKeyCode, bool bPress)
{
    CKeyMap * pKeyMap = CKeyMap::GetKeyMap();
    int nUiKeyCode = nKeyCode;

    if (NULL != pKeyMap)
    {
        nUiKeyCode = pKeyMap->MapPhoneKeyToUiKey(nKeyCode, bPress);
    }
#ifndef WIN32
    QWSServer::processKeyEvent(0xffff, nUiKeyCode, Qt::NoModifier, bPress, false);
#endif
}

void inputHook_RegisterKeyPadHookProc(KEYPAD_HOOK_PROC keypadHook, HOOK_MODULE priority)
{
    CInputHookManager::GetInputHookManager().RegisterKeyPadHookProc(keypadHook, priority);
}

void inputHook_UnRegisterKeyPadHookProc(KEYPAD_HOOK_PROC keypadHook)
{
    CInputHookManager::GetInputHookManager().UnRegisterKeyPadHookProc(keypadHook);
}

void inputHook_RegisterMouseHookProc(MOUSE_HOOK_PROC mouseHook, HOOK_MODULE priority)
{
    CInputHookManager::GetInputHookManager().RegisterMouseHookProc(mouseHook, priority);
}

void inputHook_UnRegisterMouseHookProc(MOUSE_HOOK_PROC mouseHook)
{
    CInputHookManager::GetInputHookManager().UnRegisterMouseHookProc(mouseHook);
}
