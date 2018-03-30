#ifndef __FWD_ENTITY_TEST_H__
#define __FWD_ENTITY_TEST_H__

#include <gtest/gtest.h>

#include "fwddndmsgdefine.h"

#include "mocklibs/configparser/mockconfigparser.h"
#include "mocklibs/etlmsghandler/mocketlmsghandler.h"

class FwdEntityTest : public testing::Test
{
public:
    static void SetUpTestCase();

protected:
    MockConfigParser m_objMockConfigParser;
    MockEtlMsgHandler m_objMockEtlMsgHandler;

    static FwdMessageStatusData s_objGlobalAlwaysForwardData;
    static FwdMessageStatusData s_objGlobalBusyForwardData;
    static FwdMessageStatusData s_objGlobalNoAnswerForwardData;
    static FwdMessageStatusData s_objAccountAlwaysForwardData;
    static FwdMessageStatusData s_objAccountBusyForwardData;
    static FwdMessageStatusData s_objAccountNoAnswerForwardData;

    virtual void SetUp();
    virtual void TearDown();

    void EXPECT_CALL_GetConfigOfGlobalAlwaysFoward(
        bool bEnable,
        const yl::string & strTarget = s_objGlobalAlwaysForwardData.szTarget);
    void EXPECT_CALL_GetConfigOfGlobalBusyFoward(
        bool bEnable,
        const yl::string & strTarget = s_objGlobalBusyForwardData.szTarget);
    void EXPECT_CALL_GetConfigOfGlobalNoAnswerFoward(
        bool bEnable,
        const yl::string & strTarget = s_objGlobalNoAnswerForwardData.szTarget,
        int iRingDelay = s_objGlobalNoAnswerForwardData.iRingDelay);

    void EXPECT_CALL_SetConfigOfGlobalAlwaysFoward(bool bEnable,
            const yl::string & strTarget, bool bBackup);
    void EXPECT_CALL_SetConfigOfGlobalBusyFoward(bool bEnable,
            const yl::string & strTarget);
    void EXPECT_CALL_SetConfigOfGlobalNoAnswerFoward(bool bEnable,
            const yl::string & strTarget, int iRingDelay);

    void EXPECT_CALL_GetConfigOfAccountAlwaysFoward(
        bool bEnable,
        int iAccountId,
        const yl::string & strTarget = s_objAccountAlwaysForwardData.szTarget);
    void EXPECT_CALL_GetConfigOfAccountBusyFoward(
        bool bEnable,
        int iAccountId,
        const yl::string & strTarget = s_objAccountBusyForwardData.szTarget);
    void EXPECT_CALL_GetConfigOfAccountNoAnswerFoward(
        bool bEnable,
        int iAccountId,
        const yl::string & strTarget = s_objAccountNoAnswerForwardData.szTarget,
        int iRingDelay = s_objAccountNoAnswerForwardData.iRingDelay);

    void EXPECT_CALL_SetConfigOfAccountAlwaysFoward(bool bEnable, int iAccountId,
            const yl::string & strTarget);
    void EXPECT_CALL_SetConfigOfAccountBusyFoward(bool bEnable, int iAccountId,
            const yl::string & strTarget);
    void EXPECT_CALL_SetConfigOfAccountNoAnswerFoward(bool bEnable, int iAccountId,
            const yl::string & strTarget, int iRingDelay);
};

bool operator==(const FwdMessageStatusData & lhs, const FwdMessageStatusData & rhs);

#endif // !__FWD_ENTITY_TEST_H__
