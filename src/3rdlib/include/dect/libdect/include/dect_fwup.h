/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2010-10-21
*
*******************************************************************/
#ifndef __DECT_FWUP_H__
#define __DECT_FWUP_H__

#include "dect_msg.h"


/************************************************************************/
/*                                                                      */
/************************************************************************/
/* max inst.*/
#define FWUP_MAX_INST           5
#define FWUP_MAX_INTERVAL_TIME_CASE           (FWUP_MAX_INST*2+1)

#define FWUP_ROM_NAME_LEN       16
#define FWUP_ROM_VER_LEN        24

#define FWUP_ROM_DEV_W5X_HS     26
#define FWUP_ROM_DEV_W56_HS     61

/* hs version and hs state check */
/* hs signal.*/
#define FWUP_HS_SIGNAL_NOTSUB   1
#define FWUP_HS_SIGNAL_OUTRANGE 2
#define FWUP_HS_SIGNAL_1        3
#define FWUP_HS_SIGNAL_2        4
#define FWUP_HS_SIGNAL_3        5
#define FWUP_HS_SIGNAL_4        6
#define FWUP_HS_SIGNAL_FULL     FWUP_HS_SIGNAL_4

#define FWUP_HS_SIGNAL_THRD     FWUP_HS_SIGNAL_2

/* hs battery.*/
#define FWUP_HS_BATTERY_EMPTY   0
#define FWUP_HS_BATTERY_1       1
#define FWUP_HS_BATTERY_2       2
#define FWUP_HS_BATTERY_3       3
#define FWUP_HS_BATTERY_4       4
#define FWUP_HS_BATTERY_5       5
#define FWUP_HS_BATTERY_CHARGING_1  6
#define FWUP_HS_BATTERY_CHARGING_2  7
#define FWUP_HS_BATTERY_CHARGING_3  8
#define FWUP_HS_BATTERY_CHARGING_4  9
#define FWUP_HS_BATTERY_CHARGING_5  10

#define FWUP_HS_BATTERY_THRD     FWUP_HS_BATTERY_3

#define FWUP_HS_HW_VER_THRD     6

/* error code */
#define FWUP_ERR_OK        0
#define FWUP_ERR_FAILURE  -1
#define FWUP_ERR_TIMEOUT  -2
#define FWUP_ERR_BUS      -3
#define FWUP_ERR_PROTO    -4
#define FWUP_ERR_CMD      -5
#define FWUP_ERR_ARG      -6
#define FWUP_ERR_NACK     -7
#define FWUP_ERR_LEN      -8
#define FWUP_ERR_RESP     -9
#define FWUP_ERR_CRC      -10
#define FWUP_ERR_MAGIC    -11
#define FWUP_ERR_CRC_VERIFY      -12
#define FWUP_ERR_CMD_TIMEOUT  -13
#define FWUP_ERR_RESP_TIMEOUT  -14

/* same as data handle api error types.*/
#define FWUP_ERR_HS_LINK_BUSY           -30
#define FWUP_ERR_DATA_HANDLE_CLOSED     -31
#define FWUP_ERR_SEND_BUSY              -32
#define FWUP_ERR_DATA_HANDLE_EXSIT      -33
#define FWUP_ERR_EST_TIME_OUT           -34
#define FWUP_ERR_SEND_TIME_OUT          -35
#define FWUP_ERR_DATA_HANDLE_DEINIT     -36
#define FWUP_ERR_HS_NOT_ALIVE           -37
#define FWUP_ERR_SPEED_TEST             -38


#define FWUP_ERR_OEM_VER              -51
#define FWUP_ERR_DEV_ID               -52
#define FWUP_ERR_SAME_ROM_VER         -53
#define FWUP_ERR_UPGRADE_FFS          -54
#define FWUP_ERR_UPGRADE_APP          -55
#define FWUP_ERR_GET_PVERSION         -56



/* create inst flag.*/
#define FWUP_FLAG_SKIP_CHECK_VERSION    (1L<<0) 
#define FWUP_FLAG_SKIP_VERIFY           (1L<<1)
#define FWUP_FLAG_SKIP_BURN_APP         (1L<<2)
#define FWUP_FLAG_SKIP_BURN_FFS         (1L<<3)
#define FWUP_FLAG_SKIP_BURN_VERSION     (1L<<4)
#define FWUP_FLAG_SHOW_ALL_LOG          (1L<<5)
#define FWUP_FLAG_LOOP_TEST             (1L<<6)
#define FWUP_FLAG_APPFLAG_TEST          (1L<<7)
#define FWUP_FLAG_SPEED_TEST            (1L<<8)
#define FWUP_FALG_POWER_LOST_TEST       (1L<<9)
#define FWUP_FALG_CHECK_APP_VERSION     (1L<<10)

/* command timeout */
#define FWUP_CMD_TIMEOUT_SEC 11 /* 11 seconds.*/
#define FWUP_RESP_TIMEOUT_SEC 11 /* 11 seconds.*/

/* data call est timeout */
#define FWUP_EST_TIMEOUT_SEC 30 /* 30 seconds.*/
#define FWUP_MAX_EST_TIMES   3  /* 30 seconds.*/
#define FWUP_MAX_REOPEN_EST_TIMES   2  /* 20 seconds.*/

/************************************************************************/
/*                                                                      */
/************************************************************************/
/* user event indication */
#define FWUP_EVENT_ONDOING                      0X01
#define FWUP_EVENT_PROGRESS                     0X02
#define FWUP_EVENT_RESULT                       0X03

/* OnDoing -- Check Version*/
#define FWUP_ONDOING_CV_CMP_ROM_VER             0X04
#define FWUP_ONDOING_CV_CMP_APP_VER             0X05
#define FWUP_ONDOING_CV_CMP_FFS_VER             0X06

/* OnDoing -- Uprade FFS*/
#define FWUP_ONDOING_UF_ERASE_VER               0X07
#define FWUP_ONDOING_UF_BURN_FFS                0X08
#define FWUP_ONDOING_UF_BURN_VER                0X09

/* OnDoing -- Uprade App*/
#define FWUP_ONDOING_UA_ERASE_VER               0X0A
#define FWUP_ONDOING_UA_ERASE_APP_FLAG          0X0B
#define FWUP_ONDOING_UA_BURN_APP                0X0C
#define FWUP_ONDOING_UA_BURN_APP_FLAG           0X0D
#define FWUP_ONDOING_UA_BUEN_VER                0X0E
#define FWUP_ONDOING_UA_ERASE_APP_VER           0X0F

#endif /* __DECT_FWUP_H__ */
