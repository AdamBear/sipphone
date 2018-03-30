#pragma once

#include "config.h"

// 语言相关模块的测试模式，可打印更多日誌信息，默认关
#ifdef IF_TEST_MODE
#undef IF_TEST_MODE
#endif

//#define IF_TEST_MODE

// 阿拉伯语优化空间占用，去掉Unicode表，默认开
#define ARABIC_SIZE_OPT

// unicode字符属性提取，默认关
//#define ARABIC_DEBUG

// 是否开启时间检测，可观察部分函数的消耗时间，默认关
//#define LANG_TIME_TRACE


#ifdef ARABIC_SIZE_OPT
#undef ARABIC_DEBUG
#endif

#ifdef IF_TEST_MODE
#ifndef LANG_TIME_TRACE
#define LANG_TIME_TRACE
#endif
#endif

// 翻译文件是否支持UTF8、UTF16混合编码，默认开
#define IF_SUPPORT_UTF8_TRANS

// 是否支持语系下语言的细分，默认关
//#define IF_SUPOURT_SPITE_LANGUAGE_FAMILY

// 是否支持XML格式的翻译文件, V80可以不支持
#define IF_SUPPORT_XML_TRANS



