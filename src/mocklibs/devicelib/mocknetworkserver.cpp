#include "mocknetworkserver.h"

MockNetworkServer * g_pMockNetworkServer = NULL;

MockNetworkServer::MockNetworkServer()
{
    // 严格模式：如果用例中没有EXPECT_CALL，则不允许被测程序调用接口
    EXPECT_CALL(*this, netGetLocalTime()).Times(0);
}

#ifdef __cplusplus
extern "C" {
#endif

time_t netGetLocalTime()
{
    if (g_pMockNetworkServer)
    {
        return g_pMockNetworkServer->netGetLocalTime();
    }
    return 0;
}

#ifdef __cplusplus
} // extern "C"
#endif
