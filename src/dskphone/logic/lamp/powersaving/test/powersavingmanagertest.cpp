#include <gtest/gtest.h>

#include <service_config.h> // CONFIG_MSG_BCAST_CHANGED
#include <service_system.h> // SYS_MSG_NOTIFY_TIME_CHANGED

#include "mocklibs/configparser/mockconfigparser.h"
#include "mocklibs/ETLLib/mocksub-interface.h"
#include "mocklibs/etlmsghandler/mocketlmsghandler.h"
#include "mocklibs/sysservice/mockservice_system.h"

#include "powersavingmsgdefine.h"
#include "powersavingmanager.h"

#include "powersavingconfigtest.h"


using ::testing::_;

class PowerSavingManagerTest : public testing::Test
{
protected:
    MockConfigParser m_objMockConfigParser;
    MockEtlMsgHandler m_objMockEtlMsgHandler;
    MockSubInterface m_objMockSubInterface;
    MockSysService m_objMockSysService;

    virtual void SetUp()
    {
        g_pMockConfigParser = &m_objMockConfigParser;
        g_pMockEtlMsgHandler = &m_objMockEtlMsgHandler;
        g_pMockSubInterface = &m_objMockSubInterface;
        g_pMockSysService = &m_objMockSysService;

        // 注册/注销消息处理函数
        EXPECT_CALL(
            m_objMockEtlMsgHandler,
            RegisterMsgHandle(TM_TIMER, TM_TIMER, _)
        ).Times(testing::AtLeast(0)).WillRepeatedly(testing::Return(TRUE)); // 定时器
        EXPECT_CALL(
            m_objMockEtlMsgHandler,
            UnregisterMsgHandle(TM_TIMER, TM_TIMER, _)
        ).Times(testing::AtLeast(0)).WillRepeatedly(testing::Return(TRUE)); // 定时器
        EXPECT_CALL(
            m_objMockEtlMsgHandler,
            RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, _)
        ).Times(testing::AtLeast(0)).WillRepeatedly(testing::Return(TRUE)); // 配置改变
        EXPECT_CALL(
            m_objMockEtlMsgHandler,
            UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, _)
        ).Times(testing::AtLeast(0)).WillRepeatedly(testing::Return(TRUE)); // 配置改变
        EXPECT_CALL(
            m_objMockEtlMsgHandler,
            RegisterMsgHandle(SYS_MSG_NOTIFY_TIME_CHANGED, SYS_MSG_NOTIFY_TIME_CHANGED, _)
        ).Times(testing::AtLeast(0)).WillRepeatedly(testing::Return(TRUE)); // 系统时间改变
        EXPECT_CALL(
            m_objMockEtlMsgHandler,
            UnregisterMsgHandle(SYS_MSG_NOTIFY_TIME_CHANGED, SYS_MSG_NOTIFY_TIME_CHANGED, _)
        ).Times(testing::AtLeast(0)).WillRepeatedly(testing::Return(TRUE)); // 系统时间改变
        EXPECT_CALL(
            m_objMockEtlMsgHandler,
            RegisterMsgHandle(POWERSAVIGNMEG_NOTIFY_OPERATE, POWERSAVIGNMEG_NOTIFY_OPERATE, _)
        ).Times(testing::AtLeast(0)).WillRepeatedly(testing::Return(TRUE)); // 主动/被动操作
        EXPECT_CALL(
            m_objMockEtlMsgHandler,
            UnregisterMsgHandle(POWERSAVIGNMEG_NOTIFY_OPERATE, POWERSAVIGNMEG_NOTIFY_OPERATE, _)
        ).Times(testing::AtLeast(0)).WillRepeatedly(testing::Return(TRUE)); // 主动/被动操作
    }

    virtual void TearDown()
    {
        g_pMockConfigParser = NULL;
        g_pMockEtlMsgHandler = NULL;
        g_pMockSubInterface = NULL;
        g_pMockSysService = NULL;
    }
};


