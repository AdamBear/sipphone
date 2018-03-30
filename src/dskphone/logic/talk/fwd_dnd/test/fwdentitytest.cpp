#include "fwdentitytest.h"

#include "fwdentity.h"

#include <popupboxmsgdefine.h> // MULTIFWD_ENABLED_NOTICE



FwdMessageStatusData FwdEntityTest::s_objGlobalAlwaysForwardData(FWD_TYPE_ALWAYS);
FwdMessageStatusData FwdEntityTest::s_objGlobalBusyForwardData(FWD_TYPE_BUSY);
FwdMessageStatusData FwdEntityTest::s_objGlobalNoAnswerForwardData(FWD_TYPE_NO_ANSWER);
FwdMessageStatusData FwdEntityTest::s_objAccountAlwaysForwardData(FWD_TYPE_ALWAYS);
FwdMessageStatusData FwdEntityTest::s_objAccountBusyForwardData(FWD_TYPE_BUSY);
FwdMessageStatusData FwdEntityTest::s_objAccountNoAnswerForwardData(FWD_TYPE_NO_ANSWER);

void FwdEntityTest::SetUpTestCase()
{
    strcpy(s_objGlobalAlwaysForwardData.szTarget, "always_fwd_target");
    strcpy(s_objGlobalBusyForwardData.szTarget, "busy_fwd_target");
    strcpy(s_objGlobalNoAnswerForwardData.szTarget, "no_answer_fwd_target");
    s_objGlobalNoAnswerForwardData.iRingDelay = 0;

    strcpy(s_objAccountAlwaysForwardData.szTarget, "_always_fwd_target");
    strcpy(s_objAccountBusyForwardData.szTarget, "_busy_fwd_target");
    strcpy(s_objAccountNoAnswerForwardData.szTarget, "_no_answer_fwd_target");
    s_objAccountNoAnswerForwardData.iRingDelay = 20;
}

void FwdEntityTest::SetUp()
{
    g_pMockConfigParser = &m_objMockConfigParser;
    g_pMockEtlMsgHandler = &m_objMockEtlMsgHandler;
}

void FwdEntityTest::TearDown()
{
    g_pMockConfigParser = NULL;
    g_pMockEtlMsgHandler = NULL;
}

void FwdEntityTest::EXPECT_CALL_GetConfigOfGlobalAlwaysFoward(
    bool bEnable,
    const yl::string & strTarget /* = s_objGlobalAlwaysForwardData.szTarget */)
{
    EXPECT_CALL_GetConfigInt(kszForwardAlwaysSwitch, bEnable);
    EXPECT_CALL_GetConfigString(kszForwardAlwaysTarget, strTarget.c_str());
}
void FwdEntityTest::EXPECT_CALL_GetConfigOfGlobalBusyFoward(
    bool bEnable,
    const yl::string & strTarget /* = s_objGlobalBusyForwardData.szTarget */)
{
    EXPECT_CALL_GetConfigInt(kszForwardBusySwitch, bEnable);
    EXPECT_CALL_GetConfigString(kszForwardBusyTarget, strTarget.c_str());
}
void FwdEntityTest::EXPECT_CALL_GetConfigOfGlobalNoAnswerFoward(
    bool bEnable,
    const yl::string & strTarget /* = s_objGlobalNoAnswerForwardData.szTarget */,
    int iRingDelay /* = s_objGlobalNoAnswerForwardData.iRingDelay */)
{
    EXPECT_CALL_GetConfigInt(kszForwardNoAnswerSwitch, bEnable);
    EXPECT_CALL_GetConfigString(kszForwardNoAnswerTarget, strTarget.c_str());
    EXPECT_CALL_GetConfigInt(kszForwardNoAnswerDelayTime, iRingDelay);
}

void FwdEntityTest::EXPECT_CALL_SetConfigOfGlobalAlwaysFoward(bool bEnable,
        const yl::string & strTarget, bool bBackup)
{
    EXPECT_CALL_SetConfigInt(kszForwardAlwaysSwitch, bEnable);
    EXPECT_CALL_SetConfigString(kszForwardAlwaysTarget, strTarget.c_str());
    if (bBackup)
    {
        EXPECT_CALL_SetConfigString(kszForwardAlwaysTargetBackup, strTarget.c_str());
    }
}
void FwdEntityTest::EXPECT_CALL_SetConfigOfGlobalBusyFoward(bool bEnable,
        const yl::string & strTarget)
{
    EXPECT_CALL_SetConfigInt(kszForwardBusySwitch, bEnable);
    EXPECT_CALL_SetConfigString(kszForwardBusyTarget, strTarget.c_str());
}
void FwdEntityTest::EXPECT_CALL_SetConfigOfGlobalNoAnswerFoward(bool bEnable,
        const yl::string & strTarget, int iRingDelay)
{
    EXPECT_CALL_SetConfigInt(kszForwardNoAnswerSwitch, bEnable);
    EXPECT_CALL_SetConfigString(kszForwardNoAnswerTarget, strTarget.c_str());
    EXPECT_CALL_SetConfigInt(kszForwardNoAnswerDelayTime, iRingDelay);
}

