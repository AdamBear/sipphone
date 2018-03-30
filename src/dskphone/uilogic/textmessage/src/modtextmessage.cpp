#include "modtextmessage.h"
#include "textmsgmanager.h"

bool tmsg_Init()
{
    _TextMsgManager.Init();
    return true;
}

bool tmsg_Uninit()
{
    _TextMsgManager.Uninit();
    return true;
}

bool tmsg_IsSMSEnable()
{
    return _TextMsgManager.IsSMSEnable();
}

int tmsg_GetTotalUnRead()
{
    return _TextMsgManager.GetTotalUnRead();
}

int tmsg_GetAccountUnRead(const yl::string & strUserName)
{
    return _TextMsgManager.GetAccountUnRead(strUserName);
}

bool tmsg_GetAccountMsgList(AccountTextMsgListData & refData)
{
    return _TextMsgManager.GetAccountMsgList(refData);
}

int tmsg_GetFirstUnReadInMessageIndex()
{
    return _TextMsgManager.GetFirstUnReadInMessageIndex();
}

bool tmsg_GetMessageList(TextMsgSimpListData & refData, TextMsgGroupType eGtype)
{
    return _TextMsgManager.GetMessageList(refData, eGtype);
}

bool tmsg_GetMessageList(TextMsgSimpListData & refData, const yl::string & strClickAction)
{
    TextMsgGroupType eGtype = _TextMsgManager.ClickAction2GroupType(strClickAction);
    return _TextMsgManager.GetMessageList(refData, eGtype);
}

bool tmsg_GetMessageDetail(TextMsgItemDetailData & refData, TextMsgGroupType eGtype, int nIndex)
{
    return _TextMsgManager.GetMessageDetail(refData, eGtype, nIndex);
}

bool tmsg_GetMessageDetail(TextMsgItemDetailData & refData, const yl::string & strClickAction,
                           int nIndex)
{
    TextMsgGroupType eGtype = _TextMsgManager.ClickAction2GroupType(strClickAction);
    return _TextMsgManager.GetMessageDetail(refData, eGtype, nIndex);
}

TEXTMSG_STATUS_TYPE tmsg_GetMessageStatus(TextMsgGroupType eGtype, int nIndex)
{
    return _TextMsgManager.GetMessageStatus(eGtype, nIndex);
}

//TEXTMSG_STATUS_TYPE tmsg_GetMessageStatus(const yl::string& strClickAction, int nIndex)
//{
//  TextMsgGroupType eGtype = _TextMsgManager.ClickAction2GroupType(strClickAction);
//  return _TextMsgManager.GetMessageStatus(eGtype, nIndex);
//}

int tmsg_AddDraftMessage(const TextMsgSendItemData & refData)
{
    return _TextMsgManager.AddDraftMessage(refData);
}

//int tmsg_AddMessage(const TextMsgSendItemData& refItem, TextMsgGroupType eGType, TEXTMSG_STATUS_TYPE eSType)
//{
//  return _TextMsgManager.AddMessage(refItem, eGType, eSType);
//}
//
//int tmsg_AddMessage(const TextMsgSendItemData& refItem, const yl::string& strClickAction, TEXTMSG_STATUS_TYPE eSType)
//{
//  TextMsgGroupType eGType = _TextMsgManager.ClickAction2GroupType(strClickAction);
//  return _TextMsgManager.AddMessage(refItem, eGType, eSType);
//}

bool tmsg_SendTheMessage(const TextMsgSendItemData & refData)
{
    return _TextMsgManager.SendTheMessage(refData);
}

bool tmsg_RemoveMessage(TextMsgGroupType eGtype, int iIndex)
{
    return _TextMsgManager.RemoveMessage(eGtype, iIndex);
}

bool tmsg_RemoveMessage(const yl::string & strClickAction, int iIndex)
{
    TextMsgGroupType eGtype = _TextMsgManager.ClickAction2GroupType(strClickAction);
    return _TextMsgManager.RemoveMessage(eGtype, iIndex);
}

//bool tmsg_RemoveMessageList(TextMsgGroupType eGtype, YLList<int>& listIndex)
//{
//  return _TextMsgManager.RemoveMessageList(eGtype, listIndex);
//}
//
//bool tmsg_RemoveMessageList(const yl::string& strClickAction, YLList<int>& listIndex)
//{
//  TextMsgGroupType eGtype = _TextMsgManager.ClickAction2GroupType(strClickAction);
//  return _TextMsgManager.RemoveMessageList(eGtype, listIndex);
//}

bool tmsg_RemoveAllMessage(TextMsgGroupType eGtype)
{
    return _TextMsgManager.RemoveAllMessage(eGtype);
}

bool tmsg_RemoveAllMessage(const yl::string & strClickAction)
{
    TextMsgGroupType eGtype = _TextMsgManager.ClickAction2GroupType(strClickAction);
    return _TextMsgManager.RemoveAllMessage(eGtype);
}

//bool tmsg_RemoveAllMessage(bool bCheck/* = true*/)
//{
//  return _TextMsgManager.RemoveAllMessage(bCheck);
//}

//#if SUPPORT_SMS
//int tmsg_AddDetailMessage(TextMsgItemData& refItem)
//#else
//int tmsg_AddDetailMessage(const TextMsgItemData& refItem)
//#endif //SUPPORT_SMS
//{
//  return _TextMsgManager.AddDetailMessage(refItem);
//}