// 获取操作对应的进入节电时间点
TEST(PowerSavingManager, GetTimeoutOfOperation)
{
    /// 使用默认值计算
    EXPECT_TRUE(_PowerSavingManager.m_config.AddWeekTimeRangeOfWork(WEEKDAY_MONDAY, 7, 19));
    EXPECT_TRUE(_PowerSavingManager.m_config.AddWeekTimeRangeOfWork(WEEKDAY_TUESDAY, 7, 19));
    EXPECT_TRUE(_PowerSavingManager.m_config.AddWeekTimeRangeOfWork(WEEKDAY_WEDNESDAY, 7, 19));
    EXPECT_TRUE(_PowerSavingManager.m_config.AddWeekTimeRangeOfWork(WEEKDAY_THURSDAY, 7, 19));
    EXPECT_TRUE(_PowerSavingManager.m_config.AddWeekTimeRangeOfWork(WEEKDAY_FRIDAY, 7, 19));
    /* 工作时间+主动操作
    ** time=2016-10-20 09:00:00 周四(1476954000)
    ** interval=6h
    ** timeout=2016-10-20 15:00:00 周四(1476975600)
    **/
    EXPECT_EQ(1476975600L, _PowerSavingManager.GetTimeoutOfOperation(OPERATION_ACTIVE, 1476954000L));
    /* 工作时间+被动操作
    ** time=2016-10-20 18:00:00 周四(1476986400)
    ** interval=6h/10min 跨段马上进入节能
    ** timeout=2016-10-20 19:00:00 周四(1476990000)
    **/
    EXPECT_EQ(1476990000L, _PowerSavingManager.GetTimeoutOfOperation(OPERATION_ACTIVE, 1476986400L));
    /* 休息时间+主动操作
    ** time=2016-10-20 00:00:00 周四(1476921600)
    ** interval=10min
    ** timeout=2016-10-20 00:10:00 周四(1476922200)
    **/
    EXPECT_EQ(1476922200L, _PowerSavingManager.GetTimeoutOfOperation(OPERATION_ACTIVE, 1476921600L));
    /* 休息时间+被动操作
    ** time=2016-10-20 06:59:00 周四(1476946740)
    ** interval=10min/6h 跨段继续计时
    ** timeout=2016-10-20 12:59:00 周四(1476968340)
    **/
    EXPECT_EQ(1476968340L, _PowerSavingManager.GetTimeoutOfOperation(OPERATION_ACTIVE, 1476946740L));

    /// 禁用节电模式，进入容灾
    _PowerSavingManager.m_config.m_bPowerSavingEnable = false;
    /* 工作时间+主动操作
    ** time=2016-10-20 09:00:00 周四(1476954000)
    ** interval=3d
    ** timeout=2016-10-23 09:00:00 周四(1477213200)
    **/
    EXPECT_EQ(1477213200L, _PowerSavingManager.GetTimeoutOfOperation(OPERATION_ACTIVE, 1476954000L));
    /* 工作时间+被动操作
    ** time=2016-10-20 18:00:00 周四(1476986400)
    ** interval=3d
    ** timeout=2016-10-23 18:00:00 周四(1477245600)
    **/
    EXPECT_EQ(1477245600L, _PowerSavingManager.GetTimeoutOfOperation(OPERATION_ACTIVE, 1476986400L));
    /* 休息时间+主动操作
    ** time=2016-10-20 00:00:00 周四(1476921600)
    ** interval=3d
    ** timeout=2016-10-23 00:00:00 周四(1477180800)
    **/
    EXPECT_EQ(1477180800L, _PowerSavingManager.GetTimeoutOfOperation(OPERATION_ACTIVE, 1476921600L));
    /* 休息时间+被动操作
    ** time=2016-10-20 06:59:00 周四(1476946740)
    ** interval=3d
    ** timeout=2016-10-23 6:59:00 周四(1477205940)
    **/
    EXPECT_EQ(1477205940L, _PowerSavingManager.GetTimeoutOfOperation(OPERATION_ACTIVE, 1476946740L));
    _PowerSavingManager.m_config.m_bPowerSavingEnable = true;

    /// 跨越多个时段
    _PowerSavingManager.m_config.m_timePassiveIntervalOfRest = 5 * 60 * 60; // 5h
    EXPECT_TRUE(_PowerSavingManager.m_config.AddWeekTimeRangeOfWork(WEEKDAY_THURSDAY, 20, 21));
    EXPECT_TRUE(_PowerSavingManager.m_config.AddWeekTimeRangeOfWork(WEEKDAY_THURSDAY, 22, 23));
    /* 休息时间+被动操作
    ** time=2016-10-20 18:00:00 周四(1476986400)
    ** interval=6h/5h/6h/5h/6h/5h
    ** timeout=2016-10-20 23:00:00 周四(1477004400)
    **/
    EXPECT_EQ(1477004400L, _PowerSavingManager.GetTimeoutOfOperation(OPERATION_ACTIVE, 1476986400L));
}

