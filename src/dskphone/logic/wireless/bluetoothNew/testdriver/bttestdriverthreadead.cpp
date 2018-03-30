#ifdef IF_BT_TEST_LOGIC
#include "bttestdriverthread.h"

IMPLEMENT_GETINSTANCE(CBTTestDriverThread)

CBTTestDriverThread::CBTTestDriverThread()
    : CMKThread(VP_BLUETOOTH_TESTDRIVERTHREAD_NAME, NULL)
{

}


CBTTestDriverThread::~CBTTestDriverThread()
{

}

// 初始化
BOOL CBTTestDriverThread::AfterThreadLaunched()
{
    return TRUE;
}

void CBTTestDriverThread::BeforeThreadShutdown()
{
}

BOOL CBTTestDriverThread::OnETLMessage(msgObject & objMessage)
{
    if (objMessage.message != BLUETOOTH_MSG_TEST)
    {
        return FALSE;
    }

    if (objMessage.wParam <= BTTEST_MSG_BEGIN
            || objMessage.wParam >= BTTEST_MSG_END)
    {
        return FALSE;
    }

    return etl_NotifyAppEx(FALSE, BLUETOOTH_MSG_TO_TESTDRIVER, objMessage.wParam, objMessage.lParam,
                           objMessage.GetExtraSize(), objMessage.GetExtraData());
}
#endif // IF_BT_TEST_LOGIC
