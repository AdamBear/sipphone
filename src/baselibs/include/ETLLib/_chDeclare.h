#ifndef __chDeclare_h__
#define __chDeclare_h__

namespace ETL_MiscHelper
{

EXTERN_DECLARE(double mathNear(double x, double mod, double dRange));
EXTERN_DECLARE(int mathRound(double x, double dRange));

EXTERN_DECLARE(PROCESS_ID etlGetCurrentProcessId());
EXTERN_DECLARE(THREAD_ID etlGetCurrentThreadId());
EXTERN_DECLARE(TASK_ID etlGetCurrentTaskId());

};
using namespace ETL_MiscHelper;

#if LINUX_SYSTEM && !IF_WCHAR_IS_UNICODE32
EXTERN_DECLARE(int wcslen(LPCWSTR lpszText));
EXTERN_DECLARE(LPWSTR wcscpy(LPWSTR lpszDes, LPCWSTR lpszSrc));
EXTERN_DECLARE(LPWSTR wcsncpy(LPWSTR lpszDes, LPCWSTR lpszSrc, int nCount));
EXTERN_DECLARE(LPWSTR wcscat(LPWSTR lpszDes, LPCWSTR lpszSrc));
EXTERN_DECLARE(int wcscmp(LPCWSTR s1, LPCWSTR s2));
EXTERN_DECLARE(int wcscasecmp(LPCWSTR s1, LPCWSTR s2));
EXTERN_DECLARE(int wcsncmp(LPCWSTR s1, LPCWSTR s2, int nCount));
EXTERN_DECLARE(int wcsncasecmp(LPCWSTR s1, LPCWSTR s2, int nCount));

EXTERN_DECLARE(int vswprintf(LPWSTR lpszBuffer, int nCount, LPCWSTR lpszFmt, va_list ap));
EXTERN_DECLARE(int swscanf(LPCWSTR lpszBuffer, LPCWSTR lpszFmt, ...));

EXTERN_DECLARE_CPP(int wcstombs(LPSTR lpszBuffer, LPCWSTR lpszText, int nCount));
EXTERN_DECLARE_CPP(int mbstowcs(LPWSTR lpszBuffer, LPCSTR lpszText, int nCount));
EXTERN_DECLARE_CPP(int mbtowc(LPWSTR lpszBuffer, LPCSTR lpszText, int nCount));
EXTERN_DECLARE(long wcstol(LPCWSTR lpszDigit, LPWSTR * ppEnd, int ibase));
EXTERN_DECLARE(double wcstod(LPCWSTR lpszDigit, LPWSTR * ppEnd));
#endif
#if IF_EXPORT_SYMBOLS
#include "_chDeclare.cpp"
#endif

// declare some C interface
EXTERN_DECLARE_C(BOOL nameHelperLock());
EXTERN_DECLARE_C(LPVOID nameGetSystemObject(LPCSTR lpszObjName, int nSize, BOOL * pbFirstAlloc));
EXTERN_DECLARE_C(void nameHelperUnlock());

#endif // __chDeclare_h__
