#include <gtest/gtest.h>

#include "datetimehelper.h"

#include "mocklibs/sysservice/mockservice_system.h"


TEST(DateTimeHelper, GetLocalTime)
{
    testing::StrictMock<MockSysService> mockSysService;
    g_pMockSysService = &mockSysService;
    EXPECT_GET_SYS_TIME(1511784000L, 0);
    EXPECT_EQ(CDateTimeHelper::GetLocalTime(), 1511784000L);
    EXPECT_GET_SYS_TIME(1511766000L, 28800);
    EXPECT_EQ(CDateTimeHelper::GetLocalTime(), 1511794800L);
    g_pMockSysService = NULL;
}

TEST(DateTimeHelper, ValidateHour)
{
    EXPECT_EQ(CDateTimeHelper::ValidateHour(-1), 0);
    EXPECT_EQ(CDateTimeHelper::ValidateHour(0), 0);
    EXPECT_EQ(CDateTimeHelper::ValidateHour(8), 8);
    EXPECT_EQ(CDateTimeHelper::ValidateHour(23), 23);
    EXPECT_EQ(CDateTimeHelper::ValidateHour(24), 23);
}

TEST(DateTimeHelper, GetNextWeekday)
{
    EXPECT_EQ(CDateTimeHelper::GetNextWeekday(WEEKDAY_MONDAY), WEEKDAY_TUESDAY);
    EXPECT_EQ(CDateTimeHelper::GetNextWeekday(WEEKDAY_FRIDAY), WEEKDAY_SATURDAY);
    EXPECT_EQ(CDateTimeHelper::GetNextWeekday(WEEKDAY_SATURDAY), WEEKDAY_SUNDAY);
    EXPECT_EQ(CDateTimeHelper::GetNextWeekday(WEEKDAY_SUNDAY), WEEKDAY_MONDAY);
}

TEST(DateTimeHelper, TimeToWeekdayAndHour)
{
    WEEKDAY_TYPE day = WEEKDAY_SUNDAY;
    Hour hour = 0;
    CDateTimeHelper::TimeToWeekdayAndHour(1234567890, day, hour);
    EXPECT_EQ(day, WEEKDAY_FRIDAY);
    EXPECT_EQ(hour, 23);
    CDateTimeHelper::TimeToWeekdayAndHour(1234566000, day, hour);
    EXPECT_EQ(day, WEEKDAY_FRIDAY);
    EXPECT_EQ(hour, 23);
    CDateTimeHelper::TimeToWeekdayAndHour(655693199, day, hour);
    EXPECT_EQ(day, WEEKDAY_FRIDAY);
    EXPECT_EQ(hour, 0);
    CDateTimeHelper::TimeToWeekdayAndHour(655689600, day, hour);
    EXPECT_EQ(day, WEEKDAY_FRIDAY);
    EXPECT_EQ(hour, 0);
    // 1456704000 = 2016年2月29日 星期一 00:00:00
    CDateTimeHelper::TimeToWeekdayAndHour(1456704000, day, hour);
    EXPECT_EQ(day, WEEKDAY_MONDAY);
    EXPECT_EQ(hour, 0);
}

TEST(DateTimeHelper, TimeTrimMinAndSec)
{
    EXPECT_EQ(CDateTimeHelper::TimeTrimMinAndSec(1234567890), 1234566000);
    EXPECT_EQ(CDateTimeHelper::TimeTrimMinAndSec(655693199), 655689600);
}
