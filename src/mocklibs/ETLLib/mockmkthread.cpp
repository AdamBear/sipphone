#include "mockmkthread.h"


MockMkThread * g_pMockMkThread = NULL;

mkit_handle_t mkGetHandle()
{
    if (g_pMockMkThread)
    {
        return g_pMockMkThread->mkGetHandle();
    }
    return 0;
}
