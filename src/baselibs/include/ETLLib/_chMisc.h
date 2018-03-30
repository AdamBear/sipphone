#ifndef __chMICS_H__
#define __chMICS_H__
#include "ylstl/ylstring.h"
#ifndef __ETL_H__
#error _chMics.h only can be include in ETL.h
#endif

#if LINUX_SYSTEM
#include "xInc/xMisc.h"
#else
#include "wInc/wMisc.h"
#endif

namespace ETL_MiscHelper
{

//////////////////////////////////////////////////////////////////////////
// declare block in "xInc/xMisc.h" or "wInc/wMisc.h"
EXTERN_DECLARE(UINT64 etlGetCPUTickCount());
EXTERN_DECLARE(void etlGetLocalTime(SYSTEMTIME * lpTime));

EXTERN_DECLARE(PROCESS_ID etlGetCurrentProcessId());
EXTERN_DECLARE(BOOL etlIsValidProcessID(PROCESS_ID idProcess));
EXTERN_DECLARE(THREAD_ID etlGetCurrentThreadId());
EXTERN_DECLARE(BOOL etlIsValidThreadID(THREAD_ID idThread));
EXTERN_DECLARE(TASK_ID etlGetCurrentTaskId());
EXTERN_DECLARE(BOOL etlIsValidTaskID(TASK_ID idTask));

EXTERN_DECLARE_CPP(yl::string etlGetExeFileName());
EXTERN_DECLARE_CPP(yl::string etlGetCurrentDirectory());
EXTERN_DECLARE_CPP(yl::string etlGetHomeDirectory());
EXTERN_DECLARE_CPP(yl::string etlGetTempDirectory());

EXTERN_DECLARE(UINT pathStat(LPCSTR lpszFileName));
EXTERN_DECLARE(BOOL pathFileExist(LPCSTR lpszFileName));
EXTERN_DECLARE(BOOL pathCreateDirectory(LPCSTR lpszDirectoryName));

EXTERN_DECLARE(void etlSleep(int nTimeInms));
EXTERN_DECLARE(void etlDelay(int nTimeInms));
EXTERN_DECLARE(void etlSwitchToThread());

EXTERN_DECLARE(DWORD etlGetLastError());
EXTERN_DECLARE_CPP(yl::string etlTransLastError());

EXTERN_DECLARE(LONG etlInterlockedIncrement(LONG * pData));
EXTERN_DECLARE(LONG etlInterlockedDecrement(LONG * pData));

EXTERN_DECLARE(int etlWideCharToMultiByte(LPCWSTR lpWideCharStr, LPSTR lpMultiByteStr,
               int nBufferCount));
EXTERN_DECLARE(int etlMultiByteToWideChar(LPCSTR lpMultiByteStr, LPWSTR lpWideCharStr,
               int nBufferCount));
// end declare block
//////////////////////////////////////////////////////////////////////////

EXTERN_DECLARE(void etlSwapBytes(void * pBuffer, int nCount));

//////////////////////////////////////////////////////////////////////////
// BE is Big-endian         0x31323334 = "1234"             arm / Mips used
// LE is Little-endian      0x31323334 = "4321"             i386 used

#define chIsLESystem()      ((DWORD)0x31323334 == *(DWORD*)"4321")

#define chBE2Host   chHost2BE
EXTERN_DECLARE_CPP(DWORD chHost2BE(DWORD data));
EXTERN_DECLARE_CPP(WORD chHost2BE(WORD data));

//////////////////////////////////////////////////////////////////////////
// BE is Big-endian         0x31323334 = "1234"             ppc / mips used
// LE is Little-endian      0x31323334 = "4321"             i386 used
#define chLE2Host           chHost2LE
EXTERN_DECLARE_CPP(DWORD chHost2LE(DWORD data));
EXTERN_DECLARE_CPP(WORD chHost2LE(WORD data));

EXTERN_DECLARE(yl::string chW2UTF8(LPCWSTR lpszTextW));
EXTERN_DECLARE(yl::wstring  chUTF82W(LPCSTR lpszTextA));

EXTERN_DECLARE(yl::string chW2A(LPCWSTR lpszTextW));
EXTERN_DECLARE(yl::wstring chA2W(LPCSTR lpszTextA));

#ifdef _UNICODE
#define chA2T(txt)      chA2W(txt)
#define chT2A(txt)      chW2A(txt)
#define chW2T(txt)      (txt)
#define chT2W(txt)      (txt)
#else
#define chA2T(txt)      (txt)
#define chT2A(txt)      (txt)
#define chW2T(txt)      chW2A(txt)
#define chT2W(txt)      chA2W(txt)
#endif

//////////////////////////////////////////////////////////////////////////
// simply convert
EXTERN_DECLARE(yl::wstring simpA2W(LPCSTR lpszTextA));

EXTERN_DECLARE(yl::string simpW2A(LPCWSTR lpszTextW));

#ifdef _UNICODE
#define simpA2T(txt)        simpA2W(txt)
#define simpT2A(txt)        simpW2A(txt)
#define simpW2T(txt)        (txt)
#define simpT2W(txt)        (txt)
#else
#define simpA2T(txt)        (txt)
#define simpT2A(txt)        (txt)
#define simpW2T(txt)        simpW2A(txt)
#define simpT2W(txt)        simpA2W(txt)
#endif

EXTERN_DECLARE(int pathFileSize(LPCSTR lpszFileName));

//////////////////////////////////////////////////////////////////////////
// /pathName/fileName.extern    => fileName.extern
// /pathName/fileName           => fileName
// /pathName/                   => pathName/
// /                            => /
// C:/                          => C:/
// fileName.extern              => fileName.extern
EXTERN_DECLARE(LPCSTR pathGetShortNamePos(LPCSTR lpszPath));

// fileName.extern                  => fileName.extern
// /pathName/fileName.extern        => fileName.extern
// /pathName/fileName               => fileName
// /pathName/                       => '\0'
EXTERN_DECLARE(LPCSTR pathGetFileNamePos(LPCSTR lpszPath));

// fileName.extern              => .extern
// /pathName/fileName.extern    => .extern
// /pathName/fileName           => '\0'
// /pathName/                   => '\0'
EXTERN_DECLARE(LPCSTR pathGetFileExtensionPos(LPCSTR lpszPath));

// change /pathName/fileName.extern to /pathName/fileName
EXTERN_DECLARE(LPSTR pathRemoveExtension(LPSTR pszPath));

// lpszNewExtern = ".extern2"
// change /pathName/fileName.extern to /pathName/fileName.extern2
// change /pathName/fileName to /pathName/fileName.extern2
// change /pathName/ to /pathName/.extern2
EXTERN_DECLARE(LPSTR pathRenameExtension(LPSTR pszPath, LPCSTR lpszNewExtern));

// change /pathName/fileName.extern to fileName.extern2
// change /pathName/fileName to fileName
// change /pathName/ to '\0'
EXTERN_DECLARE(LPSTR pathStripPath(LPSTR lpszPath));

// change fileName.extern to '\0'
// change /pathName/fileName.extern to /pathName/
// change /pathName/fileName to /pathName/
// change /pathName/ to /pathName/
// change /pathName to /
EXTERN_DECLARE(LPSTR pathRemoveFileSpec(LPSTR lpszPath));

// link 2 part with '/'
EXTERN_DECLARE(LPSTR pathAppend(LPSTR pszPath, LPCSTR pszMore));

// /path/file       => /path/file
// /path/file/      => /path/file
// /                => '\0'
EXTERN_DECLARE(LPSTR pathCutTail(LPSTR pszPath));

EXTERN_DECLARE_CPP(yl::string pathGetAbsolutePath(LPCSTR lpszRelativePath, LPCSTR lpszBasePath));
EXTERN_DECLARE_CPP(yl::string pathGetRelativePath(LPCSTR lpszAbsolutePath, LPCSTR lpszBasePath));

//////////////////////////////////////////////////////////////////////////

EXTERN_DECLARE(yl::string etlBin2Text(const void * pDataBuffer, int nSize));

EXTERN_DECLARE(BYTE etlA2HEX(ACHAR ch));

EXTERN_DECLARE(chTempByteArray etlText2Bin(LPCSTR lpszText));

EXTERN_DECLARE(chTempByteArray etlBase162Bin(LPCSTR lpszText));
EXTERN_DECLARE(yl::string etlBin2Base16(const void * pDataBuffer, int nSize));

EXTERN_DECLARE(chTempByteArray etlBase642Bin(LPCSTR lpszText));
EXTERN_DECLARE(yl::string etlBin2Base64(const void * pDataBuffer, int nSize));

#ifdef type_info
EXTERN_DECLARE(LPCSTR etlGetTypeString(const type_info & rType, BOOL bName));
#endif

EXTERN_DECLARE(BYTE etlSwap8Bit(BYTE bVal));
//////////////////////////////////////////////////////////////////////////

EXTERN_DECLARE_CPP(double mathFloor(double x, double mod));
EXTERN_DECLARE_CPP(int mathFloor(double x));
EXTERN_DECLARE_CPP(double mathCeil(double x, double mod));
EXTERN_DECLARE_CPP(double mathCeil(double x));

EXTERN_DECLARE(double mathNear(double x, double mod, double dRange));

EXTERN_DECLARE(int mathRound(double x, double dRange));


enum etlLoadLibraryFlag
{
    ETL_LOADLIBRARY_RTLD_LAZY,
    ETL_LOADLIBRARY_RTLD_NOW,
    ETL_LOADLIBRARY_RTLD_GLOBAL,
    ETL_LOADLIBRARY_RTLD_LOCAL,
    ETL_LOADLIBRARY_RTLD_NODELETE,
    ETL_LOADLIBRARY_RTLD_NOLOAD
};


EXTERN_DECLARE(void * etlLoadLibrary(LPCSTR lpFileName, int nFlag));
EXTERN_DECLARE(void * etlGetProcAddr(void * handle, const char * symbol));
EXTERN_DECLARE(int    etlFreeLibrary(void * handle));
EXTERN_DECLARE(int    etlGetLoadFlag(etlLoadLibraryFlag eFlag));

};
using namespace ETL_MiscHelper;


#if IF_EXPORT_SYMBOLS
#include "_chMisc.cpp"
#endif

#endif //__chMICS_H__
