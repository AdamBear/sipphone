#ifndef __xDefine_H__
#define __xDefine_H__
#if !LINUX_SYSTEM
#error only linux system supported
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include <malloc.h>
#include <termios.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <new>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <locale.h>         // setlocale(LC_ALL, "zh_CN.gb2312");

#include <dirent.h>
#include <limits.h>

#include <signal.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include <sys/ioctl.h>
#include <sys/socket.h>

#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <net/if.h>

/*
// comment for cygwin
#include <net/if_arp.h>
#include <net/ethernet.h>
*/

#include <netpacket/packet.h>

/*
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
*/

#include <typeinfo>
//using namespace std;
//////////////////////////////////////////////////////////////////////////
#define IF_WCHAR_IS_UNICODE32           1

typedef char                ACHAR;
#if IF_WCHAR_IS_UNICODE32
typedef wchar_t         WCHAR;
#else
typedef unsigned short  WCHAR;
#endif

#ifdef _UNICODE
typedef WCHAR           TCHAR;
#else
typedef ACHAR           TCHAR;
#endif

typedef ACHAR        *       LPSTR;
typedef WCHAR        *       LPWSTR;
typedef TCHAR        *       LPTSTR;
typedef const ACHAR     *    LPCSTR;
typedef const WCHAR     *    LPCWSTR;
typedef const TCHAR     *    LPCTSTR;

typedef unsigned char       BYTE, *LPBYTE;
typedef const unsigned char * LPCBYTE;

typedef unsigned short      WORD;           // WORD = 2 Bytes
typedef short               SHORT;
typedef unsigned short      USHORT;

typedef unsigned int        DWORD;          // DWORD = 2 * WORD = 4 Bytes

typedef int                 BOOL;
typedef int         *        LPBOOL;

typedef int                 INT;            // INT = 4 Bytes
typedef unsigned int        UINT;

typedef long                LONG;           // INT64 = 8 Bytes
typedef unsigned long       ULONG;          // INT64 = 8 Bytes

typedef long long           INT64;          // INT64 = 8 Bytes
typedef unsigned long long  UINT64;

typedef void        *        LPVOID;
typedef const void     *     LPCVOID;

typedef char                int8;
typedef unsigned char       uint8;
typedef void                VOID;

#define TRUE                (BOOL)(1)
#define FALSE               (BOOL)(0)

#define MAX_PATH            256

#define CALLBACK            // __stdcall
#define CDECL               // __attribute__((cdecl))

typedef pthread_t           THREAD_ID;
typedef pid_t               PROCESS_ID;
typedef pid_t               TASK_ID;


typedef intptr_t            INT_PTR;     // xxx_PTR means signed/unsigned type with same size with void*
typedef uintptr_t           UINT_PTR;

typedef UINT_PTR            TIMER_ID;
typedef UINT_PTR            WPARAM;
typedef INT_PTR             LPARAM;
typedef INT_PTR             LRESULT;

typedef unsigned long       DSK_NOTIFY_T;
#ifdef _UNICODE
#define _T(x)               L##x
#else
#define _T(x)               x
#endif

#if IF_WCHAR_IS_UNICODE32
#include <wchar.h>
#define _W(p)               L##p
#else
#define _W(p)               NULL
#endif


#ifdef __CYGWIN__
#define EXPORT_MODULE_VERSION(sVer)     const char* _szVersion_##sVer __attribute__ ((section (".Version$"sVer ))) = sVer
#else
#define EXPORT_MODULE_VERSION(sVer)     const char* _szVersion_##sVer __attribute__ ((section (".Version:"sVer ))) = sVer
#endif
// DEFINE_HANDLE(HWND);

#define xTimer                          chTimer
#define xReadWriteLock                  chReadWriteLock
#define xCriticalSection                chCriticalSection
#define xMapFile                        chMapFile
#define xRawMapFile                     chRawMapFile
#define xMutex                          chMutex
#define xEvent                          chEvent
#define xStdInput                       chStdInput
#define xPathWalker                     chPathWalker

struct SYSTEMTIME
{
    WORD wYear;                 // Specifies the current year. The year must be greater than 1601.
    WORD wMonth;                // Specifies the current month; January = 1, February = 2, and so on.
    WORD wDayOfWeek;            // Specifies the current day of the week; Sunday = 0, Monday = 1, and so on.
    WORD wDay;                  // Specifies the current day of the month. 1. 2. 3
    WORD wHour;                 // Specifies the current hour.
    WORD wMinute;               // Specifies the current minute.
    WORD wSecond;               // Specifies the current second.
    WORD wMilliseconds;         // Specifies the current millisecond
};

enum
{
    DT_TOP              = 0x00000000,
    DT_LEFT             = 0x00000000,
    DT_CENTER           = 0x00000001,
    DT_RIGHT            = 0x00000002,
    DT_VCENTER          = 0x00000004,
    DT_BOTTOM           = 0x00000008,
};

#endif //__xDefine_H__
