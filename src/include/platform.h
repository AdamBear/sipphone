/************************
windows 编译的头文件
*************************/

#pragma once

#ifdef _MSC_VER

#pragma warning(disable:4099) // type name first seen using 'class' now seen using 'struct'
//#pragma warning(disable:4005) // macro redefinition
#pragma warning(disable:4800) // forcing value to bool
#pragma warning(disable:4200) // nonstandard extension used : zero-sized array in struct/union
#pragma warning(disable:4244) // conversion from 'WPARAM' to 'UINT', possible loss of data
#pragma warning(disable:4018) // '<' : signed/unsigned mismatch
#pragma warning(disable:4805) // '==' : unsafe mix of type 'bool' and type 'BOOL' in operation
#pragma warning(disable:4267) // 'argument' : conversion from 'size_t' to 
#pragma warning(disable:4312) // 'type cast' : conversion from 'unsigned int' to 'void *' of greater size
#pragma warning(disable:4311) // 'type cast' : pointer truncation from 'void *' to 'unsigned int'
#pragma warning(disable:4715) // not all control paths return a value
#pragma warning(disable:4996) // unreferenced formal parameter
#pragma warning(disable:4309) // 'initializing' : truncation of constant value
#pragma warning(disable:4065) // switch statement contains 'default' but no 'case' labels
#pragma warning(disable:4101) // unreferenced local variable
#pragma warning(disable:4293) // '<<' : shift count negative or too big, undefined behavior
#pragma warning(disable:4554) // '>>' : check operator precedence for possible error; use parentheses to clarify precedence
#pragma warning(disable:4700) // uninitialized local variable 'hop_type_first' used
#pragma warning(disable:4518) // '__declspec(dllexport)' : storage-class or type specifier(s) unexpected here; ignored
#pragma warning(disable:4502) // 'linkage specification' requires use of keyword 'extern' and must precede all other specifiers
#pragma warning(disable:4251)
#pragma warning(disable:4275)
#pragma warning(disable:4273)
#pragma warning(disable:4190)
#pragma warning(disable:4042) // C4042: 'padd6' : has bad storage class
#pragma warning(disable:4067)
#pragma warning(disable:4995)
#pragma warning(disable:4819) // The file contains a character that cannot be represented in the current code page (936). Save the file in Unicode format to prevent data loss


#ifdef BASELIB_BUILD
#define EXPORT_API __declspec (dllexport)
#else
#define EXPORT_API __declspec (dllimport)
#endif

#define DECLARE_ATTR

#define IF_AUTO_TIMER

#if defined(__cplusplus) && (__cplusplus < 201103L)
/* vs2015默认的cplusplus还是199711L */
#if defined(_MSC_VER) && (_MSC_VER <= 1500)
#ifndef nullptr
#define nullptr NULL
#endif
#ifndef nullptr_t
#define nullptr_t NULL
#endif
#endif
#endif

#if defined(_MSC_VER) && _MSC_VER < 1600
#   pragma include_alias("stdint.h", "stdio.h")
#   pragma include_alias(<stdint.h>, <stdio.h>)
#endif
#if defined(_MSC_VER) && _MSC_VER < 1700
#   pragma include_alias("stdbool.h", "stdio.h")
#   pragma include_alias(<stdbool.h>, <stdio.h>)
#endif

#ifdef BASELIB_BUILD
#   pragma include_alias(<unistd.h>, <stdio.h>)
#   pragma include_alias("unistd.h", "stdio.h")
#endif

#ifdef __cplusplus
#include <string>
#include <time.h>
#include <dsklog/log.h>

struct CAutoTimer
{
    CAutoTimer(const char * lpFunc, unsigned long uLine) : _strFunction(lpFunc), _uLine(uLine)
    {
        _start_time = getCurrentTimer();
    }

    void restart()
    {
        _start_time = getCurrentTimer();
    }

    unsigned long elapsed()
    {
        return getCurrentTimer() - _start_time;
    }

    void trace(const std::string & strFunc, unsigned long uLine)
    {
        APP_INFO("AutoTimerLog Fuction=%s Line=%ld Time=%ld", strFunc.c_str(), uLine, elapsed());
    }

    ~CAutoTimer()
    {
        trace(_strFunction, _uLine);
    }

#if 1
    unsigned long getCurrentTimer()
    {
        return clock();
    }
#else
    unsigned long getCurrentTimer()
    {
        return GetTickCount();
    }
#endif

private:
    unsigned long   _start_time;
    unsigned long   _uLine;
    std::string     _strFunction;
};

template <typename T>
struct CAutoDeleteGuard
{
    CAutoDeleteGuard(T * p) : point(p) {}
    ~CAutoDeleteGuard()
    {
        delete point;
        point = NULL;
    }
private:
    T * point;
};

template <typename T>
struct CAutoDeleteArrGuard
{
    CAutoDeleteArrGuard(T * p) : point(p) {}
    ~CAutoDeleteArrGuard()
    {
        delete [] point;
        point = NULL;
    }
private:
    T * point;
};


#define AUTO_LOG_TIMER \
     CAutoTimer autoLogTimer(__FUNCTION__, __LINE__);

#define AUTO_LOG_TIMER_TRACE \
     autoLogTimer.trace(__FUNCTION__, __LINE__);

#define AUTO_LOG_TIMER_RESTART \
     autoLogTimer.restart();
#else
#define inline  __inline
#endif // __cplusplus

#endif // _MSC_VER

#ifndef AUTO_LOG_TIMER
#define AUTO_LOG_TIMER
#endif

#ifndef AUTO_LOG_TIMER_TRACE
#define AUTO_LOG_TIMER_TRACE
#endif

#ifndef AUTO_LOG_TIMER_RESTART
#define AUTO_LOG_TIMER_RESTART
#endif

#ifndef EXPORT_API
#define EXPORT_API
#endif