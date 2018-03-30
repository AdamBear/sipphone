/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2011-07-12
*    warning:
*       sample implementaion for syslog,SHOULD modify by user!
*
*    history:
*------------------------------------------------------------------
*    修改时间: 2015.10.10   修改者 :  zzq
*    原版本号: 2.0.0.0      新版本号: 2.0.0.1
*    修改问题:
*    1. 增加Android平台支持，Andriod平台/dev/log被占用，
*       接收日志设备改为/dev/log/log
*------------------------------------------------------------------
*------------------------------------------------------------------
*    修改时间: 2014.12.15   修改者 :  hyy
*    原版本号: 1.0.0.5      新版本号: 2.0.0.0
*    修改问题:
*    1. 与V80的log版本合并，将模块名和PID改为强制输出，并对输出
*       到syslogd的增加PRI和时间戳(RFC5424)
*    2. log输出失败强制修复(重新open)
*    3. 按照编码规范整理
*------------------------------------------------------------------
*------------------------------------------------------------------
*    修改时间:14/03/25    修改者 :  zzq
*    原版本号: 1.0.0.4   新版本号: 1.0.0.5
*    修改问题:
*    1.将syslog移植到log.c后，自定义的log格式不符合RFC5424标准，缺少PRI
*      和TIMESTAMP，导致在syslog服务器上显示异常
*    规范打印到syslogd的log，输出到cons的log不改变
*------------------------------------------------------------------
*------------------------------------------------------------------
*    修改时间:13/5/28    修改者 :  Job.pan
*    原版本号: 1.0.0.3   新版本号: 1.0.0.4
*    修改问题:
*    1.在写入到syslogd前，如果socket<0,那么再次尝试打开socket
*      修复syslogd启动较晚时,无法打印到syslogd的问题.
*    2.修复当关闭标准输入时,socket返回0时的处理.
*------------------------------------------------------------------
*------------------------------------------------------------------
*    修改时间:13/4/9    修改者 :  Job.pan
*    原版本号: 1.0.0.2   新版本号: 1.0.0.3
*    修改问题:
*    1.增加基本log函数的宏定义
*    2.修正1.0.0.2版本中syslog导致stdin异常bug
*    3.增加windows编译支持
*------------------------------------------------------------------
*------------------------------------------------------------------
*    修改时间:13/3/31    修改者 :  Job.pan
*    原版本号: 1.0.0.1   新版本号: 1.0.0.2
*    修改问题:
*    增加vlog接口,将sylog函数移植到log.c中
*------------------------------------------------------------------
*------------------------------------------------------------------
*    修改时间:13/2/19    修改者 :  pjb
*    原版本号: 1.0.0.0   新版本号: 1.0.0.1
*    修改问题:
*    增加DBG_LOG_POSITION宏，用于单独控制file和line是否打印，防止和
*    DBG_LOG_LOGANY共用.
*------------------------------------------------------------------
*------------------------------------------------------------------
*    修改时间:13/1/9    修改者 :  pjb
*    原版本号: 0.0.0.0   新版本号: 1.0.0.0
*    修改问题:
*    调整一些参数的位置,并引入DBG_DEBUG宏
*------------------------------------------------------------------
*******************************************************************/

#ifndef __DEBUG_LOG_H__
#define __DEBUG_LOG_H__
/*
* for c
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

/*
* ----------------------------------------------------------------------------------------
* ------------USER option-----------------------------------------------------------------
* ----------------------------------------------------------------------------------------
* for config
*
* \# the config setting
* ----------------------------------------------------------------------------------------
*/
/*
* log config
* 0==disable
* 1==enable
*/

/*
* log localize by name,
* must be set in lowercase and captital!!
*/
#define DBG_LOG_LOCALIZE_a(fn)  log ##fn
#define DBG_LOG_LOCALIZE_A(fn)  LOG ##fn

/* log debug setting */
#define  DBG_LOG_LOGANY        0        /* enable any info referenced by log */
/* log style setting */
#define  DBG_LOG_LOGTIME       1        /* enable log precise time */
#define  DBG_LOG_LOGMAX        256      /* log max length */

/*
* ----------------------------------------------------------------------------------------
* ------------FIX option------------------------------------------------------------------
* ----------------------------------------------------------------------------------------
* for define
*
* \# the basic setting
* ----------------------------------------------------------------------------------------
*/
#if defined DBG_DEBUG
/* log debug setting */
#define  DBG_LOG_LOGCHECK      1        /* enable safe check,set off when release */
#define  DBG_LOG_DEBUG         1        /* enable debug info,else will log nothing for lower than debug */
/* log style setting */
#define  DBG_LOG_LOGCOLOR      1        /* enable console color */
#define  DBG_LOG_POSITION      1        /* enable log position */
#else
/* log debug setting */
#define  DBG_LOG_LOGCHECK      0        /* enable safe check,set off when release */
#define  DBG_LOG_DEBUG         0        /* enable debug info,else will log nothing for lower than debug */
/* log style setting */
#define  DBG_LOG_LOGCOLOR      0        /* enable console color */
#define  DBG_LOG_POSITION      0        /* enable log position */
#endif
/*
* log level
*/
#define  DBG_LOG_INV          -1
#define  DBG_LOG_ALL           0
#define  DBG_LOG_LEVEL_EMERG   0        /* Emergency: system is unusable */
#define  DBG_LOG_LEVEL_ALERT   1        /* Alert: action must be taken immediately */
#define  DBG_LOG_LEVEL_CRIT    2        /* Critical: critical conditions */
#define  DBG_LOG_LEVEL_ERR     3        /* Error: error conditions */
#define  DBG_LOG_LEVEL_WARN    4        /* Warning: warning conditions */
#define  DBG_LOG_LEVEL_NOTICE  5        /* Notice: normal but significant condition */
#define  DBG_LOG_LEVEL_INFO    6        /* Informational: informational messages */
#define  DBG_LOG_LEVEL_DEBUG   7        /* Debug: debug-level messages */
#define  DBG_LOG_LEVEL_SPY     8        /* reserved */
#define  DBG_LOG_LEVEL_ANY     9        /* reserved */

/*
* log output type
*/
#define  DBG_LOG_TYPE_NONE     0
#define  DBG_LOG_TYPE_SYS      1        /* output to syslogd */
#define  DBG_LOG_TYPE_STD      2        /* output to stdout */
#define  DBG_LOG_TYPE_FILE     3        /* output to file */
#define  DBG_LOG_TYPE_MAX      4

/*
* log format mode
*/
#define  DBG_LOG_MODE_NONE     0x0000
#define  DBG_LOG_MODE_TIME     0x0001   /* log have precise time */

/*
*   log API
*/
#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief  : use for user to init basic data of log
*
* @param  : ident, log name(must align to four char)
*           type, log output type(DBG_LOG_TYPE_SYS or DBG_LOG_TYPE_STD or DBG_LOG_TYPE_FILE)
*           mode, log format mode
*           file, include path(log will output to this file when log output type is DBG_LOG_TYPE_FILE)
*
* @warning: Output only one type(different from the previous version)
*
* @return : 0 as success,otherwise failed.
*
*/
int DBG_LOG_LOCALIZE_a(_log_init)(const char * ident, int type, int mode, const char * file);

/**
* @brief  : use for user reset log data
*
* @param  : none
*
* @return : 0 as success,otherwise failed.
*
*/
int DBG_LOG_LOCALIZE_a(_log_exit)(void);

/**
* @brief  : use for user open log
*
* @param  : type, log output type(DBG_LOG_TYPE_SYS or DBG_LOG_TYPE_STD or DBG_LOG_TYPE_FILE)
*           file, include path(log will output to this file when log output type is DBG_LOG_TYPE_FILE)
*
* @warning: reopen if type changed, so you can change log output type when program running
*
* @return : 0 as success,otherwise failed.
*
*/
int DBG_LOG_LOCALIZE_a(_log_open)(int type, const char * file);

/**
* @brief  : use for user get module log level
*
* @param  : module, log module define ID
*
* @return : log level define ID, DBG_LOG_INV failed, otherwise success.
*
*/
int DBG_LOG_LOCALIZE_a(_log_get)(int module);

/**
* @brief  : use for user get module name
*
* @param  : module, log module define ID
*
* @return : log module name as success,NULL failed.
*
*/
const char * DBG_LOG_LOCALIZE_a(_log_name)(int module);

/**
* @brief  : use for user set module log level
*
* @param  : module, log module define ID
*           level,  log level define ID
*
* @return : log level define ID, DBG_LOG_INV failed, otherwise success.
*
*/
int DBG_LOG_LOCALIZE_a(_log_set)(int module, int level);

/**
* @brief  : use for user set module log level by module name
*
* @param  : name,  log module name
*           level, log level define ID
*
* @return : log level define ID, DBG_LOG_INV failed, otherwise success.
*
*/
int DBG_LOG_LOCALIZE_a(_log_set2)(const char * name, int level);

/**
* @brief  : use for user print log
*
* @param  : module, log module name
*           level,  log level define ID
*           fmt,    print format
*           vargs,  variable list
*
* @return : none
*
*/
void DBG_LOG_LOCALIZE_a(_log)(int module, int level, const char * fmt, ...);
void DBG_LOG_LOCALIZE_a(_vlog)(int module, int level, const char * fmt, va_list vargs);

#ifdef __cplusplus
}
#endif

/*
*   log func
*/
#define  LOG_API_INIT                     DBG_LOG_LOCALIZE_a(_log_init)
#define  LOG_API_EXIT                     DBG_LOG_LOCALIZE_a(_log_exit)
#define  LOG_API_OPEN                     DBG_LOG_LOCALIZE_a(_log_open)
#define  LOG_API_GET                      DBG_LOG_LOCALIZE_a(_log_get)
#define  LOG_API_NAME                     DBG_LOG_LOCALIZE_a(_log_name)
#define  LOG_API_SET                      DBG_LOG_LOCALIZE_a(_log_set)
#define  LOG_API_SET2                     DBG_LOG_LOCALIZE_a(_log_set2)
#define  LOG_API_LOG                      DBG_LOG_LOCALIZE_a(_log)
#define  LOG_API_VLOG                     DBG_LOG_LOCALIZE_a(_vlog)

/*
*   log micro
*/
#define  LOG_INIT(name, type, mode, file) LOG_API_INIT(name, type, mode, file)
#define  LOG_EXIT()                       LOG_API_EXIT()
#define  LOG_OPEN(type, file)             LOG_API_OPEN(type, file)
#define  LOG_GET(m)                       LOG_API_GET(m)
#define  LOG_NAME(m)                      LOG_API_NAME(m)
#define  LOG_SET(m, l)                    LOG_API_SET(m, l)
#define  LOG_SET2(m, l)                   LOG_API_SET2(m, l)
#define  LOG_LOG(m, l, fmt, ...)          LOG_API_LOG(m, l, fmt, ##__VA_ARGS__)
#define  LOG_VLOG(m, l, fmt, vargs)       LOG_API_VLOG(m, l, fmt, vargs)

#ifdef WIN32
#define  ANY_LOG(name, level, fmt, ...) \
    do \
    {  \
        DBG_LOG_LOCALIZE_a(_log)(name, level, fmt, ##__VA_ARGS__); \
}while (0)
#else
#define  ANY_LOG(name, level, fmt, ...) \
    do \
    {  \
        DBG_LOG_LOCALIZE_a(_log)(name, level, "[%-16s:%-4d]" fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
    }while (0)
#endif

#define  SMP_LOG(name, level, fmt, ...) \
    do \
    {  \
        DBG_LOG_LOCALIZE_a(_log)(name, level, fmt, ##__VA_ARGS__); \
    }while (0)


#ifdef IF_DEBUG_LOG
#define MAX_LOG_LEVEL  DBG_LOG_LEVEL_SPY
#else
#define  MAX_LOG_LEVEL  DBG_LOG_LEVEL_DEBUG
#endif


#define  DEF_LOG(_name, _level, _fmt, ...) \
    do \
    {  \
        if (DBG_LOG_DEBUG || _level < MAX_LOG_LEVEL) \
        { \
            if (DBG_LOG_POSITION || DBG_LOG_ANY_ ##_name) \
                ANY_LOG(DBG_LOG_ ##_name, _level, _fmt, ##__VA_ARGS__); \
            else \
                SMP_LOG(DBG_LOG_ ##_name, _level, _fmt, ##__VA_ARGS__); \
        } \
    }while (0)
/*
* ----------------------------------------------------------------------------------------
* ------------USER option-----------------------------------------------------------------
* ----------------------------------------------------------------------------------------
* for config
*
* \# the module setting
* ----------------------------------------------------------------------------------------
*/
/*
* log name
* must align to 4 char,and line as ID respectively!
*/
#define  DBG_LOG_MOD  \
    "ANY ", \
    "MEM ", \
    "APP ", \
    "TALK", \
    "TKUI", \
    "SCRN", \
    "DIR ", \
    "CLOG", \
    "SET ", \
    "STUI", \
    "SPSH", \
    "ACCU", \
    "URIL", \
    "ATRD", \
    "ANYW", \
    "SCA ", \
    "CCTL", \
    "CCET", \
    "CC  ", \
    "CPRK", \
    "CMUI", \
    "CAPI", \
    "CUIT", \
    "CPSR", \
    "DEVL", \
    "DIRU", \
    "DOTW", \
    "DSSK", \
    "EMSG", \
    "FWDD", \
    "HOTD", \
    "IDLE", \
    "IDUI", \
    "LOCK", \
    "LDAP", \
    "POPB", \
    "TEST", \
    "UIMG", \
    "VOIC", \
    "XMLB", \
    "XBUI", \
    "XSI ", \
    "ZERO", \
    "ZOUI", \
    "SYNC", \
    "GBIH", \
    "INME", \
    "MWND", \
    "MSUI", \
    "UIKE", \
    "BSUI", \
    "APP ", \
    "BKLT", \
    "SCA ", \
    "CLUI", \
    "DKUI", \
    "LEDL", \
    "MENU", \
    "TASK", \
    "CGI ", \
    "CONT", \
    "IBOX", \
    "HELP", \
    "CTPS", \
    "WEB ", \
    "ETLL", \
    "CSRV", \
    "WIRE", \
    "EDK ", \
    "VLOG", \
    "VKE ", \
    "LYNC", \
    "LINE", \
    "USBD", \
    "BLTH", \
    "HAND", \
    "CMDP", \
    "SS7 ", \
    "FWUP", \
    "XMLP", \
    "XWIN", \
    "WIND"


#define  DBG_LOG_MIN               0
/*log model*/
#define  DBG_LOG_ANY               0 /*fix setting*/
#define  DBG_LOG_MEM               1 /*for memory*/
#define  DBG_LOG_APP               2 /*for app*/
#define  DBG_LOG_TALK              3 /*for talklogic*/
#define  DBG_LOG_TALKUI            4 /*for talkui*/
#define  DBG_LOG_SCREEN            5 /*for screen*/
#define  DBG_LOG_DIR               6 /*for directory*/
#define  DBG_LOG_CALLLOG           7 /*for calllog*/
#define  DBG_LOG_SETTING           8 /*for setting*/
#define  DBG_LOG_SETTINGUI         9 /*for settingui*/
#define  DBG_LOG_SUPERSEARCH       10 /*for super search*/
#define  DBG_LOG_ACCOUNT           11 /*for account*/
#define  DBG_LOG_ACTION            12 /*for action uri url*/
#define  DBG_LOG_AUTORD            13 /*for auto redial*/
#define  DBG_LOG_ANYWHERE          14 /*for any where*/
#define  DBG_LOG_SCA               15 /*for sca*/
#define  DBG_LOG_CALLCONTROL       16 /*for call control*/
#define  DBG_LOG_CALLCENTER        17 /*for call center*/
#define  DBG_LOG_CC                18 /*for call completion*/
#define  DBG_LOG_CALLPARK          19 /*for callpark*/
#define  DBG_LOG_COMMONUI          20 /*for commonui*/
#define  DBG_LOG_COMMONAPI         21 /*for commonapi*/
#define  DBG_LOG_COMMONUNITS       22 /*for commonunits*/
#define  DBG_LOG_CONFIGPARSER      23 /*for configparser*/
#define  DBG_LOG_DEVICELIB         24 /*for devicelib*/
#define  DBG_LOG_DIRUI             25 /*for directory ui*/
#define  DBG_LOG_DOTWINDOWS        26 /*for dotwindows*/
#define  DBG_LOG_DSSKEY            27 /*for dsskey*/
#define  DBG_LOG_ETLMSGHANDLER     28 /*for etlmsghandler*/
#define  DBG_LOG_FWDDND            29 /*for fwd dnd*/
#define  DBG_LOG_HOTDESKING        30 /*for hotdesking*/
#define  DBG_LOG_IDLE              31 /*for idle*/
#define  DBG_LOG_IDLEUI            32 /*for idle ui*/
#define  DBG_LOG_LOCK              33 /*for lock*/
#define  DBG_LOG_LDAP              34 /*for ldap*/
#define  DBG_LOG_POPUPBOX          35 /*for popupbox*/
#define  DBG_LOG_TESTMODE          36 /*for testmode*/
#define  DBG_LOG_UIMANAGER         37 /*for uimanager*/
#define  DBG_LOG_VOICE             38 /*for voice*/
#define  DBG_LOG_XMLBROWSER        39 /*for xmlbrowser*/
#define  DBG_LOG_XMLBROWSERUI      40 /*for xmlbrowser ui*/
#define  DBG_LOG_XSI               41 /*for xsi*/
#define  DBG_LOG_ZERO              42 /*for zero*/
#define  DBG_LOG_ZEROUI                     43 /*for zeroUI*/
#define  DBG_LOG_SYNC                       44 /*for sync*/
#define  DBG_LOG_GLOBALINPUTHOOK            45 /*for globalInputHook*/
#define  DBG_LOG_INPUTMETHOD                46 /*for inputmethod*/
#define  DBG_LOG_MAINWND                    47 /*for mainwnd*/
#define  DBG_LOG_MESSAGEUI                  48 /*for messageui*/
#define  DBG_LOG_UIKERNEL                   49 /*for uikernel*/
#define  DBG_LOG_BASEUI                     50 /*for baseui*/
#define  DBG_LOG_APPLICATION                51 /*for application*/
#define  DBG_LOG_BACKLIGHT                  52 /*for backlight*/
#define  DBG_LOG_BROADSOFT_SCA              53 /*for broadsoft_sca*/
#define  DBG_LOG_CALLLOGUI                  54 /*for calllogui*/
#define  DBG_LOG_DSSKEYUI                   55 /*for dsskeyui*/
#define  DBG_LOG_LEDLAMP                    56 /*for ledlamp*/
#define  DBG_LOG_MENU                       57 /*for menu*/
#define  DBG_LOG_TASKACTION                 58 /*for taskaction*/
#define  DBG_LOG_CGISERVER                  59 /*for cgiserver*/
#define  DBG_LOG_CONFIGTRANSLATE            60 /*for configtranslate*/
#define  DBG_LOG_IDLEBOX                    61 /*for idlebox*/
#define  DBG_LOG_HELP                       62 /*for help*/
#define  DBG_LOG_CONTACTPARSER              63 /*for contactparser*/
#define  DBG_LOG_WEB                        64 /*for web*/
#define  DBG_LOG_ETLLIB            65 /*for ETLLib*/
#define  DBG_LOG_CSRV              66 /*for configServer*/
#define  DBG_LOG_WIRE              67 /*for wifi and bluetooth*/
#define  DBG_LOG_EDK               68 /*for EDK*/
#define  DBG_LOG_VLOG              69 /*for VLOG*/
#define  DBG_LOG_VKE               70 /*for VKE*/
#define  DBG_LOG_LYNC              71 /*for LYNC*/
#define  DBG_LOG_LINE              72 /*for line device*/
#define  DBG_LOG_USBD              73 /*for usb record*/
#define  DBG_LOG_BLTH              74 /*for bluetooth*/
#define  DBG_LOG_HAND              75 /*for HandSet Infor*/
#define  DBG_LOG_CMDP              76 /*for CMD Process*/
#define  DBG_LOG_SS7               77 /*for SS7*/
#define  DBG_LOG_FWUP              78 /*for Hsupdate*/
#define  DBG_LOG_XMLP              79 /*for xml parser*/
#define  DBG_LOG_XWINDOW           80 /*for x window*/
#define  DBG_LOG_WIN               81 /*for win32 object*/
#define  DBG_LOG_MAX               82

/*log config*/
#define  DBG_LOG_ANY_APP           0
#define  DBG_LOG_ANY_TALK          0
#define  DBG_LOG_ANY_TALKUI        0
#define  DBG_LOG_ANY_SCREEN        0
#define  DBG_LOG_ANY_DIR           0
#define  DBG_LOG_ANY_CALLLOG       0
#define  DBG_LOG_ANY_SETTING       0
#define  DBG_LOG_ANY_SETTINGUI     0
#define  DBG_LOG_ANY_SUPERSEARCH   0
#define  DBG_LOG_ANY_ACCOUNT       0
#define  DBG_LOG_ANY_ACTION        0
#define  DBG_LOG_ANY_AUTORD        0
#define  DBG_LOG_ANY_ANYWHERE      0
#define  DBG_LOG_ANY_SCA           0
#define  DBG_LOG_ANY_CALLCONTROL   0
#define  DBG_LOG_ANY_CALLCENTER    0
#define  DBG_LOG_ANY_CC            0
#define  DBG_LOG_ANY_CALLPARK      0
#define  DBG_LOG_ANY_COMMONUI      0
#define  DBG_LOG_ANY_COMMONAPI     0
#define  DBG_LOG_ANY_COMMONUNITS   0
#define  DBG_LOG_ANY_CONFIGPARSER  0
#define  DBG_LOG_ANY_DEVICELIB     0
#define  DBG_LOG_ANY_DIRUI         0
#define  DBG_LOG_ANY_DOTWINDOWS    0
#define  DBG_LOG_ANY_DSSKEY        0
#define  DBG_LOG_ANY_ETLMSGHANDLER 0
#define  DBG_LOG_ANY_FWDDND        0
#define  DBG_LOG_ANY_HOTDESKING    0
#define  DBG_LOG_ANY_IDLE          0
#define  DBG_LOG_ANY_IDLEUI        0
#define  DBG_LOG_ANY_LOCK          0
#define  DBG_LOG_ANY_LDAP          0
#define  DBG_LOG_ANY_POPUPBOX      0
#define  DBG_LOG_ANY_TESTMODE      0
#define  DBG_LOG_ANY_UIMANAGER     0
#define  DBG_LOG_ANY_VOICE         0
#define  DBG_LOG_ANY_XMLBROWSER    0
#define  DBG_LOG_ANY_XMLBROWSERUI  0
#define  DBG_LOG_ANY_XSI           0
#define  DBG_LOG_ANY_ZERO          0
#define  DBG_LOG_ANY_ZEROUI             0
#define  DBG_LOG_ANY_SYNC               0
#define  DBG_LOG_ANY_GLOBALINPUTHOOK    0
#define  DBG_LOG_ANY_INPUTMETHOD        0
#define  DBG_LOG_ANY_MAINWND            0
#define  DBG_LOG_ANY_MESSAGEUI          0
#define  DBG_LOG_ANY_UIKERNEL           0
#define  DBG_LOG_ANY_BASEUI             0
#define  DBG_LOG_ANY_APPLICATION        0
#define  DBG_LOG_ANY_BACKLIGHT          0
#define  DBG_LOG_ANY_BROADSOFT_SCA      0
#define  DBG_LOG_ANY_CALLLOGUI          0
#define  DBG_LOG_ANY_DSSKEYUI           0
#define  DBG_LOG_ANY_LEDLAMP            0
#define  DBG_LOG_ANY_MENU               0
#define  DBG_LOG_ANY_TASKACTION         0
#define  DBG_LOG_ANY_CGISERVER          0
#define  DBG_LOG_ANY_CONFIGTRANSLATE    0
#define  DBG_LOG_ANY_IDLEBOX            0
#define  DBG_LOG_ANY_HELP               0
#define  DBG_LOG_ANY_CONTACTPARSER      0
#define  DBG_LOG_ANY_WEB                0
#define  DBG_LOG_ANY_ETLLIB        0
#define  DBG_LOG_ANY_CSRV          0
#define  DBG_LOG_ANY_WIRE          0
#define  DBG_LOG_ANY_EDK           0
#define  DBG_LOG_ANY_VLOG          0
#define  DBG_LOG_ANY_VKE           0
#define  DBG_LOG_ANY_LYNC          0
#define  DBG_LOG_ANY_LINE          0
#define  DBG_LOG_ANY_USBD          0
#define  DBG_LOG_ANY_BLTH          0
#define  DBG_LOG_ANY_HAND          0
#define  DBG_LOG_ANY_CMDP          0
#define  DBG_LOG_ANY_SS7           0
#define  DBG_LOG_ANY_FWUP          0
#define  DBG_LOG_ANY_XMLP          0
#define  DBG_LOG_ANY_XWINDOW       0
#define  DBG_LOG_ANY_WIN           0

/*log xwindow*/
#define  XWINDOW_ERR(fmt,...)    DEF_LOG(XWINDOW,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  XWINDOW_WARN(fmt,...)   DEF_LOG(XWINDOW,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  XWINDOW_NOTICE(fmt,...) DEF_LOG(XWINDOW,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  XWINDOW_INFO(fmt,...)   DEF_LOG(XWINDOW,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  XWINDOW_DEBUG(fmt,...)  DEF_LOG(XWINDOW,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)


/*log memory*/
#define  MEM_ERR(fmt,...)    DEF_LOG(MEM,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  MEM_WARN(fmt,...)   DEF_LOG(MEM,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  MEM_NOTICE(fmt,...) DEF_LOG(MEM,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  MEM_INFO(fmt,...)   DEF_LOG(MEM,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  MEM_DEBUG(fmt,...)  DEF_LOG(MEM,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log app*/
#define  APP_ERR(fmt,...)    DEF_LOG(APP,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  APP_WARN(fmt,...)   DEF_LOG(APP,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  APP_NOTICE(fmt,...) DEF_LOG(APP,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  APP_INFO(fmt,...)   DEF_LOG(APP,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  APP_DEBUG(fmt,...)  DEF_LOG(APP,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log TALK*/
#define  TALK_ERR(fmt,...)    DEF_LOG(TALK,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  TALK_WARN(fmt,...)   DEF_LOG(TALK,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  TALK_NOTICE(fmt,...) DEF_LOG(TALK,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  TALK_INFO(fmt,...)   DEF_LOG(TALK,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  TALK_DEBUG(fmt,...)  DEF_LOG(TALK,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log TalkUI*/
#define  TALKUI_ERR(fmt,...)    DEF_LOG(TALKUI,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  TALKUI_WARN(fmt,...)   DEF_LOG(TALKUI,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  TALKUI_NOTICE(fmt,...) DEF_LOG(TALKUI,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  TALKUI_INFO(fmt,...)   DEF_LOG(TALKUI,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  TALKUI_DEBUG(fmt,...)  DEF_LOG(TALKUI,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log Screen*/
#define  SCREEN_ERR(fmt,...)    DEF_LOG(SCREEN,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  SCREEN_WARN(fmt,...)   DEF_LOG(SCREEN,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  SCREEN_NOTICE(fmt,...) DEF_LOG(SCREEN,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  SCREEN_INFO(fmt,...)   DEF_LOG(SCREEN,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  SCREEN_DEBUG(fmt,...)  DEF_LOG(SCREEN,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log Directory*/
#define  DIR_ERR(fmt,...)    DEF_LOG(DIR,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  DIR_WARN(fmt,...)   DEF_LOG(DIR,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  DIR_NOTICE(fmt,...) DEF_LOG(DIR,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  DIR_INFO(fmt,...)   DEF_LOG(DIR,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  DIR_DEBUG(fmt,...)  DEF_LOG(DIR,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log calllog*/
#define  CALLLOG_ERR(fmt,...)    DEF_LOG(CALLLOG,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  CALLLOG_WARN(fmt,...)   DEF_LOG(CALLLOG,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  CALLLOG_NOTICE(fmt,...) DEF_LOG(CALLLOG,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  CALLLOG_INFO(fmt,...)   DEF_LOG(CALLLOG,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  CALLLOG_DEBUG(fmt,...)  DEF_LOG(CALLLOG,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log setting*/
#define  SETTING_ERR(fmt,...)    DEF_LOG(SETTING,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  SETTING_WARN(fmt,...)   DEF_LOG(SETTING,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  SETTING_NOTICE(fmt,...) DEF_LOG(SETTING,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  SETTING_INFO(fmt,...)   DEF_LOG(SETTING,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  SETTING_DEBUG(fmt,...)  DEF_LOG(SETTING,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log setting ui*/
#define  SETTINGUI_ERR(fmt,...)    DEF_LOG(SETTINGUI,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  SETTINGUI_WARN(fmt,...)   DEF_LOG(SETTINGUI,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  SETTINGUI_NOTICE(fmt,...) DEF_LOG(SETTINGUI,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  SETTINGUI_INFO(fmt,...)   DEF_LOG(SETTINGUI,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  SETTINGUI_DEBUG(fmt,...)  DEF_LOG(SETTINGUI,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log super search*/
#define  SUPERSEARCH_ERR(fmt,...)    DEF_LOG(SUPERSEARCH,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  SUPERSEARCH_WARN(fmt,...)   DEF_LOG(SUPERSEARCH,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  SUPERSEARCH_NOTICE(fmt,...) DEF_LOG(SUPERSEARCH,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  SUPERSEARCH_INFO(fmt,...)   DEF_LOG(SUPERSEARCH,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  SUPERSEARCH_DEBUG(fmt,...)  DEF_LOG(SUPERSEARCH,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log account*/
#define  ACCOUNT_ERR(fmt,...)    DEF_LOG(ACCOUNT,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  ACCOUNT_WARN(fmt,...)   DEF_LOG(ACCOUNT,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  ACCOUNT_NOTICE(fmt,...) DEF_LOG(ACCOUNT,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  ACCOUNT_INFO(fmt,...)   DEF_LOG(ACCOUNT,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  ACCOUNT_DEBUG(fmt,...)  DEF_LOG(ACCOUNT,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log action uri url*/
#define  ACTION_ERR(fmt,...)    DEF_LOG(ACTION,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  ACTION_WARN(fmt,...)   DEF_LOG(ACTION,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  ACTION_NOTICE(fmt,...) DEF_LOG(ACTION,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  ACTION_INFO(fmt,...)   DEF_LOG(ACTION,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  ACTION_DEBUG(fmt,...)  DEF_LOG(ACTION,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log autoredial*/
#define  AUTORD_ERR(fmt,...)    DEF_LOG(AUTORD,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  AUTORD_WARN(fmt,...)   DEF_LOG(AUTORD,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  AUTORD_NOTICE(fmt,...) DEF_LOG(AUTORD,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  AUTORD_INFO(fmt,...)   DEF_LOG(AUTORD,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  AUTORD_DEBUG(fmt,...)  DEF_LOG(AUTORD,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log anywhere*/
#define  ANYWHERE_ERR(fmt,...)    DEF_LOG(ANYWHERE,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  ANYWHERE_WARN(fmt,...)   DEF_LOG(ANYWHERE,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  ANYWHERE_NOTICE(fmt,...) DEF_LOG(ANYWHERE,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  ANYWHERE_INFO(fmt,...)   DEF_LOG(ANYWHERE,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  ANYWHERE_DEBUG(fmt,...)  DEF_LOG(ANYWHERE,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log sca*/
#define  SCA_ERR(fmt,...)    DEF_LOG(SCA,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  SCA_WARN(fmt,...)   DEF_LOG(SCA,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  SCA_NOTICE(fmt,...) DEF_LOG(SCA,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  SCA_INFO(fmt,...)   DEF_LOG(SCA,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  SCA_DEBUG(fmt,...)  DEF_LOG(SCA,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log call control*/
#define  CALLCONTROL_ERR(fmt,...)    DEF_LOG(CALLCONTROL,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  CALLCONTROL_WARN(fmt,...)   DEF_LOG(CALLCONTROL,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  CALLCONTROL_NOTICE(fmt,...) DEF_LOG(CALLCONTROL,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  CALLCONTROL_INFO(fmt,...)   DEF_LOG(CALLCONTROL,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  CALLCONTROL_DEBUG(fmt,...)  DEF_LOG(CALLCONTROL,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log call center*/
#define  CALLCENTER_ERR(fmt,...)    DEF_LOG(CALLCENTER,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  CALLCENTER_WARN(fmt,...)   DEF_LOG(CALLCENTER,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  CALLCENTER_NOTICE(fmt,...) DEF_LOG(CALLCENTER,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  CALLCENTER_INFO(fmt,...)   DEF_LOG(CALLCENTER,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  CALLCENTER_DEBUG(fmt,...)  DEF_LOG(CALLCENTER,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log call completion*/
#define  CC_ERR(fmt,...)    DEF_LOG(CC,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  CC_WARN(fmt,...)   DEF_LOG(CC,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  CC_NOTICE(fmt,...) DEF_LOG(CC,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  CC_INFO(fmt,...)   DEF_LOG(CC,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  CC_DEBUG(fmt,...)  DEF_LOG(CC,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log call callpark*/
#define  CALLPARK_ERR(fmt,...)    DEF_LOG(CALLPARK,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  CALLPARK_WARN(fmt,...)   DEF_LOG(CALLPARK,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  CALLPARK_NOTICE(fmt,...) DEF_LOG(CALLPARK,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  CALLPARK_INFO(fmt,...)   DEF_LOG(CALLPARK,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  CALLPARK_DEBUG(fmt,...)  DEF_LOG(CALLPARK,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log common ui*/
#define  COMMONUI_ERR(fmt,...)    DEF_LOG(COMMONUI,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  COMMONUI_WARN(fmt,...)   DEF_LOG(COMMONUI,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  COMMONUI_NOTICE(fmt,...) DEF_LOG(COMMONUI,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  COMMONUI_INFO(fmt,...)   DEF_LOG(COMMONUI,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  COMMONUI_DEBUG(fmt,...)  DEF_LOG(COMMONUI,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log common api*/
#define  COMMONAPI_ERR(fmt,...)    DEF_LOG(COMMONAPI,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  COMMONAPI_WARN(fmt,...)   DEF_LOG(COMMONAPI,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  COMMONAPI_NOTICE(fmt,...) DEF_LOG(COMMONAPI,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  COMMONAPI_INFO(fmt,...)   DEF_LOG(COMMONAPI,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  COMMONAPI_DEBUG(fmt,...)  DEF_LOG(COMMONAPI,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log common units*/
#define  COMMONUNITS_ERR(fmt,...)    DEF_LOG(COMMONUNITS,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  COMMONUNITS_WARN(fmt,...)   DEF_LOG(COMMONUNITS,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  COMMONUNITS_NOTICE(fmt,...) DEF_LOG(COMMONUNITS,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  COMMONUNITS_INFO(fmt,...)   DEF_LOG(COMMONUNITS,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  COMMONUNITS_DEBUG(fmt,...)  DEF_LOG(COMMONUNITS,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log configparser*/
#define  CONFIGPARSER_ERR(fmt,...)    DEF_LOG(CONFIGPARSER,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  CONFIGPARSER_WARN(fmt,...)   DEF_LOG(CONFIGPARSER,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  CONFIGPARSER_NOTICE(fmt,...) DEF_LOG(CONFIGPARSER,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  CONFIGPARSER_INFO(fmt,...)   DEF_LOG(CONFIGPARSER,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  CONFIGPARSER_DEBUG(fmt,...)  DEF_LOG(CONFIGPARSER,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log devicelib*/
#define  DEVICELIB_ERR(fmt,...)    DEF_LOG(DEVICELIB,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  DEVICELIB_WARN(fmt,...)   DEF_LOG(DEVICELIB,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  DEVICELIB_NOTICE(fmt,...) DEF_LOG(DEVICELIB,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  DEVICELIB_INFO(fmt,...)   DEF_LOG(DEVICELIB,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  DEVICELIB_DEBUG(fmt,...)  DEF_LOG(DEVICELIB,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log directory ui*/
#define  DIRUI_ERR(fmt,...)    DEF_LOG(DIRUI,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  DIRUI_WARN(fmt,...)   DEF_LOG(DIRUI,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  DIRUI_NOTICE(fmt,...) DEF_LOG(DIRUI,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  DIRUI_INFO(fmt,...)   DEF_LOG(DIRUI,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  DIRUI_DEBUG(fmt,...)  DEF_LOG(DIRUI,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log dotwindows*/
#define  DOTWINDOWS_ERR(fmt,...)    DEF_LOG(DOTWINDOWS,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  DOTWINDOWS_WARN(fmt,...)   DEF_LOG(DOTWINDOWS,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  DOTWINDOWS_NOTICE(fmt,...) DEF_LOG(DOTWINDOWS,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  DOTWINDOWS_INFO(fmt,...)   DEF_LOG(DOTWINDOWS,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  DOTWINDOWS_DEBUG(fmt,...)  DEF_LOG(DOTWINDOWS,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log dsskey*/
#define  DSSKEY_ERR(fmt,...)    DEF_LOG(DSSKEY,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  DSSKEY_WARN(fmt,...)   DEF_LOG(DSSKEY,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  DSSKEY_NOTICE(fmt,...) DEF_LOG(DSSKEY,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  DSSKEY_INFO(fmt,...)   DEF_LOG(DSSKEY,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  DSSKEY_DEBUG(fmt,...)  DEF_LOG(DSSKEY,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log etlmsghandler*/
#define  ETLMSGHANDLER_ERR(fmt,...)    DEF_LOG(ETLMSGHANDLER,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  ETLMSGHANDLER_WARN(fmt,...)   DEF_LOG(ETLMSGHANDLER,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  ETLMSGHANDLER_NOTICE(fmt,...) DEF_LOG(ETLMSGHANDLER,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  ETLMSGHANDLER_INFO(fmt,...)   DEF_LOG(ETLMSGHANDLER,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  ETLMSGHANDLER_DEBUG(fmt,...)  DEF_LOG(ETLMSGHANDLER,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log fwd dnd*/
#define  FWDDND_ERR(fmt,...)    DEF_LOG(FWDDND,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  FWDDND_WARN(fmt,...)   DEF_LOG(FWDDND,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  FWDDND_NOTICE(fmt,...) DEF_LOG(FWDDND,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  FWDDND_INFO(fmt,...)   DEF_LOG(FWDDND,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  FWDDND_DEBUG(fmt,...)  DEF_LOG(FWDDND,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log hotdesking*/
#define  HOTDESKING_ERR(fmt,...)    DEF_LOG(HOTDESKING,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  HOTDESKING_WARN(fmt,...)   DEF_LOG(HOTDESKING,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  HOTDESKING_NOTICE(fmt,...) DEF_LOG(HOTDESKING,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  HOTDESKING_INFO(fmt,...)   DEF_LOG(HOTDESKING,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  HOTDESKING_DEBUG(fmt,...)  DEF_LOG(HOTDESKING,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log idle*/
#define  IDLE_ERR(fmt,...)    DEF_LOG(IDLE,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  IDLE_WARN(fmt,...)   DEF_LOG(IDLE,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  IDLE_NOTICE(fmt,...) DEF_LOG(IDLE,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  IDLE_INFO(fmt,...)   DEF_LOG(IDLE,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  IDLE_DEBUG(fmt,...)  DEF_LOG(IDLE,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log idleui*/
#define  IDLEUI_ERR(fmt,...)    DEF_LOG(IDLEUI,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  IDLEUI_WARN(fmt,...)   DEF_LOG(IDLEUI,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  IDLEUI_NOTICE(fmt,...) DEF_LOG(IDLEUI,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  IDLEUI_INFO(fmt,...)   DEF_LOG(IDLEUI,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  IDLEUI_DEBUG(fmt,...)  DEF_LOG(IDLEUI,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log lock*/
#define  LOCK_ERR(fmt,...)    DEF_LOG(LOCK,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  LOCK_WARN(fmt,...)   DEF_LOG(LOCK,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  LOCK_NOTICE(fmt,...) DEF_LOG(LOCK,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  LOCK_INFO(fmt,...)   DEF_LOG(LOCK,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  LOCK_DEBUG(fmt,...)  DEF_LOG(LOCK,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log ldap*/
#define  LDAP_ERR(fmt,...)    DEF_LOG(LDAP,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  LDAP_WARN(fmt,...)   DEF_LOG(LDAP,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  LDAP_NOTICE(fmt,...) DEF_LOG(LDAP,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  LDAP_INFO(fmt,...)   DEF_LOG(LDAP,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  LDAP_DEBUG(fmt,...)  DEF_LOG(LDAP,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log popupbox*/
#define  POPUPBOX_ERR(fmt,...)    DEF_LOG(POPUPBOX,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  POPUPBOX_WARN(fmt,...)   DEF_LOG(POPUPBOX,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  POPUPBOX_NOTICE(fmt,...) DEF_LOG(POPUPBOX,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  POPUPBOX_INFO(fmt,...)   DEF_LOG(POPUPBOX,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  POPUPBOX_DEBUG(fmt,...)  DEF_LOG(POPUPBOX,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log testmode*/
#define  TESTMODE_ERR(fmt,...)    DEF_LOG(TESTMODE,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  TESTMODE_WARN(fmt,...)   DEF_LOG(TESTMODE,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  TESTMODE_NOTICE(fmt,...) DEF_LOG(TESTMODE,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  TESTMODE_INFO(fmt,...)   DEF_LOG(TESTMODE,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  TESTMODE_DEBUG(fmt,...)  DEF_LOG(TESTMODE,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log uimanager*/
#define  UIMANAGER_ERR(fmt,...)    DEF_LOG(UIMANAGER,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  UIMANAGER_WARN(fmt,...)   DEF_LOG(UIMANAGER,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  UIMANAGER_NOTICE(fmt,...) DEF_LOG(UIMANAGER,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  UIMANAGER_INFO(fmt,...)   DEF_LOG(UIMANAGER,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  UIMANAGER_DEBUG(fmt,...)  DEF_LOG(UIMANAGER,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log voice*/
#define  VOICE_ERR(fmt,...)    DEF_LOG(VOICE,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  VOICE_WARN(fmt,...)   DEF_LOG(VOICE,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  VOICE_NOTICE(fmt,...) DEF_LOG(VOICE,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  VOICE_INFO(fmt,...)   DEF_LOG(VOICE,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  VOICE_DEBUG(fmt,...)  DEF_LOG(VOICE,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log xmlbrowser*/
#define  XMLBROWSER_ERR(fmt,...)    DEF_LOG(XMLBROWSER,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  XMLBROWSER_WARN(fmt,...)   DEF_LOG(XMLBROWSER,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  XMLBROWSER_NOTICE(fmt,...) DEF_LOG(XMLBROWSER,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  XMLBROWSER_INFO(fmt,...)   DEF_LOG(XMLBROWSER,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  XMLBROWSER_DEBUG(fmt,...)  DEF_LOG(XMLBROWSER,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log xmlbrowserui*/
#define  XMLBROWSERUI_ERR(fmt,...)    DEF_LOG(XMLBROWSERUI,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  XMLBROWSERUI_WARN(fmt,...)   DEF_LOG(XMLBROWSERUI,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  XMLBROWSERUI_NOTICE(fmt,...) DEF_LOG(XMLBROWSERUI,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  XMLBROWSERUI_INFO(fmt,...)   DEF_LOG(XMLBROWSERUI,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  XMLBROWSERUI_DEBUG(fmt,...)  DEF_LOG(XMLBROWSERUI,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log xsi*/
#define  XSI_ERR(fmt,...)    DEF_LOG(XSI,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  XSI_WARN(fmt,...)   DEF_LOG(XSI,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  XSI_NOTICE(fmt,...) DEF_LOG(XSI,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  XSI_INFO(fmt,...)   DEF_LOG(XSI,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  XSI_DEBUG(fmt,...)  DEF_LOG(XSI,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log zero*/
#define  ZERO_ERR(fmt,...)    DEF_LOG(ZERO,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  ZERO_WARN(fmt,...)   DEF_LOG(ZERO,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  ZERO_NOTICE(fmt,...) DEF_LOG(ZERO,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  ZERO_INFO(fmt,...)   DEF_LOG(ZERO,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  ZERO_DEBUG(fmt,...)  DEF_LOG(ZERO,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log zeroui*/
#define  ZEROUI_ERR(fmt,...)    DEF_LOG(ZEROUI,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  ZEROUI_WARN(fmt,...)   DEF_LOG(ZEROUI,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  ZEROUI_NOTICE(fmt,...) DEF_LOG(ZEROUI,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  ZEROUI_INFO(fmt,...)   DEF_LOG(ZEROUI,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  ZEROUI_DEBUG(fmt,...)  DEF_LOG(ZEROUI,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log sync*/
#define  SYNC_ERR(fmt,...)    DEF_LOG(SYNC,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  SYNC_WARN(fmt,...)   DEF_LOG(SYNC,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  SYNC_NOTICE(fmt,...) DEF_LOG(SYNC,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  SYNC_INFO(fmt,...)   DEF_LOG(SYNC,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  SYNC_DEBUG(fmt,...)  DEF_LOG(SYNC,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)


/*log globalinputhook*/
#define  GLOBALINPUTHOOK_ERR(fmt,...)    DEF_LOG(GLOBALINPUTHOOK,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  GLOBALINPUTHOOK_WARN(fmt,...)   DEF_LOG(GLOBALINPUTHOOK,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  GLOBALINPUTHOOK_NOTICE(fmt,...) DEF_LOG(GLOBALINPUTHOOK,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  GLOBALINPUTHOOK_INFO(fmt,...)   DEF_LOG(GLOBALINPUTHOOK,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  GLOBALINPUTHOOK_DEBUG(fmt,...)  DEF_LOG(GLOBALINPUTHOOK,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log inputmethod*/
#define  INPUTMETHOD_ERR(fmt,...)    DEF_LOG(INPUTMETHOD,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  INPUTMETHOD_WARN(fmt,...)   DEF_LOG(INPUTMETHOD,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  INPUTMETHOD_NOTICE(fmt,...) DEF_LOG(INPUTMETHOD,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  INPUTMETHOD_INFO(fmt,...)   DEF_LOG(INPUTMETHOD,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  INPUTMETHOD_DEBUG(fmt,...)  DEF_LOG(INPUTMETHOD,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log mainwnd*/
#define  MAINWND_ERR(fmt,...)    DEF_LOG(MAINWND,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  MAINWND_WARN(fmt,...)   DEF_LOG(MAINWND,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  MAINWND_NOTICE(fmt,...) DEF_LOG(MAINWND,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  MAINWND_INFO(fmt,...)   DEF_LOG(MAINWND,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  MAINWND_DEBUG(fmt,...)  DEF_LOG(MAINWND,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log messageui*/
#define  MESSAGEUI_ERR(fmt,...)    DEF_LOG(MESSAGEUI,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  MESSAGEUI_WARN(fmt,...)   DEF_LOG(MESSAGEUI,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  MESSAGEUI_NOTICE(fmt,...) DEF_LOG(MESSAGEUI,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  MESSAGEUI_INFO(fmt,...)   DEF_LOG(MESSAGEUI,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  MESSAGEUI_DEBUG(fmt,...)  DEF_LOG(MESSAGEUI,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log uikernel*/
#define  UIKERNEL_ERR(fmt,...)    DEF_LOG(UIKERNEL,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  UIKERNEL_WARN(fmt,...)   DEF_LOG(UIKERNEL,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  UIKERNEL_NOTICE(fmt,...) DEF_LOG(UIKERNEL,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  UIKERNEL_INFO(fmt,...)   DEF_LOG(UIKERNEL,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  UIKERNEL_DEBUG(fmt,...)  DEF_LOG(UIKERNEL,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log baseui*/
#define  BASEUI_ERR(fmt,...)    DEF_LOG(BASEUI,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  BASEUI_WARN(fmt,...)   DEF_LOG(BASEUI,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  BASEUI_NOTICE(fmt,...) DEF_LOG(BASEUI,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  BASEUI_INFO(fmt,...)   DEF_LOG(BASEUI,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  BASEUI_DEBUG(fmt,...)  DEF_LOG(BASEUI,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log application*/
#define  APPLICATION_ERR(fmt,...)    DEF_LOG(APPLICATION,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  APPLICATION_WARN(fmt,...)   DEF_LOG(APPLICATION,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  APPLICATION_NOTICE(fmt,...) DEF_LOG(APPLICATION,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  APPLICATION_INFO(fmt,...)   DEF_LOG(APPLICATION,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  APPLICATION_DEBUG(fmt,...)  DEF_LOG(APPLICATION,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log backlight*/
#define  BACKLIGHT_ERR(fmt,...)    DEF_LOG(BACKLIGHT,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  BACKLIGHT_WARN(fmt,...)   DEF_LOG(BACKLIGHT,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  BACKLIGHT_NOTICE(fmt,...) DEF_LOG(BACKLIGHT,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  BACKLIGHT_INFO(fmt,...)   DEF_LOG(BACKLIGHT,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  BACKLIGHT_DEBUG(fmt,...)  DEF_LOG(BACKLIGHT,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log broadsoft_sca*/
#define  BROADSOFT_SCA_ERR(fmt,...)    DEF_LOG(BROADSOFT_SCA,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  BROADSOFT_SCA_WARN(fmt,...)   DEF_LOG(BROADSOFT_SCA,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  BROADSOFT_SCA_NOTICE(fmt,...) DEF_LOG(BROADSOFT_SCA,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  BROADSOFT_SCA_INFO(fmt,...)   DEF_LOG(BROADSOFT_SCA,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  BROADSOFT_SCA_DEBUG(fmt,...)  DEF_LOG(BROADSOFT_SCA,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log calllogui*/
#define  CALLLOGUI_ERR(fmt,...)    DEF_LOG(CALLLOGUI,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  CALLLOGUI_WARN(fmt,...)   DEF_LOG(CALLLOGUI,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  CALLLOGUI_NOTICE(fmt,...) DEF_LOG(CALLLOGUI,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  CALLLOGUI_INFO(fmt,...)   DEF_LOG(CALLLOGUI,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  CALLLOGUI_DEBUG(fmt,...)  DEF_LOG(CALLLOGUI,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log dsskeyui*/
#define  DSSKEYUI_ERR(fmt,...)    DEF_LOG(DSSKEYUI,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  DSSKEYUI_WARN(fmt,...)   DEF_LOG(DSSKEYUI,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  DSSKEYUI_NOTICE(fmt,...) DEF_LOG(DSSKEYUI,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  DSSKEYUI_INFO(fmt,...)   DEF_LOG(DSSKEYUI,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  DSSKEYUI_DEBUG(fmt,...)  DEF_LOG(DSSKEYUI,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log ledlamp*/
#define  LEDLAMP_ERR(fmt,...)    DEF_LOG(LEDLAMP,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  LEDLAMP_WARN(fmt,...)   DEF_LOG(LEDLAMP,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  LEDLAMP_NOTICE(fmt,...) DEF_LOG(LEDLAMP,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  LEDLAMP_INFO(fmt,...)   DEF_LOG(LEDLAMP,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  LEDLAMP_DEBUG(fmt,...)  DEF_LOG(LEDLAMP,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log menu*/
#define  MENU_ERR(fmt,...)    DEF_LOG(MENU,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  MENU_WARN(fmt,...)   DEF_LOG(MENU,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  MENU_NOTICE(fmt,...) DEF_LOG(MENU,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  MENU_INFO(fmt,...)   DEF_LOG(MENU,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  MENU_DEBUG(fmt,...)  DEF_LOG(MENU,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log taskaction*/
#define  TA_ERR(fmt,...)    DEF_LOG(TASKACTION,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  TA_WARN(fmt,...)   DEF_LOG(TASKACTION,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  TA_NOTICE(fmt,...) DEF_LOG(TASKACTION,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  TA_INFO(fmt,...)   DEF_LOG(TASKACTION,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  TA_DEBUG(fmt,...)  DEF_LOG(TASKACTION,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log cgiserver*/
#define  CGISERVER_ERR(fmt,...)    DEF_LOG(CGISERVER,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  CGISERVER_WARN(fmt,...)   DEF_LOG(CGISERVER,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  CGISERVER_NOTICE(fmt,...) DEF_LOG(CGISERVER,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  CGISERVER_INFO(fmt,...)   DEF_LOG(CGISERVER,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  CGISERVER_DEBUG(fmt,...)  DEF_LOG(CGISERVER,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log configtranslate*/
#define  CONFIGTRANSLATE_ERR(fmt,...)    DEF_LOG(CONFIGTRANSLATE,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  CONFIGTRANSLATE_WARN(fmt,...)   DEF_LOG(CONFIGTRANSLATE,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  CONFIGTRANSLATE_NOTICE(fmt,...) DEF_LOG(CONFIGTRANSLATE,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  CONFIGTRANSLATE_INFO(fmt,...)   DEF_LOG(CONFIGTRANSLATE,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  CONFIGTRANSLATE_DEBUG(fmt,...)  DEF_LOG(CONFIGTRANSLATE,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log idlebox*/
#define  IDLEBOX_ERR(fmt,...)    DEF_LOG(IDLEBOX,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  IDLEBOX_WARN(fmt,...)   DEF_LOG(IDLEBOX,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  IDLEBOX_NOTICE(fmt,...) DEF_LOG(IDLEBOX,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  IDLEBOX_INFO(fmt,...)   DEF_LOG(IDLEBOX,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  IDLEBOX_DEBUG(fmt,...)  DEF_LOG(IDLEBOX,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log help*/
#define  HELP_ERR(fmt,...)    DEF_LOG(HELP,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  HELP_WARN(fmt,...)   DEF_LOG(HELP,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  HELP_NOTICE(fmt,...) DEF_LOG(HELP,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  HELP_INFO(fmt,...)   DEF_LOG(HELP,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  HELP_DEBUG(fmt,...)  DEF_LOG(HELP,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log contactparser*/
#define  CONTACTPARSER_ERR(fmt,...)    DEF_LOG(CONTACTPARSER,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  CONTACTPARSER_WARN(fmt,...)   DEF_LOG(CONTACTPARSER,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  CONTACTPARSER_NOTICE(fmt,...) DEF_LOG(CONTACTPARSER,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  CONTACTPARSER_INFO(fmt,...)   DEF_LOG(CONTACTPARSER,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  CONTACTPARSER_DEBUG(fmt,...)  DEF_LOG(CONTACTPARSER,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log web*/
#define  WEB_ERR(fmt,...)    DEF_LOG(WEB,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  WEB_WARN(fmt,...)   DEF_LOG(WEB,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  WEB_NOTICE(fmt,...) DEF_LOG(WEB,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  WEB_INFO(fmt,...)   DEF_LOG(WEB,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  WEB_DEBUG(fmt,...)  DEF_LOG(WEB,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log etll*/
#define  ETLLIB_ERR(fmt,...)    DEF_LOG(ETLLIB,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  ETLLIB_WARN(fmt,...)   DEF_LOG(ETLLIB,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  ETLLIB_NOTICE(fmt,...) DEF_LOG(ETLLIB,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  ETLLIB_INFO(fmt,...)   DEF_LOG(ETLLIB,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  ETLLIB_DEBUG(fmt,...)  DEF_LOG(ETLLIB,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log configServer*/
#define  CSRV_ERR(fmt,...)    DEF_LOG(CSRV,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  CSRV_WARN(fmt,...)   DEF_LOG(CSRV,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  CSRV_NOTICE(fmt,...) DEF_LOG(CSRV,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  CSRV_INFO(fmt,...)   DEF_LOG(CSRV,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  CSRV_DEBUG(fmt,...)  DEF_LOG(CSRV,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log WIFI & BLUETOOTH*/
#define  WIRE_ERR(fmt,...)    DEF_LOG(WIRE,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  WIRE_WARN(fmt,...)   DEF_LOG(WIRE,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  WIRE_NOTICE(fmt,...) DEF_LOG(WIRE,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  WIRE_INFO(fmt,...)   DEF_LOG(WIRE,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  WIRE_DEBUG(fmt,...)  DEF_LOG(WIRE,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log EDK*/
#define  EDK_ERR(fmt,...)    DEF_LOG(EDK,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  EDK_WARN(fmt,...)   DEF_LOG(EDK,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  EDK_NOTICE(fmt,...) DEF_LOG(EDK,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  EDK_INFO(fmt,...)   DEF_LOG(EDK,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  EDK_DEBUG(fmt,...)  DEF_LOG(EDK,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log VLOG*/
#define  VIDEO_ERR(fmt,...)    DEF_LOG(VLOG,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  VIDEO_WARN(fmt,...)   DEF_LOG(VLOG,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  VIDEO_NOTICE(fmt,...) DEF_LOG(VLOG,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  VIDEO_INFO(fmt,...)   DEF_LOG(VLOG,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  VIDEO_DEBUG(fmt,...)  DEF_LOG(VLOG,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log VKE*/
#define  VKE_ERR(fmt,...)    DEF_LOG(VKE,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  VKE_WARN(fmt,...)   DEF_LOG(VKE,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  VKE_NOTICE(fmt,...) DEF_LOG(VKE,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  VKE_INFO(fmt,...)   DEF_LOG(VKE,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  VKE_DEBUG(fmt,...)  DEF_LOG(VKE,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log LYNC*/
#define  LYNC_ERR(fmt,...)    DEF_LOG(LYNC,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  LYNC_WARN(fmt,...)   DEF_LOG(LYNC,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  LYNC_NOTICE(fmt,...) DEF_LOG(LYNC,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  LYNC_INFO(fmt,...)   DEF_LOG(LYNC,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  LYNC_DEBUG(fmt,...)  DEF_LOG(LYNC,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log Line Device*/
#define  LINEDEVICE_ERR(fmt,...)    DEF_LOG(LINE,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  LINEDEVICE_WARN(fmt,...)   DEF_LOG(LINE,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  LINEDEVICE_NOTICE(fmt,...) DEF_LOG(LINE,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  LINEDEVICE_INFO(fmt,...)   DEF_LOG(LINE,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  LINEDEVICE_DEBUG(fmt,...)  DEF_LOG(LINE,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log USB Record*/
#define  USBDEVICE_ERR(fmt,...)    DEF_LOG(USBD,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  USBDEVICE_WARN(fmt,...)   DEF_LOG(USBD,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  USBDEVICE_NOTICE(fmt,...) DEF_LOG(USBD,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  USBDEVICE_INFO(fmt,...)   DEF_LOG(USBD,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  USBDEVICE_DEBUG(fmt,...)  DEF_LOG(USBD,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log Bluetooth*/
#define  BLUETOOTH_ERR(fmt,...)    DEF_LOG(BLTH,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  BLUETOOTH_WARN(fmt,...)   DEF_LOG(BLTH,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  BLUETOOTH_NOTICE(fmt,...) DEF_LOG(BLTH,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  BLUETOOTH_INFO(fmt,...)   DEF_LOG(BLTH,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  BLUETOOTH_DEBUG(fmt,...)  DEF_LOG(BLTH,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log HandSet*/
#define  HANDSET_ERR(fmt,...)    DEF_LOG(HAND,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  HANDSET_WARN(fmt,...)   DEF_LOG(HAND,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  HANDSET_NOTICE(fmt,...) DEF_LOG(HAND,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  HANDSET_INFO(fmt,...)   DEF_LOG(HAND,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  HANDSET_DEBUG(fmt,...)  DEF_LOG(HAND,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log CMD Porcess*/
#define  CMDPROCESSOR_ERR(fmt,...)    DEF_LOG(CMDP,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  CMDPROCESSOR_WARN(fmt,...)   DEF_LOG(CMDP,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  CMDPROCESSOR_NOTICE(fmt,...) DEF_LOG(CMDP,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  CMDPROCESSOR_INFO(fmt,...)   DEF_LOG(CMDP,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  CMDPROCESSOR_DEBUG(fmt,...)  DEF_LOG(CMDP,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log CMD Porcess*/
#define  SS7_ERR(fmt,...)    DEF_LOG(SS7,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  SS7_WARN(fmt,...)   DEF_LOG(SS7,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  SS7_NOTICE(fmt,...) DEF_LOG(SS7,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  SS7_INFO(fmt,...)   DEF_LOG(SS7,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  SS7_DEBUG(fmt,...)  DEF_LOG(SS7,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log FWUP*/
#define  FWUP_ERR(fmt,...)    DEF_LOG(FWUP,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  FWUP_WARN(fmt,...)   DEF_LOG(FWUP,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  FWUP_NOTICE(fmt,...) DEF_LOG(FWUP,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  FWUP_INFO(fmt,...)   DEF_LOG(FWUP,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  FWUP_DEBUG(fmt,...)  DEF_LOG(FWUP,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)

/*log xml parser*/
//#define  XMLP_ERR(fmt,...)    DEF_LOG(XMLP,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
//#define  XMLP_WARN(fmt,...)   DEF_LOG(XMLP,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
//#define  XMLP_NOTICE(fmt,...) DEF_LOG(XMLP,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
//#define  XMLP_INFO(fmt,...)   DEF_LOG(XMLP,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
//#define  XMLP_DEBUG(fmt,...)  DEF_LOG(XMLP,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)
#ifdef WIN32
#define  WIN_ERR(fmt,...)    DEF_LOG(WIN,DBG_LOG_LEVEL_ERR,    fmt"\n", ##__VA_ARGS__)
#define  WIN_WARN(fmt,...)   DEF_LOG(WIN,DBG_LOG_LEVEL_WARN,   fmt"\n", ##__VA_ARGS__)
#define  WIN_NOTICE(fmt,...) DEF_LOG(WIN,DBG_LOG_LEVEL_NOTICE, fmt"\n", ##__VA_ARGS__)
#define  WIN_INFO(fmt,...)   DEF_LOG(WIN,DBG_LOG_LEVEL_INFO,   fmt"\n", ##__VA_ARGS__)
#define  WIN_DEBUG(fmt,...)  DEF_LOG(WIN,DBG_LOG_LEVEL_DEBUG,  fmt"\n", ##__VA_ARGS__)
#else
#define  WIN_ERR(fmt,...)
#define  WIN_WARN(fmt,...)
#define  WIN_NOTICE(fmt,...)
#define  WIN_INFO(fmt,...)
#define  WIN_DEBUG(fmt,...)
#endif //end ifdef _WIN32

/************************************************************************/
/*                                                                      */
/************************************************************************/

#endif /*__APP_DEBUG_LOG_H__*/
