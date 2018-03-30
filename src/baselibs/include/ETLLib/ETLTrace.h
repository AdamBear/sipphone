#ifndef __ETLTRACE_H__
#define __ETLTRACE_H__
#include "dsklog/dsklog.h"
typedef void(*etllib_log_callback)(int module, int level, const char * fmt, ...);
static void etlTrace(int module, int level, const char * fmt, ...);

static etllib_log_callback  g_log_callback = NULL;
static int g_log_mode = 0;

#ifdef IF_DEBUG_LOG
#define  IF_ETL_DEBUG              1
#else
#define  IF_ETL_DEBUG              0
#endif

#if IF_ETL_DEBUG
#define DEF_ETL_LOG(mode,level,callback,fmt,...) \
   do \
   { \
        if (NULL != callback) \
        { \
            callback(mode,level,fmt,##__VA_ARGS__); \
        } \
        else \
        { \
            etlTrace(mode,level,fmt,##__VA_ARGS__); \
        } \
   } while (0)
#else
#define DEF_ETL_LOG(mode,level,callback,fmt,...) etlTrace(mode,level,fmt,##__VA_ARGS__);
#endif


#define etlTraceInfo(fmt,...)    DEF_ETL_LOG(g_log_mode,DBG_LOG_LEVEL_INFO,g_log_callback,fmt"\n", ##__VA_ARGS__)
#define etlTraceWarn(fmt,...)    DEF_ETL_LOG(g_log_mode,DBG_LOG_LEVEL_WARN,g_log_callback,fmt"\n", ##__VA_ARGS__)
#define etlTraceNotice(fmt,...)  DEF_ETL_LOG(g_log_mode,DBG_LOG_LEVEL_NOTICE,g_log_callback,fmt"\n", ##__VA_ARGS__)
#define etlTraceErr(fmt,...)     DEF_ETL_LOG(g_log_mode,DBG_LOG_LEVEL_ERR,g_log_callback,fmt"\n", ##__VA_ARGS__)
#define etlTraceDebug(fmt,...)   DEF_ETL_LOG(g_log_mode,DBG_LOG_LEVEL_DEBUG,g_log_callback,fmt"\n", ##__VA_ARGS__)

#endif //__ETLTRACE_H__
