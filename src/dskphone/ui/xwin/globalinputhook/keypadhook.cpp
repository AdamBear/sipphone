#include <ETLLib/ETLLib.hpp>

#include "keypadhook.h"
#include "keymap.h"

#include "dsklog/log.h"

CKeypadHook::CKeypadHook()
#if IF_INPUT_HOOK_KEEP_KEY_RAW_VALUE
    : m_nCurRawKey(0)
#endif
{
    m_mapHookProc.clear();
    m_bKeypadHook = false;
}

CKeypadHook::~CKeypadHook()
{
    m_mapHookProc.clear();
    m_bKeypadHook = false;
}

bool CKeypadHook::RegisterHookProc(KEYPAD_HOOK_PROC keypadHook, HOOK_MODULE priority)
{
    if (keypadHook != NULL)
    {
        KEY_HOOK_MAP::iterator iter = m_mapHookProc.find(priority);
        if (iter != m_mapHookProc.end())
        {
            //查找到相同回调指针，不重复添加
            return false;
        }
        else
        {
            m_mapHookProc.insert(priority, keypadHook);
        }
    }
    return true;
}

bool CKeypadHook::UnRegisterHookProc(HOOK_MODULE priority)
{
    KEY_HOOK_MAP::iterator iter = m_mapHookProc.find(priority);
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

bool CKeypadHook::GetKeypadHook()
{
    return m_bKeypadHook;
}

void CKeypadHook::SetKeypadHook(bool bKeypadHook)
{
    m_bKeypadHook = bKeypadHook;
}

#if IF_INPUT_HOOK_KEEP_KEY_RAW_VALUE
int CKeypadHook::GetCurrentRawKey() const
{
    return m_nCurRawKey;
}

void CKeypadHook::SetRawKey(int nKey)
{
    m_nCurRawKey = nKey;
}
#endif

bool CKeypadHook::processKeyInput(int nKey, bool bPressed)
{
    // 不进行事件的拦截
    if (!m_bKeypadHook)
    {
        return false;
    }

    GLOBALINPUTHOOK_INFO("Global Key %s:(%x)", bPressed ? "Down" : "Up", nKey);

    // 对按键进行进入Qt事件前的预处理
    return PreProcessKey(nKey, bPressed);
}

bool CKeypadHook::PreProcessKey(int nPhoneKeyCode, bool bPress)
{
    //外部模块注册回调调用
    for (int i = HOOK_MODULE_PREHOOKBEGIN; i <= HOOK_MODULE_PREHOOKEND; ++i)
    {
        KEY_HOOK_MAP::iterator itProc = m_mapHookProc.find(HOOK_MODULE(i));
        if (itProc != m_mapHookProc.end())
        {
            KEYPAD_HOOK_PROC pHookProc = itProc->second;
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
