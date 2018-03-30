#include "expscannertest.h"


// 基本环境构造
class ExpScannerTest : public testing::Test
{

protected:

    MockMisc m_objMockMisc;
    MockEtlMsgHandler m_objETLMsgHandler;
    CMockUsbScanDrive m_objUsbSacndrive;
    CMockRjScanDrive m_objRjSacndrive;

public:
    virtual void SetUp()
    {
        g_pMockMisc = &m_objMockMisc;
        g_pMockEtlMsgHandler = &m_objETLMsgHandler;
        g_MockRjScanDrive = &(m_objRjSacndrive);
        g_MockUsbScanDrive = &m_objUsbSacndrive;
    }

    virtual void TearDown()
    {
        g_pMockMisc = NULL;
        g_pMockEtlMsgHandler = NULL;
        g_MockRjScanDrive = NULL;
        g_MockUsbScanDrive = NULL;
        g_ExpScanner.UnInit();
    }
};

// 构造一个已经插入exp20 的环境
class Rj1ExpPlugInTest : public ExpScannerTest
{
public:

    virtual void SetUp()
    {
        ExpScannerTest::SetUp();
        g_ExpScanner.Init(EXP_DRIVE_TYPE_RJ);

        msgObject msgRjExpIn;

        msgRjExpIn.message = EXP_IN;    // msg type
        msgRjExpIn.wParam = 0;          // exp index
        msgRjExpIn.lParam = DT_EXP_20;  // exp type

        //EXPECT_CALL(*g_MockRjScanDrive, InitExpByIndex(0)).Times(testing::AtLeast(1));


        TriggerEtlMsgCallback(msgRjExpIn);

        EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 1);
        EXPECT_EQ(DT_EXP_20, g_ExpScanner.GetCurrentExpType());
    }

    virtual void TearDown()
    {
        g_ExpScanner.UnInit();

        ExpScannerTest::TearDown();
    }

};

// 构造一个已经插入exp50 的环境
class Usb1ExpPlugInTest : public ExpScannerTest
{
public:

    virtual void SetUp()
    {
        ExpScannerTest::SetUp();
        g_ExpScanner.Init(EXP_DRIVE_TYPE_USB);

        msgObject msgRjExpIn;

        msgRjExpIn.message = EXP_IN;    // msg type
        msgRjExpIn.wParam = 0;          // exp index
        msgRjExpIn.lParam = DT_EXP_50;  // exp type

        //EXPECT_CALL(*g_MockRjScanDrive, InitExpByIndex(0)).Times(testing::AtLeast(1));

        TriggerEtlMsgCallback(msgRjExpIn);
    }

    virtual void TearDown()
    {
        g_ExpScanner.UnInit();

        ExpScannerTest::TearDown();
    }

};

//  构造 5 台  exp连接的环境
class Rj5ExpPlugInTest : public Rj1ExpPlugInTest
{
public:
    virtual void SetUp()
    {
        Rj1ExpPlugInTest::SetUp();

        FAKE_CALL_MSG(EXP_IN, 1, DT_EXP_20);
        FAKE_CALL_MSG(EXP_IN, 2, DT_EXP_20);
        FAKE_CALL_MSG(EXP_IN, 3, DT_EXP_20);
        FAKE_CALL_MSG(EXP_IN, 4, DT_EXP_20);

    }

    virtual void TearDown()
    {
        /*FAKE_CALL_MSG(EXP_OUT, 4, DT_EXP_20);
        FAKE_CALL_MSG(EXP_OUT, 3, DT_EXP_20);
        FAKE_CALL_MSG(EXP_OUT, 2, DT_EXP_20);
        FAKE_CALL_MSG(EXP_OUT, 1, DT_EXP_20);*/

        Rj1ExpPlugInTest::TearDown();
    }

};

