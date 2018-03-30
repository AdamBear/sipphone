#include "emergency_inc.h"


void Emergency_Init()
{
    _Emergency.Init();
}

bool Emergency_IsEmergency(const char * cszTarget, bool IsCharCmp /*= false*/)
{
    return _Emergency.IsMatched(cszTarget, EM_WL | EM_ADV, IsCharCmp) != EM_NONE;
}

bool Emergency_IsIncomingEmergency(const char * cszTarget)
{
    return _Emergency.IsInWindows() || _Emergency.IsMatched(cszTarget, EM_WL_DND | EM_ADV) != EM_NONE;
}

bool Emergency_IsForwardEmergency(const char * cszTarget)
{
    return _Emergency.IsInWindows() || _Emergency.IsMatched(cszTarget, EM_WL_FWD | EM_ADV) != EM_NONE;
}

void Emergency_CallEmergency()
{
    _Emergency.CallEmergency();
}

#ifdef IF_FEATURE_EMERGENCY
bool Emergency_IsEmergencyMode(LPCSTR lpNum, bool IsCharCmp/* = false*/)
{
    return _Emergency.IsEmergencyMode(lpNum, IsCharCmp);
}

LPCSTR Emergency_GetAssertId()
{
    return _Emergency.GetAssertedId();
}

int Emergency_GetPosIndex(LPCSTR lpNum)
{
    return _Emergency.GetPosIndex(lpNum);
}

yl::string Emergency_GetServerPriority(int iIndex)
{
    return configParser_GetCfgItemStringValue(kszEmengencyServerPriority, iIndex);
}

bool Emergency_IsEmergencyModeEnable()
{
    return !_Emergency.IsEmpty(EM_ADV) && configParser_GetConfigInt(kszEmengencySwtich) != 0;
}
#else
bool Emergency_IsEmergencyMode(LPCSTR lpNum, bool IsCharCmp/* = false*/)
{
    return false;
}

bool Emergency_IsEmergencyModeEnable()
{
    return false;
}

yl::string Emergency_GetAssertId()
{
    return yl::string();
}

int Emergency_GetPosIndex(LPCSTR lpNum)
{
    return -1;
}

yl::string Emergency_GetServerPriority(int iIndex)
{
    return yl::string();
}
#endif

void Emergency_StartWindows()
{
    _Emergency.StartWindows();
}

bool Emergency_IsAcceptNewCall(LPCSTR lpNum)
{
    return _Emergency.IsInWindows() || _Emergency.IsEmergencyMode(lpNum);
}

bool Emergency_IsEnabelEmergencyCalling()
{
    return ((1 == configParser_GetConfigInt(kszBroadsoftActive))
         && (1 == configParser_GetConfigInt(kszBroadsoftEmergencyCalling)));
}
