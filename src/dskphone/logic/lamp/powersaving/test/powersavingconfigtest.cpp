#include "powersavingconfigtest.h"

#include <gtest/gtest.h>

#include "mocklibs/configparser/mockconfigparser.h"

#include "powersavingconfig.h"


// 加载节电相关配置
void EXPECT_LoadConfig(bool bEnable/* = true*/, bool bSmartWakeEnable/* = true*/,
                       int timeActiveInterval/* = 10*/, int timePassiveIntervalOfWork/* = 360*/,
                       int timePassiveIntervalOfRest/* = 10*/, int timeFailbackModeInterval/* = 4320*/,
                       int iPreSavingBacklightLevel/* = 1*/)
{
    EXPECT_CALL_GetConfigInt(kszPowerSavingEnable, bEnable);
    EXPECT_CALL_GetConfigInt(kszPowerSavingIntelligentMode, bSmartWakeEnable);
    EXPECT_CALL_GetConfigInt(kszPowerSavingUserInputTimeout, timeActiveInterval);
    EXPECT_CALL_GetConfigInt(kszPowerSavingOfficeTimeout, timePassiveIntervalOfWork);
    EXPECT_CALL_GetConfigInt(kszPowerSavingOffTimeout, timePassiveIntervalOfRest);
    EXPECT_CALL_GetConfigInt(kszPowerSavingSrceenProtectTime, timeFailbackModeInterval);
    EXPECT_CALL_GetConfigInt(kszInactiveLevel, iPreSavingBacklightLevel);
    // 加载工作时间段
    EXPECT_LoadWorkTimeOfDayConfig(WEEKDAY_MONDAY, 7, 19);
    EXPECT_LoadWorkTimeOfDayConfig(WEEKDAY_TUESDAY, 7, 19);
    EXPECT_LoadWorkTimeOfDayConfig(WEEKDAY_WEDNESDAY, 7, 19);
    EXPECT_LoadWorkTimeOfDayConfig(WEEKDAY_THURSDAY, 7, 19);
    EXPECT_LoadWorkTimeOfDayConfig(WEEKDAY_FRIDAY, 7, 19);
    EXPECT_LoadWorkTimeOfDayConfig(WEEKDAY_SATURDAY, 7, 7);
    EXPECT_LoadWorkTimeOfDayConfig(WEEKDAY_SUNDAY, 7, 7);
}

void EXPECT_LoadWorkTimeOfDayConfig(WEEKDAY_TYPE day, int hourFrom, int hourTo)
{
    char value[8] = { 0 };
    sprintf(value, "%d,%d", hourFrom, hourTo);
    switch (day)
    {
    case WEEKDAY_MONDAY:
        EXPECT_CALL_GetConfigString(kszPowerSavingOfficeMonday, value);
        break;
    case WEEKDAY_TUESDAY:
        EXPECT_CALL_GetConfigString(kszPowerSavingOfficeTuesday, value);
        break;
    case WEEKDAY_WEDNESDAY:
        EXPECT_CALL_GetConfigString(kszPowerSavingOfficeWednesday, value);
        break;
    case WEEKDAY_THURSDAY:
        EXPECT_CALL_GetConfigString(kszPowerSavingOfficeThursday, value);
        break;
    case WEEKDAY_FRIDAY:
        EXPECT_CALL_GetConfigString(kszPowerSavingOfficeFriday, value);
        break;
    case WEEKDAY_SATURDAY:
        EXPECT_CALL_GetConfigString(kszPowerSavingOfficeSaturday, value);
        break;
    case WEEKDAY_SUNDAY:
        EXPECT_CALL_GetConfigString(kszPowerSavingOfficeSunday, value);
        break;
    default:
        break;
    }
}

class PowerSavingConfigTest : public testing::Test
{
protected:
    MockConfigParser m_objMockConfigParser;

    virtual void SetUp()
    {
        g_pMockConfigParser = &m_objMockConfigParser;
    }

    virtual void TearDown()
    {
        g_pMockConfigParser = NULL;
    }

    // 时段检查函数，校验某一时段是否均为指定类型
    bool CheckTimeRangeType(TIME_RANGE_TYPE arrWorkHoursWeekly[7][24], TIME_RANGE_TYPE type,
                            WEEKDAY_TYPE dayBegin, Hour hourBegin, WEEKDAY_TYPE dayEnd, Hour hourEnd)
    {
        // 赋初值
        short day = dayBegin;
        Hour hour = hourBegin;
        // 开始循环
        while (day != dayEnd || hour != hourEnd)
        {
            //printf("arrWorkHoursWeekly[%hd][%hd]=%d\n", day, hour, arrWorkHoursWeekly[day][hour]);
            // 校验类型
            if (arrWorkHoursWeekly[day][hour] != type)
            {
                return false;
            }
            // 时段后移
            ++hour;
            if (hour > 23)
            {
                hour = 0;
                ++day;
                if (day > 6)
                {
                    day = 0;
                }
            }
        }
        return true;
    }

};


