#ifndef _DBG_MEM_CHECK_H
#define _DBG_MEM_CHECK_H

//如不需替换new则注释下面宏
#ifdef _DEBUGMEM

//#include <string_like.h>
//#include "YLAlgorithm.h"
//#include <YLVector.h>
//#include <YLList.h>
// #include <YLHashMap.h>
// #include "utility.hpp"
// #include "ylstring.h"
#if IS_QT_BUILD
#include <QtCore/qobject.h>
#include <QtGui/qbrush.h>
#include <QtCore/qdebug.h>
#include <QtCore/qsettings.h>
#include <QtCore/qsharedpointer.h>
#include <QtCore/QtCore>
#endif

#if IS_COLOR
#include <algorithm>
#include <map>
#endif

#include <new>
#include "ETLLib.hpp"

void ylListInitCountAdd();

void MarkAlloc(void* pAddr, const char* szLocation);
void MarkFree(void* pAddr);

#define MACROSTR(VAR) #VAR
#define VALUESTR(VAR) MACROSTR(VAR) 
#define MARKALLOC(pAddr) MarkAlloc(pAddr, __FILE__ ":" VALUESTR(__LINE__));
#define MARKFREE(pAddr) MarkFree(pAddr);

void* operator new(size_t size, const char * szLocation);
void* operator new[](size_t size, const char * szLocation);

void  operator delete(void* p) throw();
void  operator delete[](void* p) throw();

#define DEBUG_NEW new(__FILE__ ":" VALUESTR(__LINE__))

class CPrintTimer : public CMKProcessor
{
public:
    CPrintTimer();

    // 处理计时器到时消息.
    BOOL OnTimeoutMessage(unsigned int iTimerID);

private:
    int m_iTimer;
};

//////////////////////////////////////////////////////////////////////////
#ifndef _WIN32
#include <pthread.h>
#endif
// 互斥锁类
class CMutex_t
{
public:
    CMutex_t();
    virtual ~CMutex_t();

    virtual int Lock() const;
    virtual int Unlock() const;

private:
#ifndef _WIN32
    mutable pthread_mutex_t m_mutex;
#endif
};

// 锁
class CMyLock
{
public:
    CMyLock(const CMutex_t & mute);
    virtual ~CMyLock();

private:
    const CMutex_t & m_lock;
    bool m_bLockSuc;
};
//////////////////////////////////////////////////////////////////////////

// 输出内存检测结果
void Dump(bool bFirst);
// 开启定时
CPrintTimer & GetDumpMemInstance();

#else
#define DEBUG_NEW new

#define MARKALLOC(pAddr) ;
#define MARKFREE(pAddr) ;



#endif // end of _DEBUGMEM




#endif /*MEM_CHECK_LOCALIZE(MEM_CHECK_H*/

