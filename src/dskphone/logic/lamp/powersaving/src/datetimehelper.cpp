#include "datetimehelper.h"
#include "service_system.h"

CDateTimeHelper::CDateTimeHelper()
{
}

Time CDateTimeHelper::GetLocalTime()
{
    sys_Time_t timer;
    sys_get_systime(&timer);
    return mktime(&timer.sys_tm) + timer.sys_tm.tm_gmtoff;
}

Hour CDateTimeHelper::ValidateHour(Hour hour)
{
    if (hour < 0)
    {
        return 0;
    }
    if (hour > 23)
    {
        return 23;
    }
    return hour;
}

WEEKDAY_TYPE CDateTimeHelper::GetNextWeekday(WEEKDAY_TYPE day)
{
    short sDay = day;
    ++sDay;
    if (sDay > 6)
    {
        sDay = 0;
    }
    return (WEEKDAY_TYPE)sDay;
}

void CDateTimeHelper::TimeToWeekdayAndHour(Time time, WEEKDAY_TYPE & day, Hour & hour)
{
    // POSIX时间戳的起点1970年1月1日是星期四，裁剪为从周一开始计算
    Time timeInWeek = time - 4 * 24 * 60 * 60;
    // 裁剪掉分与秒，留下的单位为小时（这里不能用Hours类型存储，因为Hours长度不够）
    Time timeHours = timeInWeek / (60 * 60);
    // 计算点钟数
    hour = timeHours % 24;
    // 裁剪掉小时，留下的单位为天
    Time timeDays = timeHours / 24;
    // 计算星期几
    day = (WEEKDAY_TYPE)(timeDays % 7);
}

Time CDateTimeHelper::TimeTrimMinAndSec(Time time)
{
    return (time / 3600) * 3600;
}
