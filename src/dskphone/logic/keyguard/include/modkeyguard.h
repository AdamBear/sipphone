#ifndef _MOD_KEY_GUARD_H_
#define _MOD_KEY_GUARD_H_

#include "keyguard_common.h"
#include <ETLLib.hpp>

// 初始化
void keyGuard_Init();

// 键盘锁功能是否开启
bool keyGuard_IsEnable();

// 开启/关闭键盘锁
void keyGuard_Enable(bool bEnable);

// 键盘是否处于锁定状态
bool keyGuard_IsLock();

// 执行上锁操作
void keyGuard_Lock(bool bLock);

// 能否进入特定的模块
bool keyGuard_IsEnableModuld(KEY_GUARD_MODULE eModule);

// 键盘锁处理相关的消息
// 返回false表示该消息键盘锁模块不处理
// 返回true表示该消息已经被键盘锁模块处理
bool keyGuard_ProcessMsg(msgObject & msg);

// 设置验证解锁密码的函数
void keyGuard_SetCheckUnlockPswFun(CheckUnlockPswFun pFun);

// 设置非紧急号码的提示框函数
void keyGuard_SetEmergencyTipFun(PopupEmergencyNumNote pFun);

// 弹出紧急号码的提示框
void keyGuard_PopEmergencyTip();

// 设置键盘锁类型
void keyGuard_SetKeyGuardType(KEY_GUARD_TYPE eKGType);

// 获取键盘锁类型
KEY_GUARD_TYPE keyGuard_GetKeyGuardType();

// 获取解锁密码
yl::string keyGuard_GetUnlockPIN();

//获取Pin类型
PinType keyGuard_GetPinType(const yl::string & strPin);

// 检查密码
bool keyGuard_CheckUnlockPin(const yl::string & strUnLockPin);

// 保存密码
bool keyGuard_SaveUnlockPin(const yl::string & strUnLockPin);

// 重置键盘锁状态
void keyGuard_ResetKeypadLock();

// 取消键盘锁计时器
void keyGuard_StopKeypadLockTimer();

#endif // _MOD_KEY_GUARD_H_
