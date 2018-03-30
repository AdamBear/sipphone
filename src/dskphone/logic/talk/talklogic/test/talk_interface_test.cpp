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

void FuncExitAllSession()
{
    _SessionManager.ExitAllSession();
}

class IsPrimeParamTest : public::testing::TestWithParam<int>
{

};

INSTANTIATE_TEST_CASE_P(TrueReturn, IsPrimeParamTest, testing::Range(-1, 6, 1));

TEST_P(IsPrimeParamTest, talklogic_EnterDialUI)
{
    FuncExitAllSession();

    yl::string strNumber = "123";
    int iAccountId = GetParam();
    bool bSwitchChannel = true;
    TALK_CALLOUT_TYPE eCallOutType = TCT_NORMAL;

    talklogic_EnterDialUI(strNumber, iAccountId, bSwitchChannel, eCallOutType);

    EXPECT_EQ(_SessionManager.GetSessionListSize(), 1);

    CBaseSession * pSession = _SessionManager.GetFocusedSession();
    ASSERT_TRUE(pSession != NULL);
    EXPECT_EQ(pSession->GetSessionState(), SESSION_DIALING);

    CCallInfo * pCallInfo = pSession->GetCallInfoAsSub();
    ASSERT_TRUE(pCallInfo != NULL);
    EXPECT_EQ(pCallInfo->m_iSipAccount, iAccountId);

    FuncExitAllSession();
    EXPECT_EQ(_SessionManager.GetSessionListSize(), 0);
}

// Test begin
/*
bool talklogic_EnterDialUI(const yl::string& strNumber, int iAccountId = AUTO_SELECT_LINE_ID,
bool bSwitchChannel = true, TALK_CALLOUT_TYPE eCallOutType = TCT_NORMAL);
*/
TEST(TalkTest, talklogic_EnterDialUI)
{
    FuncExitAllSession();

    yl::string strNumber = "123";
    int iAccountId = 0;
    bool bSwitchChannel = true;
    TALK_CALLOUT_TYPE eCallOutType = TCT_NORMAL;

    talklogic_EnterDialUI(strNumber, iAccountId, bSwitchChannel, eCallOutType);

    EXPECT_EQ(_SessionManager.GetSessionListSize(), 1);

    CBaseSession * pSession = _SessionManager.GetFocusedSession();
    ASSERT_TRUE(pSession != NULL);
    EXPECT_EQ(pSession->GetSessionState(), SESSION_DIALING);

    CCallInfo * pCallInfo = pSession->GetCallInfoAsSub();
    ASSERT_TRUE(pCallInfo != NULL);
    EXPECT_EQ(pCallInfo->m_iSipAccount, iAccountId);

    FuncExitAllSession();
    EXPECT_EQ(_SessionManager.GetSessionListSize(), 0);
}

/*
void talklogic_EnterPredialUI(int iDigitKey, int iAccountId = AUTO_SELECT_LINE_ID);
*/
TEST(TalkTest, talklogic_EnterPredialUI)
{
    FuncExitAllSession();

    int iDigitKey = PHONE_KEY_0;
    int iAccountId = 0;

    talklogic_EnterPredialUI(iDigitKey, iAccountId);

    EXPECT_EQ(_SessionManager.GetSessionListSize(), 1);

    CBaseSession * pSession = _SessionManager.GetFocusedSession();
    ASSERT_TRUE(pSession != NULL);
    EXPECT_EQ(pSession->GetSessionState(), SESSION_PREDIAL);

    FuncExitAllSession();
    EXPECT_EQ(_SessionManager.GetSessionListSize(), 0);
}

#ifdef IF_FEATURE_BROADSOFT_CALLPARK
/*
void talklogic_EnterRetrieveParkUI(const yl::string& strNumber, int iAccountId = AUTO_SELECT_LINE_ID);
*/
TEST(TalkTest, talklogic_EnterRetrieveParkUI)
{
    FuncExitAllSession();

    yl::string strNumber = "123";
    int iAccountId = 0;

    talklogic_EnterRetrieveParkUI(strNumber, iAccountId);

    EXPECT_EQ(_SessionManager.GetSessionListSize(), 1);

    CBaseSession * pSession = _SessionManager.GetFocusedSession();
    ASSERT_TRUE(pSession != NULL);
    EXPECT_EQ(pSession->GetSessionState(), SESSION_PRE_RETRIEVEPARK);

    CCallInfo * pCallInfo = pSession->GetCallInfoAsSub();
    ASSERT_TRUE(pCallInfo != NULL);
    EXPECT_EQ(pCallInfo->m_iSipAccount, iAccountId);

    FuncExitAllSession();
    EXPECT_EQ(_SessionManager.GetSessionListSize(), 0);
}
#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK

/*
bool talklogic_CallOut(void* pDsskey);
*/
TEST(TalkTest, talklogic_CallOut1)
{
    FuncExitAllSession();

    // how?
}

/*
bool talklogic_CallOut(const yl::string& strNumber, const yl::string& strName,
int iAccountId = AUTO_SELECT_LINE_ID, CallOut_Param* pCallParam = NULL);
*/
TEST(TalkTest, talklogic_CallOut2)
{
    FuncExitAllSession();

    yl::string strNumber = "123";
    yl::string strName = "name1";
    int iAccountId = 0;
    CallOut_Param sParam;

    talklogic_CallOut(strNumber, strName, iAccountId, &sParam);

    EXPECT_EQ(_SessionManager.GetSessionListSize(), 1);

    CBaseSession * pSession = _SessionManager.GetFocusedSession();
    ASSERT_TRUE(pSession != NULL);
    EXPECT_EQ(pSession->GetSessionState(), SESSION_CONNECTING);

    CCallInfo * pCallInfo = pSession->GetCallInfoAsSub();
    ASSERT_TRUE(pCallInfo != NULL);
    EXPECT_EQ(pCallInfo->m_iSipAccount, iAccountId);

    FuncExitAllSession();
    EXPECT_EQ(_SessionManager.GetSessionListSize(), 0);
}
