#ifndef __KEYPAD_HOOK_H__
#define __KEYPAD_HOOK_H__

#include "keyjudder.h"
#include "inputhook_include.h"
using namespace std;

#ifdef USE_KEYBOARDFILTER
#include <QWSServer>
class CKeypadHook : public QWSServer::KeyboardFilter
#else
class CKeypadHook
#endif
{
public:
    CKeypadHook(void);
    virtual ~CKeypadHook(void);

public:
    //注册按键处理回调函数
    bool RegisterHookProc(KEYPAD_HOOK_PROC keypadHook, HOOK_MODULE priority);
    //注销按键处理回调函数
    bool UnRegisterHookProc(KEYPAD_HOOK_PROC keypadHook);

    //获取按键拦截处理状态
    bool GetKeypadHook();
    //设置按键拦截处理状态
    void SetKeypadHook(bool bKeypadHook);

    //按键事件的过滤拦截函数
    bool filter(QObject * pObject, QKeyEvent * pKeyEvent);

#ifdef USE_KEYBOARDFILTER
    bool filter(int unicode, int nKeyCode, int modifiers, bool bPress, bool autoRepeat);
#endif

private:
    // 将按键传入UIManager前的预处理
    // 预先处理一些和Qt事件机制无关的操作
    bool PreProcessKey(int nPhoneKeyCode, bool bPress);

private:
    //按键防抖处理对象
    CKeyJudder          m_objKeyJudder;
    //按键处理回调函数列表
    KEY_HOOK_MAP    m_mapHookProc;
    //是否进行按键拦截处理
    bool                m_bKeypadHook;
};

#endif //__KEYPAD_HOOK_H__
