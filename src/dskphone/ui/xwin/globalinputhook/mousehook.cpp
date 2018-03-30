#include "mousehook.h"
#include "dsklog/log.h"

CMouseHook::CMouseHook()
{
    m_mapHookProc.clear();
    m_bMouseHook = false;
}

CMouseHook::~CMouseHook()
{
    m_mapHookProc.clear();
    m_bMouseHook = false;
}

bool CMouseHook::RegisterHookProc(MOUSE_HOOK_PROC mouseHook, HOOK_MODULE priority)
{
    if (mouseHook != NULL)
    {
        MOUSE_HOOK_MAP::iterator iter = m_mapHookProc.find(priority);
        if (iter != m_mapHookProc.end())
        {
            //查找到相同回调指针，不重复添加
            return false;
        }
        else
        {
            m_mapHookProc.insert(priority, mouseHook);
        }
    }
    return true;
}

bool CMouseHook::UnRegisterHookProc(HOOK_MODULE priority)
{
    MOUSE_HOOK_MAP::iterator iter = m_mapHookProc.find(priority);
    if (iter != m_mapHookProc.end())
    {
        //查找到相应的回调指针，从链表中清除
        m_mapHookProc.erase(iter);
        return true;
    }
    else
    {
        return false;
    }
}

bool CMouseHook::GetMouseHook()
{
    return m_bMouseHook;
}

void CMouseHook::SetMouseHook(bool bMouseHook)
{
    m_bMouseHook = bMouseHook;
}

bool CMouseHook::processMouseInput(int key, bool bPressed)
{
    if (!m_bMouseHook)
    {
        //不进行鼠标事件拦截处理
        return false;
    }

    //TO DO:其他系统模块处理???

    //外部模块注册回调调用
    MOUSE_HOOK_MAP::iterator itProc = m_mapHookProc.begin();
    for (; itProc != m_mapHookProc.end(); ++itProc)
    {

        MOUSE_HOOK_PROC pHookProc = itProc->second;
        if (pHookProc != NULL)
        {
            if (pHookProc(key, bPressed))
            {
                return true;
            }
        }
    }

    //所有模块处理完成，交由xWindow处理
    return false;
}
