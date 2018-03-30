/************************************************************************
 * FileName  : QDSKApplication.cpp (implementation file)
 * Purpose   :
 * Date      : 2012/09/01 11:05:01
 ************************************************************************/
#include "qdskapplication.h"
#include "interfacedefine.h"
#include <pthread.h>

#ifdef WIN32
#include <QTimer>
#include <taskaction/modtaskaction.h>
#include <taskaction/taskactioncallbackbase.h>
#include <commonapi/systemhelperapi.h>
#include <taskaction/taskaction.h>
#include "talk/talklogic/include/modtalklogic.h"
#include "uimanager/moduimanager.h"
#include <winimp/winimp.h>

struct CPhoneInit
{
    CPhoneInit()
    {
        WAPIMainProcessInit();
    }

    ~CPhoneInit()
    {
    }
};

#pragma init_seg(lib)
static CPhoneInit s_PhoneInitParam;
#else
extern pthread_key_t msgkit_private_key;
#endif

// 到主线程的MsgQueue取一条ETL消息的间隔，单位为毫秒
static const int  GET_MSG_TIME_INTERVAL = 40;

QDSKApplication::QDSKApplication(int & argc, char ** argv)
    : QApplication(argc, argv)
    , CMKThread(VP_PHONE_THREAD_NAME, NULL)
    , m_pMKNotifier(NULL)
    , m_pTimer(NULL)
{
    InitMsgKit();

    m_pTimer = new QTimer(this);
    if (NULL != m_pTimer)
    {
        QObject::connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));

#ifdef WIN32
        m_pTimer->start(15);
#endif
    }
}

QDSKApplication::~QDSKApplication()
{
    UnInitMsgKit();

    if (NULL != m_pTimer)
    {
        QObject::disconnect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));

        delete m_pTimer;
        m_pTimer = NULL;
    }
}

void QDSKApplication::InitMsgKit()
{
#if MK_MSG_USER_DEFINECONFIG
    SetMsgDepth(128);
#endif
    Register();
    mkit_handle_t hMsgkit = GetHandle();

#ifdef WIN32
    Launch();
#else
    pthread_setspecific(msgkit_private_key, (void *)hMsgkit);
#endif
    int nMKFD = mkit_get_filedes(hMsgkit);

    if (INVALID_SOCKET == nMKFD)
    {
        return;
    }

    ReleaseNotifier();

    m_pMKNotifier = new QSocketNotifier(nMKFD, QSocketNotifier::Read, NULL);

    if (NULL != m_pMKNotifier)
    {
        QObject::connect(m_pMKNotifier, SIGNAL(activated(int)), this, SLOT(OnMsgKitFDActive(int)));

        m_pMKNotifier->setEnabled(true);
    }
}

void QDSKApplication::UnInitMsgKit()
{
    ReleaseNotifier();

    Destroy();
}

void QDSKApplication::ReleaseNotifier()
{
    if (NULL == m_pMKNotifier)
    {
        return;
    }

    m_pMKNotifier->setEnabled(false);
    QObject::disconnect(m_pMKNotifier, SIGNAL(activated(int)), this, SLOT(OnMsgKitFDActive(int)));

    delete m_pMKNotifier;
    m_pMKNotifier = NULL;
}

void QDSKApplication::OnMsgKitFDActive(int socket)
{
#ifdef WIN32
    mkit_handle_t hMsgkit = GetHandle();
    mkit_message_t mkMsg = {0};

    mkMsg.flags = MKIT_MFL_NONE;
    mkMsg.sync = 10;

    int nResult = mkit_peek(hMsgkit, &mkMsg);

    if (nResult < 0)
    {
        m_pTimer->start(10);
        return;
    }
    else if (0 == nResult)
    {
        mkit_message_t msgIdle = {0, 0, 0, MKIT_MSG_IDLE, 0, 0, 0, 0, 0};

        CMKThread::OnEvent(hMsgkit, (CMKThread *)this, &msgIdle);

        mktask_on_default(hMsgkit, &msgIdle);
        m_pTimer->start(50);
        return;
    }

    m_pTimer->start(15);

    // TaskAction回调处理，通过peek实现回调
    if (mkMsg.message == TM_MESSAGE_PROXY)
    {
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
                    mkit_message_t retmsg = {0};
                    mkit_reply(mkGetHandle(), &retmsg);
                }

                mk_free(mkMsg.data_ptr);
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
        m_pTimer->stop();

        talklogic_ExitAllSession();
        UIManager_ReturnToIdle();
    }

    mk_free(mkMsg.data_ptr);
#else
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

        CMKThread::OnEvent(hMsgkit, (CMKThread *)this, &msgIdle);

        mktask_on_default(hMsgkit, &msgIdle);
        return;
    }

    CMKThread::OnEvent(hMsgkit, (CMKThread *)this, &mkMsg);

    mktask_on_default(hMsgkit, &mkMsg);

    // 容错处理：
    // 定时器到说明主线程消息队列为空
    // 再次显示调用peek，告知上次消息已处理
    // http://10.3.5.199/Bug.php?BugID=63845
    if (TM_MESSAGE_PROXY == mkMsg.message)
    {
        m_pTimer->start(10);
    }
#endif
}

void QDSKApplication::OnTimeOut()
{
    if (NULL != m_pTimer)
    {
        m_pTimer->stop();
    }

    if (NULL != m_pMKNotifier)
    {
        OnMsgKitFDActive(m_pMKNotifier->socket());
    }
}
