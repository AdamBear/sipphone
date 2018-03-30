#ifndef __MK_THREAD_H__
#define __MK_THREAD_H__

#include "ETLLib/ETLLib.hpp"
#include "msgobject.h"
#include "dskthreaddefine.h"
#include "msgkit/mkmessage.h"
#include "msgkit/mkservice.h"
#include <ylstl/yllist.h>
#include <ylstl/ylhashmap.h>

//#define MKIT_TIMER_LIST   (MKIT_MSG_USR_MIN + 0x010) /*min user-define message id*/

//////////////////////////////////////////////////////////////////////////
class CMKProcessor
{
public:
    virtual BOOL TryThreadMessage(msgObject & objMessage);
public:
    CMKProcessor(UINT64 uID = 0);
    virtual UINT64 GetProcrssorID() ;

    /**
     * @brief 加入代理列表.
     *
     * @author Song
     * @date 2018/1/17
     */
    void InstallProxy(const char * file, int line);

    /**
     * @brief 从代理列表移除.
     *
     * @author Song
     * @date 2018/1/17
     */
    void UninstallProxy(const char * file, int line);

    virtual ~CMKProcessor();
private:
    UINT64 m_uProcessID;
};

typedef BOOL(CMKProcessor::*mkMsgEntry)(msgObject & objMessage);
#define MK_MSG_ENTRY(pObj, meth) pObj, static_cast<mkMsgEntry>(&meth)

//////////////////////////////////////////////////////////////////////////
// class mkMsgProxy
class mkMsgProxy
{
public:
    UINT                    m_uMessage;
    CMKProcessor      *     m_pMessageReceiver;
};

//////////////////////////////////////////////////////////////////////////
// some marco for message processer

//////////////////////////////////////////////////////////////////////////
class CMKThread;
class mkThreadTimer;
class CMKThread : public CMKProcessor
{
public:
    CMKThread(const char * cszTName, THREAD_MSG_PROC_EX pCallbackEx = NULL);
    virtual ~CMKThread();

public:
    virtual BOOL AfterThreadLaunched()
    {
        return true;
    }

    virtual void BeforeThreadShutdown();

    virtual void DefaultProcessMessage(msgObject & msg);

    virtual BOOL TryThreadMessage(msgObject & msg);
#ifdef WIN32
    virtual bool IsRegisterCallBack()
    {
        return true;
    }
#endif

public:
    // Register. Timer can't be set before Run.
    bool Register();

    // Launch a new thread
    bool Launch();

    // Run on current thread
    int Run();

    // Destroy
    bool Destroy();

    // Thread name
    bool SetTName(const char * cszTName);
    yl::string GetTName();

    // msgkit flags
    DSK_HANDLE_T GetHandle()
    {
        return m_handle;
    }

    DSK_TARGET_T GetTarget();

#if MK_MSG_USER_DEFINECONFIG
    inline void SetMsgDepth(int iDepth)
    {
        m_uMsgDepth = iDepth;
    }

    inline void SetBufferParam(int iBufferMax, int iBufferPer)
    {
        m_uBufferPer = iBufferPer;
        m_uBufferMax = iBufferMax;
    }

    inline void SetTimerDepth(int iTimerDepth)
    {
        m_uTimerMax = iTimerDepth;
    }

    inline void SetThreadPriority(int iPriority)
    {
        m_iPriority = iPriority;
    }
#endif // 0
protected:

    static int OnEvent(mkit_handle_t handle, void * private_data, const mkit_message_t * msg);

private:
    DSK_HANDLE_T m_handle;
    yl::string   m_strTName;
    THREAD_MSG_PROC_EX m_pCallbackEx;
    const mkit_message_t * m_cpMKMsg;

private:
#if MK_MSG_USER_DEFINECONFIG
    int m_uMsgDepth;
    int m_uBufferMax;
    int m_uBufferPer;
    int m_uTimerMax;
    int m_iPriority;
#endif // MK_MSG_USER_DEFINECONFIG
};


//////////////////////////////////////////////////////////////////////////
// mkThreadTimer
class mkThreadTimer : public CMKProcessor
{
    friend class CMKThread;
public:
    BOOL SetTimer(UINT64 nElapse, void * lpExtraData, CMKProcessor * pTimerReceiver,
                  mkMsgEntry pTimerMeth);

    BOOL SetTimer(UINT64 nElapse, CMKProcessor * pTimerReceiver);
    void KillTimer();

#ifdef WIN32
    void KillTimerEx();
#endif
    BOOL IsTimerRuning();

    // operator const TIMER_ID() ;

    CMKThread & hostThread();

    virtual UINT64 GetProcrssorID()
    {
        return static_cast<UINT64>(m_iTimerID);
    }

    TIMER_ID GetTimerID()
    {
        return m_iTimerID;
    }
public:
    mkThreadTimer(TIMER_ID nTimerID = 0);
    ~mkThreadTimer();

    CMKProcessor    *   m_pTimerReceiver;
    mkMsgEntry          m_pTimerMeth;
    CMKThread     *     m_pHostThread;
    void        *       m_pExtraData;
private:
    UINT64              m_nElapse;
    TIMER_ID            m_iTimerID;
};

#endif // __MK_THREAD_H__


