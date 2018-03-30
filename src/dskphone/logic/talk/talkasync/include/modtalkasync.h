#ifndef __TALK_MOD_TALK_ASYNC_HEADER_H__
#define __TALK_MOD_TALK_ASYNC_HEADER_H__

#include "talk/talkasync/include/talkasync_data.h"

class CCallInfo;

/* 初始化 */
void talkasync_Init();

/* 析构 */
void talkasync_UnInit();

/* 创建异步流程 : 联系人头像 */
bool talkasync_CreateUpdateCallerImage(int iCallID, const yl::string& strUrl);

/* 创建异步流程 : 下载铃声 */
bool talkasync_CreateGetRingFile(int iCallID, const yl::string& strUrl);

/* 创建异步流程 : Ldap联系人搜索 */
bool talkasync_CreateLdapSearch(CCallInfo* pCallInfo);

#if IF_BUG_38979
//来电异步匹配联系人
bool talkasync_GetCotactNameByNumber(int iCallID, const yl::string& strNumber);
#endif

void talkasync_ReleaseLdapSearch(CCallInfo* pCallInfo);
#endif
