#ifndef __MOUSE_HOOK_H__
#define __MOUSE_HOOK_H__

#include "inputhook_include.h"

class CMouseHook
{
public:
    CMouseHook();
    virtual ~CMouseHook();

public:
    //注册鼠标处理回调函数
    bool RegisterHookProc(MOUSE_HOOK_PROC mouseHook, HOOK_MODULE priority);
    //注销鼠标处理回调函数
    bool UnRegisterHookProc(HOOK_MODULE priority);

    //获取按键拦截处理状态
    bool GetMouseHook();
    //设置按键拦截处理状态
    void SetMouseHook(bool bMouseHook);

    //鼠标事件的过滤拦截函数
    bool processMouseInput(int key, bool bPressed);
private:
    //鼠标处理回调函数列表
    MOUSE_HOOK_MAP  m_mapHookProc;
    //是否进行鼠标事件拦截处理
    bool                m_bMouseHook;
};

#endif //__MOUSE_HOOK_H__
