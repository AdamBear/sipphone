#ifndef __wDefine_H__
#define __wDefine_H__

#if !WINDOWS_SYSTEM
#error only windows system supported
#endif

#ifndef WINVER
#define WINVER          0x0501
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT    0x0501
#endif
#ifndef _WIN32_IE
#define _WIN32_IE       0x0501
#endif

typedef char                    ACHAR;

#ifdef __int64
typedef __int64                 INT64;
typedef unsigned __int64        UINT64;
#else
typedef long long               INT64;          // INT64 = 8 Bytes
typedef unsigned long long      UINT64;
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <stdio.h>
#include <stdlib.h>
// #include <stdlib.h>
// #include <malloc.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#include <typeinfo.h>
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#include <tchar.h>
#include <string.h>
#include <typeinfo.h>
#include <math.h>
#include <time.h>
#include <locale.h>         // setlocale(LC_ALL, "chs");
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <new>
#include <direct.h>
#include <strsafe.h>
#include <TlHelp32.h>

#include <sys/timeb.h>

#include <shlwapi.h>
#ifdef _MSC_VER
#pragma comment(lib, "shlwapi.lib")
#endif

#include <mmsystem.h>
#ifdef _MSC_VER
#   pragma comment(lib, "winmm.lib")
#endif

#include <aclapi.h>
#ifdef _MSC_VER
#   pragma comment(lib, "Advapi32.lib")
#endif

#include <imm.h>
#ifdef _MSC_VER
#   pragma comment(lib, "imm32.lib")
#endif

#include <Psapi.h>
#ifdef _MSC_VER
#   pragma comment(lib, "Psapi.lib")
#endif

#include <iphlpapi.h>
#ifdef _MSC_VER
#   pragma comment(lib, "Iphlpapi.lib")
#endif

#ifdef _MSC_VER
#   pragma comment(lib, "ws2_32.lib")
#endif

#include <RpcDce.h>
#ifdef _MSC_VER
#   pragma comment(lib, "rpcrt4.lib")
#endif

#include <algorithm>
#include <functional>
#include <fstream>
#include <iosfwd>
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <new>
#include <utility>
#include <memory>
#include <exception>
#include <iterator>

#define JOIN_LETTER(l1, l2)             l1##l2
#define COMMENT_LINE                    JOIN_LETTER(/, /)


typedef DWORD                   THREAD_ID;
typedef DWORD                   PROCESS_ID;
typedef DWORD                   TASK_ID;
typedef uintptr_t               TIMER_ID;
typedef const unsigned char   *  LPCBYTE;

#define snprintf _snprintf
#define F_OK 0

#ifndef uintptr_t
typedef unsigned int   uintptr_t;
#endif

typedef char                int8;
typedef unsigned char       uint8;

#ifndef STDIN_FILENO
#define STDIN_FILENO    fileno(stdin)
#define STDOUT_FILENO   fileno(stdout)
#define STDERR_FILENO   fileno(stderr)
#endif

#ifdef _MSC_VER
#define EXPORT_MODULE_VERSION(sVer)         const static char* _szVersion_##sVer = ".Version:"sVer
#else
#define EXPORT_MODULE_VERSION(sVer)
#endif

#define link_lib( prefix )                  comment(lib, prefix UNICODE_SUF DEBUG_SUF ".lib")
#define unlink_lib( prefix )                comment(linker, "/NODEFAULTLIB:"prefix UNICODE_SUF DEBUG_SUF ".lib")

#if !IF_EXPORT_SYMBOLS
#ifdef _MSC_VER
//#pragma link_lib("ETLLib")
#endif
#endif

#ifndef S_ISREG
#define S_ISREG(m)  chCHECK_BITS(m, _S_IFREG)
#define S_ISDIR(m)  chCHECK_BITS(m, _S_IFDIR)
#define S_ISLNK(m)  0
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4786)
#pragma warning(disable : 4996)
//#pragma warning(disable : C4552)
#endif

#define _W(p)               L##p

#define strcasecmp                      StrCmpIA
#define wcscasecmp                      StrCmpIW
#define strncasecmp                     StrCmpNIA
#define wcsncasecmp                     StrCmpNIW

#define wTimer                          chTimer
#define wTimerRT                        chTimerRT
#define wReadWriteLock                  chReadWriteLock
#define wCriticalSection                chCriticalSection
#define wMapFile                        chMapFile
#define wRawMapFile                     chRawMapFile
#define wMutex                          chMutex
#define wEvent                          chEvent
#define wStdInput                       chStdInput
#define wPathWalker                     chPathWalker

#endif //__wDefine_H__