// 测试Expscanner 初始化情况
TEST_F(ExpScannerTest, InitScanner)
{

    CScannner * pScanner = &(g_ExpScanner);

    ASSERT_THAT(pScanner, testing::NotNull());

    ON_CALL(*g_pMockEtlMsgHandler, RegisterMsgHandle(testing::_, testing::_,
            testing::_)).WillByDefault(testing::Return(TRUE));
    EXPECT_CALL(*g_pMockEtlMsgHandler, RegisterMsgHandle(testing::_, testing::_, testing::_)).Times(5);

    /*ON_CALL(*g_pMockEtlMsgHandler, UnregisterMsgHandle(testing::_, testing::_, testing::_)).WillByDefault(testing::Return(TRUE));
    EXPECT_CALL(*g_pMockEtlMsgHandler, UnregisterMsgHandle(testing::_, testing::_, testing::_)).Times(5);*/

    EXPECT_CALL(*g_MockRjScanDrive, StartKeyScan()).Times(testing::AtLeast(1));
    EXPECT_CALL(*g_MockRjScanDrive, StartInsertScan()).Times(testing::AtLeast(1));
    EXPECT_CALL(*g_MockRjScanDrive, InitDrive()).Times(testing::AtLeast(1));

    pScanner->Init(EXP_DRIVE_TYPE_RJ);

    EXPECT_EQ(pScanner->GetEhsIndex(), 0);
    EXPECT_EQ(pScanner->GetConnectedExpCounts(), 0);
    EXPECT_EQ(pScanner->IsTestMode(), false);
    EXPECT_EQ(pScanner->GetCurrentExpType(), DT_INVALID);

    pScanner->UnInit();
}
// 测试插入一台exp20的情况
TEST_F(ExpScannerTest, RjExpIn)
{
    g_ExpScanner.Init(EXP_DRIVE_TYPE_RJ);

    // fake a msg first
    msgObject msgRjExpIn;

    msgRjExpIn.message = EXP_IN;    // msg type
    msgRjExpIn.wParam = 0;          // exp index
    msgRjExpIn.lParam = DT_EXP_20;  // exp type

    EXPECT_CALL(*g_MockRjScanDrive, InitExpByIndex(0)).Times(testing::AtLeast(1));
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerSetThreadTimer1(_, _, _, _));
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerKillThreadTimer(_));
    TriggerEtlMsgCallback(msgRjExpIn);

    EXPECT_EQ(g_ExpScanner.GetEhsIndex(), 0);
    EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 1);
    EXPECT_EQ(g_ExpScanner.IsTestMode(), false);
    EXPECT_EQ(DT_EXP_20, g_ExpScanner.GetCurrentExpType());
    EXPECT_EQ(g_ExpScanner.GetCurrentPageByExpIndex(0), 0);
}



// 测试拔出一台exp20的情况
TEST_F(ExpScannerTest, RjExpOut)
{
    g_ExpScanner.Init(EXP_DRIVE_TYPE_RJ);

    msgObject msgRjExpOut;

    msgRjExpOut.message = EXP_OUT;  // msg type
    msgRjExpOut.wParam = 0;         // exp index
    msgRjExpOut.lParam = DT_EXP_20; // exp type

    TriggerEtlMsgCallback(msgRjExpOut);

    EXPECT_EQ(g_ExpScanner.GetEhsIndex(), 0);
    EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 0);
    EXPECT_EQ(g_ExpScanner.IsTestMode(), false);
    EXPECT_EQ(DT_INVALID, g_ExpScanner.GetCurrentExpType());
    EXPECT_EQ(g_ExpScanner.GetCurrentPageByExpIndex(0), 0);

    g_ExpScanner.UnInit();
}


// 测试插入一台exp50的情况
TEST_F(ExpScannerTest, UsbExpIn)
{
    g_ExpScanner.Init(EXP_DRIVE_TYPE_USB);



    EXPECT_CALL(*g_MockUsbScanDrive, InitExpByIndex(0)).Times(testing::AtLeast(1));
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerSetThreadTimer1(_, _, _, _)).Times(0);
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerKillThreadTimer(_)).Times(0);

    // fake a msg first
    msgObject msgRjExpIn;
    msgRjExpIn.message = EXP_IN;    // msg type
    msgRjExpIn.wParam = 0;          // exp index
    msgRjExpIn.lParam = DT_EXP_50;  // exp type
    TriggerEtlMsgCallback(msgRjExpIn);


    EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 1);
    EXPECT_EQ(g_ExpScanner.IsTestMode(), false);
    EXPECT_EQ(DT_EXP_50, g_ExpScanner.GetCurrentExpType());
    EXPECT_EQ(g_ExpScanner.GetCurrentPageByExpIndex(0), 0);
}


