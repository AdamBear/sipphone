#include <gtest/gtest.h>

#include "dndmanager.h"
#include "fwddndmsgdefine.h"

#include "mockaccount.h"
#include "mocklibs/configparser/mockconfigparser.h"
#include "mocklibs/etlmsghandler/mocketlmsghandler.h"
#include <service_config.h> // CONFIG_MSG_BCAST_CHANGED


class DndManagerTest : public testing::Test
{
protected:
    MockAccount m_objMockAccount;
    MockConfigParser m_objMockConfigParser;
    MockEtlMsgHandler m_objMockEtlMsgHandler;

    virtual void SetUp()
    {
        g_pMockAccount = &m_objMockAccount;

        g_pMockConfigParser = &m_objMockConfigParser;
        // 默认启用DND功能
        ON_CALL(m_objMockConfigParser, GetConfigInt(testing::StrEq(kszDndForbidden), testing::_))
        .WillByDefault(testing::Return(0));
        EXPECT_CALL(m_objMockConfigParser, GetConfigInt(testing::StrEq(kszDndForbidden), testing::_))
        .Times(testing::AtLeast(1));

        g_pMockEtlMsgHandler = &m_objMockEtlMsgHandler;
        EXPECT_CALL(
            m_objMockEtlMsgHandler,
            RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, testing::_)
        ).Times(testing::AtLeast(0)).WillRepeatedly(testing::Return(TRUE));
        EXPECT_CALL(
            m_objMockEtlMsgHandler,
            UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, testing::_)
        ).Times(testing::AtLeast(0)).WillRepeatedly(testing::Return(TRUE));
    }

    virtual void TearDown()
    {
        g_pMockAccount = NULL;
        g_pMockConfigParser = NULL;
        g_pMockEtlMsgHandler = NULL;
    }
};

// DND初始化：DND禁用
TEST_F(DndManagerTest, Inactive)
{
    // 禁用DND功能
    ON_CALL(m_objMockConfigParser, GetConfigInt(testing::StrEq(kszDndForbidden), testing::_))
    .WillByDefault(testing::Return(1));

    _DndManager.Init();
    EXPECT_FALSE(_DndManager.IsActive());
    EXPECT_FALSE(_DndManager.IsEnable(FWDDND_ACCOUNT_GLOBAL));
    EXPECT_FALSE(_DndManager.IsEnable(0));
    EXPECT_FALSE(_DndManager.SetEnable(0, false, DND_CFG_SRC_TYPE_COMMON));
    _DndManager.Finalize();
}

// DND初始化：全局DND=Off
TEST_F(DndManagerTest, GlobalDisabled)
{
    // 全局模式
    EXPECT_CALL_GetConfigInt(kszDndMode, FWDDND_MODE_GLOBAL);
    // 全局DND状态：Off
    EXPECT_CALL_GetConfigInt(kszDndStatus, 0);

    _DndManager.Init();
    EXPECT_TRUE(_DndManager.IsActive());
    EXPECT_EQ(_DndManager.GetMode(), FWDDND_MODE_GLOBAL);
    EXPECT_FALSE(_DndManager.IsEnable(FWDDND_ACCOUNT_GLOBAL));
    EXPECT_FALSE(_DndManager.IsEnable(0)); // 任意账号视全局DND状态而定
    _DndManager.Finalize();
}

