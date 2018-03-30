#ifndef __TALK_MOD_RINGHELP_HEADER_H__
#define __TALK_MOD_RINGHELP_HEADER_H__

#include "ETLLib/ETLLib.hpp"
#include <ylstring.h>
#include "talk/talkhelp/include/talkhelp_data.h"
#include "talk/talklogic/include/sipstructmapfunc.h"
#include "voice/include/voicecommon.h"

/* 初始化 */
void talkhelp_Init();

/* 析构 */
void talkhelp_UnInit();

/* 获取联系人对应的铃声路径 */
bool talkhelp_GetContractRing(yl::string & strPath, LPCSTR lpNumber);

/* 获取默认铃声 */
bool talkhelp_GetDefaultRing(yl::string & strPath);

/* 获取CallWaiting铃声类型 */
bool talkhelp_GetCallWaitingToneType(TONE_TYPE & eToneType, CallInUserInfo * pCallData);

/* 获取DialTone铃声类型 */
TONE_TYPE talkhelp_GetDialTone(int iAccountID);

/* 获取错误码对应的错误标志描述字符 */
LPCSTR talkhelp_GetErrorInfo(int iType);

/* 判断CallWaiting开关状态 */
bool talkhelp_IsCallWaitingEnable();

/*******************************************************************
** 函数描述:   获取账号对应的安全等级开关
** 参数:       iAccount：账号ID
** 返回:       开关状态
********************************************************************/
bool talklogic_IsShowSecurity(int iAccountID);


/*******************************************************************
** 函数描述:   获取账号对应的安全等级
** 参数:     iAccount：账号ID
** 返回:       当前的安全等级
********************************************************************/
yl::string talklogic_GetAccountSecurityLevel(int iAccount);

/*******************************************************************
** 函数描述:   获取账号的覆盖安全等级：通话中设置的等级
** 参数:     iAccount：账号ID
** 返回:       覆盖的安全等级
********************************************************************/
yl::string talklogic_GetAccountOverrideLevel(int iAccount);

/*******************************************************************
** 函数描述:   获取账号可设置的安全等级列表
** 参数:     lstAllowLevels：可设置的安全等级列表
               iAccount：账号ID
** 返回:       获取成功返回true,否则返回false
********************************************************************/
bool talklogic_GetAccountAllowSecurityLevels(YLList<yl::string> & lstAllowLevels, int iAccount);

/*******************************************************************
** 函数描述:   通话中修改当前账号的安全等级
** 参数:     iAccount：账号ID
               strLevel:安全等级
** 返回:       修改成功返回true,否则返回false
********************************************************************/
bool talklogic_ModifyAccountOverrideLevel(int iAccount, const yl::string & strLevel);

/*******************************************************************
** 函数描述:   获取通话的安全等级
** 参数:     iSessId：话路的唯一标识
** 返回:       返回通话的安全等级
********************************************************************/
yl::string talklogic_GetTalkSecurityLevel(int iSessId);

/*******************************************************************
** 函数描述:   获取本地会议中安全等级最小的账号
** 参数:
** 返回:       返回账号ID
********************************************************************/
int talklogic_GetAccountLeastLevFromLocalConf();

#endif
