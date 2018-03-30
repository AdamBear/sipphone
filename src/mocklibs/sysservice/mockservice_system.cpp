#include "mockservice_system.h"


MockSysService * g_pMockSysService = NULL;

#ifdef __cplusplus
extern "C" {
#endif

int sys_get_systime(sys_Time_t * time)
{
    return g_pMockSysService ? g_pMockSysService->GetSysTime(time) : 0;
}

#ifdef __cplusplus
} // extern "C"
#endif