// DND初始化：账号DND=On&Off（各账号不同）
TEST_F(DndManagerTest, AccountDisabledAndEnabled)
{
    // 账号模式
    EXPECT_CALL_GetConfigInt(kszDndMode, FWDDND_MODE_ACCOUNT);
    // 账号0：DND=Off
    EXPECT_CALL_GetCfgItemIntValue(kszAccountDNDStatus, 0, 0);
    // 账号1：DND=On
    EXPECT_CALL_GetCfgItemIntValue(kszAccountDNDStatus, 1, 1);
    // 账号2：DND=Off
    EXPECT_CALL_GetCfgItemIntValue(kszAccountDNDStatus, 2, 0);
    // 账号列表
    ListAccounts l;
    for (size_t i = 0; i < 3; ++i)
    {
        l.push_back(i);
    }
    ON_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).Times(1);
    // 通知状态改变为On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            1, DND_CFG_SRC_TYPE_STARTUP, true);

    _DndManager.Init();
    EXPECT_TRUE(_DndManager.IsActive());
    EXPECT_EQ(_DndManager.GetMode(), FWDDND_MODE_ACCOUNT);
    EXPECT_FALSE(_DndManager.IsEnable(FWDDND_ACCOUNT_GLOBAL)); // 账号无效，视为Off
    EXPECT_FALSE(_DndManager.IsEnable(0));
    EXPECT_TRUE(_DndManager.IsEnable(1));
    EXPECT_FALSE(_DndManager.IsEnable(2));
    EXPECT_FALSE(_DndManager.IsEnable(3)); // 账号无效，视为Off

    // 状态改变消息：账号1=Off
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            1, DND_CFG_SRC_TYPE_SHUTDOWN, false);

    _DndManager.Finalize();
}

// DND功能开关配置变更：关->开（全局=On）
TEST_F(DndManagerTest, ActivateOfGlobalMode)
{
    // 禁用DND功能
    ON_CALL(m_objMockConfigParser, GetConfigInt(testing::StrEq(kszDndForbidden), testing::_))
    .WillByDefault(testing::Return(1));

    _DndManager.Init();
    EXPECT_FALSE(_DndManager.IsActive());
    EXPECT_FALSE(_DndManager.IsEnable(FWDDND_ACCOUNT_GLOBAL));

    // 启用DND功能
    ON_CALL(m_objMockConfigParser, GetConfigInt(testing::StrEq(kszDndForbidden), testing::_))
    .WillByDefault(testing::Return(0));
    // 全局模式
    EXPECT_CALL_GetConfigInt(kszDndMode, FWDDND_MODE_GLOBAL);
    // 全局DND状态：On
    EXPECT_CALL_GetConfigInt(kszDndStatus, 1);
    // 通知状态改变为On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_STARTUP, true);

    // 配置变更消息CONFIG_MSG_BCAST_CHANGED(wParam=ST_ADV_SETTING)，触发重新加载
    msgObject msg;
    msg.message = CONFIG_MSG_BCAST_CHANGED;
    msg.wParam = ST_ADV_SETTING;
    TriggerEtlMsgCallback(msg);
    EXPECT_TRUE(_DndManager.IsActive());
    EXPECT_EQ(_DndManager.GetMode(), FWDDND_MODE_GLOBAL);
    EXPECT_TRUE(_DndManager.IsEnable(FWDDND_ACCOUNT_GLOBAL));
    EXPECT_TRUE(_DndManager.IsEnable(0)); // 任意账号视全局DND状态而定

    // 状态改变消息
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_SHUTDOWN, false);

    _DndManager.Finalize();
}

// DND功能开关配置变更：开（账号）->关
TEST_F(DndManagerTest, DeactivateOfAccountMode)
{
    // 账号模式
    EXPECT_CALL_GetConfigInt(kszDndMode, FWDDND_MODE_ACCOUNT);
    // 账号0：DND=On
    EXPECT_CALL_GetCfgItemIntValue(kszAccountDNDStatus, 0, 1);
    // 账号列表
    ListAccounts l;
    l.push_back(0);
    ON_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).Times(1);
    // 通知状态改变为On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            0, DND_CFG_SRC_TYPE_STARTUP, true);

    _DndManager.Init();
    EXPECT_TRUE(_DndManager.IsActive());
    EXPECT_EQ(_DndManager.GetMode(), FWDDND_MODE_ACCOUNT);
    EXPECT_TRUE(_DndManager.IsEnable(0));

    // 禁用DND功能
    ON_CALL(m_objMockConfigParser, GetConfigInt(testing::StrEq(kszDndForbidden), testing::_))
    .WillByDefault(testing::Return(1));
    // 状态改变消息
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            0, DND_CFG_SRC_TYPE_SHUTDOWN, false);

    // 配置变更消息CONFIG_MSG_BCAST_CHANGED(wParam=ST_ADV_SETTING)，触发重新加载
    msgObject msg;
    msg.message = CONFIG_MSG_BCAST_CHANGED;
    msg.wParam = ST_ADV_SETTING;
    TriggerEtlMsgCallback(msg);
    EXPECT_FALSE(_DndManager.IsActive());
    EXPECT_FALSE(_DndManager.IsEnable(0));

    _DndManager.Finalize();
}

