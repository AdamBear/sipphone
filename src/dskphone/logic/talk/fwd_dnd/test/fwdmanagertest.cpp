#include "fwdentitytest.h"

#include "fwdmanager.h"
#include "fwddndmsgdefine.h"

#include "mockaccount.h"
#include "mocklibs/configparser/mockconfigparser.h"
#include "mocklibs/etlmsghandler/mocketlmsghandler.h"
#include <service_config.h> // CONFIG_MSG_BCAST_CHANGED


class FwdManagerTest : public FwdEntityTest
{
protected:
    MockAccount m_objMockAccount;

    yl::string strTarget;
    int iRingDelay;

    virtual void SetUp()
    {
        FwdEntityTest::SetUp();

        g_pMockAccount = &m_objMockAccount;

        // 默认启用FWD功能
        ON_CALL(m_objMockConfigParser, GetConfigInt(testing::StrEq(kszForwardForbidden), testing::_))
        .WillByDefault(testing::Return(0));
        EXPECT_CALL(m_objMockConfigParser, GetConfigInt(testing::StrEq(kszForwardForbidden), testing::_))
        .Times(testing::AtLeast(1));

        // 注册/注销消息处理函数
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
        FwdEntityTest::TearDown();
        g_pMockAccount = NULL;
    }

    void EXPECT_CALL_GetConfigOfFoward(int iAccount, bool bAlways, bool bBusy, bool bNoAnswer)
    {
        if (iAccount == FWDDND_ACCOUNT_GLOBAL)
        {
            EXPECT_CALL_GetConfigOfGlobalAlwaysFoward(bAlways);
            EXPECT_CALL_GetConfigOfGlobalBusyFoward(bBusy);
            EXPECT_CALL_GetConfigOfGlobalNoAnswerFoward(bNoAnswer);
        }
        else
        {
            EXPECT_CALL_GetConfigOfAccountAlwaysFoward(bAlways, iAccount);
            EXPECT_CALL_GetConfigOfAccountBusyFoward(bBusy, iAccount);
            EXPECT_CALL_GetConfigOfAccountNoAnswerFoward(bNoAnswer, iAccount);
        }
    }

    void EXPECT_CALL_NotifyAppExOfForward(int iAccount,
                                          bool bAlways, bool bAlwaysNotify,
                                          bool bBusy, bool bBusyNotify,
                                          bool bNoAnswer, bool bNoAnswerNotify,
                                          FWD_CONFIG_SOURCE_TYPE eType = FWD_CFG_SRC_TYPE_COMMON)
    {
        if (bAlwaysNotify)
        {
            FwdMessageStatusData & refData = (iAccount == FWDDND_ACCOUNT_GLOBAL ?
                                              s_objGlobalAlwaysForwardData : s_objAccountAlwaysForwardData);
            refData.bEnabled = bAlways;
            EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                    iAccount, eType, refData);
        }
        if (bBusyNotify)
        {
            FwdMessageStatusData & refData = (iAccount == FWDDND_ACCOUNT_GLOBAL ?
                                              s_objGlobalBusyForwardData : s_objAccountBusyForwardData);
            refData.bEnabled = bBusy;
            EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                    iAccount, eType, refData);
        }
        if (bNoAnswerNotify)
        {
            FwdMessageStatusData & refData = (iAccount == FWDDND_ACCOUNT_GLOBAL ?
                                              s_objGlobalNoAnswerForwardData : s_objAccountNoAnswerForwardData);
            refData.bEnabled = bNoAnswer;
            EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                    iAccount, eType, refData);
        }
    }

    void EXPECT_ForwardInfoEq(int iAccount, bool bAlways, bool bBusy, bool bNoAnswer)
    {
        EXPECT_EQ(_FwdManager.GetInfo(iAccount, FWD_TYPE_ALWAYS,
                                      &strTarget), bAlways);
        if (bAlways)
        {
            if (iAccount == FWDDND_ACCOUNT_GLOBAL)
            {
                EXPECT_STREQ(strTarget.c_str(), s_objGlobalAlwaysForwardData.szTarget);
            }
            else
            {
                EXPECT_STREQ(strTarget.c_str(), s_objAccountAlwaysForwardData.szTarget);
            }
        }
        EXPECT_EQ(_FwdManager.GetInfo(iAccount, FWD_TYPE_BUSY, &strTarget), bBusy);
        if (bBusy)
        {
            if (iAccount == FWDDND_ACCOUNT_GLOBAL)
            {
                EXPECT_STREQ(strTarget.c_str(), s_objGlobalBusyForwardData.szTarget);
            }
            else
            {
                EXPECT_STREQ(strTarget.c_str(), s_objAccountBusyForwardData.szTarget);
            }
        }
        EXPECT_EQ(_FwdManager.GetInfo(iAccount, FWD_TYPE_NO_ANSWER,
                                      &strTarget, &iRingDelay), bNoAnswer);
        if (bNoAnswer)
        {
            if (iAccount == FWDDND_ACCOUNT_GLOBAL)
            {
                EXPECT_STREQ(strTarget.c_str(), s_objGlobalNoAnswerForwardData.szTarget);
                EXPECT_EQ(iRingDelay, s_objGlobalNoAnswerForwardData.iRingDelay);
            }
            else
            {
                EXPECT_STREQ(strTarget.c_str(), s_objAccountNoAnswerForwardData.szTarget);
                EXPECT_EQ(iRingDelay, s_objAccountNoAnswerForwardData.iRingDelay);
            }
        }
    }
};