void FwdEntityTest::EXPECT_CALL_GetConfigOfAccountAlwaysFoward(
    bool bEnable,
    int iAccountId,
    const yl::string & strTarget /* = s_objAccountAlwaysForwardData.szTarget */)
{
    EXPECT_CALL_GetCfgItemIntValue(kszAccountAlwaysFWDStatus, iAccountId, bEnable);
    EXPECT_CALL_GetCfgItemStringValue(kszAccountAlwaysFWDTarget, iAccountId, strTarget.c_str());
}
void FwdEntityTest::EXPECT_CALL_GetConfigOfAccountBusyFoward(
    bool bEnable,
    int iAccountId,
    const yl::string & strTarget /* = s_objAccountBusyForwardData.szTarget */)
{
    EXPECT_CALL_GetCfgItemIntValue(kszAccountBusyFWDStatus, iAccountId, bEnable);
    EXPECT_CALL_GetCfgItemStringValue(kszAccountBusyFWDTarget, iAccountId, strTarget.c_str());
}
void FwdEntityTest::EXPECT_CALL_GetConfigOfAccountNoAnswerFoward(
    bool bEnable,
    int iAccountId,
    const yl::string & strTarget /* = s_objAccountNoAnswerForwardData.szTarget */,
    int iRingDelay /* = s_objAccountNoAnswerForwardData.iRingDelay */)
{
    EXPECT_CALL_GetCfgItemIntValue(kszAccountNoASWFWDStatus, iAccountId, bEnable);
    EXPECT_CALL_GetCfgItemStringValue(kszAccountNoASWFWDTarget, iAccountId, strTarget.c_str());
    EXPECT_CALL_GetCfgItemIntValue(kszAccountNoASWFWDTimeout, iAccountId, iRingDelay);
}

void FwdEntityTest::EXPECT_CALL_SetConfigOfAccountAlwaysFoward(bool bEnable, int iAccountId,
        const yl::string & strTarget)
{
    EXPECT_CALL_SetCfgItemIntValue(kszAccountAlwaysFWDStatus, iAccountId, bEnable);
    EXPECT_CALL_SetCfgItemStringValue(kszAccountAlwaysFWDTarget, iAccountId, strTarget.c_str());
}
void FwdEntityTest::EXPECT_CALL_SetConfigOfAccountBusyFoward(bool bEnable, int iAccountId,
        const yl::string & strTarget)
{
    EXPECT_CALL_SetCfgItemIntValue(kszAccountBusyFWDStatus, iAccountId, bEnable);
    EXPECT_CALL_SetCfgItemStringValue(kszAccountBusyFWDTarget, iAccountId, strTarget.c_str());
}
void FwdEntityTest::EXPECT_CALL_SetConfigOfAccountNoAnswerFoward(bool bEnable, int iAccountId,
        const yl::string & strTarget, int iRingDelay)
{
    EXPECT_CALL_SetCfgItemIntValue(kszAccountNoASWFWDStatus, iAccountId, bEnable);
    EXPECT_CALL_SetCfgItemStringValue(kszAccountNoASWFWDTarget, iAccountId, strTarget.c_str());
    EXPECT_CALL_SetCfgItemIntValue(kszAccountNoASWFWDTimeout, iAccountId, iRingDelay);
}

bool operator==(const FwdMessageStatusData & lhs, const FwdMessageStatusData & rhs)
{
    //printf("lhs: type=%d, enable=%d, target=%s, target_account=%d, ring_delay=%d\n",
    //  lhs.eType, lhs.bEnabled, lhs.szTarget, lhs.iTargetAccount, lhs.iRingDelay);
    //printf("lhs: type=%d, enable=%d, target=%s, target_account=%d, ring_delay=%d\n",
    //  rhs.eType, rhs.bEnabled, rhs.szTarget, rhs.iTargetAccount, rhs.iRingDelay);
    if (lhs.eType != rhs.eType || lhs.bEnabled != rhs.bEnabled
            || strcmp(lhs.szTarget, rhs.szTarget) != 0)
    {
        return false;
    }
    if (lhs.eType == FWD_TYPE_NO_ANSWER && lhs.iRingDelay != rhs.iRingDelay)
    {
        return false;
    }
    return true;
}

