#include "modinputhook.h"
#include "inputhookmanager.h"
#include "ckeymap.h"
#ifdef WIN32
#include "mainwnd/mainwnd.h"
#else
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

//重发按键
void inputHook_SendKey(int nKeyCode, bool bPress)
{
    int nUiKeyCode = nKeyCode;
    CKeyMap * pKeyMap = CKeyMap::GetKeyMap();
    if (pKeyMap != NULL)
    {
        nUiKeyCode = pKeyMap->MapPhoneKeyToUiKey(nKeyCode, bPress);
    }
#ifdef WIN32
    //先都发给mainwnd
    QApplication::postEvent(&_MainWND, new QKeyEvent(bPress ? QEvent::KeyPress : QEvent::KeyRelease,
                            nUiKeyCode, Qt::NoModifier));
#else
    QWSServer::processKeyEvent(0xffff, nUiKeyCode, Qt::NoModifier, bPress, false);
#endif
}

// 全键盘重发按键
// 使用post会有延迟，故改为send
void inputHook_FullKb_Sendkey(int nKeyCode, bool bPress)
{
    QWidget* pKeyWidget = QApplication::focusWidget();

    if (NULL == pKeyWidget)
    {
        pKeyWidget = QApplication::activeWindow();

        if (NULL == pKeyWidget)
        {
            return;
        }
    }

    CKeyMap* pKeyMap = CKeyMap::GetKeyMap();
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

void inputHook_ApplicationSendKey(int nKeyCode, bool bPress)
{
    QWidget * pKeyWidget = QApplication::focusWidget();

    if (NULL == pKeyWidget)
    {
        pKeyWidget = QApplication::activeWindow();

        if (NULL == pKeyWidget)
        {
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
