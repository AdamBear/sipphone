#include "powersavingconfig.h"

#include <configiddefine.h>
#include <modconfigparser.h>
#include <commonunits/modcommonunits.h>
#include <ETLLib.hpp> // chMAX

#include "datetimehelper.h"


CPowerSavingConfig::CPowerSavingConfig()
{
    m_bPowerSavingEnable = true;
    m_bSmartWakeEnable = true;
    m_iPreSavingBacklightLevel = 1;
    m_timeActiveInterval = 10 * 60;
    m_timePassiveIntervalOfWork = 360 * 60;
    m_timePassiveIntervalOfRest = 10 * 60;
    m_timeFailbackModeInterval = 4320 * 60;
    // 一周时段赋初值，全部初始化为休息时间
    for (size_t day = 0; day < 7; day++)
    {
        for (Hour hour = 0; hour < 24; hour++)
        {
            m_arrWorkHoursWeekly[day][hour] = TIME_RANGE_REST;
        }
    }
}

void CPowerSavingConfig::Load()
{
    m_bPowerSavingEnable = configParser_GetConfigInt(kszPowerSavingEnable);
    m_bSmartWakeEnable = configParser_GetConfigInt(kszPowerSavingIntelligentMode);
    m_timeActiveInterval = configParser_GetConfigInt(kszPowerSavingUserInputTimeout) * 60;
    m_timeActiveInterval = m_timeActiveInterval < 60 ? 60 : m_timeActiveInterval;
    m_timePassiveIntervalOfWork = configParser_GetConfigInt(kszPowerSavingOfficeTimeout) * 60;
    m_timePassiveIntervalOfWork = m_timePassiveIntervalOfWork < 60 ? 60 : m_timePassiveIntervalOfWork;
    m_timePassiveIntervalOfRest = configParser_GetConfigInt(kszPowerSavingOffTimeout) * 60;
    m_timePassiveIntervalOfRest = m_timePassiveIntervalOfRest < 60 ? 60 : m_timePassiveIntervalOfRest;
    m_timeFailbackModeInterval = configParser_GetConfigInt(kszPowerSavingSrceenProtectTime) * 60;
    m_timeFailbackModeInterval = m_timeFailbackModeInterval < 60 ? 60 : m_timeFailbackModeInterval;
    m_iPreSavingBacklightLevel = configParser_GetConfigInt(kszInactiveLevel);
    // 一周时段赋初值，全部初始化为休息时间
    for (size_t day = 0; day < 7; day++)
    {
        for (Hour hour = 0; hour < 24; hour++)
        {
            m_arrWorkHoursWeekly[day][hour] = TIME_RANGE_REST;
        }
    }
    // 加载一周内每天的工作时间配置
    LoadWorkTimeOfDay(WEEKDAY_MONDAY, kszPowerSavingOfficeMonday);
    LoadWorkTimeOfDay(WEEKDAY_TUESDAY, kszPowerSavingOfficeTuesday);
    LoadWorkTimeOfDay(WEEKDAY_WEDNESDAY, kszPowerSavingOfficeWednesday);
    LoadWorkTimeOfDay(WEEKDAY_THURSDAY, kszPowerSavingOfficeThursday);
    LoadWorkTimeOfDay(WEEKDAY_FRIDAY, kszPowerSavingOfficeFriday);
    LoadWorkTimeOfDay(WEEKDAY_SATURDAY, kszPowerSavingOfficeSaturday);
    LoadWorkTimeOfDay(WEEKDAY_SUNDAY, kszPowerSavingOfficeSunday);
}