// 创建全局FWD实体
TEST_F(FwdEntityTest, CreateGlobalEntityOff)
{
    yl::string strTarget;
    int iRingDelay = 0;

    // 全局FWD状态：Off
    EXPECT_CALL_GetConfigOfGlobalAlwaysFoward(false);
    EXPECT_CALL_GetConfigOfGlobalBusyFoward(false);
    EXPECT_CALL_GetConfigOfGlobalNoAnswerFoward(false);

    CFwdEntity * pEntity = new CFwdEntity(FWDDND_ACCOUNT_GLOBAL);
    EXPECT_EQ(pEntity->GetAccountId(), FWDDND_ACCOUNT_GLOBAL);
    EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_ALWAYS));
    EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_BUSY));
    EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_NO_ANSWER));
    EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_ALL));
    delete pEntity;
    pEntity = NULL;
}
TEST_F(FwdEntityTest, CreateGlobalEntityOn)
{
    yl::string strTarget;
    int iRingDelay = 0;
    // 全局FWD状态：On
    EXPECT_CALL_GetConfigOfGlobalAlwaysFoward(true);
    EXPECT_CALL_GetConfigOfGlobalBusyFoward(true);
    EXPECT_CALL_GetConfigOfGlobalNoAnswerFoward(true);
    // 通知状态改变为On
    s_objGlobalAlwaysForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_STARTUP, s_objGlobalAlwaysForwardData);
    s_objGlobalBusyForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_STARTUP, s_objGlobalBusyForwardData);
    s_objGlobalNoAnswerForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_STARTUP, s_objGlobalNoAnswerForwardData);

    CFwdEntity * pEntity = new CFwdEntity(FWDDND_ACCOUNT_GLOBAL);
    EXPECT_EQ(pEntity->GetAccountId(), FWDDND_ACCOUNT_GLOBAL);
    EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_ALWAYS, &strTarget));
    EXPECT_STREQ(strTarget.c_str(), s_objGlobalAlwaysForwardData.szTarget);
    EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_BUSY, &strTarget));
    EXPECT_STREQ(strTarget.c_str(), s_objGlobalBusyForwardData.szTarget);
    EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_NO_ANSWER, &strTarget, &iRingDelay));
    EXPECT_STREQ(strTarget.c_str(), s_objGlobalNoAnswerForwardData.szTarget);
    EXPECT_EQ(iRingDelay, s_objGlobalNoAnswerForwardData.iRingDelay);
    EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_ALL));

    // 释放前会通知状态改变为Off
    s_objGlobalAlwaysForwardData.bEnabled = false;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_SHUTDOWN, s_objGlobalAlwaysForwardData);
    s_objGlobalBusyForwardData.bEnabled = false;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_SHUTDOWN, s_objGlobalBusyForwardData);
    s_objGlobalNoAnswerForwardData.bEnabled = false;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_SHUTDOWN, s_objGlobalNoAnswerForwardData);
    delete pEntity;
    pEntity = NULL;
}

// 创建账号FWD实体
TEST_F(FwdEntityTest, CreateAccountEntity)
{
    const int iAccountId = 0;
    yl::string strTarget;
    int iRingDelay = 0;

    // 账号FWD状态：Off
    EXPECT_CALL_GetConfigOfAccountAlwaysFoward(false, iAccountId);
    EXPECT_CALL_GetConfigOfAccountBusyFoward(false, iAccountId);
    EXPECT_CALL_GetConfigOfAccountNoAnswerFoward(false, iAccountId);

    CFwdEntity * pEntity = new CFwdEntity(iAccountId);
    EXPECT_EQ(pEntity->GetAccountId(), iAccountId);
    EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_ALWAYS));
    EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_BUSY));
    EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_NO_ANSWER));
    EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_ALL));
    delete pEntity;
    pEntity = NULL;

    // 账号FWD状态：On
    EXPECT_CALL_GetConfigOfAccountAlwaysFoward(true, iAccountId);
    EXPECT_CALL_GetConfigOfAccountBusyFoward(true, iAccountId);
    EXPECT_CALL_GetConfigOfAccountNoAnswerFoward(true, iAccountId);
    // 通知状态改变为On
    s_objAccountAlwaysForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            iAccountId, FWD_CFG_SRC_TYPE_STARTUP, s_objAccountAlwaysForwardData);
    s_objAccountBusyForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            iAccountId, FWD_CFG_SRC_TYPE_STARTUP, s_objAccountBusyForwardData);
    s_objAccountNoAnswerForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            iAccountId, FWD_CFG_SRC_TYPE_STARTUP, s_objAccountNoAnswerForwardData);

    pEntity = new CFwdEntity(iAccountId);
    EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_ALWAYS, &strTarget));
    EXPECT_STREQ(strTarget.c_str(), s_objAccountAlwaysForwardData.szTarget);
    EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_BUSY, &strTarget));
    EXPECT_STREQ(strTarget.c_str(), s_objAccountBusyForwardData.szTarget);
    EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_NO_ANSWER, &strTarget, &iRingDelay));
    EXPECT_STREQ(strTarget.c_str(), s_objAccountNoAnswerForwardData.szTarget);
    EXPECT_EQ(iRingDelay, s_objAccountNoAnswerForwardData.iRingDelay);
    EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_ALL));

    // 释放前会通知状态改变为Off
    s_objAccountAlwaysForwardData.bEnabled = false;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            iAccountId, FWD_CFG_SRC_TYPE_SHUTDOWN, s_objAccountAlwaysForwardData);
    s_objAccountBusyForwardData.bEnabled = false;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            iAccountId, FWD_CFG_SRC_TYPE_SHUTDOWN, s_objAccountBusyForwardData);
    s_objAccountNoAnswerForwardData.bEnabled = false;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            iAccountId, FWD_CFG_SRC_TYPE_SHUTDOWN, s_objAccountNoAnswerForwardData);
    delete pEntity;
    pEntity = NULL;
}