// 构造函数初值测试
TEST_F(PowerSavingConfigTest, Constructor)
{
    CPowerSavingConfig config;
    EXPECT_TRUE(config.m_bPowerSavingEnable);
    EXPECT_TRUE(config.m_bSmartWakeEnable);
    EXPECT_EQ(config.m_timeActiveInterval, 10 * 60);
    EXPECT_EQ(config.m_timePassiveIntervalOfWork, 360 * 60);
    EXPECT_EQ(config.m_timePassiveIntervalOfRest, 10 * 60);
    EXPECT_EQ(config.m_timeFailbackModeInterval, 4320 * 60);
    for (size_t day = 0; day < 7; day++)
    {
        for (Hour hour = 0; hour < 24; hour++)
        {
            EXPECT_EQ(config.m_arrWorkHoursWeekly[day][hour], TIME_RANGE_REST);
        }
    }
}

// 加载配置文件
TEST_F(PowerSavingConfigTest, Load)
{
    EXPECT_LoadConfig();

    CPowerSavingConfig config;
    config.Load();

    EXPECT_TRUE(CheckTimeRangeType(config.m_arrWorkHoursWeekly,
                                   TIME_RANGE_WORK, WEEKDAY_MONDAY, 7, WEEKDAY_MONDAY, 19));
    EXPECT_TRUE(CheckTimeRangeType(config.m_arrWorkHoursWeekly,
                                   TIME_RANGE_REST, WEEKDAY_MONDAY, 19, WEEKDAY_TUESDAY, 7));
    EXPECT_TRUE(CheckTimeRangeType(config.m_arrWorkHoursWeekly,
                                   TIME_RANGE_WORK, WEEKDAY_TUESDAY, 7, WEEKDAY_TUESDAY, 19));
    EXPECT_TRUE(CheckTimeRangeType(config.m_arrWorkHoursWeekly,
                                   TIME_RANGE_REST, WEEKDAY_TUESDAY, 19, WEEKDAY_WEDNESDAY, 7));
    EXPECT_TRUE(CheckTimeRangeType(config.m_arrWorkHoursWeekly,
                                   TIME_RANGE_WORK, WEEKDAY_WEDNESDAY, 7, WEEKDAY_WEDNESDAY, 19));
    EXPECT_TRUE(CheckTimeRangeType(config.m_arrWorkHoursWeekly,
                                   TIME_RANGE_REST, WEEKDAY_WEDNESDAY, 19, WEEKDAY_THURSDAY, 7));
    EXPECT_TRUE(CheckTimeRangeType(config.m_arrWorkHoursWeekly,
                                   TIME_RANGE_WORK, WEEKDAY_THURSDAY, 7, WEEKDAY_THURSDAY, 19));
    EXPECT_TRUE(CheckTimeRangeType(config.m_arrWorkHoursWeekly,
                                   TIME_RANGE_REST, WEEKDAY_THURSDAY, 19, WEEKDAY_FRIDAY, 7));
    EXPECT_TRUE(CheckTimeRangeType(config.m_arrWorkHoursWeekly,
                                   TIME_RANGE_WORK, WEEKDAY_FRIDAY, 7, WEEKDAY_FRIDAY, 19));
    EXPECT_TRUE(CheckTimeRangeType(config.m_arrWorkHoursWeekly,
                                   TIME_RANGE_REST, WEEKDAY_FRIDAY, 19, WEEKDAY_MONDAY, 7));
}

