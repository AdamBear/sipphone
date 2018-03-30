#include "inputhookmanager.h"

CInputHookManager::CInputHookManager(void)
{
}

CInputHookManager::~CInputHookManager(void)
{
    qApp->removeEventFilter(this);
}

CInputHookManager & CInputHookManager::GetInputHookManager()
{
    static CInputHookManager objInputHookManager;
    return objInputHookManager;
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

    if (m_objKeypadHook.GetKeypadHook()
            || m_objMouseHook.GetMouseHook())
    {
        //添加事件过滤器
        qApp->installEventFilter(this);
    }
    else
    {
        //移除事件过滤器
        qApp->removeEventFilter(this);
    }
}

//注册按键处理回调函数
bool CInputHookManager::RegisterKeyPadHookProc(KEYPAD_HOOK_PROC keypadHook, HOOK_MODULE priority)
{
    return m_objKeypadHook.RegisterHookProc(keypadHook, priority);
}

//注销按键处理回调函数
bool CInputHookManager::UnRegisterKeyPadHookProc(KEYPAD_HOOK_PROC keypadHook)
{
    return m_objKeypadHook.UnRegisterHookProc(keypadHook);
}

//注册鼠标处理回调函数
bool CInputHookManager::RegisterMouseHookProc(MOUSE_HOOK_PROC mouseHook, HOOK_MODULE priority)
{
    return m_objMouseHook.RegisterHookProc(mouseHook, priority);
}

//注销鼠标处理回调函数
bool CInputHookManager::UnRegisterMouseHookProc(MOUSE_HOOK_PROC mouseHook)
{
    return m_objMouseHook.UnRegisterHookProc(mouseHook);
}

//事件过滤函数
bool CInputHookManager::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }
    switch (pEvent->type())
    {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        {
            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            //直接将按键事件传递给按键钩子处理
            if (m_objKeypadHook.filter(pObject, pKeyEvent))
            {
                return true;
            }
        }
        break;
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
        {
            QMouseEvent * pMouseEvent = static_cast<QMouseEvent *>(pEvent);
            if (m_objMouseHook.filter(pObject, pMouseEvent))
            {
                return true;
            }
        }
        break;
    default:
        break;
    }

    return false;
}
