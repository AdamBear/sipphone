#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include <gtest/gtest.h>

#include <ETLLib.hpp>
#include <netmsgtypedefine.h>
#include <configiddefine.h>
#include <dsklog/log.h>
#include <etlmsghandler/modetlmsghandler.h>
#include <configparser/modconfigparser.h>
#include <configiddefine.h>
#include <cfgservice/service_config.h>
#include <devicelib/phonedevice.h>

#include "modtalklogic.h"
#include "talklogic_inc.h"

#define GTEST_THREAD "app_gtest_thread"

bool g_bAfterInit = true;

#if IS_COVER_TEST
extern "C" void __gcov_flush(void);
#endif

class EndExit : public testing::Environment
{
public:
    virtual void SetUp()
    {

    }
    virtual void TearDown()
    {
#if IS_COVER_TEST
        //刷新gcov，生成gcna
        __gcov_flush();
#endif
    }
};

LRESULT CALLBACK talklogic_test_msg(msgObject * pMessage)
{
    switch (pMessage->message)
    {
    case TM_INIT:
        //config_init(NULL, 0);
        //devicelib_Init();
        talklogic_Init();
        return RUN_ALL_TESTS();
        break;
    default:
        break;
    };

    return false;
}

int main(int argc, char * argv[])
{
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new EndExit);
    mkit_handle_t hTestThread = mkRegistThread(GTEST_THREAD, talklogic_test_msg);
    mkRunThread(hTestThread);
    return 0;
}
