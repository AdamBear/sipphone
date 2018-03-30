#include "../include/moddiagnosis.h"
#include "diagprocesser.h"

#ifdef IF_FEATURE_DIAGNOSE
void diag_Init()
{
    g_objDiagManager.Init();
}
#endif //#ifdef IF_FEATURE_DIAGNOSE
