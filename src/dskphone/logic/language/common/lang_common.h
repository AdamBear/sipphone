#pragma once

#include <config.h>
#include <ylvector.h>
#include <yllist.h>
#include <ylstringalgorithm.h>
#include <ETLLib.hpp>
#include <pugixml.hpp>
#include "singletonbase.h"
#include "lang_config.h"

// 用于兼容旧版本输入法，标识一类输入法的子输入法
// 实际的输入法类型
enum IME_ITEM_TYPE
{
    IT_STD_NONE = -1,
    IT_STD_2aB,
    IT_STD_123,
    IT_STD_ABC,
    IT_STD_abc,
    IT_STD_Dial,
    IT_STD_IP,
    IT_STD_Pwd_abc,
    IT_STD_Pwd_ABC,
    IT_STD_Pwd_2aB,
    IT_STD_Pwd_123,
    IT_STD_Custom,  // 为了兼容老本版，hebrew等输入法，抽象出来的一个自定义输入法
    IT_STD_Abc,
    IT_STD_LAST,
};

// 定义各种输入法对应的字符串
#define IME_DIY         "diy"
#define IME_123         "123"
#define IME_abc         "abc"
#define IME_ABC         "ABC"
#define IME_Abc         "Abc"
#define IME_2aB         "2aB"
#define IME_123_Dial    "123_Dial"
#define IME_123_IP      "123_IP"
// 密码界面输入法对应的字符串
#define IME_2aB_Pwd     "2aB_PWD"
#define IME_abc_Pwd     "abc_PWD"
#define IME_ABC_Pwd     "ABC_PWD"
#define IME_123_Pwd     "123_PWD"
#define IME_NONE        ((const char*)0)



#ifdef LANG_TIME_TRACE

#define IF_MODE_LOG_TRACE(fmt,...)  APP_INFO("fun=[%s] " fmt, __FUNCTION__, ##__VA_ARGS__)

struct TimeLapse
{
    TimeLapse(const yl::string & strTips) : strDsp(strTips)
    {
        gettimeofday(&dwStart, NULL);
    }

    void ReStart(LPCSTR chStr)
    {
        if (chStr)
        {
            gettimeofday(&dwEnd, NULL);
            unsigned long dwTime = 1000 * (dwEnd.tv_sec - dwStart.tv_sec) + (dwEnd.tv_usec - dwStart.tv_usec) /
                                   1000;
            gettimeofday(&dwStart, NULL);
            printf("%s Time Take [%ld]ms \n", chStr, dwTime);
        }
    }

    ~TimeLapse()
    {
        gettimeofday(&dwEnd, NULL);
        unsigned long dwTime = 1000 * (dwEnd.tv_sec - dwStart.tv_sec) + (dwEnd.tv_usec - dwStart.tv_usec) /
                               1000;
        printf("%s Time Take [%ld]ms \n", strDsp.c_str(), dwTime);
    }

private:
    struct timeval dwStart;
    struct timeval dwEnd;
    yl::string strDsp;
};


#define IF_TimeTrace \
    TimeLapse ttrace(__FUNCTION__);
#endif

#ifndef IF_TimeTrace
#define IF_TimeTrace void(0)
#endif
#ifndef IF_MODE_LOG_TRACE
#define IF_MODE_LOG_TRACE(fmt,...) void(0)
#endif


/*****************************************************华丽的分割线********************************************************/



