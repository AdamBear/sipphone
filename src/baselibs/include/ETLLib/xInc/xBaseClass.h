#ifndef __xBaseClass_H__
#define __xBaseClass_H__
#include <ylstl/ylstring.h>
#if !LINUX_SYSTEM
#error only linux system supported
#endif

namespace ETL_BaseHelper
{

#define xGetConsoleLineInput            etlGetConsoleLineInput

//////////////////////////////////////////////////////////////////////////
// class xTimer
class xTimer
{
    friend class chTick;
public:
    static chTick GetTickFromBoot();
private:
    static chTick GetJiffiesTickCount();
    static chTick GetDayTickCount();
    xTimer();
};

//////////////////////////////////////////////////////////////////////////
// class xReadWriteLock
class xReadWriteLock
{
public:
    BOOL tryLockRead();
    BOOL LockRead();
    void UnlockRead();

    BOOL LockWrite();
    void UnlockWrite();
public:
    xReadWriteLock();
    ~xReadWriteLock();
private:
    pthread_rwlock_t    m_vReadWriteLock;
};

//////////////////////////////////////////////////////////////////////////
// class xCriticalSection
class xCriticalSection
{
public:
    BOOL IsLocked();
    BOOL IsLockedByThisThread();
public:
    BOOL Lock();
    void Unlock();
public:
    xCriticalSection();
    ~xCriticalSection();
private:
    // Private to avoid accidental use
    xCriticalSection(const xCriticalSection &);
    xCriticalSection & operator=(const xCriticalSection &);
private:
    pthread_mutex_t         m_vCriticalSection;
    THREAD_ID               m_idLockedThread;
};

//////////////////////////////////////////////////////////////////////////
// class xFileKey
hide_class xFileKey
{
public:
    key_t OpenFileKey(LPCSTR lpszFileName, BYTE byteKeyType, bool bCreate);
    void CloseFileKey();
    void DestroyFileKey();
private:
    void buildFilePath(LPSTR szFilePath, LPCSTR lpszFileName);
protected:
    // use stack memory for kill dependence
    ACHAR           m_szFilePath[MAX_PATH];
};

//////////////////////////////////////////////////////////////////////////
// class xSemaphore
class xSemaphore : public xFileKey
{
    union semun
    {
        int val;                    /* value for SETVAL */
        struct semid_ds * buf;      /* buffer for IPC_STAT, IPC_SET */
        unsigned short * array;     /* array for GETALL, SETALL */
        /* Linux specific part: */
        struct seminfo * __buf;     /* buffer for IPC_INFO */
    };
public:
    BOOL IsCreateByMe();
    BOOL OpenSemaphore(LPCSTR lpszFileName);
    void CloseSemaphore();
    void DestroySemaphore();

protected:
    int  SemaphoreAddValue(int iValue, int nTimeout = NEVER_TIMEOUT);
    BOOL SemaphoreSetValue(int iValue);
public:
    xSemaphore();
    ~xSemaphore();
private:
    // Private to avoid accidental use
    xSemaphore(const xSemaphore &);
    xSemaphore & operator=(const xSemaphore &);
protected:
    TASK_ID     m_idCreatorTask;
    int         m_fdSemaphore;
};

//////////////////////////////////////////////////////////////////////////
// class xMapFile
class xMapFile : public xFileKey
{
public:
    LPVOID OpenMapFile(LPCSTR lpszFileName);
    LPVOID OpenMapFile(LPCSTR lpszFileName, int nSize);
    void CloseMapFile();
    void DestroyMapFile();
    void Flush();
    BOOL IsCreateByMe();
    LPVOID getData();
public:
    xMapFile();
    ~xMapFile();
private:
    // Private to avoid accidental use
    xMapFile(const xMapFile &);
    xMapFile & operator=(const xMapFile &);
protected:
    TASK_ID     m_idCreatorTask;
    LPVOID      m_pMapData;
    int         m_fdMapFile;
};

//////////////////////////////////////////////////////////////////////////
// class xRawMapFile
class xRawMapFile
{
public:
    LPVOID OpenMapFile(LPCSTR lpszFileName, int nSize);
    void CloseMapFile();
    void DestroyMapFile();
    void Flush();
    BOOL IsCreateByMe();
    LPVOID getData();
public:
    xRawMapFile();
    ~xRawMapFile();
private:
    // Private to avoid accidental use
    xRawMapFile(const xRawMapFile &);
    xRawMapFile & operator=(const xRawMapFile &);
public:
    int                 m_fdMapFile;
    LPVOID              m_pMapData;
    int                 m_nFileSize;
    TASK_ID             m_idCreatorTask;
    char                m_szFileName[MAX_PATH];
};

//////////////////////////////////////////////////////////////////////////
// class xMutex
class xMutex : public xSemaphore
{
public:
    BOOL IsLocked();
    BOOL IsLockedByThisThread();
public:
    BOOL OpenMutex(LPCSTR lpszMutexName);
    void CloseMutex();
    void DestroyMutex();
    BOOL Lock(int nTimeout = NEVER_TIMEOUT);
    void Unlock();
public:
    xMutex();
    ~xMutex();
protected:
    THREAD_ID       m_idLockedThread;
};

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// class xEvent
class xEvent : public xSemaphore
{
public:
    BOOL OpenEvent(LPCSTR lpszFileName);
    void CloseEvent();
    void DestroyEvent();

    BOOL WaitEvent(int nTimeout = NEVER_TIMEOUT);
    BOOL SetEvent();
    void ResetEvent();
};

//////////////////////////////////////////////////////////////////////////
// class xStdInput
#if 0
class xStdInput
{
public:
    int getch();
private:
    int PeekKeyToBuffer();
    int TransBufferToKey();
    void RemoveKeys(int nCount);
public:
    xStdInput();
    ~xStdInput();
private:
    termios             m_oldTermIO;
    char                m_arrBuffer[10];
    int                 m_nBufferBytes;
};
#endif

//////////////////////////////////////////////////////////////////////////
// class xPathWalker
class xPathWalker
{
public:
    BOOL OpenPath(LPCSTR lpszPathName);
    BOOL ReadFileName(yl::string & strFileName);
    void ClosePath();
public:
    xPathWalker(LPCSTR lpszPathName = NULL);
    ~xPathWalker();
private:
    DIR          *          m_pTargetDir;
};

};
using namespace ETL_BaseHelper;

#endif //__xBaseClass_H__
