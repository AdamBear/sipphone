#include "extended/event/src/eventbase.h"
#include "extended/event/include/modevent.h"

CEventBase::CEventBase()
{
    m_eModuleType = EMT_NONE;
}

CEventBase::~CEventBase()
{
}

CEventBase::CEventBase(EventModuleType eType)
{
    m_eModuleType = eType;
}

EventModuleType CEventBase::GetModuleType()
{
    return m_eModuleType;
}

bool CEventBase::RegisterModule()
{
    return event_RegisterModule(this);
}

bool CEventBase::UnRegisterModule()
{
    return event_UnRegisterModule(this);
}
