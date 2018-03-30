#include "memory.h"
#include "etlmsghandler/modetlmsghandler.h"


LRESULT ProcessMemoryCheckMsg(msgObject & msg)
{
    switch (msg.message)
    {
    case PRINTF_MSG:
        {
#ifdef _DEBUGMEM
            Dump(false);
#endif
        }
        break;
    default:
        break;
    }

    return 0;
}

void MemoryCheckInit()
{
#ifdef _DEBUGMEM
    GetDumpMemInstance();
#endif

    etl_RegisterMsgHandle(PRINTF_MSG, PRINTF_MSG, ProcessMemoryCheckMsg);
}

bool MemoryCheck_OnTimeOut(unsigned int iTimerID)
{
#ifdef _DEBUGMEM
    return GetDumpMemInstance().OnTimeoutMessage(iTimerID);
#endif
    return true;
}