// FWD初始化：FWD禁用
TEST_F(FwdManagerTest, Inactive)
{
    // 禁用FWD功能
    ON_CALL(m_objMockConfigParser, GetConfigInt(testing::StrEq(kszForwardForbidden), testing::_))
    .WillByDefault(testing::Return(1));

    _FwdManager.Init();
    EXPECT_FALSE(_FwdManager.IsActive());
    EXPECT_FALSE(_FwdManager.GetInfo(FWDDND_ACCOUNT_GLOBAL, FWD_TYPE_ALWAYS));
    EXPECT_FALSE(_FwdManager.GetInfo(0, FWD_TYPE_ALWAYS));
    EXPECT_FALSE(_FwdManager.SetInfo(0, FWD_TYPE_BUSY, false,
                                     &strTarget, &iRingDelay, FWD_CFG_SRC_TYPE_COMMON));
    _FwdManager.Finalize();
}

// 初始化：全局FWD
TEST_F(FwdManagerTest, GlobalDisabled)
{
    // 全局模式
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_GLOBAL);

    // 全局FWD状态：Off
    EXPECT_CALL_GetConfigOfFoward(FWDDND_ACCOUNT_GLOBAL, false, false, false);

    _FwdManager.Init();
    EXPECT_TRUE(_FwdManager.IsActive());
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_GLOBAL);
    EXPECT_ForwardInfoEq(FWDDND_ACCOUNT_GLOBAL, false, false, false);
    EXPECT_ForwardInfoEq(5, false, false, false); // 任意账号视全局FWD状态而定
    _FwdManager.Finalize();

    // 全局FWD状态：On
    EXPECT_CALL_GetConfigOfFoward(FWDDND_ACCOUNT_GLOBAL, true, true, true);
    // 通知状态改变为On
    EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL,
                                     true, true, true, true, true, true, FWD_CFG_SRC_TYPE_STARTUP);

    _FwdManager.Init();
    EXPECT_TRUE(_FwdManager.IsActive());
    EXPECT_ForwardInfoEq(FWDDND_ACCOUNT_GLOBAL, true, true, true);
    // 任意账号视全局FWD状态而定
    EXPECT_TRUE(_FwdManager.GetInfo(0, FWD_TYPE_ALWAYS,
                                    &strTarget));
    EXPECT_STREQ(strTarget.c_str(), s_objGlobalAlwaysForwardData.szTarget);
    EXPECT_TRUE(_FwdManager.GetInfo(1, FWD_TYPE_BUSY, &strTarget));
    EXPECT_STREQ(strTarget.c_str(), s_objGlobalBusyForwardData.szTarget);
    EXPECT_TRUE(_FwdManager.GetInfo(2, FWD_TYPE_NO_ANSWER, &strTarget, &iRingDelay));
    EXPECT_STREQ(strTarget.c_str(), s_objGlobalNoAnswerForwardData.szTarget);
    EXPECT_EQ(iRingDelay, s_objGlobalNoAnswerForwardData.iRingDelay);

    // 释放前会通知状态改变为Off
    EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL,
                                     false, true, false, true, false, true, FWD_CFG_SRC_TYPE_SHUTDOWN);
    _FwdManager.Finalize();
}

