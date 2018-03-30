#ifndef __VIRTUAL_VOICEMAIL_MANAGER_H__
#define __VIRTUAL_VOICEMAIL_MANAGER_H__

#include "voicemailcommon.h"

//进入调用前先设置账号
void vvMail_SetAccountId(int nLine);
//获取当前Voicemail账号
int vvMail_GetAccountId();
//请求Message列表
bool vvMail_ReqVoicemessageList();
//请求Message播放内容
bool vvMail_ReqPlayContentByIndex(int index);
//标记所有为已读
bool vvMail_ReqMarkAllRead();
//标记所有为未读
bool vvMail_ReqMarkAllUnRead();
//切换状态
bool vvMail_ReqMarkByIndex(int index, bool bRead);
//删除指定Message
bool vvMail_ReqDeleteMessageByIndex(int index);
//退出界面最好调用该接口清除数据，减小内存压力
void vvMail_ClearListData();
//获取Message列表
bool vvMail_GetVoiceMailListData(VoiceMailListData * pData);
//获取指定Message详细信息
bool vvMail_GetDetailByIndex(sVoiceMailData & refData, int index);
//Visual Voicemail是否可用
bool vvMail_IsEnable();
//取消所有请求
void vvMail_CancelAllRequest();
//播放Voicemail
bool vvMail_Play();
//暂停Voicemail
bool vvMail_Pause();
//恢复Voicemail
bool vvMail_Resume();
//停止Voicemail
bool vvMail_Stop();
//定位Voicemail
bool vvMail_Seek(unsigned int locate);
//获取当前播放状态
ePlayStatus vvMail_GetPlayStatus();
//获取Voicemail总数
int vvMail_GetTotalCount();

#endif
