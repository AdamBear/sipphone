#include "mousehook.h"
#include <QMouseEvent>
#include <QObject>
#include "dsklog/log.h"

////----qt
#include "uimanager/basemainwnd.h"
#include "uimanager/moduimanager.h"

CMouseHook::CMouseHook(void)
{
    m_mapHookProc.clear();
    m_bMouseHook = false;
}

CMouseHook::~CMouseHook(void)
{
    m_mapHookProc.clear();
    m_bMouseHook = false;
}

bool CMouseHook::RegisterHookProc(MOUSE_HOOK_PROC mouseHook, HOOK_MODULE priority)
{
    if (mouseHook != NULL)
    {
        MOUSE_HOOK_MAP::iterator itProc = m_mapHookProc.begin();
        for (; itProc != m_mapHookProc.end(); itProc++)
        {
            MOUSE_HOOK_PROC pHookProc = itProc.value();
            if (pHookProc != NULL
                    && pHookProc == mouseHook)
            {
                //查找到相同回调指针，不需重复添加
                return true;
            }
        }
        //不用map.contains的理由是contains只能判断key不能判断value
        m_mapHookProc.insertMulti(priority, mouseHook);
    }
    return false;
}

bool CMouseHook::UnRegisterHookProc(MOUSE_HOOK_PROC mouseHook)
{
    if (mouseHook != NULL)
    {
        MOUSE_HOOK_MAP::iterator itProc = m_mapHookProc.begin();
        for (; itProc != m_mapHookProc.end(); itProc++)
        {
            MOUSE_HOOK_PROC pHookProc = itProc.value();
            if (pHookProc != NULL
                    && pHookProc == mouseHook)
            {
                //查找到相应的回调指针，从链表中清除
                m_mapHookProc.erase(itProc);
                return true;
            }
        }
    }
    return false;
}

bool CMouseHook::GetMouseHook()
{
    return m_bMouseHook;
}

void CMouseHook::SetMouseHook(bool bMouseHook)
{
    m_bMouseHook = bMouseHook;
}

bool CMouseHook::filter(QObject * pObject, QMouseEvent * pEvent)
{
    if (!m_bMouseHook
            || pObject == NULL
            || pEvent == NULL)
    {
        //不进行鼠标事件拦截处理
        return false;
    }

    //TO DO:其他系统模块处理???

    //外部模块注册回调调用
    for (int i = HOOK_MODULE_PREHOOKBEGIN; i <= HOOK_MODULE_PREHOOKEND; ++i)
    {
        MOUSE_HOOK_MAP::iterator itProc = m_mapHookProc.find(HOOK_MODULE(i));
        if (itProc != m_mapHookProc.end())
        {
            MOUSE_HOOK_PROC pHookProc = itProc.value();
            if (pHookProc != NULL)
            {
                if (pHookProc(pObject, pEvent))
                {
                    return true;
                }
            }
        }
    }


    //所有模块处理完成，交由UIManager处理
    //!!!!!
    CBaseMainWnd * pMainWnd = UIManager_GetMainWnd();
    if (pMainWnd != NULL
            && pMainWnd->eventFilter(pObject, pEvent))
    {
        return true;
    }

    return false;
}
