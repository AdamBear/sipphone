#include "msgkithandler.h"
#include "interfacedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#ifdef _WIN32
#include <taskaction/modtaskaction.h>
#include <taskaction/taskactioncallbackbase.h>
#include <taskaction/taskaction.h>
#endif


#ifdef _WIN32
#define IDT_MSG_KIT_TIMER 100
#define MSG_KIT_PROC_TIMER 15
#define MSG_KIT_PROC_IDLE_TIMER 45
void CALLBACK MsgKitProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    ::KillTimer(NULL, IDT_MSG_KIT_TIMER);

    g_MsgKitHandler.OnMsgKitFDActive();
}
#else
extern pthread_key_t msgkit_private_key;
#endif

CMsgKitHandler::CMsgKitHandler()
    : CMKThread(VP_PHONE_THREAD_NAME, NULL)
#ifndef _WIN32
    , m_nMKFD(INVALID_SOCKET)
#endif
{
#if MK_MSG_USER_DEFINECONFIG
    SetMsgDepth(128);
#endif
}

CMsgKitHandler::~CMsgKitHandler()
{

}

CMsgKitHandler& CMsgKitHandler::getInstance()
{
    static CMsgKitHandler* s_pMsgKitHandler = NULL;

    if (NULL == s_pMsgKitHandler)
    {
        s_pMsgKitHandler = new CMsgKitHandler;
    }

    return (*s_pMsgKitHandler);
}

bool CMsgKitHandler::InitMsgKit()
{
    if (!Register())
    {
        APP_ERR("Register fail in InitMsgKit function.");

        return false;
    }

#ifdef _WIN32
    Launch();

    ::SetTimer(NULL, IDT_MSG_KIT_TIMER, MSG_KIT_PROC_TIMER, (TIMERPROC)MsgKitProc);
#else
    mkit_handle_t hMsgkit = GetHandle();

    pthread_setspecific(msgkit_private_key, (void*)hMsgkit);

    m_nMKFD = mkit_get_filedes(hMsgkit);

    if (INVALID_SOCKET == m_nMKFD)
    {
        return false;
    }
#endif

    // 向msgkit注册服务器,才能接收到其他进程的注册消息
    mkservice_register(mkGetHandle(), MKSRV_ID_DSK);

    return true;
}

void CMsgKitHandler::UnInitMsgKit()
{
#ifndef _WIN32
    m_nMKFD = INVALID_SOCKET;
#endif

    Destroy();
}

#ifdef _WIN32
void CMsgKitHandler::OnMsgKitFDActive()
{
    mkit_handle_t hMsgkit = GetHandle();
    mkit_message_t mkMsg = { 0 };

    mkMsg.flags = MKIT_MFL_NONE;
    mkMsg.sync = 10;

    int nResult = mkit_peek(hMsgkit, &mkMsg);

    if (nResult < 0)
    {
        ::SetTimer(NULL, IDT_MSG_KIT_TIMER, MSG_KIT_PROC_TIMER, (TIMERPROC)MsgKitProc);
        return;
    }
    else if (0 == nResult)
    {
        mkit_message_t msgIdle = { 0, 0, 0, MKIT_MSG_IDLE, 0, 0, 0, 0, 0 };

        CMKThread::OnEvent(hMsgkit, (CMKThread *)this, &msgIdle);

        mktask_on_default(hMsgkit, &msgIdle);

        ::SetTimer(NULL, IDT_MSG_KIT_TIMER, MSG_KIT_PROC_IDLE_TIMER, (TIMERPROC)MsgKitProc);

        return;
    }
    // TaskAction回调处理，通过peek实现回调
    if (mkMsg.message == TM_MESSAGE_PROXY)
    {
        AUTO_LOG_TIMER;
        mkMsgProxy * pMsgProxy = reinterpret_cast<mkMsgProxy *>(mkMsg.data_ptr);
        if (pMsgProxy != NULL && pMsgProxy->m_uMessage == TM_NOP)
        {
            using NS_TA::CTaskAction;
            using NS_TA::CTaskActionCallbackBase;

            CTaskAction * pTaskAction = reinterpret_cast<CTaskAction *>(mkMsg.lparam);
            CTaskActionCallbackBase * pTaskActionCallback =
                reinterpret_cast<CTaskActionCallbackBase *>(pMsgProxy->m_pMessageReceiver);

            if (pTaskActionCallback != NULL && pTaskAction != NULL)
            {
                pTaskActionCallback->OnTaskActionCallback(pTaskAction);

                if (mkMsg.flags & (MKIT_MFL_CALL_RET | MKIT_MFL_SYNC))
                {
                    mkit_message_t retmsg = { 0 };
                    mkit_reply(mkGetHandle(), &retmsg);
                }

                mk_free(mkMsg.data_ptr);

                ::SetTimer(NULL, IDT_MSG_KIT_TIMER, MSG_KIT_PROC_TIMER, (TIMERPROC)MsgKitProc);

                return;
            }
        }
    }

    CMKThread::OnEvent(hMsgkit, (CMKThread *)this, &mkMsg);

    mktask_on_default(hMsgkit, &mkMsg);

    if (mkMsg.message == MKIT_MSG_QUIT)
    {
        APP_INFO("App MKIT_MSG_QUIT received");

        BeforeThreadShutdown();

        ::KillTimer(NULL, IDT_MSG_KIT_TIMER);
    }

    mk_free(mkMsg.data_ptr);
    ::SetTimer(NULL, IDT_MSG_KIT_TIMER, MSG_KIT_PROC_TIMER, (TIMERPROC)MsgKitProc);
}
#else
void CMsgKitHandler::OnMsgKitFDActive()
{
    mkit_handle_t hMsgkit = GetHandle();
    mkit_message_t mkMsg;

    mkMsg.flags = MKIT_MFL_NONE;
    mkMsg.sync = 50;

    int nResult = mkit_peek(hMsgkit, &mkMsg);

    if (nResult < 0)
    {
        return;
    }
    else if (0 == nResult)
    {
        mkit_message_t msgIdle = {0, 0, 0, MKIT_MSG_IDLE, 0, 0, 0, 0, 0};

        CMKThread::OnEvent(hMsgkit, (CMKThread*)this, &msgIdle);

        mktask_on_default(hMsgkit, &msgIdle);

        return;
    }

    CMKThread::OnEvent(hMsgkit, (CMKThread*)this, &mkMsg);

    mktask_on_default(hMsgkit, &mkMsg);

    // http://bugfree.yealink.com/Bug.php?BugID=63845
    bool bTimerRunning = m_objTimer.IsTimerRuning();

    if (TM_MESSAGE_PROXY == mkMsg.message)
    {
        if (!bTimerRunning)
        {
            m_objTimer.SetTimer(10, NULL, MK_MSG_ENTRY(this, CMsgKitHandler::OnTimeOut));
        }
    }
    else if (bTimerRunning)
    {
        m_objTimer.KillTimer();
    }
}
#endif

// 回调函数，遍历消息函数表
BOOL CMsgKitHandler::OnMessage(msgObject& objMessage)
{
    /*
        APP_INFO("~~~~~~~~~~~~~~~~~~~APP Receive MSG:0x%x | wP:%d | lP:%d~~~~~~~~~~~~~~~~~~~~~~~",
            objMessage.message, objMessage.wParam, objMessage.lParam);
    */
    return etl_OnMessageProcess(objMessage);
}

#ifndef _WIN32
BOOL CMsgKitHandler::OnTimeOut(msgObject& refObj)
{
    m_objTimer.KillTimer();

    return TRUE;
}
#endif