// 加载一周中某天的工作时间配置
TEST_F(PowerSavingConfigTest, LoadWorkTimeOfDay)
{
    // 以周五为例
    const WEEKDAY_TYPE day = WEEKDAY_FRIDAY;
    const char * kszConfigKey = kszPowerSavingOfficeFriday;
    CPowerSavingConfig * config = NULL;

    // 尝试解析常见值：7,19――正常
    config = new CPowerSavingConfig();
    EXPECT_CALL_GetConfigString(kszConfigKey, "7,19");
    EXPECT_TRUE(config->LoadWorkTimeOfDay(day, kszConfigKey));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_WORK,
                                   WEEKDAY_FRIDAY, 7, WEEKDAY_FRIDAY, 19));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_FRIDAY, 19, WEEKDAY_FRIDAY, 7));
    delete config;
    config = NULL;

    // 尝试解析空时段：7,7――正常
    config = new CPowerSavingConfig();
    EXPECT_CALL_GetConfigString(kszConfigKey, "7,7");
    EXPECT_TRUE(config->LoadWorkTimeOfDay(day, kszConfigKey));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_FRIDAY, 7, WEEKDAY_FRIDAY, 19));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_FRIDAY, 19, WEEKDAY_FRIDAY,
                                   7)); // CheckTimeRangeType无法一次性校验全部时间，因此需要分两次校验
    delete config;
    config = NULL;

    // 尝试解析跨天时段：7,6――正常
    config = new CPowerSavingConfig();
    EXPECT_CALL_GetConfigString(kszConfigKey, "7,6");
    EXPECT_TRUE(config->LoadWorkTimeOfDay(day, kszConfigKey));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_WORK,
                                   WEEKDAY_FRIDAY, 7, WEEKDAY_SATURDAY, 6));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_SATURDAY, 6, WEEKDAY_FRIDAY, 7));
    delete config;
    config = NULL;

    // 尝试解析无效范围：0,24――修正为0,23
    config = new CPowerSavingConfig();
    EXPECT_CALL_GetConfigString(kszConfigKey, "0,24");
    EXPECT_TRUE(config->LoadWorkTimeOfDay(day, kszConfigKey));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_WORK,
                                   WEEKDAY_FRIDAY, 0, WEEKDAY_FRIDAY, 23));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_FRIDAY, 23, WEEKDAY_FRIDAY, 0));
    delete config;
    config = NULL;

    // 尝试解析无效范围：99,-99――修正为23,0
    config = new CPowerSavingConfig();
    EXPECT_CALL_GetConfigString(kszConfigKey, "99,-99");
    EXPECT_TRUE(config->LoadWorkTimeOfDay(day, kszConfigKey));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_WORK,
                                   WEEKDAY_FRIDAY, 23, WEEKDAY_SATURDAY, 0));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_SATURDAY, 0, WEEKDAY_FRIDAY, 23));
    delete config;
    config = NULL;

    // 尝试解析带有空格：7, 19――修正为7,19
    config = new CPowerSavingConfig();
    EXPECT_CALL_GetConfigString(kszConfigKey, "7, 19");
    EXPECT_TRUE(config->LoadWorkTimeOfDay(day, kszConfigKey));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_WORK,
                                   WEEKDAY_FRIDAY, 7, WEEKDAY_FRIDAY, 19));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_FRIDAY, 19, WEEKDAY_FRIDAY, 7));
    delete config;
    config = NULL;

    // 尝试解析中文逗号：8，18――使用默认值：7,19
    config = new CPowerSavingConfig();
    EXPECT_CALL_GetConfigString(kszConfigKey, "8，18");
    EXPECT_CALL_GetDefaultValueString(kszConfigKey, "7,19");
    EXPECT_TRUE(config->LoadWorkTimeOfDay(day, kszConfigKey));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_WORK,
                                   day, 7, day, 19));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   day, 19, day, 7));
    delete config;
    config = NULL;

    // 尝试解析错误连接符：8-18――使用默认值：空字符串――不添加该时段
    config = new CPowerSavingConfig();
    EXPECT_CALL_GetConfigString(kszConfigKey, "8-18");
    EXPECT_CALL_GetDefaultValueString(kszConfigKey, "");
    EXPECT_FALSE(config->LoadWorkTimeOfDay(day, kszConfigKey));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_MONDAY, 0, WEEKDAY_SUNDAY, 23));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_SUNDAY, 23, WEEKDAY_MONDAY, 0));
    delete config;
    config = NULL;

    // 尝试解析错误数量：719――使用默认值：a,b――不添加该时段
    config = new CPowerSavingConfig();
    EXPECT_CALL_GetConfigString(kszConfigKey, "719");
    EXPECT_CALL_GetDefaultValueString(kszConfigKey, "a,b");
    EXPECT_FALSE(config->LoadWorkTimeOfDay(day, kszConfigKey));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_MONDAY, 0, WEEKDAY_SUNDAY, 23));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_SUNDAY, 23, WEEKDAY_MONDAY, 0));
    delete config;
    config = NULL;

    // 尝试解析错误数量：8,18,20――修正为8,18
    config = new CPowerSavingConfig();
    EXPECT_CALL_GetConfigString(kszConfigKey, "8,18,20");
    EXPECT_TRUE(config->LoadWorkTimeOfDay(day, kszConfigKey));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_WORK,
                                   WEEKDAY_FRIDAY, 8, WEEKDAY_FRIDAY, 18));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_FRIDAY, 18, WEEKDAY_FRIDAY, 8));
    delete config;
    config = NULL;

    // 尝试解析0前缀：07,19――修正为7,19
    config = new CPowerSavingConfig();
    EXPECT_CALL_GetConfigString(kszConfigKey, "07,19");
    EXPECT_TRUE(config->LoadWorkTimeOfDay(day, kszConfigKey));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_WORK,
                                   WEEKDAY_FRIDAY, 7, WEEKDAY_FRIDAY, 19));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_FRIDAY, 19, WEEKDAY_FRIDAY, 7));
    delete config;
    config = NULL;

    // 尝试解析超长超大数字：0000000000000,9999999999999――修正为0,23
    config = new CPowerSavingConfig();
    EXPECT_CALL_GetConfigString(kszConfigKey, "0000000000000,9999999999999");
    EXPECT_FALSE(config->LoadWorkTimeOfDay(day, kszConfigKey));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_MONDAY, 0, WEEKDAY_SUNDAY, 23));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_SUNDAY, 23, WEEKDAY_MONDAY, 0));
    delete config;
    config = NULL;
}

