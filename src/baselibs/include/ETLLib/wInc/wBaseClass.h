#ifndef __wBaseClass_H__
#define __wBaseClass_H__

#if !WINDOWS_SYSTEM
#error only windows system supported
#endif

namespace ETL_BaseHelper
{

#define wGetConsoleLineInput            etlGetConsoleLineInput

//////////////////////////////////////////////////////////////////////////
// class wTimer
class wTimer
{
    friend class chTick;
public:
    static chTick GetTickFromBoot();
    static UINT64 GetTimeFromBoot_ms();
private:
    wTimer();
private:
    LARGE_INTEGER m_thisFreq;
    static wTimer timeFromBoot;
};

//////////////////////////////////////////////////////////////////////////
// class wTimerRT
class wTimerRT
{
    wTimerRT(const wTimerRT & objSrc);              // no implementation
    void operator=(const wTimerRT & objSrc);    // no implementation
public:
    wTimerRT();
    ~wTimerRT();
    BOOL SetTimerRT(double dTimer_ms);
    void KillTimerRT();
protected:
    virtual void OnTimerRT(UINT nTimerID) = 0;
private:
    static void CALLBACK TimerCallBack(UINT uEventID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
protected:
    UINT        m_wTimerRes;
    UINT        m_nTimerID;
};

//////////////////////////////////////////////////////////////////////////
// class wCriticalSection
class wCriticalSection
{
public:
    BOOL IsLocked()
    {
        return m_bIsLocked;
    }
    BOOL Lock();
    void Unlock();
public:
    wCriticalSection();
    ~wCriticalSection();
private:
    // Private to avoid accidental use
    wCriticalSection(const wCriticalSection &);
    wCriticalSection & operator=(const wCriticalSection &);
private:
    CRITICAL_SECTION        m_objCriSec;
    BOOL                    m_bIsLocked;
};

#include <sddl.h>
//////////////////////////////////////////////////////////////////////////
// class wSAEveryone
hide_class wSAEveryone :
public SECURITY_ATTRIBUTES
{
public:
    wSAEveryone(DWORD dwAccessRight) ;
    ~wSAEveryone();
private:
    BOOL InitEveryoneSA(DWORD dwAccessRight);
private:
    PSID                    m_pEveryoneSID;
    PACL                    m_pACL;
    SECURITY_DESCRIPTOR     m_vSD;
};

//////////////////////////////////////////////////////////////////////////
// class wSemaphore
class wSemaphore
{
public:
    BOOL OpenSemaphore(LPCSTR lpszFileName);
    void CloseSemaphore();
    void DestroySemaphore();

    BOOL SemaphoreConstrain(int nTimeout = NEVER_TIMEOUT);
    void SemaphoreRelease(int nCount);

    BOOL IsCreateByMe();
public:
    wSemaphore();
    ~wSemaphore();
private:
    BOOL        m_bIsCreateByMe;
    HANDLE      m_hSemaphoreObject;
};

//////////////////////////////////////////////////////////////////////////
// class wMapFile
class wMapFile
{
public:
    LPVOID OpenMapFile(LPCSTR lpszFileName, int nSize);
    LPVOID OpenMapFile(LPCSTR lpszFileName);
    void CloseMapFile();
    void DestroyMapFile();
    void Flush();
    BOOL IsCreateByMe();
    LPVOID getData();
public:
    wMapFile();
    ~wMapFile();
private:
    // Private to avoid accidental use
    wMapFile(const wMapFile &);
    wMapFile & operator=(const wMapFile &);
protected:
    BOOL        m_bIsCreateByMe;
    LPVOID      m_pMapData;
    HANDLE      m_hMapFileObject;
};

//////////////////////////////////////////////////////////////////////////
// class wRawMapFile
class wRawMapFile
{
public:
    LPVOID OpenMapFile(LPCSTR lpszFileName, int nSize);
    void CloseMapFile();
    void DestroyMapFile();
    void Flush();
    BOOL IsCreateByMe();
    LPVOID getData();
public:
    wRawMapFile();
    ~wRawMapFile();
private:
    // Private to avoid accidental use
    wRawMapFile(const wRawMapFile &);
    wRawMapFile & operator=(const wRawMapFile &);
protected:
    yl::string      m_strFileName;
    HANDLE              m_hRawFile;
    HANDLE              m_hMapFileObject;
    LPVOID              m_pMapData;
    BOOL                m_bIsCreateByMe;
};

//////////////////////////////////////////////////////////////////////////
// class wMutex
class wMutex
{
public:
    BOOL OpenMutex(LPCSTR lpszMutexName);
    BOOL IsCreateByMe();
    void CloseMutex();
    void DestroyMutex();
    BOOL Lock(int nTimeout = NEVER_TIMEOUT);
    BOOL Unlock();
public:
    wMutex();
    ~wMutex();
private:
    // Private to avoid accidental use
    wMutex(const wMutex &);
    wMutex & operator=(const wMutex &);
protected:
    BOOL        m_bIsLocked;
    BOOL        m_bIsCreateByMe;
    HANDLE      m_hMutexObject;
};

//////////////////////////////////////////////////////////////////////////
// class wEvent
class wEvent
{
public:
    BOOL OpenEvent(LPCSTR lpszFileName);
    BOOL IsCreateByMe();
    void CloseEvent();
    void DestroyEvent();

    BOOL WaitEvent(int nTimeout = NEVER_TIMEOUT);
    BOOL SetEvent();
    void ResetEvent();
public:
    wEvent();
    ~wEvent();
private:
    // Private to avoid accidental use
    wEvent(const wEvent &);
    wEvent & operator=(const wEvent &);
private:
    BOOL        m_bIsCreateByMe;
    HANDLE      m_hEventObject;
};

//////////////////////////////////////////////////////////////////////////
// class wStdInput
class wStdInput
{
public:
    int getch();
public:
    wStdInput();
    ~wStdInput();
private:
    HANDLE              m_hConsoleStdin;
    DWORD               m_dwConsoleMode;
};

//////////////////////////////////////////////////////////////////////////
// class wPathWalker
class wPathWalker
{
public:
    BOOL OpenPath(LPCSTR lpszPathName);
    BOOL ReadFileName(yl::string & strFileName);
    void ClosePath();
public:
    wPathWalker(LPCSTR lpszPathName = NULL);
    ~wPathWalker();
private:
    yl::string              m_strPathName;
    HANDLE                  m_hDir;
};

};
using namespace ETL_BaseHelper;

#endif //__wBaseClass_H__
