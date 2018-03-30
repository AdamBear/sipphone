#ifndef __ETL_H__
#define __ETL_H__

#ifdef _WIN32
#define WINDOWS_SYSTEM              1
#define LINUX_SYSTEM                0
#else
#define WINDOWS_SYSTEM              0
#define LINUX_SYSTEM                1
#endif

#if defined(NO_DEBUG) || defined(NDEBUG)
#define IF_DEBUG_ENABLE             0
#else
#define IF_DEBUG_ENABLE             1
#endif

#if defined(NO_TRACE)
#define IF_USE_SYZ_TRACE            0
#else
#define IF_USE_SYZ_TRACE            1
#endif


#ifndef IF_EXPORT_SYMBOLS
#define IF_EXPORT_SYMBOLS           0
#endif

#define IF_IPC_SUPPORT                  1

#define nameSystemShareObjects          "ipc_system_share"
#ifdef _WIN32
#define sizeSystemShareObjects          64 * 1024
#else
#define sizeSystemShareObjects          512 * 1024
#endif
#define verSystemShareObjects           1006

#define nameSystemNamedObjects          "ipc_system_named"
#define sizeSystemNamedObjects          8 * 1024
#define verSystemNamedObjects           1002

#define nameSystemTraceHelper           "ipc_system_trace"
#define sizeSystemTraceHelper           64 * 1024
#define verSystemTraceHelper            1005

#define nameSystemStorageObjects        "ipc_system_storage"
#define sizeSystemStorageObjects        4 * 1024
#define verSystemStorageObjects         1003

#define minSystemAtom                   0x10000000
#define maxSystemAtom                   0x1000FFFF

#define minProcessAtom                  0x20000000
#define maxProcessAtom                  0x2000FFFF
#define countProcessAtom                1024

#define minTimerID                      0x30000000
#define maxTimerID                      0x3000FFFF

//////////////////////////////////////////////////////////////////////////
#include "_chDefine.h"

//////////////////////////////////////////////////////////////////////////
#include "_chDeclare.h"

//////////////////////////////////////////////////////////////////////////
#include "_chChain.h"
#include "_chAlloc.h"       /* some userful class no system relation */
#include "_chArray.h"       /* such as array, list, map, simple map */
#include "_chList.h"
#include "_chMap.h"
#include "_chString.h"

#include "_chMisc.h"        /* some userful API no system relation */

#include "_chBaseClass.h"

#endif //__ETL_H__