// 测试拔出一台exp50的情况
TEST_F(Usb1ExpPlugInTest, UsbExpOut)
{
    g_ExpScanner.Init(EXP_DRIVE_TYPE_USB);

    msgObject msgRjExpOut;

    msgRjExpOut.message = EXP_OUT;  // msg type
    msgRjExpOut.wParam = 0;         // exp index
    msgRjExpOut.lParam = DT_EXP_50; // exp type

    TriggerEtlMsgCallback(msgRjExpOut);

    EXPECT_EQ(g_ExpScanner.GetEhsIndex(), -1);
    EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 0);
    EXPECT_EQ(g_ExpScanner.IsTestMode(), false);
    EXPECT_EQ(DT_INVALID, g_ExpScanner.GetCurrentExpType());
    EXPECT_EQ(g_ExpScanner.GetCurrentPageByExpIndex(0), 0);

    g_ExpScanner.UnInit();
}

// 测试没有exp的情况下，收到了exp拔出信号
TEST_F(ExpScannerTest, RjExpOutWithPlugIn)
{
    g_ExpScanner.Init(EXP_DRIVE_TYPE_RJ);

    FAKE_CALL_MSG(EXP_OUT, 0, DT_EXP_39);

    EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 0);

    EXPECT_EQ(DT_INVALID, g_ExpScanner.GetCurrentExpType());

}

// 测试ehs插入的情况
TEST_F(ExpScannerTest, EhsPlugIn)
{
    g_ExpScanner.Init(EXP_DRIVE_TYPE_RJ);

    EXPECT_CALL(*g_pMockEtlMsgHandler, msgPostMsgToThreadEx(_, DEV_MSG_EHS_INSERT, 0, _, _,
                _)).Times(1);

    FAKE_CALL_MSG(EXP_IN, 0, DT_EXP_EHS);

    EXPECT_EQ(g_ExpScanner.GetEhsIndex(), 0);
    EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 0);
    EXPECT_EQ(DT_EXP_EHS, g_ExpScanner.GetCurrentExpType());
}


// 测试ehs拔出的情况

TEST_F(ExpScannerTest, EhsPlugOut)
{
    g_ExpScanner.Init(EXP_DRIVE_TYPE_RJ);

    EXPECT_CALL(*g_pMockEtlMsgHandler, msgPostMsgToThreadEx(_, DEV_MSG_EHS_INSERT, 0, _, _,
                _)).Times(1);

    FAKE_CALL_MSG(EXP_IN, 0, DT_EXP_EHS);

    EXPECT_EQ(g_ExpScanner.GetEhsIndex(), 0);
    EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 0);
    EXPECT_EQ(DT_EXP_EHS, g_ExpScanner.GetCurrentExpType());
    EXPECT_CALL(*g_pMockEtlMsgHandler, msgPostMsgToThreadEx(_, DEV_MSG_EHS_REMOVE, 0, _, _,
                _)).Times(1);

    FAKE_CALL_MSG(EXP_OUT, 0, DT_EXP_EHS);

    EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 0);
}



// 在一个已经插入exp20 的环境 再进行测试
// 再插入一台exp20
TEST_F(Rj1ExpPlugInTest, 1MoreExpPlugIn)
{
    EXPECT_CALL(*g_MockRjScanDrive, InitExpByIndex(1)).Times(testing::AtLeast(1));
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerSetThreadTimer1(_, _, _, _));
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerKillThreadTimer(_));

    FAKE_CALL_MSG(EXP_IN, 1, DT_EXP_20);

    EXPECT_EQ(g_ExpScanner.GetEhsIndex(), 0);
    EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 2);
    EXPECT_EQ(g_ExpScanner.IsTestMode(), false);
    EXPECT_EQ(DT_EXP_20, g_ExpScanner.GetCurrentExpType());
    EXPECT_EQ(g_ExpScanner.GetCurrentPageByExpIndex(0), 0);
}


