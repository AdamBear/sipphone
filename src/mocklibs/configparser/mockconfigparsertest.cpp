#include <gtest/gtest.h>

#include "mockconfigparser.h"

#define TEST_CONFIG_INT_KEY kszAutoProvisionPowerOn
TEST(MockConfigParser, GetConfigInt)
{
    MockConfigParser objMock;
    g_pMockConfigParser = &objMock;

    ON_CALL(objMock, GetConfigInt(TEST_CONFIG_INT_KEY,
                                  CONFIG_LEVEL_IGNORE)).WillByDefault(testing::Return(1));
    EXPECT_CALL(objMock, GetConfigInt(TEST_CONFIG_INT_KEY, CONFIG_LEVEL_IGNORE)).Times(1);
    EXPECT_EQ(configParser_GetConfigInt(TEST_CONFIG_INT_KEY), 1);

    g_pMockConfigParser = NULL;
}

TEST(MockConfigParser, SetConfigInt)
{
    MockConfigParser objMock;
    g_pMockConfigParser = &objMock;

    ON_CALL(objMock, SetConfigInt(TEST_CONFIG_INT_KEY, 1,
                                  CONFIG_LEVEL_IGNORE)).WillByDefault(testing::Return(true));
    EXPECT_CALL(objMock, SetConfigInt(TEST_CONFIG_INT_KEY, 1, CONFIG_LEVEL_IGNORE)).Times(1);
    EXPECT_TRUE(configParser_SetConfigInt(TEST_CONFIG_INT_KEY, 1));

    g_pMockConfigParser = NULL;
}

#define TEST_CONFIG_STR_KEY kszAutoProvisionUrl
#define TEST_CONFIG_STR_VALUE "http://localhost/autop.cfg"
TEST(MockConfigParser, GetConfigString)
{
    MockConfigParser objMock;
    g_pMockConfigParser = &objMock;

    ON_CALL(objMock, GetConfigString(TEST_CONFIG_STR_KEY,
                                     CONFIG_LEVEL_IGNORE)).WillByDefault(testing::Return(TEST_CONFIG_STR_VALUE));
    EXPECT_CALL(objMock, GetConfigString(TEST_CONFIG_STR_KEY, CONFIG_LEVEL_IGNORE)).Times(1);
    EXPECT_STREQ(configParser_GetConfigString(TEST_CONFIG_STR_KEY).c_str(), TEST_CONFIG_STR_VALUE);

    g_pMockConfigParser = NULL;
}

TEST(MockConfigParser, SetConfigString)
{
    MockConfigParser objMock;
    g_pMockConfigParser = &objMock;

    ON_CALL(objMock, SetConfigString(TEST_CONFIG_STR_KEY, TEST_CONFIG_STR_VALUE,
                                     CONFIG_LEVEL_IGNORE)).WillByDefault(testing::Return(true));
    EXPECT_CALL(objMock, SetConfigString(TEST_CONFIG_STR_KEY, TEST_CONFIG_STR_VALUE,
                                         CONFIG_LEVEL_IGNORE)).Times(1);
    EXPECT_TRUE(configParser_SetConfigString(TEST_CONFIG_STR_KEY, TEST_CONFIG_STR_VALUE));

    g_pMockConfigParser = NULL;
}

#define TEST_CFG_ITEM_INT_KEY kszAccountEnable
TEST(MockConfigParser, GetCfgItemIntValue)
{
    MockConfigParser objMock;
    g_pMockConfigParser = &objMock;

    ON_CALL(objMock, GetCfgItemIntValue(TEST_CFG_ITEM_INT_KEY, 0,
                                        CONFIG_LEVEL_IGNORE)).WillByDefault(testing::Return(1));
    EXPECT_CALL(objMock, GetCfgItemIntValue(TEST_CFG_ITEM_INT_KEY, 0, CONFIG_LEVEL_IGNORE)).Times(1);
    EXPECT_EQ(configParser_GetCfgItemIntValue(TEST_CFG_ITEM_INT_KEY, 0), 1);

    g_pMockConfigParser = NULL;
}

TEST(MockConfigParser, SetCfgItemIntValue)
{
    MockConfigParser objMock;
    g_pMockConfigParser = &objMock;

    ON_CALL(objMock, SetCfgItemIntValue(TEST_CFG_ITEM_INT_KEY, 0, 1,
                                        CONFIG_LEVEL_IGNORE)).WillByDefault(testing::Return(true));
    EXPECT_CALL(objMock, SetCfgItemIntValue(TEST_CFG_ITEM_INT_KEY, 0, 1, CONFIG_LEVEL_IGNORE)).Times(1);
    EXPECT_TRUE(configParser_SetCfgItemIntValue(TEST_CFG_ITEM_INT_KEY, 0, 1));

    g_pMockConfigParser = NULL;
}

#define TEST_CFG_ITEM_STR_KEY kszAccountLabel
#define TEST_CFG_ITEM_STR_VALUE "account name"
TEST(MockConfigParser, GetCfgItemStringValue)
{
    MockConfigParser objMock;
    g_pMockConfigParser = &objMock;

    ON_CALL(objMock, GetCfgItemStringValue(TEST_CFG_ITEM_STR_KEY, 1,
                                           CONFIG_LEVEL_IGNORE)).WillByDefault(testing::Return(TEST_CFG_ITEM_STR_VALUE));
    EXPECT_CALL(objMock, GetCfgItemStringValue(TEST_CFG_ITEM_STR_KEY, 1, CONFIG_LEVEL_IGNORE)).Times(1);
    EXPECT_STREQ(configParser_GetCfgItemStringValue(TEST_CFG_ITEM_STR_KEY, 1).c_str(),
                 TEST_CFG_ITEM_STR_VALUE);

    g_pMockConfigParser = NULL;
}

TEST(MockConfigParser, SetCfgItemStringValue)
{
    MockConfigParser objMock;
    g_pMockConfigParser = &objMock;

    ON_CALL(
        objMock,
        SetCfgItemStringValue(TEST_CFG_ITEM_STR_KEY, 0, testing::StrEq(TEST_CFG_ITEM_STR_VALUE),
                              CONFIG_LEVEL_IGNORE)
    ).WillByDefault(testing::Return(true));
    EXPECT_CALL(
        objMock,
        SetCfgItemStringValue(TEST_CFG_ITEM_STR_KEY, 0, testing::StrEq(TEST_CFG_ITEM_STR_VALUE),
                              CONFIG_LEVEL_IGNORE)
    ).Times(1);
    EXPECT_TRUE(configParser_SetCfgItemStringValue(TEST_CFG_ITEM_STR_KEY, 0, TEST_CFG_ITEM_STR_VALUE));

    g_pMockConfigParser = NULL;
}
