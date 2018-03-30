#ifndef __EMERGENCY_HELD_MOD_H__
#define __EMERGENCY_HELD_MOD_H__
#if IF_FEATURE_EMERGENCY_HELD
#include "ylstl/ylstring.h"

void EmergencyHELD_Init();

yl::string EmergencyHELD_getLocation();

#endif

#endif
