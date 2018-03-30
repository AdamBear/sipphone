#ifndef __MOD_TEXTMSG_H
#define __MOD_TEXTMSG_H
#include "textmsgdefines.h"

bool tmsg_Init();
bool tmsg_Uninit();

bool tmsg_IsSMSEnable();

int tmsg_GetTotalUnRead();

// 根据账号名获取账号当前的未读信息数.
int tmsg_GetAccountUnRead(const yl::string & strUserName);

// 获取所有账号当前有的未读信息列表.
// | pData | [out] 是buffer.
// 如果获取成功则返回true, 否则返回false.
bool tmsg_GetAccountMsgList(AccountTextMsgListData & refData);

// 获取第一个未读信息的下标.
// 如果获取成功则返回取得的下标, 否则返回-1.
int tmsg_GetFirstUnReadInMessageIndex();

// Get message general list by click action.
// | pData | [out] is the buffer.
// | eGtype | is the TextMsgGroupType.
// Return true if retrieved successfully, otherwise return false.
bool tmsg_GetMessageList(TextMsgSimpListData & refData, TextMsgGroupType eGtype);

// Get message general list by click action.
// | pData | [out] is the buffer.
// | strClickAction | is the TextMsgGroupType.
// Return true if retrieved successfully, otherwise return false.
bool tmsg_GetMessageList(TextMsgSimpListData & refData, const yl::string & strClickAction);

// Get message detail info by click action and index.
// | pData | [out] is the buffer.
// | eGtype | is the TextMsgGroupType.
// | nIndex | is the index the message in the group.
// Return true if Retrieved successfully, otherwise return false.
bool tmsg_GetMessageDetail(TextMsgItemDetailData & refData, TextMsgGroupType eGtype, int nIndex);

// Get message detail info by click action and index.
// | pData | [out] is the buffer.
// | strClickAction | is the TextMsgGroupType.
// | nIndex | is the index the message in the group.
// Return true if Retrieved successfully, otherwise return false.
bool tmsg_GetMessageDetail(TextMsgItemDetailData & refData, const yl::string & strClickAction,
                           int nIndex);

// Get message staus info by click action and index.
// | eGtype | is the TextMsgGroupType.
// | nIndex | is the index the message in the group.
// Return status
TEXTMSG_STATUS_TYPE tmsg_GetMessageStatus(TextMsgGroupType eGtype, int nIndex);

//// Get message staus info by click action and index.
//// | strClickAction | is the TextMsgGroupType.
//// | nIndex | is the index the message in the group.
//// Return status
//TEXTMSG_STATUS_TYPE tmsg_GetMessageStatus(const yl::string& strClickAction, int nIndex);

// Add a draft message.
// | refData | is the message data to add.
// Return the index the new added message is at in the list, otherwise
// return -1.
int tmsg_AddDraftMessage(const TextMsgSendItemData & refData);

//// 向指定组中增加一条信息.
//// | refData | 是要增加的文本数据.
//// | nGType | 是组类型.
//// | eSType | 是状态类型.
//// 如果增加成功则返回信息在链表中的序号, 否则返回-1.
//int tmsg_AddMessage(const TextMsgSendItemData& refItem, TextMsgGroupType eGType, TEXTMSG_STATUS_TYPE eSType);
//
//// 向指定组中增加一条信息.
//// | refData | 是要增加的文本数据.
//// | strClickAction | 是组类型.
//// | eSType | 是状态类型.
//// 如果增加成功则返回信息在链表中的序号, 否则返回-1.
//int tmsg_AddMessage(const TextMsgSendItemData& refItem, const yl::string& strClickAction, TEXTMSG_STATUS_TYPE eSType);

// Send a message.
// | refData | is the message data to send.
// Return true if sent successfully, otherwise return false.
bool tmsg_SendTheMessage(const TextMsgSendItemData & refData);

// Remove the message at some index in the list.
// | eGtype | is the click action.
// | nIndex | is the index message at in the list.
// Return true if removed successfully, otherwise return false.
bool tmsg_RemoveMessage(TextMsgGroupType eGtype, int iIndex);

// Remove the message at some index in the list.
// | strClickAction | is the click action.
// | nIndex | is the index message at in the list.
// Return true if removed successfully, otherwise return false.
bool tmsg_RemoveMessage(const yl::string & strClickAction, int iIndex);

// 删除一组信息.
// | strClickAction | 是click action.
// | listIndex | 是要删除的下标列表.
// 如果删除成功则返回true, 否则返回false.
//bool tmsg_RemoveMessageList(const yl::string& strClickAction, YLList<int>& listIndex);
//
//// 删除一组信息.
//// | eGtype | 是click action.
//// | listIndex | 是要删除的下标列表.
//// 如果删除成功则返回true, 否则返回false.
//bool tmsg_RemoveMessageList(TextMsgGroupType eGtype, YLList<int>& listIndex);

// Remove all the message in the list.
// | strClickAction | is the click action.
// Return true if removed successfully, otherwise return false.
bool tmsg_RemoveAllMessage(const yl::string & strClickAction);

// Remove all the message in the list.
// | eGtype | is the click action.
// Return true if removed successfully, otherwise return false.
bool tmsg_RemoveAllMessage(TextMsgGroupType eGtype);

// Remove all the message in the list.
// | bCheck | is the click action.
// Return true if removed successfully, otherwise return false.
//bool tmsg_RemoveAllMessage(bool bCheck = true);

// 插入一条信息到链表, 不同于AddMessage, 此函数的参数里信息
// 更加详细.
// | refData | 是要增加的文本数据.
// 如果增加成功则返回信息在链表中的序号, 否则返回-1.
//#if SUPPORT_SMS
//int tmsg_AddDetailMessage(TextMsgItemData& refItem);
//#else
//int tmsg_AddDetailMessage(const TextMsgItemData& refItem);
//#endif//SUPPORT_SMS
#endif //__MOD_TEXTMSG_H