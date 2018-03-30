#ifndef __ETLLib_H__
#define __ETLLib_H__

#ifdef  __cplusplus
#error use <ETLLib.hpp> NOT <ETLLib.h> for c++ program
#endif

/*
#ifdef _UNICODE
#pragma comment(linker, "/ENTRY:wWinMainCRTStartup")
#else // _UNICODE
#pragma comment(linker, "/ENTRY:WinMainCRTStartup")
#endif

IMPLEMENT_GLOBLE_OBJECT(wCLangHelper,           _LangHelper);
IMPLEMENT_GLOBLE_OBJECT(wCTimerIDHelper,        _TimerIDHelper);
IMPLEMENT_GLOBLE_OBJECT(wCTipTraceHelper,       _TipTraceHelper);
*/
#ifdef _WIN32
#define WINDOWS_SYSTEM              1
#define LINUX_SYSTEM                0
#else
#define WINDOWS_SYSTEM              0
#define LINUX_SYSTEM                1
#endif

#if WINDOWS_SYSTEM

#ifndef WINVER
#define WINVER          0x0500
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT    0x0501
#endif
#ifndef _WIN32_IE
#define _WIN32_IE       0x0500
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//  #include <AFX.h>
#include <Windows.h>
#include <stdio.h>
#include <string.h>

// syz added for UNICODESUF and DEBUGSUF
#if defined(_UNICODE) || defined(UNICODE)
#define _USUF           "u"
#else
#define _USUF           ""
#endif

#if defined(_DEBUG)
#define _DSUF           "d"
#else
#define _DSUF           ""
#endif

#define DECLARE_THREAD_METHOD(method)           extern DWORD __stdcall method(void* lpData)
#define IMPLEMENT_THREAD_METHOD(method)         DWORD __stdcall method(void* lpData)
#define CREATE_THREAD(thread_method)            do{THREAD_ID id = 0; CreateThread(NULL, 0, thread_method, NULL, 0, &id);}while(0)
#define link_lib(prex) comment(lib, prex _USUF _DSUF ".lib")
#pragma link_lib("LTLLib")
#else

typedef char                ACHAR;
typedef unsigned short      WCHAR;

#ifdef _UNICODE
typedef WCHAR               TCHAR;
#else
typedef ACHAR               TCHAR;
#endif

typedef unsigned char       BYTE, *LPBYTE;
typedef unsigned short      WORD;
typedef unsigned short      USHORT;

typedef int                 BOOL;
typedef int         *        LPBOOL;
typedef unsigned int        UINT;

typedef long                LONG;
typedef unsigned long       ULONG;
typedef unsigned long       DWORD;
typedef unsigned long       WPARAM;
typedef unsigned long       LPARAM;
typedef unsigned long       LRESULT;
typedef long long           INT64;
typedef unsigned long long  UINT64;

typedef void        *        LPVOID;
typedef const void     *     LPCVOID;
typedef unsigned int        DPTR;

typedef ACHAR        *       LPSTR;
typedef WCHAR        *       LPWSTR;
typedef TCHAR        *       LPTSTR;
typedef const ACHAR     *    LPCSTR;
typedef const WCHAR     *    LPCWSTR;
typedef const TCHAR     *    LPCTSTR;

#define TRUE                (BOOL)(1)
#define FALSE               (BOOL)(0)

#define MAX_PATH            256

#define CALLBACK    //__stdcall
#define CDECL       //_cdecl

#include <stdio.h>
#include <stdarg.h>
//#include <wchar.h> // not support UNICODE
#include <stdlib.h>

#include <math.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <sys/ioctl.h>
#include <sys/socket.h>

#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <net/if.h>

#include <netpacket/packet.h>
//////////////////////////////////////////////////////////////////////////

typedef pthread_t               THREAD_ID;
typedef pid_t                   PROCESS_ID;
typedef pid_t                   TASK_ID;

#ifdef _UNICODE
#define _T(x)               L##x
#else
#define _T(x)               x
#endif
#define DECLARE_THREAD_METHOD(method)       extern void* method(void* lpData)
#define IMPLEMENT_THREAD_METHOD(method)     void* method(void* lpData)
#define CREATE_THREAD(thread_method)        do{THREAD_ID id = 0; pthread_create(&id, NULL, thread_method, NULL);}while(0)
#endif

enum threadMessage
{
    TM_NULL                 = 0x0000,
    TM_INIT                 = 0x8001,
    TM_QUIT                 = 0x8002,
    TM_TIMER                = 0x8113,
    TM_IDLE                 = 0x8121,
    TM_USER                 = 0x8400,
};

#define DEFINE_HANDLE(name)             struct name##__ { int unused; }; typedef struct name##__ *name

//////////////////////////////////////////////////////////////////////////

typedef struct simpMSG
{
    UINT        message;
    WPARAM      wParam;
    LPARAM      lParam;
} simpMSG;

// typedef LRESULT (CALLBACK* THREAD_MSG_PROC_EX)(simpMSG* pMessage);
//typedef LRESULT(CALLBACK * THREAD_MSG_PROC_EX_C)(simpMSG * pMessage);

//////////////////////////////////////////////////////////////////////////
#include "sub-interface.h"

#endif //__ETLLib_H__