// 添加工作时段
TEST_F(PowerSavingConfigTest, AddWeekTimeRangeOfWork)
{
    // 以周一为例
    const WEEKDAY_TYPE day = WEEKDAY_MONDAY;
    CPowerSavingConfig * config = NULL;
    // 尝试解析常见值：7,19
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(day, 7, 19));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_WORK,
                                   day, 7, day, 19));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   day, 19, day, 7));
    delete config;
    config = NULL;
    // 尝试解析空时段：7,7
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(day, 7, 7));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_MONDAY, 0, WEEKDAY_SUNDAY, 23));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_SUNDAY, 23, WEEKDAY_MONDAY,
                                   0)); // CheckTimeRangeType无法一次性校验全部时间，因此需要分两次校验
    delete config;
    config = NULL;
    // 尝试解析跨天时段：7,6
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(day, 7, 6));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_WORK,
                                   WEEKDAY_MONDAY, 7, WEEKDAY_TUESDAY, 6));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_TUESDAY, 6, WEEKDAY_MONDAY, 7));
    delete config;
    config = NULL;
    // 尝试解析无效范围：0,24
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(day, 0, 24));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_WORK,
                                   day, 0, day, 23));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   day, 23, day, 0));
    delete config;
    config = NULL;
    // 尝试解析无效范围：100,-100
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(day, 100, -100));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_WORK,
                                   WEEKDAY_MONDAY, 23, WEEKDAY_TUESDAY, 0));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_TUESDAY, 0, WEEKDAY_MONDAY, 23));
    delete config;
    config = NULL;
    // 尝试解析两个时段连接：7,6 & 6,19
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_MONDAY, 7, 6));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_TUESDAY, 6, 19));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_WORK,
                                   WEEKDAY_MONDAY, 7, WEEKDAY_TUESDAY, 19));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_TUESDAY, 19, WEEKDAY_MONDAY, 7));
    delete config;
    config = NULL;
    // 尝试解析两个时段重叠：7,6 & 3,19
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_MONDAY, 7, 6));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_TUESDAY, 3, 19));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_WORK,
                                   WEEKDAY_MONDAY, 7, WEEKDAY_TUESDAY, 19));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_TUESDAY, 19, WEEKDAY_MONDAY, 7));
    delete config;
    config = NULL;
    // 尝试解析最大化工作时间
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_MONDAY, 9, 8));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_TUESDAY, 8, 7));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_WEDNESDAY, 7, 6));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_THURSDAY, 6, 5));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_FRIDAY, 5, 4));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_SATURDAY, 4, 3));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_SUNDAY, 3, 2));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_WORK,
                                   WEEKDAY_MONDAY, 9, WEEKDAY_MONDAY, 2));
    EXPECT_TRUE(CheckTimeRangeType(config->m_arrWorkHoursWeekly, TIME_RANGE_REST,
                                   WEEKDAY_MONDAY, 2, WEEKDAY_MONDAY, 9));
    delete config;
    config = NULL;
}

// 当前是否启用节电模式
TEST_F(PowerSavingConfigTest, IsPowerSavingEnable)
{
    CPowerSavingConfig config;
    EXPECT_EQ(config.IsPowerSavingEnable(), config.m_bPowerSavingEnable);
    config.m_bPowerSavingEnable = false;
    EXPECT_EQ(config.IsPowerSavingEnable(), config.m_bPowerSavingEnable);
    config.m_bPowerSavingEnable = true;
    EXPECT_EQ(config.IsPowerSavingEnable(), config.m_bPowerSavingEnable);
}