// 配置修改引发全局FWD状态切换：Off->On
TEST_F(FwdEntityTest, ReloadGlobalConfigToEnable)
{
    yl::string strTarget;
    int iTargetAccount = -1;
    int iRingDelay = 0;

    // 全局FWD状态：Off
    EXPECT_CALL_GetConfigOfGlobalAlwaysFoward(false);
    EXPECT_CALL_GetConfigOfGlobalBusyFoward(false);
    EXPECT_CALL_GetConfigOfGlobalNoAnswerFoward(false);

    CFwdEntity * pEntity = new CFwdEntity(FWDDND_ACCOUNT_GLOBAL);

    // 全局遇忙转移状态改为On
    EXPECT_CALL_GetConfigOfGlobalBusyFoward(true);
    // 消息通知遇忙转移状态变为On
    s_objGlobalBusyForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_COMMON, s_objGlobalBusyForwardData);

    pEntity->ReloadConfig(); // 重新加载配置
    EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_BUSY, &strTarget));
    EXPECT_STREQ(strTarget.c_str(), s_objGlobalBusyForwardData.szTarget);
    EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_ALL));

    // 释放前会通知状态改变为Off
    s_objGlobalBusyForwardData.bEnabled = false;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_SHUTDOWN, s_objGlobalBusyForwardData);

    delete pEntity;
    pEntity = NULL;
}

// 配置修改引发账号FWD状态切换：On->Off
TEST_F(FwdEntityTest, ReloadAccountConfigToDisable)
{
    const int iAccountId = 0;
    yl::string strTarget;
    int iRingDelay = 0;

    // 账号无条件转移状态：On
    EXPECT_CALL_GetConfigOfAccountAlwaysFoward(true, iAccountId);
    EXPECT_CALL_GetConfigOfAccountBusyFoward(false, iAccountId);
    EXPECT_CALL_GetConfigOfAccountNoAnswerFoward(false, iAccountId);
    // 初始化时通知无条件转移状态改变为On
    s_objAccountAlwaysForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            iAccountId, FWD_CFG_SRC_TYPE_STARTUP, s_objAccountAlwaysForwardData);

    CFwdEntity * pEntity = new CFwdEntity(iAccountId);
    EXPECT_EQ(pEntity->GetAccountId(), iAccountId);
    EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_ALWAYS, &strTarget));
    EXPECT_STREQ(strTarget.c_str(), s_objAccountAlwaysForwardData.szTarget);
    EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_BUSY));
    EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_NO_ANSWER));
    EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_ALL));

    // 账号无条件转移状态改为Off
    EXPECT_CALL_GetConfigOfAccountAlwaysFoward(false, iAccountId);
    // 消息通知无条件转移状态改变为Off
    s_objAccountAlwaysForwardData.bEnabled = false;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            iAccountId, FWD_CFG_SRC_TYPE_COMMON, s_objAccountAlwaysForwardData);

    pEntity->ReloadConfig();
    EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_ALWAYS));
    EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_ALL));

    delete pEntity;
    pEntity = NULL;
}

// 重新加载配置但全局FWD状态未变化：On->On
TEST_F(FwdEntityTest, ReloadGlobalConfigUnchanged)
{
    yl::string strTarget;
    int iRingDelay = 0;

    // 全局模式无应答转移状态：On
    EXPECT_CALL_GetConfigOfGlobalAlwaysFoward(false);
    EXPECT_CALL_GetConfigOfGlobalBusyFoward(false);
    EXPECT_CALL_GetConfigOfGlobalNoAnswerFoward(true);
    // 消息通知无应答转移状态变为On
    s_objGlobalNoAnswerForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_STARTUP, s_objGlobalNoAnswerForwardData);

    CFwdEntity * pEntity = new CFwdEntity(FWDDND_ACCOUNT_GLOBAL);
    EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_NO_ANSWER, &strTarget, &iRingDelay));
    EXPECT_STREQ(strTarget.c_str(), s_objGlobalNoAnswerForwardData.szTarget);
    pEntity->ReloadConfig(); // 重新加载配置
    EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_NO_ANSWER, &strTarget, &iRingDelay));
    EXPECT_STREQ(strTarget.c_str(), s_objGlobalNoAnswerForwardData.szTarget);

    // 释放前会通知状态改变为Off
    s_objGlobalNoAnswerForwardData.bEnabled = false;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_SHUTDOWN, s_objGlobalNoAnswerForwardData);

    delete pEntity;
    pEntity = NULL;
}

