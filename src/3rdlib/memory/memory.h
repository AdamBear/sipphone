#ifndef __MEMORY_H__
#define __MEMORY_H__

#ifdef __cplusplus

#include "memcheck.h"

#include "ETLLib.hpp"
#define PRINTF_MSG  0x60001

//#define _DEBUGMEM

#define new DEBUG_NEW

LRESULT ProcessMemoryCheckMsg(msgObject & msg);

void MemoryCheckInit();

bool MemoryCheck_OnTimeOut(unsigned int iTimerID);

#endif

#endif