// 当前是否启用智能点亮
TEST_F(PowerSavingConfigTest, IsSmartWakeEnable)
{
    CPowerSavingConfig config;
    EXPECT_EQ(config.IsSmartWakeEnable(), config.m_bSmartWakeEnable);
    config.m_bSmartWakeEnable = false;
    EXPECT_EQ(config.IsSmartWakeEnable(), config.m_bSmartWakeEnable);
    config.m_bSmartWakeEnable = true;
    EXPECT_EQ(config.IsSmartWakeEnable(), config.m_bSmartWakeEnable);
}

// 是否容灾模式
TEST_F(PowerSavingConfigTest, IsFailBackMode)
{
    CPowerSavingConfig config;
    EXPECT_EQ(config.IsFailBackMode(), !config.m_bPowerSavingEnable);
    config.m_bPowerSavingEnable = false;
    EXPECT_EQ(config.IsFailBackMode(), !config.m_bPowerSavingEnable);
    config.m_bSmartWakeEnable = false;
    EXPECT_EQ(config.IsFailBackMode(), !config.m_bPowerSavingEnable);
    config.m_bPowerSavingEnable = true;
    EXPECT_EQ(config.IsFailBackMode(), !config.m_bPowerSavingEnable);
    config.m_bSmartWakeEnable = true;
    EXPECT_EQ(config.IsFailBackMode(), !config.m_bPowerSavingEnable);
}

// 根据时间获取所在时间段
TEST_F(PowerSavingConfigTest, GetTimeRangeOfTime)
{
    CPowerSavingConfig * config = NULL;
    TimeRange range;
    // 配置常见值：周一到周五7:00-19:00，周末双休7:00-7:00
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_MONDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_TUESDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_WEDNESDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_THURSDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_FRIDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_SATURDAY, 7, 7));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_SUNDAY, 7, 7));
    range = config->GetTimeRangeOfTime(1476931567L);    // 1476931567 = 2016/10/20 02:46:07 星期四
    EXPECT_EQ(range.begin,  1476903600L);               // 1476903600 = 2016/10/19 19:00:00 星期三
    EXPECT_EQ(range.end,    1476946800L);               // 1476946800 = 2016/10/20 07:00:00 星期四
    EXPECT_EQ(range.type, TIME_RANGE_REST);
    range = config->GetTimeRangeOfTime(1476946800L);    // 1476946800 = 2016/10/20 07:00:00 星期四
    EXPECT_EQ(range.begin,  1476946800L);               // 1476946800 = 2016/10/20 07:00:00 星期四
    EXPECT_EQ(range.end,    1476990000L);               // 1476990000 = 2016/10/20 19:00:00 星期四
    EXPECT_EQ(range.type, TIME_RANGE_WORK);
    delete config;
    config = NULL;
    // 空配置：全部休息
    config = new CPowerSavingConfig();
    range = config->GetTimeRangeOfTime(1476931567L);    // 1476931567 = 2016/10/20 02:46:07 星期四
    EXPECT_EQ(range.begin, TIME_MIN);
    EXPECT_EQ(range.end, TIME_MAX);
    EXPECT_EQ(range.type, TIME_RANGE_REST);
    delete config;
    config = NULL;
    // 上班时间只有一个小时
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_THURSDAY, 7, 8));
    range = config->GetTimeRangeOfTime(1476946859L);    // 1476946859 = 2016/10/20 07:00:59 星期四
    EXPECT_EQ(range.begin,  1476946800L);               // 1476946800 = 2016/10/20 07:00:00 星期四
    EXPECT_EQ(range.end,    1476950400L);               // 1476950400 = 2016/10/20 08:00:00 星期四
    EXPECT_EQ(range.type, TIME_RANGE_WORK);
    range = config->GetTimeRangeOfTime(1476931567L);    // 1476931567 = 2016/10/20 02:46:07 星期四
    EXPECT_EQ(range.begin,  1476345600L);               // 1476345600 = 2016/10/13 08:00:00 星期四
    EXPECT_EQ(range.end,    1476946800L);               // 1476946800 = 2016/10/20 07:00:00 星期四
    EXPECT_EQ(range.type, TIME_RANGE_REST);
    delete config;
    config = NULL;
    // 时间段粘连
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_WEDNESDAY, 7, 6));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_THURSDAY, 6, 19));
    range = config->GetTimeRangeOfTime(1476931567L);    // 1476931567 = 2016/10/20 02:46:07 星期四
    EXPECT_EQ(range.begin,  1476860400L);               // 1476860400 = 2016/10/19 07:00:00 星期三
    EXPECT_EQ(range.end,    1476990000L);               // 1476990000 = 2016/10/20 19:00:00 星期四
    EXPECT_EQ(range.type, TIME_RANGE_WORK);
    range = config->GetTimeRangeOfTime(1476991800L);    // 1476991800 = 2016/10/20 19:30:00 星期四
    EXPECT_EQ(range.begin,  1476990000L);               // 1476990000 = 2016/10/20 19:00:00 星期四
    EXPECT_EQ(range.end,    1477465200L);               // 1477465200 = 2016/10/26 07:00:00 星期三
    EXPECT_EQ(range.type, TIME_RANGE_REST);
    delete config;
    config = NULL;
}

