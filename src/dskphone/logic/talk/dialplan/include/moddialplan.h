#ifndef __TALK_HELP_DIALPLAN_H_
#define __TALK_HELP_DIALPLAN_H_

#include "talk/dialplan/include/dialplan_data.h"

struct CallOut_Param;

/* 初始化 */
void dialplan_Init();

/* 析构 */
void dialplan_UnInit();

/* 获取DialNow数据 */
void dialplan_GetDialNowData(DialNowListData& listDialNowData);

/* 匹配AreaCode */
bool dialplan_GetAreaCodeNumber(const yl::string& strText, yl::string& strAreaCode, int iLineId);

/* 匹配DialPlan内部的黑名单号码 */
bool dialplan_IsBlockCallout(const yl::string& strText, int iLineID,
                             CallOut_Param* pCallParam = NULL);

/* 执行DialPlan内部替换规则 */
bool dialplan_GetReplaceNumber(yl::string& strText, int iLineID, CallOut_Param* pCallParam = NULL);

/* 执行DialPlan内部替换规则 */
bool dialplan_GetReplaceNumberEx(yl::string& strText, int iLineID, DialPlanCallMask mask);

/* 获取呼出的参数 */
bool dialplan_GetCalloutParam(int iSessionID, int iLineID, int iMask, CallOut_Param& callparam);

/* 强制将号码改为规则定义的号码 */
bool dialplan_GetChangeAccount(int& iLineID, LPCSTR lpNumber, CallOut_Param* pCallParam = NULL);

#endif  // __TALK_HELP_DIALPLAN_H_