// 模式配置变更：全局->账号
TEST_F(DndManagerTest, ModeConfigGlobalToAccount)
{
    // 全局模式
    EXPECT_CALL_GetConfigInt(kszDndMode, FWDDND_MODE_GLOBAL);
    // 全局DND状态：On
    EXPECT_CALL_GetConfigInt(kszDndStatus, 1);
    // 通知状态改变为On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_STARTUP, true);

    _DndManager.Init();
    EXPECT_TRUE(_DndManager.IsActive());
    EXPECT_EQ(_DndManager.GetMode(), FWDDND_MODE_GLOBAL);
    EXPECT_TRUE(_DndManager.IsEnable(FWDDND_ACCOUNT_GLOBAL));

    // 配置变为账号模式
    EXPECT_CALL_GetConfigInt(kszDndMode, FWDDND_MODE_ACCOUNT);
    // 账号0：DND=On
    EXPECT_CALL_GetCfgItemIntValue(kszAccountDNDStatus, 0, 1);
    // 账号列表
    ListAccounts l;
    l.push_back(0);
    ON_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).Times(1);
    // 状态改变消息：全局状态On->Off
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_SHUTDOWN, false);
    // 通知状态改变为On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            0, DND_CFG_SRC_TYPE_STARTUP, true);

    // 配置变更消息CONFIG_MSG_BCAST_CHANGED(wParam=ST_DND)，触发重新加载
    msgObject msg;
    msg.message = CONFIG_MSG_BCAST_CHANGED;
    msg.wParam = ST_DND;
    TriggerEtlMsgCallback(msg);
    EXPECT_EQ(_DndManager.GetMode(), FWDDND_MODE_ACCOUNT);
    EXPECT_TRUE(_DndManager.IsEnable(0));

    // 状态改变消息
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            0, DND_CFG_SRC_TYPE_SHUTDOWN, false);

    _DndManager.Finalize();
}

// 模式配置变更：账号->全局
TEST_F(DndManagerTest, ModeConfigAccountToGlobal)
{
    // 配置为账号模式
    EXPECT_CALL_GetConfigInt(kszDndMode, FWDDND_MODE_ACCOUNT);
    // 账号0：DND=Off
    EXPECT_CALL_GetCfgItemIntValue(kszAccountDNDStatus, 0, 0);
    // 账号列表
    ListAccounts l;
    l.push_back(0);
    ON_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).Times(1);

    _DndManager.Init();
    EXPECT_TRUE(_DndManager.IsActive());
    EXPECT_EQ(_DndManager.GetMode(), FWDDND_MODE_ACCOUNT);
    EXPECT_FALSE(_DndManager.IsEnable(0));

    // 配置变为全局模式
    EXPECT_CALL_GetConfigInt(kszDndMode, FWDDND_MODE_GLOBAL);
    // 全局DND状态：On
    EXPECT_CALL_GetConfigInt(kszDndStatus, 1);
    // 通知状态改变为On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_STARTUP, true);

    // 配置变更消息CONFIG_MSG_BCAST_CHANGED(wParam=ST_DND)，触发重新加载
    msgObject msg;
    msg.message = CONFIG_MSG_BCAST_CHANGED;
    msg.wParam = ST_DND;
    TriggerEtlMsgCallback(msg);
    EXPECT_EQ(_DndManager.GetMode(), FWDDND_MODE_GLOBAL);
    EXPECT_TRUE(_DndManager.IsEnable(FWDDND_ACCOUNT_GLOBAL));

    // 状态改变消息
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_SHUTDOWN, false);

    _DndManager.Finalize();
}

