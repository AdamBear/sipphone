#include "../src/statusmanager.h"
#include "../src/statusmonitor.h"
#include "modidlescreen.h"
#if LINUX_SYSTEM
#include "gtest/gtest.h"


/*****************
模拟函数 Start
****************/
mkit_handle_t thisThread;
extern void idleScreen_RegisterStatus();
LRESULT CALLBACK idle_test_msg(msgObject * pMessage);

void commonUnits_ResetToFatory(ResetOptionType eOption) {}

LRESULT inline SendToAccount(UINT uMsg, WPARAM wParam, LPARAM lParam, int ex_size = 0,
                             void * ex = NULL)
{
    msgObject msg;
    msg.message = uMsg;
    msg.wParam = wParam;
    msg.lParam = lParam;
    msg.ResetExtraData(ex_size, ex);
    return acc_AccountOnMessage(msg);
}

#define msgSendMsgToThread(PASS, MSG, WPRM, LPRM) SendToThis(MSG, WPRM, LPRM)
LRESULT inline SendToThis(UINT uMsg, WPARAM wParam, LPARAM lParam, int ex_size = 0,
                          void * ex = NULL)
{
    msgObject msg;
    msg.message = uMsg;
    msg.wParam = wParam;
    msg.lParam = lParam;
    msg.ResetExtraData(ex_size, ex);
    return idle_test_msg(&msg);
}
//刷新账号配置
//开关账号 Enable/Disable
void inline reconfig_account(int iAccount)
{
    SendToAccount(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, iAccount);
}

//开关账号 Enable/Disable
void inline turn_account(int iAccount, bool bEnable)
{
    configParser_SetCfgItemIntValue(kszAccountEnable, iAccount, bEnable, CONFIG_LEVEL_PHONE);
    reconfig_account(iAccount);
}

//重置账号，Disable所有账号
void inline reset_account()
{
    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        turn_account(i, false);
    }
}


//注册账号
void inline register_account(int iAccount)
{
    if (acc_IsAccountEnable(iAccount) == false)
    {
        turn_account(iAccount, true);
    }
    SendToAccount(SIP_REG_UPDATE_REGISTER, iAccount, LS_REGISTERED);
}

/*****************
模拟函数 End
****************/


/******************************
TEST Start
*******************************/
TEST(idleScreenTest, SetStatusTest)
{
    _StatusManager.reset();
    _StatusManager.TurnNotify(false);
    idleScreen_RegisterStatus();

    //非固定状态测试
    idleScreen_SetStatus(PS_STATE_VPN);
    idleScreen_SetStatus(PS_STATE_MISSCALL);

    idleScreen_SetNotifyFunction(NULL, 5);
    YLVector<IdleStatusItem> topList = idleScreen_GetStatusList();
    //应该返回两个状态
    ASSERT_EQ(2, topList.size());
    //应该为后到状态位于链表前端
    EXPECT_EQ(PS_STATE_MISSCALL, topList[0].m_nId);
    EXPECT_EQ(PS_STATE_VPN, topList[1].m_nId);

    //固定状态测试
    const int TestFixedStatus = 0x0000e0b2;

    //注册固定状态
    _StatusManager.RegisterStatus(TestFixedStatus, POPUP_WINDOW_NO_POPUP, 1);
    idleScreen_SetStatus(TestFixedStatus);
    idleScreen_SetStatus(PS_STATE_TEXTMESSAGE);

    //获取目前状态
    topList = idleScreen_GetStatusList();
    //应该返回4个状态
    ASSERT_EQ(4, topList.size());
    //固定状态应该在最前面
    EXPECT_EQ(TestFixedStatus, topList[0].m_nId);
    EXPECT_EQ(PS_STATE_TEXTMESSAGE, topList[1].m_nId);
    EXPECT_EQ(PS_STATE_MISSCALL, topList[2].m_nId);
    EXPECT_EQ(PS_STATE_VPN, topList[3].m_nId);

    //刷新已有状态
    idleScreen_SetStatus(PS_STATE_VPN);

    //应该有四个状态
    topList = idleScreen_GetStatusList();
    ASSERT_EQ(4, topList.size());

    EXPECT_EQ(TestFixedStatus, topList[0].m_nId);
    EXPECT_EQ(PS_STATE_VPN, topList[1].m_nId);
    EXPECT_EQ(PS_STATE_TEXTMESSAGE, topList[2].m_nId);
    EXPECT_EQ(PS_STATE_MISSCALL, topList[3].m_nId);
}

