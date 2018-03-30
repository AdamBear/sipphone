#if IF_FEATURE_CTRIP_CTI
#include "modctripcti.h"
#include "ctripprocesser.h"

void ctrip_Init()
{
    g_objCtripManager;
}

bool ctrip_IsControlByCtrip(int nLineId)
{
    return g_objCtripManager.IsControlByCtrip(nLineId);
}

#endif