// 在一个已经插入exp20 的环境 再进行测试
// 再插入一台exp39
TEST_F(Rj1ExpPlugInTest, 1MoreExp39PlugIn)
{
    EXPECT_CALL(*g_MockRjScanDrive, InitExpByIndex(1)).Times(0);
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerSetThreadTimer1(_, _, _, _)).Times(0);
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerKillThreadTimer(_)).Times(0);

    FAKE_CALL_MSG(EXP_IN, 1, DT_EXP_39);

    EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 1);

    EXPECT_EQ(DT_EXP_20, g_ExpScanner.GetCurrentExpType());

}

// 在一个已经插入exp20 的环境 再进行测试
// 再插入一台exp39, 再拔出
TEST_F(Rj1ExpPlugInTest, 1MoreExp39PlugOut)
{
    EXPECT_CALL(*g_MockRjScanDrive, InitExpByIndex(1)).Times(0);
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerSetThreadTimer1(_, _, _, _)).Times(0);
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerKillThreadTimer(_)).Times(0);

    FAKE_CALL_MSG(EXP_IN, 1, DT_EXP_39);

    EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 1);

    EXPECT_EQ(DT_EXP_20, g_ExpScanner.GetCurrentExpType());

    FAKE_CALL_MSG(EXP_OUT, 1, DT_EXP_39);

    EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 1);

    EXPECT_EQ(DT_EXP_20, g_ExpScanner.GetCurrentExpType());

}

// 在一个已经插入exp50 的环境 再进行测试
// 再插入一台exp50
TEST_F(Usb1ExpPlugInTest, 1MoreUsbExpPlugIn)
{
    EXPECT_CALL(*g_MockUsbScanDrive, InitExpByIndex(1)).Times(testing::AtLeast(1));
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerSetThreadTimer1(_, _, _, _)).Times(0);
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerKillThreadTimer(_)).Times(0);

    FAKE_CALL_MSG(EXP_IN, 1, DT_EXP_50);

    EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 2);
    EXPECT_EQ(g_ExpScanner.IsTestMode(), false);
    EXPECT_EQ(DT_EXP_50, g_ExpScanner.GetCurrentExpType());
    EXPECT_EQ(g_ExpScanner.GetCurrentPageByExpIndex(0), 0);
    EXPECT_EQ(g_ExpScanner.GetCurrentPageByExpIndex(1), 0);
}


// 连接5台exp基本测试
TEST_F(Rj5ExpPlugInTest, 5RjexpBasicTest)
{
    EXPECT_EQ(5, g_ExpScanner.GetConnectedExpCounts());
    EXPECT_EQ(DT_EXP_20, g_ExpScanner.GetCurrentExpType());
    EXPECT_EQ(0, g_ExpScanner.GetCurrentPageByExpIndex(0));
    EXPECT_EQ(0, g_ExpScanner.GetCurrentPageByExpIndex(1));
    EXPECT_EQ(0, g_ExpScanner.GetCurrentPageByExpIndex(5));
}

// 连接5台exp , 再接入1 台 exp 20
TEST_F(Rj5ExpPlugInTest, 5RjexpTestWith1MoreExp20)
{

    EXPECT_CALL(*g_MockRjScanDrive, InitExpByIndex(5)).Times(0);
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerSetThreadTimer1(_, _, _, _)).Times(0);
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerKillThreadTimer(_)).Times(0);

    FAKE_CALL_MSG(EXP_IN, 5, DT_EXP_20);

    EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 5);
    EXPECT_EQ(DT_EXP_20, g_ExpScanner.GetCurrentExpType());
}

// 连接5台exp , 再接入1 台 exp 39
TEST_F(Rj5ExpPlugInTest, 5RjexpTestWith1MoreExp39)
{

    EXPECT_CALL(*g_MockRjScanDrive, InitExpByIndex(1)).Times(0);
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerSetThreadTimer1(_, _, _, _)).Times(0);
    EXPECT_CALL(*g_pMockEtlMsgHandler, timerKillThreadTimer(_)).Times(0);

    FAKE_CALL_MSG(EXP_IN, 5, DT_EXP_39);

    EXPECT_EQ(g_ExpScanner.GetConnectedExpCounts(), 5);
    EXPECT_EQ(DT_EXP_20, g_ExpScanner.GetCurrentExpType());
}