// FWD初始化：账号FWD=On&Off（各账号不同）
TEST_F(FwdManagerTest, AccountDisabledAndEnabled)
{
    // 账号模式
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_ACCOUNT);

    // 账号列表
    ListAccounts l;
    for (size_t i = 0; i < 3; ++i)
    {
        l.push_back(i);
    }
    ON_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).Times(1);

    // 各账号FWD配置不同
    EXPECT_CALL_GetConfigOfFoward(0, true, false, false);
    EXPECT_CALL_NotifyAppExOfForward(0, true, true, false, false, false, false,
                                     FWD_CFG_SRC_TYPE_STARTUP);
    EXPECT_CALL_GetConfigOfFoward(1, false, true, false);
    EXPECT_CALL_NotifyAppExOfForward(1, false, false, true, true, false, false,
                                     FWD_CFG_SRC_TYPE_STARTUP);
    EXPECT_CALL_GetConfigOfFoward(2, false, false, true);
    EXPECT_CALL_NotifyAppExOfForward(2, false, false, false, false, true, true,
                                     FWD_CFG_SRC_TYPE_STARTUP);

    _FwdManager.Init();
    EXPECT_TRUE(_FwdManager.IsActive());
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_ACCOUNT);
    EXPECT_ForwardInfoEq(FWDDND_ACCOUNT_GLOBAL, false, false, false); // 账号无效，视为Off
    EXPECT_ForwardInfoEq(0, true, false, false);
    EXPECT_ForwardInfoEq(1, false, true, false);
    EXPECT_ForwardInfoEq(2, false, false, true);
    EXPECT_ForwardInfoEq(3, false, false, false); // 账号无效，视为Off

    // 释放时发送状态改变消息
    EXPECT_CALL_NotifyAppExOfForward(0, false, true, false, false, false, false,
                                     FWD_CFG_SRC_TYPE_SHUTDOWN);
    EXPECT_CALL_NotifyAppExOfForward(1, false, false, false, true, false, false,
                                     FWD_CFG_SRC_TYPE_SHUTDOWN);
    EXPECT_CALL_NotifyAppExOfForward(2, false, false, false, false, false, true,
                                     FWD_CFG_SRC_TYPE_SHUTDOWN);

    _FwdManager.Finalize();
}

// FWD功能开关配置变更：关->开（全局）
TEST_F(FwdManagerTest, ActivateOfGlobalMode)
{
    // 配置为禁用FWD功能
    ON_CALL(m_objMockConfigParser, GetConfigInt(testing::StrEq(kszForwardForbidden), testing::_))
    .WillByDefault(testing::Return(1));
    // 初始化
    _FwdManager.Init();
    // 启用FWD功能
    ON_CALL(m_objMockConfigParser, GetConfigInt(testing::StrEq(kszForwardForbidden), testing::_))
    .WillByDefault(testing::Return(0));
    // 全局模式
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_GLOBAL);
    // 全局FWD状态
    EXPECT_CALL_GetConfigOfFoward(FWDDND_ACCOUNT_GLOBAL, true, false, true);
    // 通知状态改变
    EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL,
                                     true, true, false, false, true, true, FWD_CFG_SRC_TYPE_STARTUP);
    // 配置变更消息CONFIG_MSG_BCAST_CHANGED(wParam=ST_ADV_SETTING)，触发重新加载
    msgObject msg;
    msg.message = CONFIG_MSG_BCAST_CHANGED;
    msg.wParam = ST_ADV_SETTING;
    TriggerEtlMsgCallback(msg);
    // 校验当前状态
    EXPECT_TRUE(_FwdManager.IsActive());
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_GLOBAL);
    EXPECT_ForwardInfoEq(FWDDND_ACCOUNT_GLOBAL, true, false, true);
    // 释放前会通知状态改变为Off
    EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL,
                                     false, true, false, false, false, true, FWD_CFG_SRC_TYPE_SHUTDOWN);
    _FwdManager.Finalize();
}