// 模式切换：全局->账号
TEST_F(DndManagerTest, SwitchModeGlobalToAccount)
{
    // 全局模式
    EXPECT_CALL_GetConfigInt(kszDndMode, FWDDND_MODE_GLOBAL);
    // 全局DND状态：On
    EXPECT_CALL_GetConfigInt(kszDndStatus, 1);
    // 状态改变消息：全局状态Off->On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_STARTUP, true);

    _DndManager.Init();
    EXPECT_TRUE(_DndManager.IsActive());
    EXPECT_EQ(_DndManager.GetMode(), FWDDND_MODE_GLOBAL);
    EXPECT_TRUE(_DndManager.IsEnable(FWDDND_ACCOUNT_GLOBAL));

    // 配置变为账号模式
    EXPECT_CALL_SetConfigInt(kszDndMode, FWDDND_MODE_ACCOUNT);
    EXPECT_CALL_GetConfigInt(kszDndMode, FWDDND_MODE_ACCOUNT);
    // 账号0：DND=On
    EXPECT_CALL_GetCfgItemIntValue(kszAccountDNDStatus, 0, 1);
    // 账号列表
    ListAccounts l;
    l.push_back(0);
    ON_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).Times(1);
    // 状态改变消息：全局状态On->Off
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_SHUTDOWN, false);
    // 状态改变消息：账号#0状态Off->On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            0, DND_CFG_SRC_TYPE_STARTUP, true);

    // 设置模式为账号模式
    _DndManager.SetMode(FWDDND_MODE_ACCOUNT);
    EXPECT_EQ(_DndManager.GetMode(), FWDDND_MODE_ACCOUNT);
    EXPECT_TRUE(_DndManager.IsEnable(0));

    // 状态改变消息：账号#0状态On->Off
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            0, DND_CFG_SRC_TYPE_SHUTDOWN, false);

    _DndManager.Finalize();
}

// 模式切换：账号->全局
TEST_F(DndManagerTest, SwitchModeAccountToGlobal)
{
    // 配置为账号模式
    EXPECT_CALL_GetConfigInt(kszDndMode, FWDDND_MODE_ACCOUNT);
    // 账号0：DND=Off
    EXPECT_CALL_GetCfgItemIntValue(kszAccountDNDStatus, 0, 0);
    // 账号列表
    ListAccounts l;
    l.push_back(0);
    ON_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).Times(1);

    _DndManager.Init();
    EXPECT_TRUE(_DndManager.IsActive());
    EXPECT_EQ(_DndManager.GetMode(), FWDDND_MODE_ACCOUNT);
    EXPECT_FALSE(_DndManager.IsEnable(0));

    // 配置变为全局模式
    EXPECT_CALL_SetConfigInt(kszDndMode, FWDDND_MODE_GLOBAL);
    EXPECT_CALL_GetConfigInt(kszDndMode, FWDDND_MODE_GLOBAL);
    // 全局DND状态：On
    EXPECT_CALL_GetConfigInt(kszDndStatus, 1);
    // 状态改变消息：全局状态Off->On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_STARTUP, true);

    // 设置模式为全局模式
    _DndManager.SetMode(FWDDND_MODE_GLOBAL);
    EXPECT_EQ(_DndManager.GetMode(), FWDDND_MODE_GLOBAL);
    EXPECT_TRUE(_DndManager.IsEnable(FWDDND_ACCOUNT_GLOBAL));

    // 状态改变消息：全局账号状态On->Off
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_SHUTDOWN, false);

    _DndManager.Finalize();
}