TimeRange CPowerSavingConfig::GetTimeRangeOfTime(Time time) const
{
    WEEKDAY_TYPE day = WEEKDAY_MONDAY;
    Hour hour = 0;
    // 把时间点转换为一周中的星期几和点钟数
    CDateTimeHelper::TimeToWeekdayAndHour(time, day, hour);
    POWERSAVING_DEBUG("Time[%ld]: day=%d, hour=%d", time, day, hour);
    TimeRange range;
    range.type = m_arrWorkHoursWeekly[day][hour]; // 时间点对应的时段类型
    Hours begin_hours = GetHoursAfterTimeRangeBegin(day, hour); // 获取时段已经开始的小时数
    POWERSAVING_DEBUG("HoursAfterTimeRangeBegin=%d", begin_hours);
    if (begin_hours < 0)
    {
        range.begin = TIME_MIN;
    }
    else
    {
        // 计算时段开始时间
        range.begin = CDateTimeHelper::TimeTrimMinAndSec(time) - begin_hours * 60 * 60;
    }
    Hours end_hours = GetHoursBeforeTimeRangeEnd(day, hour); // 获取时段还将持续的小时数
    POWERSAVING_DEBUG("GetHoursBeforeTimeRangeEnd=%d", end_hours);
    if (end_hours < 0)
    {
        range.end = TIME_MAX;
    }
    else
    {
        // 计算时段结束时间
        range.end = CDateTimeHelper::TimeTrimMinAndSec(time) + end_hours * 60 * 60;
    }
    return range;
}

TimeRange CPowerSavingConfig::GetNextTimeRange(const TimeRange & range) const
{
    // 时段是半闭半开区间，因此获取下一个时段可以直接根据上一时段的结束时间获取
    return GetTimeRangeOfTime(range.end);
}

TimeDiff CPowerSavingConfig::GetIntervalOfTimeRange(const TimeRange & range,
        OPERATION_TYPE operation) const
{
    if (IsFailBackMode()) // 容灾模式
    {
        if (operation == OPERATION_ACTIVE)
        {
            // 主动操作：使用容灾值
            return m_timeFailbackModeInterval;
        }
        else
        {
            // 被动操作：使用休息时间被动操作值
            return m_timePassiveIntervalOfRest;
        }
    }
    else
    {
        if (operation == OPERATION_ACTIVE)
        {
            // 主动操作：使用主动操作和被动操作的最大值
            return chMAX(GetPassiveOperationInterval(range.type), GetActiveOperationInterval());
        }
        else // OPERATION_PASSIVE
        {
            // 被动操作：使用被动操作值
            return GetPassiveOperationInterval(range.type);
        }
    }
}

bool CPowerSavingConfig::IsPowerSavingEnable() const
{
    return m_bPowerSavingEnable;
}

bool CPowerSavingConfig::IsFailBackMode() const
{
    return !m_bPowerSavingEnable;
}

bool CPowerSavingConfig::IsSmartWakeEnable() const
{
    return m_bSmartWakeEnable;
}

int CPowerSavingConfig::GetPreSavingBacklightLevel() const
{
    return m_iPreSavingBacklightLevel;
}

bool CPowerSavingConfig::LoadWorkTimeOfDay(WEEKDAY_TYPE day, const char * cszConfigName)
{
    yl::string strConfigValue = configParser_GetConfigString(cszConfigName);
    POWERSAVING_DEBUG("Config of day #%d: %s", day, strConfigValue.c_str());
    Hour hourBegin = 0, hourEnd = 0;
    int iScanfResults = sscanf(strConfigValue.c_str(), "%2hd,%2hd", &hourBegin, &hourEnd);
    POWERSAVING_DEBUG("sscanf=%d, begin=%hd, end=%hd", iScanfResults, hourBegin, hourEnd);
    if (iScanfResults != 2)
    {
        yl::string strDefaultValue = configParser_GetDefaultValueString(cszConfigName);
        POWERSAVING_WARN("Config wrong format! [Day#%d: %s] Using default value: %s",
                         day, strConfigValue.c_str(), strDefaultValue.c_str());
        iScanfResults = sscanf(strDefaultValue.c_str(), "%2hd,%2hd", &hourBegin, &hourEnd);
        if (iScanfResults != 2)
        {
            POWERSAVING_ERR("Default value wrong format! [Day#%d: %s]",
                            day, strDefaultValue.c_str());
            return false;
        }
    }
    hourBegin = CDateTimeHelper::ValidateHour(hourBegin);
    hourEnd = CDateTimeHelper::ValidateHour(hourEnd);
    return AddWeekTimeRangeOfWork(day, hourBegin, hourEnd);
}

