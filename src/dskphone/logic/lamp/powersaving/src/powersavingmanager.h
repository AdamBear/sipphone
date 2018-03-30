#ifndef __POWERSAVING_MANAGER_H__
#define __POWERSAVING_MANAGER_H__

#include <features-define.h> // DECLARE_ATTR
#include <singletonclassdefine.h> // DECLARE_SINGLETON_CLASS GET_SINGLETON_INSTANCE IMPLEMENT_GETINSTANCE
#include <ETLLib.hpp> // msgObject
#include <config.h> // IS_CODE_TEST

#include "powersavingdatadefine.h"
#include "powersavingconfig.h"

class CPowerSavingManager
{
    DECLARE_SINGLETON_CLASS(CPowerSavingManager)

public:
    // 初始化
    void Init();
    // 逆初始化
    void Uninit();
    // 获取当前节电状态
    POWERSAVING_STATUS GetStatus() const;
    // 设置是否允许进入节电
    void SetAvoidPowerSaving(bool bAvoid);

#if IS_CODE_TEST
public:
#else
private:
#endif // IS_CODE_TEST
    CPowerSavingConfig m_config; // 配置管理
    Time m_timeout; // 计划进入节电的时间点【变量地址作为进入节电/预节电的定时器】
    POWERSAVING_STATUS m_status; // 当前节电状态
    bool m_bSmartWakeTimerEnable; // 当前是否启用智能点亮定时器【变量地址作为智能点亮定时器】

    // 设置智能点亮定时器是否启用
    void SetSmartWakeTimer(bool bSmartWakeEnable);
    // 启动智能点亮定时器
    void StartSmartWakeTimer();
    // 消息处理函数（主要用于分发）
    static LRESULT OnMessage(msgObject & msg);
    // 操作处理函数
    bool OnOperation(OPERATION_TYPE operation);
    // 定时器处理函数
    bool OnTimer(const msgObject & msg);
    // 系统时间修改事件处理函数
    bool OnSystemTimeChanged(msgObject & msg);
    // 配置修改事件处理函数
    bool OnConfigChanged(const msgObject & msg);
    // 设置当前节电状态，并执行状态切换操作（发送消息等）
    void SetStatus(POWERSAVING_STATUS status);
    // 获取操作对应的进入节电时间点
    Time GetTimeoutOfOperation(OPERATION_TYPE operation, Time time) const;
    // 重新设置进入节电的定时器
    void ResetPowerSavingTimer(Time timeout);
};

#define _PowerSavingManager GET_SINGLETON_INSTANCE(CPowerSavingManager)


#endif // __POWERSAVING_MANAGER_H__