// 设置全局模式转移状态：On->Off（手动设置，需要备份配置）
TEST_F(FwdEntityTest, SetGlobalInfo)
{
    yl::string strTarget;
    int iRingDelay = 0;

    // 全局模式FWD状态：On
    EXPECT_CALL_GetConfigOfGlobalAlwaysFoward(true);
    EXPECT_CALL_GetConfigOfGlobalBusyFoward(true);
    EXPECT_CALL_GetConfigOfGlobalNoAnswerFoward(true);
    // 初始化时通知FWD状态改变为On
    s_objGlobalAlwaysForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_STARTUP, s_objGlobalAlwaysForwardData);
    s_objGlobalBusyForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_STARTUP, s_objGlobalBusyForwardData);
    s_objGlobalNoAnswerForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_STARTUP, s_objGlobalNoAnswerForwardData);

    CFwdEntity * pEntity = new CFwdEntity(FWDDND_ACCOUNT_GLOBAL);

    // 修改全局模式无条件转移配置
    {
        FwdMessageStatusData objNewAlwaysForwardData(FWD_TYPE_ALWAYS);
        objNewAlwaysForwardData.bEnabled = false;
        strcpy(objNewAlwaysForwardData.szTarget, "new_target_a");
        // 写入配置：Off
        EXPECT_CALL_SetConfigOfGlobalAlwaysFoward(false,
                objNewAlwaysForwardData.szTarget, true);
        // 写入最后一次修改配置时启用的FWD类型
        EXPECT_CALL_SetConfigInt(kszForwardTypeLastSaved, FWD_TYPE_BUSY | FWD_TYPE_NO_ANSWER);
        // 修改配置后广播状态变化消息
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_COMMON, objNewAlwaysForwardData);

        EXPECT_TRUE(pEntity->SetInfo(FWD_TYPE_ALWAYS, objNewAlwaysForwardData.bEnabled,
                                     &yl::string(objNewAlwaysForwardData.szTarget),
                                     NULL, FWD_CFG_SRC_TYPE_COMMON));
        EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_ALWAYS, &strTarget));
        EXPECT_STREQ(strTarget.c_str(), objNewAlwaysForwardData.szTarget);
    }

    // 修改全局模式遇忙转移配置
    {
        FwdMessageStatusData objNewBusyForwardData(FWD_TYPE_BUSY);
        objNewBusyForwardData.bEnabled = false;
        strcpy(objNewBusyForwardData.szTarget, "new_target_b");
        // 写入配置：Off
        EXPECT_CALL_SetConfigOfGlobalBusyFoward(false, objNewBusyForwardData.szTarget);
        // 写入最后一次修改配置时启用的FWD类型
        EXPECT_CALL_SetConfigInt(kszForwardTypeLastSaved, FWD_TYPE_NO_ANSWER);
        // 修改配置后广播状态变化消息
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_COMMON, objNewBusyForwardData);

        EXPECT_TRUE(pEntity->SetInfo(FWD_TYPE_BUSY, objNewBusyForwardData.bEnabled,
                                     &yl::string(objNewBusyForwardData.szTarget), NULL, FWD_CFG_SRC_TYPE_COMMON));
        EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_BUSY, &strTarget));
        EXPECT_STREQ(strTarget.c_str(), objNewBusyForwardData.szTarget);
    }

    // 修改全局模式无应答转移配置
    {
        FwdMessageStatusData objNewNoAnswerForwardData(FWD_TYPE_NO_ANSWER);
        objNewNoAnswerForwardData.bEnabled = false;
        strcpy(objNewNoAnswerForwardData.szTarget, "new_target_na");
        objNewNoAnswerForwardData.iRingDelay = 9;
        // 写入配置：Off
        EXPECT_CALL_SetConfigOfGlobalNoAnswerFoward(false, objNewNoAnswerForwardData.szTarget,
                objNewNoAnswerForwardData.iRingDelay);
        // 修改配置后广播状态变化消息
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_COMMON, objNewNoAnswerForwardData);

        EXPECT_TRUE(pEntity->SetInfo(FWD_TYPE_NO_ANSWER, objNewNoAnswerForwardData.bEnabled,
                                     &yl::string(objNewNoAnswerForwardData.szTarget),
                                     &objNewNoAnswerForwardData.iRingDelay, FWD_CFG_SRC_TYPE_COMMON));
        EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_NO_ANSWER, &strTarget, &iRingDelay));
        EXPECT_STREQ(strTarget.c_str(), objNewNoAnswerForwardData.szTarget);
        EXPECT_EQ(iRingDelay, objNewNoAnswerForwardData.iRingDelay);
    }

    delete pEntity;
    pEntity = NULL;
}

