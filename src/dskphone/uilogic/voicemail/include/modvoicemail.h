#ifndef __MOD_VOICEMAIL_H__
#define __MOD_VOICEMAIL_H__
#include "voicemaildefines.h"

//初始化
bool vmail_Init();

/************************************************************************
** 函数描述:   获取未读语音留言数量
** 参数：      [in] iAccountID: 账号ID
              [in] bNotify： 通知的消息数量
** 返回:      未读语音留言数量
************************************************************************/
int vmail_GetUnReadVoiceMailNum(int iAccountID = -1, bool bNotify = false);

/************************************************************************
** 函数描述:   获取已读语音留言数量
** 参数：      [in] iAccountID: 账号ID
              [in] bNotify： 通知的消息数量
** 返回:      已读语音留言数量
************************************************************************/
int vmail_GetReadedVoiceMailNum(int iAccountID = -1, bool bNotify = false);

/************************************************************************
** 函数描述:   获取最后一个来语音留言的账号
** 参数：
** 返回:      最后一个来语音留言的账号ID
************************************************************************/
int vmail_GetLastVMailAccountId();

/************************************************************************
** 函数描述:  点击语音信箱信息
** 参数：    [in] iAccountID: 账号ID
** 返回:
************************************************************************/
void vmail_ClickInfoByAccount(int iAccount);

// 获取Voice Mail配置项列表.
// | pData | [out] 是buffer.
// 如果获取成功则返回true, 否则返回false.
bool vmail_GetConfigList(VoiceMailConfigListData & refData);

// 设置Voice Mail配置项列表.
// | refData | 是传入的配置列表.
void vmail_SetConfigList(VoiceMailConfigListData & refData);

// 获取所有账号当前有的Voice Mail列表.
// | pData | [out] 是buffer.
// 如果获取成功则返回true, 否则返回false.
bool vmail_GetInfoList(VoiceMailInfoListData & refData);

// Message按键处理
bool vmail_OnMsgKeyPress();

// 清除通知，但不删除数量
void vmail_RemoveNotify(int iAccountID = -1);

#if 0
// 获取账号的Voice Mail列表.
// | pData | [out] 是buffer.
// 如果获取成功则返回true, 否则返回false.
bool vmail_GetInfo(int iAccountID, VoiceMailInfoData & refData);

// 设置账号的Voice Mail配置项列表.
// | refData | 是传入的配置列表.
void vmail_SetConfig(int iAccountID, VoiceMailConfigData & refData);

// 获取账号的Voice Mail配置项.
// | pData | [out] 是buffer.
// 如果获取成功则返回true, 否则返回false.
bool vmail_GetConfig(int iAccountID, VoiceMailConfigData & refData);
#endif // 0

#endif //__MOD_VOICEMAIL_H__
