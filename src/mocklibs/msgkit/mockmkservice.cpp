#include "mockmkservice.h"


MockMkService * g_pMockMkService = NULL;

#ifdef __cplusplus
extern "C" {
#endif

unsigned long mkservice_get_data(mkit_handle_t server, int id, int index)
{
    return g_pMockMkService ? g_pMockMkService->GetData(server, id, index) : 0;
}

#ifdef __cplusplus
} // extern "C"
#endif
