#if IF_FEATURE_EMERGENCY_HELD
#include "talk/emergency/src/emergencyheld.h"
#include "talk/emergency/include/modemergencyheld.h"

void EmergencyHELD_Init()
{
    g_objEmergencyHELDManager;
}

yl::string EmergencyHELD_getLocation()
{
    return g_objEmergencyHELDManager.getLocation();
}
#endif
