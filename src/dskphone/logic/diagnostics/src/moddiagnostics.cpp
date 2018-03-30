#include "../include/moddiagnostics.h"

//#ifdef __cplusplus
//extern "C"
//{
//#endif

#include "../src/diagnostics.h"

//#ifdef __cplusplus
//}
//#endif


bool Diagnostics_RunSysCmd(DIAGNOSTICSTYPE eCmd, yl::string & strTargetURL)
{
    return g_pDiagnostics->RunSystemCmd(eCmd, strTargetURL);
}

void Diagnostics_StopSysCmd()
{
    g_pDiagnostics->StopSystemCmd();
}

yl::string Diagnostics_GetDiagnositcsResult()
{
    return g_pDiagnostics->GetRunCmdResult();
}

yl::string Diagnostics_GetDiagnositcsUrl()
{
    return g_pDiagnostics->GetRunUrl();
}

bool Diagnostics_IsSystemCmdRunning()
{
    return g_pDiagnostics->IsSystemCmdRunning();
}
