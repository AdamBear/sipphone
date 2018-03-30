#ifndef EVENT_MANAGER_H_
#define EVENT_MANAGER_H_

#include <interfacedefine.h>
#include "extended/event/include/event_def.h"
#include "extended/event/src/eventbase.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "features-define.h" // DECLARE_ATTR
#include "talk/fwd_dnd/include/fwddndmsgdefine.h" // FwdMessageStatusData

typedef YLList<CEventBase * >    typeListModule;

#define FOR_EVENT_MODULE_LIST(itBegin) \
    typeListModule::iterator itBegin = ListEventModule.begin(); \
    for ( ;itBegin != ListEventModule.end(); ++itBegin)

class CEventManager
{
public:
    static CEventManager * GetInstance() DECLARE_ATTR;
    // 初始化.
    bool MyInit();
    // 注册
    bool RegisterModule(CEventBase * pEvent);
    // 注销
    bool UnRegisterModule(CEventBase * pEvent);
    // 发送事件
    SEND_RESULT SendEvent(EventDataBase * pData, EventModuleType eType = EMT_ALL);
    // 发送话机信息事件
    SEND_RESULT SendPhoneInfoEvent(EventType eEventType, EventModuleType eType = EMT_ALL);
    // 发送外围设备信息事件
    bool SendConnectedDeviceEvent(EventType eEventType, EventModuleType eType = EMT_ALL);
    // 处理计时器到时消息.
    bool OnTimeoutMessage(unsigned int uTimerID) DECLARE_ATTR;
#if IF_FEATURE_EVENT_MODULE
    // 处理DND状态改变消息
    bool OnDndMessage(msgObject & refObj);
    // 处理FWD状态改变消息
    bool OnFwdMessage(msgObject & refObj);
#endif // IF_FEATURE_EVENT_MODULE

    const PhoneInfoEventData GetPhoneInfo()
    {
        return m_phoneInfoData;
    }
    bool GetConnectedDeviceInfo(ConnectedDeviceEventData & refConnectedDeviceData);
private:
    CEventManager();
    ~CEventManager();

    // 初始化相关事件模块
    void InitEventModule();

private:
    typeListModule ListEventModule;
    PhoneInfoEventData m_phoneInfoData;
    // 外围设备信息
    ConnectedDeviceEventData m_ConnectedDeviceData;
    bool m_bPhoneSetupConpleted;
    bool m_bFirstSendEvent; // 第一次发送事件
    bool m_bPowerOnChangeIP;
};

#define g_pEventManager (CEventManager::GetInstance())

#endif  // EVENT_MANAGER_H_
