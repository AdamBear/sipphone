#ifdef _DEBUGMEM
#include <ylhashmap.h>
#include "memcheck.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "ylstring.h"
#include "yllist.h"
#include <errno.h>

// #ifdef new
// #undef new
// #endif

// CMutex_t g_mutex;
CMutex_t & GetMemCheckMutex()
{
    static CMutex_t s_Instance;
    return s_Instance;
}

typedef YLHashMap<void *, const char *> MemCheckMap;
typedef YLHashMap<const char *, int> StaMap;

// MemCheckMap g_mapInfo;
MemCheckMap & GetMemCheckInfoMap()
{
    static MemCheckMap s_Instance;
    return s_Instance;
}

void MarkAlloc(void * pAddr, const char * lpLocation)
{
    if (NULL == lpLocation)
    {
        //printf("error MarkAlloc\n");
        return;
    }

    if (pAddr == NULL)
    {
        printf("error MarkAlloc\n");
        return;
    }


    CMyLock lock(GetMemCheckMutex());
    GetMemCheckInfoMap().put(pAddr, lpLocation);
}

void MarkFree(void * pAddr)
{
    CMyLock lock(GetMemCheckMutex());
    MemCheckMap & mapInfo = GetMemCheckInfoMap();
    MemCheckMap::iterator itr = mapInfo.find(pAddr);

    if (itr != mapInfo.end())
    {
        mapInfo.erase(itr);
    }
}

void * operator new (size_t size, const char * szLocation)
{
    void * pNew = malloc(size);
    if (pNew == NULL)
    {
        printf("alloc error:%m\n"
               "location[%s]\n"
               "size[%d]\n",
               errno,
               szLocation,
               size);
    }
    else
    {
        MarkAlloc(pNew, szLocation);
    }

    return pNew;
}

void * operator new[](size_t size, const char * szLocation)
{
    void * pNew = malloc(size);
    if (pNew == NULL)
    {
        printf("alloc error:%m\n"
               "location[%s]\n"
               "size[%d]\n",
               errno,
               szLocation,
               size);
    }
    else
    {
        MarkAlloc(pNew, szLocation);
    }

    return pNew;
}

void operator delete (void * p) throw()
{
    MarkFree(p);
    free(p);
}

void operator delete[](void * p) throw()
{
    MarkFree(p);
    free(p);
}

//////////////////////////////////////////////////////////////////////////
static LRESULT OnPrintTimerOnTimer(msgObject & msg)
{
    return GetDumpMemInstance().OnTimeoutMessage(msg.wParam);
}

CPrintTimer::CPrintTimer()
{
    SingleMsgReg(TM_TIMER, OnPrintTimerOnTimer);
    int nInterVal = 10 * 60 * 1000;

    timerSetThreadTimer((int)&m_iTimer, nInterVal);

    Dump(true);
}

BOOL CPrintTimer::OnTimeoutMessage(unsigned int iTimerID)
{
    if (iTimerID == (int)&m_iTimer)
    {
        Dump(false);
        return TRUE;

    }
    return FALSE;
}

void Dump(bool bFirst)
{
    printf("-----------------member Dump begin------------------\n");
    FILE * pFile = NULL;

    if (bFirst)
    {
        pFile = fopen("/tmp/log/memcheckFirstInfo.txt", "wt");
    }
    else
    {
        pFile = fopen("/tmp/log/memcheckInfo.txt", "wt");
    }

    if (NULL == pFile)
    {
        return;
    }

    StaMap fileLineMap;

//   StaMap::iterator fileIter;
//   StaMap::iterator fileLineIter;

    CMyLock lock(GetMemCheckMutex());
    MemCheckMap & mapInfo = GetMemCheckInfoMap();
    printf("mapInfo size[%d]\n", mapInfo.size());
    MemCheckMap::iterator itr = mapInfo.begin();
    for (; itr != mapInfo.end(); ++itr)
    {
        const char * szLocation = itr->second;

        StaMap::iterator fileLineIter = fileLineMap.find(szLocation);

        if (fileLineIter == fileLineMap.end())
        {
            fileLineMap.put(szLocation, 1);
        }
        else
        {
            ++fileLineIter->second;
        }
    }

    fprintf(pFile, "Totol size:%d\n", mapInfo.size());
    fprintf(pFile, "Memory leak: fileLine statistical:\n");

    StaMap::iterator fileLineIter = fileLineMap.begin();
    for (; fileLineIter != fileLineMap.end(); fileLineIter++)
    {
        fprintf(pFile, "Memory leak:[%s] [%d]\n", fileLineIter->first, fileLineIter->second);
    }

    fclose(pFile);
    fileLineMap.clear();
    printf("-----------------member Dump end------------------\n");
}

CPrintTimer & GetDumpMemInstance()
{
    static CPrintTimer s_Instance;

    return s_Instance;
}

//////////////////////////////////////////////////////////////////////////
// 动态方式初始化互斥锁
CMutex_t::CMutex_t()
{
#ifndef _WIN32
    // 嵌套锁，允许同一个线程对同一个锁成功获得多次，并通过多次unlock解锁.如果是不同线程请求，则在加锁线程解锁时重新竞争
    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_mutex, &mattr);
#endif
}

// 注销互斥锁
CMutex_t::~CMutex_t()
{
#ifndef _WIN32
    pthread_mutex_destroy(&m_mutex);
#endif
}

// 确保拥有互斥锁的线程对被保护资源的独自访问
int CMutex_t::Lock() const
{
#ifndef _WIN32
    return pthread_mutex_lock(&m_mutex);
#else
    return 0;
#endif
}

//释放当前线程拥有的锁，以使其它线程可以拥有互斥锁，对被保护资源进行访问
int CMutex_t::Unlock() const
{
#ifndef _WIN32
    return pthread_mutex_unlock(&m_mutex);
#else
    return 0;
#endif
}

// 利用C++特性，进行自动加锁
CMyLock::CMyLock(const CMutex_t & mute) : m_lock(mute)
{
    int nRec = 0;
    if (0 == (nRec = m_lock.Lock()))
    {
        // printf("[%d] %s\n", nRec, strerror(nRec));
        m_bLockSuc = true;
    }
    else
    {
        printf("[%d] %s\n", nRec, strerror(nRec));
        m_bLockSuc = false;
    }
}

// 利用C++特性，进行自动解锁
CMyLock::~CMyLock()
{
    if (m_bLockSuc)
    {
        // printf("************** UnLock\n");
        m_lock.Unlock();
    }
}

#endif