// 容灾模式测试
TEST_F(PowerSavingManagerTest, FailBack)
{
    EXPECT_LoadConfig(false,
                      false); // 禁用节电，进入容灾模式（禁用智能唤醒，避免场景复杂）

    EXPECT_GET_SYS_TIME(1493132400L, 0); // 2017-04-25 15:00:00 周二(1493132400)
    EXPECT_CALL_timerSetThreadTimer(_,
                                    (4320 * 60 - 15) * 1000); // 容灾预节电时间：4320mins - 15s
    _PowerSavingManager.Init();
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_WAITING);

    // 进入预节电
    EXPECT_GET_SYS_TIME(1493391585L, 0); // 2017-04-28 14:59:45 周五(1493391585)
    EXPECT_CALL_timerSetThreadTimer(_, 15 * 1000); // 节电时间：15s
    EXPECT_CALL_NotifyApp(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVING_MODE_PRE_CLOSE,
                          _PowerSavingManager.m_config.GetPreSavingBacklightLevel());
    TriggerThreadTimerOfInterval((4320 * 60 - 15) * 1000);
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_PRE_SAVING);

    // 进入节电
    EXPECT_GET_SYS_TIME(1493391600L, 0); // 2017-04-28 15:00:00 周五(1493391600)
    EXPECT_CALL_NotifyApp(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVING_MODE_CLOSE, 0);
    TriggerThreadTimerOfInterval(15 * 1000);
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_SAVING);

    // 主动操作：重新计算容灾节电
    EXPECT_GET_SYS_TIME(1493391900L, 0); // 2017-04-28 15:05:00 周五(1493391900)
    EXPECT_CALL_timerSetThreadTimer(_,
                                    (4320 * 60 - 15) * 1000); // 容灾预节电时间：4320mins - 15s
    EXPECT_CALL_NotifyApp(POWERSAVINGMEG_SCREEN_AWAKE, 0, 0);
    msgObject msg;
    msg.message = POWERSAVIGNMEG_NOTIFY_OPERATE;
    msg.wParam = 1;
    TriggerEtlMsgCallback(msg); // 触发主动操作
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_WAITING);

    // 等待节电过程中被动操作：不重新计算容灾节电
    EXPECT_GET_SYS_TIME(1493650800L, 0); // 2017-05-01 15:00:00 周一(1493650800)
    msg.wParam = 0;
    TriggerEtlMsgCallback(msg); // 触发被动操作
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_WAITING);

    // 进入预节电
    EXPECT_GET_SYS_TIME(1493651085L, 0); // 2017-05-01 15:04:45 周一(1493651085)
    EXPECT_CALL_timerSetThreadTimer(_, 15 * 1000); // 节电时间：15s
    EXPECT_CALL_NotifyApp(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVING_MODE_PRE_CLOSE,
                          _PowerSavingManager.m_config.GetPreSavingBacklightLevel());
    TriggerThreadTimerOfInterval((4320 * 60 - 15) * 1000);
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_PRE_SAVING);

    // 预节电状态中被动操作：按照空闲时间段被动操作节电时间计算
    EXPECT_GET_SYS_TIME(1493651090L, 0); // 2017-05-01 15:04:50 周一(1493651090)
    EXPECT_CALL_timerSetThreadTimer(_,
                                    (10 * 60 - 15) * 1000); // 空闲被动操作预节电时间：10mins - 15s
    EXPECT_CALL_NotifyApp(POWERSAVINGMEG_SCREEN_AWAKE, 0, 0);
    msg.wParam = 0;
    TriggerEtlMsgCallback(msg); // 触发被动操作
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_WAITING);

    // 进入预节电
    EXPECT_GET_SYS_TIME(1493651675L, 0); // 2017-05-01 15:14:35 周一(1493651675)
    EXPECT_CALL_timerSetThreadTimer(_, 15 * 1000); // 节电时间：15s
    EXPECT_CALL_NotifyApp(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVING_MODE_PRE_CLOSE,
                          _PowerSavingManager.m_config.GetPreSavingBacklightLevel());
    TriggerThreadTimerOfInterval((10 * 60 - 15) * 1000);
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_PRE_SAVING);

    // 进入节电
    EXPECT_GET_SYS_TIME(1493651690L, 0); // 2017-05-01 15:14:50 周五(1493651690)
    EXPECT_CALL_NotifyApp(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVING_MODE_CLOSE, 0);
    TriggerThreadTimerOfInterval(15 * 1000);
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_SAVING);

    // 节电状态中被动操作：按照空闲时间段被动操作节电时间计算
    EXPECT_GET_SYS_TIME(1493651700L, 0); // 2017-05-01 15:15:00 周一(1493651700)
    EXPECT_CALL_timerSetThreadTimer(_,
                                    (10 * 60 - 15) * 1000); // 空闲被动操作预节电时间：10mins - 15s
    EXPECT_CALL_NotifyApp(POWERSAVINGMEG_SCREEN_AWAKE, 0, 0);
    msg.wParam = 0;
    TriggerEtlMsgCallback(msg); // 触发被动操作
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_WAITING);

    _PowerSavingManager.Uninit();
}

