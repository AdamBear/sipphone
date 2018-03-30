#ifndef _MOD_POWER_SAVING_H_
#define _MOD_POWER_SAVING_H_

#include "powersavingmsgdefine.h"
// 节电模块初始化
void PowerSaving_Init();

// 节电模块逆初始化
void PowerSaving_Uninit();

// 判断当前是否处于节电模式中
bool PowerSaving_IsInPowerSavingMode();

// 设置是否可以进入节电模式（通话状态等情况禁止进入节电模式）
void PowerSaving_SetAvoidPowerSaving(bool bAvoid);

#endif