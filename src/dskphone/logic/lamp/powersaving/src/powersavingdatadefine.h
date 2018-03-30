#ifndef __POWERSAVING_DATA_DEFINE_H__
#define __POWERSAVING_DATA_DEFINE_H__

#include <ctime>

// Log
#if IS_CODE_TEST
#include <stdio.h>
#define POWERSAVING_LOG(fmt, ...)\
    printf(fmt "\n", ##__VA_ARGS__)
#define POWERSAVING_DEBUG   //POWERSAVING_LOG
#define POWERSAVING_INFO    POWERSAVING_LOG
#define POWERSAVING_NOTICE  POWERSAVING_LOG
#define POWERSAVING_WARN    POWERSAVING_LOG
#define POWERSAVING_ERR     POWERSAVING_LOG
#else
#include <dsklog/log.h>
#define POWERSAVING_DEBUG   BACKLIGHT_DEBUG
#define POWERSAVING_INFO    BACKLIGHT_INFO
#define POWERSAVING_NOTICE  BACKLIGHT_NOTICE
#define POWERSAVING_WARN    BACKLIGHT_WARN
#define POWERSAVING_ERR     BACKLIGHT_ERR
#endif // IS_CODE_TEST


// 操作类型
enum OPERATION_TYPE
{
    OPERATION_PASSIVE, // 被动操作
    OPERATION_ACTIVE // 主动操作
};

// 时间区段类型
enum TIME_RANGE_TYPE
{
    TIME_RANGE_WORK, // 工作时间
    TIME_RANGE_REST // 休息时间
};

// 一周内的日子名称
enum WEEKDAY_TYPE
{
    WEEKDAY_MONDAY,
    WEEKDAY_TUESDAY,
    WEEKDAY_WEDNESDAY,
    WEEKDAY_THURSDAY,
    WEEKDAY_FRIDAY,
    WEEKDAY_SATURDAY,
    WEEKDAY_SUNDAY
};

// 节电状态
enum POWERSAVING_STATUS
{
    POWERSAVING_STATUS_WAITING,     // 空闲状态，等待进入节电
    POWERSAVING_STATUS_FORBIDDEN,   // 禁止进入节电状态，如通话中
    POWERSAVING_STATUS_PRE_SAVING,  // 预节电状态，发生于进入节电前，先调低背光亮度
    POWERSAVING_STATUS_SAVING       // 节电状态，关闭背光，彩屏UI覆盖黑色图片并闪电源灯
};

// 时间类型
typedef time_t Time;        // 时间
typedef time_t TimeDiff;    // 时间间隔
typedef short Hour;         // 点钟
typedef short Hours;        // 小时数
#define TIME_MIN    ((Time)(1LL << (sizeof(time_t) * 8 - 1)))       // 时间最小值（兼容32/64位时间类型）
#define TIME_MAX    ((Time)((1LL << (sizeof(time_t) * 8 - 1)) - 1)) // 时间最大值（兼容32/64位时间类型）

// 时间区段
struct TimeRange
{
    Time begin;
    Time end;
    TIME_RANGE_TYPE type;
};

#endif // !__POWERSAVING_DATA_DEFINE_H__

