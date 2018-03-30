/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2015-12-10
*
*******************************************************************/

#ifndef __DECT_STAT_H__
#define __DECT_STAT_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/

typedef struct{
	unsigned int u32_locate_cnt;
	unsigned int u32_alive_cnt;
	unsigned int u32_shutdown_cnt;
	unsigned int u32_page_cnt;
	unsigned int u32_page_fail_cnt;
	unsigned int u32_dl04_disc_cnt;
	unsigned int u32_ic_cnt;
	unsigned int u32_oc_cnt;
	unsigned int u32_la_cnt;
	unsigned int u32_notify_cnt;
	unsigned int u32_notify_unreachable_cnt;
	unsigned int u32_LinkDisWhileRun;
	unsigned short u16_la_queue_full_cnt;
	unsigned short u16_iwu_queue_full_cnt;
	unsigned short u16_ptp_queue_full_cnt;
	unsigned short u16_reserved1;
}st_term_stat;


typedef struct{
	st_term_stat st_term[5+1]; /* SD09_MAX_NUM_INST+1 */
	unsigned int u32_paging_cnt;
	unsigned int u32_ic_cnt;
	unsigned int u32_oc_cnt;
	unsigned int u32_notify_cnt;


	unsigned int u32_facility_reset_cnt;
	unsigned int u32_dsr_notify_SLC_cnt;
	unsigned int u32_coma_error_cnt;
	unsigned int u32_fatal_error_cnt;
	unsigned int u32_calldata_slottype_error_cnt;
	unsigned int u32_hlpdata_slottype_error_cnt;
	unsigned int u32_BFiledCleanupCnt;
	unsigned short u16_NodeAliasCleanupCnt;
	unsigned short u16_Reserve0;
	unsigned int u32_LinkDisWhileRun;
	unsigned int u32_LostFrameCnt;
    unsigned int u32_RunTimeMin;

	unsigned int u32_Magic; /*end flag*/
}st_coma_stat;

/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif

