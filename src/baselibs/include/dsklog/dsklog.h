/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 20010-2020 Xiamen  Network Technology Co,.Ltd
*
*    AUTHOR:songk
*
*    HISTORY:
*
*    DATE:2018年1月9日
*
*******************************************************************/
#ifndef __DSK_LOG_H__
#define __DSK_LOG_H__
#include "log.h"

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * @brief log 初始化
 *
 * @author Song
 * @date 2018/1/9
 *
 * @param 从命令行解析参数，并转化成具体的数据结构
 *        为了保持兼容性，不对外暴露数据结构
 * @param argc The argc.
 * @param argv The argv.
 * @param log_name log中的标识符，默认为"GUI".
 *
 * @return True if it succeeds, false if it fails.
 */

bool dsklog_init(int argc, char * argv[], const char * log_name);

/**
 * @brief Dsklog simple init.
 *
 * @author Song
 * @date 2018/1/27
 *
 * @param mode     The mode.
 *      DBG_LOG_MODE_NONE     0x0000
 *      DBG_LOG_MODE_TIME     0x0001
 * @param type     The type.
 *      DBG_LOG_TYPE_NONE     0
 *      DBG_LOG_TYPE_SYS      1        output to syslogd
 *      DBG_LOG_TYPE_STD      2        output to stdout
 *      DBG_LOG_TYPE_FILE     3        output to file
 * @param level    The level.
 * @param file     The file path. only  type is DBG_LOG_TYPE_FILE
 * @param log_name Name of the log.
 *
 * @return True if it succeeds, false if it fails.
 */

bool dsklog_simple_init(int mode, int type, int level, const char * file, const char * log_name);
/**
 *
 * @brief 是否手动初始化，用于前台运行(-l 标识手动运行)
 *
 * @author Song
 * @date 2018/1/9
 *
 * @param argc The argc.
 * @param argv The argv.
 *
 * @return True if it succeeds, false if it fails.
 */

bool dsklog_is_manual_init(int argc, char * argv[]);
/**
 * @brief 重新设置日志等级
 *
 * @author Song
 * @date 2018/1/9
 *
 * @param level:新设置的等级
 *
 */

void dsklog_setlog_level(int level);

/**
 * @brief Dsklog setlog levelex.
 *
 * @author Song
 * @date 2018/1/9
 *
 * @param module   The module.
 * @param level    The level.
 * @param log_type Type of the log.
 * @param file     The file.
 */

void dsklog_setlog_levelex(int module, int level, int log_type, const char * file);

void dsklog_helper();

/**
 * @brief Determines if we can dsklog uninit.
 *
 * @author Song
 * @date 2018/1/9
 *
 * @return True if it succeeds, false if it fails.
 */

bool dsklog_uninit();

#ifdef  __cplusplus
}
#endif

#endif //__DSK_LOG_H__