// FWD功能开关配置变更：开（账号）->关
TEST_F(FwdManagerTest, DeactivateOfAccountMode)
{
    // 账号模式
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_ACCOUNT);
    // 账号列表
    ListAccounts l;
    l.push_back(1);
    ON_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).Times(1);
    // 账号FWD配置
    EXPECT_CALL_GetConfigOfFoward(1, false, true, true);
    EXPECT_CALL_NotifyAppExOfForward(1, false, false, true, true, true, true,
                                     FWD_CFG_SRC_TYPE_STARTUP);
    // 初始化
    _FwdManager.Init();
    // 校验账号模式信息
    EXPECT_TRUE(_FwdManager.IsActive());
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_ACCOUNT);
    EXPECT_ForwardInfoEq(1, false, true, true);
    // 配置为禁用FWD功能
    ON_CALL(m_objMockConfigParser, GetConfigInt(testing::StrEq(kszForwardForbidden), testing::_))
    .WillByDefault(testing::Return(1));
    // 禁用FWD功能时发送状态改变消息
    EXPECT_CALL_NotifyAppExOfForward(1, false, false, false, true, false, true,
                                     FWD_CFG_SRC_TYPE_SHUTDOWN);
    // 配置变更消息CONFIG_MSG_BCAST_CHANGED(wParam=ST_ADV_SETTING)，触发重新加载
    msgObject msg;
    msg.message = CONFIG_MSG_BCAST_CHANGED;
    msg.wParam = ST_ADV_SETTING;
    TriggerEtlMsgCallback(msg);
    // 校验FWD功能是否被禁用
    EXPECT_FALSE(_FwdManager.IsActive());
    EXPECT_ForwardInfoEq(1, false, false, false);
    _FwdManager.Finalize();
}

// 模式配置变更：全局->账号
TEST_F(FwdManagerTest, ModeConfigGlobalToAccount)
{
    // 全局模式
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_GLOBAL);
    // 全局FWD状态
    EXPECT_CALL_GetConfigOfFoward(FWDDND_ACCOUNT_GLOBAL, true, true, false);
    // 通知状态改变
    EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL,
                                     true, true, true, true, false, false, FWD_CFG_SRC_TYPE_STARTUP);
    // 初始化
    _FwdManager.Init();
    // 校验全局模式信息
    EXPECT_TRUE(_FwdManager.IsActive());
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_GLOBAL);
    EXPECT_ForwardInfoEq(FWDDND_ACCOUNT_GLOBAL, true, true, false);
    // 配置变为账号模式
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_ACCOUNT);
    // 账号列表
    const int iAccountId = 2;
    ListAccounts l;
    l.push_back(iAccountId);
    ON_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).Times(1);
    // 账号FWD配置
    EXPECT_CALL_GetConfigOfFoward(iAccountId, false, true, true);
    // 账号FWD状态改变
    EXPECT_CALL_NotifyAppExOfForward(iAccountId,
                                     false, false, true, true, true, true, FWD_CFG_SRC_TYPE_STARTUP);
    // 全局FWD状态改变
    EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL,
                                     false, true, false, true, false, false, FWD_CFG_SRC_TYPE_SHUTDOWN);
    // 配置变更消息CONFIG_MSG_BCAST_CHANGED(wParam=ST_FORWARD)，触发重新加载
    msgObject msg;
    msg.message = CONFIG_MSG_BCAST_CHANGED;
    msg.wParam = ST_FORWARD;
    TriggerEtlMsgCallback(msg);
    // 校验账号模式信息
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_ACCOUNT);
    EXPECT_ForwardInfoEq(iAccountId, false, true, true);
    // 释放前会通知状态改变为Off
    EXPECT_CALL_NotifyAppExOfForward(iAccountId,
                                     false, false, false, true, false, true, FWD_CFG_SRC_TYPE_SHUTDOWN);
    _FwdManager.Finalize();
}

