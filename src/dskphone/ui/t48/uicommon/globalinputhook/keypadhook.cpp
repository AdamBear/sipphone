#include <ETLLib/ETLLib.hpp>
#include "keypadhook.h"
#include "ckeymap.h"
#include "uimanager/basemainwnd.h"
#include "uimanager/moduimanager.h"
#include "inputmethod/inputmethod.h"
#include "dsklog/log.h"

CKeypadHook::CKeypadHook(void)
{
    m_mapHookProc.clear();
    m_bKeypadHook = false;
}

CKeypadHook::~CKeypadHook(void)
{
    m_mapHookProc.clear();
    m_bKeypadHook = false;
}

bool CKeypadHook::RegisterHookProc(KEYPAD_HOOK_PROC keypadHook, HOOK_MODULE priority)
{
    if (keypadHook != NULL)
    {
        //查找回调指针
        KEY_HOOK_MAP::iterator itProc = m_mapHookProc.begin();
        for (; itProc != m_mapHookProc.end(); itProc++)
        {
            KEYPAD_HOOK_PROC pHookProc = itProc.value();
            if (pHookProc != NULL
                    && pHookProc == keypadHook)
            {
                //查找到相同回调指针，不需重复添加
                return true;
            }
        }
        //将回调指针添入列表中
        m_mapHookProc.insertMulti(priority, keypadHook);
    }
    return false;
}

bool CKeypadHook::UnRegisterHookProc(KEYPAD_HOOK_PROC keypadHook)
{
    if (keypadHook != NULL)
    {
        //查找回调指针
        KEY_HOOK_MAP::iterator itProc = m_mapHookProc.begin();
        for (; itProc != m_mapHookProc.end(); itProc++)
        {
            KEYPAD_HOOK_PROC pHookProc = itProc.value();
            if (pHookProc != NULL
                    && pHookProc == keypadHook)
            {
                //查找到相应的回调指针，从链表中清除
                m_mapHookProc.erase(itProc);
                return true;
            }
        }
    }
    return false;
}

bool CKeypadHook::GetKeypadHook()
{
    return m_bKeypadHook;
}

void CKeypadHook::SetKeypadHook(bool bKeypadHook)
{
    m_bKeypadHook = bKeypadHook;

#ifdef USE_KEYBOARDFILTER
    if (m_bKeypadHook)
    {
        // 安装键盘过滤器
        qwsServer->addKeyboardFilter(this);
    }
    else
    {
        // 移除键盘过滤器
        QWSServer::removeKeyboardFilter();
    }
#endif
}

bool CKeypadHook::filter(QObject * pObject, QKeyEvent * pKeyEvent)
{
    if (!m_bKeypadHook
            || pObject == NULL
            || pKeyEvent == NULL)
    {
        //不进行按键拦截处理
        return false;
    }

    //获取按键属性
    int nKeycode = pKeyEvent->key();
    bool bPress = pKeyEvent->type() == QEvent::KeyPress ? true : false;

    //映射按键值
    int nPhoneKeyCode = nKeycode;

    CKeyMap * pKeyMap = CKeyMap::GetKeyMap();
    if (pKeyMap != NULL)
    {
        nPhoneKeyCode = pKeyMap->MapUiKeyToPhoneKey(nKeycode, bPress);
    }


#ifndef USE_KEYBOARDFILTER
    GLOBALINPUTHOOK_INFO("Global Key %s:(%x)", bPress ? "Down" : "Up", nKeycode);

    // window下没有键盘过滤机制,需要在这里对按键进行预处理
    if (PreProcessKey(nPhoneKeyCode, bPress))
    {
        return true;
    }
#endif

    //所有模块处理完成，交由UIManager处理
    QKeyEvent keyEvent(bPress ? QEvent::KeyPress : QEvent::KeyRelease, nPhoneKeyCode,
                       pKeyEvent->modifiers(), pKeyEvent->text(), pKeyEvent->isAutoRepeat(), pKeyEvent->count());
    //!!!!!
    CBaseMainWnd * pMainWnd = UIManager_GetMainWnd();
    if (pMainWnd != NULL
            && pMainWnd->eventFilter(pObject, &keyEvent))
    {
        return true;
    }

    //若Object为输入框，则将事件传递给IME处理
    if ((pObject->inherits("QLineEdit")
            || pObject->inherits("QTextEdit"))
            && qPassEventToEdit(pObject, &keyEvent))
    {
        return true;
    }

#ifdef WIN32
    // windows下用鼠标点数字键输入
    if (QApplication::focusWidget()
            && (QApplication::focusWidget()->inherits("QLineEdit")
                || QApplication::focusWidget()->inherits("QTextEdit"))
            && qPassEventToEdit(QApplication::focusWidget(), &keyEvent))
    {
        return true;
    }

    // xmlbrowser
    if (QApplication::focusWidget() != pMainWnd
            && UIManager_GetTopWindow()
            && UIManager_GetTopWindow()->GetWindowType() == eWT_XMLBrower
            && pMainWnd->eventFilter(QApplication::focusWidget(), &keyEvent))
    {
        return true;
    }
#endif
    //交由对应的对象处理
    pObject->event(pKeyEvent);

    //拦截该事件
    pKeyEvent->accept();

    return true;
}

#ifdef USE_KEYBOARDFILTER
bool CKeypadHook::filter(int unicode, int nKeyCode, int modifiers, bool bPress, bool autoRepeat)
{
    // 不进行事件的拦截
    if (!m_bKeypadHook)
    {
        return false;
    }

    // 进行按键的转换
    int nPhoneKeyCode = nKeyCode;
    CKeyMap * pKeyMap = CKeyMap::GetKeyMap();
    if (pKeyMap != NULL)
    {
        nPhoneKeyCode = pKeyMap->MapUiKeyToPhoneKey(nKeyCode, bPress);
    }

    if (!autoRepeat)
    {
        GLOBALINPUTHOOK_INFO("Global Key %s:(%x)", bPress ? "Down" : "Up", nPhoneKeyCode);
//      SYZ_TRACE_RT("Global Key %s:(%x)", bPress ? "Down" : "Up", nPhoneKeyCode);
    }

    // 对按键进行进入Qt事件前的预处理
    return PreProcessKey(nPhoneKeyCode, bPress);
}
#endif

bool CKeypadHook::PreProcessKey(int nPhoneKeyCode, bool bPress)
{
    //按键防抖
    if (m_objKeyJudder.setKeyCheck(nPhoneKeyCode, bPress))
    {
        return true;
    }

    for (int i = HOOK_MODULE_PREHOOKBEGIN; i <= HOOK_MODULE_PREHOOKEND; ++i)
    {
        KEY_HOOK_MAP::iterator itProc = m_mapHookProc.find(HOOK_MODULE(i));
        if (itProc != m_mapHookProc.end())
        {
            KEYPAD_HOOK_PROC pHookProc = itProc.value();
            if (pHookProc != NULL)
            {
                if (pHookProc(nPhoneKeyCode, bPress))
                {
                    return true;
                }
            }
        }
    }

    return false;
}
