#include "modpowersaving.h"

#include "powersavingmanager.h"


void PowerSaving_Init()
{
    _PowerSavingManager.Init();
}

void PowerSaving_Uninit()
{
    _PowerSavingManager.Uninit();
}

bool PowerSaving_IsInPowerSavingMode()
{
    return _PowerSavingManager.GetStatus() == POWERSAVING_STATUS_SAVING;
}

void PowerSaving_SetAvoidPowerSaving(bool bAvoid)
{
    _PowerSavingManager.SetAvoidPowerSaving(bAvoid);
}
