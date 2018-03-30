#ifndef EVENT_BASE_H_
#define EVENT_BASE_H_

#include "extended/event/include/event_def.h"

class CEventBase
{
public:
    CEventBase(EventModuleType eType);
    CEventBase();
    virtual ~CEventBase();

    //注册
    bool RegisterModule();
    //注销
    bool UnRegisterModule();
    //发送事件
    virtual bool SendEvent(EventDataBase * pData) = 0;
    //获取模块类型
    EventModuleType GetModuleType();

private:
    EventModuleType m_eModuleType;
};


#endif  // EVENT_BASE_H_
