#ifndef __TALK_MOD_VQRTCP_HEADER_H__
#define __TALK_MOD_VQRTCP_HEADER_H__

#include "talk/vqrtcp/include/vqrtcp_data.h"

/* 模块初始化 */
void vqrtcp_Init();

/* 模块析构 */
void vqrtcp_UnInit();

/*******************************************************************
** 函数描述:    获取最后一次通话的语音质量数据
** 参数:        [out] sData: 显示数据结构体
** 返回:        无
********************************************************************/
void vqrtcp_RTPStatusGetSessionReport(SVQReportData& sData);

/*******************************************************************
** 函数描述:    获取通话中的语音质量数据
** 参数:        [out] sData: 显示数据结构体
** 返回:        true:获取成功， false:获取失败
********************************************************************/
bool vqrtcp_RTPStatusGetCurrentReport(SVQReportData& sData);

/*******************************************************************
** 函数描述:    是否可切换TP Status
** 参数:        无
** 返回:        true:切换成功， false:切换失败
********************************************************************/
bool vqrtcp_IsRTPStatusSwitch();

/*******************************************************************
** 函数描述:    切换显示RTP Status
** 参数:        无
** 返回:        true:切换成功， false:切换失败
********************************************************************/
bool vqrtcp_RTPStatusSwitch();

#endif