// DSSKey设置全局模式转移配置，以及还原配置
TEST_F(FwdEntityTest, SetGlobalInfoViaDsskeyAndRestore)
{
    yl::string strTarget;
    int iRingDelay = 0;

    // 全局模式FWD状态
    EXPECT_CALL_GetConfigOfGlobalAlwaysFoward(false);
    EXPECT_CALL_GetConfigOfGlobalBusyFoward(true);
    EXPECT_CALL_GetConfigOfGlobalNoAnswerFoward(false);
    // 初始化时通知FWD状态改变为On
    s_objGlobalBusyForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_STARTUP, s_objGlobalBusyForwardData);

    CFwdEntity * pEntity = new CFwdEntity(FWDDND_ACCOUNT_GLOBAL);

    // DSSKey修改全局模式无条件转移配置
    {
        FwdMessageStatusData objNewAlwaysForwardData(FWD_TYPE_ALWAYS);
        objNewAlwaysForwardData.bEnabled = true;
        strcpy(objNewAlwaysForwardData.szTarget, "new_target_a");
        // 写入配置：On
        EXPECT_CALL_SetConfigOfGlobalAlwaysFoward(true,
                objNewAlwaysForwardData.szTarget, false);
        // 写入最后一次修改配置时启用的FWD类型
        EXPECT_CALL_SetConfigInt(kszForwardTypeLastSaved, FWD_TYPE_ALWAYS | FWD_TYPE_BUSY);
        // 修改配置后广播状态变化消息
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_DSSKEY, objNewAlwaysForwardData);
        // 发送消息显示Multi-FWD提示
        EXPECT_CALL_NotifyAppEx(MULTIFWD_ENABLED_NOTICE, 0, 0,
                                (const char *)objNewAlwaysForwardData.szTarget);

        EXPECT_TRUE(pEntity->SetInfo(FWD_TYPE_ALWAYS, objNewAlwaysForwardData.bEnabled,
                                     &yl::string(objNewAlwaysForwardData.szTarget),
                                     NULL, FWD_CFG_SRC_TYPE_DSSKEY));
        EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_ALWAYS, &strTarget));
        EXPECT_STREQ(strTarget.c_str(), objNewAlwaysForwardData.szTarget);
    }
    // DSSKey还原全局模式无条件转移配置
    {
        // 读取备份配置
        EXPECT_CALL_GetConfigString(kszForwardAlwaysTargetBackup,
                                    s_objGlobalAlwaysForwardData.szTarget);
        // 写入配置：Off
        EXPECT_CALL_SetConfigOfGlobalAlwaysFoward(false,
                s_objGlobalAlwaysForwardData.szTarget, false);
        // 写入最后一次修改配置时启用的FWD类型
        EXPECT_CALL_SetConfigInt(kszForwardTypeLastSaved, FWD_TYPE_BUSY);
        // 修改配置后广播状态变化消息
        s_objGlobalAlwaysForwardData.bEnabled = false;
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_DSSKEY, s_objGlobalAlwaysForwardData);

        EXPECT_TRUE(pEntity->RestoreInfo(FWD_TYPE_ALWAYS));
        EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_ALWAYS, &strTarget));
        EXPECT_STREQ(strTarget.c_str(), s_objGlobalAlwaysForwardData.szTarget);
    }

    // 释放前会通知状态改变为Off
    s_objGlobalBusyForwardData.bEnabled = false;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_SHUTDOWN, s_objGlobalBusyForwardData);

    delete pEntity;
    pEntity = NULL;
}

// 同时还原多个全局FWD配置
TEST_F(FwdEntityTest, RestoreGlobalMultiInfo)
{
    // 全局模式FWD状态：On
    EXPECT_CALL_GetConfigOfGlobalAlwaysFoward(true);
    EXPECT_CALL_GetConfigOfGlobalBusyFoward(true);
    EXPECT_CALL_GetConfigOfGlobalNoAnswerFoward(true);
    // 初始化时通知FWD状态改变为On
    s_objGlobalAlwaysForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_STARTUP, s_objGlobalAlwaysForwardData);
    s_objGlobalBusyForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_STARTUP, s_objGlobalBusyForwardData);
    s_objGlobalNoAnswerForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_STARTUP, s_objGlobalNoAnswerForwardData);

    CFwdEntity * pEntity = new CFwdEntity(FWDDND_ACCOUNT_GLOBAL);

    // 还原所有类型转移配置
    {
        // 读取备份配置
        strcpy(s_objGlobalAlwaysForwardData.szTarget, "backup_target");
        EXPECT_CALL_GetConfigString(kszForwardAlwaysTargetBackup,
                                    s_objGlobalAlwaysForwardData.szTarget);
        // 写入配置：Off
        EXPECT_CALL_SetConfigOfGlobalAlwaysFoward(false,
                s_objGlobalAlwaysForwardData.szTarget, false);
        EXPECT_CALL_SetConfigOfGlobalBusyFoward(false,
                                                s_objGlobalBusyForwardData.szTarget);
        EXPECT_CALL_SetConfigOfGlobalNoAnswerFoward(false,
                s_objGlobalNoAnswerForwardData.szTarget,
                s_objGlobalNoAnswerForwardData.iRingDelay);
        // 修改配置后广播状态变化消息
        s_objGlobalAlwaysForwardData.bEnabled = false;
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_DSSKEY, s_objGlobalAlwaysForwardData);
        s_objGlobalBusyForwardData.bEnabled = false;
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_DSSKEY, s_objGlobalBusyForwardData);
        s_objGlobalNoAnswerForwardData.bEnabled = false;
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_DSSKEY, s_objGlobalNoAnswerForwardData);

        EXPECT_TRUE(pEntity->RestoreInfo(FWD_TYPE_ALL));
        EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_ALWAYS));
        EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_BUSY));
        EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_NO_ANSWER));
    }

    delete pEntity;
    pEntity = NULL;
}

