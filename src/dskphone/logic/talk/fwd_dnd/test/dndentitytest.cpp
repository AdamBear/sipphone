#include <gtest/gtest.h>

#include "dndentity.h"
#include "fwddndmsgdefine.h"

#include "mocklibs/configparser/mockconfigparser.h"
#include "mocklibs/etlmsghandler/mocketlmsghandler.h"


class DndEntityTest : public testing::Test
{
protected:
    MockConfigParser m_objMockConfigParser;
    MockEtlMsgHandler m_objMockEtlMsgHandler;

    virtual void SetUp()
    {
        g_pMockConfigParser = &m_objMockConfigParser;
        g_pMockEtlMsgHandler = &m_objMockEtlMsgHandler;
    }

    virtual void TearDown()
    {
        g_pMockConfigParser = NULL;
        g_pMockEtlMsgHandler = NULL;
    }
};

// 创建全局DND实体
TEST_F(DndEntityTest, CreateGlobalEntity)
{
    // 全局DND状态：Off
    EXPECT_CALL_GetConfigInt(kszDndStatus, 0);

    CDndEntity * pEntity = new CDndEntity(FWDDND_ACCOUNT_GLOBAL);
    EXPECT_EQ(pEntity->GetAccountId(), FWDDND_ACCOUNT_GLOBAL);
    EXPECT_FALSE(pEntity->IsEnable());
    delete pEntity;
    pEntity = NULL;

    // 全局DND状态：On
    EXPECT_CALL_GetConfigInt(kszDndStatus, 1);
    // 通知状态改变为On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_STARTUP, true);

    pEntity = new CDndEntity(FWDDND_ACCOUNT_GLOBAL);
    EXPECT_TRUE(pEntity->IsEnable());
    // 释放前会通知状态改变为Off
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_SHUTDOWN, false);
    delete pEntity;
    pEntity = NULL;
}

// 创建账号DND实体
TEST_F(DndEntityTest, CreateAccountEntity)
{
    const int iAccountId = 0;
    // 账号DND状态：Off
    EXPECT_CALL_GetCfgItemIntValue(kszAccountDNDStatus, iAccountId, 0);

    CDndEntity * pEntity = new CDndEntity(iAccountId);
    EXPECT_EQ(pEntity->GetAccountId(), iAccountId);
    EXPECT_FALSE(pEntity->IsEnable());
    delete pEntity;
    pEntity = NULL;

    // 账号DND状态：On
    EXPECT_CALL_GetCfgItemIntValue(kszAccountDNDStatus, iAccountId, 1);
    // 通知状态改变为On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            iAccountId, DND_CFG_SRC_TYPE_STARTUP, true);

    pEntity = new CDndEntity(iAccountId);
    EXPECT_TRUE(pEntity->IsEnable());
    // 释放前会通知状态改变为Off
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            iAccountId, DND_CFG_SRC_TYPE_SHUTDOWN, false);
    delete pEntity;
    pEntity = NULL;
}

// 配置修改引发全局DND状态切换：Off->On
TEST_F(DndEntityTest, ReloadGlobalConfigToEnable)
{
    // 全局DND状态：Off
    EXPECT_CALL_GetConfigInt(kszDndStatus, 0);

    CDndEntity * pEntity = new CDndEntity(FWDDND_ACCOUNT_GLOBAL);
    EXPECT_FALSE(pEntity->IsEnable());

    // 全局DND状态改为On
    EXPECT_CALL_GetConfigInt(kszDndStatus, 1);
    // 状态改变消息
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_COMMON, true);

    pEntity->ReloadConfig(); // 重新加载配置
    EXPECT_TRUE(pEntity->IsEnable());

    // 释放前会通知状态改变为Off
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_SHUTDOWN, false);

    delete pEntity;
    pEntity = NULL;
}

