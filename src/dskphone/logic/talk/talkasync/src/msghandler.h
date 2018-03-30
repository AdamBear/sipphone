#ifndef __TALK_AYNC_MSG_HANDLER_HEADER_H__
#define __TALK_AYNC_MSG_HANDLER_HEADER_H__

/* 来电异步处理 */
bool talksync_OnIncomingCall(int iCallID, int iAccountID, CallInUserInfo& userInfo);

/* 创建同步流程 : 搜索铃声 */
bool talksync_GetIncomingCallRing(int iCallId, int iAccount, CallInUserInfo* pCallData,
                                  yl::string& strPath, int& iStatus, int& iRingIndex);

bool talksync_GetIncomingCallRing(int iAccount, const yl::string& strNumber, yl::string& strPath);


#endif
