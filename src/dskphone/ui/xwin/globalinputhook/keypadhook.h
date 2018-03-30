#ifndef __KEYPAD_HOOK_H__
#define __KEYPAD_HOOK_H__

#include "inputhook_include.h"

class CKeypadHook
{
public:
    CKeypadHook();
    virtual ~CKeypadHook();

public:
    //注册按键处理回调函数
    bool RegisterHookProc(KEYPAD_HOOK_PROC keypadHook, HOOK_MODULE priority);
    //注销按键处理回调函数
    bool UnRegisterHookProc(HOOK_MODULE priority);

    //获取按键拦截处理状态
    bool GetKeypadHook();
    //设置按键拦截处理状态
    void SetKeypadHook(bool bKeypadHook);

#if IF_INPUT_HOOK_KEEP_KEY_RAW_VALUE
    int GetCurrentRawKey() const;
    void SetRawKey(int nKey);
#endif

    //按键事件的过滤拦截函数
    bool processKeyInput(int nKey, bool bPressed);

private:
    // 将按键传入各钩子函数前的预处理
    bool PreProcessKey(int nPhoneKeyCode, bool bPress);

private:
    //按键处理回调函数列表
    KEY_HOOK_MAP    m_mapHookProc;
    //是否进行按键拦截处理
    bool                m_bKeypadHook;
#if IF_INPUT_HOOK_KEEP_KEY_RAW_VALUE
    int                 m_nCurRawKey;
#endif
};

#endif //__KEYPAD_HOOK_H__