// 获取下一个时间段
TEST_F(PowerSavingConfigTest, GetNextTimeRange)
{
    CPowerSavingConfig * config = NULL;
    TimeRange range;
    // 配置常见值：周一到周五7:00-19:00，周末双休7:00-7:00
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_MONDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_TUESDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_WEDNESDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_THURSDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_FRIDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_SATURDAY, 7, 7));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_SUNDAY, 7, 7));
    range = config->GetTimeRangeOfTime(1476931567L);    // 1476931567 = 2016/10/20 02:46:07 星期四
    EXPECT_EQ(range.begin, 1476903600L);                // 1476903600 = 2016/10/19 19:00:00 星期三
    EXPECT_EQ(range.end, 1476946800L);              // 1476946800 = 2016/10/20 07:00:00 星期四
    EXPECT_EQ(range.type, TIME_RANGE_REST);
    range = config->GetTimeRangeOfTime(1476946800L);    // 1476946800 = 2016/10/20 07:00:00 星期四
    EXPECT_EQ(range.begin, 1476946800L);                // 1476946800 = 2016/10/20 07:00:00 星期四
    EXPECT_EQ(range.end, 1476990000L);              // 1476990000 = 2016/10/20 19:00:00 星期四
    EXPECT_EQ(range.type, TIME_RANGE_WORK);
    delete config;
    config = NULL;
    // 空配置：全部休息
    config = new CPowerSavingConfig();
    range = config->GetTimeRangeOfTime(1476931567L);    // 1476931567 = 2016/10/20 02:46:07 星期四
    EXPECT_EQ(range.begin, TIME_MIN);
    EXPECT_EQ(range.end, TIME_MAX);
    EXPECT_EQ(range.type, TIME_RANGE_REST);
    range = config->GetNextTimeRange(range);
    EXPECT_EQ(range.begin, TIME_MIN);
    EXPECT_EQ(range.end, TIME_MAX);
    EXPECT_EQ(range.type, TIME_RANGE_REST);
    delete config;
    config = NULL;
    // 上班时间只有一个小时
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_THURSDAY, 7, 8));
    range = config->GetTimeRangeOfTime(1476946859L);    // 1476946859 = 2016/10/20 07:00:59 星期四
    EXPECT_EQ(range.begin, 1476946800L);                // 1476946800 = 2016/10/20 07:00:00 星期四
    EXPECT_EQ(range.end, 1476950400L);              // 1476950400 = 2016/10/20 08:00:00 星期四
    EXPECT_EQ(range.type, TIME_RANGE_WORK);
    range = config->GetTimeRangeOfTime(1476931567L);    // 1476931567 = 2016/10/20 02:46:07 星期四
    EXPECT_EQ(range.begin, 1476345600L);                // 1476345600 = 2016/10/13 08:00:00 星期四
    EXPECT_EQ(range.end, 1476946800L);              // 1476946800 = 2016/10/20 07:00:00 星期四
    EXPECT_EQ(range.type, TIME_RANGE_REST);
    delete config;
    config = NULL;
}

