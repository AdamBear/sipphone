#ifdef IF_WIFI_SIMULATION_DRIVER
#ifndef _WIFITESTDRIVERTHREAD_H_
#define _WIFITESTDRIVERTHREAD_H_

#include "../src/wifi_inc.h"

#define WIFI_MSG_TEST          WIFI_MESSAGE_END - 1
#define WIFI_MSG_TO_TESTDRIVER WIFI_MESSAGE_END - 2
#define VP_WIFI_TESTDRIVERTHREAD_NAME "WIFITestThread"

enum WIFITestOption
{
    WIFITEST_MSG_BEGIN = 0,

    WIFITEST_MSG_END,
};

class CWIFITestDriverThread : public CMKThread
{
    BEGIN_MSG_MAP(CWIFITestDriverThread)
    CHAIN_MSG_ON_METHOD(OnETLMessage)
    CHAIN_MSG_TO_BASE(CMKThread)
    END_MSG_MAP()

    DECLARE_SINGLETON_CLASS(CWIFITestDriverThread)

    //父类继承消息
    virtual BOOL AfterThreadLaunched();
    virtual void BeforeThreadShutdown();

protected:
    //消息处理函数
    BOOL OnETLMessage(msgObject & objMessage);
};

#define _WIFITestThread GET_SINGLETON_INSTANCE(CWIFITestDriverThread)

#endif
#endif //IF_WIFI_SIMULATION_DRIVER
