#ifndef _POWER_LED_MANAGER_H_
#define _POWER_LED_MANAGER_H_

#include "ETLLib.hpp"
#include "globalmodel.h"

class CPowerLedManager
{
public:
    //构造函数
    CPowerLedManager();
    //析构函数
    ~CPowerLedManager();

    static CPowerLedManager & GetInstance() DECLARE_ATTR;

public:
    // 初始化
    void Init();

    // 注册外部消息
    void RegisterMessage();

    // 处理外部事件
    bool OnPowerLedEvent(msgObject & objMessage);

    // 处理配置改变
    bool OnConfigChanged(msgObject & objMessage);

    void Forbid(BOOL bForbidden);

    void FlushPowerLed();
    // 是否有语音或短信 信息
    bool BeHaveMail();
    // 是否有未接来电
    bool BeHaveMissCall();

    bool BeHaveSession();

private:

    bool m_bPowerLedLogicEnable;
    // Idle&Talk状态下是否亮灯的配置
    bool m_bEnableLedIdleTalk;
    // VoiceMail/TextMessag是否提示
    bool m_bEnableLedMail;
    bool m_bForbidden;
    bool m_iLastStatus;
};

// 获取全局唯一的窗口管理对象
#define _PowerLedManager CPowerLedManager::GetInstance()

#endif  // _POWER_LED_MANAGER_H_
