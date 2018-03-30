#ifdef IF_WIFI_SIMULATION_DRIVER
#include "wifitestdriverthread.h"

IMPLEMENT_GETINSTANCE(CWIFITestDriverThread)

CWIFITestDriverThread::CWIFITestDriverThread()
    : CMKThread(VP_WIFI_TESTDRIVERTHREAD_NAME, NULL)
{

}

CWIFITestDriverThread::~CWIFITestDriverThread()
{

}

// 初始化
BOOL CWIFITestDriverThread::AfterThreadLaunched()
{
    return TRUE;
}

void CWIFITestDriverThread::BeforeThreadShutdown()
{
}

BOOL CWIFITestDriverThread::OnETLMessage(msgObject & objMessage)
{
    if (objMessage.message != WIFI_MSG_TEST)
    {
        return FALSE;
    }

    if (objMessage.wParam <= WIFITEST_MSG_BEGIN
            || objMessage.wParam >= WIFITEST_MSG_END)
    {
        return FALSE;
    }

    return etl_NotifyAppEx(FALSE, WIFI_MSG_TO_TESTDRIVER, objMessage.wParam,
                           objMessage.lParam, objMessage.GetExtraSize(), objMessage.GetExtraData());
}
#endif //IF_WIFI_SIMULATION_DRIVER