TEST(idleScreenTest, RemoveStatusTest)
{
    _StatusManager.reset();
    _StatusManager.TurnNotify(false);
    idleScreen_SetNotifyFunction(NULL, 5);
    idleScreen_RegisterStatus();

    //添加非固定状态测试
    idleScreen_SetStatus(PS_STATE_VPN);
    idleScreen_SetStatus(PS_STATE_MISSCALL);
    idleScreen_SetStatus(PS_STATE_MUTE);

    YLVector<IdleStatusItem> topList = idleScreen_GetStatusList();
    //应该返回3个状态
    ASSERT_EQ(3, topList.size());

    //删除测试
    idleScreen_DropStatus(PS_STATE_MISSCALL);
    topList = idleScreen_GetStatusList();
    //应该返回2个状态
    ASSERT_EQ(2, topList.size());
    EXPECT_EQ(PS_STATE_MUTE, topList[0].m_nId);
    EXPECT_EQ(PS_STATE_VPN, topList[1].m_nId);
}

YLVector<IdleStatusItem> vecStatus;
void NotifiedFunction(const YLVector<IdleStatusItem> & status)
{
    vecStatus = status;
}

TEST(idleScreenTest, NotifyTest)
{
    _StatusManager.reset();
    _StatusManager.TurnNotify(false);
    idleScreen_SetNotifyFunction(NotifiedFunction, 5);
    idleScreen_RegisterStatus();

    //添加非固定状态
    idleScreen_SetStatus(PS_STATE_VPN);
    idleScreen_SetStatus(PS_STATE_MISSCALL);
    idleScreen_SetStatus(PS_STATE_MUTE);
    //通知未打开
    EXPECT_EQ(0, vecStatus.size());

    //打开通知
    idleScreen_TurnStatusNotify(true);
    EXPECT_EQ(3, vecStatus.size());

    //添加状态
    idleScreen_SetStatus(PS_STATE_TEXTMESSAGE);
    EXPECT_EQ(4, vecStatus.size());
}

TEST(idleScreenTest, StatusPopupTest)
{
    _StatusManager.reset();
    _StatusManager.TurnNotify(false);
    idleScreen_SetNotifyFunction(NotifiedFunction, 5);
    idleScreen_RegisterStatus();

    idleScreen_SetStatus(PS_STATE_NETWORK_FAILED);
    YLVector<int> popStatus = idleScreen_GetStatusToPopup();
    ASSERT_EQ(1, popStatus.size());
    EXPECT_EQ(PS_STATE_NETWORK_FAILED, popStatus[0]);

    idleScreen_SetPopupStatus(PS_STATE_NETWORK_FAILED, POPUP_WINDOW_NO_POPUP);
    EXPECT_EQ(POPUP_WINDOW_NO_POPUP, idleScreen_GetPopupStatus(PS_STATE_NETWORK_FAILED));

    popStatus = idleScreen_GetStatusToPopup();
    ASSERT_EQ(0, popStatus.size());
}

