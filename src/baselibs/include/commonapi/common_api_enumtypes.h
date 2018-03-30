#ifndef _COMMON_APIT_ENUMTYPES_HEADER_
#define _COMMON_APIT_ENUMTYPES_HEADER_
// 字符串转换方式.
enum StringConvertType
{
    STRING_CONVERT_TYPE_NONE = 0,  // 不正确的转换方式.
    STRING_CONVERT_TO_LOWER = 1,  // 将字符串转换为小写.
    STRING_CONVERT_TO_UPPER = 2,  // 将字符串转换为大写.
    STRING_CONVERT_TYPE_MAX
};

enum StringMatchType
{
    LEAD_MATCH = 0, //前置匹配
    CONTAIN_MATCH,   //包含匹配，及模糊匹配，如src=1234，key= 23，匹配成功
    TOTAL_MATH,
    TAIL_MATCH,
    TOTAL_TAIL_MATCH
};

/***********************************************************************
**                  字符串公共函数
***********************************************************************/
// Enum, trim type.
enum TrimType
{
    TRIM_TYPE_NONE = 0,  // Trim none.
    TRIM_LEFT = 1 << 0,  // Trim left.
    TRIM_RIGHT = 1 << 1,  // Trim right.
    TRIM_ALL = TRIM_LEFT | TRIM_RIGHT,  // Trim all.
    TRIM_TYPE_MAX
};

enum TIME_FORMAT_TYPE
{
    TM_TYPE_DEFAULT,
    TM_TYPE_DATE,  //只要日期，不要具体时间
    TM_TYPE_TIME,  //只要时间，不要具体日期
};
#endif
