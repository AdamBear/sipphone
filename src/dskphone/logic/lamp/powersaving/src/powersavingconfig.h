#ifndef __POWERSAVING_CONFIG_H__
#define __POWERSAVING_CONFIG_H__

#include <config.in.h> // IS_CODE_TEST

#include "powersavingdatadefine.h"

class CPowerSavingConfig
{
public:
    // 构造函数
    CPowerSavingConfig();
    // 加载配置
    void Load();
    // 根据时间点获取所在时间段
    TimeRange GetTimeRangeOfTime(Time time) const;
    // 获取时间段的下一个时间段
    TimeRange GetNextTimeRange(const TimeRange & range) const;
    // 根据时间段与操作类型获取进入节电状态的延时
    TimeDiff GetIntervalOfTimeRange(const TimeRange & range, OPERATION_TYPE operation) const;
    // 判断是否启用节电模式
    bool IsPowerSavingEnable() const;
    // 判断是否容灾模式（禁用节电模式即为容灾模式）
    bool IsFailBackMode() const;
    // 判断是否启用智能点亮
    bool IsSmartWakeEnable() const;
    // 获取预节电状态的背光灯亮度
    int GetPreSavingBacklightLevel() const;

#if IS_CODE_TEST
public:
#else
private:
#endif // IS_CODE_TEST

    bool m_bPowerSavingEnable; // 是否启用节电模式
    bool m_bSmartWakeEnable; // 是否启用智能点亮
    TimeDiff m_timeActiveInterval; // 主动操作后进入节电的时间间隔（单位：s）
    TimeDiff m_timePassiveIntervalOfWork; // 工作时间被动操作后进入节电的时间间隔（单位：s）
    TimeDiff m_timePassiveIntervalOfRest; // 休息时间被动操作后进入节电的时间间隔（单位：s）
    TimeDiff m_timeFailbackModeInterval; // 容灾模式进入节电的时间间隔（单位：s）
    int m_iPreSavingBacklightLevel; // 预节电模式背光灯亮度
    TIME_RANGE_TYPE
    m_arrWorkHoursWeekly[7][24]; // 一周内每天每小时的时段类型（是否工作时间）

    // 从配置中加载一周中某天的工作时间
    bool LoadWorkTimeOfDay(WEEKDAY_TYPE day, const char * cszConfigName);
    // 添加一周中某天的工作时间
    bool AddWeekTimeRangeOfWork(WEEKDAY_TYPE day, Hour hourBegin, Hour hourEnd);
    // 获取一周中某个整点时间距离所在时段开始时间的小时数（单位：h），一整周都是同一时段类型（时段没有开始时间）则返回-1
    Hours GetHoursAfterTimeRangeBegin(WEEKDAY_TYPE day, Hour hour) const;
    // 获取一周中某个整点时间距离所在时段结束时间的小时数（单位：h），一整周都是同一时段类型（时段没有开始时间）则返回-1
    Hours GetHoursBeforeTimeRangeEnd(WEEKDAY_TYPE day, Hour hour) const;
    // 获取主动操作对应的进入节电状态的延时（单位：s）
    TimeDiff GetActiveOperationInterval() const;
    // 获取指定时段类型的被动操作对应进入节电状态的延时（单位：s）
    TimeDiff GetPassiveOperationInterval(TIME_RANGE_TYPE type) const;
};

#endif // !__POWERSAVING_CONFIG_H__