// 模式配置变更：账号->全局
TEST_F(FwdManagerTest, ModeConfigAccountToGlobal)
{
    // 账号模式
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_ACCOUNT);
    // 账号列表
    const int iAccountId = 3;
    ListAccounts l;
    l.push_back(iAccountId);
    ON_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).Times(1);
    // 账号FWD配置
    EXPECT_CALL_GetConfigOfFoward(iAccountId, true, true, false);
    // 账号FWD状态改变
    EXPECT_CALL_NotifyAppExOfForward(iAccountId,
                                     true, true, true, true, false, false, FWD_CFG_SRC_TYPE_STARTUP);
    // 初始化
    _FwdManager.Init();
    // 校验账号模式信息
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_ACCOUNT);
    EXPECT_ForwardInfoEq(iAccountId, true, true, false);
    // 配置变为全局模式
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_GLOBAL);
    // 全局FWD状态
    EXPECT_CALL_GetConfigOfFoward(FWDDND_ACCOUNT_GLOBAL, false, true, false);
    // 通知状态改变
    EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL,
                                     false, false, true, true, false, false, FWD_CFG_SRC_TYPE_STARTUP);
    EXPECT_CALL_NotifyAppExOfForward(iAccountId,
                                     false, true, false, true, false, false, FWD_CFG_SRC_TYPE_SHUTDOWN);
    // 配置变更消息CONFIG_MSG_BCAST_CHANGED(wParam=ST_FORWARD)，触发重新加载
    msgObject msg;
    msg.message = CONFIG_MSG_BCAST_CHANGED;
    msg.wParam = ST_FORWARD;
    TriggerEtlMsgCallback(msg);
    // 校验全局模式信息
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_GLOBAL);
    EXPECT_ForwardInfoEq(FWDDND_ACCOUNT_GLOBAL, false, true, false);
    // 释放前会通知状态改变为Off
    EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL,
                                     false, false, false, true, false, false, FWD_CFG_SRC_TYPE_SHUTDOWN);
    _FwdManager.Finalize();
}

// 模式切换：全局->账号
TEST_F(FwdManagerTest, SwitchModeGlobalToAccount)
{
    // 全局模式
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_GLOBAL);
    // 全局FWD状态
    EXPECT_CALL_GetConfigOfFoward(FWDDND_ACCOUNT_GLOBAL, true, true, false);
    // 通知状态改变
    EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL,
                                     true, true, true, true, false, false, FWD_CFG_SRC_TYPE_STARTUP);
    // 初始化
    _FwdManager.Init();
    // 校验全局模式信息
    EXPECT_TRUE(_FwdManager.IsActive());
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_GLOBAL);
    EXPECT_ForwardInfoEq(FWDDND_ACCOUNT_GLOBAL, true, true, false);
    // 配置变为账号模式（写入与读出）
    EXPECT_CALL_SetConfigInt(kszForwardMode, FWDDND_MODE_ACCOUNT);
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_ACCOUNT);
    // 账号列表
    const int iAccountId = 5;
    ListAccounts l;
    l.push_back(iAccountId);
    ON_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).Times(1);
    // 账号FWD配置
    EXPECT_CALL_GetConfigOfFoward(iAccountId, false, true, true);
    // 账号FWD状态改变
    EXPECT_CALL_NotifyAppExOfForward(iAccountId,
                                     false, false, true, true, true, true, FWD_CFG_SRC_TYPE_STARTUP);
    // 全局FWD状态改变
    EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL,
                                     false, true, false, true, false, false, FWD_CFG_SRC_TYPE_SHUTDOWN);
    // 设置模式为账号模式
    _FwdManager.SetMode(FWDDND_MODE_ACCOUNT);
    // 校验账号模式信息
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_ACCOUNT);
    EXPECT_ForwardInfoEq(iAccountId, false, true, true);
    // 释放前会通知状态改变为Off
    EXPECT_CALL_NotifyAppExOfForward(iAccountId,
                                     false, false, false, true, false, true, FWD_CFG_SRC_TYPE_SHUTDOWN);
    _FwdManager.Finalize();
}