TEST(idleScreenTest, msgAATest)
{
    _StatusManager.reset();
    _StatusManager.TurnNotify(false);
    idleScreen_SetNotifyFunction(NotifiedFunction, 5);
    idleScreen_RegisterStatus();

    //注册两个账号
    turn_account(0, true);
    turn_account(1, true);
    register_account(0);
    register_account(1);

    //打开默认账号的AA
    acc_SetDefaultAccount(0);
    configParser_SetCfgItemIntValue(kszAutoAnswerSwitch, 0, 1, CONFIG_LEVEL_PHONE);
    msgSendMsgToThread(thisThread, CONFIG_MSG_BCAST_CHANGED, ST_AUTOANSWER, 0);

    YLVector<IdleStatusItem> vecStatus = idleScreen_GetStatusList();
    ASSERT_EQ(1, vecStatus.size());
    EXPECT_EQ(PS_STATE_AA, vecStatus[0].m_nId);

    //切换到没打开AA的账号
    configParser_SetCfgItemIntValue(kszAutoAnswerSwitch, 1, 0, CONFIG_LEVEL_PHONE);
    acc_SwitchDefaultAccount();
    ASSERT_EQ(1, acc_GetDefaultAccount());
    vecStatus = idleScreen_GetStatusList();
    EXPECT_EQ(0, vecStatus.size());

    //默认账号切换成AA账号
    acc_SwitchDefaultAccount();
    ASSERT_EQ(0, acc_GetDefaultAccount());
    vecStatus = idleScreen_GetStatusList();
    EXPECT_EQ(1, vecStatus.size());

    //关闭默认账号的AA
    configParser_SetCfgItemIntValue(kszAutoAnswerSwitch, 0, 0, CONFIG_LEVEL_PHONE);
    msgSendMsgToThread(thisThread, CONFIG_MSG_BCAST_CHANGED, ST_AUTOANSWER, 0);
    vecStatus = idleScreen_GetStatusList();
    EXPECT_EQ(0, vecStatus.size());
}

TEST(idleScreenTest, MuteTest)
{
    _StatusManager.reset();
    idleScreen_RegisterStatus();

    idleScreen_SetStatus(PS_STATE_RING_MUTE);
    idleScreen_SetStatus(PS_STATE_MISSCALL);

    YLVector<IdleStatusItem> vecStatus = idleScreen_GetStatusList();
    ASSERT_EQ(2, vecStatus.size());

    EXPECT_EQ(PS_STATE_RING_MUTE, vecStatus[0].m_nId);
}

TEST(idleScreenTest, NetChangeTest)
{
    //pass
    /*网络状态无法模拟
        _StatusManager.reset();
        _StatusManager.TurnNotify(false);
        idleScreen_SetNotifyFunction(NotifiedFunction, 5);
        idleScreen_RegisterStatus();

        //网络失败
        msgSendMsgToThread(thisThread, SYS_MSG_NOTIFY_NETWORK_STATUS, 0, 0);
        YLVector<IdleStatusItem> vecStatus = idleScreen_GetStatusList();
        ASSERT_EQ(1, vecStatus.size());
        EXPECT_EQ(PS_STATE_NETWORK_FAILED, vecStatus[0].m_nId);

        //网络恢复
        msgSendMsgToThread(thisThread, SYS_MSG_NOTIFY_NETWORK_STATUS, 1, 0);
        vecStatus = idleScreen_GetStatusList();
        EXPECT_EQ(0, vecStatus.size());
    */
}
/* 由于改成直接接口获取状态，该段废弃
TEST(idleScreenTest, IpConflitTest)
{
    _StatusManager.reset();
    _StatusManager.TurnNotify(false);
    idleScreen_SetNotifyFunction(NotifiedFunction, 5);
    idleScreen_RegisterStatus();

    //IP冲突 1
    msgSendMsgToThread(thisThread, SYS_MSG_NOTIFY_NETWORK_STATUS, SYS_IP_PROTO_IPV4, SYS_IC_BE_CONFLICT);
    YLVector<IdleStatusItem> vecStatus = idleScreen_GetStatusList();
    ASSERT_EQ(1, vecStatus.size());
    EXPECT_EQ(PS_STATE_NETWORK_FAILED, vecStatus[0].m_nId);

    //IP冲突 2, 由于防抖，IP冲突状态不会更新
    idleScreen_SetStatus(PS_STATE_MISSCALL);
    msgSendMsgToThread(thisThread, SYS_MSG_NOTIFY_NETWORK_STATUS, SYS_IP_PROTO_IPV4, SYS_IC_BE_CONFLICT);
    vecStatus = idleScreen_GetStatusList();
    ASSERT_EQ(2, vecStatus.size());
    EXPECT_EQ(PS_STATE_NETWORK_FAILED, vecStatus[1].m_nId);

    //从IP冲突中恢复
    msgSendMsgToThread(thisThread, SYS_MSG_NOTIFY_NETWORK_STATUS, SYS_IP_PROTO_IPV4, SYS_IC_NO_CONFLICT);
    vecStatus = idleScreen_GetStatusList();
    ASSERT_EQ(1, vecStatus.size());
    EXPECT_EQ(PS_STATE_MISSCALL, vecStatus[0].m_nId);
}
*/
TEST(idleScreenTest, NetConnectTest)
{
    //该消息状态由StatusMonitor自己刷新
    msgSendMsgToThread(thisThread, SYS_MSG_NOTIFY_PHY_STATUS, 0, 0);
    //pass
}

