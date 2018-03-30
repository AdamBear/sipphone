#include <gtest/gtest.h>

#include "mocketlmsghandler.h"

#include <service_config.h> // CONFIG_MSG_BCAST_CHANGED

LRESULT DummyCallback(msgObject & msg)
{
    return LRESULT();
}

TEST(MockEtlMsgHandler, RegisterMsgHandle)
{
    MockEtlMsgHandler objMock;
    g_pMockEtlMsgHandler = &objMock;

    ON_CALL(
        objMock,
        RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, testing::_)
    ).WillByDefault(testing::Return(TRUE));
    EXPECT_CALL(
        objMock,
        RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, testing::_)
    ).Times(1);
    EXPECT_TRUE(etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                                      DummyCallback));

    g_pMockEtlMsgHandler = NULL;
}

TEST(MockEtlMsgHandler, UnregisterMsgHandle)
{
    MockEtlMsgHandler objMock;
    g_pMockEtlMsgHandler = &objMock;

    ON_CALL(
        objMock,
        UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            CMSGSubscriber(DummyCallback))
    ).WillByDefault(testing::Return(TRUE));
    EXPECT_CALL(
        objMock,
        UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            CMSGSubscriber(DummyCallback))
    ).Times(1);
    EXPECT_TRUE(etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                                        DummyCallback));

    g_pMockEtlMsgHandler = NULL;
}

TEST(MockEtlMsgHandler, NotifyAppEx)
{
    MockEtlMsgHandler objMock;
    g_pMockEtlMsgHandler = &objMock;

    ON_CALL(
        objMock,
        NotifyAppEx(FALSE, CONFIG_MSG_BCAST_CHANGED, 0, 1, 0, NULL)
    ).WillByDefault(testing::Return(TRUE));
    EXPECT_CALL(
        objMock,
        NotifyAppEx(FALSE, CONFIG_MSG_BCAST_CHANGED, 0, 1, 0, NULL)
    ).Times(1);
    EXPECT_TRUE(etl_NotifyAppEx(FALSE, CONFIG_MSG_BCAST_CHANGED, 0, 1, 0, NULL));

    g_pMockEtlMsgHandler = NULL;
}
