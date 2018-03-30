#ifndef __DATETIME_HELPER_H__
#define __DATETIME_HELPER_H__

#include "powersavingdatadefine.h"


class CDateTimeHelper
{
private:
    CDateTimeHelper();

public:
    static Time GetLocalTime();
    // 获取有效的小时数（[0,23]的整数，大于23则返回23，小于0则返回0）
    static Hour ValidateHour(Hour hour);
    // 获取一周中的下一天
    static WEEKDAY_TYPE GetNextWeekday(WEEKDAY_TYPE day);
    // 把时间点（单位：s）转换为一周中的星期几和整点数
    static void TimeToWeekdayAndHour(Time time, WEEKDAY_TYPE & day, Hour & hour);
    // 把时间点（单位：s）的分钟数和秒数裁剪掉，得到前一个整点时间
    static Time TimeTrimMinAndSec(Time time);
};

#endif // !__DATETIME_HELPER_H__