// 设置账号模式转移状态：On->Off（手动设置，需要备份配置）
TEST_F(FwdEntityTest, SetAccountInfo)
{
    const int iAccountId = 1;
    yl::string strTarget;
    int iRingDelay = 0;

    // 账号模式FWD状态
    EXPECT_CALL_GetConfigOfAccountAlwaysFoward(true, iAccountId);
    EXPECT_CALL_GetConfigOfAccountBusyFoward(true, iAccountId);
    EXPECT_CALL_GetConfigOfAccountNoAnswerFoward(false, iAccountId);
    // 初始化时通知FWD状态改变为On
    s_objAccountAlwaysForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            iAccountId, FWD_CFG_SRC_TYPE_STARTUP, s_objAccountAlwaysForwardData);
    s_objAccountBusyForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            iAccountId, FWD_CFG_SRC_TYPE_STARTUP, s_objAccountBusyForwardData);

    CFwdEntity * pEntity = new CFwdEntity(iAccountId);

    // 修改账号模式无条件转移配置
    {
        FwdMessageStatusData objNewAlwaysForwardData(FWD_TYPE_ALWAYS);
        objNewAlwaysForwardData.bEnabled = false;
        strcpy(objNewAlwaysForwardData.szTarget, "new_target_a_");
        // 写入配置：Off
        EXPECT_CALL_SetConfigOfAccountAlwaysFoward(false, iAccountId,
                objNewAlwaysForwardData.szTarget);
        // 修改配置后广播状态变化消息
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                iAccountId, FWD_CFG_SRC_TYPE_COMMON, objNewAlwaysForwardData);

        EXPECT_TRUE(pEntity->SetInfo(FWD_TYPE_ALWAYS, objNewAlwaysForwardData.bEnabled,
                                     &yl::string(objNewAlwaysForwardData.szTarget),
                                     NULL, FWD_CFG_SRC_TYPE_COMMON));
        EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_ALWAYS, &strTarget));
        EXPECT_STREQ(strTarget.c_str(), objNewAlwaysForwardData.szTarget);
    }

    // 修改账号模式遇忙转移配置
    {
        FwdMessageStatusData objNewBusyForwardData(FWD_TYPE_BUSY);
        objNewBusyForwardData.bEnabled = false;
        strcpy(objNewBusyForwardData.szTarget, "new_target_b_");
        // 写入配置：Off
        EXPECT_CALL_SetConfigOfAccountBusyFoward(false, iAccountId, objNewBusyForwardData.szTarget);
        // 修改配置后广播状态变化消息
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                iAccountId, FWD_CFG_SRC_TYPE_COMMON, objNewBusyForwardData);

        EXPECT_TRUE(pEntity->SetInfo(FWD_TYPE_BUSY, objNewBusyForwardData.bEnabled,
                                     &yl::string(objNewBusyForwardData.szTarget), NULL, FWD_CFG_SRC_TYPE_COMMON));
        EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_BUSY, &strTarget));
        EXPECT_STREQ(strTarget.c_str(), objNewBusyForwardData.szTarget);
    }

    // 修改账号模式无应答转移配置
    {
        FwdMessageStatusData objNewNoAnswerForwardData(FWD_TYPE_NO_ANSWER);
        objNewNoAnswerForwardData.bEnabled = true;
        strcpy(objNewNoAnswerForwardData.szTarget, "new_target_na_");
        objNewNoAnswerForwardData.iRingDelay = 19;
        // 写入配置：On
        EXPECT_CALL_SetConfigOfAccountNoAnswerFoward(true, iAccountId,
                objNewNoAnswerForwardData.szTarget, objNewNoAnswerForwardData.iRingDelay);
        // 修改配置后广播状态变化消息
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                iAccountId, FWD_CFG_SRC_TYPE_COMMON, objNewNoAnswerForwardData);

        EXPECT_TRUE(pEntity->SetInfo(FWD_TYPE_NO_ANSWER, objNewNoAnswerForwardData.bEnabled,
                                     &yl::string(objNewNoAnswerForwardData.szTarget),
                                     &objNewNoAnswerForwardData.iRingDelay, FWD_CFG_SRC_TYPE_COMMON));
        EXPECT_TRUE(pEntity->GetInfo(FWD_TYPE_NO_ANSWER, &strTarget, &iRingDelay));
        EXPECT_STREQ(strTarget.c_str(), objNewNoAnswerForwardData.szTarget);
        EXPECT_EQ(iRingDelay, objNewNoAnswerForwardData.iRingDelay);

        // 释放前广播状态变化消息
        objNewNoAnswerForwardData.bEnabled = false;
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                iAccountId, FWD_CFG_SRC_TYPE_SHUTDOWN, objNewNoAnswerForwardData);
    }

    delete pEntity;
    pEntity = NULL;
}

