/************************************************************************
 * FileName  : DssKeyUI_common.h (header file)
 * Purpose   :
 * Date      : 2012/09/28 20:02:32
 ************************************************************************/

#ifndef _DSSKEYUI_COMMON_H_
#define _DSSKEYUI_COMMON_H_

#include "dsskeyui/include/dsskeyuicommondefine.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "dsklog/log.h"

#define DSSK_SLOWER_FLASH_TIMER     1000
#define DSSK_SLOW_FLASH_TIMER       500
#define DSSK_FAST_FLASH_TIMER       300
#define DSSK_FASTER_FLASH_TIMER     200

static const  yl::string DSSKEY_NULL_ACTION = "N/A";
static const int EXPKEY_PER_PAGE_COUNT  = 20;

#ifdef _T3X
static const int LINEKEY_PER_PAGE_COUNT = 6;
static const int LINEKEY_LED_NUM = 6;
static const int MEMORY_PER_PAGE_COUNT = 10;
static const int MEMORY_LED_NUM = 10;
#endif

#ifdef _T4X
static const int LINEKEY_PER_PAGE_COUNT = 10;   //每页DssKey的个数
static const int LINEKEY_LED_NUM = 10;
#endif

#endif //_DSSKEYUI_COMMON_H_