// [Bug #106883]容灾模式修改系统时间
TEST_F(PowerSavingManagerTest, TimeChangedWhenFailBack)
{
    EXPECT_LoadConfig(false,
                      false); // 禁用节电，进入容灾模式（禁用智能唤醒，避免场景复杂）

    EXPECT_GET_SYS_TIME(1493132400L, 0); // 2017-04-25 15:00:00 周二(1493132400)
    EXPECT_CALL_timerSetThreadTimer(_,
                                    (4320 * 60 - 15) * 1000); // 容灾预节电时间：4320mins - 15s
    _PowerSavingManager.Init();
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_WAITING);

    // 修改系统时间：视为被动操作，重新计算容灾节电
    EXPECT_GET_SYS_TIME(1496213340L, 0); // 2017-05-31 06:49:00 周三(1496213340)
    msgObject msg;
    msg.message = CONFIG_MSG_BCAST_CHANGED;
    msg.wParam = ST_SETTIME;
    TriggerEtlMsgCallback(msg); // 触发系统时间改变消息
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_WAITING);

    // 进入预节电
    EXPECT_GET_SYS_TIME(1496213925L, 0); // 2017-05-31 06:58:45 周三(1496213925)
    EXPECT_CALL_timerSetThreadTimer(_, 15 * 1000); // 节电时间：15s
    EXPECT_CALL_NotifyApp(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVING_MODE_PRE_CLOSE,
                          _PowerSavingManager.m_config.GetPreSavingBacklightLevel());
    TriggerThreadTimerOfInterval((4320 * 60 - 15) * 1000);
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_PRE_SAVING);

    // 进入节电
    EXPECT_GET_SYS_TIME(1496213940L, 0); // 2017-05-31 07:59:00 周三(1496213940)
    EXPECT_CALL_NotifyApp(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVING_MODE_CLOSE, 0);
    TriggerThreadTimerOfInterval(15 * 1000);
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_SAVING);

	// 修改系统时间：视为被动操作，熄屏情况下，重置节电定时器
	EXPECT_CALL_netGetLocalTime(1496213340L); // 2017-05-31 06:49:00 周三(1496213340)
	EXPECT_CALL_timerSetThreadTimer(_, (10 * 60 - 15) * 1000); // 容灾预节电时间：10mins - 15s
	EXPECT_CALL_NotifyApp(POWERSAVINGMEG_SCREEN_AWAKE, 0, 0);
	msg.message = CONFIG_MSG_BCAST_CHANGED;
	msg.wParam = ST_SETTIME;
	TriggerEtlMsgCallback(msg); // 触发系统时间改变消息
	EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_WAITING);

    _PowerSavingManager.Uninit();
}