// 模式切换：账号->全局
TEST_F(FwdManagerTest, SwitchModeAccountToGlobal)
{
    // 账号模式
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_ACCOUNT);
    // 账号列表
    const int iAccountId = 6;
    ListAccounts l;
    l.push_back(iAccountId);
    ON_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).Times(1);
    // 账号FWD配置
    EXPECT_CALL_GetConfigOfFoward(iAccountId, true, true, false);
    // 账号FWD状态改变
    EXPECT_CALL_NotifyAppExOfForward(iAccountId,
                                     true, true, true, true, false, false, FWD_CFG_SRC_TYPE_STARTUP);
    // 初始化
    _FwdManager.Init();
    // 校验账号模式信息
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_ACCOUNT);
    EXPECT_ForwardInfoEq(iAccountId, true, true, false);
    // 配置变为全局模式（写入与读出）
    EXPECT_CALL_SetConfigInt(kszForwardMode, FWDDND_MODE_GLOBAL);
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_GLOBAL);
    // 全局FWD状态
    EXPECT_CALL_GetConfigOfFoward(FWDDND_ACCOUNT_GLOBAL, false, true, false);
    // 通知状态改变
    EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL,
                                     false, false, true, true, false, false, FWD_CFG_SRC_TYPE_STARTUP);
    EXPECT_CALL_NotifyAppExOfForward(iAccountId,
                                     false, true, false, true, false, false, FWD_CFG_SRC_TYPE_SHUTDOWN);
    // 设置模式为全局模式
    _FwdManager.SetMode(FWDDND_MODE_GLOBAL);
    // 校验全局模式信息
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_GLOBAL);
    EXPECT_ForwardInfoEq(FWDDND_ACCOUNT_GLOBAL, false, true, false);
    // 释放前会通知状态改变为Off
    EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL,
                                     false, false, false, true, false, false, FWD_CFG_SRC_TYPE_SHUTDOWN);
    _FwdManager.Finalize();
}

// 设置全局模式状态
TEST_F(FwdManagerTest, SetGlobalStatus)
{
    // 全局模式
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_GLOBAL);
    // 全局FWD状态
    EXPECT_CALL_GetConfigOfFoward(FWDDND_ACCOUNT_GLOBAL, true, true, false);
    // 通知状态改变
    EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL,
                                     true, true, true, true, false, false, FWD_CFG_SRC_TYPE_STARTUP);
    // 初始化
    _FwdManager.Init();
    // 校验全局模式信息
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_GLOBAL);
    EXPECT_ForwardInfoEq(FWDDND_ACCOUNT_GLOBAL, true, true, false);
    {
        // 修改信息时配置改变
        EXPECT_CALL_SetConfigOfGlobalAlwaysFoward(false,
                s_objGlobalAlwaysForwardData.szTarget, true);
        // 写入最后一次修改配置时启用的FWD类型
        EXPECT_CALL_SetConfigInt(kszForwardTypeLastSaved, FWD_TYPE_BUSY);
        // 修改信息后通知状态改变
        EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL, false, true, true, false, false, false);
        // 修改信息
        EXPECT_TRUE(_FwdManager.SetInfo(FWDDND_ACCOUNT_GLOBAL, FWD_TYPE_ALWAYS, false,
                                        &yl::string(s_objGlobalAlwaysForwardData.szTarget),
                                        NULL, FWD_CFG_SRC_TYPE_COMMON));
        EXPECT_ForwardInfoEq(FWDDND_ACCOUNT_GLOBAL, false, true, false);
    }
    {
        // 修改信息时配置改变（不备份）
        EXPECT_CALL_SetConfigOfGlobalNoAnswerFoward(true, s_objGlobalNoAnswerForwardData.szTarget,
                s_objGlobalNoAnswerForwardData.iRingDelay);
        // 写入最后一次修改配置时启用的FWD类型
        EXPECT_CALL_SetConfigInt(kszForwardTypeLastSaved, FWD_TYPE_BUSY | FWD_TYPE_NO_ANSWER);
        // 修改信息后通知状态改变
        EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL, false, false, true, false, true, true,
                                         FWD_CFG_SRC_TYPE_DSSKEY);
        // 修改信息
        EXPECT_TRUE(_FwdManager.SetInfo(FWDDND_ACCOUNT_GLOBAL, FWD_TYPE_NO_ANSWER, true,
                                        &yl::string(s_objGlobalNoAnswerForwardData.szTarget),
                                        &s_objGlobalNoAnswerForwardData.iRingDelay, FWD_CFG_SRC_TYPE_DSSKEY));
        EXPECT_ForwardInfoEq(FWDDND_ACCOUNT_GLOBAL, false, true, true);
    }
    // 释放前会通知状态改变为Off
    EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL,
                                     false, false, false, true, false, true, FWD_CFG_SRC_TYPE_SHUTDOWN);
    _FwdManager.Finalize();
}