// 配置修改引发账号DND状态切换：On->Off
TEST_F(DndEntityTest, ReloadAccountConfigToDisable)
{
    const int iAccountId = 0;

    // 账号DND状态：On
    EXPECT_CALL_GetCfgItemIntValue(kszAccountDNDStatus, iAccountId, 1);
    // 通知状态改变为On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            iAccountId, DND_CFG_SRC_TYPE_STARTUP, true);

    CDndEntity * pEntity = new CDndEntity(iAccountId);
    EXPECT_EQ(pEntity->GetAccountId(), iAccountId);
    EXPECT_TRUE(pEntity->IsEnable());

    // 账号DND状态改为Off
    EXPECT_CALL_GetCfgItemIntValue(kszAccountDNDStatus, iAccountId, 0);
    // 状态改变消息
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            iAccountId, DND_CFG_SRC_TYPE_COMMON, false);

    pEntity->ReloadConfig();
    EXPECT_FALSE(pEntity->IsEnable());

    delete pEntity;
    pEntity = NULL;
}

// 重新加载配置但全局DND状态未变化：On->On
TEST_F(DndEntityTest, ReloadGlobalConfigUnchanged)
{
    // 全局DND状态：On
    EXPECT_CALL_GetConfigInt(kszDndStatus, 1);
    // 通知状态改变为On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_STARTUP, true);

    CDndEntity * pEntity = new CDndEntity(FWDDND_ACCOUNT_GLOBAL);
    EXPECT_TRUE(pEntity->IsEnable());

    pEntity->ReloadConfig(); // 重新加载配置，不会发送消息
    EXPECT_TRUE(pEntity->IsEnable());

    // 释放前会通知状态改变为Off
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_SHUTDOWN, false);

    delete pEntity;
    pEntity = NULL;
}

// 设置全局DND状态：On->Off
TEST_F(DndEntityTest, SetGlobalDisable)
{
    // 全局DND状态：On
    EXPECT_CALL_GetConfigInt(kszDndStatus, 1);
    // 通知状态改变为On
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_STARTUP, true);

    CDndEntity * pEntity = new CDndEntity(FWDDND_ACCOUNT_GLOBAL);
    EXPECT_TRUE(pEntity->IsEnable());

    // 写入配置：Off
    EXPECT_CALL_SetConfigInt(kszDndStatus, 0);
    // 修改配置后广播状态变化消息
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, DND_CFG_SRC_TYPE_COMMON, false);

    EXPECT_TRUE(pEntity->SetEnable(false, DND_CFG_SRC_TYPE_COMMON));
    EXPECT_FALSE(pEntity->IsEnable());

    delete pEntity;
    pEntity = NULL;
}

// 设置账号DND状态：Off->On
TEST_F(DndEntityTest, SetAccountEnable)
{
    const int iAccountId = 0;

    // 账号DND状态：Off
    EXPECT_CALL_GetCfgItemIntValue(kszAccountDNDStatus, iAccountId, 0);

    CDndEntity * pEntity = new CDndEntity(iAccountId);
    EXPECT_FALSE(pEntity->IsEnable());

    // 写入配置：On
    EXPECT_CALL_SetCfgItemIntValue(kszAccountDNDStatus, iAccountId, 1);
    // 修改配置后广播状态变化消息
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            iAccountId, DND_CFG_SRC_TYPE_COMMON, true);

    pEntity->SetEnable(true, DND_CFG_SRC_TYPE_COMMON);
    EXPECT_TRUE(pEntity->IsEnable());

    // 释放前会通知状态改变为Off
    EXPECT_CALL_NotifyAppEx(DND_MESSAGE_STATUS_CHANGED,
                            iAccountId, DND_CFG_SRC_TYPE_SHUTDOWN, false);

    delete pEntity;
    pEntity = NULL;
}

// 设置账号DND状态：Off->Off
TEST_F(DndEntityTest, SetAccountEnableUnchanged)
{
    const int iAccountId = 0;

    // 账号DND状态：Off
    EXPECT_CALL_GetCfgItemIntValue(kszAccountDNDStatus, iAccountId, 0);

    CDndEntity * pEntity = new CDndEntity(iAccountId);
    EXPECT_FALSE(pEntity->IsEnable());

    // 修改为Off，应该不会有任何操作
    pEntity->SetEnable(false, DND_CFG_SRC_TYPE_COMMON);
    EXPECT_FALSE(pEntity->IsEnable());

    delete pEntity;
    pEntity = NULL;
}