TEST(idleScreenTest, KeyGuradTest)
{
    _StatusManager.reset();
    _StatusManager.TurnNotify(false);
    idleScreen_SetNotifyFunction(NotifiedFunction, 5);
    idleScreen_RegisterStatus();

    //打开键盘锁
    msgSendMsgToThread(thisThread, KEYGUARD_LOCK_CHANGED, 1, 0);
    YLVector<IdleStatusItem> vecStatus = idleScreen_GetStatusList();
    ASSERT_EQ(1, vecStatus.size());
    EXPECT_EQ(PS_STATE_LOCK, vecStatus[0].m_nId);

    //关闭键盘锁
    msgSendMsgToThread(thisThread, KEYGUARD_LOCK_CHANGED, 0, 0);
    vecStatus = idleScreen_GetStatusList();
    ASSERT_EQ(0, vecStatus.size());
}

TEST(idleScreenTest, VPNTest)
{
    _StatusManager.reset();
    _StatusManager.TurnNotify(false);
    idleScreen_SetNotifyFunction(NotifiedFunction, 5);
    idleScreen_RegisterStatus();

    //打开VPN
    msgSendMsgToThread(thisThread, SYS_MSG_OPENVPN_STATUS, 1, 0);
    YLVector<IdleStatusItem> vecStatus = idleScreen_GetStatusList();
    ASSERT_EQ(1, vecStatus.size());
    EXPECT_EQ(PS_STATE_VPN, vecStatus[0].m_nId);

    //关闭VPN
    msgSendMsgToThread(thisThread, SYS_MSG_OPENVPN_STATUS, 0, 0);
    vecStatus = idleScreen_GetStatusList();
    ASSERT_EQ(0, vecStatus.size());
}

/******************************
TEST End
*******************************/



/******************************
Boot Start
*******************************/
/*************************************************
gcov 在程序退出时生成数据文件
进入msgkit系统的程序无法正常退出，怎么办？
gcov使用__gcov_flush函数刷新.gcna文件，程序中直接调用。
在源代码中直接声明该函数，C++代码中注意extern。
extern "C" void __gcov_flush(void);
在需要的地方调用。
***************************************************/
#if IS_COVER_TEST
extern "C" void __gcov_flush(void);
#endif

class EndExit : public testing::Environment
{
public:
    //该函数在所有用例执行完好执行
    virtual void SetUp()
    {
        devicelib_Init(false);

        idleScreen_Init();
        acc_Init();
    }
    //该函数在所有用例执行完后执行
    virtual void TearDown()
    {
#if IS_COVER_TEST
        //刷新gcov，生成gcna
        __gcov_flush();
#endif
    }
};


LRESULT CALLBACK idle_test_msg(msgObject * pMessage)
{
    switch (pMessage->message)
    {
    case TM_INIT:
        //初始化后调用RUN_ALL_TESTS运行测试用例
        return RUN_ALL_TESTS();
        break;
    default:
        break;
    };
    //_StatusMonitor.OnStatusMessage(*pMessage);
    return etl_OnMessageProcess(*pMessage);
}

int main(int argc, char * argv[])
{
    //设置gtest环境
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new EndExit);

    //进入msgkit消息系统
    thisThread = mkRegistThread("test_idlescreen", idle_test_msg);
    mkRunThread(thisThread);
    return 0;
}

/******************************
Boot End
*******************************/

#endif