bool CPowerSavingConfig::AddWeekTimeRangeOfWork(WEEKDAY_TYPE day, Hour hourBegin, Hour hourEnd)
{
    hourBegin = CDateTimeHelper::ValidateHour(hourBegin);
    hourEnd = CDateTimeHelper::ValidateHour(hourEnd);
    if (hourBegin == hourEnd) // 空时段
    {
        return true;
    }
    if (hourBegin < hourEnd) // 时段不跨天
    {
        for (Hour h = hourBegin; h < hourEnd; ++h)
        {
            m_arrWorkHoursWeekly[day][h] = TIME_RANGE_WORK;
        }
    }
    else // hourBegin > hourEnd： 时段跨天
    {
        for (Hour h = hourBegin; h < 24; ++h)
        {
            m_arrWorkHoursWeekly[day][h] = TIME_RANGE_WORK;
        }
        day = CDateTimeHelper::GetNextWeekday(day);
        for (Hour h = 0; h < hourEnd; ++h)
        {
            m_arrWorkHoursWeekly[day][h] = TIME_RANGE_WORK;
        }
    }
    return true;
}

Hours CPowerSavingConfig::GetHoursAfterTimeRangeBegin(WEEKDAY_TYPE day, Hour hour) const
{
    POWERSAVING_DEBUG("CPowerSavingConfig::GetHoursAfterTimeRangeBegin(%d, %d)", day, hour);
    short day_mark = day;
    Hour hour_mark = hour;
    POWERSAVING_DEBUG("WorkHourType[%d][%d]=%d",
                      day_mark, hour_mark, m_arrWorkHoursWeekly[day_mark][hour_mark]);
    Hours result_hours = 0;
    do
    {
        // 时间前移
        --hour_mark;
        if (hour_mark < 0)
        {
            hour_mark = 23;
            --day_mark;
            if (day_mark < 0)
            {
                day_mark = 6;
            }
        }
        POWERSAVING_DEBUG("WorkHourType[%d][%d]=%d",
                          day_mark, hour_mark, m_arrWorkHoursWeekly[day_mark][hour_mark]);
        // 判断时段类型是否一致，一致则扩展时段并继续遍历，否则退出并返回结果
        if (m_arrWorkHoursWeekly[day][hour] == m_arrWorkHoursWeekly[day_mark][hour_mark])
        {
            ++result_hours;
        }
        else
        {
            return result_hours;
        }
    }
    while (day != day_mark || hour != hour_mark);   // 判断是否已经遍历一周内的所有时间
    return -1;
}

Hours CPowerSavingConfig::GetHoursBeforeTimeRangeEnd(WEEKDAY_TYPE day, Hour hour) const
{
    POWERSAVING_DEBUG("CPowerSavingConfig::GetHoursBeforeTimeRangeEnd(%d, %d)", day, hour);
    short day_mark = day;
    Hour hour_mark = hour;
    POWERSAVING_DEBUG("WorkHourType[%d][%d]=%d",
                      day_mark, hour_mark, m_arrWorkHoursWeekly[day_mark][hour_mark]);
    Hours result_hours = 1;
    do
    {
        // 时间后移
        ++hour_mark;
        if (hour_mark > 23)
        {
            hour_mark = 0;
            ++day_mark;
            if (day_mark > 6)
            {
                day_mark = 0;
            }
        }
        POWERSAVING_DEBUG("WorkHourType[%d][%d]=%d",
                          day_mark, hour_mark, m_arrWorkHoursWeekly[day_mark][hour_mark]);
        // 判断时段类型是否一致，一致则扩展时段并继续遍历，否则退出并返回结果
        if (m_arrWorkHoursWeekly[day][hour] == m_arrWorkHoursWeekly[day_mark][hour_mark])
        {
            ++result_hours;
        }
        else
        {
            return result_hours;
        }
    }
    while (day != day_mark || hour != hour_mark);   // 判断是否已经遍历一周内的所有时间
    return -1;
}

TimeDiff CPowerSavingConfig::GetActiveOperationInterval() const
{
    if (IsFailBackMode())
    {
        return m_timeFailbackModeInterval;
    }
    return m_timeActiveInterval;
}

TimeDiff CPowerSavingConfig::GetPassiveOperationInterval(TIME_RANGE_TYPE type) const
{
    if (IsFailBackMode())
    {
        return m_timePassiveIntervalOfRest;
    }
    if (type == TIME_RANGE_WORK)
    {
        return m_timePassiveIntervalOfWork;
    }
    else // TIME_RANGE_REST
    {
        return m_timePassiveIntervalOfRest;
    }
}
