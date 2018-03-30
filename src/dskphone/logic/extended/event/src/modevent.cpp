#include "extended/event/include/modevent.h"
#include "extended/event/src/eventmanager.h"

LRESULT event_TimeOutMegProcess(msgObject & refObj)
{
    g_pEventManager->OnTimeoutMessage(refObj.wParam);
    return TRUE;
}

#if IF_FEATURE_EVENT_MODULE
LRESULT event_DndMegProcess(msgObject & refObj)
{
    return g_pEventManager->OnDndMessage(refObj);
}
LRESULT event_FwdMegProcess(msgObject & refObj)
{
    return g_pEventManager->OnFwdMessage(refObj);
}
#endif // IF_FEATURE_EVENT_MODULE

void event_Init()
{
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, event_TimeOutMegProcess);
#if IF_FEATURE_EVENT_MODULE
    SingleMsgReg(DND_MESSAGE_STATUS_CHANGED, event_DndMegProcess);
    SingleMsgReg(FWD_MESSAGE_STATUS_CHANGED, event_FwdMegProcess);
#endif // IF_FEATURE_EVENT_MODULE
    g_pEventManager->MyInit();
}

bool event_RegisterModule(CEventBase * pEvent)
{
    return g_pEventManager->RegisterModule(pEvent);
}

bool event_UnRegisterModule(CEventBase * pEvent)
{
    return g_pEventManager->UnRegisterModule(pEvent);
}

SEND_RESULT event_SendEvent(EventDataBase * pData, EventModuleType eType/* = EMT_ALL*/)
{
    return g_pEventManager->SendEvent(pData, eType);
}

SEND_RESULT event_SendEvent(EventType eEventType, EventModuleType eType/* = EMT_ALL*/)
{
    EventDataBase eventData;
    eventData.m_eEventType = eEventType;
    return g_pEventManager->SendEvent(&eventData, eType);
}

SEND_RESULT event_SendEvent(EventType eEventType, int wParam, int lParam,
                            const yl::string & strExt /*= ""*/,
                            EventModuleType eType /*= EMT_ALL*/)
{
    EventDataBase eventData;
    eventData.m_eEventType = eEventType;
    eventData.m_wParam = wParam;
    eventData.m_lParam = lParam;
    eventData.m_strExt = strExt;
    return g_pEventManager->SendEvent(&eventData, eType);
}

SEND_RESULT event_SendPhoneInfoEvent(EventType eEventType, EventModuleType eType/* = EMT_ALL*/)
{
    return g_pEventManager->SendPhoneInfoEvent(eEventType, eType);
}

bool event_SendConnectedDeviceEvent(EventType eEventType, EventModuleType eType/* = EMT_ALL*/)
{
    return g_pEventManager->SendConnectedDeviceEvent(eEventType, eType);
}

const PhoneInfoEventData event_GetPhoneInfo()
{
    return g_pEventManager->GetPhoneInfo();
}