// 同时设置多个全局FWD开关
TEST_F(FwdEntityTest, SetGlobalMultiInfo)
{
    // 全局模式FWD状态：On
    EXPECT_CALL_GetConfigOfGlobalAlwaysFoward(true);
    EXPECT_CALL_GetConfigOfGlobalBusyFoward(true);
    EXPECT_CALL_GetConfigOfGlobalNoAnswerFoward(true);
    // 初始化时通知FWD状态改变为On
    s_objGlobalAlwaysForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_STARTUP, s_objGlobalAlwaysForwardData);
    s_objGlobalBusyForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_STARTUP, s_objGlobalBusyForwardData);
    s_objGlobalNoAnswerForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_STARTUP, s_objGlobalNoAnswerForwardData);

    CFwdEntity * pEntity = new CFwdEntity(FWDDND_ACCOUNT_GLOBAL);

    // 修改无条件转移和遇忙转移开关
    {
        // 写入配置：Off
        EXPECT_CALL_SetConfigOfGlobalAlwaysFoward(false,
                s_objGlobalAlwaysForwardData.szTarget, false);
        EXPECT_CALL_SetConfigOfGlobalBusyFoward(false,
                                                s_objGlobalBusyForwardData.szTarget);
        // 写入最后一次修改配置时启用的FWD类型
        EXPECT_CALL_SetConfigInt(kszForwardTypeLastSaved, FWD_TYPE_NO_ANSWER);
        // 修改配置后广播状态变化消息
        s_objGlobalAlwaysForwardData.bEnabled = false;
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_DSSKEY, s_objGlobalAlwaysForwardData);
        s_objGlobalBusyForwardData.bEnabled = false;
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_DSSKEY, s_objGlobalBusyForwardData);

        EXPECT_TRUE(pEntity->SetMultiInfo(FWD_TYPE_ALWAYS | FWD_TYPE_BUSY, false,
                                          FWD_CFG_SRC_TYPE_DSSKEY));
        EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_ALWAYS));
        EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_BUSY));
    }

    // 释放前广播状态变化消息
    s_objGlobalNoAnswerForwardData.bEnabled = false;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            FWDDND_ACCOUNT_GLOBAL, FWD_CFG_SRC_TYPE_SHUTDOWN, s_objGlobalNoAnswerForwardData);

    delete pEntity;
    pEntity = NULL;
}

// 同时设置多个全局FWD开关
TEST_F(FwdEntityTest, SetAccountMultiInfo)
{
    const int iAccountId = 1;

    // 账号模式FWD状态
    EXPECT_CALL_GetConfigOfAccountAlwaysFoward(true, iAccountId);
    EXPECT_CALL_GetConfigOfAccountBusyFoward(false, iAccountId);
    EXPECT_CALL_GetConfigOfAccountNoAnswerFoward(true, iAccountId);
    // 初始化时通知FWD状态改变为On
    s_objAccountAlwaysForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            iAccountId, FWD_CFG_SRC_TYPE_STARTUP, s_objAccountAlwaysForwardData);
    s_objAccountNoAnswerForwardData.bEnabled = true;
    EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                            iAccountId, FWD_CFG_SRC_TYPE_STARTUP, s_objAccountNoAnswerForwardData);

    CFwdEntity * pEntity = new CFwdEntity(iAccountId);

    // 修改账号模式所有转移开关
    {
        // 写入配置：Off
        EXPECT_CALL_SetConfigOfAccountAlwaysFoward(false, iAccountId,
                s_objAccountAlwaysForwardData.szTarget);
        EXPECT_CALL_SetConfigOfAccountNoAnswerFoward(false, iAccountId,
                s_objAccountNoAnswerForwardData.szTarget, s_objAccountNoAnswerForwardData.iRingDelay);
        // 修改配置后广播状态变化消息
        s_objAccountAlwaysForwardData.bEnabled = false;
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                iAccountId, FWD_CFG_SRC_TYPE_COMMON, s_objAccountAlwaysForwardData);
        s_objAccountNoAnswerForwardData.bEnabled = false;
        EXPECT_CALL_NotifyAppEx(FWD_MESSAGE_STATUS_CHANGED,
                                iAccountId, FWD_CFG_SRC_TYPE_COMMON, s_objAccountNoAnswerForwardData);

        EXPECT_TRUE(pEntity->SetMultiInfo(FWD_TYPE_ALL, FALSE, FWD_CFG_SRC_TYPE_COMMON));
        EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_ALWAYS));
        EXPECT_FALSE(pEntity->GetInfo(FWD_TYPE_NO_ANSWER));
    }

    delete pEntity;
    pEntity = NULL;
}
