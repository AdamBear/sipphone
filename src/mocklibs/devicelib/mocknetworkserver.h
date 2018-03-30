#ifndef __MOCK_NETWORK_SERVER_H__
#define __MOCK_NETWORK_SERVER_H__

#include <gmock/gmock.h>
#include <time.h>

class MockNetworkServer
{
public:
    MockNetworkServer();

    MOCK_METHOD0(netGetLocalTime, time_t());
};

extern MockNetworkServer * g_pMockNetworkServer;

#define EXPECT_CALL_netGetLocalTime(now) \
        EXPECT_CALL(*g_pMockNetworkServer, netGetLocalTime()).WillRepeatedly(testing::Return(now))

#endif // !__MOCK_NETWORK_SERVER_H__
