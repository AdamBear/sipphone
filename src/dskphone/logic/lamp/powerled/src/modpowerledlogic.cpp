#include "powerledmanager.h"
#include "modpowerledlogic.h"
#include "globalmodel.h"

void powerled_Init()
{
    _PowerLedManager.Init();
}

void powerled_Forbid(BOOL bForbidden)
{
    _PowerLedManager.Forbid(bForbidden);
}

void powerled_FlushPowerLed()
{
    _PowerLedManager.FlushPowerLed();
}