// 设置全局DND状态
TEST_F(DndManagerTest, SetGlobalStatus)
{
    // 全局模式
    EXPECT_CALL_GetConfigInt(kszDndMode, FWDDND_MODE_GLOBAL);
    // 全局DND状态：On
    EXPECT_CALL_GetConfigInt(kszDndStatus, 1);
    // 状态改变消息：全局状态Off->On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_STARTUP, true);

    _DndManager.Init();
    EXPECT_TRUE(_DndManager.IsActive());
    EXPECT_EQ(_DndManager.GetMode(), FWDDND_MODE_GLOBAL);
    EXPECT_TRUE(_DndManager.IsEnable(FWDDND_ACCOUNT_GLOBAL));

    // 全局DND状态配置变为Off
    EXPECT_CALL_SetConfigInt(kszDndStatus, 0);
    // 状态改变消息：全局状态On->Off
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_COMMON, false);

    // 设置全局DND状态为Off
    _DndManager.SetEnable(FWDDND_ACCOUNT_GLOBAL, false, DND_CFG_SRC_TYPE_COMMON);
    EXPECT_FALSE(_DndManager.IsEnable(FWDDND_ACCOUNT_GLOBAL));

    // 全局DND状态配置变为On
    EXPECT_CALL_SetConfigInt(kszDndStatus, 1);
    // 状态改变消息：全局状态On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_COMMON, true);

    // 指定账号设置全局DND状态为On
    _DndManager.SetEnable(0, true, DND_CFG_SRC_TYPE_COMMON);
    EXPECT_TRUE(_DndManager.IsEnable(FWDDND_ACCOUNT_GLOBAL));
    EXPECT_TRUE(_DndManager.IsEnable(0));

    // 设置相同DND状态（On），将不会写配置，也不会发消息
    _DndManager.SetEnable(1, true, DND_CFG_SRC_TYPE_COMMON);
    EXPECT_TRUE(_DndManager.IsEnable(1));
    _DndManager.SetEnable(FWDDND_ACCOUNT_GLOBAL, true, DND_CFG_SRC_TYPE_COMMON);

    // 状态改变消息：释放时全局DND状态Off
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_SHUTDOWN, false);

    _DndManager.Finalize();
}

// 设置账号DND状态
TEST_F(DndManagerTest, SetAccountStatus)
{
    const int iAccountCount = 5;

    // 配置为账号模式
    EXPECT_CALL_GetConfigInt(kszDndMode, FWDDND_MODE_ACCOUNT);
    // 账号列表
    ListAccounts l;
    for (size_t i = 0; i < iAccountCount; ++i)
    {
        l.push_back(i);
        // 每个账号DND状态均为Off
        EXPECT_CALL_GetCfgItemIntValue(kszAccountDNDStatus, i, 0);
    }
    ON_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).Times(1);

    _DndManager.Init();
    EXPECT_TRUE(_DndManager.IsActive());
    EXPECT_EQ(_DndManager.GetMode(), FWDDND_MODE_ACCOUNT);
    for (size_t i = 0; i < iAccountCount; ++i)
    {
        EXPECT_FALSE(_DndManager.IsEnable(i));
    }

    int iAccountId = 1;
    // 指定账号DND状态配置改变为On
    EXPECT_CALL_SetCfgItemIntValue(kszAccountDNDStatus, iAccountId, 1);
    // 状态改变消息：指定账号状态On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            iAccountId, DND_CFG_SRC_TYPE_COMMON, true);

    // 设置指定账号DND状态为On
    _DndManager.SetEnable(iAccountId, true, DND_CFG_SRC_TYPE_COMMON);
    for (size_t i = 0; i < iAccountCount; ++i)
    {
        if (i == iAccountId)
        {
            EXPECT_TRUE(_DndManager.IsEnable(i));
        }
        else
        {
            EXPECT_FALSE(_DndManager.IsEnable(i));
        }
    }

    // 设置无效账号DND状态为On，不会有任何变化
    _DndManager.SetEnable(iAccountCount, true, DND_CFG_SRC_TYPE_COMMON);
    _DndManager.SetEnable(-1, true, DND_CFG_SRC_TYPE_COMMON);
    for (size_t i = 0; i < iAccountCount; ++i)
    {
        if (i == iAccountId)
        {
            EXPECT_TRUE(_DndManager.IsEnable(i));
        }
        else
        {
            EXPECT_FALSE(_DndManager.IsEnable(i));
        }
    }

    // 指定账号DND状态配置改变为On
    EXPECT_CALL_SetCfgItemIntValue(kszAccountDNDStatus, iAccountId, 0);
    // 状态改变消息：指定账号状态Off
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            iAccountId, DND_CFG_SRC_TYPE_COMMON, false);

    // 设置指定账号DND状态为On
    _DndManager.SetEnable(iAccountId, false, DND_CFG_SRC_TYPE_COMMON);
    for (size_t i = 0; i < iAccountCount; ++i)
    {
        EXPECT_FALSE(_DndManager.IsEnable(i));
    }

    _DndManager.Finalize();
}