//
//// 按键测试
//// 无mock定时器，长按部分暂时无法测试
////
//TEST_F(Rj1ExpPlugInTest, KeyPressTest)
//{
//  int dssId = FakeDssId(DT_EXP_20, EXP40_KEY_1, 0);
//  EXPECT_CALL(*g_pMockEtlMsgHandler, msgPostMsgToThreadEx(_, DEV_MSG_EXP_KEYDOWN, dssId, _, _, _)).Times(1);
//  EXPECT_CALL(*g_pMockEtlMsgHandler, msgPostMsgToThreadEx(_, DEV_MSG_EXP_KEYUP, dssId, _, _, _)).Times(1);
//  FAKE_CALL_MSG(EXP_KEY_DOWN, dssId, DT_EXP_20);
//  FAKE_CALL_MSG(EXP_KEY_UP, dssId, DT_EXP_20);
//
//  EXPECT_EQ(g_ExpScanner.GetCurrentPageByExpIndex(0), 0);
//
//
//}
//
//// 测试上层手工翻页之后，获取的dssId 是否正确
////
//TEST_F(Rj1ExpPlugInTest, KeyPressTestWithPageInfo)
//{
//  EXPECT_EQ(g_ExpScanner.GetCurrentPageByExpIndex(0), 0);
//
//  g_ExpScanner.SetCurrentPageByIndex(0, 1);
//
//  EXPECT_EQ(g_ExpScanner.GetCurrentPageByExpIndex(0), 1);
//
//  int dssId = FakeDssId(DT_EXP_20, EXP40_KEY_1, 0);
//  EXPECT_CALL(*g_pMockEtlMsgHandler, msgPostMsgToThreadEx(_, DEV_MSG_EXP_KEYDOWN, dssId + 20, _, _, _)).Times(1);
//  EXPECT_CALL(*g_pMockEtlMsgHandler, msgPostMsgToThreadEx(_, DEV_MSG_EXP_KEYUP, dssId + 20, _, _, _)).Times(1);
//  FAKE_CALL_MSG(EXP_KEY_DOWN, dssId, DT_EXP_20);
//  FAKE_CALL_MSG(EXP_KEY_UP, dssId, DT_EXP_20);
//
//}
//
//// 测试翻页键是否正常
//
//TEST_F(Rj1ExpPlugInTest,PageKeyPressTest)
//{
//  EXPECT_EQ(g_ExpScanner.GetCurrentPageByExpIndex(0), 0);
//
//  int dssId = FakeDssId(DT_EXP_20, EXP40_KEY_PAGE2, 0);
//
//  EXPECT_CALL(*g_pMockEtlMsgHandler, msgPostMsgToThreadEx(_, EXP_MESSAGE_SWITCH_PAGE, 0, 1, _, _)).Times(1);
//
//  FAKE_CALL_MSG(EXP_KEY_DOWN, dssId, DT_EXP_20);
//  FAKE_CALL_MSG(EXP_KEY_UP, dssId, DT_EXP_20);
//
//  EXPECT_EQ(1, g_ExpScanner.GetCurrentPageByExpIndex(0));
//
//}
//
//// 测试 testmode 的dssid
//TEST_F(Rj1ExpPlugInTest, KeyPressTestInExpTestMode)
//{
//
//  EXPECT_EQ(g_ExpScanner.GetCurrentPageByExpIndex(0), 0);
//
//  EXPECT_EQ(g_ExpScanner.IsTestMode(), false);
//
//  g_ExpScanner.SetTestMode(true);
//  g_ExpScanner.SetCurrentPageByIndex(0, 1);
//
//  EXPECT_EQ(g_ExpScanner.IsTestMode(), true);
//  EXPECT_EQ(g_ExpScanner.GetCurrentPageByExpIndex(0), 1);
//
//  int dssId = FakeDssId(DT_EXP_20, EXP40_KEY_1, 0);
//
//  EXPECT_CALL(*g_pMockEtlMsgHandler, msgPostMsgToThreadEx(_, DEV_MSG_EXP_KEYDOWN, dssId, _, _, _)).Times(1);
//
//  FAKE_CALL_MSG(EXP_KEY_DOWN, dssId, DT_EXP_20);
//  FAKE_CALL_MSG(EXP_KEY_UP, dssId, DT_EXP_20);
//
//}