// [Bug #133725]节电模式修改配置进入容灾模式
TEST_F(PowerSavingManagerTest, PowerSaving2FailBack)
{
    EXPECT_LoadConfig(true, false); // 启用节电，禁用智能唤醒，避免场景复杂

    EXPECT_GET_SYS_TIME(1509615000L, 0); // 2017-11-02 09:30:00 周四(1509615000)
    EXPECT_CALL_timerSetThreadTimer(_, (360 * 60 - 15) * 1000); // 工作预节电时间：360mins - 15s
    _PowerSavingManager.Init();
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_WAITING);

    // 修改配置，禁用节电，进入容灾：视为被动操作，重新计算容灾节电
    EXPECT_LoadConfig(false, false); // 禁用节电，禁用智能唤醒，避免场景复杂
    EXPECT_GET_SYS_TIME(1509616800L, 0); // 2017-11-02 10:00:00 周四(1509616800)
    EXPECT_CALL_timerSetThreadTimer(_, (4320 * 60 - 15) * 1000); // 容灾预节电时间：4320mins - 15s
    msgObject msg;
    msg.message = CONFIG_MSG_BCAST_CHANGED;
    msg.wParam = ST_POWERSAVING;
    TriggerEtlMsgCallback(msg); // 触发节电配置改变消息
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_WAITING);

    EXPECT_GET_SYS_TIME(1509617400L, 0); // 2017-11-02 10:10:00 周四(1509617400)
    // 触发账号配置改变消息，应该视为被动操作，进入节电的时长小于容灾时间，定时器不变
    msg.message = CONFIG_MSG_BCAST_CHANGED;
    msg.wParam = ST_SIPLINE;
    TriggerEtlMsgCallback(msg);
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_WAITING);

    // 进入预节电
    EXPECT_GET_SYS_TIME(1509875985L, 0); // 2017-11-05 09:59:45 周日(1509875985)
    EXPECT_CALL_timerSetThreadTimer(_, 15 * 1000); // 节电时间：15s
    EXPECT_CALL_NotifyApp(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVING_MODE_PRE_CLOSE,
                          _PowerSavingManager.m_config.GetPreSavingBacklightLevel());
    TriggerThreadTimerOfInterval((4320 * 60 - 15) * 1000);
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_PRE_SAVING);

    // 进入节电
    EXPECT_GET_SYS_TIME(1509876000L, 0); // 2017-11-05 10:00:00 周日(1509876000)
    EXPECT_CALL_NotifyApp(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVING_MODE_CLOSE, 0);
    TriggerThreadTimerOfInterval(15 * 1000);
    EXPECT_EQ(_PowerSavingManager.GetStatus(), POWERSAVING_STATUS_SAVING);

    _PowerSavingManager.Uninit();
}