// 设置账号DND状态
TEST_F(FwdManagerTest, SetAccountStatus)
{
    // 账号模式
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_ACCOUNT);
    // 账号列表
    const int iAccountId = 9;
    ListAccounts l;
    l.push_back(iAccountId);
    ON_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).Times(1);
    // 账号FWD配置
    EXPECT_CALL_GetConfigOfFoward(iAccountId, false, false, true);
    // 账号FWD状态改变
    EXPECT_CALL_NotifyAppExOfForward(iAccountId,
                                     false, false, false, false, true, true, FWD_CFG_SRC_TYPE_STARTUP);
    // 初始化
    _FwdManager.Init();
    // 校验账号模式信息
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_ACCOUNT);
    EXPECT_ForwardInfoEq(iAccountId, false, false, true);
    {
        // 修改信息时配置改变
        EXPECT_CALL_SetConfigOfAccountAlwaysFoward(true, iAccountId,
                s_objAccountAlwaysForwardData.szTarget);
        // 修改信息后通知状态改变
        EXPECT_CALL_NotifyAppExOfForward(iAccountId, true, true, false, false, true, false);
        // 修改信息
        EXPECT_TRUE(_FwdManager.SetInfo(iAccountId, FWD_TYPE_ALWAYS, true,
                                        &yl::string(s_objAccountAlwaysForwardData.szTarget),
                                        NULL, FWD_CFG_SRC_TYPE_COMMON));
        EXPECT_ForwardInfoEq(iAccountId, true, false, true);
    }
    {
        // 修改信息时配置改变（不备份）
        EXPECT_CALL_SetConfigOfAccountBusyFoward(true, iAccountId,
                s_objAccountBusyForwardData.szTarget);
        // 修改信息后通知状态改变
        EXPECT_CALL_NotifyAppExOfForward(iAccountId, true, false, true, true, true, false,
                                         FWD_CFG_SRC_TYPE_DSSKEY);
        // 修改信息
        EXPECT_TRUE(_FwdManager.SetInfo(iAccountId, FWD_TYPE_BUSY, true,
                                        &yl::string(s_objAccountBusyForwardData.szTarget), NULL, FWD_CFG_SRC_TYPE_DSSKEY));
        EXPECT_ForwardInfoEq(iAccountId, true, true, true);
    }
    // 释放前会通知状态改变为Off
    EXPECT_CALL_NotifyAppExOfForward(iAccountId,
                                     false, true, false, true, false, true, FWD_CFG_SRC_TYPE_SHUTDOWN);
    _FwdManager.Finalize();
}

// 同时设置多个全局FWD状态
TEST_F(FwdManagerTest, SetMultiInfo)
{
    // 全局模式
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_GLOBAL);
    // 全局FWD状态
    EXPECT_CALL_GetConfigOfFoward(FWDDND_ACCOUNT_GLOBAL, false, true, true);
    // 通知状态改变
    EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL,
                                     false, false, true, true, true, true, FWD_CFG_SRC_TYPE_STARTUP);
    // 初始化
    _FwdManager.Init();
    // 校验全局模式信息
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_GLOBAL);
    EXPECT_ForwardInfoEq(FWDDND_ACCOUNT_GLOBAL, false, true, true);
    {
        // 修改信息时配置改变
        EXPECT_CALL_SetConfigOfGlobalBusyFoward(false, s_objGlobalBusyForwardData.szTarget);
        EXPECT_CALL_SetConfigOfGlobalNoAnswerFoward(false,
                s_objGlobalNoAnswerForwardData.szTarget, s_objGlobalNoAnswerForwardData.iRingDelay);
        // 修改信息后通知状态改变
        EXPECT_CALL_NotifyAppExOfForward(FWDDND_ACCOUNT_GLOBAL, false, false, false, true, false, true);
        // 修改信息
        EXPECT_TRUE(_FwdManager.SetMultiInfo(FWDDND_ACCOUNT_GLOBAL, FWD_TYPE_ALL, false));
        EXPECT_ForwardInfoEq(FWDDND_ACCOUNT_GLOBAL, false, false, false);
    }
    _FwdManager.Finalize();
}