// 获取主动/被动操作所在时段进入节电的延时
TEST_F(PowerSavingConfigTest, GetIntervalOfTimeRange)
{
    CPowerSavingConfig * config = NULL;
    TimeRange range;
    // 配置默认值
    config = new CPowerSavingConfig();
    config->m_bPowerSavingEnable = true;
    config->m_timeActiveInterval = 10 * 60;
    config->m_timePassiveIntervalOfWork = 360 * 60;
    config->m_timePassiveIntervalOfRest = 10 * 60;
    config->m_timeFailbackModeInterval = 4320 * 60;
    // 校验各个值
    range.type = TIME_RANGE_WORK;
    EXPECT_EQ(360 * 60, config->GetIntervalOfTimeRange(range,
              OPERATION_ACTIVE)); // 主动操作应该取主动和被动的最大值
    EXPECT_EQ(360 * 60, config->GetIntervalOfTimeRange(range, OPERATION_PASSIVE));
    range.type = TIME_RANGE_REST;
    EXPECT_EQ(10 * 60, config->GetIntervalOfTimeRange(range, OPERATION_ACTIVE));
    EXPECT_EQ(10 * 60, config->GetIntervalOfTimeRange(range, OPERATION_PASSIVE));
    // 主动操作比被动操作延时长
    config->m_timeActiveInterval = 60 * 60;
    range.type = TIME_RANGE_WORK;
    EXPECT_EQ(360 * 60, config->GetIntervalOfTimeRange(range, OPERATION_ACTIVE));
    EXPECT_EQ(360 * 60, config->GetIntervalOfTimeRange(range, OPERATION_PASSIVE));
    range.type = TIME_RANGE_REST;
    EXPECT_EQ(60 * 60, config->GetIntervalOfTimeRange(range,
              OPERATION_ACTIVE)); // 主动操作应该取主动和被动的最大值
    EXPECT_EQ(10 * 60, config->GetIntervalOfTimeRange(range, OPERATION_PASSIVE));
    // 禁用节电模式，进入容灾模式：主动操作使用容灾值，被动操作使用空闲时间被动操作值
    config->m_bPowerSavingEnable = false;
    range.type = TIME_RANGE_WORK;
    EXPECT_EQ(config->m_timeFailbackModeInterval,
              config->GetIntervalOfTimeRange(range, OPERATION_ACTIVE));
    EXPECT_EQ(config->m_timePassiveIntervalOfRest,
              config->GetIntervalOfTimeRange(range, OPERATION_PASSIVE));
    range.type = TIME_RANGE_REST;
    EXPECT_EQ(config->m_timeFailbackModeInterval,
              config->GetIntervalOfTimeRange(range, OPERATION_ACTIVE));
    EXPECT_EQ(config->m_timePassiveIntervalOfRest,
              config->GetIntervalOfTimeRange(range, OPERATION_PASSIVE));
}

// 获取时间所在时段已经开始的小时数
TEST_F(PowerSavingConfigTest, GetHoursAfterTimeRangeBegin)
{
    CPowerSavingConfig * config = NULL;
    // 配置常见值：周一到周五7:00-19:00，周末双休7:00-7:00
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_MONDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_TUESDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_WEDNESDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_THURSDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_FRIDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_SATURDAY, 7, 7));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_SUNDAY, 7, 7));
    // 工作时间中段
    EXPECT_EQ(config->GetHoursAfterTimeRangeBegin(WEEKDAY_MONDAY, 10), 3);
    // 工作时间起始
    EXPECT_EQ(config->GetHoursAfterTimeRangeBegin(WEEKDAY_TUESDAY, 7), 0);
    // 工作时间结束
    EXPECT_EQ(config->GetHoursAfterTimeRangeBegin(WEEKDAY_WEDNESDAY, 18), 11);
    // 休息时间中段
    EXPECT_EQ(config->GetHoursAfterTimeRangeBegin(WEEKDAY_THURSDAY, 3), 8);
    // 休息时间起始
    EXPECT_EQ(config->GetHoursAfterTimeRangeBegin(WEEKDAY_FRIDAY, 19), 0);
    // 休息时间结束
    EXPECT_EQ(config->GetHoursAfterTimeRangeBegin(WEEKDAY_MONDAY, 6), 59);
    delete config;
    config = NULL;
    // 空配置：全部休息
    config = new CPowerSavingConfig();
    EXPECT_EQ(config->GetHoursAfterTimeRangeBegin(WEEKDAY_MONDAY, 10), -1);
    EXPECT_EQ(config->GetHoursAfterTimeRangeBegin(WEEKDAY_SUNDAY, 0), -1);
    delete config;
    config = NULL;
    // 上班时间只有一个小时
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_MONDAY, 7, 8));
    EXPECT_EQ(config->GetHoursAfterTimeRangeBegin(WEEKDAY_MONDAY, 7), 0);
    EXPECT_EQ(config->GetHoursAfterTimeRangeBegin(WEEKDAY_MONDAY, 8), 0);
    EXPECT_EQ(config->GetHoursAfterTimeRangeBegin(WEEKDAY_MONDAY, 12), 4);
    delete config;
    config = NULL;
}

