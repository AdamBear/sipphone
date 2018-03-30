#include "modledlampmock.h"


LedLampMock * g_pLedLampMock = NULL;

bool ledlamp_PostEvent(LAMP_EVENTTYPE eEventType, bool bRegister)
{
    return g_pLedLampMock->PostEvent(eEventType, bRegister);
}
