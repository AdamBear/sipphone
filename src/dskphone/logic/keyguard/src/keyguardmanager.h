#ifndef _KEY_GUARD_MANAGER_H_
#define _KEY_GUARD_MANAGER_H_

#include "keyguard_common.h"
#include <ETLLib.hpp>


class CKeyGuardManager
{
public:
    // Get the singleton instance.
    static CKeyGuardManager * GetInstance() DECLARE_ATTR;

    // Release the singleton instance.
    static void ReleaseInstance();

    // 开启/关闭键盘锁
    void EnableKeyGuard(bool bEnable);

    // 是否开启/关闭
    bool IsEnable();

    // 上锁
    void LockKeyPad(bool bLock);

    // 是否上锁
    bool IsKeyPadLock();

    // 设置键盘锁类型
    void SetKeyGuardType(KEY_GUARD_TYPE eKGType);

    // 返回键盘锁类型
    KEY_GUARD_TYPE GetKeyGuardType();

    // 是否可以进入某个功能
    // pExtraData[in] 附加数据
    bool IsEnableModuld(KEY_GUARD_MODULE eModule);

    // 键盘锁处理相关的消息
    // 返回true表示该消息键盘锁模块不处理
    // 返回false表示该消息已经被键盘锁模块处理
    bool ProcessMsg(msgObject & msg);

    // 设置验证解锁密码的回调函数
    void SetCheckUnlockPswFun(CheckUnlockPswFun pFun);

    // 设置非紧急号码的提示框函数
    void SetEmergencyTip(PopupEmergencyNumNote pFun);

    // 弹出紧急号码的提示框
    void PopEmergencyTip();

    // 重新读取话机配置
    void ReloadConfig();

    // 开启定时器
    void ResetKeypadLockTimer();

    // 取消定时器
    void StopKeypadLockTimer();

    // 处理定时消息
    BOOL OnTimer(UINT iTimerId) DECLARE_ATTR;

private:
    // 设置上锁时间
    void SetLockTimerValue(int iLockTimer);

private:
    CKeyGuardManager();
    ~CKeyGuardManager();

private:
    static CKeyGuardManager * m_pOneSelf;
    bool                     m_bEnable;
    bool                     m_bLock;
    int                      m_iAutoLockTime;
    KEY_GUARD_TYPE           m_eKeyGuardType;
    CheckUnlockPswFun        m_pCheckUnlockPswFun;
    PopupEmergencyNumNote    m_pEmergencyTip;
};

// 获取全局的KeyGuardManager
#define g_pKeyGuardManager (CKeyGuardManager::GetInstance())

#endif // #ifndef _KEY_GUARD_MANAGER_H_

