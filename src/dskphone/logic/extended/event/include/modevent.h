#ifndef __MOD_EVENT_H__
#define __MOD_EVENT_H__
#include "extended/event/include/event_def.h"
#include "extended/event/src/eventbase.h"

/*******************************************************************
** 函数描述:     初始化
** 参数：         无
** 返回:       无
********************************************************************/
void event_Init();

/*******************************************************************
** 函数描述:     将事件模块注册到EventManager
** 参数：         CEventBase实例指针
** 返回:       true注册成功,false注册失败
********************************************************************/
bool event_RegisterModule(CEventBase * pEvent);

/*******************************************************************
** 函数描述:     将事件模块从EventManager中注销
** 参数：         CEventBase实例指针
** 返回:       true注销成功,false注销失败
********************************************************************/
bool event_UnRegisterModule(CEventBase * pEvent);

/*******************************************************************
** 函数描述:     发送事件数据给对应模块
** 参数：         EventDataBase 事件数据， EventModuleType 需要发送模块
** 返回:       发送结果枚举值
********************************************************************/
SEND_RESULT event_SendEvent(EventDataBase * pData, EventModuleType eType = EMT_ALL);

/*******************************************************************
** 函数描述:     发送事件给对应模块
** 参数：         EventType 事件类型， EventModuleType 需要发送模块
** 返回:       true发送成功,false发送失败
********************************************************************/
SEND_RESULT event_SendEvent(EventType eEventType, EventModuleType eType = EMT_ALL);

/*******************************************************************
** 函数描述:     发送事件给对应模块
** 参数：         EventType 事件类型， EventModuleType 需要发送模块
** 返回:       true发送成功,false发送失败
********************************************************************/
SEND_RESULT event_SendEvent(EventType eEventType, int wParam, int lParam,
                            const yl::string & strExt = "",
                            EventModuleType eType = EMT_ALL);

/*******************************************************************
** 函数描述:     发送话机信息事件给对应模块
** 参数：         EventType 事件类型， EventModuleType 需要发送模块
** 返回:       true发送成功,false发送失败
********************************************************************/
SEND_RESULT event_SendPhoneInfoEvent(EventType eEventType, EventModuleType eType = EMT_ALL);

/*******************************************************************
** 函数描述:     发送外围设备信息给对应模块
** 参数：         EventType 事件类型， EventModuleType 需要发送模块
** 返回:       true发送成功,false发送失败
********************************************************************/
bool event_SendConnectedDeviceEvent(EventType eEventType, EventModuleType eType = EMT_ALL);

/*******************************************************************
** 函数描述:     获取话机信息
** 参数：         无
** 返回:       PhoneInfoEventData
********************************************************************/
const PhoneInfoEventData event_GetPhoneInfo();

#endif // __MOD_EVENT_H__