// 获取全局模式最后一次修改配置时启用的FWD类型
TEST_F(FwdManagerTest, GetGlobalLastSavedType)
{
    // 全局模式
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_GLOBAL);
    // 全局FWD状态
    EXPECT_CALL_GetConfigOfFoward(FWDDND_ACCOUNT_GLOBAL, false, false, false);
    // 初始化
    _FwdManager.Init();
    // 校验全局模式信息
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_GLOBAL);
    // 读取最后一次修改配置时启用的FWD类型配置
    EXPECT_CALL_GetConfigInt(kszForwardTypeLastSaved, FWD_TYPE_ALL);
    EXPECT_EQ(_FwdManager.GetLastSavedType(), FWD_TYPE_ALL);
    _FwdManager.Finalize();
}

// 获取账号模式最后一次修改配置时启用的FWD类型
TEST_F(FwdManagerTest, GetAccountLastSavedType)
{
    // 账号模式
    EXPECT_CALL_GetConfigInt(kszForwardMode, FWDDND_MODE_ACCOUNT);
    // 账号列表
    const int iAccountId = 5;
    ListAccounts l;
    l.push_back(iAccountId);
    ON_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(m_objMockAccount, GetAccountList(ACCOUNT_TYPE_SIP)).Times(1);
    // FWD状态
    EXPECT_CALL_GetConfigOfFoward(iAccountId, false, false, false);
    // 初始化
    _FwdManager.Init();
    // 校验模式
    EXPECT_EQ(_FwdManager.GetMode(), FWDDND_MODE_ACCOUNT);
    // 读取最后一次修改配置时启用的FWD类型配置
    EXPECT_EQ(_FwdManager.GetLastSavedType(), 0);
    _FwdManager.Finalize();
}

// 判断是否允许转移到指定号码（国际长途）
TEST(FwdManagerStaticTest, IsAllowForwardTo)
{
    MockConfigParser objMockConfigParser;
    g_pMockConfigParser = &objMockConfigParser;
    // 启用国际长途功能，应该不做转移限制
    EXPECT_CALL_GetConfigInt(kszForwardInternationalEnable, 1);
    EXPECT_TRUE(CFwdManager::IsAllowForwardTo(""));
    EXPECT_TRUE(CFwdManager::IsAllowForwardTo("0"));
    EXPECT_TRUE(CFwdManager::IsAllowForwardTo("00"));
    EXPECT_TRUE(CFwdManager::IsAllowForwardTo("008615812345678"));
    EXPECT_TRUE(CFwdManager::IsAllowForwardTo("+8615812345678"));

    // 限制国际长途功能，根据号码前缀判断是否允许转移
    EXPECT_CALL_GetConfigInt(kszForwardInternationalEnable, 0);
    EXPECT_CALL_GetConfigString(kszForwardInternationalNumber, "00");
    EXPECT_TRUE(CFwdManager::IsAllowForwardTo(""));
    EXPECT_TRUE(CFwdManager::IsAllowForwardTo("0"));
    EXPECT_TRUE(CFwdManager::IsAllowForwardTo("00"));
    EXPECT_FALSE(CFwdManager::IsAllowForwardTo("008615812345678"));
    EXPECT_TRUE(CFwdManager::IsAllowForwardTo("+8615812345678"));

    // 国际长途前缀为空，应该不做转移限制
    EXPECT_CALL_GetConfigString(kszForwardInternationalNumber, "");
    EXPECT_TRUE(CFwdManager::IsAllowForwardTo(""));
    EXPECT_TRUE(CFwdManager::IsAllowForwardTo("0"));
    EXPECT_TRUE(CFwdManager::IsAllowForwardTo("00"));
    EXPECT_TRUE(CFwdManager::IsAllowForwardTo("008615812345678"));
    EXPECT_TRUE(CFwdManager::IsAllowForwardTo("+8615812345678"));

    g_pMockConfigParser = NULL;
}