// 获取时间所在时段还将持续的小时数
TEST_F(PowerSavingConfigTest, GetHoursBeforeTimeRangeEnd)
{
    CPowerSavingConfig * config = NULL;
    // 配置常见值：周一到周五7:00-19:00，周末双休7:00-7:00
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_MONDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_TUESDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_WEDNESDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_THURSDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_FRIDAY, 7, 19));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_SATURDAY, 7, 7));
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_SUNDAY, 7, 7));
    // 工作时间中段
    EXPECT_EQ(config->GetHoursBeforeTimeRangeEnd(WEEKDAY_MONDAY, 10), 9);
    // 工作时间起始
    EXPECT_EQ(config->GetHoursBeforeTimeRangeEnd(WEEKDAY_TUESDAY, 7), 12);
    // 工作时间结束
    EXPECT_EQ(config->GetHoursBeforeTimeRangeEnd(WEEKDAY_WEDNESDAY, 18), 1);
    // 休息时间中段
    EXPECT_EQ(config->GetHoursBeforeTimeRangeEnd(WEEKDAY_THURSDAY, 3), 4);
    // 休息时间起始
    EXPECT_EQ(config->GetHoursBeforeTimeRangeEnd(WEEKDAY_FRIDAY, 19), 60);
    // 休息时间结束
    EXPECT_EQ(config->GetHoursBeforeTimeRangeEnd(WEEKDAY_MONDAY, 6), 1);
    delete config;
    config = NULL;
    // 空配置：全部休息
    config = new CPowerSavingConfig();
    EXPECT_EQ(config->GetHoursBeforeTimeRangeEnd(WEEKDAY_MONDAY, 10), -1);
    EXPECT_EQ(config->GetHoursBeforeTimeRangeEnd(WEEKDAY_SUNDAY, 0), -1);
    delete config;
    config = NULL;
    // 上班时间只有一个小时
    config = new CPowerSavingConfig();
    EXPECT_TRUE(config->AddWeekTimeRangeOfWork(WEEKDAY_MONDAY, 7, 8));
    EXPECT_EQ(config->GetHoursBeforeTimeRangeEnd(WEEKDAY_MONDAY, 7), 1);
    EXPECT_EQ(config->GetHoursBeforeTimeRangeEnd(WEEKDAY_MONDAY, 8), 167);
    EXPECT_EQ(config->GetHoursBeforeTimeRangeEnd(WEEKDAY_MONDAY, 3), 4);
    delete config;
    config = NULL;
}

// 获取主动操作对应的进入节电的延时
TEST_F(PowerSavingConfigTest, GetActiveOperationInterval)
{
    CPowerSavingConfig * config = NULL;
    // 配置常见值
    config = new CPowerSavingConfig();
    config->m_bPowerSavingEnable = true;
    config->m_bSmartWakeEnable = true;
    config->m_timeActiveInterval = 10 * 60;
    config->m_timePassiveIntervalOfWork = 360 * 60;
    config->m_timePassiveIntervalOfRest = 10 * 60;
    config->m_timeFailbackModeInterval = 4320 * 60;
    // 配置值
    EXPECT_EQ(10 * 60, config->GetActiveOperationInterval());
    // 不受是否启用智能点亮影响
    config->m_bSmartWakeEnable = false;
    EXPECT_EQ(10 * 60, config->GetActiveOperationInterval());
    // 禁用节电功能时采用容灾值
    config->m_bPowerSavingEnable = false;
    EXPECT_EQ(4320 * 60, config->GetActiveOperationInterval());
    delete config;
    config = NULL;
}

// 获取被动操作对应的进入节电的延时
TEST_F(PowerSavingConfigTest, GetPassiveOperationInterval)
{
    CPowerSavingConfig * config = NULL;
    // 配置常见值
    config = new CPowerSavingConfig();
    config->m_bPowerSavingEnable = true;
    config->m_bSmartWakeEnable = true;
    config->m_timeActiveInterval = 10 * 60;
    config->m_timePassiveIntervalOfWork = 360 * 60;
    config->m_timePassiveIntervalOfRest = 10 * 60;
    config->m_timeFailbackModeInterval = 4320 * 60;
    // 配置值
    EXPECT_EQ(360 * 60, config->GetPassiveOperationInterval(TIME_RANGE_WORK));
    EXPECT_EQ(10 * 60, config->GetPassiveOperationInterval(TIME_RANGE_REST));
    // 不受是否启用智能点亮影响
    config->m_bSmartWakeEnable = false;
    EXPECT_EQ(360 * 60, config->GetPassiveOperationInterval(TIME_RANGE_WORK));
    EXPECT_EQ(10 * 60, config->GetPassiveOperationInterval(TIME_RANGE_REST));
    // 禁用节电功能，进入容灾模式：使用空闲时间被动操作值
    config->m_bPowerSavingEnable = false;
    EXPECT_EQ(10 * 60, config->GetPassiveOperationInterval(TIME_RANGE_WORK));
    EXPECT_EQ(10 * 60, config->GetPassiveOperationInterval(TIME_RANGE_REST));
    delete config;
    config = NULL;
}
